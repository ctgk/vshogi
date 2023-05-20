# VShogi

Shogi API for ordinary Shogi and its variants.

Here are lists of shogi to be implemented:

- Shogi (将棋)
- Animal Shogi (動物将棋)

## Getting Started

1. Installation process
1. Software dependencies
1. Latest releases
1. API references

## Examples

### [Minishogi](https://en.wikipedia.org/wiki/Minishogi)

```python
>>> from vshogi.minishogi import *
>>> game = Game()
>>> print(game)
Turn: BLACK
White: -
    5   4   3   2   1
  *---*---*---*---*---*
A |-HI|-KA|-GI|-KI|-OU|
  *---*---*---*---*---*
B |   |   |   |   |-FU|
  *---*---*---*---*---*
C |   |   |   |   |   |
  *---*---*---*---*---*
D |+FU|   |   |   |   |
  *---*---*---*---*---*
E |+OU|+KI|+GI|+KA|+HI|
  *---*---*---*---*---*
Black: -
>>> game.apply(Move(SQ_4C, SQ_2E))
Game(sfen="rbsgk/4p/1B3/P4/KGS1R w - 2")
>>> game.apply(Move(SQ_1C, SQ_1B))
Game(sfen="rbsgk/5/1B2p/P4/KGS1R b - 3")
>>> game.apply(Move(SQ_2E, SQ_1E))
Game(sfen="rbsgk/5/1B2p/P4/KGSR1 w - 4")
>>> game.apply(Move(SQ_1D, SQ_1C))
Game(sfen="rbsgk/5/1B3/P3p/KGSR1 b - 5")
>>> game.apply(Move(SQ_2A, SQ_2E, promote=True))
Game(sfen="rbs+Rk/5/1B3/P3p/KGS2 w G 6")
>>> print(game)
BLACK_WIN
White: -
    5   4   3   2   1
  *---*---*---*---*---*
A |-HI|-KA|-GI|+RY|-OU|
  *---*---*---*---*---*
B |   |   |   |   |   |
  *---*---*---*---*---*
C |   |+KA|   |   |   |
  *---*---*---*---*---*
D |+FU|   |   |   |-FU|
  *---*---*---*---*---*
E |+OU|+KI|+GI|   |   |
  *---*---*---*---*---*
Black: KI
```

## Changelog

Please read the [changelog](CHANGELOG.md) to check any notable changes of this project.

## Contributing

Please read the [contribution guidelines](CONTRIBUTING.md) before starting work on a pull request.
