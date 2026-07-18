# Devlog: 2026/07/14 MCGS $\lambda$-returns

## Training

## Result

```shell
for l in 0.4 0.5 0.6 0.7 0.8; for i in (seq -f "%04g" 2 2 10); python -m vshogi match judkins_shogi -p1 (ls judkins_shogi_lambda0.6_*/models/model_{$i}.tflite) -p2 (ls judkins_shogi_lambda{$l}_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005; end; end
```

Each entry is shown as W-D-L for the λ = 0.6 player against the listed opponent λ.

| Config | Checkpoint 2 | Checkpoint 4 | Checkpoint 6 | Checkpoint 8 | Checkpoint 10 | Total |
|---|---:|---:|---:|---:|---:|---:|
| $\lambda=0.6$ vs $\lambda=0.4$ | 774-0-826 | 770-6-824 | 804-3-793 | 816-2-782 | 826-9-765 | 3990-20-3990 |
| $\lambda=0.6$ vs $\lambda=0.5$ | 799-8-793 | 799-9-792 | 871-2-727 | 763-2-835 | 781-9-810 | 4013-30-3957 |
| $\lambda=0.6$ vs $\lambda=0.6$ | 796-4-800 | 807-7-786 | 818-1-781 | 784-4-812 | 784-9-807 | 3989-25-3986 |
| $\lambda=0.6$ vs $\lambda=0.7$ | 774-7-819 | 841-7-752 | 828-6-766 | 810-5-785 | 860-2-738 | 4113-27-3860 |
| $\lambda=0.6$ vs $\lambda=0.8$ | 833-9-758 | 910-3-687 | 882-2-716 | 853-14-733 | 780-6-814 | 4258-34-3708 |

### Takeaway
- The totals are broadly similar across the different opponent λ values, so the evidence for a strong overall trend is limited.
- There is a mild tendency for the λ = 0.6 player to perform slightly better against higher-λ opponents, especially λ = 0.7 and λ = 0.8, but the difference is modest.
- The strongest single result is λ = 0.6 vs λ = 0.8 at checkpoint 4, where the λ = 0.6 player achieved a 910-3-687 outcome, but the pattern is not consistent across all checkpoints.
