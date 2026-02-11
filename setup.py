from glob import glob

import nanobind
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# DEPRECATED: This setup.py is kept for backwards compatibility.
# The project now uses scikit-build-core (see CMakeLists.txt and pyproject.toml)
# For building, use: pip install .
# For development: pip install --no-build-isolation -ve .

class NanobindBuildExt(build_ext):
    def build_extensions(self):
        # Add nanobind include directory
        for ext in self.extensions:
            nb_include = nanobind.include_dir()
            ext.include_dirs.append(nb_include)
            # Add the src directory for internal headers
            import os
            ext.include_dirs.append(os.path.join(nb_include, '..', 'src'))
            # Add robin_map include directory
            ext.include_dirs.append(os.path.join(nb_include, '..', 'ext', 'robin_map', 'include'))
        super().build_extensions()


def _get_nanobind_sources():
    """Get all nanobind source files."""
    import os
    src_dir = nanobind.source_dir()
    return [os.path.join(src_dir, 'nb_combined.cpp')]


_vshogi_extension = Extension(
    name='vshogi._vshogi',
    sources=sorted(
        glob('cpp/src/**/*.cpp', recursive=True)
        + ["cpp/python/vshogi_wrapper.cpp"]
        + _get_nanobind_sources(),
    ),
    include_dirs=['cpp/include', 'cpp/src', 'cpp/python'],
    language='c++',
    extra_compile_args=['-std=c++17'],
)


def _get_extension(variant: str) -> Extension:
    return Extension(
        name=f"vshogi.{variant}._{variant}",
        sources=sorted(
            glob("cpp/src/**/*.cpp", recursive=True)
            + [f"cpp/python/{variant}_wrapper.cpp"]
            + _get_nanobind_sources(),
        ),
        include_dirs=['cpp/include', 'cpp/src', 'cpp/python'],
        language='c++',
        extra_compile_args=['-std=c++17'],
    )


setup(
    ext_modules=[_vshogi_extension] + [
        _get_extension(variant)
        for variant in ("minishogi", "judkins_shogi", "shogi")
    ],
    cmdclass={'build_ext': NanobindBuildExt},
    classifiers=[
        "Programming Language :: Python :: 3",
    ],
)
