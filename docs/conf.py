# -*- coding: utf-8 -*-
#
# libCellML documentation build configuration file, created by
# sphinx-quickstart on Mon Jun  9 21:58:16 2014.
#
# This file is execfile()d with the current directory set to its
# containing dir.
#
# Note that not all possible configuration values are present in this
# autogenerated file.
#
# All configuration values have a default; values that are commented out
# serve to show the default.

import sys
import os

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#sys.path.insert(0, os.path.abspath('.'))

# -- General configuration ------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#needs_sphinx = '3.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',
    'sphinx.ext.extlinks',
    'sphinx.ext.autosectionlabel',
    'sphinx.ext.intersphinx',
    'hoverxref.extension',          # Enables tooltips and previews on hover
    'sphinxcontrib.osexample',      # Testing out multiple language tabs on code-blocks
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['static/templates']

# The suffix of source filenames.
source_suffix = '.rst'

# The encoding of source files.
#source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = 'index'

autosectionlabel_prefix_document = True

# These are the shorthand for external links.  Use them in the other pages as:
#   :shortcut:`Shortcut text <extra string if needed>` NB space before <
# Declare below as:
#   'shortcut': ('http://linkhere/%s',''), NB have to put the string insertion %s to make it work
extlinks = {
    'api': ('https://libcellml.github.io/apidocs/classlibcellml_1_1%s', ''),
    'buildbot': ('https://buildbot.net%s', ''),
    'cellml1to2': ('https://github.com/hsorby/cellml1to2%s', ''),
    'cellml2': ('https://cellml-specification.readthedocs.io/en/latest/%s', ''),
    'cellsolver': ('https://github.com/hsorby/cellsolver%s', ''),
    'cmake': ('https://cmake.org/%s', ''),
    'doxygen': ('http://www.doxygen.nl/%s', ''),
    'euler_method': ('https://en.wikipedia.org/wiki/Euler_method%s', ''),
    'git': ('https://git-scm.com/%s', ''),
    'github': ('https://github.com/%s', ''),
    'google_styleguide': ('https://google.github.io/styleguide/cppguide.html/%s', ''),
    'href_locator': ('https://www.w3.org/TR/2001/REC-xlink-20010627/#link-locators/%s', ''),
    'libcellml_repo': ('https://github.com/cellml/libcellml.git%s', ''),
    'libxml2': ('http://www.xmlsoft.org/%s', ''),
    'mathml2help': ('https://www.w3.org/TR/MathML2/chapter4.html#id.4.2.1.3%s', ''),
    'mathml2spec': ('http://www.w3.org/TR/2003/REC-MathML2-20031021/%s', ''),
    'mathml2types': ('https://www.w3.org/TR/MathML2/chapter4.html#contm.typeattrib%s', ''),
    'opencor': ('https://opencor.ws/%s', ''),
    'pmr': ('https://models.physiomeproject.org/welcome/%s', ''),
    'python': ('https://www.python.org/%s', ''),
    'rfc2119': ('http://www.ietf.org/rfc/rfc2119.txt%s', ''),
    'sphinx': ('http://sphinx-doc.org/%s', ''),
    'swig': ('http://www.swig.org/%s', ''),
    'unicode': ('http://www.fileformat.info/info/unicode/char/%s/index.htm', 'U+'),
    'xlink': ('http://www.w3.org/TR/2001/REC-xlink-20010627/%s', ''),
    'xml_help': ('https://www.w3.org/XML/%s', ''),
    'xml_1_1': ('http://www.w3.org/TR/xml11/%s', ''),
    'xml_infoset': ('http://www.w3.org/TR/2004/REC-xml-infoset-20040204/%s', ''),
    'xml_namespace_1_1': ('https://www.w3.org/TR/2006/REC-xml-names11-20060816/%s', ''),
}

# Mapping to get references in the specification document repo into this one
# See: https://docs.readthedocs.io/en/stable/guides/intersphinx.html
intersphinx_mapping = {
    'spec': ('https://cellml-specification-dev.readthedocs.io/', None),
}

# rst_epilog to define formatted, clickable code substitutions throughout.  It's included
# at the bottom of every file in the project.
# rst_prolog = """
# .. |internalBookmark2| replace:: ``internalBookmark2``
# .. |model| replace:: ``model``
# .. |component| replace:: ``component``
# .. |units| replace:: ``units``
# .. |unit| replace:: ``unit``
# .. |variable| replace:: ``variable``
# .. |reset| replace:: ``reset``
# .. |reset_value| replace:: ``reset_value``
# .. |test_value| replace:: ``test_value``
# .. |import| replace:: ``import``
# .. |import_component| replace:: ``import component``
# .. |import_units| replace:: ``import units``
# .. |map_variables| replace:: ``map_variables``
# .. |connection| replace:: ``connection``
# .. |math| replace:: ``math``
# .. |encapsulation| replace:: ``encapsulation``
# """
# rst_epilog = """
# .. |component| replace:: ``component``
# .. _component: :ref:`component`
# """

# General information about the project.
project = u'libCellML'
copyright = u'2019-@SPHINX_YEAR@, libCellML Contributors'

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = 'latest'
# The full version, including alpha/beta/rc tags.
release = 'latest'

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#language = None

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''
# Else, today_fmt is used as the format for a strftime call.
#today_fmt = '%B %d, %Y'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = []

# The reST default role (used for this markup: `text`) to use for all
# documents.
#default_role = None

# If true, '()' will be appended to :func: etc. cross-reference text.
#add_function_parentheses = True

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
#add_module_names = True

# If true, sectionauthor and moduleauthor directives will be shown in the
# output. They are ignored by default.
#show_authors = False

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# A list of ignored prefixes for module index sorting.
#modindex_common_prefix = []

# If true, keep warnings as "system message" paragraphs in the built documents.
#keep_warnings = False

# If true, enable figure numbering
numfig = True

# -- Options for HTML output ----------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
# html_theme = '@SPHINX_THEME@'
html_theme = 'sphinx_rtd_theme'
# html_theme = 'bizstyle'

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
html_theme_options = {
    'navigation_depth': 2,
}

# Automagically convert all :ref: blocks to show a tooltip using the hoverxref
# extension.
hoverxref_auto_ref = True
hoverxref_roles = ['term', 'code']

# Add any paths that contain custom themes here, relative to this directory.
# html_theme_path = ['@SPHINX_THEME_DIR@']

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
#html_title = None

# A shorter title for the navigation bar.  Default is the same as html_title.
#html_short_title = None

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
#html_logo = None

# The name of an image file (within the static path) to use as favicon of the
# docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
#html_favicon = None

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
# html_static_path = ['_static']
# html_style = 'css/theme.css'

html_static_path = ['static/']
html_css_files = ['css/libcellml.css']

# Add any extra paths that contain custom files (such as robots.txt or
# .htaccess) here, relative to this directory. These files are copied
# directly to the root of the documentation.
#html_extra_path = []

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
#html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
#html_sidebars = {}

# Additional templates that should be rendered to pages, maps page names to
# template names.
#html_additional_pages = {}

# If false, no module index is generated.
#html_domain_indices = True

# If false, no index is generated.
#html_use_index = True

# If true, the index is split into individual pages for each letter.
#html_split_index = False

# If true, links to the reST sources are added to the pages.
#html_show_sourcelink = True

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
#html_show_sphinx = True

# If true, "(C) Copyright ..." is shown in the HTML footer. Default is True.
#html_show_copyright = True

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
#html_use_opensearch = ''

# This is the file name suffix for HTML files (e.g. ".xhtml").
#html_file_suffix = None

# Output file base name for HTML help builder.
htmlhelp_basename = 'libCellMLdoc'


# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    'preamble': 'usepackage{textgreek}',

}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    ('index', 'libCellML.tex', u'libCellML Documentation',
     u'libCellML Contributors', 'manual'),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
#latex_logo = None

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
#latex_use_parts = False

# If true, show page references after internal links.
#latex_show_pagerefs = False

# If true, show URL addresses after external links.
#latex_show_urls = False

# Documents to append as an appendix to all manuals.
#latex_appendices = []

# If false, no module index is generated.
#latex_domain_indices = True


# -- Options for manual page output ---------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('index', 'libcellml', u'libCellML Documentation',
     [u'libCellML Contributors'], 1)
]

# If true, show URL addresses after external links.
#man_show_urls = False


# -- Options for Texinfo output -------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    ('index', 'libCellML', u'libCellML Documentation',
     u'libCellML Contributors', 'libCellML', 'C++ library for processing CellML models.',
     'Miscellaneous'),
]

# Documents to append as an appendix to all manuals.
#texinfo_appendices = []

# If false, no module index is generated.
#texinfo_domain_indices = True

# How to display URL addresses: 'footnote', 'no', or 'inline'.
#texinfo_show_urls = 'footnote'

# If true, do not generate a @detailmenu in the "Top" node's menu.
#texinfo_no_detailmenu = False


def setup(app):
    app.add_css_file('libcellml.css')
