import doctest

import pytest

from vshogi import Game


def run_doctest_of_class_method(class_method):
    tests = doctest.DocTestFinder().find(class_method)
    runner = doctest.DocTestRunner()
    for test in tests:
        actual = runner.run(test)
        print(actual)
        assert actual.failed == 0


def test_to_policy_probas():
    run_doctest_of_class_method(Game.to_policy_probas)


def test_get_move_at():
    run_doctest_of_class_method(Game.get_move_at)


def test_get_sfen_at():
    run_doctest_of_class_method(Game.get_sfen_at)


if __name__ == '__main__':
    pytest.main([__file__])
