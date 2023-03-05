/*
  Apery, a USI shogi playing engine derived from Stockfish, a UCI chess playing engine.
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2018 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad
  Copyright (C) 2011-2018 Hiraoka Takuya

  Apery is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Apery is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// clang-format off

#include "position.hpp"
#include "move.hpp"
#include "mt64bit.hpp"
#include "generateMoves.hpp"

Key Position::zobrist_[piece::PieceTypeNum][SquareNum][color::NUM_COLORS];
Key Position::zobHand_[piece::NUM_CAPTURED_PIECE_TYPES][color::NUM_COLORS];

const HuffmanCode HuffmanCodedPos::boardCodeTable[piece::PieceNone] = {
    {Binary<         0>::value, 1}, // piece::Empty
    {Binary<         1>::value, 4}, // piece::B_FU
    {Binary<        11>::value, 6}, // piece::B_KY
    {Binary<       111>::value, 6}, // piece::B_KE
    {Binary<      1011>::value, 6}, // piece::B_GI
    {Binary<     11111>::value, 8}, // piece::B_KA
    {Binary<    111111>::value, 8}, // piece::B_HI
    {Binary<      1111>::value, 6}, // piece::B_KI
    {Binary<         0>::value, 0}, // piece::B_OU 玉の位置は別途、位置を符号化する。使用しないので numOfBit を 0 にしておく。
    {Binary<      1001>::value, 4}, // piece::B_TO
    {Binary<    100011>::value, 6}, // piece::B_NY
    {Binary<    100111>::value, 6}, // piece::B_NK
    {Binary<    101011>::value, 6}, // piece::B_NG
    {Binary<  10011111>::value, 8}, // piece::B_UM
    {Binary<  10111111>::value, 8}, // BDragona
    {Binary<         0>::value, 0}, // 使用しないので numOfBit を 0 にしておく。
    {Binary<         0>::value, 0}, // 使用しないので numOfBit を 0 にしておく。
    {Binary<       101>::value, 4}, // piece::W_FU
    {Binary<     10011>::value, 6}, // piece::W_KY
    {Binary<     10111>::value, 6}, // piece::W_KE
    {Binary<     11011>::value, 6}, // piece::W_GI
    {Binary<   1011111>::value, 8}, // piece::W_KA
    {Binary<   1111111>::value, 8}, // piece::W_HI
    {Binary<    101111>::value, 6}, // piece::W_KI
    {Binary<         0>::value, 0}, // piece::W_OU 玉の位置は別途、位置を符号化する。
    {Binary<      1101>::value, 4}, // piece::W_TO
    {Binary<    110011>::value, 6}, // piece::W_NY
    {Binary<    110111>::value, 6}, // piece::W_NK
    {Binary<    111011>::value, 6}, // piece::W_NG
    {Binary<  11011111>::value, 8}, // piece::W_UM
    {Binary<  11111111>::value, 8}, // piece::W_RY
};

// 盤上の bit 数 - 1 で表現出来るようにする。持ち駒があると、盤上には piece::Empty の 1 bit が増えるので、
// これで局面の bit 数が固定化される。
const HuffmanCode HuffmanCodedPos::handCodeTable[piece::NUM_CAPTURED_PIECE_TYPES][color::NUM_COLORS] = {
    {{Binary<        0>::value, 3}, {Binary<      100>::value, 3}}, // piece::C_FU
    {{Binary<        1>::value, 5}, {Binary<    10001>::value, 5}}, // piece::C_KY
    {{Binary<       11>::value, 5}, {Binary<    10011>::value, 5}}, // piece::C_KE
    {{Binary<      101>::value, 5}, {Binary<    10101>::value, 5}}, // piece::C_GI
    {{Binary<      111>::value, 5}, {Binary<    10111>::value, 5}}, // piece::C_KI
    {{Binary<    11111>::value, 7}, {Binary<  1011111>::value, 7}}, // piece::C_KA
    {{Binary<   111111>::value, 7}, {Binary<  1111111>::value, 7}}, // piece::C_HI
};

HuffmanCodeToPieceHash HuffmanCodedPos::boardCodeToPieceHash;
HuffmanCodeToPieceHash HuffmanCodedPos::handCodeToPieceHash;

// やねうら王のpacked sfenのハフマン符号
//   ※　 なのはminiの符号化から、変換が楽になるように単純化。
//
//   盤上の1升(NO_PIECE以外) = 2～6bit ( + 成りフラグ1bit+ 先後1bit )
//   手駒の1枚               = 1～5bit ( + 成りフラグ1bit+ 先後1bit )
//
//    空     xxxxx0 + 0    (none)
//    歩     xxxx01 + 2    xxxx0 + 2
//    香     xx0011 + 2    xx001 + 2
//    桂     xx1011 + 2    xx101 + 2
//    銀     xx0111 + 2    xx011 + 2
//    金     x01111 + 1    x0111 + 1 // 金は成りフラグはない。
//    角     011111 + 2    01111 + 2
//    飛     111111 + 2    11111 + 2
//
// すべての駒が盤上にあるとして、
//     空 81 - 40駒 = 41升 = 41bit
//     歩      4bit*18駒   = 72bit
//     香      6bit* 4駒   = 24bit
//     桂      6bit* 4駒   = 24bit
//     銀      6bit* 4駒   = 24bit
//     金      6bit* 4駒   = 24bit
//     角      8bit* 2駒   = 16bit
//     飛      8bit* 2駒   = 16bit
//                          -------
//                          241bit + 1bit(手番) + 7bit×2(王の位置先後) = 256bit
//
// 盤上の駒が手駒に移動すると盤上の駒が空になるので盤上のその升は1bitで表現でき、
// 手駒は、盤上の駒より1bit少なく表現できるので結局、全体のbit数に変化はない。
// ゆえに、この表現において、どんな局面でもこのbit数で表現できる。
// 手駒に成りフラグは不要だが、これも含めておくと盤上の駒のbit数-1になるので
// 全体のbit数が固定化できるのでこれも含めておくことにする。
const HuffmanCode PackedSfen::boardCodeTable[piece::PieceNone] = {
	{ Binary<         0>::value, 1 }, // piece::Empty
	{ Binary<         1>::value, 4 }, // piece::B_FU
	{ Binary<        11>::value, 6 }, // piece::B_KY
	{ Binary<      1011>::value, 6 }, // piece::B_KE
	{ Binary<       111>::value, 6 }, // piece::B_GI
	{ Binary<     11111>::value, 8 }, // piece::B_KA
	{ Binary<    111111>::value, 8 }, // piece::B_HI
	{ Binary<      1111>::value, 6 }, // piece::B_KI
	{ Binary<         0>::value, 0 }, // piece::B_OU 玉の位置は別途、位置を符号化する。使用しないので numOfBit を 0 にしておく。
	{ Binary<       101>::value, 4 }, // piece::B_TO
	{ Binary<     10011>::value, 6 }, // piece::B_NY
	{ Binary<     11011>::value, 6 }, // piece::B_NK
	{ Binary<     10111>::value, 6 }, // piece::B_NG
	{ Binary<   1011111>::value, 8 }, // piece::B_UM
	{ Binary<   1111111>::value, 8 }, // BDragona
	{ Binary<         0>::value, 0 }, // 使用しないので numOfBit を 0 にしておく。
	{ Binary<         0>::value, 0 }, // 使用しないので numOfBit を 0 にしておく。
	{ Binary<      1001>::value, 4 }, // piece::W_FU
	{ Binary<    100011>::value, 6 }, // piece::W_KY
	{ Binary<    101011>::value, 6 }, // piece::W_KE
	{ Binary<    100111>::value, 6 }, // piece::W_GI
	{ Binary<  10011111>::value, 8 }, // piece::W_KA
	{ Binary<  10111111>::value, 8 }, // piece::W_HI
	{ Binary<    101111>::value, 6 }, // piece::W_KI
	{ Binary<         0>::value, 0 }, // piece::W_OU 玉の位置は別途、位置を符号化する。
	{ Binary<      1101>::value, 4 }, // piece::W_TO
	{ Binary<    110011>::value, 6 }, // piece::W_NY
	{ Binary<    111011>::value, 6 }, // piece::W_NK
	{ Binary<    110111>::value, 6 }, // piece::W_NG
	{ Binary<  11011111>::value, 8 }, // piece::W_UM
	{ Binary<  11111111>::value, 8 }, // piece::W_RY
};

// 盤上の bit 数 - 1 で表現出来るようにする。持ち駒があると、盤上には piece::Empty の 1 bit が増えるので、
// これで局面の bit 数が固定化される。
const HuffmanCode PackedSfen::handCodeTable[piece::NUM_CAPTURED_PIECE_TYPES][color::NUM_COLORS] = {
	{ { Binary<        0>::value, 3 },{ Binary<      100>::value, 3 } }, // piece::C_FU
	{ { Binary<        1>::value, 5 },{ Binary<    10001>::value, 5 } }, // piece::C_KY
	{ { Binary<      101>::value, 5 },{ Binary<    10101>::value, 5 } }, // piece::C_KE
	{ { Binary<       11>::value, 5 },{ Binary<    10011>::value, 5 } }, // piece::C_GI
	{ { Binary<      111>::value, 5 },{ Binary<    10111>::value, 5 } }, // piece::C_KI
	{ { Binary<     1111>::value, 7 },{ Binary<  1001111>::value, 7 } }, // piece::C_KA
	{ { Binary<    11111>::value, 7 },{ Binary<  1011111>::value, 7 } }, // piece::C_HI
};

HuffmanCodeToPieceHash PackedSfen::boardCodeToPieceHash;
HuffmanCodeToPieceHash PackedSfen::handCodeToPieceHash;

const CharToPieceUSI g_charToPieceUSI;

namespace {
    const char* PieceToCharCSATable[piece::PieceNone] = {
        " * ", "+FU", "+KY", "+KE", "+GI", "+KA", "+HI", "+KI", "+OU", "+TO", "+NY", "+NK", "+NG", "+UM", "+RY", "", "",
        "-FU", "-KY", "-KE", "-GI", "-KA", "-HI", "-KI", "-OU", "-TO", "-NY", "-NK", "-NG", "-UM", "-RY"
    };
    inline const char* pieceToCharCSA(const piece::ColoredPieceEnum pc) {
        return PieceToCharCSATable[pc];
    }
    const char* PieceToCharUSITable[piece::PieceNone] = {
        "", "P", "L", "N", "S", "B", "R", "G", "K", "+P", "+L", "+N", "+S", "+B", "+R", "", "",
        "p", "l", "n", "s", "b", "r", "g", "k", "+p", "+l", "+n", "+s", "+b", "+r"
    };
    inline const char* pieceToCharUSI(const piece::ColoredPieceEnum pc) {
        return PieceToCharUSITable[pc];
    }
}

CheckInfo::CheckInfo(const Position& pos) {
    const color::ColorEnum them = color::opposite(pos.turn());
    const Square ksq = pos.kingSquare(them);

    pinned = pos.pinnedBB();
    dcBB = pos.discoveredCheckBB();

    checkBB[piece::FU     ] = pos.attacksFrom<piece::FU  >(them, ksq);
    checkBB[piece::KY    ] = pos.attacksFrom<piece::KY >(them, ksq);
    checkBB[piece::KE   ] = pos.attacksFrom<piece::KE>(them, ksq);
    checkBB[piece::GI   ] = pos.attacksFrom<piece::GI>(them, ksq);
    checkBB[piece::KA   ] = pos.attacksFrom<piece::KA>(ksq);
    checkBB[piece::HI     ] = pos.attacksFrom<piece::HI  >(ksq);
    checkBB[piece::KI     ] = pos.attacksFrom<piece::KI  >(them, ksq);
    checkBB[piece::OU     ] = allZeroBB();
    // todo: ここで AVX2 使えそう。
    //       checkBB のreadアクセスは switch (pt) で場合分けして、余計なコピー減らした方が良いかも。
    checkBB[piece::TO  ] = checkBB[piece::KI];
    checkBB[piece::NY ] = checkBB[piece::KI];
    checkBB[piece::NK] = checkBB[piece::KI];
    checkBB[piece::NG] = checkBB[piece::KI];
    checkBB[piece::UM    ] = checkBB[piece::KA] | pos.attacksFrom<piece::OU>(ksq);
    checkBB[piece::RY   ] = checkBB[piece::HI  ] | pos.attacksFrom<piece::OU>(ksq);
}

Bitboard Position::attacksFrom(const piece::PieceTypeEnum pt, const color::ColorEnum c, const Square sq, const Bitboard& occupied) {
    switch (pt) {
    case piece::Occupied:  return allZeroBB();
    case piece::FU:      return pawnAttack(c, sq);
    case piece::KY:     return lanceAttack(c, sq, occupied);
    case piece::KE:    return knightAttack(c, sq);
    case piece::GI:    return silverAttack(c, sq);
    case piece::KA:    return bishopAttack(sq, occupied);
    case piece::HI:      return rookAttack(sq, occupied);
    case piece::KI:
    case piece::TO:
    case piece::NY:
    case piece::NK:
    case piece::NG: return goldAttack(c, sq);
    case piece::OU:      return kingAttack(sq);
    case piece::UM:     return horseAttack(sq, occupied);
    case piece::RY:    return dragonAttack(sq, occupied);
    default:        UNREACHABLE; return allOneBB();
    }
}

// 実際に指し手が合法手かどうか判定
// 連続王手の千日手は排除しない。
// 確実に駒打ちではないときは、MUSTNOTDROP == true とする。
// 確実に玉の移動で無いときは、FROMMUSTNOTKING == true とする。英語として正しい？
// 遠隔駒で王手されているとき、その駒の利きがある場所に逃げる手を検出出来ない場合があるので、
// そのような手を指し手生成してはいけない。
template <bool MUSTNOTDROP, bool FROMMUSTNOTKING>
bool Position::pseudoLegalMoveIsLegal(const Move move, const Bitboard& pinned) const {
    // 駒打ちは、打ち歩詰めや二歩は指し手生成時や、killerをMovePicker::nextMove() 内で排除しているので、常に合法手
    // (連続王手の千日手は省いていないけれど。)
    if (!MUSTNOTDROP && move.isDrop())
        return true;
    assert(!move.isDrop());

    const color::ColorEnum us = turn();
    const Square from = move.from();

    if (!FROMMUSTNOTKING && piece::to_piece_type(piece(from)) == piece::OU) {
        const color::ColorEnum them = color::opposite(us);
        // 玉の移動先に相手の駒の利きがあれば、合法手でないので、false
        return !attackersToIsAny(them, move.to());
    }
    // 玉以外の駒の移動
    return !isPinnedIllegal(from, move.to(), kingSquare(us), pinned);
}

template bool Position::pseudoLegalMoveIsLegal<false, false>(const Move move, const Bitboard& pinned) const;
template bool Position::pseudoLegalMoveIsLegal<false, true >(const Move move, const Bitboard& pinned) const;
template bool Position::pseudoLegalMoveIsLegal<true,  false>(const Move move, const Bitboard& pinned) const;

bool Position::pseudoLegalMoveIsEvasion(const Move move, const Bitboard& pinned) const {
    assert(isOK());

    // 玉の移動
    if (move.pieceTypeFrom() == piece::OU) {
        // 遠隔駒で王手されたとき、王手している遠隔駒の利きには移動しないように指し手を生成している。
        // その為、移動先に他の駒の利きが無いか調べるだけで良い。
        const bool canMove = !attackersToIsAny(color::opposite(turn()), move.to());
        assert(canMove == (pseudoLegalMoveIsLegal<false, false>(move, pinned)));
        return canMove;
    }

    // 玉の移動以外
    Bitboard target = checkersBB();
    const Square checkSq = target.firstOneFromSQ11();

    if (target)
        // 両王手のとき、玉の移動以外の手は指せない。
        return false;

    const color::ColorEnum us = turn();
    const Square to = move.to();
    // 移動、又は打った駒が、王手をさえぎるか、王手している駒を取る必要がある。
    target = betweenBB(checkSq, kingSquare(us)) | checkersBB();
    return target.isSet(to) && pseudoLegalMoveIsLegal<false, true>(move, pinned);
}

// Searching: true なら探索時に内部で生成した手の合法手判定を行う。
//            ttMove で hash 値が衝突した時などで、大駒の不成など明らかに価値の低い手が生成される事がある。
//            これは非合法手として省いて良い。
//            false なら、外部入力の合法手判定なので、ルールと同一の条件になる事が望ましい。
template <bool Searching> bool Position::moveIsPseudoLegal(const Move move) const {
    const color::ColorEnum us = turn();
    const color::ColorEnum them = color::opposite(us);
    const Square to = move.to();

    if (move.isDrop()) {
        const piece::PieceTypeEnum ptFrom = move.pieceTypeDropped();
        if (!hand(us).exists(piece::to_captured_piece_type(ptFrom)) || piece(to) != piece::Empty)
            return false;

        if (inCheck()) {
            // 王手されているので、合駒でなければならない。
            Bitboard target = checkersBB();
            const Square checksq = target.firstOneFromSQ11();

            if (target)
                // 両王手は合駒出来無い。
                return false;

            target = betweenBB(checksq, kingSquare(us));
            if (!target.isSet(to))
                // 玉と、王手した駒との間に駒を打っていない。
                return false;
        }

        if (ptFrom == piece::FU) {
            if ((bbOf(piece::FU, us) & fileMask(makeFile(to))))
                // 二歩
                return false;
            const SquareDelta TDeltaN = (us == color::BLACK ? DeltaN : DeltaS);
            if (to + TDeltaN == kingSquare(them) && isPawnDropCheckMate(us, to))
                // 王手かつ打ち歩詰め
                return false;
        }
    }
    else {
        const Square from = move.from();
        const piece::PieceTypeEnum ptFrom = move.pieceTypeFrom();
        if (piece(from) != to_colored_piece(us, ptFrom) || bbOf(us).isSet(to))
            return false;

        if (!attacksFrom(ptFrom, us, from).isSet(to))
            return false;

        if (Searching) {
            switch (ptFrom) {
            case piece::FU  :
                if (move.isPromotion()) {
                    if (!canPromote(us, makeRank(to)))
                        return false;
                }
                else if (canPromote(us, makeRank(to)))
                    return false;
                break;
            case piece::KY :
                if (move.isPromotion()) {
                    if (!canPromote(us, makeRank(to)))
                        return false;
                }
                else {
                    // 1段目の不成は非合法なので省く。2段目の不成と3段目の駒を取らない不成もついでに省く。
                    const Rank toRank = makeRank(to);
                    if (us == color::BLACK ? isInFrontOf<color::BLACK, Rank3, Rank7>(toRank) : isInFrontOf<color::WHITE, Rank3, Rank7>(toRank))
                        return false;
                    if (canPromote(us, toRank) && !move.isCapture())
                        return false;
                }
                break;
            case piece::KE:
                // hash 値が衝突して別の局面の合法手の ttMove が入力されても、桂馬である事は確定。(桂馬は移動元、移動先が特殊なので。)
                // よって、行きどころの無い駒になる move は生成されない。
                // 特にチェックすべき事は無いので、break
                break;
            case piece::GI: case piece::KA: case piece::HI  :
                if (move.isPromotion())
                    if (!canPromote(us, makeRank(to)) && !canPromote(us, makeRank(from)))
                        return false;
                break;
            default: // 成れない駒
                if (move.isPromotion())
                    return false;
            }
        }

        if (inCheck()) {
            if (ptFrom == piece::OU) {
                Bitboard occ = occupiedBB();
                occ.clearBit(from);
                if (attackersToIsAny(them, to, occ))
                    // 王手から逃げていない。
                    return false;
            }
            else {
                // 玉以外の駒を移動させたとき。
                Bitboard target = checkersBB();
                const Square checksq = target.firstOneFromSQ11();

                if (target)
                    // 両王手なので、玉が逃げない手は駄目
                    return false;

                target = betweenBB(checksq, kingSquare(us)) | checkersBB();
                if (!target.isSet(to))
                    // 玉と、王手した駒との間に移動するか、王手した駒を取る以外は駄目。
                    return false;
            }
        }
    }

    return true;
}

template bool Position::moveIsPseudoLegal<true >(const Move move) const;
template bool Position::moveIsPseudoLegal<false>(const Move move) const;

// 過去(又は現在)に生成した指し手が現在の局面でも有効か判定。
// あまり速度が要求される場面で使ってはいけない。
bool Position::moveIsLegal(const Move move) const {
    return MoveList<LegalAll>(*this).contains(move);
}

// 局面の更新
void Position::doMove(const Move move, StateInfo& newSt) {
    const CheckInfo ci(*this);
    doMove(move, newSt, ci, moveGivesCheck(move, ci));
}

// 局面の更新
void Position::doMove(const Move move, StateInfo& newSt, const CheckInfo& ci, const bool moveIsCheck) {
    assert(isOK());
    assert(move);
    assert(&newSt != st_);

    Key boardKey = getBoardKey();
    Key handKey = getHandKey();
    boardKey ^= zobTurn();

    memcpy(&newSt, st_, offsetof(StateInfo, boardKey));
    newSt.previous = st_;
    st_ = &newSt;

    gamePly_++;

    const color::ColorEnum us = turn();
    const Square to = move.to();
    const piece::PieceTypeEnum ptCaptured = move.cap();
    piece::PieceTypeEnum ptTo;
    if (move.isDrop()) {
        ptTo = move.pieceTypeDropped();
        const piece::CapturedPieceTypeEnum hpTo = piece::to_captured_piece_type(ptTo);

        handKey -= zobHand(hpTo, us);
        boardKey += zobrist(ptTo, to, us);

        hand_[us].minusOne(hpTo);
        xorBBs(ptTo, to, us);
        piece_[to] = to_colored_piece(us, ptTo);

        if (moveIsCheck) {
            // Direct checks
            st_->checkersBB = setMaskBB(to);
            st_->continuousCheck[us] += 2;
        }
        else {
            st_->checkersBB = allZeroBB();
            st_->continuousCheck[us] = 0;
        }
    }
    else {
        const Square from = move.from();
        const piece::PieceTypeEnum ptFrom = move.pieceTypeFrom();
        ptTo = move.pieceTypeTo(ptFrom);

        byTypeBB_[ptFrom].xorBit(from);
        byTypeBB_[ptTo].xorBit(to);
        byColorBB_[us].xorBit(from, to);
        piece_[from] = piece::Empty;
        piece_[to] = to_colored_piece(us, ptTo);
        boardKey -= zobrist(ptFrom, from, us);
        boardKey += zobrist(ptTo, to, us);

        if (ptCaptured) {
            // 駒を取ったとき
            const piece::CapturedPieceTypeEnum hpCaptured = piece::to_captured_piece_type(ptCaptured);
            const color::ColorEnum them = color::opposite(us);

            boardKey -= zobrist(ptCaptured, to, them);
            handKey += zobHand(hpCaptured, us);

            byTypeBB_[ptCaptured].xorBit(to);
            byColorBB_[them].xorBit(to);

            hand_[us].plusOne(hpCaptured);
        }
        // piece::Occupied は to, from の位置のビットを操作するよりも、
        // color::BLACK と White の or を取る方が速いはず。
        byTypeBB_[piece::Occupied] = bbOf(color::BLACK) | bbOf(color::WHITE);

        if (ptTo == piece::OU)
            kingSquare_[us] = to;

        if (moveIsCheck) {
            // Direct checks
            st_->checkersBB = ci.checkBB[ptTo] & setMaskBB(to);

            // Discovery checks
            const Square ksq = kingSquare(color::opposite(us));
            if (isDiscoveredCheck(from, to, ksq, ci.dcBB)) {
                switch (squareRelation(from, ksq)) {
                case DirecMisc: assert(false); break; // 最適化の為のダミー
                case DirecFile:
                    // from の位置から縦に利きを調べると相手玉と、空き王手している駒に当たっているはず。味方の駒が空き王手している駒。
                    st_->checkersBB |= rookAttackFile(from, occupiedBB()) & bbOf(us);
                    break;
                case DirecRank:
                    st_->checkersBB |= rookAttackRank(ksq, occupiedBB()) & bbOf(piece::HI, piece::RY, us);
                    break;
                case DirecDiagNESW: case DirecDiagNWSE:
                    st_->checkersBB |= attacksFrom<piece::KA>(ksq) & bbOf(piece::KA, piece::UM, us);
                    break;
                default: UNREACHABLE;
                }
            }
            st_->continuousCheck[us] += 2;
        }
        else {
            st_->checkersBB = allZeroBB();
            st_->continuousCheck[us] = 0;
        }
    }
    goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

    st_->boardKey = boardKey;
    st_->handKey = handKey;
    ++st_->pliesFromNull;

    turn_ = color::opposite(us);
    st_->hand = hand(turn());

    assert(isOK());
}

void Position::undoMove(const Move move) {
    assert(isOK());
    assert(move);

    const color::ColorEnum them = turn();
    const color::ColorEnum us = color::opposite(them);
    const Square to = move.to();
    turn_ = us;
    // ここで先に turn_ を戻したので、以下、move は us の指し手とする。
    if (move.isDrop()) {
        const piece::PieceTypeEnum ptTo = move.pieceTypeDropped();
        byTypeBB_[ptTo].xorBit(to);
        byColorBB_[us].xorBit(to);
        piece_[to] = piece::Empty;

        const piece::CapturedPieceTypeEnum hp = piece::to_captured_piece_type(ptTo);
        hand_[us].plusOne(hp);
    }
    else {
        const Square from = move.from();
        const piece::PieceTypeEnum ptFrom = move.pieceTypeFrom();
        const piece::PieceTypeEnum ptTo = move.pieceTypeTo(ptFrom);
        const piece::PieceTypeEnum ptCaptured = move.cap(); // todo: st_->capturedType 使えば良い。

        if (ptTo == piece::OU)
            kingSquare_[us] = from;

        if (ptCaptured) {
            // 駒を取ったとき
            byTypeBB_[ptCaptured].xorBit(to);
            byColorBB_[them].xorBit(to);
            const piece::CapturedPieceTypeEnum hpCaptured = piece::to_captured_piece_type(ptCaptured);
            const piece::ColoredPieceEnum pcCaptured = to_colored_piece(them, ptCaptured);
            piece_[to] = pcCaptured;

            hand_[us].minusOne(hpCaptured);
        }
        else
            // 駒を取らないときは、to_colored_piece(us, ptCaptured) は 0 または 16 になる。
            // 16 になると困るので、駒を取らないときは明示的に piece::Empty にする。
            piece_[to] = piece::Empty;
        byTypeBB_[ptFrom].xorBit(from);
        byTypeBB_[ptTo].xorBit(to);
        byColorBB_[us].xorBit(from, to);
        piece_[from] = to_colored_piece(us, ptFrom);
    }
    // piece::Occupied は to, from の位置のビットを操作するよりも、
    // color::BLACK と White の or を取る方が速いはず。
    byTypeBB_[piece::Occupied] = bbOf(color::BLACK) | bbOf(color::WHITE);
    goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

    // key などは StateInfo にまとめられているので、
    // previous のポインタを st_ に代入するだけで良い。
    st_ = st_->previous;

    gamePly_--;

    assert(isOK());
}

template <bool DO> void Position::doNullMove(StateInfo& backUpSt) {
    assert(!inCheck());

    StateInfo* src = (DO ? st_ : &backUpSt);
    StateInfo* dst = (DO ? &backUpSt : st_);

    dst->boardKey = src->boardKey;
    dst->handKey = src->handKey;
    dst->pliesFromNull = src->pliesFromNull;
    dst->hand = hand(turn());
    turn_ = color::opposite(turn());

    if (DO) {
        st_->boardKey ^= zobTurn();
        st_->pliesFromNull = 0;
        st_->continuousCheck[turn()] = 0;
    }
    st_->hand = hand(turn());

    assert(isOK());
}
template void Position::doNullMove<true>(StateInfo& backUpSt);
template void Position::doNullMove<false>(StateInfo& backUpSt);

namespace {
    // SEE の順番
    template <piece::PieceTypeEnum PT> struct SEENextPieceType {}; // これはインスタンス化しない。
    template <> struct SEENextPieceType<piece::FU     > { static const piece::PieceTypeEnum value = piece::KY;     };
    template <> struct SEENextPieceType<piece::KY    > { static const piece::PieceTypeEnum value = piece::KE;    };
    template <> struct SEENextPieceType<piece::KE   > { static const piece::PieceTypeEnum value = piece::TO;   };
    template <> struct SEENextPieceType<piece::TO  > { static const piece::PieceTypeEnum value = piece::NY;  };
    template <> struct SEENextPieceType<piece::NY > { static const piece::PieceTypeEnum value = piece::NK; };
    template <> struct SEENextPieceType<piece::NK> { static const piece::PieceTypeEnum value = piece::GI;    };
    template <> struct SEENextPieceType<piece::GI   > { static const piece::PieceTypeEnum value = piece::NG; };
    template <> struct SEENextPieceType<piece::NG> { static const piece::PieceTypeEnum value = piece::KI;      };
    template <> struct SEENextPieceType<piece::KI     > { static const piece::PieceTypeEnum value = piece::KA;    };
    template <> struct SEENextPieceType<piece::KA   > { static const piece::PieceTypeEnum value = piece::UM;     };
    template <> struct SEENextPieceType<piece::UM    > { static const piece::PieceTypeEnum value = piece::HI;      };
    template <> struct SEENextPieceType<piece::HI     > { static const piece::PieceTypeEnum value = piece::RY;    };
    template <> struct SEENextPieceType<piece::RY   > { static const piece::PieceTypeEnum value = piece::OU;      };

    template <piece::PieceTypeEnum PT> FORCE_INLINE piece::PieceTypeEnum nextAttacker(const Position& pos, const Square to, const Bitboard& opponentAttackers,
                                                                Bitboard& occupied, Bitboard& attackers, const color::ColorEnum turn)
    {
        if (opponentAttackers.andIsAny(pos.bbOf(PT))) {
            const Bitboard bb = opponentAttackers & pos.bbOf(PT);
            const Square from = bb.constFirstOneFromSQ11();
            occupied.xorBit(from);
            // todo: 実際に移動した方向を基にattackersを更新すれば、template, inline を使用しなくても良さそう。
            //       その場合、キャッシュに乗りやすくなるので逆に速くなるかも。
            if (PT == piece::FU || PT == piece::KY)
                attackers |= (lanceAttack(color::opposite(turn), to, occupied) & (pos.bbOf(piece::HI, piece::RY) | pos.bbOf(piece::KY, turn)));
            if (PT == piece::KI || PT == piece::TO || PT == piece::NY || PT == piece::NK || PT == piece::NG || PT == piece::UM || PT == piece::RY)
                attackers |= (lanceAttack(color::opposite(turn), to, occupied) & pos.bbOf(piece::KY, turn))
                    | (lanceAttack(turn, to, occupied) & pos.bbOf(piece::KY, color::opposite(turn)))
                    | (rookAttack(to, occupied) & pos.bbOf(piece::HI, piece::RY))
                    | (bishopAttack(to, occupied) & pos.bbOf(piece::KA, piece::UM));
            if (PT == piece::GI)
                attackers |= (lanceAttack(color::opposite(turn), to, occupied) & pos.bbOf(piece::KY, turn))
                    | (rookAttack(to, occupied) & pos.bbOf(piece::HI, piece::RY))
                    | (bishopAttack(to, occupied) & pos.bbOf(piece::KA, piece::UM));
            if (PT == piece::KA)
                attackers |= (bishopAttack(to, occupied) & pos.bbOf(piece::KA, piece::UM));
            if (PT == piece::HI)
                attackers |= (lanceAttack(color::opposite(turn), to, occupied) & pos.bbOf(piece::KY, turn))
                    | (lanceAttack(turn, to, occupied) & pos.bbOf(piece::KY, color::opposite(turn)))
                    | (rookAttack(to, occupied) & pos.bbOf(piece::HI, piece::RY));

            if (PT == piece::FU || PT == piece::KY || PT == piece::KE)
                if (canPromote(turn, makeRank(to)))
                    return PT + piece::PTPromote;
            if (PT == piece::GI || PT == piece::KA || PT == piece::HI)
                if (canPromote(turn, makeRank(to)) || canPromote(turn, makeRank(from)))
                    return PT + piece::PTPromote;
            return PT;
        }
        return nextAttacker<SEENextPieceType<PT>::value>(pos, to, opponentAttackers, occupied, attackers, turn);
    }
    template <> FORCE_INLINE piece::PieceTypeEnum nextAttacker<piece::OU>(const Position&, const Square, const Bitboard&,
                                                          Bitboard&, Bitboard&, const color::ColorEnum)
    {
        return piece::OU;
    }
}

namespace {
    // them(相手) 側の玉が逃げられるか。
    // sq : 王手した相手の駒の位置。紐付きか、桂馬の位置とする。よって、玉は sq には行けない。
    // bb : sq の利きのある場所のBitboard。よって、玉は bb のビットが立っている場所には行けない。
    // sq と ksq の位置の piece::Occupied Bitboard のみは、ここで更新して評価し、元に戻す。
    // (実際にはテンポラリのOccupied Bitboard を使うので、元には戻さない。)
    bool canKingEscape(const Position& pos, const color::ColorEnum us, const Square sq, const Bitboard& bb) {
        const color::ColorEnum them = color::opposite(us);
        const Square ksq = pos.kingSquare(them);
        Bitboard kingMoveBB = bb.notThisAnd(pos.bbOf(them).notThisAnd(kingAttack(ksq)));
        kingMoveBB.clearBit(sq); // sq には行けないので、クリアする。xorBit(sq)ではダメ。

        if (kingMoveBB) {
            Bitboard tempOccupied = pos.occupiedBB();
            tempOccupied.setBit(sq);
            tempOccupied.clearBit(ksq);
            do {
                const Square to = kingMoveBB.firstOneFromSQ11();
                // 玉の移動先に、us 側の利きが無ければ、true
                if (!pos.attackersToIsAny(us, to, tempOccupied))
                    return true;
            } while (kingMoveBB);
        }
        // 玉の移動先が無い。
        return false;
    }
    // them(相手) 側の玉以外の駒が sq にある us 側の駒を取れるか。
    bool canPieceCapture(const Position& pos, const color::ColorEnum them, const Square sq, const Bitboard& dcBB) {
        // 玉以外で打った駒を取れる相手側の駒の Bitboard
        Bitboard fromBB = pos.attackersToExceptKing(them, sq);

        if (fromBB) {
            const Square ksq = pos.kingSquare(them);
            do {
                const Square from = fromBB.firstOneFromSQ11();
                if (!pos.isDiscoveredCheck(from, sq, ksq, dcBB))
                    // them 側から見て、pin されていない駒で、打たれた駒を取れるので、true
                    return true;
            } while (fromBB);
        }
        // 玉以外の駒で、打った駒を取れない。
        return false;
    }

    // pos.discoveredCheckBB<false>() を遅延評価するバージョン。
    bool canPieceCapture(const Position& pos, const color::ColorEnum them, const Square sq) {
        Bitboard fromBB = pos.attackersToExceptKing(them, sq);

        if (fromBB) {
            const Square ksq = pos.kingSquare(them);
            const Bitboard dcBB = pos.discoveredCheckBB<false>();
            do {
                const Square from = fromBB.firstOneFromSQ11();
                if (!pos.isDiscoveredCheck(from, sq, ksq, dcBB))
                    // them 側から見て、pin されていない駒で、打たれた駒を取れるので、true
                    return true;
            } while (fromBB);
        }
        // 玉以外の駒で、打った駒を取れない。
        return false;
    }
}

// us が sq へ歩を打ったとき、them の玉が詰むか。
// us が sq へ歩を打つのは王手であると仮定する。
// 打ち歩詰めのとき、true を返す。
bool Position::isPawnDropCheckMate(const color::ColorEnum us, const Square sq) const {
    const color::ColorEnum them = color::opposite(us);
    // 玉以外の駒で、打たれた歩が取れるなら、打ち歩詰めではない。
    if (canPieceCapture(*this, them, sq))
        return false;
    // todo: ここで玉の位置を求めるのは、上位で求めたものと2重になるので無駄。後で整理すること。
    const Square ksq = kingSquare(them);

    // 玉以外で打った歩を取れないとき、玉が歩を取るか、玉が逃げるか。

    // 利きを求める際に、occupied の歩を打った位置の bit を立てた Bitboard を使用する。
    // ここでは歩の Bitboard は更新する必要がない。
    // color::ColorEnum の Bitboard も更新する必要がない。(相手玉が動くとき、こちらの打った歩で玉を取ることは無い為。)
    const Bitboard tempOccupied = occupiedBB() | setMaskBB(sq);
    Bitboard kingMoveBB = bbOf(them).notThisAnd(kingAttack(ksq));

    // 少なくとも歩を取る方向には玉が動けるはずなので、do while を使用。
    assert(kingMoveBB);
    do {
        const Square to = kingMoveBB.firstOneFromSQ11();
        if (!attackersToIsAny(us, to, tempOccupied))
            // 相手玉の移動先に自駒の利きがないなら、打ち歩詰めではない。
            return false;
    } while (kingMoveBB);

    return true;
}

inline void Position::xorBBs(const piece::PieceTypeEnum pt, const Square sq, const color::ColorEnum c) {
    byTypeBB_[piece::Occupied].xorBit(sq);
    byTypeBB_[pt].xorBit(sq);
    byColorBB_[c].xorBit(sq);
}

// sgn関数
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// 玉周辺の利きを求めるときに使う、玉周辺に利きをつける候補の駒を表すBB
// COLORのところは王手する側の駒
Bitboard CHECK_AROUND_BB[SquareNum + 1][piece::Promoted][color::NUM_COLORS];

// 敵玉8近傍の利きに関係する自駒の候補のbitboardを返す。ここになければ玉周辺に利きをつけない。
// pt = piece::FU～HDK
inline Bitboard check_around_bb(color::ColorEnum us, piece::PieceTypeEnum pt, Square sq_king) {
    return CHECK_AROUND_BB[sq_king][pt - 1][us];
}

// sq1に対してsq2の升の延長上にある次の升を得る。
// 隣接していないか、盤外になるときはSQUARE_NB
// テーブルサイズを小さくしておきたいのでu8にしておく。
/*Square*/ u8 NextSquare[SquareNum + 1][SquareNum + 1];
inline Square nextSquare(Square sq1, Square sq2) { return (Square)NextSquare[sq1][sq2]; }

// CHECK_AROUND_BBの初期化
void initMate1Ply() {
    for (piece::PieceTypeEnum p = piece::FU; p <= piece::OU; ++p)
        for (Square sq = SQ11; sq < SquareNum; ++sq)
            for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c) {
                Bitboard bb = allZeroBB(), tmp = allZeroBB();
                Square to;

                switch (p) {
                case piece::FU:
                    // これ用意するほどでもないんだな
                    // 一応、用意するコード書いておくか..
                    bb = pawnAttack(c, bb);
                    // →　このシフトでp[0]の63bit目に来るとまずいので..
                    bb &= allOneBB(); // allOneBB()でand取っておく。
                    break;

                case piece::KY:
                    // 香で玉8近傍の利きに関与するのは…。玉と同じ段より攻撃側の陣にある香だけか..
                    bb = lanceAttackToEdge(~c, sq);
                    if (makeFile(sq) != File1)
                        bb |= lanceAttackToEdge(~c, sq + DeltaE) | setMaskBB(sq + DeltaE);
                    if (makeFile(sq) != File9)
                        bb |= lanceAttackToEdge(~c, sq + DeltaW) | setMaskBB(sq + DeltaW);
                    break;

                case piece::KE:
                    // 桂は玉8近傍の逆桂か。
                    tmp = kingAttack(sq);
                    while (tmp) {
                        to = tmp.firstOneFromSQ11();
                        bb |= knightAttack(~c, to);
                    }
                    break;

                case piece::GI:
                    // 同じく
                    tmp = kingAttack(sq);
                    while (tmp) {
                        to = tmp.firstOneFromSQ11();
                        bb |= silverAttack(~c, to);
                    }
                    break;

                case piece::KI:
                    // 同じく
                    tmp = kingAttack(sq);
                    while (tmp) {
                        to = tmp.firstOneFromSQ11();
                        bb |= goldAttack(~c, to);
                    }
                    break;

                case piece::KA:
                    // 同じく
                    tmp = kingAttack(sq);
                    while (tmp) {
                        to = tmp.firstOneFromSQ11();
                        bb |= bishopAttackToEdge(to);
                    }
                    break;

                case piece::HI:
                    // 同じく
                    tmp = kingAttack(sq);
                    while (tmp) {
                        to = tmp.firstOneFromSQ11();
                        bb |= rookAttackToEdge(to);
                    }
                    break;

                    // HDK相当
                case piece::OU:
                    // 同じく
                    tmp = kingAttack(sq);
                    while (tmp) {
                        to = tmp.firstOneFromSQ11();
                        bb |= kingAttack(to);
                    }
                    break;

                default:
                    UNREACHABLE;
                }

                bb &= ~setMaskBB(sq); // sqの地点邪魔なので消しておく。
                // CHECK_CAND_BBとは並び順を変えたので注意。
                CHECK_AROUND_BB[sq][p - 1][c] = bb;
            }

    // NextSquareの初期化
    // Square NextSquare[SQUARE_NB][SQUARE_NB];
    // sq1に対してsq2の升の延長上にある次の升を得る。
    // 隣接していないか、盤外になるときはSQUARE_NB

    for (Square s1 = SQ11; s1 < SquareNum; ++s1)
        for (Square s2 = SQ11; s2 < SquareNum; ++s2) {
            Square next_sq = SquareNum;

            // 隣接していなくてもok。縦横斜かどうかだけ判定すべし。
            if ((rookAttackToEdge(s1) | bishopAttackToEdge(s1)) & setMaskBB(s2)) {
                const File vf = File(sgn(makeFile(s2) - makeFile(s1)));
                const Rank vr = Rank(sgn(makeRank(s2) - makeRank(s1)));

                const File s3f = makeFile(s2) + vf;
                const Rank s3r = makeRank(s2) + vr;
                // 盤面の範囲外に出ていないかのテスト
                if (isInFile(s3f) && isInRank(s3r))
                    next_sq = makeSquare(s3f, s3r);
            }
            NextSquare[s1][s2] = next_sq;
        }

}

// 桂馬が次に成れる移動元の表現のために必要となるので用意。
static const Bitboard RANK3_5BB = rankMask<Rank3>() | rankMask<Rank4>() | rankMask<Rank5>();
static const Bitboard RANK5_7BB = rankMask<Rank5>() | rankMask<Rank6>() | rankMask<Rank7>();

// usのSliderの利きを列挙する。
// avoid升にいる駒の利きは除外される。
Bitboard Position::attacksSlider(const color::ColorEnum us, const Bitboard& slide) const {
    Bitboard bb, sum = allZeroBB();
    Square from;

    bb = bbOf(piece::KY, us);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= lanceAttack(us, from, slide);
    }
    bb = bbOf(piece::KA, piece::UM, us);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= bishopAttack(from, slide);
    }
    bb = bbOf(piece::HI, piece::RY, us);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= rookAttack(from, slide);
    }
    return sum;
}

// usのSliderの利きを列挙する
// avoid升にいる駒の利きは除外される。
Bitboard Position::attacksSlider(const color::ColorEnum us, const Square avoid_from, const Bitboard& occ) const {
    Bitboard bb, sum = allZeroBB();
    Bitboard avoid_bb = ~setMaskBB(avoid_from);
    Square from;

    bb = bbOf(piece::KY, us) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= lanceAttack(us, from, occ);
    }
    bb = bbOf(piece::KA, piece::UM, us) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= bishopAttack(from, occ);
    }
    bb = bbOf(piece::HI, piece::RY, us) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= rookAttack(from, occ);
    }
    return sum;
}

// NonSliderの利きのみ列挙
template <color::ColorEnum US>
Bitboard Position::attacksAroundKingNonSlider() const {
    const Square sq_king = kingSquare(US);
    const color::ColorEnum Them = ~US;
    Square from;
    Bitboard bb;

    // 歩は普通でいい
    Bitboard sum = pawnAttack<Them>(bbOf(piece::FU, Them));

    // ほとんどのケースにおいて候補になる駒はなく、whileで回らずに抜けると期待している。
    bb = bbOf(piece::KE, Them) & check_around_bb(Them, piece::KE, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= knightAttack(Them, from);
    }
    bb = bbOf(piece::GI, Them) & check_around_bb(Them, piece::GI, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= silverAttack(Them, from);
    }
    bb = goldsBB(Them) & check_around_bb(Them, piece::KI, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= goldAttack(Them, from);
    }
    bb = bbOf(piece::UM, piece::RY, piece::OU, Them) & check_around_bb(Them, piece::OU, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= kingAttack(from);
    }
    return sum;
}

// Sliderの利きのみ列挙
template <color::ColorEnum US>
Bitboard Position::attacksAroundKingSlider() const {
    const Square sq_king = kingSquare(US);
    const color::ColorEnum Them = ~US;
    Square from;
    Bitboard bb;
    Bitboard sum = allZeroBB();

    bb = bbOf(piece::KY, Them) & check_around_bb(Them, piece::KY, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= lanceAttack(Them, from, occupiedBB());
    }
    bb = bbOf(piece::KA, piece::UM, Them) & check_around_bb(Them, piece::KA, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= bishopAttack(from, occupiedBB());
    }
    bb = bbOf(piece::HI, piece::RY, Them) & check_around_bb(Them, piece::HI, sq_king);
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= rookAttack(from, occupiedBB());
    }
    return sum;
}

template <color::ColorEnum US>
Bitboard Position::attacksAroundKingNonSliderInAvoiding(Square avoid_from) const {
    const Square sq_king = kingSquare(US);
    const color::ColorEnum Them = ~US;
    Bitboard bb;
    const Bitboard avoid_bb = ~setMaskBB(avoid_from);
    Square from;

    // 歩は普通でいい
    Bitboard sum = pawnAttack<Them>(bbOf(piece::FU, Them));

    // ほとんどのケースにおいて候補になる駒はなく、whileで回らずに抜けると期待している。
    bb = bbOf(piece::KE, Them) & check_around_bb(Them, piece::KE, sq_king) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= knightAttack(Them, from);
    }
    bb = bbOf(piece::GI, Them) & check_around_bb(Them, piece::GI, sq_king) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= silverAttack(Them, from);
    }
    bb = goldsBB(Them) & check_around_bb(Them, piece::KI, sq_king) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= goldAttack(Them, from);
    }
    bb = bbOf(piece::UM, piece::RY, piece::OU, Them) & check_around_bb(Them, piece::OU, sq_king) & avoid_bb;
    while (bb) {
        from = bb.firstOneFromSQ11();
        sum |= kingAttack(from);
    }
    return sum;
}

Bitboard Position::pinnedPieces(const color::ColorEnum us, const Square from, const Square to) const {
    Bitboard b, pinners, result = allZeroBB();
    const Square ksq = kingSquare(us);

    // avoidを除外して考える。
    const Bitboard avoid_bb = ~setMaskBB(from);

    pinners = (
        (bbOf(piece::HI, piece::RY) & rookAttackToEdge(ksq))
        | (bbOf(piece::KA, piece::UM) & bishopAttackToEdge(ksq))
        | (bbOf(piece::KY) & lanceAttackToEdge(us, ksq))
        ) & avoid_bb & bbOf(~us);

    // fromからは消えて、toの地点に駒が現れているものとして
    const Bitboard new_pieces = (occupiedBB() & avoid_bb) | setMaskBB(to);
    while (pinners) {
        b = betweenBB(ksq, pinners.firstOneFromSQ11()) & new_pieces;
        if (b.popCount<false>() <= 1)
            result |= b & bbOf(us);
    }
    return result;
}

namespace {

    // kingがtoとbb_avoid以外の升に逃げられるか
    // toに駒がない場合、駒が打たれているのでこれによって升は遮断されているものとして考える。
    bool can_king_escape(const Position& pos, const color::ColorEnum us, const Square to, const Bitboard& bb_avoid, const Bitboard& slide_) {
        // toには駒が置かれているのでこれにより利きの遮断は発生している。(attackers_to()で利きを見るときに重要)
        // captureの場合、もともとtoには駒があるわけで、ここをxorで処理するわけにはいかない。
        const Bitboard slide = slide_ | setMaskBB(to);

        const Square sq_king = pos.kingSquare(us);
        /*
        // kingもいないものとして考える必要がある。
        slide ^= sq_king;
        // これは呼び出し側でbb_avoidを計算するときに保証するものとする。
        */

        // bbとtoと自駒のないところから移動先を探す
        Bitboard bb = kingAttack(sq_king) & ~(bb_avoid | setMaskBB(to) | pos.bbOf(us));

        while (bb) {
            const Square escape = bb.firstOneFromSQ11();

            if (!pos.attackersTo(~us, escape, slide))
                return true;
            // 何も破壊していないので即座に返って良い。

        }
        return false;
    }

    // kingがtoとbb_avoid以外の升に逃げられるか
    // toに駒がない場合、駒が打たれているのでこれによって升は遮断されているものとして考える。
    // またfromからは駒が除去されているものとして考える。
    bool can_king_escape(const Position& pos, const color::ColorEnum us, const Square from, const Square to, const Bitboard& bb_avoid, const Bitboard& slide_) {
        const Square sq_king = pos.kingSquare(us);

        // toには駒が置かれているのでこれにより利きの遮断は発生している。(attackers_to()で利きを見るときに重要)
        // kingもいないものとして考える必要がある。
        const Bitboard slide = (slide_ | setMaskBB(to)) ^ setMaskBB(sq_king);

        // これは呼び出し側でbb_avoidを計算するときに保証するものとする。
        // →　ああ、だめだ。fromの後ろにあった駒での開き王手が..

        // bb_avoidとtoと自駒のないところから移動先を探す
        Bitboard bb = kingAttack(sq_king) & ~(bb_avoid | setMaskBB(to) | pos.bbOf(us));

        while (bb) {
            const Square escape = bb.firstOneFromSQ11();

            if (!(pos.attackersTo(~us, escape, slide) & ~setMaskBB(from)))
                // fromにある攻撃駒は移動済なのでこれは対象外。
                return true;
            // 何も破壊していないので即座に返って良い。

        }
        return false;
    }

    // kingがbb_avoid以外の升に逃げられるか
    // toに駒がない場合、駒が打たれているのでこれによって升は遮断されているものとして考える。
    // またfromからは駒が除去されているものとして考える。
    // ただしtoには行けるものとする。
    bool can_king_escape_cangoto(const Position& pos, const color::ColorEnum us, const Square from, const Square to, const Bitboard& bb_avoid, const Bitboard& slide_) {
        const Square sq_king = pos.kingSquare(us);

        // toには駒が置かれているのでこれにより利きの遮断は発生している。(attackers_to()で利きを見るときに重要)
        // kingもいないものとして考える必要がある。
        const Bitboard slide = (slide_ | setMaskBB(to)) ^ setMaskBB(sq_king);

        // これは呼び出し側でbb_avoidを計算するときに保証するものとする。
        // →　ああ、だめだ。fromの後ろにあった駒での開き王手が..

        // bb_avoid/*とto*/と自駒のないところから移動先を探す
        Bitboard bb = kingAttack(sq_king) & ~((bb_avoid /*| to*/ | pos.bbOf(us)) & ~setMaskBB(to));

        // toには移動できるのだよ。pos.pieces(us)には玉側の駒がtoにあることがあって、これは取られるものとして
        // 考える必要があるから、toを除外するコードが必要なのだよ。

        while (bb) {
            const Square escape = bb.firstOneFromSQ11();

            if (!(pos.attackersTo(~us, escape, slide) & ~setMaskBB(from)))
                // fromにある攻撃駒は移動済なのでこれは対象外。
                return true;
            // 何も破壊していないので即座に返って良い。

        }
        return false;
    }

    // 玉以外の駒でtoの駒が取れるのか？(toの地点には敵の利きがある or 届かないので玉では取れないものとする)
    bool can_piece_capture(const Position& pos, const color::ColorEnum us, const Square to, const Bitboard& pinned, const Bitboard& slide) {
        const Square sq_king = pos.kingSquare(us);

        // 玉以外の駒でこれが取れるのか？(toの地点には敵の利きがある or 届かないので玉では取れないものとする)
        Bitboard sum = pos.attackersTo(us, to, slide) & ~pos.bbOf(piece::OU);
        while (sum) {
            const Square from = sum.firstOneFromSQ11();

            // fromからtoに移動させて素抜きに合わないならばこれをもって良し。
            if (!pinned
                || !(pinned & setMaskBB(from))
                || isAligned<true>(from, to, sq_king)
                )
                return true;
        }

        return false;
    }

    // toにある駒が捕獲できるのか
    // ただしavoid升の駒でのcaptureは除外する。
    bool can_piece_capture(const Position& pos, const color::ColorEnum us, const Square to, const Square avoid, const Bitboard& pinned, const Bitboard& slide) {
        assert(isInSquare(to));

        const Square sq_king = pos.kingSquare(us);

        // 玉以外の駒でこれが取れるのか？(toの地点には敵の利きがあるので玉では取れないものとする)
        Bitboard sum = pos.attackersTo(us, to, slide) & ~(pos.bbOf(piece::OU) | setMaskBB(avoid));
        while (sum) {
            const Square from = sum.firstOneFromSQ11();

            // fromからtoに移動させて素抜きに合わないならばこれをもって良し。
            if (!pinned
                || !(pinned & setMaskBB(from))
                || isAligned<true>(from, to, sq_king)
                )
                return true;
        }

        return false;
    }

}

// 相手玉が1手詰みかどうかを判定。
// 1手詰みなら、詰みに至る指し手の一部の情報(from, to のみとか)を返す。
// 1手詰みでないなら、Move::moveNone() を返す。
// Bitboard の状態を途中で更新する為、const 関数ではない。(更新後、元に戻すが。)
template <color::ColorEnum US, bool Additional> Move Position::mateMoveIn1Ply() {
    constexpr color::ColorEnum Them = color::opposite(US);
    const Square ksq = kingSquare(Them);
    const SquareDelta TDeltaS = (US == color::BLACK ? DeltaS : DeltaN);

    assert(!attackersToIsAny(Them, kingSquare(US)));

    // 駒打ちを調べる。
    const Bitboard dropTarget = nOccupiedBB(); // emptyBB() ではないので注意して使うこと。
    const Hand ourHand = hand(US);
    // 王手する前の状態の dcBB。
    // 間にある駒は相手側の駒。
    // 駒打ちのときは、打った後も、打たれる前の状態の dcBB を使用する。
    const Bitboard dcBB_betweenIsThem = discoveredCheckBB<false>();

    // 飛車打ち
    if (ourHand.exists<piece::C_HI>()) {
        // 合駒されるとややこしいので、3手詰み関数の中で調べる。
        // ここでは離れた位置から王手するのは考えない。
        Bitboard toBB = dropTarget & rookStepAttacks(ksq);
        while (toBB) {
            const Square to = toBB.firstOneFromSQ11();
            // 駒を打った場所に自駒の利きがあるか。(無ければ玉で取られて詰まない)
            if (attackersToIsAny(US, to)) {
                // 玉が逃げられず、他の駒で取ることも出来ないか
                if (!canKingEscape(*this, US, to, rookAttackToEdge(to))
                    && !canPieceCapture(*this, Them, to, dcBB_betweenIsThem))
                {
                    return makeDropMove(piece::HI, to);
                }
            }
        }
    }
    // 香車打ち
    // 飛車で詰まなければ香車でも詰まないので、else if を使用。
    // 玉が 9(1) 段目にいれば香車で王手出来無いので、それも省く。
    else if (ourHand.exists<piece::C_KY>() && isInFrontOf<US, Rank9, Rank1>(makeRank(ksq))) {
        const Square to = ksq + TDeltaS;
        if (piece(to) == piece::Empty && attackersToIsAny(US, to)) {
            if (!canKingEscape(*this, US, to, lanceAttackToEdge(US, to))
                && !canPieceCapture(*this, Them, to, dcBB_betweenIsThem))
            {
                return makeDropMove(piece::KY, to);
            }
        }
    }

    // 角打ち
    if (ourHand.exists<piece::C_KA>()) {
        Bitboard toBB = dropTarget & bishopStepAttacks(ksq);
        while (toBB) {
            const Square to = toBB.firstOneFromSQ11();
            if (attackersToIsAny(US, to)) {
                if (!canKingEscape(*this, US, to, bishopAttackToEdge(to))
                    && !canPieceCapture(*this, Them, to, dcBB_betweenIsThem))
                {
                    return makeDropMove(piece::KA, to);
                }
            }
        }
    }

    // 金打ち
    if (ourHand.exists<piece::C_KI>()) {
        Bitboard toBB;
        if (ourHand.exists<piece::C_HI>())
            // 飛車打ちを先に調べたので、尻金だけは省く。
            toBB = dropTarget & (goldAttack(Them, ksq) ^ pawnAttack(US, ksq));
        else
            toBB = dropTarget & goldAttack(Them, ksq);
        while (toBB) {
            const Square to = toBB.firstOneFromSQ11();
            if (attackersToIsAny(US, to)) {
                if (!canKingEscape(*this, US, to, goldAttack(US, to))
                    && !canPieceCapture(*this, Them, to, dcBB_betweenIsThem))
                {
                    return makeDropMove(piece::KI, to);
                }
            }
        }
    }

    if (ourHand.exists<piece::C_GI>()) {
        Bitboard toBB;
        if (ourHand.exists<piece::C_KI>()) {
            // 金打ちを先に調べたので、斜め後ろから打つ場合だけを調べる。

            if (ourHand.exists<piece::C_KA>())
                // 角打ちを先に調べたので、斜めからの王手も除外できる。銀打ちを調べる必要がない。
                goto silver_drop_end;
            // 斜め後ろから打つ場合を調べる必要がある。
            toBB = dropTarget & (silverAttack(Them, ksq) & inFrontMask(US, makeRank(ksq)));
        }
        else {
            if (ourHand.exists<piece::C_KA>())
                // 斜め後ろを除外。前方から打つ場合を調べる必要がある。
                toBB = dropTarget & goldAndSilverAttacks(Them, ksq);
            else
                toBB = dropTarget & silverAttack(Them, ksq);
        }
        while (toBB) {
            const Square to = toBB.firstOneFromSQ11();
            if (attackersToIsAny(US, to)) {
                if (!canKingEscape(*this, US, to, silverAttack(US, to))
                    && !canPieceCapture(*this, Them, to, dcBB_betweenIsThem))
                {
                    return makeDropMove(piece::GI, to);
                }
            }
        }
    }
silver_drop_end:

    if (ourHand.exists<piece::C_KE>()) {
        Bitboard toBB = dropTarget & knightAttack(Them, ksq);
        while (toBB) {
            const Square to = toBB.firstOneFromSQ11();
            // 桂馬は紐が付いている必要はない。
            // よって、このcanKingEscape() 内での to の位置に逃げられないようにする処理は無駄。
            if (!canKingEscape(*this, US, to, allZeroBB())
                && !canPieceCapture(*this, Them, to, dcBB_betweenIsThem))
            {
                return makeDropMove(piece::KE, to);
            }
        }
    }

    // 歩打ちで詰ますと反則なので、調べない。

    // 駒を移動する場合
    // moveTarget は桂馬以外の移動先の大まかな位置。飛角香の遠隔王手は含まない。
    const Bitboard moveTarget = bbOf(US).notThisAnd(kingAttack(ksq));
    const Bitboard pinned = pinnedBB();
    const Bitboard dcBB_betweenIsUs = discoveredCheckBB<true>();

    {
        // 竜による移動
        Bitboard fromBB = bbOf(piece::RY, US);
        while (fromBB) {
            const Square from = fromBB.firstOneFromSQ11();
            // 遠隔王手は考えない。
            Bitboard toBB = moveTarget & attacksFrom<piece::RY>(from);
            if (toBB) {
                xorBBs(piece::RY, from, US);
                // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                // to の位置の Bitboard は canKingEscape の中で更新する。
                do {
                    const Square to = toBB.firstOneFromSQ11();
                    // 王手した駒の場所に自駒の利きがあるか。(無ければ玉で取られて詰まない)
                    if (unDropCheckIsSupported(US, to)) {
                        // 玉が逃げられない
                        // かつ、(空き王手 または 他の駒で取れない)
                        // かつ、王手した駒が pin されていない
                        if (!canKingEscape(*this, US, to, attacksFrom<piece::RY>(to, occupiedBB() ^ setMaskBB(ksq)))
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::RY, from, US);
                            return makeCaptureMove(piece::RY, from, to, *this);
                        }
                    }
                } while (toBB);
                xorBBs(piece::RY, from, US);
            }
        }
    }

    // Txxx は先手、後手の情報を吸収した変数。数字は先手に合わせている。
    const Rank TRank4 = (US == color::BLACK ? Rank4 : Rank6);
    const Bitboard TRank123BB = inFrontMask<US, TRank4>();
    {
        // 飛車による移動
        Bitboard fromBB = bbOf(piece::HI, US);
        Bitboard fromOn123BB = fromBB & TRank123BB;
        // from が 123 段目
        if (fromOn123BB) {
            fromBB.andEqualNot(TRank123BB);
            do {
                const Square from = fromOn123BB.firstOneFromSQ11();
                Bitboard toBB = moveTarget & attacksFrom<piece::HI>(from);
                if (toBB) {
                    xorBBs(piece::HI, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。
                    do {
                        const Square to = toBB.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            if (!canKingEscape(*this, US, to, attacksFrom<piece::RY>(to, occupiedBB() ^ setMaskBB(ksq)))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::HI, from, US);
                                return makeCapturePromoteMove(piece::HI, from, to, *this);
                            }
                        }
                    } while (toBB);
                    xorBBs(piece::HI, from, US);
                }
            } while (fromOn123BB);
        }

        // from が 4~9 段目
        while (fromBB) {
            const Square from = fromBB.firstOneFromSQ11();
            Bitboard toBB = moveTarget & attacksFrom<piece::HI>(from) & (rookStepAttacks(ksq) | TRank123BB);
            if (toBB) {
                xorBBs(piece::HI, from, US);
                // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();

                Bitboard toOn123BB = toBB & TRank123BB;
                // 成り
                if (toOn123BB) {
                    do {
                        const Square to = toOn123BB.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            if (!canKingEscape(*this, US, to, attacksFrom<piece::RY>(to, occupiedBB() ^ setMaskBB(ksq)))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::HI, from, US);
                                return makeCapturePromoteMove(piece::HI, from, to, *this);
                            }
                        }
                    } while (toOn123BB);

                    toBB.andEqualNot(TRank123BB);
                }
                // 不成
                while (toBB) {
                    const Square to = toBB.firstOneFromSQ11();
                    if (unDropCheckIsSupported(US, to)) {
                        if (!canKingEscape(*this, US, to, rookAttackToEdge(to))
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::HI, from, US);
                            return makeCaptureMove(piece::HI, from, to, *this);
                        }
                    }
                }
                xorBBs(piece::HI, from, US);
            }
        }
    }

    {
        // 馬による移動
        Bitboard fromBB = bbOf(piece::UM, US);
        while (fromBB) {
            const Square from = fromBB.firstOneFromSQ11();
            // 遠隔王手は考えない。
            Bitboard toBB = moveTarget & attacksFrom<piece::UM>(from);
            if (toBB) {
                xorBBs(piece::UM, from, US);
                // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                // to の位置の Bitboard は canKingEscape の中で更新する。
                do {
                    const Square to = toBB.firstOneFromSQ11();
                    // 王手した駒の場所に自駒の利きがあるか。(無ければ玉で取られて詰まない)
                    if (unDropCheckIsSupported(US, to)) {
                        // 玉が逃げられない
                        // かつ、(空き王手 または 他の駒で取れない)
                        // かつ、動かした駒が pin されていない)
                        if (!canKingEscape(*this, US, to, horseAttackToEdge(to)) // 竜の場合と違って、常に最大の利きを使用して良い。
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::UM, from, US);
                            return makeCaptureMove(piece::UM, from, to, *this);
                        }
                    }
                } while (toBB);
                xorBBs(piece::UM, from, US);
            }
        }
    }

    {
        // 角による移動
        Bitboard fromBB = bbOf(piece::KA, US);
        Bitboard fromOn123BB = fromBB & TRank123BB;
        // from が 123 段目
        if (fromOn123BB) {
            fromBB.andEqualNot(TRank123BB);
            do {
                const Square from = fromOn123BB.firstOneFromSQ11();
                Bitboard toBB = moveTarget & attacksFrom<piece::KA>(from);
                if (toBB) {
                    xorBBs(piece::KA, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。
                    do {
                        const Square to = toBB.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            if (!canKingEscape(*this, US, to, horseAttackToEdge(to))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::KA, from, US);
                                return makeCapturePromoteMove(piece::KA, from, to, *this);
                            }
                        }
                    } while (toBB);
                    xorBBs(piece::KA, from, US);
                }
            } while (fromOn123BB);
        }

        // from が 4~9 段目
        while (fromBB) {
            const Square from = fromBB.firstOneFromSQ11();
            Bitboard toBB = moveTarget & attacksFrom<piece::KA>(from) & (bishopStepAttacks(ksq) | TRank123BB);
            if (toBB) {
                xorBBs(piece::KA, from, US);
                // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();

                Bitboard toOn123BB = toBB & TRank123BB;
                // 成り
                if (toOn123BB) {
                    do {
                        const Square to = toOn123BB.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            if (!canKingEscape(*this, US, to, horseAttackToEdge(to))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::KA, from, US);
                                return makeCapturePromoteMove(piece::KA, from, to, *this);
                            }
                        }
                    } while (toOn123BB);

                    toBB.andEqualNot(TRank123BB);
                }
                // 不成
                while (toBB) {
                    const Square to = toBB.firstOneFromSQ11();
                    if (unDropCheckIsSupported(US, to)) {
                        if (!canKingEscape(*this, US, to, bishopAttackToEdge(to))
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::KA, from, US);
                            return makeCaptureMove(piece::KA, from, to, *this);
                        }
                    }
                }
                xorBBs(piece::KA, from, US);
            }
        }
    }

    {
        // 金、成り金による移動
        Bitboard fromBB = goldsBB(US) & goldCheckTable(US, ksq);
        while (fromBB) {
            const Square from = fromBB.firstOneFromSQ11();
            Bitboard toBB = moveTarget & attacksFrom<piece::KI>(US, from) & attacksFrom<piece::KI>(Them, ksq);
            if (toBB) {
                const piece::PieceTypeEnum pt = piece::to_piece_type(piece(from));
                xorBBs(pt, from, US);
                goldsBB_.xorBit(from);
                // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                // to の位置の Bitboard は canKingEscape の中で更新する。
                do {
                    const Square to = toBB.firstOneFromSQ11();
                    // 王手した駒の場所に自駒の利きがあるか。(無ければ玉で取られて詰まない)
                    if (unDropCheckIsSupported(US, to)) {
                        // 玉が逃げられない
                        // かつ、(空き王手 または 他の駒で取れない)
                        // かつ、動かした駒が pin されていない)
                        if (!canKingEscape(*this, US, to, attacksFrom<piece::KI>(US, to))
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(pt, from, US);
                            goldsBB_.xorBit(from);
                            return makeCaptureMove(pt, from, to, *this);
                        }
                    }
                } while (toBB);
                xorBBs(pt, from, US);
                goldsBB_.xorBit(from);
            }
        }
    }

    {
        // 銀による移動
        Bitboard fromBB = bbOf(piece::GI, US) & silverCheckTable(US, ksq);
        if (fromBB) {
            // Txxx は先手、後手の情報を吸収した変数。数字は先手に合わせている。
            const Bitboard TRank5_9BB = inFrontMask<Them, TRank4>();
            const Bitboard chkBB = attacksFrom<piece::GI>(Them, ksq);
            const Bitboard chkBB_promo = attacksFrom<piece::KI>(Them, ksq);

            Bitboard fromOn123BB = fromBB & TRank123BB;
            // from が敵陣
            if (fromOn123BB) {
                fromBB.andEqualNot(TRank123BB);
                do {
                    const Square from = fromOn123BB.firstOneFromSQ11();
                    Bitboard toBB = moveTarget & attacksFrom<piece::GI>(US, from);
                    Bitboard toBB_promo = toBB & chkBB_promo;

                    toBB &= chkBB;
                    if ((toBB_promo | toBB)) {
                        xorBBs(piece::GI, from, US);
                        // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                        const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                        // to の位置の Bitboard は canKingEscape の中で更新する。
                        while (toBB_promo) {
                            const Square to = toBB_promo.firstOneFromSQ11();
                            if (unDropCheckIsSupported(US, to)) {
                                // 成り
                                if (!canKingEscape(*this, US, to, attacksFrom<piece::KI>(US, to))
                                    && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                        || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                    && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                                {
                                    xorBBs(piece::GI, from, US);
                                    return makeCapturePromoteMove(piece::GI, from, to, *this);
                                }
                            }
                        }

                        // 玉の前方に移動する場合、成で詰まなかったら不成でも詰まないので、ここで省く。
                        // sakurapyon の作者が言ってたので実装。
                        toBB.andEqualNot(inFrontMask(Them, makeRank(ksq)));
                        while (toBB) {
                            const Square to = toBB.firstOneFromSQ11();
                            if (unDropCheckIsSupported(US, to)) {
                                // 不成
                                if (!canKingEscape(*this, US, to, attacksFrom<piece::GI>(US, to))
                                    && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                        || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                    && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                                {
                                    xorBBs(piece::GI, from, US);
                                    return makeCaptureMove(piece::GI, from, to, *this);
                                }
                            }
                        }

                        xorBBs(piece::GI, from, US);
                    }
                } while (fromOn123BB);
            }

            // from が 5~9段目 (必ず不成)
            Bitboard fromOn5_9BB = fromBB & TRank5_9BB;
            if (fromOn5_9BB) {
                fromBB.andEqualNot(TRank5_9BB);
                do {
                    const Square from = fromOn5_9BB.firstOneFromSQ11();
                    Bitboard toBB = moveTarget & attacksFrom<piece::GI>(US, from) & chkBB;

                    if (toBB) {
                        xorBBs(piece::GI, from, US);
                        // 動いた後の dcBB, pinned: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                        const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                        // to の位置の Bitboard は canKingEscape の中で更新する。
                        while (toBB) {
                            const Square to = toBB.firstOneFromSQ11();
                            if (unDropCheckIsSupported(US, to)) {
                                // 不成
                                if (!canKingEscape(*this, US, to, attacksFrom<piece::GI>(US, to))
                                    && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                        || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                    && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                                {
                                    xorBBs(piece::GI, from, US);
                                    return makeCaptureMove(piece::GI, from, to, *this);
                                }
                            }
                        }

                        xorBBs(piece::GI, from, US);
                    }
                } while (fromOn5_9BB);
            }

            // 残り 4 段目のみ
            // 前進するときは成れるが、後退するときは成れない。
            while (fromBB) {
                const Square from = fromBB.firstOneFromSQ11();
                Bitboard toBB = moveTarget & attacksFrom<piece::GI>(US, from);
                Bitboard toBB_promo = toBB & TRank123BB & chkBB_promo; // 3 段目にしか成れない。

                toBB &= chkBB;
                if ((toBB_promo | toBB)) {
                    xorBBs(piece::GI, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。
                    while (toBB_promo) {
                        const Square to = toBB_promo.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            // 成り
                            if (!canKingEscape(*this, US, to, attacksFrom<piece::KI>(US, to))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::GI, from, US);
                                return makeCapturePromoteMove(piece::GI, from, to, *this);
                            }
                        }
                    }

                    while (toBB) {
                        const Square to = toBB.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            // 不成
                            if (!canKingEscape(*this, US, to, attacksFrom<piece::GI>(US, to))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::GI, from, US);
                                return makeCaptureMove(piece::GI, from, to, *this);
                            }
                        }
                    }

                    xorBBs(piece::GI, from, US);
                }
            }
        }
    }

    {
        // 桂による移動
        Bitboard fromBB = bbOf(piece::KE, US) & knightCheckTable(US, ksq);
        if (fromBB) {
            const Bitboard chkBB_promo = attacksFrom<piece::KI>(Them, ksq) & TRank123BB;
            const Bitboard chkBB = attacksFrom<piece::KE>(Them, ksq);

            do {
                const Square from = fromBB.firstOneFromSQ11();
                Bitboard toBB = bbOf(US).notThisAnd(attacksFrom<piece::KE>(US, from));
                Bitboard toBB_promo = toBB & chkBB_promo;
                toBB &= chkBB;
                if ((toBB_promo | toBB)) {
                    xorBBs(piece::KE, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。
                    while (toBB_promo) {
                        const Square to = toBB_promo.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            // 成り
                            if (!canKingEscape(*this, US, to, attacksFrom<piece::KI>(US, to))
                                && (isDiscoveredCheck<true>(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal<true>(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::KE, from, US);
                                return makeCapturePromoteMove(piece::KE, from, to, *this);
                            }
                        }
                    }

                    while (toBB) {
                        const Square to = toBB.firstOneFromSQ11();
                        // 桂馬は紐が付いてなくて良いので、紐が付いているかは調べない。
                        // 不成
                        if (!canKingEscape(*this, US, to, allZeroBB())
                            && (isDiscoveredCheck<true>(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal<true>(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::KE, from, US);
                            return makeCaptureMove(piece::KE, from, to, *this);
                        }
                    }
                    xorBBs(piece::KE, from, US);
                }
            } while (fromBB);
        }
    }

    {
        // 香車による移動
        Bitboard fromBB = bbOf(piece::KY, US) & lanceCheckTable(US, ksq);
        if (fromBB) {
            // Txxx は先手、後手の情報を吸収した変数。数字は先手に合わせている。
            const SquareDelta TDeltaS = (US == color::BLACK ? DeltaS : DeltaN);
            const Rank TRank2 = (US == color::BLACK ? Rank2 : Rank8);
            const Bitboard chkBB_promo = attacksFrom<piece::KI>(Them, ksq) & TRank123BB;
            // 玉の前方1マスのみ。
            // 玉が 1 段目にいるときは、成のみで良いので省く。
            const Bitboard chkBB = attacksFrom<piece::FU>(Them, ksq) & inFrontMask<Them, TRank2>();

            do {
                const Square from = fromBB.firstOneFromSQ11();
                Bitboard toBB = moveTarget & attacksFrom<piece::KY>(US, from);
                Bitboard toBB_promo = toBB & chkBB_promo;

                toBB &= chkBB;

                if ((toBB_promo | toBB)) {
                    xorBBs(piece::KY, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。

                    while (toBB_promo) {
                        const Square to = toBB_promo.firstOneFromSQ11();
                        if (unDropCheckIsSupported(US, to)) {
                            // 成り
                            if (!canKingEscape(*this, US, to, attacksFrom<piece::KI>(US, to))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::KY, from, US);
                                return makeCapturePromoteMove(piece::KY, from, to, *this);
                            }
                        }
                    }

                    if (toBB) {
                        assert(toBB.isOneBit());
                        // 不成で王手出来るのは、一つの場所だけなので、ループにする必要が無い。
                        const Square to = ksq + TDeltaS;
                        if (unDropCheckIsSupported(US, to)) {
                            // 不成
                            if (!canKingEscape(*this, US, to, lanceAttackToEdge(US, to))
                                && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                    || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                                && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                            {
                                xorBBs(piece::KY, from, US);
                                return makeCaptureMove(piece::KY, from, to, *this);
                            }
                        }
                    }
                    xorBBs(piece::KY, from, US);
                }
            } while (fromBB);
        }
    }

    {
        // 歩による移動
        // 成れる場合は必ずなる。
        // todo: PawnCheckBB 作って簡略化する。
        const Rank krank = makeRank(ksq);
        // 歩が移動して王手になるのは、相手玉が1~7段目の時のみ。
        if (isInFrontOf<US, Rank8, Rank2>(krank)) {
            // Txxx は先手、後手の情報を吸収した変数。数字は先手に合わせている。
            const SquareDelta TDeltaS = (US == color::BLACK ? DeltaS : DeltaN);
            const SquareDelta TDeltaN = (US == color::BLACK ? DeltaN : DeltaS);

            Bitboard fromBB = bbOf(piece::FU, US);
            // 玉が敵陣にいないと成で王手になることはない。
            if (isInFrontOf<US, Rank4, Rank6>(krank)) {
                // 成った時に王手になる位置
                const Bitboard toBB_promo = moveTarget & attacksFrom<piece::KI>(Them, ksq) & TRank123BB;
                Bitboard fromBB_promo = fromBB & pawnAttack<Them>(toBB_promo);
                while (fromBB_promo) {
                    const Square from = fromBB_promo.firstOneFromSQ11();
                    const Square to = from + TDeltaN;

                    xorBBs(piece::FU, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。
                    if (unDropCheckIsSupported(US, to)) {
                        // 成り
                        if (!canKingEscape(*this, US, to, attacksFrom<piece::KI>(US, to))
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::FU, from, US);
                            return makeCapturePromoteMove(piece::FU, from, to, *this);
                        }
                    }
                    xorBBs(piece::FU, from, US);
                }
            }

            // 不成
            // 玉が 8,9 段目にいることは無いので、from,to が隣の筋を指すことは無い。
            const Square to = ksq + TDeltaS;
            const Square from = to + TDeltaS;
            if (fromBB.isSet(from) && !bbOf(US).isSet(to)) {
                // 玉が 1, 2 段目にいるなら、成りで王手出来るので不成は調べない。
                if (isBehind<US, Rank2, Rank8>(krank)) {
                    xorBBs(piece::FU, from, US);
                    // 動いた後の dcBB: to の位置の occupied や checkers は関係ないので、ここで生成できる。
                    const Bitboard dcBB_betweenIsThem_after = discoveredCheckBB<false>();
                    // to の位置の Bitboard は canKingEscape の中で更新する。
                    if (unDropCheckIsSupported(US, to)) {
                        // 不成
                        if (!canKingEscape(*this, US, to, allZeroBB())
                            && (isDiscoveredCheck(from, to, ksq, dcBB_betweenIsUs)
                                || !canPieceCapture(*this, Them, to, dcBB_betweenIsThem_after))
                            && !isPinnedIllegal(from, to, kingSquare(US), pinned))
                        {
                            xorBBs(piece::FU, from, US);
                            return makeCaptureMove(piece::FU, from, to, *this);
                        }
                    }
                    xorBBs(piece::FU, from, US);
                }
            }
        }
    }

    if (Additional) {
        // 以下は、Aperyにはなかった処理
        // やねうら王の実装を参考にした
        // https://github.com/yaneurao/YaneuraOu/blob/013f924d58891b5830bb8f6fe27b78e414141aaa/source/mate/mate1ply_without_effect.cpp#L1105

        // 離し角・飛車等で詰むかどうか。
        // 離し角・離し飛車、移動飛車・龍での合い効かずで詰むかも知れん。
        // Bonanzaにはないが、これを入れておかないと普通の1手詰め判定と判定される集合が違って気持ち悪い。

        // 飛車持ちかつ、相手は歩だけ(歩は二歩で合い効かず)かつ
        // 移動可能箇所が3箇所以内
        // という判定条件で残り2箇所が利きがあり移動不可であることがわかれば…みたいな条件にしとくか。
        // てか、これ利き真面目に考慮してはいかんのか？
        // うーむ..

        // 合い駒なしである可能性が高い

        // 敵は歩以外を持っていないか。

        const Hand themHand = hand(Them);
        if (!themHand.exceptPawnExists()) {
            // 玉の8近傍の移動可能箇所の列挙
            const Bitboard bb_king_movable = ~bbOf(Them) & kingAttack(ksq);

            // 玉周辺の利きを列挙。(これ、せっかく求めたならできればあとで使いまわしたいが…)
            // これ王手のかかっていない局面で呼び出すことを想定しているので貫通でなくてもいいか。
            const Bitboard aakns = attacksAroundKingNonSlider<~US>(); // これはあとで使いまわす
            const Bitboard aaks = attacksAroundKingSlider<~US>();
            const Bitboard aak = aakns | aaks;

            Bitboard escape_bb = bb_king_movable & ~aak; // 利きがない場所が退路の候補

            const int esc_count = escape_bb.popCount();
            if (esc_count >= 4)
                goto NEXT1; // 残念ながら退路がありすぎて話にならんかった。詰む可能性低いので調べなくていいや。
                            // 退路3個はまだ許せる…。としよう。

                            // 退路がなかろうが、あろうが、玉8近傍の駒のない升に対して順番に探そう。
                            // 退路が3以下である以上、そんなに空いてはないはずだ。
            Bitboard bb2 = nOccupiedBB() & kingAttack(ksq);

            while (bb2) {
                // 退路
                const Square one = bb2.firstOneFromSQ11();

                // このあと
                // 1. ksqのone側のもうひとつ先の升toにksqに利く駒が打てて
                // 2. その升に敵の利きがなくて、
                // 3. oneの升に歩が立たないなら
                // これにて詰み

                // 駒が打つ場所と間の升が空いている
                // →　これは前提条件から自動的に満たす
                // if (!pos.empty(one)) continue; // もう駄目

                // toの地点が盤外
                // このチェックが惜しいのなら最初玉の8近傍ではなく、toが盤外にならない8近傍を列挙すべきだが。
                const Square to = nextSquare(ksq, one);
                if (to == SquareNum) continue; // もう駄目

                // toが自駒だとここに移動できないし..
                if (piece(to) != piece::Empty && piece::get_color(piece(to)) == US) continue;

                // oneが二歩で打てないことを確認しよう。
                if (canPawnDrop<~US>(one)) continue; // もう駄目

                // toの地点にあるのが歩だと、このtoの地点とoneが同じ筋だと
                // このtoの歩を取ってoneに打てるようになってしまう。
                if (piece::to_piece_type(piece(to)) == piece::FU && makeFile(to) == makeFile(one) && themHand.exists<piece::C_FU>()) continue;

                const auto dr = Effect8::directions_of(ksq, one);
                piece::PieceTypeEnum pt;
                bool canLanceAttack = false;
                if (dr & Effect8::DIRECTIONS_DIAG) {
                    pt = piece::KA;

                    // 斜めなら角を持ってなきゃ
                    if (!ourHand.exists<piece::C_KA>())
                        goto NEXT2;
                }
                else {
                    pt = piece::HI;

                    // 十字なら飛車を持ってなきゃ
                    // 上からなら香でもいいのか。
                    canLanceAttack = (US == color::BLACK ? dr == Effect8::DIRECTIONS_D : dr == Effect8::DIRECTIONS_U);
                    if (canLanceAttack && ourHand.exists<piece::C_KY>()) {
                        pt = piece::KY;
                    }
                    else if (!ourHand.exists<piece::C_HI>())
                        goto NEXT2;
                }

                if (piece(to)) goto NEXT2;
                // このケースはtoの駒を取ればいけるかも知れん。盤上の駒ptを移動できないか調べろ

                // oneに駒の移動合いができない
                if (can_piece_capture(*this, Them, one, dcBB_betweenIsThem, occupiedBB())) goto NEXT2;

                // toに打つ駒が取れない
                if (can_piece_capture(*this, Them, to, dcBB_betweenIsThem, occupiedBB())) goto NEXT2;

                // 退路が1個以下であればこれで詰みなのだが、もともと利きがあって塞がれていたほうの退路だとそうでもないから
                // 最終的に次のような処理が必要なのだ…。

                // 退路が2個以上ある場合は、これで詰むとは限らない。
                // escape_bbが打った駒の利きによって遮断されているかを調べる。
                // あ、しまった。toを打ったことによってescape_bbのどこかがまた状態が変わるのか…。
                escape_bb = bb_king_movable & ~(aakns | attacksSlider(US, occupiedBB() | setMaskBB(to)));

                if (dr & Effect8::DIRECTIONS_DIAG) { // pt == piece::KA
                    if (!(~bishopAttackToEdge(to) & escape_bb))
                        return makeDropMove(pt, to);
                }
                else { // if (pt == piece::HI || pt==piece::KY)
                    // Lanceの場合もtoの地点からの横の利きでは玉の8近傍に到達しないので同列に扱って良い。
                    if (!(~rookAttackToEdge(to) & escape_bb))
                        return makeDropMove(pt, to);
                }

                //    STEP2_DROP:;
                // toに打ったからsliderが遮断されたんでねーの？1升だけ延長する。
                if (esc_count <= 2) {
                    const Square nextTo = nextSquare(one, to);
                    if (nextTo == SquareNum) goto NEXT2;
                    if (piece(nextTo)) goto NEXT2;
                    if (canPawnDrop<~US>(to)) goto NEXT2;
                    if (can_piece_capture(*this, Them, nextTo, dcBB_betweenIsThem, occupiedBB())) goto NEXT2;

                    escape_bb = bb_king_movable & ~(aakns | attacksSlider(US, occupiedBB() | setMaskBB(nextTo)));

                    if (dr & Effect8::DIRECTIONS_DIAG) { // pt == piece::KA
                        if (!(~bishopAttackToEdge(nextTo) & escape_bb))
                            return makeDropMove(pt, nextTo);
                    }
                    else { // if (pt == piece::HI || pt==piece::KY)
                        if (!(~rookAttackToEdge(nextTo) & escape_bb))
                            return makeDropMove(pt, nextTo);
                    }
                }

            NEXT2:
                // この場合、toの地点に遠方駒を移動させてcapれば、高い確率で詰みなのだが。

                if (!(dr & Effect8::DIRECTIONS_DIAG)) { // (pt == piece::HI || pt == piece::KY)
                    // どこかtoの近くに飛車は落ちてないかね..
                    // 飛車を移動させた結果、oneに敵の利きが生じるかも知らんけど。
                    const bool is_rook = rookAttackToEdge(to).andIsAny(bbOf(piece::HI, piece::RY, US));
                    const bool is_dragon = kingAttack(to).andIsAny(bbOf(piece::RY, US));
                    const bool is_lance = (canLanceAttack) ? (lanceAttackToEdge(Them, to).andIsAny(bbOf(piece::KY, US))) : false;

                    if (is_rook || is_dragon || is_lance) {
                        // 落ちてるっぽい。移動可能かどうか調べる。
                        Bitboard bb = allZeroBB();
                        if (is_rook)
                            bb = rookAttack(to, occupiedBB()) & bbOf(piece::HI, piece::RY, US);
                        if (is_dragon)
                            bb |= kingAttack(to) & bbOf(piece::RY, US);
                        if (is_lance)
                            bb |= lanceAttack(Them, to, occupiedBB()) & bbOf(piece::KY, US);

                        while (bb) {
                            const Square from = bb.firstOneFromSQ11();
                            // fromからtoに移動させてこれで詰むかどうかテスト

                            // この指し手が合法でないといかん。
                            if (isPinnedIllegal(from, to, kingSquare(US), pinned)) continue;

                            const Bitboard slide = occupiedBB() ^ setMaskBB(from);

                            // toに移動させた駒が取れない
                            if (can_piece_capture(*this, Them, to, dcBB_betweenIsThem, slide)) continue;
                            // oneも移動合い等ができない。
                            // toへの移動でさらにpinされることはあっても、pinが解除されることはない。
                            // (将棋にはQUEENがないので)
                            // ゆえに古いpinで考えておけばいいや。
                            if (can_piece_capture(*this, Them, one, to, dcBB_betweenIsThem, slide)) continue;

                            // 攻撃範囲計算用
                            Bitboard bb_attacks;

                            if (piece::to_piece_type(piece(from)) == piece::KY) {
                                bb_attacks = rookAttackToEdge(to);
                                // 貫通で考えておこう。裏の退路もいけないので。
                                // 1升以上離れているので王手にするには不成りでいくしかなく、これは飛車利きに等しい
                            }
                            else if (canPromote(US, from, to) || piece::to_piece_type(piece(from)) == piece::RY) {
                                bb_attacks = rookAttackToEdge(to) | bishopAttackToEdge(to);
                            }
                            else
                                bb_attacks = rookAttackToEdge(to);
                            // 貫通で考えておこう。裏の退路もいけないので。

                            const Bitboard new_slide = (occupiedBB() ^ setMaskBB(from)) | setMaskBB(to);

                            // aakns、小駒だから関係ないと思いきや、馬を動かすと関係あるんだな
                            // aakns使わない実装にしよう..

                            if (!(kingAttack(ksq)
                                & ~(bbOf(Them) | attacksAroundKingInAvoiding<~US>(from, new_slide) | bb_attacks))) {
                                // これで詰みが確定した
                                const piece::PieceTypeEnum pt = piece::to_piece_type(piece(from));
                                // 香は不成りでの王手
                                if (pt != piece::KY && canPromote(US, from, to) && !(pt & piece::PTPromote))
                                    return makePromoteMove<Capture>(pt, from, to, *this);
                                else
                                    return makeCaptureMove(pt, from, to, *this);
                            }
                        }
                    }
                }
                else {
                    // 同じく角

                    const bool is_bishop = bishopAttackToEdge(to).andIsAny(bbOf(piece::KA, piece::UM, US));
                    const bool is_horse = kingAttack(to).andIsAny(bbOf(piece::UM, US));
                    if (is_bishop || is_horse) {
                        // 落ちてるっぽい。移動可能かどうか調べる。
                        Bitboard bb = allZeroBB();
                        if (is_bishop)
                            bb = bishopAttack(to, occupiedBB()) & bbOf(piece::KA, piece::UM, US);
                        if (is_horse)
                            bb |= kingAttack(to) & bbOf(piece::UM, US);

                        while (bb) {
                            const Square from = bb.firstOneFromSQ11();
                            // fromからtoに移動させてこれで詰むかどうかテスト

                            // この指し手が合法でないといかん。
                            if (isPinnedIllegal(from, to, kingSquare(US), pinned)) continue;

                            const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                            // oneに駒の移動合いができない
                            // このときtoの駒はcapられている可能性があるのでこの駒による移動合いは除外する。
                            if (can_piece_capture(*this, Them, one, to, dcBB_betweenIsThem, slide)) continue;

                            // toに打つ駒が取れない
                            if (can_piece_capture(*this, Them, to, dcBB_betweenIsThem, slide)) continue;

                            // fromから飛車がいなくなったことにより、利きがどうなるかを調べる必要がある。
                            // 王手になることは保証されているから、
                            // 玉周辺の退路(自駒のない場所)のうち、攻撃側の利きがないone以外の場所を探すが、それがないということだから..
                            // fromの駒を消して玉周辺の利きを調べる必要がある。少し重いがこれを呼ぶか..
                            // 馬の場合でも、one以外に玉の8近傍には利いていないので龍のときのような処理は不要。

                            //cout << kingAttack(ksq) << bbOf(Them) << aakns
                            //  << pos.AttacksAroundKingSlider(them, from, to) << pos.StepAttacksQueen(to);

                            const Bitboard new_slide = (occupiedBB() ^ setMaskBB(from)) | setMaskBB(to);

                            if (!(kingAttack(ksq)
                                & ~(bbOf(Them) | attacksAroundKingInAvoiding<~US>(from, new_slide) | rookAttackToEdge(to) | bishopAttackToEdge(to)
                                    ))) {
                                // 貫通で考えておく
                                // これで詰みが確定した
                                const piece::PieceTypeEnum pt = piece::to_piece_type(piece(from));
                                if (canPromote(US, from, to) && !(pt & piece::PTPromote))
                                    return makePromoteMove<Capture>(pt, from, to, *this);
                                else
                                    return makeCaptureMove(pt, from, to, *this);
                            }
                        }
                    }
                }
                // toへ移動させる指し手終わり。

            }
        }
    NEXT1:

        const Bitboard bb_move = ~bbOf(US);

        // 両王手による詰み
        if (dcBB_betweenIsUs) {
            // せっかくdcCandidatesが使えるのだから両王手も検出しよう。
            // 開き王手になる候補の駒があること自体レアなのでここは全駒列挙でいいだろう。

            // 敵陣
            const Bitboard enemyBB = enemyField(US);

            Bitboard dcBB = dcBB_betweenIsUs;
            Bitboard bb;

            while (dcBB) {
                const Square from = dcBB.firstOneFromSQ11();
                piece::PieceTypeEnum pt = piece::to_piece_type(piece(from));
                switch (pt) {
                    // 背後にいる駒は角が普通で、pinされているのは歩で成りとか、飛車で両王手とか、そんなのが
                    // よくあるパターンではないかいな。

                case piece::FU: {
                    // 同じ筋だとpin方向と違う方向の移動にならない。
                    if (makeFile(from) == makeFile(ksq)) { continue; }

                    // 移動性の保証
                    const Square to = from + (US == color::BLACK ? DeltaN : DeltaS);
                    if (piece(to) != piece::Empty && piece::get_color(piece(to)) != ~US) { continue; }

                    // toの地点で成れないと駄目
                    if (!canPromote(US, to)) continue;

                    // toの地点に敵の利きがあるか、もしくは自分の利きがないなら、
                    // この変化で1手詰めにならないことはすでに調べているので除外すべきだが、除外するコストも馬鹿にならないので
                    // このまま調べてしまう。

                    // これが王手になってないと話にならない。
                    const Bitboard bb_attacks = goldAttack(US, to);
                    if (!(bb_attacks & setMaskBB(ksq))) continue;

                    // 移動が合法手であること。
                    if (isPinnedIllegal(from, to, kingSquare(US), pinned)) { continue; }

                    // 駒がfromからtoに移動したときに退路があるか。ただしbb_attackはいけないものとする。
                    const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                    if (can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) { continue; }

                    // すべての条件が成立したのでこれにて詰み
                    return makePromoteMove<Capture>(piece::FU, from, to, *this);
                }
                case piece::KY:
                    continue; // 香による両王手はない。

                case piece::KE: {
                    if (!(check_around_bb(US, piece::KE, ksq) & setMaskBB(from))) continue;

                    bb = knightAttack(US, from) & knightAttack(Them, ksq) & bb_move;
                    while (bb) {
                        const Square to = bb.firstOneFromSQ11();
                        if (isAligned<true>(from, to, ksq)) { continue; }
                        const Bitboard bb_attacks = knightAttack(US, to);
                        if (bb_attacks & setMaskBB(ksq)) { continue; }
                        if (isPinnedIllegal(from, to, kingSquare(US), pinned)) { continue; }
                        const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                        if (can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) { continue; }
                        return makeCaptureMove(piece::KE, from, to, *this);
                    }

                    bb = knightAttack(US, from) & goldAttack(Them, ksq);
                    while (bb) {
                        const Square to = bb.firstOneFromSQ11();
                        if (isAligned<true>(from, to, ksq)) { continue; }
                        if (!(canPromote(US, from, to))) { continue; }
                        const Bitboard bb_attacks = goldAttack(US, to);
                        if (bb_attacks & setMaskBB(ksq)) { continue; }
                        if (isPinnedIllegal(from, to, kingSquare(US), pinned)) { continue; }
                        const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                        if (can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) { continue; }
                        return makePromoteMove<Capture>(piece::KE, from, to, *this);
                    }

                    continue; // 気をつけろ！下に落ちたら死ぬぞ！
                }

                case piece::GI: {
                    // 王手になる見込みがない
                    if (!(check_around_bb(US, piece::GI, ksq) & setMaskBB(from))) continue;
                    // これで王手にはなる。成りも選択したいのでここコピペで書くか..それともlambdaで書くか..コピペでいいか。

                    bb = silverAttack(US, from) & silverAttack(Them, ksq) & bb_move;;
                    while (bb) {
                        const Square to = bb.firstOneFromSQ11();
                        if (isAligned<true>(from, to, ksq)) { continue; }
                        const Bitboard bb_attacks = silverAttack(US, to);
                        if (bb_attacks & setMaskBB(ksq)) { continue; }
                        if (isPinnedIllegal(from, to, kingSquare(US), pinned)) { continue; }
                        const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                        if (can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) { continue; }
                        return makeCaptureMove(piece::GI, from, to, *this);
                    }

                    bb = silverAttack(US, from) & goldAttack(Them, ksq) & bb_move;;
                    while (bb) {
                        const Square to = bb.firstOneFromSQ11();
                        if (isAligned<true>(from, to, ksq)) { continue; }
                        if (!(canPromote(US, from, to))) { continue; }
                        const Bitboard bb_attacks = goldAttack(US, to);
                        if (bb_attacks & setMaskBB(ksq)) { continue; }
                        if (isPinnedIllegal(from, to, kingSquare(US), pinned)) { continue; }
                        const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                        if (can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) { continue; }
                        return makePromoteMove<Capture>(piece::GI, from, to, *this);
                    }
                    continue;
                }

                case piece::TO:
                case piece::NY:
                case piece::NK:
                case piece::NG:
                    pt = piece::KI; // 以下の処理でややこしいのでGold扱いにしてしまう。
                    // FALLTHROUGH
                case piece::KI:
                    // 王手になる見込みがない
                    if (!(check_around_bb(US, piece::KI, ksq) & setMaskBB(from))) continue;

                    // 王手生成ルーチンみたいな処理が必要なんだな..
                    bb = goldAttack(US, from) & goldAttack(Them, ksq);
                    // この移動先であれば王手になる。
                    break;

                case piece::KA:

                    bb = bishopAttack(ksq, occupiedBB()) |
                        (kingAttack(ksq) & (canPromote(US, from) ? allOneBB() : enemyBB));
                    // 敵陣8近傍、王からの角の利き、fromが敵陣であれば、敵陣にかぎらず玉8近傍も。
                    // ここが角が移動してくれば王手になる升
                    // これと角の利きとの交差をとって、そこを移動の候補とする。
                    bb &= bishopAttack(from, occupiedBB());

                    //        bb = pos.AttackBishop(from, occupiedBB()) & around24_bb(ksq);

                    break;

                case piece::UM:
                    bb = horseAttack(from, occupiedBB()) & horseAttack(ksq, occupiedBB());

                    //        bb = pos.AttackHorse(from, occupiedBB()) & around24_bb(ksq);

                    break;

                case piece::HI:
                    // 角のときと同様
                    bb = rookAttack(ksq, occupiedBB()) |
                        (kingAttack(ksq) & (canPromote(US, from) ? allOneBB() : enemyBB));
                    bb &= rookAttack(from, occupiedBB());

                    // いやー。龍がpinされているということは背後にいるのはたぶん角であって、
                    // 玉の24近傍への移動で高い確率で詰むような..

                    //        bb = pos.AttackRook(from, occupiedBB()) & around24_bb(ksq);
                    // ここ、両王手専用につき、合駒見てないのよね。だから、この条件をここに入れるわけにはいかんのよ…。

                    break;

                case piece::RY:

                    bb = dragonAttack(from, occupiedBB()) & dragonAttack(ksq, occupiedBB());

                    //        bb = pos.AttackDragon(from, occupiedBB()) & around24_bb(ksq);

                    break;

                default:
                    assert(pt == piece::OU);
                    continue;
                }

                bb &= bb_move;

                const bool is_enemy_from = canPromote(US, from);

                // 候補はほとんどないはずだが。
                while (bb) {
                    const Square to = bb.firstOneFromSQ11();
                    const bool promo = is_enemy_from || canPromote(US, to);

                    // これ、開き王手になってないと駄目
                    if (isAligned<true>(from, to, ksq)) { continue; }

                    if (isPinnedIllegal(from, to, kingSquare(US), pinned)) { continue; }

                    // この地点でのこの駒の利きは..
                    //bb_attacks = pos.attacks_from(make_piece(Us, pt), to, occupiedBB() ^ ksq); // ksqが除去されて貫通である必要がある。
                    // この処理気持ち悪いか..王手できることは確定しているのだから駒種別にやってしまうべきか。

                    const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                    // 攻撃範囲計算用
                    Bitboard bb_attacks;
                    switch (pt) {
                    case piece::GI:
                        if (!promo) goto DC_Silver_NO_PRO;
                        bb_attacks = goldAttack(US, to); break;
                    case piece::KI: bb_attacks = goldAttack(US, to); break;
                    case piece::KA:
                        bb_attacks = bishopAttackToEdge(to);
                        if (promo)
                            bb_attacks |= kingAttack(to);
                        break;
                    case piece::UM: bb_attacks = bishopAttackToEdge(to) | kingAttack(to); break;
                    case piece::HI:
                        bb_attacks = rookAttackToEdge(to);
                        if (promo)
                            bb_attacks |= kingAttack(to);
                        break;
                    case piece::RY: bb_attacks = rookAttackToEdge(to) | kingAttack(to); break;
                    default:
                        assert(false);
                        bb_attacks = allZeroBB();
                    }

                    if (!can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) {
                        if (promo && !(pt & piece::PTPromote) && pt != piece::KI)
                            return makePromoteMove<Capture>(pt, from, to, *this);
                        return makeCaptureMove(pt, from, to, *this);
                    }

                DC_Silver_NO_PRO:
                    if (pt == piece::GI) {
                        // 銀のときだけ銀成り/不成りの判定が必要だわさ..
                        // 上では成りを判定済なので不成りでの王手を判定

                        bb_attacks = silverAttack(US, to);
                        if (!can_king_escape_cangoto(*this, Them, from, to, bb_attacks, slide)) {
                            return makeCaptureMove(piece::GI, from, to, *this);
                        }
                    }
                }
            }
        }
        // 両王手ではないが、玉の24近傍から24-8 = 16近傍への移動で、かつfromに利きがなければ
        // この移動で詰む可能性が濃厚なので、これについては調べることにする。
        // 合い駒なしである可能性が高い場合についてのみ。

        // 歩以外を持っていないか。

        if (dcBB_betweenIsUs && !themHand.exceptPawnExists()) {
            // 玉の8近傍にある開き王手可能駒について
            //    bb = dcCandidates & kingAttack(ksq);
            // 24近傍まで拡張していいだろう。

            Bitboard bb = dcBB_betweenIsUs & neighbor5x5Table(ksq);

            while (bb) {
                const Square from = bb.firstOneFromSQ11();

                // if (can_piece_capture(pos, them, from, pinned, occupiedBB()))
                //  continue;
                // この駒が取られるというなら、その取られる駒のいる升への移動ぐらい調べれば良さそうだが。
                // 遠方からの利きなら、その利きを遮断できるならその方向に移動させるだとか、いろいろありそうだが…。
                // まあいいか…。判定が難しいしな。

                Bitboard atk = attackersTo(Them, from) & ~setMaskBB(ksq);
                if (atk) {
                    if (atk.popCount() >= 2)
                        continue; // 2つ以上利きがあるなら消せないわ

                                  // 1つしかないので、その場所への移動を中心に考えよう。そこは敵駒なのでbb_moveを見るまでもなく
                                  // 上の升には移動可能
                }
                else {
                    // 24近傍(ただし、馬・龍は16近傍)
                    atk = neighbor5x5Table(ksq) & bb_move; // 別にどこでも良いものとする
                }

                piece::PieceTypeEnum pt = piece::to_piece_type(piece(from));
                // 攻撃範囲計算用
                Bitboard bb_attacks;
                switch ((int)pt) { // intにcastしとかないとhandleしてない値に対して警告がでる。
                case piece::FU:
                case piece::KY:
                    // 歩が玉の24近傍から成って開き王手で詰むパターンはめったに出てこないのでこれはいいや
                    continue;

                case piece::KE:
                    // 成って詰みはあるか..それだけ見るか..
                    if (!((US == color::BLACK ? RANK3_5BB : RANK5_7BB) & setMaskBB(from)))
                        continue;

                    bb_attacks = knightAttack(US, from) & ~goldAttack(Them, ksq);
                    break;

                case piece::GI:
                    bb_attacks = silverAttack(US, from);
                    // 王手にならない升のみ列挙したいが銀は成り/不成りが選択できるので、まあこれはいいや..
                    break;

                case piece::TO:
                case piece::NY:
                case piece::NK:
                case piece::NG:
                case piece::KI:

                    bb_attacks = goldAttack(US, from) & ~goldAttack(Them, ksq);
                    pt = piece::KI;
                    break;

                case piece::KA:
                    bb_attacks = bishopAttack(from, occupiedBB());
                    break;

                case piece::UM:
                    bb_attacks = horseAttack(from, occupiedBB()) & ~kingAttack(ksq);
                    // 16近傍に(王手になってしまうので)
                    break;

                case piece::HI:
                    bb_attacks = rookAttack(from, occupiedBB());
                    break;

                case piece::RY:
                    bb_attacks = dragonAttack(from, occupiedBB()) & ~kingAttack(ksq);
                    break;

                    // 玉が敵玉24近傍にいたということを意味している。
                    // この移動による詰みはめったに出てこないから無視していいや。
                case piece::OU:
                    continue;
                }

                // この駒を玉の16近傍へ移動させる指し手を考える。
                // この処理ループの一回目だけでいいのだが…しかしループはたぶん1回で終了であることがほとんどだから
                // これはこれでいいか..
                // Bitboard target = around24_bb(ksq) & ~kingAttack(ksq);

                // 移動先
                Bitboard bb2 = bb_attacks & atk;

                const Bitboard slide = occupiedBB() ^ setMaskBB(from);
                while (bb2) {
                    const Square to = bb2.firstOneFromSQ11();

                    // 開き王手になっていない。
                    if (isAligned<true>(from, to, ksq))
                        continue;

                    // 合法手か？
                    if (isPinnedIllegal(from, to, kingSquare(US), pinned))
                        continue;

                    // fromに歩が打てない
                    if (canPawnDrop<~US>(from))
                        continue;

                    // ただし、toが歩のcaptureだとfromに歩が打ててしまう可能性があるのでskip。
                    // 盤面最上段だとアレだが、まあ除外していいだろう。
                    const bool capPawn = piece::to_piece_type(piece(to)) == piece::FU;
                    if (capPawn && makeFile(from) == makeFile(to))
                        continue;

                    const Bitboard new_slide = slide | setMaskBB(to);

                    const Bitboard new_pinned = pinnedPieces(Them, from, to);

                    // fromの地点に駒が利いていないか。
                    // fromからtoに移動したときに、toの影になってfromの地点に利いていない場合を考慮しこう書く。
                    // toの地点に駒がいくのでpinnedが変わるのか…。うわ、なんだこれ..
                    if (can_piece_capture(*this, Them, from, to, new_pinned, new_slide))
                        continue;

                    // 玉の8近傍だとcapられる可能性がある。
                    if (kingAttack(ksq) & setMaskBB(to)) {
                        // from以外の駒が利いてない == capられる!!
                        if (!(attackersTo(US, to) ^ setMaskBB(from)))
                            continue;
                    }

                    // ここでの利きを考える。
                    switch (pt) {

                    case piece::GI:
                        // 成り不成りがある。成りは、ここで調べ、不成りはあとで調べる。

                        // 成れないならば不成りの判定へ
                        if (!canPromote(US, from, to))
                            goto DISCOVER_ATTACK_CONTINUE_Silver;
                        bb_attacks = goldAttack(US, to);
                        break;

                    case piece::KE:
                        // 桂は成れるときのみ列挙しているので、移動先では金という扱いで良い。
                    case piece::KI:
                        bb_attacks = goldAttack(US, to);
                        break;

                    case piece::KA:
                        if (canPromote(US, from, to))
                            bb_attacks = horseAttack(to, new_slide);
                        else
                            bb_attacks = bishopAttack(to, new_slide);
                        break;

                    case piece::UM:
                        bb_attacks = horseAttack(to, new_slide);
                        break;

                    case piece::HI:
                        if (canPromote(US, from, to))
                            bb_attacks = dragonAttack(to, new_slide);
                        else
                            bb_attacks = rookAttack(to, new_slide);
                        break;

                    case piece::RY:
                        bb_attacks = dragonAttack(to, new_slide);
                        break;

                    default:
                        assert(false);
                        continue;
                    }

                    // これが王手になっているということは両王手であり、両王手ルーチンで調べたはずだから除外
                    if (bb_attacks & setMaskBB(ksq))
                        goto DISCOVER_ATTACK_CONTINUE_Silver;

                    // 逃げ場所があるのか？
                    // 王手がかかっている局面において逃げ場所を見るときに裏側からのpinnerによる攻撃を考慮して、玉はいないものとして考える必要がある。
                    if (kingAttack(ksq)
                        & ~(bb_attacks | attacksAroundKingInAvoiding<~US>(from, new_slide ^ setMaskBB(ksq)) | bbOf(Them)))
                        goto DISCOVER_ATTACK_CONTINUE_Silver;

                    // ここでは開き王手であることは保証されている。
                    // ksqの隣の升からpinnerとの間に利きがなければこれにて詰みが確定する。

                    // 16近傍への移動なのでs1,s2が近接でないことは保証されているが、ksq側の升から調べないといけないので..


                    // !!!
                    // !!!  以下の部分のコードを修正するときは、その下に銀の不成りのコードがあるのでそちらも修正すること！
                    // !!!

                    {
                        Square s1 = ksq;
                        Square s2 = s1 + (nextSquare(s1, from) - from);
                        do {
                            // s2の地点に玉以外の駒が利いていたらアウト

                            // 駒はfromからtoに移動しているものとする。ゆえにtoの地点に元いた駒の利きは除外して考える必要がある。
                            // それからfromから駒が除去されて遮断が変わったことによる影響はnew_slideによって与えられているものとする。
                            if (can_piece_capture(*this, Them, s2, to, new_pinned, new_slide))
                                goto DISCOVER_ATTACK_CONTINUE_Silver;

                            // s2の地点がfromはなく、かつpinnerであれば、終了
                            // ただしpinnerが取られる可能性があるので、上のcaptureの判定が先にある
                            if (s2 != from && piece(s2)) // 自駒に違いない
                                break;

                            // s2に合駒ができない。
                            if (canPawnDrop<~US>(s2) || (capPawn && makeFile(s2) == makeFile(to)))
                                goto DISCOVER_ATTACK_CONTINUE_Silver;

                            const Square s3 = nextSquare(s1, s2);
                            s1 = s2;
                            s2 = s3;
                        } while (s2 != SquareNum);

                        // これで詰みが確定した
                        // 桂→成りしか調べてないので成れるなら成りで。
                        // 銀→不成と成りと選択できる。
                        if (canPromote(US, from, to) && !(piece(from) & piece::PTPromote) && pt != piece::KI)
                            return makePromoteMove<Capture>(pt, from, to, *this);
                        else
                            return makeCaptureMove(pt, from, to, *this);
                    }

                DISCOVER_ATTACK_CONTINUE_Silver:

                    if (pt == piece::GI) {
                        // 銀不成も考慮しないと..(成りは上で処理されているものとする)
                        // 以下、同様の処理
                        bb_attacks = silverAttack(US, to);
                        if (bb_attacks & setMaskBB(ksq))
                            goto DISCOVER_ATTACK_CONTINUE;

                        if (kingAttack(ksq)
                            & ~(bb_attacks | attacksAroundKingInAvoiding<~US>(from, new_slide ^ setMaskBB(ksq)) | bbOf(Them)))
                            goto DISCOVER_ATTACK_CONTINUE;

                        Square s1 = ksq;
                        Square s2 = s1 + (nextSquare(s1, from) - from);
                        do {
                            if (can_piece_capture(*this, Them, s2, to, new_pinned, new_slide))
                                goto DISCOVER_ATTACK_CONTINUE;
                            if (s2 != from && piece(s2))
                                break;
                            if (canPawnDrop<~US>(s2) || (capPawn && makeFile(s2) == makeFile(to)))
                                goto DISCOVER_ATTACK_CONTINUE;
                            const Square s3 = nextSquare(s1, s2);
                            s1 = s2;
                            s2 = s3;
                        } while (s2 != SquareNum);
                        return makeCaptureMove(piece::GI, from, to, *this);
                    }

                DISCOVER_ATTACK_CONTINUE:;

                }
            }
        }
    }

    return Move::moveNone();
}

template <bool Additional> Move Position::mateMoveIn1Ply() {
    return (turn() == color::BLACK ? mateMoveIn1Ply<color::BLACK, Additional>() : mateMoveIn1Ply<color::WHITE, Additional>());
}

// 明示的インスタンス化
template Move Position::mateMoveIn1Ply<color::BLACK, false>();
template Move Position::mateMoveIn1Ply<color::WHITE, false>();
template Move Position::mateMoveIn1Ply<color::BLACK, true>();
template Move Position::mateMoveIn1Ply<color::WHITE, true>();
template Move Position::mateMoveIn1Ply<false>();
template Move Position::mateMoveIn1Ply<true>();

void Position::initZobrist() {
    // zobTurn_ は 1 であり、その他は 1桁目を使わない。
    // zobTurn のみ xor で更新する為、他の桁に影響しないようにする為。
    // hash値の更新は普通は全て xor を使うが、持ち駒の更新の為に +, - を使用した方が都合が良い。
    for (piece::PieceTypeEnum pt = piece::Occupied; pt < piece::PieceTypeNum; ++pt) {
        for (Square sq = SQ11; sq < SquareNum; ++sq) {
            for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c)
                zobrist_[pt][sq][c] = g_mt64bit.random() & ~UINT64_C(1);
        }
    }
    for (piece::CapturedPieceTypeEnum hp = piece::C_FU; hp < piece::NUM_CAPTURED_PIECE_TYPES; ++hp) {
        zobHand_[hp][color::BLACK] = g_mt64bit.random() & ~UINT64_C(1);
        zobHand_[hp][color::WHITE] = g_mt64bit.random() & ~UINT64_C(1);
    }
}

// ある指し手を指した後のhash keyを返す。
Key Position::getKeyAfter(const Move m) const {
	color::ColorEnum Us = this->turn(); // 現局面の手番
	Key k = getBoardKey() ^ zobTurn();
	Key h = getHandKey();

	// 移動先の升
	Square to = m.to();

	if (m.isDrop())
	{
		// --- 駒打ち
		piece::PieceTypeEnum pt = m.pieceTypeDropped();

		// Zobrist keyの更新
		h -= zobHand(piece::to_captured_piece_type(pt), Us);
		k += zobrist(pt, to, Us);
	}
	else
	{
		// -- 駒の移動
		Square from = m.from();

		// 移動させる駒
		piece::ColoredPieceEnum moved_pc = piece(from);

		// 移動先に駒の配置
		// もし成る指し手であるなら、成った後の駒を配置する。
		piece::ColoredPieceEnum moved_after_pc;

		if (m.isPromotion())
		{
			moved_after_pc = moved_pc + piece::Promoted;
		}
		else {
			moved_after_pc = moved_pc;
		}

		// 移動先の升にある駒
		piece::ColoredPieceEnum to_pc = piece(to);
		if (to_pc != piece::Empty)
		{
			piece::PieceTypeEnum pt = piece::to_piece_type(to_pc);

			// 捕獲された駒が盤上から消えるので局面のhash keyを更新する
			k -= zobrist(pt, to, piece::get_color(to_pc));
			h += zobHand(piece::to_captured_piece_type(pt), Us);
		}

		// fromにあったmoved_pcがtoにmoved_after_pcとして移動した。
		k -= zobrist(piece::to_piece_type(moved_pc), from, Us);
		k += zobrist(piece::to_piece_type(moved_after_pc), to, Us);
	}

	return k + h;
}

// ある指し手を指した後のhash keyを返す。
Key Position::getBoardKeyAfter(const Move m) const {
	color::ColorEnum Us = this->turn(); // 現局面の手番
	Key k = getBoardKey() ^ zobTurn();

	// 移動先の升
	Square to = m.to();

	if (m.isDrop())
	{
		// --- 駒打ち
		piece::PieceTypeEnum pt = m.pieceTypeDropped();

		// Zobrist keyの更新
		k += zobrist(pt, to, Us);
	}
	else
	{
		// -- 駒の移動
		Square from = m.from();

		// 移動させる駒
		piece::ColoredPieceEnum moved_pc = piece(from);

		// 移動先に駒の配置
		// もし成る指し手であるなら、成った後の駒を配置する。
		piece::ColoredPieceEnum moved_after_pc;

		if (m.isPromotion())
		{
			moved_after_pc = moved_pc + piece::Promoted;
		}
		else {
			moved_after_pc = moved_pc;
		}

		// 移動先の升にある駒
		piece::ColoredPieceEnum to_pc = piece(to);
		if (to_pc != piece::Empty)
		{
			piece::PieceTypeEnum pt = piece::to_piece_type(to_pc);

			// 捕獲された駒が盤上から消えるので局面のhash keyを更新する
			k -= zobrist(pt, to, piece::get_color(to_pc));
		}

		// fromにあったmoved_pcがtoにmoved_after_pcとして移動した。
		k -= zobrist(piece::to_piece_type(moved_pc), from, Us);
		k += zobrist(piece::to_piece_type(moved_after_pc), to, Us);
	}

	return k;
}

void Position::print(std::ostream& os) const {
    os << "'  9  8  7  6  5  4  3  2  1\n";
    int i = 0;
    for (Rank r = Rank1; r != Rank9Wall; r += RankDeltaS) {
        ++i;
        os << "P" << i;
        for (File f = File9; f != File1Wall; f += FileDeltaE)
            os << pieceToCharCSA(piece(makeSquare(f, r)));
        os << "\n";
    }
    printHand(os, color::BLACK);
    printHand(os, color::WHITE);
    os << (turn() == color::BLACK ? "+" : "-") << "\n";
}

std::string Position::toSFEN(const Ply ply) const {
    std::stringstream ss;
    int space = 0;
    for (Rank rank = Rank1; rank != Rank9Wall; rank += RankDeltaS) {
        for (File file = File9; file != File1Wall; file += FileDeltaE) {
            const Square sq = makeSquare(file, rank);
            const piece::ColoredPieceEnum pc = piece(sq);
            if (pc == piece::Empty)
                ++space;
            else {
                if (space) {
                    ss << space;
                    space = 0;
                }
                ss << pieceToCharUSI(pc);
            }
        }
        if (space) {
            ss << space;
            space = 0;
        }
        if (rank != Rank9)
            ss << "/";
    }
    ss << (turn() == color::BLACK ? " b " : " w ");
    if (hand(color::BLACK).value() == 0 && hand(color::WHITE).value() == 0)
        ss << "- ";
    else {
        // USI の規格として、持ち駒の表記順は決まっており、先手、後手の順で、それぞれ 飛、角、金、銀、桂、香、歩 の順。
        for (color::ColorEnum color = color::BLACK; color < color::NUM_COLORS; ++color) {
            for (piece::CapturedPieceTypeEnum hp : {piece::C_HI, piece::C_KA, piece::C_KI, piece::C_GI, piece::C_KE, piece::C_KY, piece::C_FU}) {
                const int num = hand(color).numOf(hp);
                if (num == 0)
                    continue;
                if (num != 1)
                    ss << num;
                const piece::ColoredPieceEnum pc = to_colored_piece(color, hp);
                ss << pieceToCharUSI(pc);
            }
        }
        ss << " ";
    }
    ss << ply;
    return ss.str();
}

std::string Position::toCSAPos() const {
    std::stringstream ss;
    int i = 0;
    for (Rank r = Rank1; r != Rank9Wall; r += RankDeltaS) {
        ++i;
        ss << "P" << i;
        for (File f = File9; f != File1Wall; f += FileDeltaE)
            ss << pieceToCharCSA(piece(makeSquare(f, r)));
        ss << "\n";
    }
    printHand(ss, color::BLACK);
    printHand(ss, color::WHITE);
    ss << (turn() == color::BLACK ? "+" : "-") << "\n";
    return ss.str();
}

void Position::toHuffmanCodedPos(u8* data) const {
	std::fill(data, data + 32, 0);
    BitStream bs(data);
    // 手番 (1bit)
    bs.putBit(turn());

    // 玉の位置 (7bit * 2)
    bs.putBits(kingSquare(color::BLACK), 7);
    bs.putBits(kingSquare(color::WHITE), 7);

    // 盤上の駒
    for (Square sq = SQ11; sq < SquareNum; ++sq) {
        piece::ColoredPieceEnum pc = piece(sq);
        if (piece::to_piece_type(pc) == piece::OU)
            continue;
        const auto hc = HuffmanCodedPos::boardCodeTable[pc];
        bs.putBits(hc.code, hc.numOfBits);
    }

    // 持ち駒
    for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c) {
        const Hand h = hand(c);
        for (piece::CapturedPieceTypeEnum hp = piece::C_FU; hp < piece::NUM_CAPTURED_PIECE_TYPES; ++hp) {
            const auto hc = HuffmanCodedPos::handCodeTable[hp][c];
            for (u32 n = 0; n < h.numOf(hp); ++n)
                bs.putBits(hc.code, hc.numOfBits);
        }
    }
    assert(bs.curr() == 0);
}

void Position::toPackedSfen(u8* data) const {
	std::fill(data, data + 32, 0);
	BitStream bs(data);
	// 手番 (1bit)
	bs.putBit(turn());

	// 玉の位置 (7bit * 2)
	bs.putBits(kingSquare(color::BLACK), 7);
	bs.putBits(kingSquare(color::WHITE), 7);

	// 盤上の駒
	for (Square sq = SQ11; sq < SquareNum; ++sq) {
		piece::ColoredPieceEnum pc = piece(sq);
		if (piece::to_piece_type(pc) == piece::OU)
			continue;
		const auto hc = PackedSfen::boardCodeTable[pc];
		bs.putBits(hc.code, hc.numOfBits);
	}

	// 持ち駒
	for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c) {
		const Hand h = hand(c);
		for (piece::CapturedPieceTypeEnum hp = piece::C_FU; hp < piece::NUM_CAPTURED_PIECE_TYPES; ++hp) {
			const auto hc = PackedSfen::handCodeTable[hp][c];
			for (u32 n = 0; n < h.numOf(hp); ++n)
				bs.putBits(hc.code, hc.numOfBits);
		}
	}
	assert(bs.curr() == 0);
}

bool Position::isOK() const {
    static Key prevKey;
    const bool debugAll = true;

    const bool debugBitboards    = debugAll || false;
    const bool debugKingCount    = debugAll || false;
    const bool debugKingCapture  = debugAll || false;
    const bool debugCheckerCount = debugAll || false;
    const bool debugKey          = debugAll || false;
    const bool debugStateHand    = debugAll || false;
    const bool debugPiece        = debugAll || false;

    int failedStep = 0;
    if (debugBitboards) {
        if ((bbOf(color::BLACK) & bbOf(color::WHITE)))
            goto incorrect_position;
        if ((bbOf(color::BLACK) | bbOf(color::WHITE)) != occupiedBB())
            goto incorrect_position;
        if ((bbOf(piece::FU     ) ^ bbOf(piece::KY    ) ^ bbOf(piece::KE) ^ bbOf(piece::GI ) ^ bbOf(piece::KA  ) ^
             bbOf(piece::HI     ) ^ bbOf(piece::KI     ) ^ bbOf(piece::OU  ) ^ bbOf(piece::TO) ^ bbOf(piece::NY) ^
             bbOf(piece::NK) ^ bbOf(piece::NG) ^ bbOf(piece::UM ) ^ bbOf(piece::RY )) != occupiedBB())
        {
            goto incorrect_position;
        }
        for (piece::PieceTypeEnum pt1 = piece::FU; pt1 < piece::PieceTypeNum; ++pt1) {
            for (piece::PieceTypeEnum pt2 = pt1 + 1; pt2 < piece::PieceTypeNum; ++pt2) {
                if ((bbOf(pt1) & bbOf(pt2)))
                    goto incorrect_position;
            }
        }
    }

    ++failedStep;
    if (debugKingCount) {
        int kingCount[color::NUM_COLORS] = {0, 0};
        if (bbOf(piece::OU).popCount() != 2)
            goto incorrect_position;
        if (!bbOf(piece::OU, color::BLACK).isOneBit())
            goto incorrect_position;
        if (!bbOf(piece::OU, color::WHITE).isOneBit())
            goto incorrect_position;
        for (Square sq = SQ11; sq < SquareNum; ++sq) {
            if (piece(sq) == piece::B_OU)
                ++kingCount[color::BLACK];
            if (piece(sq) == piece::W_OU)
                ++kingCount[color::WHITE];
        }
        if (kingCount[color::BLACK] != 1 || kingCount[color::WHITE] != 1)
            goto incorrect_position;
    }

    ++failedStep;
    if (debugKingCapture) {
        // 相手玉を取れないことを確認
        const color::ColorEnum us = turn();
        const color::ColorEnum them = color::opposite(us);
        const Square ksq = kingSquare(them);
        if (attackersTo(us, ksq))
            goto incorrect_position;
    }

    ++failedStep;
    if (debugCheckerCount) {
        if (2 < st_->checkersBB.popCount())
            goto incorrect_position;
    }

    ++failedStep;
    if (debugKey) {
        if (getKey() != computeKey())
            goto incorrect_position;
    }

    ++failedStep;
    if (debugStateHand) {
        if (st_->hand != hand(turn()))
            goto incorrect_position;
    }

    ++failedStep;
    if (debugPiece) {
        for (Square sq = SQ11; sq < SquareNum; ++sq) {
            const piece::ColoredPieceEnum pc = piece(sq);
            if (pc == piece::Empty) {
                if (!emptyBB().isSet(sq))
                    goto incorrect_position;
            }
            else {
                if (!bbOf(piece::to_piece_type(pc), piece::get_color(pc)).isSet(sq))
                    goto incorrect_position;
            }
        }
    }

    prevKey = getKey();
    return true;

incorrect_position:
    std::cout << "Error! failedStep = " << failedStep << std::endl;
    std::cout << "prevKey = " << prevKey << std::endl;
    std::cout << "currKey = " << getKey() << std::endl;
    print(std::cout);
    return false;
}

Key Position::computeBoardKey() const {
    Key result = 0;
    for (Square sq = SQ11; sq < SquareNum; ++sq) {
        if (piece(sq) != piece::Empty)
            result += zobrist(piece::to_piece_type(piece(sq)), sq, piece::get_color(piece(sq)));
    }
    if (turn() == color::WHITE)
        result ^= zobTurn();
    return result;
}

Key Position::computeHandKey() const {
    Key result = 0;
    for (piece::CapturedPieceTypeEnum hp = piece::C_FU; hp < piece::NUM_CAPTURED_PIECE_TYPES; ++hp) {
        for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c) {
            const int num = hand(c).numOf(hp);
            for (int i = 0; i < num; ++i)
                result += zobHand(hp, c);
        }
    }
    return result;
}

// todo: isRepetition() に名前変えた方が良さそう。
//       同一局面4回をきちんと数えていないけど問題ないか。
RepetitionType Position::isDraw(const int checkMaxPly) const {
    const int Start = 4;
    int i = Start;
    const int e = std::min(st_->pliesFromNull, checkMaxPly);

    // 4手掛けないと千日手には絶対にならない。
    if (i <= e) {
        // 現在の局面と、少なくとも 4 手戻らないと同じ局面にならない。
        // ここでまず 2 手戻る。
        StateInfo* stp = st_->previous->previous;

        do {
            // 更に 2 手戻る。
            stp = stp->previous->previous;
            if (stp->key() == st_->key()) {
                if (i <= st_->continuousCheck[turn()])
                    return RepetitionLose;
                else if (i <= st_->continuousCheck[color::opposite(turn())])
                    return RepetitionWin;
#if defined BAN_BLACK_REPETITION
                return (turn() == color::BLACK ? RepetitionLose : RepetitionWin);
#elif defined BAN_WHITE_REPETITION
                return (turn() == White ? RepetitionLose : RepetitionWin);
#else
                return RepetitionDraw;
#endif
            }
            else if (stp->boardKey == st_->boardKey) {
                if (st_->hand.isEqualOrSuperior(stp->hand)) return RepetitionSuperior;
                if (stp->hand.isEqualOrSuperior(st_->hand)) return RepetitionInferior;
            }
            i += 2;
        } while (i <= e);
    }
    return NotRepetition;
}

namespace {
	void printHandPiece(std::ostream& os, const Position& pos, const piece::CapturedPieceTypeEnum hp, const color::ColorEnum c, const std::string& str) {
        if (pos.hand(c).numOf(hp)) {
            const char* sign = (c == color::BLACK ? "+" : "-");
            os << "P" << sign;
            for (u32 i = 0; i < pos.hand(c).numOf(hp); ++i)
                os << "00" << str;
            os << "\n";
        }
    }
}
void Position::printHand(std::ostream& os, const color::ColorEnum c) const {
    printHandPiece(os, *this, piece::C_FU  , c, "FU");
    printHandPiece(os, *this, piece::C_KY , c, "KY");
    printHandPiece(os, *this, piece::C_KE, c, "piece::KE");
    printHandPiece(os, *this, piece::C_GI, c, "GI");
    printHandPiece(os, *this, piece::C_KI  , c, "KI");
    printHandPiece(os, *this, piece::C_KA, c, "KA");
    printHandPiece(os, *this, piece::C_HI  , c, "HI");
}

Position& Position::operator = (const Position& pos) {
    memcpy(this, &pos, sizeof(Position));
    startState_ = *st_;
    st_ = &startState_;

    assert(isOK());
    return *this;
}

void Position::set(const std::string& sfen) {
    piece::ColoredPieceEnum promoteFlag = piece::UnPromoted;
    std::istringstream ss(sfen);
    char token;
    Square sq = SQ91;

    clear();

    // 盤上の駒
    while (ss.get(token) && token != ' ') {
        if (isdigit(token))
            sq += DeltaE * (token - '0');
        else if (token == '/')
            sq += (DeltaW * 9) + DeltaS;
        else if (token == '+')
            promoteFlag = piece::Promoted;
        else if (g_charToPieceUSI.isLegalChar(token)) {
            if (isInSquare(sq)) {
                setPiece(g_charToPieceUSI.value(token) + promoteFlag, sq);
                promoteFlag = piece::UnPromoted;
                sq += DeltaE;
            }
            else
                goto INCORRECT;
        }
        else
            goto INCORRECT;
    }
    kingSquare_[color::BLACK] = bbOf(piece::OU, color::BLACK).constFirstOneFromSQ11();
    kingSquare_[color::WHITE] = bbOf(piece::OU, color::WHITE).constFirstOneFromSQ11();
    goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

    // 手番
    while (ss.get(token) && token != ' ') {
        if (token == 'b')
            turn_ = color::BLACK;
        else if (token == 'w')
            turn_ = color::WHITE;
        else
            goto INCORRECT;
    }

    // 持ち駒
    for (int digits = 0; ss.get(token) && token != ' '; ) {
        if (token == '-')
            memset(hand_, 0, sizeof(hand_));
        else if (isdigit(token))
            digits = digits * 10 + token - '0';
        else if (g_charToPieceUSI.isLegalChar(token)) {
            // 持ち駒を32bit に pack する
            const piece::ColoredPieceEnum piece = g_charToPieceUSI.value(token);
            setHand(piece, (digits == 0 ? 1 : digits));

            digits = 0;
        }
        else
            goto INCORRECT;
    }

    // 次の手が何手目か
    ss >> gamePly_;

    // 残り時間, hash key, (もし実装するなら)駒番号などをここで設定
    st_->boardKey = computeBoardKey();
    st_->handKey = computeHandKey();
    st_->hand = hand(turn());

    findCheckers();

    return;
INCORRECT:
    std::cout << "incorrect SFEN string : " << sfen << std::endl;
}

void Position::set(const piece::ColoredPieceEnum pieces[SquareNum], const int pieces_in_hand[color::NUM_COLORS][piece::NUM_CAPTURED_PIECE_TYPES]) {
    const auto turn = turn_;
    const auto ply = gamePly_;
    clear();

    // 盤上の駒
    for (Square sq = SQ11; sq < SquareNum; ++sq) {
        if (pieces[sq] != piece::Empty)
            setPiece(pieces[sq], sq);
    }
    // 持ち駒
    for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c) {
        for (piece::CapturedPieceTypeEnum hp = piece::C_FU; hp < piece::NUM_CAPTURED_PIECE_TYPES; ++hp) {
            setHand(hp, c, pieces_in_hand[c][hp]);
        }
    }
    // 手番
    turn_ = turn;

    kingSquare_[color::BLACK] = bbOf(piece::OU, color::BLACK).constFirstOneFromSQ11();
    kingSquare_[color::WHITE] = bbOf(piece::OU, color::WHITE).constFirstOneFromSQ11();
    goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

    gamePly_ = ply;

    st_->boardKey = computeBoardKey();
    st_->handKey = computeHandKey();
    st_->hand = hand(turn_);

    findCheckers();
}

bool Position::set_hcp(const char* hcp_data) {
    clear();

    HuffmanCodedPos tmp(hcp_data); // ローカルにコピー
    BitStream bs(tmp.data);

    // 手番
    turn_ = static_cast<color::ColorEnum>(bs.getBit());

    // 玉の位置
    Square sq0 = (Square)bs.getBits(7);
    Square sq1 = (Square)bs.getBits(7);
    setPiece(piece::B_OU, static_cast<Square>(sq0));
    setPiece(piece::W_OU, static_cast<Square>(sq1));

    // 盤上の駒
    for (Square sq = SQ11; sq < SquareNum; ++sq) {
        if (piece::to_piece_type(piece(sq)) == piece::OU) // piece(sq) は piece::B_OU, piece::W_OU, piece::Empty のどれか。
            continue;
        HuffmanCode hc = {0, 0};
        while (hc.numOfBits <= 8) {
            hc.code |= bs.getBit() << hc.numOfBits++;
            if (HuffmanCodedPos::boardCodeToPieceHash.value(hc.key) != piece::PieceNone) {
                const piece::ColoredPieceEnum pc = HuffmanCodedPos::boardCodeToPieceHash.value(hc.key);
                if (pc != piece::Empty)
                    setPiece(HuffmanCodedPos::boardCodeToPieceHash.value(hc.key), sq);
                break;
            }
        }
        if (HuffmanCodedPos::boardCodeToPieceHash.value(hc.key) == piece::PieceNone)
            goto INCORRECT_HUFFMAN_CODE;
    }
    while (bs.data() != std::end(tmp.data)) {
        HuffmanCode hc = {0, 0};
        while (hc.numOfBits <= 8) {
            hc.code |= bs.getBit() << hc.numOfBits++;
            const piece::ColoredPieceEnum pc = HuffmanCodedPos::handCodeToPieceHash.value(hc.key);
            if (pc != piece::PieceNone) {
                hand_[piece::get_color(pc)].plusOne(piece::to_captured_piece_type(piece::to_piece_type(pc)));
                break;
            }
        }
        if (HuffmanCodedPos::handCodeToPieceHash.value(hc.key) == piece::PieceNone)
            goto INCORRECT_HUFFMAN_CODE;
    }

    kingSquare_[color::BLACK] = bbOf(piece::OU, color::BLACK).constFirstOneFromSQ11();
    kingSquare_[color::WHITE] = bbOf(piece::OU, color::WHITE).constFirstOneFromSQ11();
    goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

    gamePly_ = 1; // ply の情報は持っていないので 1 にしておく。

    st_->boardKey = computeBoardKey();
    st_->handKey = computeHandKey();
    st_->hand = hand(turn());

    findCheckers();

    return true;
INCORRECT_HUFFMAN_CODE:
    std::cout << "incorrect Huffman code." << std::endl;
    return false;
}

// やねうら王のpacked sfenから読み込む
bool Position::set_psfen(const char* psfen_data) {
	clear();

	PackedSfen tmp(psfen_data); // ローカルにコピー
	BitStream bs(tmp.data);

	// 手番
	turn_ = static_cast<color::ColorEnum>(bs.getBit());

	// 玉の位置
	Square sq0 = (Square)bs.getBits(7);
	Square sq1 = (Square)bs.getBits(7);
	setPiece(piece::B_OU, static_cast<Square>(sq0));
	setPiece(piece::W_OU, static_cast<Square>(sq1));

	// 盤上の駒
	for (Square sq = SQ11; sq < SquareNum; ++sq) {
		if (piece::to_piece_type(piece(sq)) == piece::OU) // piece(sq) は piece::B_OU, piece::W_OU, piece::Empty のどれか。
			continue;
		HuffmanCode hc = { 0, 0 };
		while (hc.numOfBits <= 8) {
			hc.code |= bs.getBit() << hc.numOfBits++;
			if (PackedSfen::boardCodeToPieceHash.value(hc.key) != piece::PieceNone) {
				const piece::ColoredPieceEnum pc = PackedSfen::boardCodeToPieceHash.value(hc.key);
				if (pc != piece::Empty)
					setPiece(PackedSfen::boardCodeToPieceHash.value(hc.key), sq);
				break;
			}
		}
		if (PackedSfen::boardCodeToPieceHash.value(hc.key) == piece::PieceNone)
			goto INCORRECT_HUFFMAN_CODE;
	}
	while (bs.data() != std::end(tmp.data)) {
		HuffmanCode hc = { 0, 0 };
		while (hc.numOfBits <= 8) {
			hc.code |= bs.getBit() << hc.numOfBits++;
			const piece::ColoredPieceEnum pc = PackedSfen::handCodeToPieceHash.value(hc.key);
			if (pc != piece::PieceNone) {
				hand_[piece::get_color(pc)].plusOne(piece::to_captured_piece_type(piece::to_piece_type(pc)));
				break;
			}
		}
		if (PackedSfen::handCodeToPieceHash.value(hc.key) == piece::PieceNone)
			goto INCORRECT_HUFFMAN_CODE;
	}

	kingSquare_[color::BLACK] = bbOf(piece::OU, color::BLACK).constFirstOneFromSQ11();
	kingSquare_[color::WHITE] = bbOf(piece::OU, color::WHITE).constFirstOneFromSQ11();
	goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

	gamePly_ = 1; // ply の情報は持っていないので 1 にしておく。

	st_->boardKey = computeBoardKey();
	st_->handKey = computeHandKey();
	st_->hand = hand(turn());

	//setEvalList();
	findCheckers();

	return true;
INCORRECT_HUFFMAN_CODE:
	std::cout << "incorrect Huffman code." << std::endl;
	return false;
}

// ランダムな局面を作成する。
void Position::set(std::mt19937& mt) {
    clear();

    // 手番の設定。
    std::uniform_int_distribution<int> colorDist(0, (int)color::NUM_COLORS - 1);
    turn_ = (color::ColorEnum)colorDist(mt);

    // 先後両方の持ち駒の数を設定する。持ち駒が多くなるほど、確率が低くなるので、取り敢えずこれで良しとする。todo: 確率分布を指定出来るように。
    auto setHandPieces = [&](const piece::CapturedPieceTypeEnum hp, const int maxNum) {
        std::uniform_int_distribution<int> handNumDist(0, maxNum);
        while (true) {
            const int nums[color::NUM_COLORS] = {handNumDist(mt), handNumDist(mt)};
            if (nums[color::BLACK] + nums[color::WHITE] <= maxNum) {
                setHand(hp, color::BLACK, nums[color::BLACK]);
                setHand(hp, color::WHITE, nums[color::WHITE]);
            }
            break;
        }
    };
    setHandPieces(piece::C_FU  , 18);
    setHandPieces(piece::C_KY ,  4);
    setHandPieces(piece::C_KE,  4);
    setHandPieces(piece::C_GI,  4);
    setHandPieces(piece::C_KI  ,  4);
    setHandPieces(piece::C_KA,  2);
    setHandPieces(piece::C_HI  ,  2);

    // 玉の位置の設定。
    std::uniform_int_distribution<int> squareDist(0, (int)SquareNum - 1);
    while (true) {
        const Square ksqs[color::NUM_COLORS] = {(Square)squareDist(mt), (Square)squareDist(mt)};
        // 玉同士が同じ位置、もしくは相手の玉の利きにいる事は絶対にない。その他なら何でも良い。
        if (ksqs[color::BLACK] == ksqs[color::WHITE])
            continue;
        if (kingAttack(ksqs[color::BLACK]) & setMaskBB(ksqs[color::WHITE]))
            continue;
        // 先手の玉の利きに後手玉がいなければ、後手玉の利きにも先手玉はいない。
        //if (kingAttack(ksqs[color::WHITE]) & setMaskBB(ksqs[color::BLACK]))
        //    continue;
        setPiece(piece::B_OU, ksqs[color::BLACK]);
        setPiece(piece::W_OU, ksqs[color::WHITE]);
        kingSquare_[color::BLACK] = ksqs[color::BLACK];
        kingSquare_[color::WHITE] = ksqs[color::WHITE];
        break;
    }

    // なる為の閾値。これ以上だと成っているとみなす。[0,99]
    // todo: 1段目が選ばれて、成っていなければ、歩香桂はやり直し。分布が偏るが、どういう分布が良いかも分からないのである程度は適当に。
    static const int promoteThresh[color::NUM_COLORS][RankNum] = {{30, 30, 30, 40, 60, 90, 99, 99, 99},
                                                         {99, 99, 99, 90, 60, 40, 30, 30, 30}};

    int checkersNum = 0;
    Square checkSquare = SquareNum; // 1つ目の王手している駒の位置。(1つしか保持する必要が無い。)
    // 飛び利きの無い駒の配置。
    auto shortPiecesSet = [&](const piece::PieceTypeEnum pt, const piece::CapturedPieceTypeEnum hp, const int maxNum) {
        for (int i = 0; i < maxNum - (int)(hand(color::BLACK).numOf(hp) + hand(color::WHITE).numOf(hp)); ++i) {
            while (true) {
                const Square sq = (Square)squareDist(mt);
                // その場所に既に駒があるか。
                if (occupiedBB().isSet(sq))
                    continue;
                const File file = makeFile(sq);
                const Rank rank = makeRank(sq);
                const color::ColorEnum t = (color::ColorEnum)colorDist(mt);
                // 出来るだけ前にいるほど成っている確率を高めに。
                std::uniform_int_distribution<int> promoteDist(0, 99);
                const piece::ColoredPieceEnum promoteFlag = (pt != piece::KI && promoteThresh[t][rank] <= promoteDist(mt) ? piece::Promoted : piece::UnPromoted);
                const piece::ColoredPieceEnum pc = to_colored_piece(t, pt) + promoteFlag;
                if (promoteFlag == piece::UnPromoted) {
                    if (pt == piece::FU) {
                        // 二歩のチェック
                        if (fileMask(file) & bbOf(piece::FU, t))
                            continue;
                        // 行き所が無いかチェック
                        if (t == color::BLACK)
                            if (isInFrontOf<color::BLACK, Rank2, Rank8>(rank))
                                continue;
                        if (t == color::WHITE)
                            if (isInFrontOf<color::WHITE, Rank2, Rank8>(rank))
                                continue;
                    }
                    else if (pt == piece::KE) {
                        // 行き所が無いかチェック
                        if (t == color::BLACK)
                            if (isInFrontOf<color::BLACK, Rank3, Rank7>(rank))
                                continue;
                        if (t == color::WHITE)
                            if (isInFrontOf<color::WHITE, Rank3, Rank7>(rank))
                                continue;
                    }
                }
                if (t == turn()) {
                    // 手番側が王手していてはいけない。
                    if (attacksFrom(piece::to_piece_type(pc), t, sq).isSet(kingSquare(color::opposite(turn()))))
                        continue;
                }
                else {
                    if (attacksFrom(piece::to_piece_type(pc), t, sq).isSet(kingSquare(turn()))) {
                        // 王手の位置。
                        // 飛び利きでない駒のみでは同時に1つの駒しか王手出来ない。
                        if (checkersNum != 0)
                            continue;
                        ++checkersNum;
                        checkSquare = sq;
                    }
                }
                setPiece(pc, sq);
                break;
            }
        }
    };
    shortPiecesSet(piece::FU  , piece::C_FU  , 18);
    shortPiecesSet(piece::KE, piece::C_KE,  4);
    shortPiecesSet(piece::GI, piece::C_GI,  4);
    shortPiecesSet(piece::KI  , piece::C_KI  ,  4);

    // 飛び利きの駒を配置。
    auto longPiecesSet = [&](const piece::PieceTypeEnum pt, const piece::CapturedPieceTypeEnum hp, const int maxNum) {
        for (int i = 0; i < maxNum - (int)(hand(color::BLACK).numOf(hp) + hand(color::WHITE).numOf(hp)); ++i) {
            while (true) {
                const Square sq = (Square)squareDist(mt);
                // その場所に既に駒があるか。
                if (occupiedBB().isSet(sq))
                    continue;
                const File file = makeFile(sq);
                const Rank rank = makeRank(sq);
                const color::ColorEnum t = (color::ColorEnum)colorDist(mt);
                // 出来るだけ前にいるほど成っている確率を高めに。
                std::uniform_int_distribution<int> promoteDist(0, 99);
                const piece::ColoredPieceEnum promoteFlag = [&] {
                    if (pt == piece::KY)
                        return (promoteThresh[t][rank] <= promoteDist(mt) ? piece::Promoted : piece::UnPromoted);
                    else // 飛角に関しては、成りと不成は同確率とする。
                        return (50 <= promoteDist(mt) ? piece::Promoted : piece::UnPromoted);
                }();
                const piece::ColoredPieceEnum pc = to_colored_piece(t, pt) + promoteFlag;
                if (promoteFlag == piece::UnPromoted) {
                    if (pt == piece::KY) {
                        // 行き所が無いかチェック
                        if (t == color::BLACK)
                            if (isInFrontOf<color::BLACK, Rank2, Rank8>(rank))
                                continue;
                        if (t == color::WHITE)
                            if (isInFrontOf<color::WHITE, Rank2, Rank8>(rank))
                                continue;
                    }
                }
                // 手番側が王手していないか。
                if (t == turn()) {
                    if (attacksFrom(piece::to_piece_type(pc), t, sq).isSet(kingSquare(color::opposite(turn()))))
                        continue;
                }
                else {
                    if (attacksFrom(piece::to_piece_type(pc), t, sq).isSet(kingSquare(turn()))) {
                        // 王手の位置。
                        // 飛び利きを含めて同時に王手する駒は2つまで。既に2つあるならそれ以上王手出来ない。
                        if (checkersNum >= 2)
                            continue;
                        if (checkersNum == 1) {
                            // 両王手。両王手は少なくとも1つは遠隔王手である必要がある。
                            // この駒は近接王手か。
                            if (kingAttack(kingSquare(turn())) & setMaskBB(sq)) {
                                // もう1つの王手している駒も近接王手か。
                                if (kingAttack(kingSquare(turn())) & setMaskBB(checkSquare))
                                    continue; // 両方が近接王ではあり得ない。
                                // もう1つの王手している駒が遠隔王手。
                                // この駒の位置を戻すともう1つの王手を隠せる必要がある。
                                if (!(attacksFrom(piece::to_piece_type(piece(sq)), color::opposite(t), sq) & betweenBB(kingSquare(turn()), checkSquare))) {
                                    // 王手を隠せなかったので、この駒が成り駒であれば、成る前の動きでチェック。
                                    if (!(piece(sq) & piece::Promoted))
                                        continue; // 成り駒ではなかった。
                                    const Bitboard hiddenBB = attacksFrom(piece::to_piece_type(piece(sq)) - piece::Promoted, color::opposite(t), sq) & betweenBB(kingSquare(turn()), checkSquare);
                                    if (!hiddenBB)
                                        continue;
                                    // 成る前の利きならもう一方の王手を隠せた。
                                    // 後は、この駒が成っていない状態から、sq に移動して成れたか。
                                    if (!canPromote(t, makeRank(sq)) && !(hiddenBB & (t == color::BLACK ? inFrontMask<color::BLACK, Rank4>() : inFrontMask<color::WHITE, Rank6>())))
                                        continue; // from, to 共に敵陣ではないので、成る事が出来ない。
                                }
                            }
                            else {
                                // この駒は遠隔王手。
                                if (!(attacksFrom(piece::to_piece_type(piece(checkSquare)), color::opposite(t), checkSquare) & betweenBB(kingSquare(turn()), sq))) {
                                    // この駒の王手を隠せなかった。
                                    if (!(piece(checkSquare) & piece::Promoted))
                                        continue; // もう一方の王手している駒が成り駒ではなかった。
                                    const Bitboard hiddenBB = attacksFrom(piece::to_piece_type(piece(checkSquare)) - piece::Promoted, color::opposite(t), checkSquare) & betweenBB(kingSquare(turn()), sq);
                                    if (!hiddenBB)
                                        continue;
                                    // 成る前の利きならこの駒の王手を隠せた。
                                    // 後は、もう一方の王手している駒が成っていない状態から、checkSquare に移動して成れたか。
                                    if (!canPromote(t, makeRank(checkSquare)) && !(hiddenBB & (t == color::BLACK ? inFrontMask<color::BLACK, Rank4>() : inFrontMask<color::WHITE, Rank6>())))
                                        continue; // from, to 共に敵陣ではないので、成る事が出来ない。
                                }
                            }
                            // 両王手でこの2つの駒が王手しているのはあり得る。
                            // ただし、これ以上厳密には判定しないが、他の駒を盤上に置く事で、この両王手が成り立たなくなる可能性はある。
                        }
                        else // 最初の王手の駒なので、位置を保持する。
                            checkSquare = sq;
                        ++checkersNum;
                    }
                }
                setPiece(pc, sq);
                break;
            }
        }
    };
    longPiecesSet(piece::KY , piece::C_KY , 4);
    longPiecesSet(piece::KA, piece::C_KA, 2);
    longPiecesSet(piece::HI  , piece::C_HI  , 2);

    goldsBB_ = bbOf(piece::KI, piece::TO, piece::NY, piece::NK, piece::NG);

    gamePly_ = 1; // ply の情報は持っていないので 1 にしておく。

    st_->boardKey = computeBoardKey();
    st_->handKey = computeHandKey();
    st_->hand = hand(turn());

    findCheckers();
}

bool Position::moveGivesCheck(const Move move) const
{
    return moveGivesCheck(move, CheckInfo(*this));
}

// move が王手なら true
bool Position::moveGivesCheck(const Move move, const CheckInfo& ci) const
{
    assert(isOK());
    assert(ci.dcBB == discoveredCheckBB());

    const Square to = move.to();
    if (move.isDrop()) {
        const piece::PieceTypeEnum ptTo = move.pieceTypeDropped();
        // Direct Check ?
        if (ci.checkBB[ptTo].isSet(to))
            return true;
    } else {
        const Square from = move.from();
        const piece::PieceTypeEnum ptFrom = move.pieceTypeFrom();
        const piece::PieceTypeEnum ptTo = move.pieceTypeTo(ptFrom);
        assert(ptFrom == piece::to_piece_type(piece(from)));
        // Direct Check ?
        if (ci.checkBB[ptTo].isSet(to))
            return true;

        // Discovery Check ?
        if (isDiscoveredCheck(
                from, to, kingSquare(color::opposite(turn())), ci.dcBB))
            return true;
    }

    return false;
}

piece::ColoredPieceEnum Position::movedPiece(const Move m) const
{
    return to_colored_piece(turn(), m.pieceTypeFromOrDropped());
}

void Position::clear()
{
    memset(this, 0, sizeof(Position));
    st_ = &startState_;
}

// 先手、後手に関わらず、sq へ移動可能な Bitboard を返す。
Bitboard Position::attackersTo(const Square sq, const Bitboard& occupied) const {
    const Bitboard golds = goldsBB();
    return (((attacksFrom<piece::FU  >(color::BLACK, sq          ) & bbOf(piece::FU  ))
             | (attacksFrom<piece::KY >(color::BLACK, sq, occupied) & bbOf(piece::KY ))
             | (attacksFrom<piece::KE>(color::BLACK, sq          ) & bbOf(piece::KE))
             | (attacksFrom<piece::GI>(color::BLACK, sq          ) & bbOf(piece::GI))
             | (attacksFrom<piece::KI  >(color::BLACK, sq          ) & golds       ))
            & bbOf(color::WHITE))
        | (((attacksFrom<piece::FU  >(color::WHITE, sq          ) & bbOf(piece::FU  ))
            | (attacksFrom<piece::KY >(color::WHITE, sq, occupied) & bbOf(piece::KY ))
            | (attacksFrom<piece::KE>(color::WHITE, sq          ) & bbOf(piece::KE))
            | (attacksFrom<piece::GI>(color::WHITE, sq          ) & bbOf(piece::GI))
            | (attacksFrom<piece::KI  >(color::WHITE, sq          ) & golds))
           & bbOf(color::BLACK))
        | (attacksFrom<piece::KA>(sq, occupied) & bbOf(piece::KA, piece::UM        ))
        | (attacksFrom<piece::HI  >(sq, occupied) & bbOf(piece::HI  , piece::RY       ))
        | (attacksFrom<piece::OU  >(sq          ) & bbOf(piece::OU  , piece::UM, piece::RY));
}

// occupied を Position::occupiedBB() 以外のものを使用する場合に使用する。
Bitboard Position::attackersTo(const color::ColorEnum c, const Square sq, const Bitboard& occupied) const {
    const color::ColorEnum opposite = color::opposite(c);
    return ((attacksFrom<piece::FU  >(opposite, sq          ) & bbOf(piece::FU  ))
            | (attacksFrom<piece::KY >(opposite, sq, occupied) & bbOf(piece::KY ))
            | (attacksFrom<piece::KE>(opposite, sq          ) & bbOf(piece::KE))
            | (attacksFrom<piece::GI>(opposite, sq          ) & bbOf(piece::GI, piece::OU, piece::RY))
            | (attacksFrom<piece::KI  >(opposite, sq          ) & (bbOf(piece::OU  , piece::UM) | goldsBB()))
            | (attacksFrom<piece::KA>(          sq, occupied) & bbOf(piece::KA, piece::UM        ))
            | (attacksFrom<piece::HI  >(          sq, occupied) & bbOf(piece::HI  , piece::RY       )))
        & bbOf(c);
}

// 玉以外で sq へ移動可能な c 側の駒の Bitboard を返す。
Bitboard Position::attackersToExceptKing(const color::ColorEnum c, const Square sq) const
{
    const color::ColorEnum opposite = color::opposite(c);
    return ((attacksFrom<piece::FU  >(opposite, sq) & bbOf(piece::FU  ))
            | (attacksFrom<piece::KY >(opposite, sq) & bbOf(piece::KY ))
            | (attacksFrom<piece::KE>(opposite, sq) & bbOf(piece::KE))
            | (attacksFrom<piece::GI>(opposite, sq) & bbOf(piece::GI, piece::RY))
            | (attacksFrom<piece::KI  >(opposite, sq) & (goldsBB() | bbOf(piece::UM)))
            | (attacksFrom<piece::KA>(          sq) & bbOf(piece::KA, piece::UM ))
            | (attacksFrom<piece::HI  >(          sq) & bbOf(piece::HI  , piece::RY)))
        & bbOf(c);
}
