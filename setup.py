from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

class my_build_ext(build_ext):
    def build_extensions(self):
        if self.compiler.compiler_type == 'unix':
            for e in self.extensions:
                e.extra_compile_args = ['-msse4.2', '-mavx2']
        build_ext.build_extensions(self)


extension = Pybind11Extension(
    name='cshogi._cshogi',
    sources=sorted(glob('cpp/python/*.cpp')) + sorted(glob('cpp/src/*.cpp')),
    include_dirs=['cpp/src'],
    define_macros=[('HAVE_SSE4', None), ('HAVE_SSE42', None), ('HAVE_AVX2', None)],
)

setup(
    packages=['cshogi', 'cshogi.usi', 'cshogi.gym_shogi', 'cshogi.gym_shogi.envs', 'cshogi.dlshogi', 'cshogi.web', 'cshogi.web.templates', 'cshogi.web.static'],
    package_data={'cshogi.web.templates': ['*'], 'cshogi.web.static': ['*']},
    ext_modules=[extension],
    cmdclass={'build_ext': my_build_ext},
    description = 'A fast Python shogi library',
    classifiers=[
        "Programming Language :: Python :: 3",
        'License :: OSI Approved :: GNU General Public License (GPL)',
        "Operating System :: OS Independent",
    ],
)
