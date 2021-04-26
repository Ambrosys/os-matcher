set ( CUSTOM_USER_VERSION "not set" CACHE STRING "Version set in build process from extern (ci/jenkins or developer)" )
set ( ProjectIncludeBaseName  OsMatcher )  # Gets OSMATCHER for some cases
set ( GeneratedIncludes_DIR ${CMAKE_BINARY_DIR}/GeneratedIncludes/${ProjectIncludeBaseName} )

file( MAKE_DIRECTORY ${GeneratedIncludes_DIR}  )

set( OUTPUT_FILE "${GeneratedIncludes_DIR}/${ProjectIncludeBaseName}Version.h" )
set( CMAKE_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/CMakeModules/GitVersionTagToHeader.cmake" )
set( FAKE_DEPENDENCIES_FILE "${GeneratedIncludes_DIR}/__fake.h" )

add_custom_target(
    test-if-git-HEAD-has-changed ALL
    DEPENDS ${FAKE_DEPENDENCIES_FILE}
)

string( TOUPPER ${ProjectIncludeBaseName} ProjectIncludeBaseName_UPPERCASE )

add_custom_command(
    OUTPUT
        ${FAKE_DEPENDENCIES_FILE}  # fake! ensure we run!
        ${OUTPUT_FILE}   # real header
    COMMAND ${CMAKE_COMMAND} -DOUTPUT_FILE="${OUTPUT_FILE}" -DCUSTOM_USER_VERSION="${CUSTOM_USER_VERSION}" -DPROJECT_BASENAME="${ProjectIncludeBaseName_UPPERCASE}" -P "${CMAKE_SCRIPT_FILE}" 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
