import doctest

import pytest

from vshogi import Game


def test_to_policy_probas():
    tests = doctest.DocTestFinder().find(Game.to_policy_probas)
    runner = doctest.DocTestRunner()
    for test in tests:
        actual = runner.run(test)
        print(actual)
        assert actual.failed == 0


def test_get_sfen_at():
    tests = doctest.DocTestFinder().find(Game.get_sfen_at)
    runner = doctest.DocTestRunner()
    for test in tests:
        actual = runner.run(test)
        print(actual)
        assert actual.failed == 0


if __name__ == '__main__':
    pytest.main([__file__])
