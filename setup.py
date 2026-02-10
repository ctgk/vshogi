from glob import glob

from nanobind.setuptools import NanobindExtension, build_ext
from setuptools import setup


_vshogi_extension = NanobindExtension(
    name='vshogi._vshogi',
    sources=sorted(
        glob('cpp/src/**/*.cpp', recursive=True)
        + ["cpp/python/vshogi_wrapper.cpp"],
    ),
    include_dirs=['cpp/include', 'cpp/src', 'cpp/python'],
)


def _get_extension(variant: str) -> NanobindExtension:
    return NanobindExtension(
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
