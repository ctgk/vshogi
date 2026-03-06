# Devlog

## Averagize replay buffer

Git hash: 706a4a3

```diff
$ git diff e85ce32
diff --git a/python/vshogi/dlshogi/_cli/_cycler.py b/python/vshogi/dlshogi/_cli/_cycler.py
index 19840dd..8285fa5 100644
--- a/python/vshogi/dlshogi/_cli/_cycler.py
+++ b/python/vshogi/dlshogi/_cli/_cycler.py
@@ -119,10 +119,7 @@ def _cycle_selfplay_and_train(**kwargs):
         return int(tflite_list[-1].split('_')[-1].split('.')[0]) + 1

     start = _resume_from()
-    buffer = ReplayBuffer(
-        buffer_size=kwargs["train_buffer_size"],
-        alpha=kwargs["train_buffer_decay"],
-    )
+    buffer = ReplayBuffer(buffer_size=kwargs["train_buffer_size"])
+++ b/python/vshogi/dlshogi/_cli/_cycler.py
@@ -119,10 +119,7 @@ def _cycle_selfplay_and_train(**kwargs):
         return int(tflite_list[-1].split('_')[-1].split('.')[0]) + 1

     start = _resume_from()
-    buffer = ReplayBuffer(
-        buffer_size=kwargs["train_buffer_size"],
-        alpha=kwargs["train_buffer_decay"],
-    )
+    buffer = ReplayBuffer(buffer_size=kwargs["train_buffer_size"])
     if start == 0:
         _train(buffer=buffer, nth_cycle=0, **kwargs)
         start += 1
diff --git a/python/vshogi/dlshogi/_cli/_nn_trainer.py b/python/vshogi/dlshogi/_cli/_nn_trainer.py
index 38512b4..1cb22cd 100644
--- a/python/vshogi/dlshogi/_cli/_nn_trainer.py
+++ b/python/vshogi/dlshogi/_cli/_nn_trainer.py
@@ -82,12 +82,6 @@ def _trainer_parameters(prefix: str = "") -> callable:
                 "samples."
             ),
         ),
-        cl.option(
-            f"--{prefix}buffer-decay",
-            default=0.9,
-            show_default=True,
-            help="Decay factor for the replay buffer.",
-        ),
         cl.option(
             f"--{prefix}per/--{prefix}no-per",
             default=False,
@@ -310,13 +304,15 @@ def _dataset(
         if (buffer._last == buffer._first) or (time() - start) > 60:
             break
     buffer._last = buffer._first
+    buffer.averagize()
     df_summary = pd.DataFrame(
         [
             {
                 'sfen': sfen,
                 'count': v,
                 'value': (
-                    2 * getattr(buffer.get_ema_of(sfen), "value01", np.nan) - 1
+                    2 * getattr(buffer.get_average_of(sfen), "value01", np.nan)
+                    - 1
                 ),
             }
             for sfen, v in count.items()
@@ -563,10 +559,7 @@ def _nn_trainer(**kwargs):
         return int(tflite_list[-1].split('_')[-1].split('.')[0]) + 1

     ii = _resume_from()
-    buffer = ReplayBuffer(
-        buffer_size=kwargs["buffer_size"],
-        alpha=kwargs["buffer_decay"],
-    )
+    buffer = ReplayBuffer(buffer_size=kwargs["buffer_size"])
     model_path = 'models/model_{:04d}.pth'
     while ii < 10000:
         _train_step(
diff --git a/python/vshogi/dlshogi/_replay_buffer.py b/python/vshogi/dlshogi/_replay_buffer.py
index aaae241..c071567 100644
--- a/python/vshogi/dlshogi/_replay_buffer.py
+++ b/python/vshogi/dlshogi/_replay_buffer.py
@@ -22,38 +22,37 @@ class ReplayBuffer(th.utils.data.Dataset):
     Examples
     --------
     >>> from vshogi.minishogi import Move; import numpy as np
-    >>> b = ReplayBuffer(buffer_size=2, alpha=0.5)
+    >>> b = ReplayBuffer(buffer_size=2)
     >>> b.add(Data('4k/5/4P/5/5 b G 1', {Move('1c1b'): 1}, 1., 1.))
     >>> len(b)  # Note that the length is doubled
     2
     >>> b.add(Data('4k/5/4P/5/5 b G 3', {}, 0.5, 0.6))
     >>> len(b)
     4
+    >>> b.averagize()
     >>> b[1][2:]  # value01, weight
     (array([0.75], dtype=float32), array(1., dtype=float32))
     """

-    def __init__(self, buffer_size: int = 100000, alpha: float = 0.9):
+    def __init__(self, buffer_size: int = 100000):
         """Initialize dataset class.

         Parameters
         ----------
         buffer_size : int
             Max size of the buffer
-        alpha : float
-            Decay rate to update duplicating data.
         """
         super().__init__()
-        self._ema: list[Data] = []
+        self._average: list[Data] = []
         self._buffer: list[Data] = []
         self._buffer_size = buffer_size
-        self._alpha: float = alpha
         self._game_variant: str | None = None

-    def _update(self, a: Data, b: Data) -> Data:
+    def _merge(self, a: Data, b: Data) -> Data:
         assert a.sfen == b.sfen
-        r: float = self._alpha  # rate of `a`
-        return Data(
+        count = getattr(a, "count", 1)
+        r: float = count / (1 + count)
+        d = Data(
             sfen=a.sfen,
             policy={}
             if (not a.policy) or (not b.policy)
@@ -63,19 +62,8 @@ class ReplayBuffer(th.utils.data.Dataset):
             },
             value01=(r * a.value01 + (1 - r) * b.value01),
         )
-
-    def _update_ema(self, data: Data) -> Data:
-        index = next(
-            (i for i, d in enumerate(self._ema) if d.sfen == data.sfen),
-            None,
-        )
-        if index is not None:
-            moving_average = self._ema.pop(index)
-            data = self._update(moving_average, data)
-        self._ema.append(data)
-        while len(self._ema) > self._buffer_size:
-            self._ema.pop(0)  # FIFO
-        return data
+        d.count = count + 1
+        return d

     def add(self, data: Data):
         """Add data to the buffer.
@@ -85,15 +73,15 @@ class ReplayBuffer(th.utils.data.Dataset):
         data : Data
             Data to add.
         """
-        data = self._update_ema(data)
+        # data = self._update_ema(data)
         self._buffer.append(data)
         if self._game_variant is None:
             self._game_variant = self._infer_game_variant(data.sfen)
         while len(self._buffer) > self._buffer_size:
             self._buffer.pop(0)  # FIFO

-    def get_ema_of(self, sfen: str) -> Data | None:
-        """Get exponential moving average data of the given SFEN game position.
+    def get_average_of(self, sfen: str) -> Data | None:
+        """Get average data of the given SFEN game position.

         Parameters
         ----------
@@ -103,10 +91,10 @@ class ReplayBuffer(th.utils.data.Dataset):
         Returns
         -------
         Data | None
-            EMA data of the game position if found, otherwise None.
+            Average data of the game position if found, otherwise None.
         """
         return next(
-            (b for b in self._ema if b.sfen == sfen),
+            (b for b in self._average if b.sfen == sfen),
             None,
         )

@@ -120,10 +108,19 @@ class ReplayBuffer(th.utils.data.Dataset):
         """
         return len(self._buffer) == self._buffer_size

-    def normalize(self) -> None:
-        """Normalize policy distributions of all data."""
+    def averagize(self) -> None:
+        """Averagize value and policy of all data."""
+        merged = {}
         for d in self._buffer:
+            if d.sfen in merged:
+                merged[d.sfen] = self._merge(merged[d.sfen], d)
+            else:
+                merged[d.sfen] = d
+        for d in merged.values():
             d.policy = _normalize(d.policy)
+        self._average = []
+        for d in self._buffer:
+            self._average.append(merged[d.sfen])

     def __len__(self):
         """Return the length of the dataset."""
@@ -149,20 +146,20 @@ class ReplayBuffer(th.utils.data.Dataset):
         """
         if self._game_variant is None:
             raise ValueError("Please add data before trying to get items.")
-        ii = index % len(self._buffer)
-        g = eval(self._game_variant)(self._buffer[ii].sfen)
-        policy = self._buffer[ii].policy
-        if index >= len(self._buffer):
+        ii = index % len(self._average)
+        g = eval(self._game_variant)(self._average[ii].sfen)
+        policy = self._average[ii].policy
+        if index >= len(self._average):
             g = g.hflip()
             policy = {m.hflip(): v for m, v in policy.items()}
         x = g.to_dlshogi_features().squeeze()
         try:
             policy = g.to_dlshogi_policy(policy, default_value=-100000.0)
         except ZeroDivisionError:
-            msg = f"Invalid policy ({policy}) at: {self._buffer[ii].sfen}"
+            msg = f"Invalid policy ({policy}) at: {self._average[ii].sfen}"
             raise ZeroDivisionError(msg)
-        value01 = np.array([np.float32(self._buffer[ii].value01)])
-        w = np.array(np.float32(self._buffer[ii].weight))
+        value01 = np.array([np.float32(self._average[ii].value01)])
+        w = np.array(np.float32(self._average[ii].weight))
         return x.squeeze(), policy.squeeze(), value01, w

     def _infer_game_variant(self, sfen: str) -> str:
```

### Training

Command:

```shell
# Git hash: e85ce32
python -m vshogi dlshogi cycler minishogi --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 --train-win-ratio-threshold 0.01 --train-device mps --train-buffer-size 10000 --train-discount-factor 1.0 --train-importance-decay 1.0 --train-result-backup-rate 0.1 --train-buffer-decay 0.0 --output minishogi_d0.0_00
for i in (seq -w 0 19); for r in 0.0 0.1 0.2; python -m vshogi dlshogi cycler minishogi \
                  --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 \
                  --train-win-ratio-threshold 0.01 --train-device mps \
                  --train-buffer-size 10000 \
                  --train-discount-factor 1.0 \
                  --train-importance-decay 1.0 \
                  --train-result-backup-rate $r \
                  --output minishogi_r{$r}_{$i}; end; end;

# Git hash: 706a4a3
for i in (seq -w 0 19); python -m vshogi dlshogi cycler minishogi \
    --cycles 10 --play-jobs 5 --play-num-games 100 --play-num-simulations 50 \
    --train-win-ratio-threshold 0.01 --train-device mps \
    --train-buffer-size 10000 \
    --train-discount-factor 1.0 \
    --train-importance-decay 1.0 \
    --train-result-backup-rate 0.1 \
    --output minishogi_averagize_{$i}; end;
```

### Result

Command:

```shell
for p1 in r0.1 averagize_r0.1 r0.2 averagize_r0.2; for p2 in r0.1 averagize_r0.1 r0.2 averagize_r0.2; python -m vshogi match minishogi -p1 (ls minishogi_{
$p1}_*/models/model_0010.tflite) -p2 (ls minishogi_{$p2}_*/models/model_0010.tflite) --num-games-each 1 --az-search-second 0.005; end; end
```

||r=0.1|averagize_r=0.1|r=0.2|averagize_r=0.2|
|---|---|---|---|---|
|r=0.1|(348-89-363)|**374-74-352**|366-76-358|319-105-376|
|averagize_r=0.1|323-85-392|(350-73-377)|354-62-384|309-103-388|
|r=0.2|353-89-358|389-68-343|(361-68-371)|311-86-403|
|**averagize_r=0.2**|**373-95-332**|**400-85-315**|**396-87-317**|(320-123-357)|

Although `r=0.1` is better than `averagize_r=0.1`, `averagize_r=0.2` is better than `r=0.2.

- Low r: Clean MCTS Q-values → averaging hurts
- High r: Noisy result labels → averaging helps by reducing variance

### What is best result backup rate when there is averaging?

Command:

```shell
for p1 in r0.1 averagize_r0.2 averagize_r0.3 averagize_r0.4; for p2 in r0.1 averagize_r0.2 averagize_r0.3 averagize_r0.4; python -m vshogi match minishogi -p1 (ls minishogi_{$p1}_*/models/model_0010.tflite) -p2 (ls minishogi_{$p2}_*/models/model_0010.tflite) --num-games-each 1 --az-search-second 0.005; end; end
```

||r=0.1|averagize_r=0.2|averagize_r=0.3|averagize_r=0.4|
|---|---|---|---|---|
|r=0.1|(353-94-353)|317-111-
372|345-70-385|396-46-358|
|averagize_r=0.2|377-107-316|(336-123-341)|347-84-369|431-41-328|
|averagize_r=0.3|395-69-336|345-83-372|(379-56-365)|396-26-378|
|averagize_r=0.4|386-43-371|333-49-418|348-26-426|(373-9-418)|

The sweet spot appears to be around r=0.3 because:

- It's high enough that averagization meaningfully reduces noise in result labels
- It's not so high that you lose the benefit of MCTS Q-values entirely
- It balances signal (MCTS) and variance reduction (averaging)
