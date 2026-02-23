# Devlog 2026-02-23

Git hash: bf289a4

## AlphaZero Result backup strategy

ref:

### Training

```shell
for i in (seq -w 0 19); for s in always best; for r in 0.0 0.1 0.2; python -m vshogi dlshogi cycler minishogi \
                                        --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 \
                                        --train-win-ratio-threshold 0.01 --train-device mps \
                                        --train-buffer-size 10000 \
                                        --train-buffer-decay 0.0 --train-discount-factor 1.0 \
                                        --train-importance-decay 1.0 --train-backup-result $s \
                                        --train-result-backup-rate $r \
                                        --output minishogi_{$s}_r{$r}_{$i}; end; end; end
```

### Test

```shell
python -m vshogi match minishogi -p1 (bash -c "ls minishogi_always_r0.0_??/models/model_0010.tflite") -p2 (bash -c "ls minishogi_best_r0.0_??/models/model_0010.tflite") --num-games-each 1 --az-search-second 0.01
```

### Result

| r | always vs best |
| --- | --- |
| r = 0.0 | 378-45-377 |
| r = 0.1 | **436**-33-331 |
| r = 0.2 | 371-39-**390** |
| r = 0.3 | **397**-46-357 |

### Conclusion

Backup strategy seems to be less important than choice of backup rate.
Set default strategy "always" for simplicity.
