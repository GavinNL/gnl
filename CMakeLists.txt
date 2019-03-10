#
# This CMake project is a boilerplate code for developing a C++ library
#
#
#
#
#
#
#
#
#
#


cmake_minimum_required(VERSION 3.8)

#########################################################
project(gnl  VERSION 1.0.0)
#########################################################

include(cmake/functions.cmake)

#########################################################
# Create an interface library so that we can link to
# The proper configurations.
#
# Any target that wants to use gnl should link to
# gnl::gnl.  eg:
#
#  target_link_libraries(myexe gnl::gnl)
#
#########################################################
create_header_only_library( NAME gnl
                            NAMESPACE gnl
                            PUBLIC_INCLUDE_PATHS
                                "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
                                "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated_headers>"
                            PUBLIC_COMPILE_FEATURES
                                cxx_std_11
                            )
#########################################################



#########################################################
# Other targets
#########################################################
add_custom_target( other_files
                   SOURCES
                   .travis/install.sh
                   .travis/build.sh
                   README.md
                   LICENSE
                   .gitlab-ci.yml
                   .travis.yml
                   appveyor.yml
                )
#########################################################



#########################################################
# If we hhave enabled coverage, make sure to add the
# specific flags required to calculate the coverage
#########################################################
if(GNL_ENABLE_COVERAGE)

  if(CMAKE_COMPILER_IS_GNUCC)
    option(GNL_ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" TRUE)

    add_compile_options(--coverage -g -O0 -fprofile-arcs -ftest-coverage)
  endif()

endif()
#########################################################

# Maros





#########################################################
# Add the src sub directory. Each subdirectory within
# the src folder will be compiled into its own library
#
# The Src folder contains 2 sub folders: bin and lib
#
# All subfolders within the lib folder will be compiled
# into a library. Ideally each of these libraries
# should be independent and should not depend on
# each other.
#
# All subfolders within the bin folder will be compiled
# into an executable and linked with each of the sub
# folders in src/lib.
#########################################################
add_subdirectory(examples)
#########################################################


#########################################################
# Add the test sub directory. This is where all unit tests
# will be stored. Each unit test will be compiled and
# linked with all the libraries in the src/ folder
#########################################################
enable_testing()
add_subdirectory(test)
#########################################################
