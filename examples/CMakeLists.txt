cmake_minimum_required(VERSION 3.8)

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

MESSAGE("\n\n")
MESSAGE("=====================================================")
MESSAGE(" Collecting Executables                              ")
MESSAGE("=====================================================")
FOREACH(subdir ${SUBDIRS})
  MESSAGE("Compiling Module ${subdir} into executable" )

  include_directories( ${GNL_INCLUDE} )

  FILE(GLOB_RECURSE  SRC_FILES  "${subdir}/*.cpp" )

  add_executable(${subdir} ${SRC_FILES})
  target_link_libraries( ${subdir} ${GNL_LIBS} ${GNL_COVERAGE_FLAGS})


ENDFOREACH()
MESSAGE("=====================================================")
