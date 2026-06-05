## Results

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls develop/judkins_shogi_develop_*/models/model_{$i}.tflite) -p2 (ls epsilon0.1/judkins_shogi_epsilon0.1_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005 --show-outer-pbar; end
```

p1: $Trained with \varepsilon=0.25, Played with \varepsilon=0.25$
p2: $Trained with \varepsilon=0.10, Played with \varepsilon=0.25$

| Generation | p1 wins | Draws | p2 wins | Total | p1 win % | Draw % | p2 win % |
|---|---:|---:|---:|---:|---:|---:|---:|
| 1 | 779 | 1 | 820 | 1600 | 48.7 | 0.1 | 51.3 |
| 2 | 710 | 2 | 888 | 1600 | 44.4 | 0.1 | 55.5 |
| 3 | 726 | 0 | 874 | 1600 | 45.4 | 0.0 | 54.6 |
| 4 | 747 | 6 | 847 | 1600 | 46.7 | 0.4 | 52.9 |
| 5 | 690 | 9 | 901 | 1600 | 43.1 | 0.6 | 56.3 |
| 6 | 798 | 22 | 780 | 1600 | 49.9 | 1.4 | 48.8 |
| 7 | 698 | 10 | 892 | 1600 | 43.6 | 0.6 | 55.8 |
| 8 | 657 | 19 | 924 | 1600 | 41.1 | 1.2 | 57.8 |
| 9 | 722 | 23 | 855 | 1600 | 45.1 | 1.4 | 53.4 |
| 10 | 646 | 19 | 935 | 1600 | 40.4 | 1.2 | 58.4 |
| **Total** | **7173** | **111** | **8716** | **16000** | **44.8** | **0.7** | **54.5** |

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match judkins_shogi -p1 (ls develop/judkins_shogi_develop_*/models/model_{$i}.tflite) -p2 (ls epsilon0.1/judkins_shogi_epsilon0.1_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005 --az-epsilon-greedy 0.25 0.1 --show-outer-pbar; end
```

p1: $Trained with \varepsilon=0.25, Played with \varepsilon=0.25$
p2: $Trained with \varepsilon=0.10, Played with \varepsilon=0.10$

| Generation | p1 wins | Draws | p2 wins | Total | p1 win % | Draw % | p2 win % |
|---|---:|---:|---:|---:|---:|---:|---:|
| 1 | 776 | 3 | 821 | 1600 | 48.5 | 0.2 | 51.3 |
| 2 | 727 | 3 | 870 | 1600 | 45.4 | 0.2 | 54.4 |
| 3 | 694 | 2 | 904 | 1600 | 43.4 | 0.1 | 56.5 |
| 4 | 684 | 7 | 909 | 1600 | 42.8 | 0.4 | 56.8 |
| 5 | 687 | 9 | 904 | 1600 | 42.9 | 0.6 | 56.5 |
| 6 | 706 | 22 | 872 | 1600 | 44.1 | 1.4 | 54.5 |
| 7 | 635 | 6 | 959 | 1600 | 39.7 | 0.4 | 59.9 |
| 8 | 643 | 13 | 944 | 1600 | 40.2 | 0.8 | 58.9 |
| 9 | 676 | 19 | 905 | 1600 | 42.3 | 1.2 | 56.6 |
| 10 | 614 | 19 | 967 | 1600 | 38.4 | 1.2 | 60.4 |
| **Total** | **6842** | **103** | **9055** | **16000** | **42.8** | **0.6** | **56.6** |
