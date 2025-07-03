import tempfile

import ai_edge_torch
import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._policy_head import PolicyHead


def test_export_policy_head_to_tflite():
    module = PolicyHead(32, 20)
    sample_inputs = (th.randn(1, 32, 5 * 5),)
    edge_model = ai_edge_torch.convert(module.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        interpreter = Interpreter(model_path=t.name)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    assert tuple(input_details['shape']) == (1, 32, 5 * 5)
    assert tuple(output_details['shape']) == (1, 5 * 5 * 20)


if __name__ == '__main__':
    pytest.main([__file__])
