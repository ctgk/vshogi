# Devlog: GumbelAlphaZero

## Training

```shell
for i in (seq -w 0 19); python -m vshogi alpha-zero cycler minishogi --cycles 10 --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps --output minishogi_az_$i; python -m vshogi gumbel-alpha-zero cycler minishogi --cycles 10 --output minishogi_gaz_$i --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps; end
<<<<<<< HEAD
for i in (seq -w 0 19); python -m vshogi alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_az_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps; python -m vshogi gumbel-alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_gaz_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps; end
=======
>>>>>>> cc26b83 (ADD: some notes on AlphaZero vs GumbelAlphaZero)
```

## Results

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match minishogi -p1 (ls minishogi_gaz_*/models/model_{$i}.tflite) -p2 (ls minishogi_az_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.01; end
<<<<<<< HEAD
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
=======
```

| Generation | GAZ vs AZ |
|---|---|
|1|1078-6-516|
|2|964-0-636|
|3|854-5-741|
|4|791-3-806|
|5|815-11-774|
|6|854-4-742|
|7|758-3-839|
|8|782-0-818|
|9|826-3-771|
|10|866-6-728|

## Discussion

The results show an interesting non-monotonic pattern in the performance comparison between Gumbel AlphaZero (GAZ) and standard AlphaZero (AZ) across 10 training generations.

**Early Advantage (Generations 1-3):** GAZ demonstrates a strong early advantage, with win rates significantly exceeding AZ. Generation 1 shows the largest margin with 1078 wins versus 516 losses (a 2.08:1 win-to-loss ratio). This advantage persists through generation 3, suggesting that the Gumbel sampling strategy provides benefits in the initial phases of learning.

**Mid-Game Deterioration (Generations 4-8):** The advantage erodes substantially in the middle generations. Generations 4, 7, and 8 show GAZ losing more games than winning, with generation 8 achieving only 782 wins against 818 losses. This reversal suggests that either:
- AZ's training dynamics become more favorable as both algorithms train longer
- GAZ may suffer from reduced exploration effectiveness at deeper training depths
- The relative benefit of Gumbel sampling diminishes as action value estimates mature

**Recovery and Stabilization (Generations 9-10):** The trend reverses again in the final generations, with GAZ recovering to a slight advantage (826-3-771 in gen 9, 866-6-728 in gen 10). This recovery suggests that the longer-term training dynamics may eventually favor GAZ's approach, possibly due to better final model quality.

**Overall Assessment:** The data indicates that Gumbel AlphaZero performs comparably to standard AlphaZero on minishogi, with no clear overall superiority (approximately equal win-loss balance across all 10 generations). The non-monotonic pattern warrants further investigation into the training dynamics and suggests that algorithmic advantages may be highly dependent on training phase and game domain characteristics.
>>>>>>> cc26b83 (ADD: some notes on AlphaZero vs GumbelAlphaZero)
