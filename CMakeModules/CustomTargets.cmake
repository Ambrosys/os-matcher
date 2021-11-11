#
# Specify custom targets for libraries, executables, and tets.
#
# Enforce use of clang-tidy and optinally iwyu on each target.
#


include( "CMakeModules/FindTool.cmake" )
include( "CMakeModules/ParseAndAddCatchTests.cmake" )

option ( RUN_CLANG_TIDY "Enable if you want to run clang-tidy" OFF )
if ( ${RUN_CLANG_TIDY} )
    find_tool2( CLANG_TIDY_EXE clang-tidy clang-tidy-6.0 REQUIRED )
    if(CLANG_TIDY_EXE)
        set(CLANG_TIDY_EXE "${CLANG_TIDY_EXE}")
    endif()
endif ()

find_tool( IWYU_EXE iwyu OPTIONAL )


function( add_generic_library TARGET )
    add_library( ${TARGET} ${ARGN} )
    add_library( Generic::${TARGET} ALIAS ${TARGET} )

    set_target_properties(
        ${TARGET} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
#         CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXE}"
    )
endfunction()


function( add_core_library TARGET )
    add_library( ${TARGET} ${ARGN} )
    add_library( Core::${TARGET} ALIAS ${TARGET} )

    set_target_properties(
        ${TARGET} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
#         CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXE}"
    )
endfunction()

function( add_object_core_library TARGET )
    add_library( ${TARGET} OBJECT ${ARGN} )
    add_library( Core::${TARGET} ALIAS ${TARGET} )

    set_target_properties(
            ${TARGET} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
            #         CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXE}"
    )
endfunction()

function( add_core_interface_library TARGET )
    add_library( ${TARGET} INTERFACE )
    add_library( Core::${TARGET} ALIAS ${TARGET} )
endfunction()


function( add_core_executable TARGET )
    add_executable( ${TARGET} ${ARGN} )

    set_target_properties(
        ${TARGET} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
#         CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXE}"
    )
endfunction()


enable_testing()
function( add_core_test TARGET )
    add_executable( ${TARGET} ${ARGN} )
    ParseAndAddCatchTests( ${TARGET} )

    #TODO(jg): Test these settings with Catch.
    # set_target_properties(
    #     ${TARGET} PROPERTIES
    #     CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    #     CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXE}"
    # )

    #TODO(jg): Catch does not inegrate well with ctest...
    # add_test( NAME ${TARGET} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} )
    add_test( NAME ${TARGET} COMMAND ${TARGET} )
endfunction()
