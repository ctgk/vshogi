# VShogi

Shogi API for ordinary Shogi and its variants.

Here are lists of shogi implemented or to be implemented:


|Shogi Variants|Board Size|module|
|--------------|----------|------|
|[Minishogi](https://en.wikipedia.org/wiki/Minishogi)|5x5|`vshogi.minishogi`|
|[Judkins shogi](https://en.wikipedia.org/wiki/Judkins_shogi)|6x6|`vshogi.judkins_shogi`|
|[Shogi](https://en.wikipedia.org/wiki/Shogi)|9x9|`vshogi.shogi`|

## Getting Started

You can install the package using `pip` command:

```
pip install git+https://github.com/ctgk/vshogi.git
```

## Examples

### [Minishogi](https://en.wikipedia.org/wiki/Minishogi)

```python
>>> from vshogi.minishogi import *
>>> game = Game()
>>> print(game)
Turn: BLACK
White: -
    5   4   3   2   1
  +---+---+---+---+---+
A |-HI|-KA|-GI|-KI|-OU|
  +---+---+---+---+---+
B |   |   |   |   |-FU|
  +---+---+---+---+---+
C |   |   |   |   |   |
  +---+---+---+---+---+
D |+FU|   |   |   |   |
  +---+---+---+---+---+
E |+OU|+KI|+GI|+KA|+HI|
  +---+---+---+---+---+
Black: -
>>> game.apply(Move(dst=SQ_4C, src=SQ_2E))
Game(sfen="rbsgk/4p/1B3/P4/KGS1R w - 2")
>>> game.apply("1b1c")
Game(sfen="rbsgk/5/1B2p/P4/KGS1R b - 3")
>>> game.apply(["1e2e", "1c1d", "2e2a+"])
Game(sfen="rbs+Rk/5/1B3/P3p/KGS2 w G 6")
>>> print(game)
BLACK_WIN
White: -
    5   4   3   2   1
  +---+---+---+---+---+
A |-HI|-KA|-GI|+RY|-OU|
  +---+---+---+---+---+
B |   |   |   |   |   |
  +---+---+---+---+---+
C |   |+KA|   |   |   |
  +---+---+---+---+---+
D |+FU|   |   |   |-FU|
  +---+---+---+---+---+
E |+OU|+KI|+GI|   |   |
  +---+---+---+---+---+
Black: KI

```

### [Shogi](https://en.wikipedia.org/wiki/Shogi)

```python
>>> from vshogi.shogi import *
>>> game = Game()
>>> print(game)
Turn: BLACK
White: -
    9   8   7   6   5   4   3   2   1
  +---+---+---+---+---+---+---+---+---+
A |-KY|-KE|-GI|-KI|-OU|-KI|-GI|-KE|-KY|
  +---+---+---+---+---+---+---+---+---+
B |   |-HI|   |   |   |   |   |-KA|   |
  +---+---+---+---+---+---+---+---+---+
C |-FU|-FU|-FU|-FU|-FU|-FU|-FU|-FU|-FU|
  +---+---+---+---+---+---+---+---+---+
D |   |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
E |   |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
F |   |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
G |+FU|+FU|+FU|+FU|+FU|+FU|+FU|+FU|+FU|
  +---+---+---+---+---+---+---+---+---+
H |   |+KA|   |   |   |   |   |+HI|   |
  +---+---+---+---+---+---+---+---+---+
I |+KY|+KE|+GI|+KI|+OU|+KI|+GI|+KE|+KY|
  +---+---+---+---+---+---+---+---+---+
Black: -
>>> game.apply(["7g7f", "5c5d", "5g5f", "8c8d", "2h5h"])
Game(sfen="lnsgkgsnl/1r5b1/p1pp1pppp/1p2p4/9/2P1P4/PP1P1PPPP/1B2R4/LNSGKGSNL w - 6")
>>> game.apply(["8d8e", "5f5e", "5d5e", "8h5e", "7a6b"])
Game(sfen="ln1gkgsnl/1r1s3b1/p1pp1pppp/9/1p2B4/2P6/PP1P1PPPP/4R4/LNSGKGSNL b Pp 11")
>>> game.apply("5e3c+")
Game(sfen="ln1gkgsnl/1r1s3b1/p1pp1p+Bpp/9/1p7/2P6/PP1P1PPPP/4R4/LNSGKGSNL w 2Pp 12")
>>> print(game)
BLACK_WIN
White: FU
    9   8   7   6   5   4   3   2   1
  +---+---+---+---+---+---+---+---+---+
A |-KY|-KE|   |-KI|-OU|-KI|-GI|-KE|-KY|
  +---+---+---+---+---+---+---+---+---+
B |   |-HI|   |-GI|   |   |   |-KA|   |
  +---+---+---+---+---+---+---+---+---+
C |-FU|   |-FU|-FU|   |-FU|+UM|-FU|-FU|
  +---+---+---+---+---+---+---+---+---+
D |   |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
E |   |-FU|   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
F |   |   |+FU|   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
G |+FU|+FU|   |+FU|   |+FU|+FU|+FU|+FU|
  +---+---+---+---+---+---+---+---+---+
H |   |   |   |   |+HI|   |   |   |   |
  +---+---+---+---+---+---+---+---+---+
I |+KY|+KE|+GI|+KI|+OU|+KI|+GI|+KE|+KY|
  +---+---+---+---+---+---+---+---+---+
Black: FUx2

```

## Changelog

Please read the [changelog](CHANGELOG.md) to check any notable changes of this project.

## Contributing

Please read the [contribution guidelines](docs/CONTRIBUTING.md) before starting work on a pull request.
