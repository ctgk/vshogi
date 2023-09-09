import doctest
import inspect

import pytest

import vshogi.animal_shogi as shogi


def run_doctest_of_class_method(class_method):
    tests = doctest.DocTestFinder().find(class_method)
    runner = doctest.DocTestRunner()
    for test in tests:
        actual = runner.run(test)
        print(actual)
        assert actual.failed == 0


def test_game_methods():
    for _, method in inspect.getmembers(
        shogi.Game(),
        predicate=inspect.ismethod,
    ):
        run_doctest_of_class_method(method)


if __name__ == '__main__':
    pytest.main([__file__])
