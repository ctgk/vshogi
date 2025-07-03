import tempfile

import ai_edge_torch
import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._residual_block import _ResidualBlock
from vshogi.minishogi import Game


def test_export_residual_block_to_tflite():
    attentions = Game.get_local_attentions()
    module = _ResidualBlock(32, 8, attentions, attention_groups=8)
    sample_inputs = (th.randn(1, 32, 5 * 5),)
    module(*sample_inputs)
    edge_model = ai_edge_torch.convert(module.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        interpreter = Interpreter(model_path=t.name)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    assert tuple(input_details['shape']) == (1, 32, 5 * 5)
    assert tuple(output_details['shape']) == (1, 32, 5 * 5)


if __name__ == '__main__':
    pytest.main([__file__])
