# Devlog: Loss discount factor

Git hash: b2f10fd

## Training

```shell
for i in (seq -w 0 19); for f in 1.00 0.99 0.95 0.90; python -m vshogi alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_df{$f}_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-loss-discount-factor $f; end; end
```

## Results: round robin match ups

```shell
for f1 in 1.00 0.99 0.95 0.90; for f2 in 1.00 0.99 0.95 0.90; python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_df{$f1}_*/models/model_0010.tflite) -p2 (ls judkins_shogi_df{$f2}_*/models/model_0010.tflite) --num-games-each 5 --az-search-second 0.01 --show-outer-pbar; end; end
```

WDL table

|discount_factor|1.00|0.99|0.95|0.90|
|---------------|----|----|----|----|
|1.00|789-16-795|782-8-810|701-10-889|648-4-948|
|0.99|831-7-762|799-9-792|734-6-860|763-6-831|
|0.95|893-9-698|845-10-745|781-4-815|819-4-777|
|0.90|919-6-675|855-6-739|773-6-821|813-3-784|

### Discussion

From this matrix, the strongest settings are 0.95 and 0.90.

When combining both directions of each pairing to reduce first-player bias, 0.95 has the best point estimate overall, with 0.90 as a very close second.

The gap between 0.95 and 0.90 is small and not statistically significant at the usual 5% level, so this experiment does not support a confident claim that one is better than the other.

In contrast, both 0.95 and 0.90 are clearly stronger than 0.99 and 1.00, while 0.99 vs 1.00 is also a small and not clearly significant difference.

Practical takeaway: use 0.95 (or 0.90) as the default candidate, and run a focused follow-up experiment with more games only on 0.95 vs 0.90 to decide the final default.

## Results:

```shell
for f in 1.00 0.99 0.90; for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_df0.95_*/models/model_{$i}.tflite) -p2 (ls judkins_shogi_df{$f}_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005 --show-outer-pbar; end; end
```

|generation|0.95 vs 1.00|0.95 vs 0.99|0.95 vs 0.90|
|----------|------------|------------|------------|
|1|734-4-862|823-2-775|845-1-754|
|2|759-6-835|766-4-830|766-3-831|
|3|804-2-794|813-4-783|776-2-822|
|4|835-8-757|783-6-811|789-10-801|
|5|866-12-722|767-12-821|746-9-845|
|6|826-8-766|835-12-753|773-18-809|
|7|834-16-750|824-14-762|739-19-842|
|8|811-15-774|816-15-769|804-15-781|
|9|854-17-729|789-13-798|742-14-844|
|10|886-13-701|857-8-735|798-4-798|

## Discussion

- Higher discount factor showed stronger results at 1st generation.
