# Devlog: GumbelAlphaZero

## Training

```shell
for i in (seq -w 0 19); python -m vshogi alpha-zero cycler minishogi --cycles 10 --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps --output minishogi_az_$i; python -m vshogi gumbel-alpha-zero cycler minishogi --cycles 10 --output minishogi_gaz_$i --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps; end
for i in (seq -w 0 19); python -m vshogi alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_az_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps; python -m vshogi gumbel-alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_gaz_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps; end
```

## Results

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match minishogi -p1 (ls minishogi_gaz_*/models/model_{$i}.tflite) -p2 (ls minishogi_az_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.01; end
for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_gaz_*/models/model_{$i}.tflite) -p2 (ls judkins_shogi_az_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.01; end
```

**GAZ vs AZ table**

| Generation | minishogi | judkins_shogi |
|---|---|---|
|1|1078-6-516|1186-0-414|
|2|964-0-636|1081-1-518|
|3|854-5-741|1075-3-522|
|4|791-3-806|1041-2-557|
|5|815-11-774|1063-2-535|
|6|854-4-742|993-2-605|
|7|758-3-839|938-7-655|
|8|782-0-818|1023-4-573|
|9|826-3-771|936-3-661|
|10|866-6-728|978-5-617|

## Discussion

The table shows different behaviors for the two game variants.

For **minishogi**, GAZ starts clearly ahead, then weakens, then recovers:
- Generations 1-3: clear lead (e.g., 1078-6-516 at generation 1).
- Generations 4-8: mostly near parity or behind (notably 791-3-806, 758-3-839, 782-0-818).
- Generations 9-10: advantage returns (826-3-771, 866-6-728).

Across all 10 generations, minishogi totals are **8588-41-7371**, so GAZ is better overall but by a moderate margin.

For **judkins_shogi**, GAZ is consistently stronger than AZ in every generation:
- All 10 rows have wins greater than losses, from 1186-0-414 at generation 1 to 978-5-617 at generation 10.
- The edge shrinks in later generations compared with the very strong start, but it never flips.

Across all 10 generations, judkins_shogi totals are **10314-29-5657**, indicating a large and stable overall advantage for GAZ.

Overall, the GAZ vs AZ comparison is game-dependent: near-competitive with phase-dependent swings in minishogi, and clearly favorable to GAZ in judkins_shogi.
