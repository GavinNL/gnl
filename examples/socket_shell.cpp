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

#define SOCKET_NAME "mysocket.socket"

#include <iostream>
#include <gnl/socket_shell.h>

std::string cmd_echo(gnl::client & client, std::vector<std::string> const & arg)
{
    std::string s;
    for(int i=1; i < arg.size() ;i++)
    {
        s += arg[i] + "\n";
    }

    return s;
}

std::string cmd_ls( gnl::client  & c, std::vector<std::string> const & arg)
{
    std::string s("file.txt\nhello.txt");
    return s;
}

std::string cmd_rand( gnl::client  & c, std::vector<std::string> const & arg)
{
    return std::to_string( std::rand() );
}

std::string cmd_exit(gnl::client  & client, std::vector<std::string> const & arg)
{
    client.send("From Server: Good Bye!\n");
    client.close();
    return "From server: Good Bye!!\n";
}

std::string cmd_none( gnl::client  & client, std::vector<std::string> const & arg )
{
    return "invalid command";
}

void on_connect(gnl::client  & client)
{
    std::cout << "Client connected" << std::endl;

    const char msg[] = "\033[1;31mbold red text\033[0m\n"
                      "\033[1;31m"  "Welcome to the Shell!\n" "\033[0m"
                      "\033[1;31m"  "- Any command you type will be echoed back.\n" "\033[0m"
                      "\033[1;31m"  "- type 'exit' to disconnect\n" "\033[0m";

    client.send(msg);
}

void on_disconnect( gnl::client  & c)
{
    std::cout << "Client Disconnected" << std::endl;
}

int main()
{

    gnl::DomainShell S;

    S.add_command("exit", cmd_exit);
    S.add_command("ls",   cmd_ls);
    S.add_command("rand", cmd_rand);
    S.add_command("echo", cmd_echo);

    S.AddOnConnect( on_connect );
    S.AddDefaultCommand(cmd_none);

    S.AddOnDisconnect( on_disconnect );

    S.Start(SOCKET_NAME);

    std::cout << "Connect to the shell from your bash terminal using:" << std::endl << std::endl;
    std::cout << "socat - UNIX-CONNECT:" << SOCKET_NAME << std::endl << std::endl;
    std::cout << "   or " <<  std::endl << std::endl;
    std::cout << "netcat -U " << SOCKET_NAME << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    S.Disconnect();
    return 0;
}

