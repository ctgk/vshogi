# Devlog 2026-02-21

Git hash: 0e42464

## AlphaZero Value Targets

ref: https://medium.com/oracledevs/lessons-from-alphazero-part-4-improving-the-training-target-6efba2e71628

### Experiment: Search for best value targets

```shell
python -m vshogi dlshogi cycler minishogi \
    --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 \
    --train-win-ratio-threshold 0.01 --train-device mps \
    --train-buffer-size 10000 \
    --train-buffer-decay 0.0 --train-discount-factor 1.0 \
    --train-importance-decay 1.0 --train-backup-result always \
    --train-default-result-rate $r \
    --output minishogi_z{$r}_{$i}  # i=00-19
```

1. r = 1.0: Train AlphaZero on game result, which is the original AlphaZero
    - $1.0 \cdot z + 0.0 \cdot q$
2. r = 0.8
    - $0.8 \cdot z + 0.2 \cdot q$
3. r = 0.5
    - $0.5 \cdot z + 0.5 \cdot q$
4. r = 0.2
    - $0.2 \cdot z + 0.8 \cdot q$
5. r = 0.0: Train AlphaZero on bootstrapped value
    - $0.0 \cdot z + 1.0 \cdot q$

#### Result

WDLs of player:

| Player | r = 1.0| r = 0.8 | r = 0.5 | r = 0.2 | r = 0.0 |
| --- | --- | --- | --- | --- | --- |
| r = 1.0 | (781-7-812) | 628-6-966 | 466-10-1124 | 381-20-1199 | 420-16-1164 |
| r = 0.8 | 966-6-628 | (822-7-771) | 636-15-949 | 486-25-1089 | 552-19-1029 |
| r = 0.5 | 1124-10-466 | 949-15-636 | (775-39-786) | 648-74-878 | 740-73-787 |
| **r = 0.2** | **1199-20-381** | **1089-25-486** | **878-74-648** | (732-132-736) | **867-93-640** |
| r = 0.0 | 1164-16-420 | 1029-19-552 | 787-73-740 | 640-93-867 | (704-140-756) |

Command:

```shell
python -m vshogi match minishogi \
    -p1 ... # 20 players trained on the same configuration \
    -p2 ... # Another group of 20 players trained on a different configuration \
    --num-games-each 2 --az-search-second 0.01
```

### Experiment: Direct comparison between `r = 0.0` to `r = 0.2`

| Generation | `r = 0.2` vs `r = 0.0`|
| --- | --- |
| 1st | 405-3-392 |
| 2nd | 444-6-350 |
| 3rd | 421-14-365 |
| 4th | 429-14-357 |
| 5th | 458-12-330 |
| 6th | 463-35-302 |
| 7th | 436-42-322 |
| 8th | 434-51-315 |
| 9th | 416-58-326 |
| 10th | 402-71-327 |

### Experiment: Hyperparameter search between `r = 0.0` to `r = 0.5`

| Player | r = 0.5 | r = 0.3 | r = 0.2 | r = 0.1 | r = 0.0 |
| --- | --- | --- | --- | --- | --- |
| r = 0.5 | (383-30-387) | 349-30-421 | 327-42-431 | 326-37-437 | 407-35-358 |
| r = 0.3 | 421-30-349 | (385-46-369) | 333-61-406 | 359-68-373 | 384-52-364 |
| r = 0.2 | 431-42-327 | 406-61-333 | (336-116-348) | 330-108-362 | 413-71-316 |
| **r = 0.1** | **437-37-326** | **373-68-359** | **362-108-330** | (351-100-349) | **401-80-319** |
| r = 0.0 | 358-35-407 | 364-52-384 | 316-71-413 | 319-80-401 | (371-74-355) |

```shell
python -m vshogi match minishogi \
    -p1 ... # 20 players trained on the same configuration \
    -p2 ... # Another group of 20 players trained on a different configuration \
    --num-games-each 1 --az-search-second 0.005
```
