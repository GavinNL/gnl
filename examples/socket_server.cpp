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
#include <gnl/socket_server.h>

#include <chrono>
#include <ctime>


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

    gnl::socket_server S;

    S.setClientFunction(
                [](gnl::socket & S) -> bool
                {
                    while(S)
                    {
                        char buff[1024];
                        auto s = S.recv(buff, 1024, false);
                        std::cout << s << std::endl;

                        if( s != -1)
                        {
                            buff[s] = 0;
                            std::cout << "Rec: " << buff;
                         //   S.send( buff, s);
                        }
                        else
                        {
                            std::cout << "asdf" << std::endl;
                        }

                    }
                    return true;
                });
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
