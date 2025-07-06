import tempfile
import warnings

with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    import ai_edge_torch

import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._value_head import _ValueHead


def test_export_value_head_to_tflite():
    module = _ValueHead(32, (5, 5))
    num_param = sum(p.numel() for p in module.parameters() if p.requires_grad)
    assert (5 * 5 * 32 + 32) == num_param
    sample_inputs = (th.randn(1, 32, 5, 5),)
    edge_model = ai_edge_torch.convert(module.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        interpreter = Interpreter(model_path=t.name)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    assert tuple(input_details['shape']) == (1, 32, 5, 5)
    assert tuple(output_details['shape']) == (1, 1)


def test_value_head_backward():
    model = _ValueHead(32, (5, 5))
    x = th.randn(2, 32, 5, 5).requires_grad_()
    y = model(x)
    loss = th.sum(th.square(y - 1))
    loss.backward()


def test_value_head_backward_mps():
    if not th.backends.mps.is_available():
        return
    model = _ValueHead(32, (5, 5))
    model.to('mps')
    x = th.randn(2, 32, 5, 5).requires_grad_().to('mps')
    y = model(x)
    loss = th.sum(th.square(y - 1))
    loss.backward()


if __name__ == '__main__':
    pytest.main([__file__])
