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
#create_example( reference_var "")
target_compile_features(message_bus PUBLIC cxx_std_17)

