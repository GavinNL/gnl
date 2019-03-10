cmake_minimum_required(VERSION 3.8)

MACRO(create_example source extra_targets)
    create_executable( NAME ${source} SOURCES ${source}.cpp PUBLIC_LINKED_TARGETS gnl::gnl ${extra_targets})
ENDMACRO()


find_package(Threads)

create_example( animate "")
create_example( domain_stream_socket Threads::Threads)
create_example( json "")
create_example( message_bus "")
create_example( multiclient_tcp_server Threads::Threads)
create_example( path "")
create_example( periodic Threads::Threads)
create_example( rk4 "")
create_example( signals Threads::Threads)
create_example( tcp_client Threads::Threads)
create_example( tcp_server_2 Threads::Threads)
create_example( thread_pool Threads::Threads)
create_example( udp_server Threads::Threads)
#create_executable( NAME animate SOURCES animate.cpp PUBLIC_LINKED_TARGETS gnl::gnl)

# SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
#
# MESSAGE("\n\n")
# MESSAGE("=====================================================")
# MESSAGE(" Collecting Executables                              ")
# MESSAGE("=====================================================")
# FOREACH(subdir ${SUBDIRS})
#
#     if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt")
#
#         MESSAGE("Custom CMakeLists.txt file found for executable: ${subdir} ")
#         add_subdirectory(${subdir})
#
#     else()
#
#         MESSAGE("Compiling Module ${subdir} into executable" )
#
#         include_directories( ${GNL_INCLUDE} )
#
#         FILE(GLOB_RECURSE  SRC_FILES  "${subdir}/*.cpp" )
#
#         add_executable(${subdir} ${SRC_FILES})
#         target_link_libraries( ${subdir} ${GNL_LIBS} ${GNL_COVERAGE_FLAGS})
#
#     endif()
#
# ENDFOREACH()
# MESSAGE("=====================================================")
#
