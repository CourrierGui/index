# SPDX-License-Identifier: GPL-3.0-or-later

find_program(SPHINX_BUILD 
    NAMES sphinx-build
    DOC "Path to sphinx-build executable"
)

include(FindPackageHandleStandardArgs)

# Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx
    "Failed to find sphinx-build executable"
    SPHINX_BUILD
)
