import numpy as np
import pytest
import torch as th

from vshogi.dlshogi._train import (
    masked_log_softmax,
    masked_softmax_cross_entropy,
)


@pytest.mark.parametrize(
    't, x, dx_expect',
    [
        (np.zeros(4) + 0.25, np.zeros(4), np.zeros(4)),
        (
            np.array([1, 0]),
            np.array([0, 0]),  # p=[0.5, 0.5]
            np.array([-0.5, 0.5]),
        ),
        (
            np.array([1, -1, 0, -1]),
            np.array([0, 10, 0, 10]),  # p=[0.5, nan, 0.5, nan]
            np.array([-0.5, 0, 0.5, 0]),
        ),
        (
            np.array([[0, 0, 0, 1], [1, -1, 0, -1]]),
            np.array([[0, 0, 0, 0], [0, 10, 0, 10]]),
            np.array(
                [
                    [0.25, 0.25, 0.25, -0.75],
                    [-0.5, 0, 0.5, 0],
                ]
            ),
        ),
    ],
)
def test_masked_log_softmax(t, x, dx_expect):
    t = th.tensor(t, dtype=th.float32)
    x = th.tensor(x, dtype=th.float32, requires_grad=True)
    lnp = masked_log_softmax(x, th.greater_equal(t, 0))
    t_masked = th.clamp(t, 0, 1)
    nll_for_each = th.sum(-t_masked * lnp, dim=-1)
    nll_total = th.sum(nll_for_each)
    nll_total.backward()
    print(x.grad)
    assert np.allclose(x.grad, dx_expect, rtol=0, atol=1e-2)


@pytest.mark.parametrize(
    't, x, dx_expect',
    [
        (
            np.array([[0, 0, 0, 1], [1, -1, 0, -1]]),
            np.array([[0, 0, 0, 0], [0, 10, 0, 10]]),
            np.array(
                [
                    [0.25, 0.25, 0.25, -0.75],
                    [-0.5, 0, 0.5, 0],
                ]
            ),
        ),
    ],
)
def test_masked_softmax_cross_entropy(t, x, dx_expect):
    t = th.tensor(t, dtype=th.float32)
    x = th.tensor(x, dtype=th.float32, requires_grad=True)
    nll_for_each = masked_softmax_cross_entropy(t, x)
    nll_total = th.sum(nll_for_each)
    nll_total.backward()
    print(x.grad)
    assert np.allclose(x.grad, dx_expect, rtol=0, atol=1e-2)


@pytest.mark.parametrize(
    't, x, dx_expect',
    [
        (
            np.array([[0, 0, 0, 1], [1, -1, 0, -1]]),
            np.array([[0, 0, 0, 0], [0, 10, 0, 10]]),
            np.array(
                [
                    [0.25, 0.25, 0.25, -0.75],
                    [-0.5, 0, 0.5, 0],
                ]
            ),
        ),
    ],
)
def test_masked_softmax_cross_entropy_mps(t, x, dx_expect):
    if not th.backends.mps.is_available():
        return
    t = th.tensor(t, dtype=th.float32, device='mps')
    x = th.tensor(x, dtype=th.float32, device='mps', requires_grad=True)
    nll_for_each = masked_softmax_cross_entropy(t, x)
    nll_total = th.sum(nll_for_each)
    nll_total.backward()
    print(x.grad)
    assert np.allclose(x.grad.cpu(), dx_expect, rtol=0, atol=1e-2)


if __name__ == '__main__':
    pytest.main([__file__])
