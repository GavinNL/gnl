project(gnl)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

FILE(GLOB_RECURSE HeaderFiles    "./*.h"
                                 "./*.cpp"
                                 "./*.md"
                                 ".gitignore")

add_custom_target(__SOURCE_FILES    SOURCES ${HeaderFiles})

include_directories ("${PROJECT_SOURCE_DIR}")

IF (WIN32)
  # set stuff for windows
  add_definitions(
      /W3
      )
ELSE()
  # set stuff for other systems
  ADD_DEFINITIONS(
      -std=c++11
      -Wfloat-equal
      -Werror=shadow
      -Wpointer-arith
      -Wcast-align
      -Wunused-variable
      -Wstrict-overflow=5
      -Wwrite-strings
      -Wcast-qual
      -Werror=switch-default
      -Wunreachable-code
      -Werror
      -Wall
      -Wextra
      -Wno-unused-parameter
      -Werror=return-type
      -Werror=cast-qual
      -pedantic-errors
      -Wfatal-errors
  )
ENDIF()


# Finds all the .cpp files in the "subdir" folder and compiles
# each of them into an executable
MACRO(COMPILE_SUBDIR_EXECS subdir)

    FILE(GLOB children relative ${CMAKE_CURRENT_LIST_DIR} ${subdir}/*.cpp)
    SET(dirlist "")

    FOREACH( child ${children} )

        IF(NOT IS_DIRECTORY ${child})
            string( REPLACE ${CMAKE_CURRENT_LIST_DIR}/${subdir}/ "" child ${child})
            string( REPLACE ".cpp" "" child ${child} )

            MESSAGE(---${child}---)

            add_executable( ${child}   ${subdir}/${child}.cpp)
            IF (WIN32)
              # set stuff for windows
            ELSE()
              target_link_libraries ( ${child}   pthread)
            ENDIF()

            set_target_properties( ${child}
                PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${subdir}"
            )

        ENDIF()

    ENDFOREACH()

ENDMACRO()


COMPILE_SUBDIR_EXECS(examples)
COMPILE_SUBDIR_EXECS(tests)


