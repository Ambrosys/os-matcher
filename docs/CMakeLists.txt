# Based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

set(DOC_CONTENT ${CMAKE_CURRENT_SOURCE_DIR}/content)
file(GLOB_RECURSE DOC_FILES CONFIGURE_DEPENDS ${DOC_CONTENT}/*.rst)
file(GLOB_RECURSE DRAWIO_FILES CONFIGURE_DEPENDS ${DOC_CONTENT}/*.drawio)

set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}/src")
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_SOURCE_DIR})
set(APPLICATIONS_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/../src)
set(APPLICATIONS_SYMLINK ${CMAKE_CURRENT_SOURCE_DIR}/_binaries)
set(DOXYGEN_XML_PATCH_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/patch-doxygen-xml-output.sh)
set(SPHINX_HTML_PATCH_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/patch-sphinx-html-output.sh)

set(DOC_OTHER_DEPENDENCIES
    ${DOC_FILES}
    ${CMAKE_SOURCE_DIR}/README.md
    ${DOXYGEN_INDEX_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/theme/static/css/style.css
    # ${CMAKE_CURRENT_SOURCE_DIR}/theme/templates/layout.html
    ${APPLICATIONS_SYMLINK}
    )
set(NEEDED_APPLICATIONS
    OsMatcher
    )

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Find all the public headers
# TODO: Seems not to work (when changing f.ex. AppComponents/Common/Filter/OsmMapReader.h,
#       Doxygen does not rerun).
get_target_property(PUBLIC_HEADER_DIR_1 OsMatcher INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE OS_MATCHER_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${PUBLIC_HEADER_DIR_1}/*.h
    )

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS ${OS_MATCHER_PUBLIC_HEADERS} ${DOXYGEN_XML_PATCH_SCRIPT}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    COMMAND bash ${DOXYGEN_XML_PATCH_SCRIPT} ${DOXYGEN_OUTPUT_DIR}/xml
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_SOURCE_DIR}/_api"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/sphinx"
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating code documentation with Doxygen"
    VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

add_custom_command(OUTPUT ${APPLICATIONS_SYMLINK}
    DEPENDS ${NEEDED_APPLICATIONS}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${APPLICATIONS_SOURCE} ${APPLICATIONS_SYMLINK}
    COMMENT "Generating applications symlink"
    VERBATIM)

#set(DRAWIO_OUTPUT_FILES "")
#foreach(DRAWIO_FILE ${DRAWIO_FILES})
#    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_FILE "${DRAWIO_FILE}.png")
#    set(DRAWIO_OUTPUT_FILES ${DRAWIO_OUTPUT_FILES} ${OUTPUT_FILE})
#    add_custom_command(
#        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/convert-drawio.sh \"${DRAWIO_FILE}\" \"${OUTPUT_FILE}\"
#        OUTPUT ${OUTPUT_FILE})
#endforeach()

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_HTML_FILE ${SPHINX_BUILD}/html/index.html)
set(SPHINX_LATEX_FILE ${SPHINX_BUILD}/latex/os-matcher.tex)
set(SPHINX_PDF_FILE ${SPHINX_BUILD}/latex/os-matcher.pdf)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_HTML_FILE}
    COMMAND ${SPHINX_EXECUTABLE} -j auto -b html
        # Tell Breathe where to find the Doxygen output
        -Dbreathe_projects.OS-Matcher=${DOXYGEN_OUTPUT_DIR}/xml
        ${SPHINX_SOURCE} ${SPHINX_BUILD}/html
    COMMAND bash ${SPHINX_HTML_PATCH_SCRIPT} ${SPHINX_BUILD}/html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
    ${SPHINX_HTML_PATCH_SCRIPT}
    #${DRAWIO_OUTPUT_FILES}
    ${DOC_OTHER_DEPENDENCIES}
    MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
    COMMENT "Generating HTML documentation with Sphinx")
add_custom_command(OUTPUT ${SPHINX_LATEX_FILE}
    COMMAND ${SPHINX_EXECUTABLE} -j auto -b latex
    # Tell Breathe where to find the Doxygen output
    -Dbreathe_projects.OS-Matcher=${DOXYGEN_OUTPUT_DIR}/xml
    ${SPHINX_SOURCE} ${SPHINX_BUILD}/latex
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
    ${SPHINX_HTML_FILE}  # TODO: Does not really depend on this, but they both
                         #       are not allowed to run in parallel, because
                         #       they both run Exhale with the common _api
                         #       directory they write to.
    #${DRAWIO_OUTPUT_FILES}
    ${DOC_OTHER_DEPENDENCIES}
    MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
    COMMENT "Generating LaTeX documentation with Sphinx")
add_custom_command(OUTPUT ${SPHINX_PDF_FILE}
    COMMAND make
    WORKING_DIRECTORY ${SPHINX_BUILD}/latex
    MAIN_DEPENDENCY ${SPHINX_LATEX_FILE}
    COMMENT "Generating PDF documentation with Sphinx")

# Nice named target so we can run the job easily
add_custom_target(SphinxHtml ALL DEPENDS ${SPHINX_HTML_FILE})
add_custom_target(SphinxPdf ALL DEPENDS ${SPHINX_PDF_FILE})
add_custom_target(Sphinx ALL DEPENDS SphinxHtml SphinxPdf)

install(
    DIRECTORY ${SPHINX_BUILD}/html
    DESTINATION doc
)
install(
    FILES ${SPHINX_PDF_FILE}
    DESTINATION doc
)
