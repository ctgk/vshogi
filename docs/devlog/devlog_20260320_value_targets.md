# Devlog: Value Targets

Git hash: 55c1b60

## Training

```shell
for i in (seq -w 0 19); for z in 0.2 0.3 0.4 0.5 1.0; python -m vshogi dlshogi cycler judkins_shogi \
    --cycles 10 --play-jobs 10 --play-num-games 100 --play-num-simulations 100 \
    --train-win-ratio-threshold 0.01 --train-device mps \
    --train-discount-factor 1.0 \
    --train-importance-decay 1.0 --train-backup-result always \
    --train-result-backup-rate $z --train-no-averagize-buffer \
    --output judkins_shogi_z{$z}_{$i}; end; end;
```

## Results

```shell
for z in 0.2 0.3 0.4 0.5; python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_z{$z}_*/models/model_0010.tflite) -p2 (ls judkins_shogi_z1.0_*/models/model_0010.tflite) --num-games-each 2 --az-search-second 0.01; end
```

| Player | WDL |
| --- | --- |
| z0.2 vs z1.0 | 1359-2-239 |
| **z0.3** vs z1.0 | **1371-6-223** |
| z0.4 vs z1.0 | 1320-0-280 |
| z0.5 vs z1.0 | 1261-2-337 |

## Discussion

| Board size | Best result backup rate |
| --- | --- |
| 5x5 | 0.1 |
| 6x6 | 0.3 |

The optimal result backup rate appears to increase with board size.

This can be understood as a bias–variance tradeoff between the two training targets:

- The game result $Z$ is **unbiased** but **high variance** — it is a single delayed signal that must propagate back across many moves.
- The bootstrapped MCTS value $Q$ has **lower variance** but **higher bias** — it reflects approximation errors accumulated in the neural network.

On a larger board, games are longer and MCTS can explore only a smaller fraction of the total game tree, so $Q$-values carry more approximation error (higher bias). This makes $Q$ a less reliable training target, and leaning more toward the unbiased $Z$ (i.e., a higher result backup rate) yields better training signal despite its higher variance.

On a smaller board, games are shorter and MCTS can explore proportionally deeper, producing more accurate $Q$-values. Relying more on $Q$ (lower result backup rate) provides a lower-variance signal that speeds up learning.
