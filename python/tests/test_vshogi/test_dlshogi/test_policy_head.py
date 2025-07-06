import tempfile
import warnings

with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    import ai_edge_torch

import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._policy_head import PolicyHead


def test_export_policy_head_to_tflite():
    module = PolicyHead(32, 20)
    sample_inputs = (th.randn(1, 32, 5, 5),)
    edge_model = ai_edge_torch.convert(module.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        interpreter = Interpreter(model_path=t.name)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    assert tuple(input_details['shape']) == (1, 32, 5, 5)
    assert tuple(output_details['shape']) == (1, 5 * 5 * 20)


def test_policy_head_backward():
    model = PolicyHead(32, 20)
    x = th.randn(2, 32, 5, 5).requires_grad_()
    y = model(x)
    loss = th.sum(th.square(y - 1))
    loss.backward()


def test_policy_head_backward_mps():
    if not th.backends.mps.is_available():
        return
    model = PolicyHead(32, 20).to('mps')
    x = th.randn(2, 32, 5, 5).requires_grad_().to('mps')
    y = model(x)
    loss = th.sum(th.square(y - 1))
    loss.backward()


if __name__ == '__main__':
    pytest.main([__file__])
