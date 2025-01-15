Documentation was originally setup following this tutorial:
[Clear, Functional C++ Documentation with Sphinx + Breathe + Doxygen + CMake](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/)

## Generating Doxygen documentation
With CMake, from the root directory:

    cmake --build build --target Doxygen

Directly with `doxygen`:

    cd docs
    doxygen

## Generating Sphinx documentation
From the root directory:

    sphinx-autobuild docs docs/_build/html --open-browser
