# Devlog: 2026/07/20 MCGS Enhanced Checks

Git hash: ab9fb81d

## Training

```shell
for i in (seq -w 0 19); for e in true false; python -m vshogi mcgs cycler judkins_shogi --cycles 10 --output judkins_shogi_ec{$e}_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 300 --play-enhanced-checks $e --train-validation-threshold 0.01 --train-device mps; end; end
```

## Results

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_ectrue_*/mod
els/model_{$i}.tflite) -p2 (ls judkins_shogi_ecfalse_*/models/model_{$i}.tflite) --num-games-each 5 --az-search-second 0.005 --show-outer-pbar; end
```

Enhanced checks vs without the enhancement

| Model | W | D | L | Net |
| --- | ---: | ---: | ---: | ---: |
| model_0001 | 2019 | 18 | 1963 | +56 |
| model_0002 | 2073 | 38 | 1889 | +184 |
| model_0003 | 2168 | 26 | 1806 | +362 |
| model_0004 | 2296 | 6 | 1698 | +598 |
| model_0005 | 2120 | 15 | 1865 | +255 |
| model_0006 | 2092 | 19 | 1889 | +203 |
| model_0007 | 1994 | 14 | 1992 | +2 |
| model_0008 | 1943 | 15 | 2042 | -99 |
| model_0009 | 2138 | 19 | 1843 | +295 |
| model_0010 | 2131 | 9 | 1860 | +271 |
| **Total** | **20974** | **179** | **18847** | **+2127** |
