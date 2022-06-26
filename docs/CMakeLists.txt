# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Other docs files you want to track should go here
file(GLOB
    rst_files
    ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
)

# Find all the public headers
get_target_property(INDEX_PUBLIC_HEADER_DIR index_utils INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE INDEX_PUBLIC_HEADERS ${INDEX_PUBLIC_HEADER_DIR}/*.hpp)

set(DOXYGEN_INPUT_DIR  ${PROJECT_SOURCE_DIR}/include)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT       ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
add_custom_command(
    OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS ${INDEX_PUBLIC_HEADERS}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating docs"
)

add_custom_target(doxygen DEPENDS ${DOXYGEN_INDEX_FILE})

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_OUT    ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_target(sphinx
    COMMAND
        ${SPHINX_BUILD} -b html
        # Tell Breathe where to find the Doxygen output
        -Dbreathe_projects.Index=${DOXYGEN_OUTPUT_DIR}/xml
        ${SPHINX_SOURCE} ${SPHINX_OUT}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
        ${rst_files}
        ${DOXYGEN_INDEX_FILE}
        ${SPHINX_SOURCE}/conf.py
    COMMENT
        "Generating documentation with Sphinx"
)
