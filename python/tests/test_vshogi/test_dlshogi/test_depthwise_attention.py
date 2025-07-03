import tempfile

import ai_edge_torch
import pytest
import torch as th
from ai_edge_litert.interpreter import Interpreter

from vshogi.dlshogi._depthwise_attention import _DepthwiseAttention
from vshogi.minishogi import Game


def test_export_depthwise_attention_to_tflite():
    attentions = Game.get_local_attentions()
    module = _DepthwiseAttention(attentions, groups=8, bias=False)
    num_param = sum(p.numel() for p in module.parameters() if p.requires_grad)
    assert (attentions.shape[0] * 8) == num_param
    sample_inputs = (th.randn(1, 32, 5 * 5),)
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
