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


if __name__ == '__main__':
    pytest.main([__file__])
