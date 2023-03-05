import numpy as np


dtypeHcp = np.dtype((np.uint8, 32))
dtypeEval = np.dtype(np.int16)
dtypeMove16 = np.dtype(np.int16)
dtypeGameResult = np.dtype(np.int8)

HuffmanCodedPos = np.dtype([
    ('hcp', dtypeHcp),
    ])

HuffmanCodedPosAndEval = np.dtype([
    ('hcp', dtypeHcp),
    ('eval', dtypeEval),
    ('bestMove16', dtypeMove16),
    ('gameResult', dtypeGameResult),
    ('dummy', np.uint8),
    ])

PackedSfen = np.dtype([
    ('sfen', np.uint8, 32),
    ])

PackedSfenValue = np.dtype([
    ('sfen', np.uint8, 32),
    ('score', np.int16),
    ('move', np.uint16),
    ('gamePly', np.uint16),
    ('game_result', np.int8),
    ('padding', np.uint8),
    ])

dtypeKey = np.dtype(np.uint64)
BookEntry = np.dtype([
    ('key', dtypeKey),
    ('fromToPro', dtypeMove16),
    ('count', np.uint16),
    ('score', np.int32),
    ])


STARTING_SFEN = 'lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1'

SQUARES = [
    A1, B1, C1, D1, E1, F1, G1, H1, I1,
    A2, B2, C2, D2, E2, F2, G2, H2, I2,
    A3, B3, C3, D3, E3, F3, G3, H3, I3,
    A4, B4, C4, D4, E4, F4, G4, H4, I4,
    A5, B5, C5, D5, E5, F5, G5, H5, I5,
    A6, B6, C6, D6, E6, F6, G6, H6, I6,
    A7, B7, C7, D7, E7, F7, G7, H7, I7,
    A8, B8, C8, D8, E8, F8, G8, H8, I8,
    A9, B9, C9, D9, E9, F9, G9, H9, I9,
] = range(81)

SQUARE_NAMES = [
    '1a', '1b', '1c', '1d', '1e', '1f', '1g', '1h', '1i',
    '2a', '2b', '2c', '2d', '2e', '2f', '2g', '2h', '2i',
    '3a', '3b', '3c', '3d', '3e', '3f', '3g', '3h', '3i',
    '4a', '4b', '4c', '4d', '4e', '4f', '4g', '4h', '4i',
    '5a', '5b', '5c', '5d', '5e', '5f', '5g', '5h', '5i',
    '6a', '6b', '6c', '6d', '6e', '6f', '6g', '6h', '6i',
    '7a', '7b', '7c', '7d', '7e', '7f', '7g', '7h', '7i',
    '8a', '8b', '8c', '8d', '8e', '8f', '8g', '8h', '8i',
    '9a', '9b', '9c', '9d', '9e', '9f', '9g', '9h', '9i',
]

COLORS = [BLACK, WHITE] = range(2)

GAME_RESULTS = [
    DRAW, BLACK_WIN, WHITE_WIN,
] = range(3)

PIECE_TYPES_WITH_NONE = [NONE,
           PAWN,      LANCE,      KNIGHT,      SILVER,
         BISHOP,       ROOK,
           GOLD,
           KING,
      PROM_PAWN, PROM_LANCE, PROM_KNIGHT, PROM_SILVER,
    PROM_BISHOP,  PROM_ROOK,
] = range(15)

PIECE_TYPES = [
           PAWN,      LANCE,      KNIGHT,      SILVER,
         BISHOP,       ROOK,
           GOLD,
           KING,
      PROM_PAWN, PROM_LANCE, PROM_KNIGHT, PROM_SILVER,
    PROM_BISHOP,  PROM_ROOK,
]

PIECES = [NONE,
          BPAWN,      BLANCE,      BKNIGHT,      BSILVER,
        BBISHOP,       BROOK,
          BGOLD,
          BKING,
     BPROM_PAWN, BPROM_LANCE, BPROM_KNIGHT, BPROM_SILVER,
   BPROM_BISHOP,  BPROM_ROOK,       NOTUSE,       NOTUSE,
          WPAWN,      WLANCE,      WKNIGHT,      WSILVER,
        WBISHOP,       WROOK,
          WGOLD,
          WKING,
     WPROM_PAWN, WPROM_LANCE, WPROM_KNIGHT, WPROM_SILVER,
   WPROM_BISHOP, WPROM_ROOK,
] = range(31)

HAND_PIECES = [
          HPAWN,     HLANCE,     HKNIGHT,     HSILVER,
          HGOLD,
        HBISHOP,      HROOK,
] = range(7)

MAX_PIECES_IN_HAND = [
    18, 4, 4, 4,
    4,
    2, 2,
]

MOVE_NONE = 0

REPETITION_TYPES = [
    NOT_REPETITION, REPETITION_DRAW, REPETITION_WIN, REPETITION_LOSE,
    REPETITION_SUPERIOR, REPETITION_INFERIOR
] = range(6)

SVG_PIECE_DEFS = [
    '<g id="black-pawn"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">歩</text></g>',
    '<g id="black-lance"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">香</text></g>',
    '<g id="black-knight"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">桂</text></g>',
    '<g id="black-silver"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">銀</text></g>',
    '<g id="black-gold"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">金</text></g>',
    '<g id="black-bishop"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">角</text></g>',
    '<g id="black-rook"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">飛</text></g>',
    '<g id="black-king"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">王</text></g>',
    '<g id="black-pro-pawn"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">と</text></g>',
    '<g id="black-pro-lance" transform="scale(1.0, 0.5)"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="18">成</text><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="34">香</text></g>',
    '<g id="black-pro-knight" transform="scale(1.0, 0.5)"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="18">成</text><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="34">桂</text></g>',
    '<g id="black-pro-silver" transform="scale(1.0, 0.5)"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="18">成</text><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="34">銀</text></g>',
    '<g id="black-horse"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">馬</text></g>',
    '<g id="black-dragon"><text font-family="serif" font-size="17" text-anchor="middle" x="10.5" y="16.5">龍</text></g>',
    '<g id="white-pawn" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">歩</text></g>',
    '<g id="white-lance" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">香</text></g>',
    '<g id="white-knight" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">桂</text></g>',
    '<g id="white-silver" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">銀</text></g>',
    '<g id="white-gold" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">金</text></g>',
    '<g id="white-bishop" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">角</text></g>',
    '<g id="white-rook" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">飛</text></g>',
    '<g id="white-king" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">王</text></g>',
    '<g id="white-pro-pawn" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">と</text></g>',
    '<g id="white-pro-lance" transform="scale(1.0, 0.5) rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-22">成</text><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-6">香</text></g>',
    '<g id="white-pro-knight" transform="scale(1.0, 0.5) rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-22">成</text><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-6">桂</text></g>',
    '<g id="white-pro-silver" transform="scale(1.0, 0.5) rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-22">成</text><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-6">銀</text></g>',
    '<g id="white-horse" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">馬</text></g>',
    '<g id="white-dragon" transform="rotate(180)"><text font-family="serif" font-size="17" text-anchor="middle" x="-10.5" y="-3.5">龍</text></g>',
]
SVG_PIECE_DEF_IDS = [None,
    "black-pawn", "black-lance", "black-knight", "black-silver",
    "black-bishop", "black-rook",
    "black-gold",
    "black-king",
    "black-pro-pawn", "black-pro-lance", "black-pro-knight", "black-pro-silver",
    "black-horse", "black-dragon", None, None,
    "white-pawn", "white-lance", "white-knight", "white-silver",
    "white-bishop", "white-rook",
    "white-gold",
    "white-king",
    "white-pro-pawn", "white-pro-lance", "white-pro-knight", "white-pro-silver",
    "white-horse", "white-dragon",
]
NUMBER_JAPANESE_NUMBER_SYMBOLS = [ None, '１', '２', '３', '４', '５', '６', '７', '８', '９' ]
NUMBER_JAPANESE_KANJI_SYMBOLS = [ None, "一", "二", "三", "四", "五", "六", "七", "八", "九", "十" ]
SVG_SQUARES = '<g stroke="black"><rect x="20" y="10" width="181" height="181" fill="none" stroke-width="1.5" /><line x1="20.5" y1="30.5" x2="200.5" y2="30.5" stroke-width="1.0" /><line x1="20.5" y1="50.5" x2="200.5" y2="50.5" stroke-width="1.0" /><line x1="20.5" y1="70.5" x2="200.5" y2="70.5" stroke-width="1.0" /><line x1="20.5" y1="90.5" x2="200.5" y2="90.5" stroke-width="1.0" /><line x1="20.5" y1="110.5" x2="200.5" y2="110.5" stroke-width="1.0" /><line x1="20.5" y1="130.5" x2="200.5" y2="130.5" stroke-width="1.0" /><line x1="20.5" y1="150.5" x2="200.5" y2="150.5" stroke-width="1.0" /><line x1="20.5" y1="170.5" x2="200.5" y2="170.5" stroke-width="1.0" /><line x1="40.5" y1="10.5" x2="40.5" y2="190.5" stroke-width="1.0" /><line x1="60.5" y1="10.5" x2="60.5" y2="190.5" stroke-width="1.0" /><line x1="80.5" y1="10.5" x2="80.5" y2="190.5" stroke-width="1.0" /><line x1="100.5" y1="10.5" x2="100.5" y2="190.5" stroke-width="1.0" /><line x1="120.5" y1="10.5" x2="120.5" y2="190.5" stroke-width="1.0" /><line x1="140.5" y1="10.5" x2="140.5" y2="190.5" stroke-width="1.0" /><line x1="160.5" y1="10.5" x2="160.5" y2="190.5" stroke-width="1.0" /><line x1="180.5" y1="10.5" x2="180.5" y2="190.5" stroke-width="1.0" /></g>'
SVG_COORDINATES = '<g><text font-family="serif" text-anchor="middle" font-size="9" x="30.5" y="8">9</text><text font-family="serif" text-anchor="middle" font-size="9" x="50.5" y="8">8</text><text font-family="serif" text-anchor="middle" font-size="9" x="70.5" y="8">7</text><text font-family="serif" text-anchor="middle" font-size="9" x="90.5" y="8">6</text><text font-family="serif" text-anchor="middle" font-size="9" x="110.5" y="8">5</text><text font-family="serif" text-anchor="middle" font-size="9" x="130.5" y="8">4</text><text font-family="serif" text-anchor="middle" font-size="9" x="150.5" y="8">3</text><text font-family="serif" text-anchor="middle" font-size="9" x="170.5" y="8">2</text><text font-family="serif" text-anchor="middle" font-size="9" x="190.5" y="8">1</text><text font-family="serif" font-size="9" x="203.5" y="23">一</text><text font-family="serif" font-size="9" x="203.5" y="43">二</text><text font-family="serif" font-size="9" x="203.5" y="63">三</text><text font-family="serif" font-size="9" x="203.5" y="83">四</text><text font-family="serif" font-size="9" x="203.5" y="103">五</text><text font-family="serif" font-size="9" x="203.5" y="123">六</text><text font-family="serif" font-size="9" x="203.5" y="143">七</text><text font-family="serif" font-size="9" x="203.5" y="163">八</text><text font-family="serif" font-size="9" x="203.5" y="183">九</text></g>'
PIECE_SYMBOLS = [
    '',
    'p', 'l', 'n', 's', 'b', 'r', 'g', 'k', '+p', '+l', '+n', '+s', '+b', '+r'
]
PIECE_JAPANESE_SYMBOLS = [
    '',
    '歩', '香', '桂', '銀', '角', '飛', '金', '玉', 'と', '杏', '圭', '全', '馬', '龍'
]
HAND_PIECE_SYMBOLS = [
    'p', 'l', 'n', 's', 'g', 'b', 'r'
]
HAND_PIECE_JAPANESE_SYMBOLS = [
    "歩", "香", "桂", "銀",
    "金",
    "角", "飛"
]
