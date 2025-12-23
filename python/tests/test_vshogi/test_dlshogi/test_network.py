import tempfile

import pytest
import torch as th
from executorch.backends.xnnpack.partition.xnnpack_partitioner import (
    XnnpackPartitioner,
)
from executorch.exir import to_edge_transform_and_lower
from executorch.runtime import Runtime

from vshogi.dlshogi._network._depthwise_attention import _DepthwiseAttention
from vshogi.dlshogi._network._network import PolicyValueNetwork
from vshogi.dlshogi._network._policy_head import _PolicyHead
from vshogi.dlshogi._network._residual_block import _ResidualBlock
from vshogi.dlshogi._network._value_head import _ValueHead
from vshogi.minishogi import Game


@pytest.mark.parametrize(
    ("module", "input_shape", "output_shape"),
    [
        (_ValueHead(16, (5, 5)), (1, 16, 5, 5), (1, 1)),
        (_ValueHead(32, (6, 6)), (1, 32, 6, 6), (1, 1)),
        (_PolicyHead(32, 20), (1, 32, 5, 5), (1, 5 * 5 * 20)),
        (_PolicyHead(64, 10), (1, 64, 9, 9), (1, 9 * 9 * 10)),
        (
            _DepthwiseAttention(Game.get_local_attentions(), groups=8),
            (1, 16, 5, 5),
            (1, 16, 5, 5),
        ),
        (
            _DepthwiseAttention(Game.get_local_attentions(), groups=8),
            (1, 32, 5, 5),
            (1, 32, 5, 5),
        ),
        (
            _ResidualBlock(
                32, 8, Game.get_local_attentions(), attention_groups=8
            ),
            (1, 32, 5, 5),
            (1, 32, 5, 5),
        ),
        (
            PolicyValueNetwork(Game, 32, 8, 3),
            (1, 5, 5, Game.feature_channels),
            {
                (1, 1),
                (1, 5 * 5 * Game._get_move_class()._num_policy_per_square()),
            },
        ),
    ],
)
def test_export_to_pte(module, input_shape, output_shape):
    sample_inputs = (th.randn(*input_shape),)
    exported = th.export.export(module.eval(), sample_inputs)
    program = to_edge_transform_and_lower(
        exported,
        partitioner=[XnnpackPartitioner()],
    ).to_executorch()
    with tempfile.NamedTemporaryFile(delete=True) as t:
        t.write(program.buffer)
        runtime = Runtime.get()
        method = runtime.load_program(t.name).load_method('forward')

    metadata = method.metadata
    expect_input_shape = metadata.input_tensor_meta(0).sizes()
    assert expect_input_shape == input_shape
    if isinstance(output_shape, set):
        actual = {
            metadata.output_tensor_meta(0).sizes(),
            metadata.output_tensor_meta(1).sizes(),
        }
        assert actual == output_shape
    else:
        assert metadata.output_tensor_meta(0).sizes() == output_shape


@pytest.mark.parametrize(
    ("model", "input_shape"),
    [
        (_ValueHead(16, (5, 5)), (2, 16, 5, 5)),
        (_PolicyHead(32, 20), (2, 32, 5, 5)),
        (
            _DepthwiseAttention(Game.get_local_attentions(), groups=8),
            (2, 16, 5, 5),
        ),
        (
            _ResidualBlock(
                32, 8, Game.get_local_attentions(), attention_groups=8
            ),
            (2, 32, 5, 5),
        ),
        (PolicyValueNetwork(Game, 32, 8, 1), (2, 5, 5, Game.feature_channels)),
    ],
)
def test_backward(model, input_shape: tuple):
    x = th.randn(*input_shape).requires_grad_()
    output = model(x)
    if isinstance(output, (tuple, list)):
        loss = sum(th.sum(th.square(o - 1)) for o in output)
    else:
        loss = th.sum(th.square(output - 1))
    loss.backward()


@pytest.mark.parametrize(
    ("model", "input_shape"),
    [
        (_ValueHead(16, (5, 5)), (2, 16, 5, 5)),
        (_PolicyHead(32, 20), (2, 32, 5, 5)),
        (
            _DepthwiseAttention(Game.get_local_attentions(), groups=8),
            (2, 16, 5, 5),
        ),
        (
            _ResidualBlock(
                32, 8, Game.get_local_attentions(), attention_groups=8
            ),
            (2, 32, 5, 5),
        ),
        (PolicyValueNetwork(Game, 32, 8, 1), (2, 5, 5, Game.feature_channels)),
    ],
)
def test_backward_mps(model, input_shape: tuple):
    if not th.backends.mps.is_available():
        return
    model = model.to('mps')
    x = th.randn(*input_shape).requires_grad_().to("mps")
    output = model(x)
    if isinstance(output, (tuple, list)):
        loss = sum(th.sum(th.square(o - 1)) for o in output)
    else:
        loss = th.sum(th.square(output - 1))
    loss.backward()


if __name__ == '__main__':
    pytest.main([__file__])
