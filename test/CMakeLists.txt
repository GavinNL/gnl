
# if(JSON_Valgrind)
#     find_program(CMAKE_MEMORYCHECK_COMMAND valgrind)
#     message(STATUS "Executing test suite with Valgrind (${CMAKE_MEMORYCHECK_COMMAND})")
#     set(memcheck_command "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1 --leak-check=full")
#     separate_arguments(memcheck_command)
# endif()

#############################################################################
# Catch library with the main function to speed up build
#############################################################################


add_library(catch_main OBJECT
    "src/unit.cpp"
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

file(GLOB files "src/unit-*.cpp")
foreach(file ${files})
    get_filename_component(file_basename ${file} NAME_WE)
    string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})

    create_test( NAME ${testcase}
                 SOURCES ${file}
                 COMMAND ${testcase}
                 PUBLIC_COMPILE_FEATURES  --coverage -g -O0 -fprofile-arcs -ftest-coverage
                 PUBLIC_LINKED_TARGETS catch_main gnl::gnl --coverage -g -O0 -fprofile-arcs -ftest-coverage)
#    add_executable(${testcase} $<TARGET_OBJECTS:catch_main> ${file})
#    target_compile_definitions(${testcase} PRIVATE
#      CATCH_CONFIG_FAST_COMPILE
#      $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
#    )
#    target_compile_options(${testcase} PRIVATE
#        $<$<CXX_COMPILER_ID:MSVC>:/EHsc;$<$<CONFIG:Release>:/Od>>
#        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-deprecated;-Wno-float-equal>
#        $<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>
#    )
#    target_include_directories(${testcase} PRIVATE
#    #    "${GNL_INCLUDE}"
#        third_party/Catch2
#    )
#
#    target_link_libraries(${testcase} gnl::gnl --coverage -g -O0 -fprofile-arcs -ftest-coverage)
#    target_compile_options(${testcase} PRIVATE --coverage -g -O0 -fprofile-arcs -ftest-coverage)
#    MESSAGE("         Adding link libraries for ${testcase}: ${GNL_LIBS}  ${GNL_COVERAGE_FLAGS} ")
#
#    add_test(NAME "${testcase}_default"
#      COMMAND ${testcase} ${CATCH_TEST_FILTER}
#      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#    )
#    set_tests_properties("${testcase}_default" PROPERTIES LABELS "default")
#
#    add_test(NAME "${testcase}_all"
#      COMMAND ${testcase} ${CATCH_TEST_FILTER} "*"
#      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#    )
#    set_tests_properties("${testcase}_all" PROPERTIES LABELS "all")
#
#    if(JSON_Valgrind)
#        add_test(NAME "${testcase}_valgrind"
#          COMMAND ${memcheck_command} ${CMAKE_CURRENT_BINARY_DIR}/${testcase} ${CATCH_TEST_FILTER}
#          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#        )
#        set_tests_properties("${testcase}_valgrind" PROPERTIES LABELS "valgrind")
#    endif()
endforeach()
MESSAGE("=====================================================")
