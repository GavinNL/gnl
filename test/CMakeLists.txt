cmake_minimum_required(VERSION 3.8)

# if(JSON_Valgrind)
#     find_program(CMAKE_MEMORYCHECK_COMMAND valgrind)
#     message(STATUS "Executing test suite with Valgrind (${CMAKE_MEMORYCHECK_COMMAND})")
#     set(memcheck_command "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1 --leak-check=full")
#     separate_arguments(memcheck_command)
# endif()

#############################################################################
# Catch library with the main function to speed up build
#############################################################################


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


file(GLOB files "unit-*.cpp")
foreach(file ${files})
    get_filename_component(file_basename ${file} NAME_WE)
    string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})

    create_test( NAME ${testcase}
                 SOURCES ${file}
                 COMMAND ${testcase}
                 PUBLIC_LINKED_TARGETS catch_main gnl::gnl ${COVERAGE_TARGET})

endforeach()

target_compile_features(test-vp_variant PUBLIC cxx_std_17)
target_compile_features(test-message_bus2 PUBLIC cxx_std_17)

MESSAGE("=====================================================")
