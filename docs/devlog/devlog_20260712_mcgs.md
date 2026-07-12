# Devlog: 2026/07/12 Monte Carlo Graph Searcher

## Training

```shell
for i in (seq -w 0 19); for e in alpha-zero mcgs; python -m vshogi $e cycler judkins_shogi --cycles 10 --output judkins_shogi_{$e}_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 300 --train-validation-threshold 0.01 --train-device mps; end; end
```

## Results

```shell
for i in (seq -f "%04g" 2 2 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_alpha-zero_*
/models/model_{$i}.tflite) -p2 (ls judkins_shogi_mcgs_*/models/model_{$i}.tflite) --num-games-each 2 --az-
search-second 0.005 --show-outer-pbar; end
```

## AlphaZero weights vs MCGS weights (with MCTS)

| Generation | AlphaZero weights vs MCGS weights|
| --- | --- |
| 2 | 676W-4D-920L |
| 4 | 664W-7D-929L |
| 6 | 654W-9D-937L |
| 8 | 667W-0D-933L |
| 10 | 602W-2D-996L |
| Total | 3263W-22D-4715L |

## Full AlphaZero vs Full MCGS

| Generation | Full AlphaZero vs Full MCGS |
| --- | --- |
| 2 | 476W-14D-1110L |
| 4 | 595W-8D-997L |
| 6 | 631W-4D-965L |
| 8 | 621W-2D-977L |
| 10 | 581W-7D-1012L |
| Total | 2904W-35D-5061L |
