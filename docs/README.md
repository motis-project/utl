Documentation was originally setup in PR [#25](https://github.com/motis-project/utl/pull/25) following this tutorial:
[Clear, Functional C++ Documentation with Sphinx + Breathe + Doxygen + CMake](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/)

The documentation is then published to GitHub Pages: <https://motis-project.github.io/utl/>

[breathe](https://breathe.readthedocs.io) is used to insert Doxygen doc snippets
into the Sphinx-generated documentation.

List of supported directives: <https://breathe.readthedocs.io/en/latest/directives.html#directives>.
They have to be inserted using [`myst-parser` directive syntax for Markdown](https://myst-parser.readthedocs.io/en/latest/syntax/roles-and-directives.html).

## Installing the necessary tools

    apt install doxygen
    pip install -r docs/requirements.txt

## Generating Doxygen documentation
With CMake, from the root directory:

    cmake --build build --target Doxygen

Directly with `doxygen`:

    cd docs
    doxygen

## Generating Sphinx documentation
From the root directory, with auto-reload:

    sphinx-autobuild docs docs/_build/html --open-browser

Only generating the documentation:

    cd docs
    sphinx-build -M html . _build
