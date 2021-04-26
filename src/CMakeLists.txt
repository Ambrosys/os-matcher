add_subdirectory( AppComponents )
add_subdirectory( Core )
add_subdirectory( Generic )

option( BUILD_UNITTESTS "Enable if you want to build unit tests" ON )
if( ${BUILD_UNITTESTS} )
    add_subdirectory( UnitTest )
endif()
