import typing as tp

import numpy as np
import tensorflow as tf

from vshogi._game import Game


class PolicyValueFunction:
    """Policy-value function class."""

    def __init__(
        self,
        model: tp.Union[tf.keras.Model, str],
        num_threads: int = 1,
    ) -> None:
        """Construct policy-value function.

        Parameters
        ----------
        model : tp.Union[tf.keras.Model, str]
            Keras model or path to tflite model.
        num_threads : int, optional
            Number of threads to use, by default 1
        """
        if isinstance(model, str):
            self._model_content = None
            self._interpreter = tf.lite.Interpreter(
                model_path=model, num_threads=num_threads)
        else:
            converter = tf.lite.TFLiteConverter.from_keras_model(model)
            self._model_content = converter.convert()
            self._interpreter = tf.lite.Interpreter(
                model_content=self._model_content, num_threads=num_threads)

        self._interpreter.allocate_tensors()
        input_details = self._interpreter.get_input_details()[0]
        self._input_placeholder = np.empty(
            input_details['shape'], dtype=np.float32)
        self._input_index = input_details['index']
        output_details = self._interpreter.get_output_details()
        self._value_index = output_details[0]['index']
        self._policy_index = output_details[1]['index']

    def __call__(self, game: Game) -> tp.Tuple[np.ndarray, float]:
        """Return logits of policy and value of the current game state.

        Parameters
        ----------
        game : Game
            Current game state to evaluate.

        Returns
        -------
        tp.Tuple[np.ndarray, float]
            Tuple of logits of policy and value.
        """
        game.to_dlshogi_features(out=self._input_placeholder)
        self._interpreter.set_tensor(
            self._input_index, self._input_placeholder)
        self._interpreter.invoke()
        value = self._interpreter.get_tensor(self._value_index).item()
        policy_logits = self._interpreter.get_tensor(self._policy_index)
        return policy_logits, value

    def save_model_as_tflite(self, output_path: str):
        """Save currently loaded model in tflite binary format.

        Parameters
        ----------
        output_path : str
            Path to dump tflite binary.

        Raises
        ------
        ValueError
            No model loaded currently.
        """
        if self._model_content is None:
            raise ValueError('Cannot save tflite binary.')
        with open(output_path, 'wb') as f:
            f.write(self._model_content)
