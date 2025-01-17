# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'MOTIS utl module'
author = 'The MOTIS project developers'
copyright = '2025, The MOTIS project developers'
html_show_copyright = False

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'README.md']
extensions = [
  'breathe',    # allow inserting Doxygen snippets: https://breathe.readthedocs.io
  'myst_parser' # allow using Markdown instead of reStructuredText: https://www.sphinx-doc.org/en/master/usage/markdown.html
]

# myst-parser configuration:
myst_enable_extensions = [ # Doc: https://myst-parser.readthedocs.io/en/latest/syntax/optional.html
    "attrs_inline",    # enable parsing of inline attributes after certain inline syntaxes
    "colon_fence",     # use ::: delimiters to denote directives, instead of ```
    "deflist",         # allow to use definition lists
    "html_admonition", # enable parsing of <div class="admonition"> HTML blocks
    "html_image",      # convert any isolated img tags to the internal representation used in Sphinx
    "replacements",    # automatically convert some common typographic texts
    "smartquotes",     # automatically convert standard quotations to their opening/closing variants
    "tasklist",        # support for markdown list items starting with [ ] or [x]
]

# breathe configuration;
breathe_projects = {
    'utl': './xml/',
}
breathe_default_project = 'utl'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_logo = './motis-logo.svg'
