import tempfile

import ai_edge_torch
import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._network import PolicyValueNetwork
from vshogi.minishogi import Game


def test_export_pvnet_to_tflite():
    module = PolicyValueNetwork(Game, 32, 8, 3)
    sample_inputs = (th.randn(1, 5, 5, Game.feature_channels),)
    edge_model = ai_edge_torch.convert(module.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        interpreter = Interpreter(model_path=t.name)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()
    assert tuple(input_details['shape']) == (1, 5, 5, Game.feature_channels)
    i = 0 if (output_details[0]['shape'][-1] == 1) else -1
    assert tuple(output_details[i]['shape']) == (1, 1)
    assert tuple(output_details[i + 1]['shape']) == (
        1, 5 * 5 * Game._get_move_class()._num_policy_per_square())


def test_pvnet_backward():
    model = PolicyValueNetwork(Game, 32, 8, 1)
    x = th.tensor(
        th.randn(2, 5, 5, Game.feature_channels),
        dtype=th.float32, requires_grad=True,
    )
    p, v = model(x)
    loss = th.sum(th.square(p - 1)) + th.sum(th.square(v - 1))
    loss.backward()


def test_pvnet_backward_mps():
    if not th.backends.mps.is_available():
        return
    model = PolicyValueNetwork(Game, 32, 8, 1)
    model.to('mps')
    x = th.tensor(
        th.randn(2, 5, 5, Game.feature_channels),
        dtype=th.float32,
        device='mps',
        requires_grad=True,
    )
    p, v = model(x)
    loss = th.sum(th.square(p - 1)) + th.sum(th.square(v - 1))
    loss.backward()


if __name__ == '__main__':
    pytest.main([__file__])
