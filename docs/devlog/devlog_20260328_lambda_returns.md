# Devlog: $\lambda$-returns

Git hash: c6e7731

## Training

```shell
for i in (seq -w 0 19); for l in 0.0 0.4 0.5 0.6 0.7 0.8 0.99; python -m vshogi alpha-zero cycler judkins_shogi --cycles 10 --output judkins_shogi_lambda{$l}_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-loss-lambda $l; end; end
```

## Results

```shell
for l in 0.4 0.5 0.6 0.7 0.8 0.99; for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_lambda0.0_*/models/model_{$i}.tflite) -p2 (ls judkins_shogi_lambda{$l}_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005; end; end
```

In this table, wins (W) are counted for the left player, i.e. $\lambda=0.0$ (the previous default).

|$\lambda=0.0$ vs|$\lambda=0.4$|$\lambda=0.5$|$\lambda=0.6$|$\lambda=0.7$|$\lambda=0.8$|$\lambda=0.99$|
|---|---|---|---|---|---|---|
|Cycle 1|394W-2D-404L|417W-0D-383L|392W-2D-406L|388W-1D-411L|406W-1D-393L|410W-1D-389L|
|Cycle 2|359W-2D-439L|368W-2D-430L|355W-1D-444L|312W-0D-488L|355W-4D-441L|376W-4D-420L|
|Cycle 3|368W-2D-430L|374W-1D-425L|339W-2D-459L|328W-3D-469L|346W-2D-452L|385W-1D-414L|
|Cycle 4|393W-1D-406L|357W-3D-440L|376W-1D-423L|376W-0D-424L|388W-4D-408L|426W-2D-372L|
|Cycle 5|398W-6D-396L|356W-4D-440L|387W-4D-409L|395W-7D-398L|399W-5D-396L|437W-1D-362L|
|Cycle 6|387W-5D-408L|381W-10D-409L|356W-8D-436L|410W-7D-383L|375W-16D-409L|447W-6D-347L|
|Cycle 7|366W-6D-428L|345W-9D-446L|378W-8D-414L|388W-7D-405L|360W-4D-436L|481W-3D-316L|
|Cycle 8|325W-7D-468L|347W-5D-448L|364W-4D-432L|382W-6D-412L|334W-1D-465L|477W-2D-321L|
|Cycle 9|339W-6D-455L|354W-5D-441L|311W-4D-485L|344W-4D-452L|354W-7D-439L|437W-2D-361L|
|Cycle 10|377W-4D-419L|348W-8D-444L|351W-5D-444L|354W-7D-439L|382W-6D-412L|462W-4D-334L|

## Discussion

Across 8,000 games per setting, $\lambda\in\{0.4,0.5,0.6,0.7,0.8\}$ consistently outperforms the previous default $\lambda=0.0$.
In these matchups, the win rate of $\lambda=0.0$ (left player) is:

- vs $\lambda=0.4$: 46.6% (3706W-41D-4253L)
- vs $\lambda=0.5$: 45.9% (3647W-47D-4306L)
- vs $\lambda=0.6$: 45.3% (3609W-39D-4352L)
- vs $\lambda=0.7$: 46.2% (3677W-42D-4281L)
- vs $\lambda=0.8$: 46.5% (3699W-50D-4251L)

So a moderate $\lambda$ improves strength by roughly 3.5-4.7 percentage points in non-draw win rate relative to $\lambda=0.0$, with $\lambda=0.6$ being the best in this sweep.

From a statistical perspective, this effect is large relative to sampling noise. With about $n\approx 8{,}000$ games per setting, the standard error of a 50% win rate is

$$
\mathrm{SE}\approx\sqrt{\frac{0.25}{8000}}\approx 0.56\%.
$$

The observed deficits of $\lambda=0.0$ versus moderate $\lambda$ ($3.4\%-4.7\%$ below 50%) correspond to roughly $6$-$8\,\mathrm{SE}$ from parity, i.e. well beyond normal run-to-run fluctuation.
As a concrete example, against $\lambda=0.6$, $\lambda=0.0$ scores 45.3%, whose 95% confidence interval is approximately 44.2%-46.4%, still clearly below 50%.

In contrast, $\lambda=0.99$ is clearly harmful: $\lambda=0.0$ scores 54.4% (4338W-26D-3636L), so near-terminal-only backup is too extreme in this setup.

One plausible interpretation is:

- $\lambda=0.0$ relies too much on one-step value targets and keeps high bootstrap bias.
- Moderate $\lambda$ reduces this bias while preserving useful intermediate value information.
- Very large $\lambda$ over-emphasizes distant/terminal outcomes, increasing variance and weakening training signal quality.

Given these results, the default should be moved from $\lambda=0.0$ to a moderate value, with $\lambda=0.6$ as the first candidate.
