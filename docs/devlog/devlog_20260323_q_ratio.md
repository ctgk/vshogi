# Devlog: `q_ratio`

Git hash:

https://lczero.org/blog/2018/10/understanding-training-against-q-as/

Training target of the value network is:

- `q_ratio = 1.0`: MCTS Q-value.
- `q_ratio = 0.0`: Game outcome.

## Training

```shell
for i in (seq -w 0 19); for q in 0.0 0.5 0.7 0.9 1.0; python -m vshogi alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_az_q{$q}_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-loss-q-ratio {$q}; python -m vshogi gumbel-alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_gaz_q{$q}_{$i} --play-jobs 10 --play-num-games 100 --play-num-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-loss-q-ratio {$q}; end; end
```

## Results

### GumbelAlphaZero vs AlphaZero with various `q_ratio`

```shell
for q in 0.0 0.5 0.7 0.9 1.0; python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_gaz_q{$q}_*/models/model_0010.tflite) -p2 (ls judkins_shogi_az_q{$q}_*/models/model_0010.tflite) --num-games-each 2 --az-search-second 0.01; end
```

WDL table of GumbelAlphaZero vs AlphaZero

|`q_ratio`|WDL|
|---|---|
|0.0|901-4-695|
|0.5|775-5-820|
|0.7|638-4-958|
|0.9|666-9-925|
|1.0|543-4-1053|

### Comparison against the original GumbelAlphaZero (`q_ratio = 1.0`)

```shell
for c in az_q0.0 az_q0.5 az_q0.7 az_q0.9 az_q1.0 gaz_q0.0 gaz_q0.5 gaz_q0.7 gaz_q0.9; python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_{$c}_*/models/model_0010.tflite) -p2 (ls judkins_shogi_gaz_q1.0_*/models/model_0010.tflite) --num-games-each 1 --az-search-second 0.005; end
```

WDL table vs GumbelAlphaZero (`q_ratio = 1.0`)

|Algorithm|`q_ratio`|WDL|
|---|---|---|
|A0|0.0|212-1-587|
|A0|0.5|496-1-303|
|A0|0.7|534-5-261|
|A0|0.9|539-4-257|
|A0|1.0|553-3-244|
|GA0|0.0|265-3-532|
|GA0|0.5|399-2-399|
|GA0|0.7|450-1-349|
|GA0|0.9|433-7-360|

## Discussion

The results show a clear `q_ratio` sensitivity difference between GumbelAlphaZero (GA0) and AlphaZero (A0).

- In GA0 vs A0 with matched `q_ratio`, GA0 is strongest at `q_ratio=0.0` (`901-4-695`), but its advantage steadily declines and reverses as `q_ratio` increases (`543-4-1053` at `q_ratio=1.0`).
- Against the GA0(`q_ratio=1.0`) baseline, both algorithms generally improve with larger `q_ratio`, and A0 with `q_ratio=0.7-1.0` performs especially well.

Interpretation:

- Low `q_ratio` (outcome-heavy target) limits bootstrap bias and preserves GA0's search-improvement advantage.
- High `q_ratio` (search-Q-heavy target) increases exposure to search-target bias; this appears to hurt GA0 more than A0 in this setup.
- A likely reason is visit allocation: GA0 evaluates all Gumbel-sampled actions more evenly, so many weak actions still receive substantial updates and influence aggregated search Q, while A0 concentrates visits on favorable actions and down-weights weak ones.
- This is consistent with a bias loop: biased search Q -> biased value target -> biased value net -> degraded future search Q.

|AlphaZero|GumbelAlphaZero|
|---|---|
|concentrates visits on favorable actions|evaluates all Gumbel-sampled actions evenly|
|Less biased Q-value|More biased Q-value|
|Higher advantage from high `q_ratio`|Lower advantage from high `q_ratio`|

Relation to prior devlog:

- The previous cross-generation result in [devlog_20260322_gumbel_alpha_zero.md](docs/devlog/devlog_20260322_gumbel_alpha_zero.md) still supports GA0 superiority at `q_ratio=0.0`.
- The current experiment adds that this advantage can shrink or flip when `q_ratio` is increased.

Practical takeaway:

- For `judkins_shogi` at model `0010`, GA0 likely needs a lower `q_ratio` (or stronger stabilization) than A0.

Recommended follow-ups:

1. Impact of averaging game outcome

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls q-ratio/judkins_shogi_az_q1.0_*/models/model_{$i}.tflite) -p2 (ls as-is-vs-averaging/judkins_shogi_az_averaging_*/models/model_{$i}.tflite) --num-games-each 1 --az-search-second 0.005; end
```

|Cycle|AZ_q1.0 vs AZ_q0.0_averaging|
|---|---|
|1|547-1-252|
|2|575-1-224|
|3|600-0-200|
|4|613-2-185|
|5|642-1-157|
|6|674-1-125|
|7|651-5-144|
|8|657-2-141|
|9|653-1-146|
|10|682-0-118|

`q_ratio = 1.0` is consistently outperforming value target averaging.
