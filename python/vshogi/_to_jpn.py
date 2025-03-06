ARABIC_NUMERALS_FULLWIDTH = [
    chr(c) for c in
    (0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17, 0xFF18, 0xFF19)
]
KANJI_NUMERALS = [
    chr(c) for c in
    (0x4E00, 0x4E8C, 0x4E09, 0x56DB, 0x4E94, 0x516D, 0x4E03, 0x516B, 0x4E5D)
]


def _to_jpn(sq, num_ranks: int) -> str:
    f, r = divmod(int(sq), num_ranks)
    return ARABIC_NUMERALS_FULLWIDTH[f] + KANJI_NUMERALS[r]
