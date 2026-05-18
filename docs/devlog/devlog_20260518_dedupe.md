# Deduplication

## Training

```shell
$ for i in (seq -w 0 19); for d in true false; python -m vshogi alpha-zero cycler judkins_shogi --cycles 20 --output judkins_shogi_dedupe_{$d}_{$i} --play-jobs 10 --play-num-games 100 --play-simulations 100 --train-validation-threshold 0.01 --train-device mps --train-max-dataset-size 2000 --train-dedupe-dataset $d; end; end
```

## Results

```shell
$ for i in (seq -f "%04g" 1 20); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_dedupe_false_*/models/model_{$i}.tflite) -p2 (ls judkins_shogi_dedupe_true_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005 --show-outer-pbar; end
```

## Summary table

| Model | false wins | draws | true wins | false win % |
|---|---:|---:|---:|---:|
| 0001 | 699 | 2 | 899 | 43.69% |
| 0002 | 677 | 8 | 915 | 42.31% |
| 0003 | 683 | 5 | 912 | 42.69% |
| 0004 | 668 | 4 | 928 | 41.75% |
| 0005 | 637 | 6 | 957 | 39.81% |
| 0006 | 676 | 5 | 919 | 42.25% |
| 0007 | 693 | 11 | 896 | 43.31% |
| 0008 | 752 | 7 | 841 | 47.00% |
| 0009 | 723 | 9 | 868 | 45.19% |
| 0010 | 739 | 8 | 853 | 46.19% |
| 0011 | 839 | 5 | 756 | 52.44% |
| 0012 | 737 | 11 | 852 | 46.06% |
| 0013 | 761 | 7 | 832 | 47.56% |
| 0014 | 844 | 22 | 734 | 52.75% |
| 0015 | 751 | 7 | 842 | 46.94% |
| 0016 | 754 | 7 | 839 | 47.13% |
| 0017 | 766 | 11 | 823 | 47.88% |
| 0018 | 833 | 13 | 754 | 52.06% |
| 0019 | 781 | 16 | 803 | 48.81% |
| 0020 | 755 | 11 | 834 | 47.19% |
| **Total** | **14,768** | **175** | **17,057** | **46.15%** |

## Discussion

The aggregated results indicate that `dedupe_true` has a consistent advantage over `dedupe_false` in this matchup series. Across 20 model checkpoints, `dedupe_true` won 17,057 games compared to 14,768 for `dedupe_false`, with only 175 draws. This corresponds to `dedupe_false` winning roughly 46.2% of the decisive games, while `dedupe_true` won the remaining 53.8%.

The advantage is not uniform across all checkpoints. `dedupe_false` performs better in a subset of checkpoints, particularly early- and mid-training stages, while `dedupe_true` shows stronger performance on average and peaks in several later checkpoints. The difference appears meaningful rather than marginal, but it is also not overwhelming; the curves likely cross over several times.

Possible explanations include:

- `dedupe_true` may provide cleaner training data by removing repeated or near-duplicate positions, which can help the network generalize better.
- `dedupe_false` does not necessarily retain more useful training variety; instead, it can produce a dataset whose distribution is closer to the raw, true distribution of played positions.

This suggests the two regimes trade off different biases: `dedupe_true` biases toward a deduplicated training distribution, while `dedupe_false` preserves the original position distribution even when that distribution contains more noise or redundancy.

For this experimental setup, the evidence still supports preferring `--train-dedupe-dataset true` over `false`, but the result should be interpreted as a tradeoff between deduplicated generalization and fidelity to the raw data distribution. Continued checkpoint-level analysis is warranted.
