# Devlog: Loss backup path

Git hash: d4ada0f

## Training

```shell
for i in (seq -w 0 19); for p in any best; python -m vshogi alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_backup_{$p}_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-loss-backup-path $p; end; end
```

## Results

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_backup_any_*/models/model_{$i}.tflite) -p2 (ls judkins_shogi_backup_best_*/models/model_{$i}.tflite) --num-games-each 5 --az-search-second 0.01 --show-outer-pbar; end
```

|Generation|any vs best|
|---|---|
|1|1732W-7D-2261L|
|2|2081W-14D-1905L|
|3|2071W-5D-1924L|
|4|2081W-24D-1895L|
|5|2021W-13D-1966L|
|6|2090W-19D-1891L|
|7|1977W-17D-2006L|
|8|2136W-14D-1850L|
|9|2156W-25D-1819L|
|10|2077W-21D-1902L|

## Discussion

### Trend we can assume from the result

- Across 10 generations, `any` wins 20422 games and loses 19419 games against `best` (159 draws), so `any` is consistently slightly stronger overall.
- The per-generation result fluctuates (notably generation 1 is clearly worse for `any`, and generation 7 is near parity), but from generation 8 onward `any` keeps a clearer edge.
- The practical effect size is small but stable: overall score rate is about 51.25% for `any`.

### Statistical significance of the trend

- If we ignore draws, the decisive-game win rate of `any` is

$$
\hat{p}=\frac{20422}{20422+19419}=0.5126
$$

- Under the null hypothesis $p=0.5$, this corresponds to a z-score of about $10.05$, giving $p \ll 10^{-6}$ (extremely significant).
- A 95% confidence interval for decisive-game win rate is approximately $[0.5077, 0.5175]$, which is entirely above 0.5.
- In other words, the direction of the effect (`any` > `best`) is statistically very reliable, even though the margin is modest.

### Preferred configuration

- Preferred option: `--train-loss-backup-path any`.
- Reason: it shows a statistically significant and repeatable advantage over `best` in aggregate head-to-head results.
- Practical note: because the gain is not large, this should still be periodically revalidated when changing other training settings (network size, simulation budget, replay settings, etc.).
