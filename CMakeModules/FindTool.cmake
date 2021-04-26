#
# Helper to find required and optional tools.
# TODO: Make TOOL_NAME an array to allow an arbitrary number of aliases and remove find_tool2().
#

function( find_tool RETURN TOOL_NAME REQUIRED )
    find_program(
        ${RETURN}
        NAMES ${TOOL_NAME}
        DOC "Path to ${TOOL_NAME} executable"
    )
    set( TARGET_EXE ${${RETURN}} )

    set( MESSAGE_LEVEL WARNING )
    if(${REQUIRED} STREQUAL "REQUIRED")
        set( MESSAGE_LEVEL FATAL_ERROR )
    endif()

    if(NOT TARGET_EXE)
        message( ${MESSAGE_LEVEL} "${TOOL_NAME} not found. Please install ${TOOL_NAME} on your system." )
    else()
        message( STATUS "${TOOL_NAME} found: ${TARGET_EXE}" )
    endif()
endfunction()

function( find_tool2 RETURN TOOL_NAME1 TOOL_NAME2 REQUIRED )
    find_program(
        ${RETURN}
        NAMES ${TOOL_NAME1} ${TOOL_NAME2}
        DOC "Path to ${TOOL_NAME1} executable"
    )
    set( TARGET_EXE ${${RETURN}} )

    set( MESSAGE_LEVEL WARNING )
    if(${REQUIRED} STREQUAL "REQUIRED")
        set( MESSAGE_LEVEL FATAL_ERROR )
    endif()

    if(NOT TARGET_EXE)
        message( ${MESSAGE_LEVEL} "${TOOL_NAME1} or ${TOOL_NAME2} not found. Please install ${TOOL_NAME1} on your system." )
    else()
        message( STATUS "${TOOL_NAME1} found: ${TARGET_EXE}" )
    endif()
endfunction()
