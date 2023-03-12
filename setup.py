from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup


_vshogi_extension = Pybind11Extension(
    name='vshogi._vshogi',
    sources=sorted(
        glob('cpp/src/**/*.cpp', recursive=True)
        + glob('cpp/python/**/*.cpp', recursive=True),
    ),
    include_dirs=['cpp/include', 'cpp/src', 'cpp/python'],
)


setup(
    ext_modules=[_vshogi_extension],
    cmdclass={'build_ext': build_ext},
    classifiers=[
        "Programming Language :: Python :: 3",
        'License :: OSI Approved :: GNU General Public License (GPL)',
    ],
)
