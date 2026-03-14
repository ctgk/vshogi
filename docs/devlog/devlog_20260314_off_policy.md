# Devlog: Off-Policy Training

Git hash: 9489404

## Training Setup

```shell
$ for i in (seq -w 0 19); for z in 1.0 0.3 0.1 0.0; for o in 0.0 0.5 1.0; python -m vshogi dlshogi cycler minishogi \
    --cycles 5 --play-jobs 10 --play-num-games 50 --play-num-simulations 50 \
    --train-win-ratio-threshold 0.01 --train-device mps \
    --train-discount-factor 1.0 \
    --train-importance-decay 1.0 --train-backup-result always \
    --train-result-backup-rate $z --train-off-policy-rate {$o} \
    --output minishogi_z{$z}_o{$o}_{$i}; end; end; end
```

## Results

### Effect of Off-Policy Rate at Fixed Result-Backup Rate

```shell
$ for z in 0.0 0.1 0.3 1.0; for o in 0.5 1.0; python -m vshogi match minishogi -p1 (ls minishogi_z{$z}_o0.0_*/models/model_0005.tflite) -p2 (ls minishogi_z{$z}_o{$o}_*/models/model_0005.tflite) --num-games-each 2 --az-search-second 0.005; end; end
```

|z|o=0.0 vs o=0.5|o=0.0 vs o=1.0|
|---|---|---|
|0.0|826-23-751|890-11-699|
|0.1|791-32-777|803-31-766|
|0.3|727-22-851|761-8-831|
|1.0|637-13-950|738-3-859|

Conclusion: Increasing the off-policy rate is not universally beneficial. Its effect depends strongly on z (the result-backup rate), and z appears to be the dominant factor. Off-policy data seems useful only when the training target is robust to policy mismatch.

### Comparison Against Simple AlphaZero

```shell
$ for z in 0.0 0.1 0.3 1.0; for o in 0.0 0.5 1.0; python -m vshogi match minishogi -p1 (ls minishogi_z1.0_o0.0_*/models/model_0005.tflite) -p2 (ls minishogi_z{$z}_o{$o}_*/models/model_0005.tflite) --num-games-each 2 --az-search-second 0.005; end; end
```

|Result-backup rate|Off-policy rate|WDL|Elo rating|
|---|---|---|---|
|0.0|0.0|1214-13-373|1202.9|
|0.0|0.5|1211-5-384|1198.8|
|0.0|1.0|1149-4-447|1163.5|
|0.1|0.0|1248-9-343|1222.7|
|0.1|0.5|1257-9-334|1228.5|
|0.1|1.0|1202-1-397|1192.3|
|0.3|0.0|1202-6-392|1193.8|
|0.3|0.5|1241-6-353|1217.3|
|0.3|1.0|1218-6-376|1203.2|
|1.0|0.0|769-11-820|988.9|
|1.0|0.5|967-14-619|1076.8|
|1.0|1.0|856-5-739|1025.5|

|Elo rating|z=0.0|z=0.1|z=0.3|z=1.0|
|---|---|---|---|---|
|o=0.0|1202.9|1222.7|1193.8|988.9|
|o=0.5|1198.8|1228.5|1217.3|1076.8|
|o=1.0|1163.5|1192.3|1203.2|1025.5|

Training with off-policy data did not produce a major improvement.
For now, I will keep the on-policy configuration as the default because it is simpler.
