# Devlog: GumbelAlphaZero training target

The original [Gumbel-AlphaZero paper](https://openreview.net/pdf?id=bERaNdoegnO)
does not explicitly specify the training target for the value network.
We use MCTS value as the training target because the value network should
approximate v_pi according to the paper:

> We will construct an approximation of v_pi.
> The exact v_pi is defined by v_pi = \sum_{a}pi(a)q(a).
> We have an approximate \hat{v}_pi from a value network,
> we know pi, and we have q(a) for the visited actions.

## Results

```shell
$ python -m vshogi match judkins_shogi -p1 (ls mcts_value/judkins_shogi_gaz_*/models/model_0010.tflite) -p2 (ls game_result/judkins_shogi_gaz_*/models/model_0010.tflite) --num-games-each 2 --az-search-second 0.01
+---------+-------+-------+-------+
| Player1 | total | black | white |
+---------+-------+-------+-------+
|   #Win  |  1087 |   568 |   519 |
+---------+-------+-------+-------+
|  #Draw  |     6 |     1 |     5 |
+---------+-------+-------+-------+
|  #Loss  |   507 |   231 |   276 |
+---------+-------+-------+-------+
```
