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

#ifndef APERY_POSITION_HPP
#define APERY_POSITION_HPP

#include <memory>
#include <stack>

#include "bitboard.hpp"
#include "common.hpp"
#include "hand.hpp"
#include "piece.hpp"

using Ply = int;

class Position;
enum EvalIndex : int32_t;

enum GameResult : int8_t
{
    Draw,
    BlackWin,
    WhiteWin,
    GameResultNum
};

enum RepetitionType
{
    NotRepetition,
    RepetitionDraw,
    RepetitionWin,
    RepetitionLose,
    RepetitionSuperior,
    RepetitionInferior
};

struct CheckInfo
{
    explicit CheckInfo(const Position&);
    Bitboard dcBB; // discoverd check candidates bitboard
    Bitboard pinned;
    Bitboard checkBB[piece::PieceTypeNum];
};

struct StateInfo
{
    // Copied when making a move
    int pliesFromNull;
    int continuousCheck[color::NUM_COLORS]; // Stockfish には無い。

    // Not copied when making a move (will be recomputed anyhow)
    Key boardKey;
    Key handKey;
    Bitboard checkersBB; // 手番側の玉へ check している駒の Bitboard
#if 0
    piece::ColoredPieceEnum capturedPiece;
#endif
    StateInfo* previous;
    Hand hand; // 手番側の持ち駒

    Key key() const
    {
        return boardKey + handKey;
    }
};

using StateListPtr = std::unique_ptr<std::deque<StateInfo>>;

class BitStream
{
public:
    // 読み込む先頭データのポインタをセットする。
    BitStream(u8* d) : data_(d), curr_()
    {
    }
    // 読み込む先頭データのポインタをセットする。
    void set(u8* d)
    {
        data_ = d;
        curr_ = 0;
    }
    // １ bit 読み込む。どこまで読み込んだかを表す bit の位置を 1 個進める。
    u8 getBit()
    {
        const u8 result = (*data_ & (1 << curr_++)) ? 1 : 0;
        if (curr_ == 8) {
            ++data_;
            curr_ = 0;
        }
        return result;
    }
    // numOfBits bit読み込む。どこまで読み込んだかを表す bit の位置を numOfBits 個進める。
    u8 getBits(const int numOfBits)
    {
        assert(numOfBits <= 8);
        u8 result = 0;
        for (int i = 0; i < numOfBits; ++i)
            result |= getBit() << i;
        return result;
    }
    // 1 bit 書き込む。
    void putBit(const u8 bit)
    {
        assert(bit <= 1);
        *data_ |= bit << curr_++;
        if (curr_ == 8) {
            ++data_;
            curr_ = 0;
        }
    }
    // val の値を numOfBits bit 書き込む。8 bit まで。
    void putBits(u8 val, const int numOfBits)
    {
        assert(numOfBits <= 8);
        for (int i = 0; i < numOfBits; ++i) {
            const u8 bit = val & 1;
            val >>= 1;
            putBit(bit);
        }
    }
    u8* data() const
    {
        return data_;
    }
    int curr() const
    {
        return curr_;
    }

private:
    u8* data_;
    int curr_; // 1byte 中の bit の位置
};

union HuffmanCode
{
    struct
    {
        u8 code; // 符号化時の bit 列
        u8 numOfBits; // 使用 bit 数
    };
    u16 key; // std::unordered_map の key として使う。
};

struct HuffmanCodeToPieceHash
    : public std::unordered_map<u16, piece::ColoredPieceEnum>
{
    piece::ColoredPieceEnum value(const u16 key) const
    {
        const auto it = find(key);
        if (it == std::end(*this))
            return piece::PieceNone;
        return it->second;
    }
};

// Huffman 符号化された局面のデータ構造。256 bit で局面を表す。
struct HuffmanCodedPos
{
    static const HuffmanCode boardCodeTable[piece::PieceNone];
    static const HuffmanCode handCodeTable[piece::NUM_CAPTURED_PIECE_TYPES]
                                          [color::NUM_COLORS];
    static HuffmanCodeToPieceHash boardCodeToPieceHash;
    static HuffmanCodeToPieceHash handCodeToPieceHash;
    static void init()
    {
        for (piece::ColoredPieceEnum pc = piece::VOID; pc <= piece::B_RY; ++pc)
            if (piece::to_piece_type(pc)
                != piece::
                    OU) // 玉は位置で符号化するので、駒の種類では符号化しない。
                boardCodeToPieceHash[boardCodeTable[pc].key] = pc;
        for (piece::ColoredPieceEnum pc = piece::W_FU; pc <= piece::W_RY; ++pc)
            if (piece::to_piece_type(pc)
                != piece::
                    OU) // 玉は位置で符号化するので、駒の種類では符号化しない。
                boardCodeToPieceHash[boardCodeTable[pc].key] = pc;
        for (piece::CapturedPieceTypeEnum hp = piece::C_FU;
             hp < piece::NUM_CAPTURED_PIECE_TYPES;
             ++hp)
            for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c)
                handCodeToPieceHash[handCodeTable[hp][c].key]
                    = to_colored_piece(c, piece::to_piece_type(hp));
    }
    HuffmanCodedPos()
    {
    }
    HuffmanCodedPos(const HuffmanCodedPos&) = default;
    HuffmanCodedPos(const char* hcp_data)
    {
        std::memcpy(data, hcp_data, sizeof(data));
    }
    void clear()
    {
        std::fill(std::begin(data), std::end(data), 0);
    }

    u8 data[32];
};
static_assert(sizeof(HuffmanCodedPos) == 32, "");

struct HuffmanCodedPosAndEval
{
    HuffmanCodedPos hcp;
    s16 eval;
    u16 bestMove16; // 使うかは分からないが教師データ生成時についでに取得しておく。
    GameResult gameResult; // 自己対局で勝ったかどうか。
};
static_assert(sizeof(HuffmanCodedPosAndEval) == 38, "");

// やねうら王のpacked sfen
struct PackedSfen
{
    static const HuffmanCode boardCodeTable[piece::PieceNone];
    static const HuffmanCode handCodeTable[piece::NUM_CAPTURED_PIECE_TYPES]
                                          [color::NUM_COLORS];
    static HuffmanCodeToPieceHash boardCodeToPieceHash;
    static HuffmanCodeToPieceHash handCodeToPieceHash;
    static void init()
    {
        for (piece::ColoredPieceEnum pc = piece::VOID; pc <= piece::B_RY; ++pc)
            if (piece::to_piece_type(pc)
                != piece::
                    OU) // 玉は位置で符号化するので、駒の種類では符号化しない。
                boardCodeToPieceHash[boardCodeTable[pc].key] = pc;
        for (piece::ColoredPieceEnum pc = piece::W_FU; pc <= piece::W_RY; ++pc)
            if (piece::to_piece_type(pc)
                != piece::
                    OU) // 玉は位置で符号化するので、駒の種類では符号化しない。
                boardCodeToPieceHash[boardCodeTable[pc].key] = pc;
        for (piece::CapturedPieceTypeEnum hp = piece::C_FU;
             hp < piece::NUM_CAPTURED_PIECE_TYPES;
             ++hp)
            for (color::ColorEnum c = color::BLACK; c < color::NUM_COLORS; ++c)
                handCodeToPieceHash[handCodeTable[hp][c].key]
                    = to_colored_piece(c, piece::to_piece_type(hp));
    }
    PackedSfen()
    {
    }
    PackedSfen(const PackedSfen&) = default;
    PackedSfen(const char* psfen_data)
    {
        std::memcpy(data, psfen_data, sizeof(data));
    }
    void clear()
    {
        std::fill(std::begin(data), std::end(data), 0);
    }

    u8 data[32];
};

// PackedSfenと評価値が一体化した構造体
// オプションごとに書き出す内容が異なると教師棋譜を再利用するときに困るので
// とりあえず、以下のメンバーはオプションによらずすべて書き出しておく。
struct PackedSfenValue
{
    // 局面
    PackedSfen sfen;

    // Learner::search()から返ってきた評価値
    s16 score;

    // PVの初手
    u16 move;

    // 初期局面からの局面の手数。
    u16 gamePly;

    // この局面の手番側が、ゲームを最終的に勝っているなら1。負けているなら-1。
    // 引き分けに至った場合は、0。
    // 引き分けは、教師局面生成コマンドgensfenにおいて、
    // LEARN_GENSFEN_DRAW_RESULTが有効なときにだけ書き出す。
    s8 game_result;

    // 教師局面を書き出したファイルを他の人とやりとりするときに
    // この構造体サイズが不定だと困るため、paddingしてどの環境でも必ず40bytesになるようにしておく。
    u8 padding;

    // 32 + 2 + 2 + 2 + 1 + 1 = 40bytes
};

void initMate1Ply();

class Move;

class Position
{
public:
    Position()
    {
    }
    Position(const Position& pos)
    {
        *this = pos;
    }
    Position(const std::string& sfen)
    {
        set(sfen);
    }

    Position& operator=(const Position& pos);
    void set(const std::string& sfen);
    void
    set(const piece::ColoredPieceEnum pieces[SquareNum],
        const int pieces_in_hand[color::NUM_COLORS]
                                [piece::NUM_CAPTURED_PIECE_TYPES]);
    bool set_hcp(const char* hcp_data); // for python
    bool set(const HuffmanCodedPos& hcp)
    {
        return set_hcp((const char*)hcp.data);
    };
    bool set_psfen(const char* psfen_data); // for python
    bool set(const PackedSfen& psfen)
    {
        return set_psfen((const char*)psfen.data);
    };
    void set(std::mt19937& mt);

    Bitboard bbOf(const piece::PieceTypeEnum pt) const
    {
        return byTypeBB_[pt];
    }
    Bitboard bbOf(const color::ColorEnum c) const
    {
        return byColorBB_[c];
    }
    Bitboard bbOf(const piece::PieceTypeEnum pt, const color::ColorEnum c) const
    {
        return bbOf(pt) & bbOf(c);
    }
    Bitboard
    bbOf(const piece::PieceTypeEnum pt1, const piece::PieceTypeEnum pt2) const
    {
        return bbOf(pt1) | bbOf(pt2);
    }
    Bitboard bbOf(
        const piece::PieceTypeEnum pt1,
        const piece::PieceTypeEnum pt2,
        const color::ColorEnum c) const
    {
        return bbOf(pt1, pt2) & bbOf(c);
    }
    Bitboard bbOf(
        const piece::PieceTypeEnum pt1,
        const piece::PieceTypeEnum pt2,
        const piece::PieceTypeEnum pt3) const
    {
        return bbOf(pt1, pt2) | bbOf(pt3);
    }
    Bitboard bbOf(
        const piece::PieceTypeEnum pt1,
        const piece::PieceTypeEnum pt2,
        const piece::PieceTypeEnum pt3,
        const color::ColorEnum c) const
    {
        return bbOf(pt1, pt2, pt3) & bbOf(c);
    }
    Bitboard bbOf(
        const piece::PieceTypeEnum pt1,
        const piece::PieceTypeEnum pt2,
        const piece::PieceTypeEnum pt3,
        const piece::PieceTypeEnum pt4) const
    {
        return bbOf(pt1, pt2, pt3) | bbOf(pt4);
    }
    Bitboard bbOf(
        const piece::PieceTypeEnum pt1,
        const piece::PieceTypeEnum pt2,
        const piece::PieceTypeEnum pt3,
        const piece::PieceTypeEnum pt4,
        const piece::PieceTypeEnum pt5) const
    {
        return bbOf(pt1, pt2, pt3, pt4) | bbOf(pt5);
    }
    Bitboard occupiedBB() const
    {
        return bbOf(piece::Occupied);
    }
    // emptyBB() よりもわずかに速いはず。
    // emptyBB() とは異なり、全く使用しない位置(0 から数えて、right の 63bit目、left の 18 ~ 63bit目)
    // の bit が 1 になっても構わないとき、こちらを使う。
    // todo: SSEにビット反転が無いので実はそんなに速くないはず。不要。
    Bitboard nOccupiedBB() const
    {
        return ~occupiedBB();
    }
    Bitboard emptyBB() const
    {
        return occupiedBB() ^ allOneBB();
    }
    // 金、成り金 の Bitboard
    Bitboard goldsBB() const
    {
        return goldsBB_;
    }
    Bitboard goldsBB(const color::ColorEnum c) const
    {
        return goldsBB() & bbOf(c);
    }

    piece::ColoredPieceEnum piece(const Square sq) const
    {
        return piece_[sq];
    }

    // hand
    Hand hand(const color::ColorEnum c) const
    {
        return hand_[c];
    }

    // turn() 側が pin されている Bitboard を返す。
    // checkersBB が更新されている必要がある。
    Bitboard pinnedBB() const
    {
        return hiddenCheckers<true, true>();
    }
    // 間の駒を動かすことで、turn() 側が空き王手が出来る駒のBitboardを返す。
    // checkersBB が更新されている必要はない。
    // BetweenIsUs == true  : 間の駒が自駒。
    // BetweenIsUs == false : 間の駒が敵駒。
    template <bool BetweenIsUs = true>
    Bitboard discoveredCheckBB() const
    {
        return hiddenCheckers<false, BetweenIsUs>();
    }

    // toFile と同じ筋に us の歩がないなら true
    bool noPawns(const color::ColorEnum us, const File toFile) const
    {
        return !bbOf(piece::FU, us).andIsAny(fileMask(toFile));
    }
    bool isPawnDropCheckMate(const color::ColorEnum us, const Square sq) const;
    // Pinされているfromの駒がtoに移動出来なければtrueを返す。
    template <bool IsKnight = false>
    bool isPinnedIllegal(
        const Square from,
        const Square to,
        const Square ksq,
        const Bitboard& pinned) const
    {
        // 桂馬ならどこに動いても駄目。
        return pinned.isSet(from)
               && (IsKnight || !isAligned<true>(from, to, ksq));
    }
    // 空き王手かどうか。
    template <bool IsKnight = false>
    bool isDiscoveredCheck(
        const Square from,
        const Square to,
        const Square ksq,
        const Bitboard& dcBB) const
    {
        // 桂馬ならどこに動いても空き王手になる。
        return dcBB.isSet(from)
               && (IsKnight || !isAligned<true>(from, to, ksq));
    }

    Bitboard checkersBB() const
    {
        return st_->checkersBB;
    }
    Bitboard prevCheckersBB() const
    {
        return st_->previous->checkersBB;
    }
    // 王手が掛かっているか。
    bool inCheck() const
    {
        return checkersBB().isAny();
    }

    FORCE_INLINE Square kingSquare(const color::ColorEnum c) const
    {
        assert(kingSquare_[c] == bbOf(piece::OU, c).constFirstOneFromSQ11());
        return kingSquare_[c];
    }

    bool moveGivesCheck(const Move m) const;
    bool moveGivesCheck(const Move move, const CheckInfo& ci) const;
    piece::ColoredPieceEnum movedPiece(const Move m) const;

    // attacks
    Bitboard attackersTo(const Square sq, const Bitboard& occupied) const;
    Bitboard attackersTo(const color::ColorEnum c, const Square sq) const
    {
        return attackersTo(c, sq, occupiedBB());
    }
    Bitboard attackersTo(
        const color::ColorEnum c,
        const Square sq,
        const Bitboard& occupied) const;
    Bitboard
    attackersToExceptKing(const color::ColorEnum c, const Square sq) const;
    // todo: 利きをデータとして持ったとき、attackersToIsAny() を高速化すること。
    bool attackersToIsAny(const color::ColorEnum c, const Square sq) const
    {
        return attackersTo(c, sq).isAny();
    }
    bool attackersToIsAny(
        const color::ColorEnum c,
        const Square sq,
        const Bitboard& occupied) const
    {
        return attackersTo(c, sq, occupied).isAny();
    }
    // 移動王手が味方の利きに支えられているか。false なら相手玉で取れば詰まない。
    bool unDropCheckIsSupported(const color::ColorEnum c, const Square sq) const
    {
        return attackersTo(c, sq).isAny();
    }
    // 利きの生成

    // 任意の occupied に対する利きを生成する。
    template <piece::PieceTypeEnum PT>
    static Bitboard attacksFrom(
        const color::ColorEnum c, const Square sq, const Bitboard& occupied);
    // 任意の occupied に対する利きを生成する。
    template <piece::PieceTypeEnum PT>
    Bitboard attacksFrom(const Square sq, const Bitboard& occupied) const
    {
        static_assert(
            PT == piece::KA || PT == piece::HI || PT == piece::UM
                || PT == piece::RY,
            "");
        // color::ColorEnum は何でも良い。
        return attacksFrom<PT>(color::NUM_COLORS, sq, occupied);
    }

    template <piece::PieceTypeEnum PT>
    Bitboard attacksFrom(const color::ColorEnum c, const Square sq) const
    {
        static_assert(
            PT == piece::KI, ""); // piece::KI 以外は template 特殊化する。
        return goldAttack(c, sq);
    }
    template <piece::PieceTypeEnum PT>
    Bitboard attacksFrom(const Square sq) const
    {
        static_assert(
            PT == piece::KA || PT == piece::HI || PT == piece::OU
                || PT == piece::UM || PT == piece::RY,
            "");
        // color::ColorEnum は何でも良い。
        return attacksFrom<PT>(color::NUM_COLORS, sq);
    }
    Bitboard attacksFrom(
        const piece::PieceTypeEnum pt,
        const color::ColorEnum c,
        const Square sq) const
    {
        return attacksFrom(pt, c, sq, occupiedBB());
    }
    static Bitboard attacksFrom(
        const piece::PieceTypeEnum pt,
        const color::ColorEnum c,
        const Square sq,
        const Bitboard& occupied);
    Bitboard
    attacksSlider(const color::ColorEnum us, const Bitboard& slide) const;
    Bitboard attacksSlider(
        const color::ColorEnum us,
        const Square avoid_from,
        const Bitboard& occ) const;
    template <color::ColorEnum US>
    Bitboard attacksAroundKingNonSlider() const;
    template <color::ColorEnum US>
    Bitboard attacksAroundKingSlider() const;
    template <color::ColorEnum US>
    Bitboard attacksAroundKingNonSliderInAvoiding(Square avoid_from) const;
    // avoidの駒の利きだけは無視して玉周辺の敵の利きを考えるバージョン。
    // この関数ではわからないため、toの地点から発生する利きはこの関数では感知しない。
    // 王手がかかっている局面において逃げ場所を見るときに裏側からのpinnerによる攻撃を考慮して、玉はいないものとして
    // 考える必要があることに注意せよ。(slide = pos.slide() ^ from ^ king | to) みたいなコードが必要
    // avoidの駒の利きだけは無視して玉周辺の利きを考えるバージョン。
    template <color::ColorEnum US>
    Bitboard
    attacksAroundKingInAvoiding(const Square from, const Bitboard& occ) const
    {
        return attacksAroundKingNonSliderInAvoiding<US>(from)
               | attacksSlider(~US, from, occ);
    }
    // 歩が打てるかの判定用。
    // 歩を持っているかの判定も含む。
    template <color::ColorEnum US>
    bool canPawnDrop(const Square sq) const
    {
        // 歩を持っていて、二歩ではない。
        return hand(US).exists<piece::C_FU>() > 0
               && !(bbOf(piece::FU, US) & fileMask(makeFile(sq)));
    }
    Bitboard pinnedPieces(
        const color::ColorEnum us, const Square from, const Square to) const;

    // 次の手番
    color::ColorEnum turn() const
    {
        return turn_;
    }
    void setTurn(const color::ColorEnum turn)
    {
        turn_ = turn;

        st_->boardKey = computeBoardKey();
        st_->handKey = computeHandKey();
        st_->hand = hand(turn_);

        findCheckers();
    }

    // pseudoLegal とは
    // ・玉が相手駒の利きがある場所に移動する
    // ・pin の駒を移動させる
    // ・連続王手の千日手の手を指す
    // これらの反則手を含めた手の事と定義する。
    // よって、打ち歩詰めや二歩の手は pseudoLegal では無い。
    template <bool MUSTNOTDROP, bool FROMMUSTNOTBEKING>
    bool pseudoLegalMoveIsLegal(const Move move, const Bitboard& pinned) const;
    bool
    pseudoLegalMoveIsEvasion(const Move move, const Bitboard& pinned) const;
    template <bool Searching = true>
    bool moveIsPseudoLegal(const Move move) const;
    bool moveIsLegal(const Move move) const;

    void doMove(const Move move, StateInfo& newSt);
    void doMove(
        const Move move,
        StateInfo& newSt,
        const CheckInfo& ci,
        const bool moveIsCheck);
    void undoMove(const Move move);
    template <bool DO>
    void doNullMove(StateInfo& backUpSt);

    template <color::ColorEnum US, bool Additional>
    Move mateMoveIn1Ply();
    template <bool Additional = true>
    Move mateMoveIn1Ply();

    Ply gamePly() const
    {
        return gamePly_;
    }

    Key getBoardKey() const
    {
        return st_->boardKey;
    }
    Key getHandKey() const
    {
        return st_->handKey;
    }
    Key getKey() const
    {
        return st_->key();
    }
    Key getKeyAfter(const Move m) const;
    Key getBoardKeyAfter(const Move m) const;
    Key getKeyExcludeTurn() const
    {
        static_assert(zobTurn_ == 1, "");
        return getKey() >> 1;
    }
    void print(std::ostream& os) const;
    std::string toSFEN(const Ply ply) const;
    std::string toSFEN() const
    {
        return toSFEN(gamePly());
    }
    std::string toCSAPos() const;
    void toHuffmanCodedPos(u8* data) const;
    void toPackedSfen(u8* data) const;

    RepetitionType
    isDraw(const int checkMaxPly = std::numeric_limits<int>::max()) const;

    void setStartPosPly(const Ply ply)
    {
        gamePly_ = ply;
    }

    // for debug
    bool isOK() const;

    static void initZobrist();

private:
    void clear();
    void setPiece(const piece::ColoredPieceEnum piece, const Square sq)
    {
        const color::ColorEnum c = piece::get_color(piece);
        const piece::PieceTypeEnum pt = piece::to_piece_type(piece);

        piece_[sq] = piece;

        byTypeBB_[pt].setBit(sq);
        byColorBB_[c].setBit(sq);
        byTypeBB_[piece::Occupied].setBit(sq);
    }
    void setHand(
        const piece::CapturedPieceTypeEnum hp,
        const color::ColorEnum c,
        const int num)
    {
        hand_[c].orEqual(num, hp);
    }
    void setHand(const piece::ColoredPieceEnum piece, const int num)
    {
        const color::ColorEnum c = piece::get_color(piece);
        const piece::PieceTypeEnum pt = piece::to_piece_type(piece);
        const piece::CapturedPieceTypeEnum hp
            = piece::to_captured_piece_type(pt);
        setHand(hp, c, num);
    }

    // 手番側の玉へ check している駒を全て探して checkersBB_ にセットする。
    // 最後の手が何か覚えておけば、attackersTo() を使用しなくても良いはずで、処理が軽くなる。
    void findCheckers()
    {
        st_->checkersBB = attackersToExceptKing(
            color::opposite(turn()), kingSquare(turn()));
    }

    void xorBBs(
        const piece::PieceTypeEnum pt,
        const Square sq,
        const color::ColorEnum c);
    // turn() 側が
    // pin されて(して)いる駒の Bitboard を返す。
    // BetweenIsUs == true  : 間の駒が自駒。
    // BetweenIsUs == false : 間の駒が敵駒。
    template <bool FindPinned, bool BetweenIsUs>
    Bitboard hiddenCheckers() const
    {
        Bitboard result = allZeroBB();
        const color::ColorEnum us = turn();
        const color::ColorEnum them = color::opposite(us);
        // pin する遠隔駒
        // まずは自駒か敵駒かで大雑把に判別
        Bitboard pinners = bbOf(FindPinned ? them : us);

        const Square ksq = kingSquare(FindPinned ? us : them);

        // 障害物が無ければ玉に到達出来る駒のBitboardだけ残す。
        pinners &= (bbOf(piece::KY)
                    & lanceAttackToEdge((FindPinned ? us : them), ksq))
                   | (bbOf(piece::HI, piece::RY) & rookAttackToEdge(ksq))
                   | (bbOf(piece::KA, piece::UM) & bishopAttackToEdge(ksq));

        while (pinners) {
            const Square sq = pinners.firstOneFromSQ11();
            // pin する遠隔駒と玉の間にある駒の位置の Bitboard
            const Bitboard between = betweenBB(sq, ksq) & occupiedBB();

            // pin する遠隔駒と玉の間にある駒が1つで、かつ、引数の色のとき、その駒は(を) pin されて(して)いる。
            if (between && between.isOneBit<false>()
                && between.andIsAny(bbOf(BetweenIsUs ? us : them))) {
                result |= between;
            }
        }

        return result;
    }

    Key computeBoardKey() const;
    Key computeHandKey() const;
    Key computeKey() const
    {
        return computeBoardKey() + computeHandKey();
    }

    void printHand(std::ostream& os, const color::ColorEnum c) const;

    static Key zobrist(
        const piece::PieceTypeEnum pt,
        const Square sq,
        const color::ColorEnum c)
    {
        return zobrist_[pt][sq][c];
    }
    static Key zobTurn()
    {
        return zobTurn_;
    }
    static Key
    zobHand(const piece::CapturedPieceTypeEnum hp, const color::ColorEnum c)
    {
        return zobHand_[hp][c];
    }

    // byTypeBB は敵、味方の駒を区別しない。
    // byColorBB は駒の種類を区別しない。
    Bitboard byTypeBB_[piece::PieceTypeNum];
    Bitboard byColorBB_[color::NUM_COLORS];
    Bitboard goldsBB_;

    // 各マスの状態
    piece::ColoredPieceEnum piece_[SquareNum];
    Square kingSquare_[color::NUM_COLORS];

    // 手駒
    Hand hand_[color::NUM_COLORS];
    color::ColorEnum turn_;

    StateInfo startState_;
    StateInfo* st_;
    Ply gamePly_;

    static Key zobrist_[piece::PieceTypeNum][SquareNum][color::NUM_COLORS];
    static const Key zobTurn_ = 1;
    static Key zobHand_[piece::NUM_CAPTURED_PIECE_TYPES][color::NUM_COLORS];
};

template <>
inline Bitboard Position::attacksFrom<piece::KY>(
    const color::ColorEnum c, const Square sq, const Bitboard& occupied)
{
    return lanceAttack(c, sq, occupied);
}
template <>
inline Bitboard Position::attacksFrom<piece::KA>(
    const color::ColorEnum, const Square sq, const Bitboard& occupied)
{
    return bishopAttack(sq, occupied);
}
template <>
inline Bitboard Position::attacksFrom<piece::HI>(
    const color::ColorEnum, const Square sq, const Bitboard& occupied)
{
    return rookAttack(sq, occupied);
}
template <>
inline Bitboard Position::attacksFrom<piece::UM>(
    const color::ColorEnum, const Square sq, const Bitboard& occupied)
{
    return horseAttack(sq, occupied);
}
template <>
inline Bitboard Position::attacksFrom<piece::RY>(
    const color::ColorEnum, const Square sq, const Bitboard& occupied)
{
    return dragonAttack(sq, occupied);
}

template <>
inline Bitboard Position::attacksFrom<piece::FU>(
    const color::ColorEnum c, const Square sq) const
{
    return pawnAttack(c, sq);
}
template <>
inline Bitboard Position::attacksFrom<piece::KY>(
    const color::ColorEnum c, const Square sq) const
{
    return lanceAttack(c, sq, occupiedBB());
}
template <>
inline Bitboard Position::attacksFrom<piece::KE>(
    const color::ColorEnum c, const Square sq) const
{
    return knightAttack(c, sq);
}
template <>
inline Bitboard Position::attacksFrom<piece::GI>(
    const color::ColorEnum c, const Square sq) const
{
    return silverAttack(c, sq);
}
template <>
inline Bitboard
Position::attacksFrom<piece::KA>(const color::ColorEnum, const Square sq) const
{
    return bishopAttack(sq, occupiedBB());
}
template <>
inline Bitboard
Position::attacksFrom<piece::HI>(const color::ColorEnum, const Square sq) const
{
    return rookAttack(sq, occupiedBB());
}
template <>
inline Bitboard
Position::attacksFrom<piece::OU>(const color::ColorEnum, const Square sq) const
{
    return kingAttack(sq);
}
template <>
inline Bitboard
Position::attacksFrom<piece::UM>(const color::ColorEnum, const Square sq) const
{
    return horseAttack(sq, occupiedBB());
}
template <>
inline Bitboard
Position::attacksFrom<piece::RY>(const color::ColorEnum, const Square sq) const
{
    return dragonAttack(sq, occupiedBB());
}

// position sfen R8/2K1S1SSk/4B4/9/9/9/9/9/1L1L1L3 b PLNSGBR17p3n3g 1
// の局面が最大合法手局面で 593 手。番兵の分、+ 1 しておく。
const int MaxLegalMoves = 593 + 1;

class CharToPieceUSI : public std::map<char, piece::ColoredPieceEnum>
{
public:
    CharToPieceUSI()
    {
        (*this)['P'] = piece::B_FU;
        (*this)['p'] = piece::W_FU;
        (*this)['L'] = piece::B_KY;
        (*this)['l'] = piece::W_KY;
        (*this)['N'] = piece::B_KE;
        (*this)['n'] = piece::W_KE;
        (*this)['S'] = piece::B_GI;
        (*this)['s'] = piece::W_GI;
        (*this)['B'] = piece::B_KA;
        (*this)['b'] = piece::W_KA;
        (*this)['R'] = piece::B_HI;
        (*this)['r'] = piece::W_HI;
        (*this)['G'] = piece::B_KI;
        (*this)['g'] = piece::W_KI;
        (*this)['K'] = piece::B_OU;
        (*this)['k'] = piece::W_OU;
    }
    piece::ColoredPieceEnum value(char c) const
    {
        return this->find(c)->second;
    }
    bool isLegalChar(char c) const
    {
        return (this->find(c) != this->end());
    }
};
extern const CharToPieceUSI g_charToPieceUSI;

#endif // #ifndef APERY_POSITION_HPP
