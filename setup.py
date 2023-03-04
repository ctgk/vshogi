from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class my_build_ext(build_ext):
    def build_extensions(self):
        if self.compiler.compiler_type == 'unix':
            for e in self.extensions:
                e.extra_compile_args = ['-msse4.2', '-mavx2']

        build_ext.build_extensions(self)

    def finalize_options(self):
        build_ext.finalize_options(self)
        __builtins__.__NUMPY_SETUP__ = False
        import numpy
        self.include_dirs.append(numpy.get_include())

ext_modules = [
    Extension('cshogi._cshogi',
        ['cshogi/_cshogi.pyx',
         "cpp/src/bitboard.cpp", "cpp/src/common.cpp", "cpp/src/generateMoves.cpp", "cpp/src/hand.cpp", "cpp/src/init.cpp", "cpp/src/move.cpp", "cpp/src/mt64bit.cpp", "cpp/src/position.cpp", "cpp/src/search.cpp", "cpp/src/square.cpp", "cpp/src/usi.cpp", "cpp/src/book.cpp", "cpp/src/mate.cpp", "cpp/src/dfpn.cpp"],
        language='c++',
        include_dirs = ["cpp/src"],
        define_macros=[('HAVE_SSE4', None), ('HAVE_SSE42', None), ('HAVE_AVX2', None)]),
]

setup(
    packages=['cshogi', 'cshogi.usi', 'cshogi.gym_shogi', 'cshogi.gym_shogi.envs', 'cshogi.dlshogi', 'cshogi.web', 'cshogi.web.templates', 'cshogi.web.static'],
    package_data={'cshogi.web.templates': ['*'], 'cshogi.web.static': ['*']},
    ext_modules=ext_modules,
    cmdclass={'build_ext': my_build_ext},
    description = 'A fast Python shogi library',
    classifiers=[
        "Programming Language :: Python :: 3",
        'License :: OSI Approved :: GNU General Public License (GPL)',
        "Operating System :: OS Independent",
    ],
)
