import subprocess
import sys
import time
from datetime import datetime, timedelta
from threading import Thread

import click as cl

from vshogi.dlshogi._alpha_zero._network_trainer import _NetworkTrainer
from vshogi.dlshogi._alpha_zero._self_play_worker import _SelfPlayWorker


_previous_streams = {
    "nn-trainer": "",
    "self-player": "",
}
def stream_output(process, process_name, stream_name):
    """Thread function to read and print output from a process stream."""
    stream = process.stdout if stream_name == "stdout" else process.stderr
    for line in iter(stream.readline, ''):
        if line and ("it/s]" not in line) and ("s/it]" not in line):
            prev = _previous_streams[process_name]
            if ("it/s]" in prev) or ("s/it]" in prev):
                print(f"[{process_name}] {prev}", end='')
            print(f"[{process_name}] {line}", end='')
        _previous_streams[process_name] = line
    stream.close()


def run_processes(**kwargs):
    duration_seconds = kwargs["duration_seconds"]
    kwargs_play = {
        k[len("play_"):]: v
        for k, v in kwargs.items()
        if k.startswith("play_")
    }
    kwargs_train = {
        k[len("train_"):]: v
        for k, v in kwargs.items()
        if k.startswith("train_")
    }
    start_time = datetime.now()
    print(f"Start time: {start_time.strftime('%Y-%m-%d %H:%M:%S')}")
    if duration_seconds is not None:
        end_time = start_time + timedelta(seconds=duration_seconds)
        print(f"End time: {end_time.strftime('%Y-%m-%d %H:%M:%S')}")
    print("-" * 60)

    # Start nn-trainer process
    nn_trainer = subprocess.Popen(
        (
            [sys.executable, "-m", "vshogi", "alpha-zero", "nn-trainer"]
            + [kwargs["shogi"]] + sum(
                [
                    [f"--{k.replace('_', '-')}", str(v)]
                    for k, v in kwargs_train.items()
                ],
                start=[],
            )
        ),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1
    )
    print(f"Started nn-trainer (PID: {nn_trainer.pid})")

    # Start self-play-worker process
    self_play_worker = subprocess.Popen(
        (
            [sys.executable, "-m", "vshogi", "alpha-zero", "self-play-worker"]
            + [kwargs["shogi"]] + sum(
                [
                    [f"--{k.replace('_', '-')}", str(v)]
                    for k, v in kwargs_play.items()
                ],
                start=[],
            )
        ),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1
    )
    print(f"Started self-play-worker (PID: {self_play_worker.pid})")

    print("-" * 60)

    # Start threads to continuously print output from nn-trainer
    Thread(
        target=stream_output,
        args=(nn_trainer, "nn-trainer", "stdout"),
        daemon=True,
    ).start()
    Thread(
        target=stream_output,
        args=(nn_trainer, "nn-trainer", "stderr"),
        daemon=True,
    ).start()
    Thread(
        target=stream_output,
        args=(self_play_worker, "self-player", "stdout"),
        daemon=True,
    ).start()
    Thread(
        target=stream_output,
        args=(self_play_worker, "self-player", "stderr"),
        daemon=True,
    ).start()

    try:
        # Wait for the specified duration
        while (duration_seconds is None) or (datetime.now() < end_time):
            # Check if processes are still running
            nn_trainer_status = nn_trainer.poll()
            self_play_worker_status = self_play_worker.poll()

            if nn_trainer_status is not None:
                print(f"\nWARNING: nn-trainer exited with code {nn_trainer_status}")

            if self_play_worker_status is not None:
                print(f"\nWARNING: self-play-worker exited with code {self_play_worker_status}")

            if nn_trainer_status is not None and self_play_worker_status is not None:
                print("\nBoth processes have exited. Stopping early.")
                break

            # Sleep for a short interval before checking again
            time.sleep(5)

        print("\n" + "-" * 60)
        print(f"Duration completed at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")

    except KeyboardInterrupt:
        print("\n\nKeyboard interrupt received. Stopping processes...")

    finally:
        # Terminate processes
        print("\nTerminating processes...")

        if nn_trainer.poll() is None:
            print(f"Terminating nn-trainer (PID: {nn_trainer.pid})")
            nn_trainer.terminate()
            try:
                nn_trainer.wait(timeout=10)
            except subprocess.TimeoutExpired:
                print(f"Force killing nn-trainer (PID: {nn_trainer.pid})")
                nn_trainer.kill()

        if self_play_worker.poll() is None:
            print(f"Terminating self-play-worker (PID: {self_play_worker.pid})")
            self_play_worker.terminate()
            try:
                self_play_worker.wait(timeout=10)
            except subprocess.TimeoutExpired:
                print(f"Force killing self-play-worker (PID: {self_play_worker.pid})")
                self_play_worker.kill()

        print("All processes terminated.")


@cl.command()
@cl.argument("shogi", type=cl.Choice(["minishogi", "judkins_shogi", "shogi"]))
@cl.option(
    "--duration-seconds",
    default=None,
    type=int,
    show_default=True,
    help="Duration in seconds to run the processes.",
)
@_SelfPlayWorker.wrap_options(prefix="play")
@_NetworkTrainer.wrap_options(prefix="train")
def main(**kwargs):
    print(kwargs)
    run_processes(**kwargs)


if __name__ == "__main__":
    main()
