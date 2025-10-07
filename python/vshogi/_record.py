from dataclasses import dataclass

from vshogi._vshogi import Result


@dataclass(frozen=True)
class Record:
    """Aggregated results of a player."""

    wins_black: int = 0
    wins_white: int = 0
    draws_black: int = 0
    draws_white: int = 0
    losses_black: int = 0
    losses_white: int = 0

    @property
    def wins_total(self) -> int:
        """Return number of wins in total.

        Notes
        -----
        It is sum of `wins_black` and `wins_white`.

        Returns
        -------
        int
            Number of wins in total.
        """
        return self.wins_black + self.wins_white

    @property
    def draws_total(self) -> int:
        """Return number of draws in total.

        Notes
        -----
        It is sum of `draws_black` and `draws_white`.

        Returns
        -------
        int
            Number of draws in total.
        """
        return self.draws_black + self.draws_white

    @property
    def losses_total(self) -> int:
        """Return number of draws in total.

        Notes
        -----
        It is sum of `losses_black` and `losses_white`.

        Returns
        -------
        int
            Number of losses in total.
        """
        return self.losses_black + self.losses_white

    def __invert__(self) -> 'Record':
        """Return a record from opponent's perspective.

        Returns
        -------
        Record
            A new record representing the same results viewed from the
            opponent's side.

        Examples
        --------
        >>> ~Record(2, 0, 0, 1, 1, 0)  # doctest: +ELLIPSIS
        Record(wins_black=0, wins_white=1, draws_black=1, ...
        """
        return Record(
            wins_black=self.losses_white,
            wins_white=self.losses_black,
            draws_black=self.draws_white,
            draws_white=self.draws_black,
            losses_black=self.wins_white,
            losses_white=self.wins_black,
        )

    def __add__(self, other: 'Record') -> 'Record':
        """Add two records and return the result.

        Parameters
        ----------
        other : Record
            Another record to add.

        Returns
        -------
        Record
            Resulting record.
        """
        return Record(
            wins_black=self.wins_black + other.wins_black,
            wins_white=self.wins_white + other.wins_white,
            draws_black=self.draws_black + other.draws_black,
            draws_white=self.draws_white + other.draws_white,
            losses_black=self.losses_black + other.losses_black,
            losses_white=self.losses_white + other.losses_white,
        )

    @staticmethod
    def from_black_result(r: Result) -> 'Record':
        """Return a record for black player from a result of a game.

        Parameters
        ----------
        r : Result
            Result of a game.

        Returns
        -------
        Record
            Record for black player from result.
        """
        if r == Result.BLACK_WIN:
            return Record(1, 0, 0, 0, 0, 0)
        if r == Result.WHITE_WIN:
            return Record(0, 0, 0, 0, 1, 0)
        if r == Result.DRAW:
            return Record(0, 0, 1, 0, 0, 0)
        return Record(0, 0, 0, 0, 0, 0)

    @staticmethod
    def from_white_result(r: Result) -> 'Record':
        """Return a record for white player from a result of a game.

        Parameters
        ----------
        r : Result
            Result of a game.

        Returns
        -------
        Record
            Record for white player from result.
        """
        if r == Result.BLACK_WIN:
            return Record(0, 0, 0, 0, 0, 1)
        if r == Result.WHITE_WIN:
            return Record(0, 1, 0, 0, 0, 0)
        if r == Result.DRAW:
            return Record(0, 0, 0, 1, 0, 0)
        return Record(0, 0, 0, 0, 0, 0)

    def wdl(self) -> str:
        """Return Wins-Draws-Losses representation of the record.

        Returns
        -------
        str
            Wins-Draws-Losses representation of the record.
        """
        return f'{self.wins_total}W-{self.draws_total}D-{self.losses_total}L'

    def score(self) -> float:
        """Compute the total score for this record.

        Scoring rules:
        - Win: 1.0 point
        - Draw as Black: 0.4 points
        - Draw as White: 0.6 points
        - Loss: 0 points

        Returns
        -------
        float
            Total score based on the record.

        Examples
        --------
        >>> Record(2, 1, 1, 0, 1, 0).score()
        3.4
        >>> Record(0, 0, 0, 2, 0, 0).score()
        1.2
        """
        return (
            self.wins_total
            + 0.4 * self.draws_black
            + 0.6 * self.draws_white
        )


if __name__ == '__main__':
    r1 = Record(1, 0, 0, 0, 0, 1)
    r2 = Record(1, 0, 0, 0, 0, 0)
    print(r1 + r2)
    print((r1 + r2).wins_total)
