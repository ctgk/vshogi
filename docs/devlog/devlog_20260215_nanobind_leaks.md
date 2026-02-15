# Devlog 2026-02-11

## nanobind reference leaks

https://github.com/pytest-dev/pytest/issues/11773

### Script

```python
# test_to_eng.py
import pytest

import vshogi.shogi as shogi


@pytest.mark.parametrize(
    'm, g, expect',
    [
        (
            shogi.Move('4i5h'),
            shogi.Game(),
            'G49-58',
        ),
    ],
)
def test_to_eng_move_parametrized(m, g, expect):
    actual = g.to_eng(m)
    assert expect == actual


def test_to_eng_move_single():
    g = shogi.Game()
    m = shogi.Move("4i5h")
    expect = 'G49-58'
    actual = g.to_eng(m)
    assert expect == actual


if __name__ == '__main__':
    pytest.main([__file__])
```

### Run `test_to_eng_move_single`

```
$ pytest python/tests/test_vshogi/test_shogi/test_to_eng.py::test_to_eng_move_single
===================================================================== test session starts ======================================================================
platform darwin -- Python 3.12.12, pytest-8.4.2, pluggy-1.6.0
rootdir: /Users/ctgk/program/vshogi
configfile: pyproject.toml
plugins: anyio-4.12.0, xdist-3.8.0, json-report-1.5.0, metadata-3.1.1, rerunfailures-15.1, kgb-7.3, hydra-core-1.3.2, hypothesis-6.148.8
collected 1 item

python/tests/test_vshogi/test_shogi/test_to_eng.py .                                                                                                     [100%]

====================================================================== 1 passed in 0.67s =======================================================================
```

### Run `test_to_eng_move_parametrized`

```
$ pytest python/tests/test_vshogi/test_shogi/test_to_eng.py::test_to_eng_move_parametrized
===================================================================== test session starts ======================================================================
platform darwin -- Python 3.12.12, pytest-8.4.2, pluggy-1.6.0
rootdir: /Users/ctgk/program/vshogi
configfile: pyproject.toml
plugins: anyio-4.12.0, xdist-3.8.0, json-report-1.5.0, metadata-3.1.1, rerunfailures-15.1, kgb-7.3, hydra-core-1.3.2, hypothesis-6.148.8
collected 1 item

python/tests/test_vshogi/test_shogi/test_to_eng.py .                                                                                                     [100%]

====================================================================== 1 passed in 0.68s =======================================================================
nanobind: leaked 2 instances!
nanobind: leaked 4 types!
 - leaked type "vshogi.shogi._shogi.Move"
 - leaked type "vshogi.shogi._shogi._Game"
 - leaked type "Color"
 - leaked type "Result"
nanobind: leaked 53 functions!
 - leaked function "ply"
 - leaked function "in_check"
 - leaked function "get_check_moves"
 - leaked function "get_zobrist_hash"
 - leaked function "is_valid_piece_count"
 - leaked function "get_attention"
 - leaked function "get_result"
 - leaked function "_num_policy_per_square"
 - leaked function "copy"
 - leaked function "to_sfen"
 - ... skipped remainder
nanobind: this is likely caused by a reference counting issue in the binding code.
See https://nanobind.readthedocs.io/en/latest/refleaks.html
```
