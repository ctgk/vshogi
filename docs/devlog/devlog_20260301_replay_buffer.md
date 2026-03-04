# Devlog 2026-03-01

## Replay Buffer Averaging

Git hash: e85ce32

### Training

```shell
for i in (seq -w 0 19); for d in (seq -f "0.%g" 0 9); python -m vshogi dlshogi cycler minishogi \
    --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 \
    --train-win-ratio-threshold 0.01 --train-device mps \
    --train-buffer-size 10000 \
    --train-discount-factor 1.0 \
    --train-importance-decay 1.0 \
    --train-result-backup-rate 0.1 \
    --train-buffer-decay $d \
    --output minishogi_d{$d}_{$i}; end; end;
```

### Result

Command:

```shell
for d1 in 0 5 9; for d2 in 0 5 9; python -m vshogi match minishogi -p1 (ls minishogi_d0.{$d1}_*/models/model_0010.tflite) -p2 (ls minishogi_d0.{$d2}_*/models/model_0010.tflite) --n
um-games-each 5 --az-search-second 0.003; end; end
```

| Player | d=0.0 | d=0.5 | d=0.9 |
| --- | --- | --- | --- |
|d=0.0|(1655-644-1701)|1796-524-1680|1768-620-1612|
|d=0.5|1748-515-1737|(1827-443-1730)|1755-466-1779|
|d=0.9|1577-651-1772|1749-461-1790|(1682-608-1710)|

The choice of buffer decay rate has no meaningful difference.

```shell
$ for d1 in 9 95 99; for d2 in 95 99; python -m vshogi match minishogi -p1 (ls minishogi_d0.{$d1}_*/models/model_0010.tflite) -p2 (ls minishogi_d0.{$d2}_*/models/model_0010.tflite) --num-games-each 2 --az-search-second 0.005; end; end
```

| Player | d=0.9 | d=0.95 | d=0.99 |
| --- | --- | --- | --- |
|d=0.9|(?-?-?)|745-152-703|741-143-716|
|d=0.95|703-152-745|(763-117-720)|788-111-701|
|d=0.99|716-143-741|736-112-752|(746-129-725)|

Command:

```shell
for d1 in 0 5 9; for d2 in 0 5 9; python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_d0.{$d1}_0*/models/model_0010.tflite) -p2 (ls judkins_shogi_d0.{$d2}_0*/models/model_0010.tflite) --num-games-each 2 --az-search-second 0.01; end; end
```

| Player | d=0.0 | d=0.5 | d=0.9 |
| --- | --- | --- | --- |
|d=0.0|(197-5-198)|213-0-187|227-3-170|
|d=0.5|173-3-224|(205-5-190)|218-2-180|
|d=0.9|184-2-214|170-2-228|(210-1-189)|

Smaller decay rate yielded better result.
Computing moving average made results worse. It is possibly because the effect of earlier generations stay longer.
