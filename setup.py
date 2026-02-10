from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup


_vshogi_extension = Pybind11Extension(
    name='vshogi._vshogi',
    sources=sorted(
        glob('cpp/src/**/*.cpp', recursive=True)
        + ["cpp/python/vshogi_wrapper.cpp"],
    ),
    include_dirs=['cpp/include', 'cpp/src', 'cpp/python'],
)


def _get_extension(variant: str) -> Pybind11Extension:
    return Pybind11Extension(
        name=f"vshogi.{variant}._{variant}",
        sources=sorted(
            glob("cpp/src/**/*.cpp", recursive=True)
            + [f"cpp/python/{variant}_wrapper.cpp"]
        ),
        include_dirs=['cpp/include', 'cpp/src', 'cpp/python'],
    )


setup(
    ext_modules=[_vshogi_extension] + [
        _get_extension(variant)
        for variant in ("minishogi", "judkins_shogi", "shogi")
    ],
    cmdclass={'build_ext': build_ext},
    classifiers=[
        "Programming Language :: Python :: 3",
    ],
)
