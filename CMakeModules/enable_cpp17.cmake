macro( enable_cpp17 )
    set( GNUCXX_REQUIRED_VERSION 5.0 )
    set( CLANG_REQUIRED_VERSION 4.0 )

    if( CMAKE_COMPILER_IS_GNUCXX )
        if( CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL ${GNUCXX_REQUIRED_VERSION} OR
                CMAKE_CXX_COMPILER_VERSION VERSION_GREATER ${GNUCXX_REQUIRED_VERSION} )
            add_compile_options( -std=c++1z )
        else()
            message ( FATAL_ERROR "A gcc version >= ${GNUCXX_REQUIRED_VERSION} is needed for C++17 support." )
        endif()
    endif()
    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
        if( CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL ${CLANG_REQUIRED_VERSION} OR
            CMAKE_CXX_COMPILER_VERSION VERSION_GREATER ${CLANG_REQUIRED_VERSION} )
            add_compile_options( -std=c++1z )
        else()
            message( FATAL_ERROR "A clang version >= ${CLANG_REQUIRED_VERSION} is needed for C++17 support." )
        endif()
    endif()
endmacro( enable_cpp17 )

