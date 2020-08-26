#if not defined WIN32
/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#include <iostream>
#include <gnl/socket_shell.h>

#include <chrono>
#include <ctime>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

#include <pthread.h>


//#define USE_UNIX_SOCKET


using shell_type   = gnl::socket_shell;
using process_type = shell_type::process_type;
using client_type  = shell_type::client_type;

int cmd_echo( process_type  & c)
{
    auto count = c.args.size()-1;
    for(size_t i=1; i < c.args.size() ;i++)
    {
        c.out << c.args[i] + (--count == 0 ? "" : "\n");
    }

    return 0;
}

int cmd_rand( process_type  & c)
{
    c.out << std::to_string( std::rand() );
    return 0;
}

int cmd_exit(process_type  & c)
{
    c.out << "[END OF INPUT]\n";
    c.user.close();
    return 0;
}


int cmd_none(  process_type  & c )
{

    c.out << "Invalid command: \n";
    c.out << c.in.str() << "\n";
    return 1;
}


// Custom function called when a client connects
// to the shell. Use this to set any initial variables
//
void on_connect(client_type & client)
{
    std::ostringstream s;
    s << std::this_thread::get_id();
    client.set_env("THREAD_ID", s.str() );

    std::cout << "Client connected" << std::endl;

    //=====================================================
    // Add extra Env variables here if needed
    //=====================================================
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() );
    auto timestr = std::string(std::ctime(&time));
    timestr.pop_back(); // remove the '\n';
    client.set_env("CONNECTION_TIME",  timestr  );
    //=====================================================

    auto   msg  = std::string("") +
                      RED  "Welcome to the Shell! You are client ID: " GREEN  + std::to_string(client.id()) + "\n" RESET
                      RED  "- type " GREEN "exit" RED " to disconnect\n"                                RESET
                      RED  "- use: " GREEN "set VAR VALUE" RED " to set an environment variable\n"      RESET
                      RED  "- use " GREEN "${VAR}" RED " to reference the environment variable\n"       RESET
                      RED  "- type " GREEN "help" RED " to get a list of commands\n"       RESET;

    client.set_env("PROMPT", RED "user" BLUE "@" GREEN "local" BLUE ":~$ "  RESET );
    client.send(msg);
}


void on_disconnect( client_type  & c)
{
    std::cout << "Client Disconnected" << std::endl;
}



int main(int argc, char ** argv)
{

    if( argc != 2)
    {
        std::cout << "Execute the shell by: " << std::endl << std::endl;

        std::cout << "./socket_shell localhost:PORT " << std::endl;
        std::cout << "or" << std::endl;
        std::cout << "./socket_shell PATH" << std::endl;


        return 1;
    }

    shell_type S;


    S.add_command("exit", cmd_exit);
    S.add_command("rand", cmd_rand);
    S.add_command("echo", cmd_echo);


    S.add_connect_function( on_connect );
    S.add_default(cmd_none);

    S.add_disconnect_function( on_disconnect );
    // Set the PROMPT env variable. Each user
    // This variable will be copied to each connected
    // client's env variables. The default is "?>"
    // and each client can set their own
    S.set_env("PROMPT", RED "shell" GREEN " >> " RESET );

    S.start( argv[1] );
   // S.start(SOCKET_NAME);


    std::cout << "Connect to the shell from your bash terminal using:" << std::endl << std::endl;
    std::cout << "\n";
    std::cout << "netcat localhost PORT " << std::endl << std::endl;
    std::cout << "\n";
    std::cout << "netcat -U PATH" << std::endl << std::endl;


    std::this_thread::sleep_for(std::chrono::seconds(50));

    S.disconnect();
    return 0;
}

#else

int main(int arc, char ** argv)
{
    return 0;
}

#endif
