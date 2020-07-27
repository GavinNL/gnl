#
# This Cmake file creates targets for better warnings
#
# Link to the following targets:
# warning::all  - displays all warnings
# warning::error - treats warnings as errors
#

add_library(gnl_warnings_all_ INTERFACE)
add_library(gnl_warning::all ALIAS gnl_warnings_all_)
target_compile_options(gnl_warnings_all_ INTERFACE -Wall -Wextra -Wpedantic)


add_library(gnl_warnings_error_ INTERFACE)
add_library(gnl_warning::error ALIAS gnl_warnings_error_)
target_compile_options(gnl_warnings_error_ INTERFACE -Werror)

set(WARNING_INSTALL_TARGET ${WARNING_INSTALL_TARGET} gnl_warnings_error_)
