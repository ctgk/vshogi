# Devlog: GumbelAlphaZero coefficient of policy loss

https://github.com/leela-zero/leela-zero/issues/1480

## Training

```shell
for i in (seq -w 0 19); for lcp in 0.01 0.1 1.0 10; python -m vshogi gumbel-alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_lcp{$lcp}_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-loss-coeff-policy {$lcp}; end; end
```

## Results

```shell
$ for lcp in 0.01 1.0 10; python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_lcp0.1_*/models/model_0010.tflite) -p2 (ls judkins_shogi_lcp{$lcp}_*/models/model_0010.tflite) --num-games-each 2 --az-search-second 0.01; end
/Users/ctgk/.pyenv/versions/vshogi-25f1ecf-3.12.12/lib/python3.12/site-packages/torch/distributed/distributed_c10d.py:354: UserWarning: Device capability of jax unspecified, assuming `cpu` and `cuda`. Please specify it via the `devices` argument of `register_backend`.
  warnings.warn(
INFO: Created TensorFlow Lite XNNPACK delegate for CPU.
player1: ('judkins_shogi_lcp0.1_00/models/model_0010.tflite', 'judkins_shogi_lcp0.1_01/models/model_0010.tflite', 'judkins_shogi_lcp0.1_02/models/model_0010.tflite', 'judkins_shogi_lcp0.1_03/models/model_0010.tflite', 'judkins_shogi_lcp0.1_04/models/model_0010.tflite', 'judkins_shogi_lcp0.1_05/models/model_0010.tflite', 'judkins_shogi_lcp0.1_06/models/model_0010.tflite', 'judkins_shogi_lcp0.1_07/models/model_0010.tflite', 'judkins_shogi_lcp0.1_08/models/model_0010.tflite', 'judkins_shogi_lcp0.1_09/models/model_0010.tflite', 'judkins_shogi_lcp0.1_10/models/model_0010.tflite', 'judkins_shogi_lcp0.1_11/models/model_0010.tflite', 'judkins_shogi_lcp0.1_12/models/model_0010.tflite', 'judkins_shogi_lcp0.1_13/models/model_0010.tflite', 'judkins_shogi_lcp0.1_14/models/model_0010.tflite', 'judkins_shogi_lcp0.1_15/models/model_0010.tflite', 'judkins_shogi_lcp0.1_16/models/model_0010.tflite', 'judkins_shogi_lcp0.1_17/models/model_0010.tflite', 'judkins_shogi_lcp0.1_18/models/model_0010.tflite', 'judkins_shogi_lcp0.1_19/models/model_0010.tflite')
player2: ('judkins_shogi_lcp0.01_00/models/model_0010.tflite', 'judkins_shogi_lcp0.01_01/models/model_0010.tflite', 'judkins_shogi_lcp0.01_02/models/model_0010.tflite', 'judkins_shogi_lcp0.01_03/models/model_0010.tflite', 'judkins_shogi_lcp0.01_04/models/model_0010.tflite', 'judkins_shogi_lcp0.01_05/models/model_0010.tflite', 'judkins_shogi_lcp0.01_06/models/model_0010.tflite', 'judkins_shogi_lcp0.01_07/models/model_0010.tflite', 'judkins_shogi_lcp0.01_08/models/model_0010.tflite', 'judkins_shogi_lcp0.01_09/models/model_0010.tflite', 'judkins_shogi_lcp0.01_10/models/model_0010.tflite', 'judkins_shogi_lcp0.01_11/models/model_0010.tflite', 'judkins_shogi_lcp0.01_12/models/model_0010.tflite', 'judkins_shogi_lcp0.01_13/models/model_0010.tflite', 'judkins_shogi_lcp0.01_14/models/model_0010.tflite', 'judkins_shogi_lcp0.01_15/models/model_0010.tflite', 'judkins_shogi_lcp0.01_16/models/model_0010.tflite', 'judkins_shogi_lcp0.01_17/models/model_0010.tflite', 'judkins_shogi_lcp0.01_18/models/model_0010.tflite', 'judkins_shogi_lcp0.01_19/models/model_0010.tflite')
+---------+-------+-------+-------+
| Player1 | total | black | white |
+---------+-------+-------+-------+
|   #Win  |   804 |   435 |   369 |
+---------+-------+-------+-------+
|  #Draw  |    10 |     7 |     3 |
+---------+-------+-------+-------+
|  #Loss  |   786 |   358 |   428 |
+---------+-------+-------+-------+

/Users/ctgk/.pyenv/versions/vshogi-25f1ecf-3.12.12/lib/python3.12/site-packages/torch/distributed/distributed_c10d.py:354: UserWarning: Device capability of jax unspecified, assuming `cpu` and `cuda`. Please specify it via the `devices` argument of `register_backend`.
  warnings.warn(
INFO: Created TensorFlow Lite XNNPACK delegate for CPU.
player1: ('judkins_shogi_lcp0.1_00/models/model_0010.tflite', 'judkins_shogi_lcp0.1_01/models/model_0010.tflite', 'judkins_shogi_lcp0.1_02/models/model_0010.tflite', 'judkins_shogi_lcp0.1_03/models/model_0010.tflite', 'judkins_shogi_lcp0.1_04/models/model_0010.tflite', 'judkins_shogi_lcp0.1_05/models/model_0010.tflite', 'judkins_shogi_lcp0.1_06/models/model_0010.tflite', 'judkins_shogi_lcp0.1_07/models/model_0010.tflite', 'judkins_shogi_lcp0.1_08/models/model_0010.tflite', 'judkins_shogi_lcp0.1_09/models/model_0010.tflite', 'judkins_shogi_lcp0.1_10/models/model_0010.tflite', 'judkins_shogi_lcp0.1_11/models/model_0010.tflite', 'judkins_shogi_lcp0.1_12/models/model_0010.tflite', 'judkins_shogi_lcp0.1_13/models/model_0010.tflite', 'judkins_shogi_lcp0.1_14/models/model_0010.tflite', 'judkins_shogi_lcp0.1_15/models/model_0010.tflite', 'judkins_shogi_lcp0.1_16/models/model_0010.tflite', 'judkins_shogi_lcp0.1_17/models/model_0010.tflite', 'judkins_shogi_lcp0.1_18/models/model_0010.tflite', 'judkins_shogi_lcp0.1_19/models/model_0010.tflite')
player2: ('judkins_shogi_lcp1.0_00/models/model_0010.tflite', 'judkins_shogi_lcp1.0_01/models/model_0010.tflite', 'judkins_shogi_lcp1.0_02/models/model_0010.tflite', 'judkins_shogi_lcp1.0_03/models/model_0010.tflite', 'judkins_shogi_lcp1.0_04/models/model_0010.tflite', 'judkins_shogi_lcp1.0_05/models/model_0010.tflite', 'judkins_shogi_lcp1.0_06/models/model_0010.tflite', 'judkins_shogi_lcp1.0_07/models/model_0010.tflite', 'judkins_shogi_lcp1.0_08/models/model_0010.tflite', 'judkins_shogi_lcp1.0_09/models/model_0010.tflite', 'judkins_shogi_lcp1.0_10/models/model_0010.tflite', 'judkins_shogi_lcp1.0_11/models/model_0010.tflite', 'judkins_shogi_lcp1.0_12/models/model_0010.tflite', 'judkins_shogi_lcp1.0_13/models/model_0010.tflite', 'judkins_shogi_lcp1.0_14/models/model_0010.tflite', 'judkins_shogi_lcp1.0_15/models/model_0010.tflite', 'judkins_shogi_lcp1.0_16/models/model_0010.tflite', 'judkins_shogi_lcp1.0_17/models/model_0010.tflite', 'judkins_shogi_lcp1.0_18/models/model_0010.tflite', 'judkins_shogi_lcp1.0_19/models/model_0010.tflite')
+---------+-------+-------+-------+
| Player1 | total | black | white |
+---------+-------+-------+-------+
|   #Win  |   943 |   517 |   426 |
+---------+-------+-------+-------+
|  #Draw  |     5 |     3 |     2 |
+---------+-------+-------+-------+
|  #Loss  |   652 |   280 |   372 |
+---------+-------+-------+-------+

/Users/ctgk/.pyenv/versions/vshogi-25f1ecf-3.12.12/lib/python3.12/site-packages/torch/distributed/distributed_c10d.py:354: UserWarning: Device capability of jax unspecified, assuming `cpu` and `cuda`. Please specify it via the `devices` argument of `register_backend`.
  warnings.warn(
INFO: Created TensorFlow Lite XNNPACK delegate for CPU.
player1: ('judkins_shogi_lcp0.1_00/models/model_0010.tflite', 'judkins_shogi_lcp0.1_01/models/model_0010.tflite', 'judkins_shogi_lcp0.1_02/models/model_0010.tflite', 'judkins_shogi_lcp0.1_03/models/model_0010.tflite', 'judkins_shogi_lcp0.1_04/models/model_0010.tflite', 'judkins_shogi_lcp0.1_05/models/model_0010.tflite', 'judkins_shogi_lcp0.1_06/models/model_0010.tflite', 'judkins_shogi_lcp0.1_07/models/model_0010.tflite', 'judkins_shogi_lcp0.1_08/models/model_0010.tflite', 'judkins_shogi_lcp0.1_09/models/model_0010.tflite', 'judkins_shogi_lcp0.1_10/models/model_0010.tflite', 'judkins_shogi_lcp0.1_11/models/model_0010.tflite', 'judkins_shogi_lcp0.1_12/models/model_0010.tflite', 'judkins_shogi_lcp0.1_13/models/model_0010.tflite', 'judkins_shogi_lcp0.1_14/models/model_0010.tflite', 'judkins_shogi_lcp0.1_15/models/model_0010.tflite', 'judkins_shogi_lcp0.1_16/models/model_0010.tflite', 'judkins_shogi_lcp0.1_17/models/model_0010.tflite', 'judkins_shogi_lcp0.1_18/models/model_0010.tflite', 'judkins_shogi_lcp0.1_19/models/model_0010.tflite')
player2: ('judkins_shogi_lcp10_00/models/model_0010.tflite', 'judkins_shogi_lcp10_01/models/model_0010.tflite', 'judkins_shogi_lcp10_02/models/model_0010.tflite', 'judkins_shogi_lcp10_03/models/model_0010.tflite', 'judkins_shogi_lcp10_04/models/model_0010.tflite', 'judkins_shogi_lcp10_05/models/model_0010.tflite', 'judkins_shogi_lcp10_06/models/model_0010.tflite', 'judkins_shogi_lcp10_07/models/model_0010.tflite', 'judkins_shogi_lcp10_08/models/model_0010.tflite', 'judkins_shogi_lcp10_09/models/model_0010.tflite', 'judkins_shogi_lcp10_10/models/model_0010.tflite', 'judkins_shogi_lcp10_11/models/model_0010.tflite', 'judkins_shogi_lcp10_12/models/model_0010.tflite', 'judkins_shogi_lcp10_13/models/model_0010.tflite', 'judkins_shogi_lcp10_14/models/model_0010.tflite', 'judkins_shogi_lcp10_15/models/model_0010.tflite', 'judkins_shogi_lcp10_16/models/model_0010.tflite', 'judkins_shogi_lcp10_17/models/model_0010.tflite', 'judkins_shogi_lcp10_18/models/model_0010.tflite', 'judkins_shogi_lcp10_19/models/model_0010.tflite')
+---------+-------+-------+-------+
| Player1 | total | black | white |
+---------+-------+-------+-------+
|   #Win  |  1255 |   640 |   615 |
+---------+-------+-------+-------+
|  #Draw  |     4 |     1 |     3 |
+---------+-------+-------+-------+
|  #Loss  |   341 |   159 |   182 |
+---------+-------+-------+-------+
```

WDL against `lcp0.1`

|lcp|WDL|
|---|---|
|0.01|786-10-804|
|1.0|652-5-943|
|10|341-4-1255|

## Discussion

`lcp=0.1` outperforms all other tested values.
Against `lcp=0.01` the margin is small (804-10-786), suggesting that too-small a policy loss coefficient only mildly hurts training.
Against `lcp=1.0` and `lcp=10` the gap widens substantially (943-5-652 and 1255-4-341 respectively), indicating that over-weighting the policy loss significantly degrades performance.
The win rate of `lcp=0.1` against each challenger increases monotonically with `lcp`, from ~51% to ~59% to ~78%, which suggests the performance penalty grows super-linearly as the policy loss dominates the training signal.

Conclusion: Keep the current default value (0.1) for the coefficient of policy loss.
