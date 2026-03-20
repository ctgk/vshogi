import typing as tp

import numpy as np
import pandas as pd
from ai_edge_litert.interpreter import Interpreter

from vshogi._game import Game


class PolicyValueFunction:
    """Policy-value function class."""

    def __init__(self, model_path: str, num_threads: int = 1) -> None:
        """Construct policy-value function.

        Parameters
        ----------
        model_path : str
            Path to tflite model.
        num_threads : int, optional
            Number of threads to use, by default 1
        """
        self._interpreter = Interpreter(
            model_path=model_path, num_threads=num_threads
        )
        self._interpreter.allocate_tensors()
        input_details = self._interpreter.get_input_details()[0]
        self._input_placeholder = np.empty(
            input_details['shape'], dtype=np.float32
        )
        self._input_index = input_details['index']
        output_details = self._interpreter.get_output_details()
        if (output_details[0]['shape'][-1] == 1) or len(
            output_details[0]["shape"]
        ) == 4:
            self._value_index = output_details[0]['index']
            self._value_shape = output_details[0]['shape']
            self._policy_index = output_details[1]['index']
        else:
            self._value_index = output_details[1]['index']
            self._value_shape = output_details[1]['shape']
            self._policy_index = output_details[0]['index']

    def __call__(self, game: Game) -> tp.Tuple[np.ndarray, float | np.ndarray]:
        """Return logits of policy and value of the current game state.

        Parameters
        ----------
        game : Game
            Current game state to evaluate.

        Returns
        -------
        tp.Tuple[np.ndarray, float | np.ndarray]
            Tuple of logits of policy and value.
        """
        game.to_dlshogi_features(out=self._input_placeholder)
        self._interpreter.set_tensor(
            self._input_index, self._input_placeholder
        )
        self._interpreter.invoke()
        policy_logits = self._interpreter.get_tensor(self._policy_index)
        value = self._interpreter.get_tensor(self._value_index)
        if self._value_shape[-1] == 1:
            value = value.item()
        return policy_logits, value

    def summary(self) -> str:
        """Return model structure summary.

        Returns
        -------
        str
            Model structure summary.
        """
        tensor_details = self._interpreter.get_tensor_details()
        data: list = []
        for op in self._interpreter._get_ops_details():
            if op['op_name'] == 'DELEGATE':
                continue
            data.append(
                {
                    'Operation': op['op_name'],
                    'Input Indices': list(op['inputs']),
                    'Output Indices': list(op['outputs']),
                    'Output Shape': tensor_details[op['outputs'][0]]['shape'],
                    'Connected to': [],
                }
            )
        for op_data in data:
            for oi in op_data['Output Indices']:
                for index in [
                    j for j, op in enumerate(data) if oi in op['Input Indices']
                ]:
                    op_data['Connected to'].append(
                        f'{index}:{data[index]["Operation"]}'
                    )

        df = pd.DataFrame(data)
        with pd.option_context(
            'display.max_rows',
            None,
            'display.max_columns',
            None,
        ):
            return str(df[['Operation', 'Output Shape', 'Connected to']])
