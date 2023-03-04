import cshogi._cshogi as _cs


class Parser:

    def __init__(self):
        self._parser = _cs._Parser()

    @staticmethod
    def parse_file(file, encoding=None):
        if type(file) is str:
            with open(file, 'r', encoding=encoding) as f:
                return Parser.parse_str(f.read())
        else:
            return Parser.parse_str(file.read())

    @staticmethod
    def parse_str(csa_str):
        parsers = []
        # split multiple matches
        matches = csa_str.split('\n/\n')
        for one_csa_str in matches:
            parser = Parser()
            parser.parse_csa_str(one_csa_str)
            parsers.append(parser)
        return parsers

    def parse_csa_file(self, path: str):
        self._parser.parse_csa_file(path)

    def parse_csa_str(self, csa_str: str):
        self._parser.parse_csa_str(csa_str)

    @property
    def version(self):
        return self._parser.version

    @property
    def var_info(self):
        d = {}
        for information in self._parser.informations:
            k, v = information.split(':', 1)
            d[k[1:]] = v
        return d

    @property
    def sfen(self):
        return self._parser.sfen

    @property
    def endgame(self):
        return self._parser.endgame

    @property
    def names(self):
        return [name for name in self._parser.names]

    @property
    def ratings(self):
        return self._parser.ratings

    @property
    def moves(self):
        return self._parser.moves

    @property
    def times(self):
        return self._parser.times

    @property
    def scores(self):
        return self._parser.scores

    @property
    def comments(self):
        return self._parser._comments

    @property
    def comment(self):
        return self._parser.comment

    @property
    def win(self):
        return self._parser.win
