
set( GIT_HOOKS pre-commit )

foreach( hook IN LISTS GIT_HOOKS )
    message( STATUS "Setting up git ${hook} to point to ${CMAKE_SOURCE_DIR}/GitHooks/${hook}")
    execute_process(
        COMMAND cmake -E compare_files GitHooks/pre-commit .git/hooks/pre-commit
        RESULT_VARIABLE test_not_successful
        OUTPUT_QUIET ERROR_QUIET
    )
    if( test_not_successful )
        message(FATAL_ERROR "Please set a symlink from ${CMAKE_SOURCE_DIR}/GitHooks/${hook} to ${CMAKE_SOURCE_DIR}/.git/hooks/${hook}.")
    endif()
#
#     if( UNIX )
#         execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/GitHooks/${hook} ${CMAKE_SOURCE_DIR}/.git/hooks/${hook} )
#     else()
#         execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/GitHooks/${hook} ${CMAKE_SOURCE_DIR}/.git/hooks/${hook} )
#     endif()
endforeach()
