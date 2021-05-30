cmake_minimum_required(VERSION 3.8)

#############################################################################
# Catch library with the main function to speed up build
#############################################################################
find_package(Threads)

add_library(catch_main STATIC
    "unit.cpp"
)
set_target_properties(catch_main PROPERTIES
    COMPILE_DEFINITIONS "$<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>"
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:MSVC>:/EHsc;$<$<CONFIG:Release>:/Od>>"
)
target_compile_features(catch_main PUBLIC cxx_std_11)
target_include_directories(catch_main PUBLIC "third_party/Catch2")


#############################################################################
# one executable for each unit test file
#############################################################################
MESSAGE("\n\n")
MESSAGE("=====================================================")
MESSAGE("Collecting Test Cases                                ")
MESSAGE("=====================================================")


#############################################################################
# C++11 tests
#############################################################################
file(GLOB files "cxx_11/unit-*.cpp")

foreach(file ${files})
    get_filename_component(file_basename ${file} NAME_WE)
    string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})

    create_test( NAME ${testcase}
                 SOURCES ${file}
                 COMMAND ${testcase}
                 PUBLIC_LINKED_TARGETS catch_main gnl::gnl ${COVERAGE_TARGET} gnl_warning::all gnl_warning::error Threads::Threads)

endforeach()
#############################################################################

# Try compiling a C++17 program and see if it works.
try_compile(IS_CXX17
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_LIST_DIR}/cxx17_check.cpp
      CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17
      #OUTPUT_VARIABLE OUTPUT
      )


#############################################################################
# C++17 tests
#############################################################################
if(IS_CXX17)

    file(GLOB files "cxx_17/unit-*.cpp")

    foreach(file ${files})
        get_filename_component(file_basename ${file} NAME_WE)

        string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})

        create_test( NAME ${testcase}
                     SOURCES ${file}
                     COMMAND ${testcase}
                     PUBLIC_LINKED_TARGETS catch_main gnl::gnl ${COVERAGE_TARGET} gnl_warning::all gnl_warning::error Threads::Threads)

        target_compile_features(${testcase} PUBLIC cxx_std_17)
    endforeach()
endif()
#############################################################################


MESSAGE("=====================================================")
