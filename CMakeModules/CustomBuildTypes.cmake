#SET( GCC_DEBUG_FLAGS "-ggdb3 -fno-omit-frame-pointer" )


#~/opt/cmake-3.4.0/bin/cmake ../.. -G Ninja -DTHIRDPARTY_ROOT=$HOME/src/ThirdPartySTM -DAMBROSYS_COMMON_VERSION=13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$(which g++-5) -DCMAKE_C_COMPILER=$(which gcc-5) -DCMAKE_CXX_FLAGS="-fsanitize=undefined -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=undefined -fno-omit-frame-pointer" -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=undefined -fno-omit-frame-pointer"
#~/opt/cmake-3.4.0/bin/cmake ../.. -G Ninja -DTHIRDPARTY_ROOT=$HOME/src/ThirdPartySTM -DAMBROSYS_COMMON_VERSION=13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$(which g++-5) -DCMAKE_C_COMPILER=$(which gcc-5) -DCMAKE_CXX_FLAGS="-fsanitize=leak -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=leak -fno-omit-frame-pointer" -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=leak -fno-omit-frame-pointer"
#~/opt/cmake-3.4.0/bin/cmake ../.. -G Ninja -DTHIRDPARTY_ROOT=$HOME/src/ThirdPartySTM -DAMBROSYS_COMMON_VERSION=13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$(which g++-5) -DCMAKE_C_COMPILER=$(which gcc-5) -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address -fno-omit-frame-pointer" -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address -fno-omit-frame-pointer"


# Coverage 

#IF( NOT CMAKE_BUILD_TYPE )
#   SET(CMAKE_BUILD_TYPE ""
#       CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage ."
#       FORCE)
#ENDIF()

SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage USAN ASAN TSAN DEV" FORCE )

SET( CMAKE_CXX_FLAGS_DEV
        "${CMAKE_CXX_FLAGS_DEBUG}  -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -O1 -ggdb3 -D_GLIBCXX_DEBUG"
        CACHE STRING "Flags used by the C++ compiler during coverage builds."
        FORCE)

SET( CMAKE_C_FLAGS_DEV
        "${CMAKE_C_FLAGS_DEBUG}"
        CACHE STRING "Flags used by the C compiler during coverage builds."
        FORCE)

SET( CMAKE_EXE_LINKER_FLAGS_DEV
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Flags used for linking binaries during coverage builds."
        FORCE)

SET( CMAKE_SHARED_LINKER_FLAGS_DEV
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Flags used by the shared libraries linker during coverage builds."
        FORCE)

MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_DEV
        CMAKE_C_FLAGS_DEV
        CMAKE_EXE_LINKER_FLAGS_DEV
        CMAKE_SHARED_LINKER_FLAGS_DEV )


SET( CMAKE_CXX_FLAGS_COVERAGE
        "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage"
        CACHE STRING "Flags used by the C++ compiler during coverage builds."
        FORCE)

SET( CMAKE_C_FLAGS_COVERAGE
        "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage"
        CACHE STRING "Flags used by the C compiler during coverage builds."
        FORCE)

SET( CMAKE_EXE_LINKER_FLAGS_COVERAGE
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Flags used for linking binaries during coverage builds."
        FORCE)

SET( CMAKE_SHARED_LINKER_FLAGS_COVERAGE
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Flags used by the shared libraries linker during coverage builds."
        FORCE)

MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_COVERAGE
        CMAKE_C_FLAGS_COVERAGE
        CMAKE_EXE_LINKER_FLAGS_COVERAGE
        CMAKE_SHARED_LINKER_FLAGS_COVERAGE )

        
        
SET( CMAKE_CXX_FLAGS_TSAN
        "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the C++ compiler during sanitize=thread builds."
        FORCE)

SET( CMAKE_C_FLAGS_TSAN
        "${CMAKE_C_FLAGS_DEBUG} -fsanitize=thread -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the C compiler during sanitize=thread builds."
        FORCE)

SET( CMAKE_EXE_LINKER_FLAGS_TSAN
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=thread -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used for linking binaries during sanitize=thread builds."
        FORCE)

SET( CMAKE_SHARED_LINKER_FLAGS_TSAN
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}  -fsanitize=thread -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the shared libraries linker during sanitize=thread builds."
        FORCE)

MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_TSAN
        CMAKE_C_FLAGS_TSAN
        CMAKE_EXE_LINKER_FLAGS_TSAN
        CMAKE_SHARED_LINKER_FLAGS_TSAN )
        
        
SET( CMAKE_CXX_FLAGS_ASAN
        "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the C++ compiler during sanitize=address builds."
        FORCE)

SET( CMAKE_C_FLAGS_ASAN
        "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the C compiler during sanitize=address builds."
        FORCE)

SET( CMAKE_EXE_LINKER_FLAGS_ASAN
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used for linking binaries during sanitize=address builds."
        FORCE)

SET( CMAKE_SHARED_LINKER_FLAGS_ASAN
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}  -fsanitize=address -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the shared libraries linker during sanitize=address builds."
        FORCE)

MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_ASAN
        CMAKE_C_FLAGS_ASAN
        CMAKE_EXE_LINKER_FLAGS_ASAN
        CMAKE_SHARED_LINKER_FLAGS_ASAN )
        
        
                
SET( CMAKE_CXX_FLAGS_USAN
        "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the C++ compiler during sanitize=undefined builds."
        FORCE)

SET( CMAKE_C_FLAGS_USAN
        "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the C compiler during sanitize=undefined builds."
        FORCE)

SET( CMAKE_EXE_LINKER_FLAGS_USAN
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=undefined -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used for linking binaries during sanitize=undefined builds."
        FORCE)

SET( CMAKE_SHARED_LINKER_FLAGS_USAN
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}  -fsanitize=undefined -fno-omit-frame-pointer -O1"
        CACHE STRING "Flags used by the shared libraries linker during sanitize=undefined builds."
        FORCE)

MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_USAN
        CMAKE_C_FLAGS_USAN
        CMAKE_EXE_LINKER_FLAGS_USAN
        CMAKE_SHARED_LINKER_FLAGS_USAN )

        
        

add_custom_target( mkdir_coverage
                  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Covarge"
                  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                  COMMENT "Creates directory for covarge output"
)         


find_program( GCOVR_EXECUTABLE NAMES gcovr DOC "Programm to generate coverage reports" )

add_custom_target( coverage  
                   COMMAND "${GCOVR_EXECUTABLE}" -r . --html --html-details --exclude=".*?ThirdParty/.*?" --gcov-exclude=".*?ThirdParty/.*?" -o "${CMAKE_BINARY_DIR}/Covarge/covarge.html"
                   DEPENDS mkdir_coverage
                   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                   COMMENT "Generating covarge report to ${CMAKE_BINARY_DIR}/Covarge/covarge.html"
)        
     
add_custom_target( clean_coverage  
                   COMMAND find "${CMAKE_BINARY_DIR}" -iname "*.gcov" -or -iname "*.gcda" -exec rm {} \\$<SEMICOLON>
                   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                   COMMENT "Removing *.gcov and *.gcda files to reset all converage counter"
)

