set( SOURCES
    Progress.cpp
    )

add_generic_library( Progress ${SOURCES} )

target_include_directories( Progress
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )

install(
    TARGETS Progress
    ARCHIVE DESTINATION lib
)