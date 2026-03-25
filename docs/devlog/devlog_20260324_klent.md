# Devlog: KLENT

Git hash: 119b2e3

## Training: 1000 evals per cycle

```shell
for i in (seq -w 0 19); python -m vshogi alpha-zero cycler minishogi --cycles 5 --output minishogi_az_{$i} --play-jobs 10 --play-num-games 10 --play-simulations 100 --train-validation-threshold 0.01 --train-device mps; end
for i in (seq -w 0 19); for t in as-is aggregate; python -m vshogi klent cycler minishogi --cycles 5 --output minishogi_klent_{$t}_{$i} --play-jobs 10 --play-num-games 1000 --train-validation-threshold 0.01 --train-device mps --train-loss-value-target $t; end; end
```

## Results: 1000 evals per cycle

```shell
for t in as-is aggregate; for i in (seq -f "%04g" 1 5); python -m vshogi match minishogi -p1 (ls minishogi_az_*/models/model_{$i}.tflite) -p2 (ls minishogi_klent_{$t}_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005; end; end
for i in (seq -f "%04g" 1 5); python -m vshogi match minishogi -p1 (ls minishogi_klent_as-is_*/models/model_{$i}.tflite) -p2 (ls minishogi_klent_aggregate_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005; end
```

|Cycle|AlphaZero vs KLENT_as-is|AlphaZero vs KLENT_aggregate|KLENT_as-is vs KLENT_aggregate|
|---|---|---|---|
|1|75-4-1521|67-3-1530|773-5-822|
|2|263-6-1331|302-8-1290|867-37-696|
|3|220-9-1371|348-6-1246|914-109-577|
|4|292-7-1301|304-10-1286|870-74-656|
|5|358-12-1230|490-8-1102|856-106-638|

## Training: 10000 evals per cycle

```shell
for i in (seq -w 0 19); python -m vshogi alpha-zero cycler minishogi --cycles 5 --output minishogi_az_10000evals_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 100 --train-validation-threshold 0.01 --train-device mps; python -m vshogi klent cycler minishogi --cycles 5 --output minishogi_klent_10000evals_{$i} --play-jobs 10 --play-num-games 10000 --train-validation-threshold 0.01 --train-device mps; end
```

## Results: 10000 evals per cycle

```shell
for i in (seq -f "%04g" 1 5); python -m vshogi match minishogi -p1 (ls minishogi_az_10000evals_*/models/model_{$i}.tflite) -p2 (ls minishogi_klent_10000evals_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005; end
```

|Cycle|AlphaZero vs KLENT|
|---|---|
|1|281-10-1309|
|2|944-20-636|
|3|1044-11-545|
|4|1145-54-401|
|5|1201-47-352|

## Discussion

The results suggest that KLENT is promising in a narrow sense, but not yet as a general replacement for AlphaZero.

At `1000 evals` per cycle, both KLENT variants outperform AlphaZero in all 5 cycles.

- `KLENT_as-is` vs AlphaZero totals: `6754-38-1208`
- `KLENT_aggregate` vs AlphaZero totals: `6454-35-1511`

This is a large margin, so KLENT appears to have a clear early-training or low-compute advantage on `minishogi`. In other words, with limited search/evaluation budget, KLENT is learning useful policies faster than AlphaZero.

However, the value-target aggregation does not improve the method overall.

- Head-to-head totals are `4280-331-3389` in favor of `KLENT_as-is`.
- `KLENT_aggregate` is slightly better only at cycle 1 (`822-5-773`), then loses in cycles 2-5.

This matters because it suggests that simple variance reduction is not the main issue. If the per-sample KLENT value targets are already biased, averaging repeated targets from the same position will preserve that bias while only shrinking variance around it. The cycle-1 gain of `aggregate` is consistent with a small early variance benefit, but the later regression suggests that target bias dominates once training proceeds.

The `10000 evals` experiment further weakens the case for KLENT as a robust long-run algorithm. KLENT wins cycle 1 (`1309-10-281`), but then AlphaZero overtakes it immediately and the gap widens every cycle:

- cycle 2: AlphaZero leads `944-20-636`
- cycle 3: AlphaZero leads `1044-11-545`
- cycle 4: AlphaZero leads `1145-54-401`
- cycle 5: AlphaZero leads `1201-47-352`

So the current picture is:

- KLENT is strong at the beginning of training.
- KLENT is strong when the evaluation budget per cycle is small.
- KLENT does not scale as well as AlphaZero when much more self-play data is added.
- Aggregating same-position move values does not solve the late-training weakness.

My current interpretation is that KLENT has a genuine sample-efficiency advantage, but also a stability or asymptotic-strength problem. That makes it promising as a low-budget learner, but not yet promising as the main training algorithm. If the goal is to replace AlphaZero rather than complement it, these results are not sufficient.

The most likely failure mode is a bias loop in the KLENT value targets. Early in training, the regularized policy-improvement rule seems to produce better actions quickly. But once KLENT starts training on its own generated move values at larger scale, any systematic error in those targets can be reinforced by later policy updates. The failure of `aggregate` supports this interpretation: averaging can reduce noise, but it cannot remove self-generated target bias.

Practical takeaway:

- KLENT is worth continuing to study.
- The present version is promising for early learning and low-compute regimes.
- The present version is not yet convincing as a stable high-data alternative to AlphaZero.

Recommended follow-ups:

1. Sweep `coeff_kl` and `coeff_entropy` in the `10000 evals` setting to see whether the late-cycle collapse is primarily a regularization issue.
2. Mix KLENT targets with a less biased return target, instead of only averaging KLENT-generated move values.
3. Run longer head-to-head curves to test whether KLENT is best used only for warm-starting and then handed off to AlphaZero.
