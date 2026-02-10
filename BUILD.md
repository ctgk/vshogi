# Building vshogi with scikit-build-core

This project uses [scikit-build-core](https://scikit-build-core.readthedocs.io/) with [nanobind](https://github.com/wjakob/nanobind) to build Python extensions from C++ code.

## Requirements

- Python 3.10+
- CMake 3.15+
- C++17 compatible compiler

## Installation

### For Users

Simply install using pip:

```bash
pip install .
```

### For Developers

Install in editable mode with no build isolation (faster for development):

```bash
# Install build dependencies first
pip install nanobind scikit-build-core[pyproject]

# Install in editable mode
pip install --no-build-isolation -ve .
```

For auto-rebuild on import (requires scikit-build-core 0.5+):

```bash
pip install -Ceditable.rebuild=true --no-build-isolation -ve .
```

## Build System Details

### Architecture

The build system consists of:

1. **pyproject.toml**: Build configuration
   - Uses `scikit-build-core` as the build backend
   - Specifies dependencies (nanobind, scikit-build-core)
   - Configures stable ABI for Python 3.12+ (reduces wheel count)

2. **CMakeLists.txt**: CMake build script
   - Builds 4 extension modules: `_vshogi`, `_minishogi`, `_judkins_shogi`, `_shogi`
   - Uses `nanobind_add_module` for proper nanobind integration
   - Installs Python source files alongside compiled modules

3. **cpp/CMakeLists.txt**: C++ library configuration (for C++ tests)

### Extension Modules

Four extension modules are built:

- `vshogi._vshogi`: Core vshogi functionality
- `vshogi.minishogi._minishogi`: Minishogi variant
- `vshogi.judkins_shogi._judkins_shogi`: Judkins shogi variant
- `vshogi.shogi._shogi`: Full shogi variant

All modules use:
- **Stable ABI** (Python 3.12+): Single wheel works across Python versions
- **Static linking**: nanobind is statically linked into each module
- **C++17**: Modern C++ standard

### Build Configuration

Key scikit-build-core settings in `pyproject.toml`:

```toml
[tool.scikit-build]
minimum-version = "build-system.requires"
build-dir = "build/{wheel_tag}"
cmake.source-dir = "."
wheel.py-api = "cp312"  # Stable ABI for Python 3.12+
wheel.install-dir = "vshogi"
```

## Development Workflow

### Clean Build

```bash
rm -rf build
pip install --no-build-isolation -ve .
```

### Incremental Build

After modifying C++ files:

```bash
pip install --no-build-isolation -ve .
```

Or with auto-rebuild:

```bash
# Once during setup
pip install -Ceditable.rebuild=true --no-build-isolation -ve .

# Then just import in Python - it will auto-rebuild if C++ changed
python -c "import vshogi"
```

### Running Tests

```bash
pytest python/tests/
```

## Benefits of scikit-build-core

1. **Modern Build System**: Standard approach for C++ Python extensions
2. **Better CMake Integration**: Native CMake support, no wrapper needed
3. **Stable ABI Support**: One wheel for multiple Python versions (3.12+)
4. **Faster Incremental Builds**: Better caching and dependency tracking
5. **Cleaner Configuration**: Separation of build logic from Python packaging

## Comparison with Old Build System

### Old (setup.py with setuptools)

- Required manual management of nanobind sources
- Manual include directory configuration
- No stable ABI support
- setuptools-specific Extension class

### New (scikit-build-core with CMake)

- CMake handles all dependencies via `find_package(nanobind)`
- Clean, declarative `nanobind_add_module` API
- Automatic stable ABI support
- Standard CMake workflow

## Migration Notes

The old `setup.py` is kept for backwards compatibility but is deprecated. All new development should use the scikit-build-core workflow.

## References

- [nanobind_example](https://github.com/wjakob/nanobind_example) - Reference implementation
- [scikit-build-core documentation](https://scikit-build-core.readthedocs.io/)
- [nanobind documentation](https://nanobind.readthedocs.io/)
