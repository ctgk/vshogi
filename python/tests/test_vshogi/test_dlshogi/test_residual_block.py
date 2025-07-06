import tempfile
import warnings

with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    import ai_edge_torch

import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._residual_block import _ResidualBlock
from vshogi.minishogi import Game


def test_export_residual_block_to_tflite():
    attentions = Game.get_local_attentions()
    module = _ResidualBlock(32, 8, attentions, attention_groups=8)
    sample_inputs = (th.randn(1, 32, 5, 5),)
    module(*sample_inputs)
    edge_model = ai_edge_torch.convert(module.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        interpreter = Interpreter(model_path=t.name)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    assert tuple(input_details['shape']) == (1, 32, 5, 5)
    assert tuple(output_details['shape']) == (1, 32, 5, 5)


def test_residual_block_backward():
    attentions = Game.get_local_attentions()
    model = _ResidualBlock(32, 8, attentions, attention_groups=8)
    x = th.randn(2, 32, 5, 5).requires_grad_()
    y = model(x)
    loss = th.sum(th.square(y - 1))
    loss.backward()


def test_residual_block_backward_mps():
    if not th.backends.mps.is_available():
        return
    attentions = Game.get_local_attentions()
    model = _ResidualBlock(32, 8, attentions, attention_groups=8).to('mps')
    x = th.randn(2, 32, 5, 5).requires_grad_().to('mps')
    y = model(x)
    loss = th.sum(th.square(y - 1))
    loss.backward()


if __name__ == '__main__':
    pytest.main([__file__])
