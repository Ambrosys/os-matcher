# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import t3SphinxThemeRtd
import exhale.utils
import sphinx  # only for Options for m2r extension


# -- Project information -----------------------------------------------------

project = 'OS-Matcher'
copyright = '2018, Ambrosys GmbH'
author = 'Fabian Sandoval Saldias, Julien Gout, Markus Abel'


# -- General configuration ---------------------------------------------------

master_doc = 'index'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'breathe',
    'exhale',
    'm2r',
    'sphinxcontrib.programoutput',
    'cloud_sptheme.ext.table_styling',  # see https://cloud-sptheme.readthedocs.io/en/latest/lib/cloud_sptheme.ext.table_styling.html
    'sphinx.ext.todo',
]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = [] # ['_build', 'Thumbs.db', '.DS_Store']

ambrosys_logo = 'theme/static/img/ambrosys.svg'
ambrosys_logo_latex = 'theme/static/img/ambrosys.pdf'


# -- Options for images -------------------------------------------------

numfig = True


# -- Options for syntax highlighting -------------------------------------------------

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'

pygments_style = 'tango' # 'trac' # 'stata' # 'lovelace' # 'default'


# -- Options for HTML output -------------------------------------------------

# Some possible options for `html_theme`:
# 'sphinx_ambrosys_theme', 't3SphinxThemeRtd', 'sphinx_rtd_theme', 'alabaster'

# Example options for sphinx_rtd_theme based themes (sphinx_ambrosys_theme, t3SphinxThemeRtd, sphinx_rtd_theme)
# (not necessarily the default ones):

# html_theme_options = {
#     'canonical_url': '',
#     'analytics_id': 'UA-XXXXXXX-1',  #  Provided by Google in your dashboard
#     'logo_only': False,
#     'display_version': True,
#     'prev_next_buttons_location': 'bottom',
#     'style_external_links': False,
#     'vcs_pageview_mode': '',
#     'style_nav_header_background': 'white',
#     # Toc options
#     'collapse_navigation': True,
#     'sticky_navigation': True,
#     'navigation_depth': 4,
#     'includehidden': True,
#     'titles_only': False
# }

# sphinx_ambrosys_theme
# *********************

# html_theme = 'sphinx_ambrosys_theme'
#
# html_theme_options = {
#     'explore': False,
#     'collapse_navigation': False,
# }
#
# html_context = {
#     "extra_css_files": [
#         '_static/pygments.css',
#     ],
# }
#
# html_logo = ambrosys_logo
#
# # Add any paths that contain templates here, relative to this directory.
# templates_path = []  # ['theme/templates']
#
# # Add any paths that contain custom static files (such as style sheets) here,
# # relative to this directory. They are copied after the builtin static files,
# # so a file named "default.css" will overwrite the builtin "default.css".
# html_static_path = []  # ['theme/static']

# END sphinx_ambrosys_theme

# t3SphinxThemeRtd
# ****************

html_theme = 't3SphinxThemeRtd'

html_theme_options = {
    # 'logo': ambrosys_logo,  # TODO: does not work
}

html_theme_path = [
    '.',
    t3SphinxThemeRtd.get_html_theme_path()
]

html_context = {
    'extra_css_files': [
        '_static/pygments.css',
        '_static/css/style.css',
    ],
}

# html_logo = ambrosys_logo  # TODO: does not work

# Add any paths that contain templates here, relative to this directory.
templates_path = []  # ['theme/templates']

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['theme/static']

# END t3SphinxThemeRtd


# -- Options for LaTeX output -------------------------------------------------

latex_elements = {
    # Additional stuff for the LaTeX preamble.
    'preamble': r'\usepackage{enumitem}\setlistdepth{99}',
}

latex_logo = ambrosys_logo_latex


# -- Options for Breathe extension -------------------------------------------------

breathe_default_project = 'OS-Matcher'


# -- Options for Exhale extension -------------------------------------------------

_exhale_api_path = './_api'

def specifications_for_kind(kind):
    '''
    For a given input ``kind``, return the list of reStructuredText specifications
    for the associated Breathe directive.
    '''
    # Change the defaults for .. doxygenclass:: and .. doxygenstruct::
    if kind == "class" or kind == "struct":
        return [
            ":members:",
            ":protected-members:",
            ":private-members:",
            ":undoc-members:"
        ]
    # An empty list signals to Exhale to use the defaults
    else:
        return []

exhale_args = {
    # These arguments are required
    'containmentFolder':     _exhale_api_path,
    'rootFileName':          'library_root.rst',
    'rootFileTitle':         'API documentation',
    'doxygenStripFromPath':  '..',
    # Suggested optional arguments
    'createTreeView':        True,
    # TIP: if using the sphinx-bootstrap-theme, you need
    # 'treeViewIsBootstrap': True,
    'exhaleExecutesDoxygen': False,
    # 'exhaleDoxygenStdin':    'INPUT = ../include',
    # See https://exhale.readthedocs.io/en/latest/usage.html#customizing-breathe-output
    "customSpecificationsMapping": exhale.utils.makeCustomSpecificationsMapping(
        specifications_for_kind
    ),
}


# -- Options for m2r extension -------------------------------------------------

# https://github.com/miyakogi/m2r/issues/51#issuecomment-618285433

def monkeypatch(cls):
    """ decorator to monkey-patch methods """
    def decorator(f):
        method = f.__name__
        old_method = getattr(cls, method)
        setattr(cls, method, lambda self, *args, **kwargs: f(old_method, self, *args, **kwargs))
    return decorator

# workaround until https://github.com/miyakogi/m2r/pull/55 is merged
@monkeypatch(sphinx.registry.SphinxComponentRegistry)
def add_source_parser(_old_add_source_parser, self, *args, **kwargs):
    # signature is (parser: Type[Parser], **kwargs), but m2r expects
    # the removed (str, parser: Type[Parser], **kwargs).
    if isinstance(args[0], str):
        args = args[1:]
    return _old_add_source_parser(self, *args, **kwargs)


# -- Options for sphinx.ext.todo extension -------------------------------------------------

todo_include_todos = True
