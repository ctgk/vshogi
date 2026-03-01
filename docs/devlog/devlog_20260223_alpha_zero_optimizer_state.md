# Devlog

## Resume or Reset Optimizer State

Git hash: c47d63a

### Experiment

```shell
for i in (seq -w 0 19); for r in 0.05 0.10 0.15 0.20; for s in resume reset; python -m vshogi dlshogi cycler minishogi \
                                    --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 \
                                    --train-win-ratio-threshold 0.01 --train-device mps \
                                    --train-buffer-size 10000 --train-{$s}-optimizer \
                                    --train-buffer-decay 0.0 --train-discount-factor 1.0 \
                                    --train-importance-decay 1.0 --train-result-backup-rate $r \
                                    --output minishogi_{$s}_r{$r}_{$i}; end; end; end
```

### Result

Command:

```shell
for r in 0.05_ 0.10_ 0.15_ 0.20_; python -m vshogi match minishogi -p1 (bash -c "ls minishogi_resume_r$r??/models/model_0010.tflite") -p2 (bash -c "ls minishogi_reset_r$r??/models/model_0010.tflite") --num-games-each 10 --az-search-second 0.01; end
```

| r | resume vs reset|
|---|---|
|0.05|3570-523-3907|
|0.10|3832-476-3692|
|0.15|3378-712-3910|
|0.20|3827-496-3677|

Resume and reset showed no meaningful difference.
