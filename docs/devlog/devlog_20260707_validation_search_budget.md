# Devlog: 2026/07/07 validation search budget

```diff
$ git diff .
diff --git a/python/vshogi/dlshogi/_alpha_zero/_self_play_worker.py b/python/vshogi/dlshogi/_alpha_zero/_self_play_worker.py
index 51bd6ac8..17ff0dee 100644
--- a/python/vshogi/dlshogi/_alpha_zero/_self_play_worker.py
+++ b/python/vshogi/dlshogi/_alpha_zero/_self_play_worker.py
@@ -159,6 +159,7 @@ class _SelfPlayWorker:
                     game_class(),
                     player_latest,
                     player_prev,
+                    search_args={"budget": self._simulations_original},
                     draw_on_max_moves=True,
                 ).result
                 record += vs.Record.from_black_result(result)
@@ -167,6 +168,7 @@ class _SelfPlayWorker:
                     game_class(),
                     player_prev,
                     player_latest,
+                    search_args={"budget": self._simulations_original},
                     draw_on_max_moves=True,
                 ).result
                 record += vs.Record.from_white_result(result)
```

## Training

```shell
python /Users/ctgk/program/vshogi/python/vshogi/__main__.py alpha-zero cycler minishogi --cycles 10 --output minishogi_develop_00 --play-jobs 10 --play-num-games 100 --play-simulations 500 --train-validation-threshold 0.01 --train-device mps
```

## Result

```shell
for i in (seq -f "%04g" 1 10); python -m vshogi match minishogi -p1 (ls minishogi_develop_*/models/model_{$i}.tflite) -p2 (ls minishogi_val500_*/models/model_{$i}.tflite) --num-games-each 2 --az-search-second 0.005 --show-outer-pbar; end
```

Validation with 100 searches vs. Validation with 500 searches

| Model | W | D | L | Record |
|---|---:|---:|---:|---|
| model_0001 | 852 | 11 | 737 | 852W-11D-737L |
| model_0002 | 827 | 60 | 713 | 827W-60D-713L |
| model_0003 | 797 | 67 | 736 | 797W-67D-736L |
| model_0004 | 778 | 100 | 722 | 778W-100D-722L |
| model_0005 | 756 | 101 | 743 | 756W-101D-743L |
| model_0006 | 704 | 168 | 728 | 704W-168D-728L |
| model_0007 | 697 | 144 | 759 | 697W-144D-759L |
| model_0008 | 684 | 102 | 814 | 684W-102D-814L |
| model_0009 | 758 | 71 | 771 | 758W-71D-771L |
| model_0010 | 729 | 70 | 801 | 729W-70D-801L |
