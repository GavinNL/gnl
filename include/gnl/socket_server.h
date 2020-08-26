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

#pragma once

#ifndef _GNL_SOCKET_SERVER_H_
#define _GNL_SOCKET_SERVER_H_

#if not defined WIN32

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <vector>
#include <map>
#include <set>
#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <algorithm>

#include <iostream>
#include <sstream>

#include "socket.h"


#define LOUT if(1) std::cout

namespace gnl
{


class socket_server
{
public:
    using socket_type  = gnl::socket;
    socket_type m_Socket;

    socket_server()
    {
    }


    /**
     * @brief start
     * @param socket_already_bound
     *
     * Start the shell using an already bound socket.
     */
    void start( socket_type && socket_already_bound)
    {
        m_Socket = std::move(socket_already_bound);
        std::thread t1(&socket_server::_listen, this);
        m_ListenThread = std::move(t1);
    }

    /**
     * @brief start
     * @param endpoint
     *
     * endpoint should be either a filepath, or a ipaddress:port
     */
    void start(const std::string endpoint)
    {
        socket_type sock;

        if( sock.bind(endpoint) )
        {
            start( std::move(sock) );
            return;
        }
        throw std::runtime_error("Failed to bind");
    }

    socket_type& getSocket()
    {
        return m_Socket;
    }

    /**
     * @brief Disconnect
     * Disconnects all clients and destroy's the socket
     */
    void disconnect()
    {
        _disconnect();
    }

    void setClientFunction(std::function< bool(socket_type&) > c)
    {
        _clientFunction = c;
    }
protected:

    std::function< bool(socket_type&) > _clientFunction;

    void _listen()
    {
        m_Socket.listen(10);
        m_client_id_count = 0;

        m_exit = std::make_shared<bool>();
        *m_exit = false;
        while(! (*m_exit) )
        {
            m_Socket.set_recv_timeout( std::chrono::seconds(1));
            auto client_socket = m_Socket.accept();

            if(!client_socket)
            {
                continue;
            }

            std::cout << "Client connected " << std::endl;
            client_socket.set_recv_timeout( std::chrono::seconds(100000));

            auto c = std::make_shared<client_type>();
            c->m_function = _clientFunction;
            c->m_exit = m_exit;
            c->m_socket = std::move(client_socket);
            c->m_thread = std::thread(
                        [c]()
                        {
                            c->m_function(c->m_socket);
                            c->m_socket.close();
                        }
                        );
            {
                std::unique_lock< std::mutex>  L(m_Mutex);
                clean_clients();
                m_ClientPointers.insert(c);
            }
        }

        int i=0;
        for(auto & c : m_ClientPointers)
        {
            LOUT << "Disconnecting client " << i++ << std::endl;
            c->m_socket.close();
        }

        clean_clients();
        m_ClientPointers.clear();

        m_Socket.close();

    }

    void clean_clients()
    {
        auto it = std::begin(m_ClientPointers);
        while( it != std::end(m_ClientPointers))
        {
            if( (*it)->m_thread.joinable())
                (*it)->m_thread.join();
            it = m_ClientPointers.erase(it);
        }
    }

    void _disconnect()
    {
        *m_exit = true;

        //m_Socket.close();
        if(m_ListenThread.joinable() )
            m_ListenThread.join();
        //m_Socket.close();
      //  unlink();
    }


    struct client_type
    {
        std::thread m_thread;
        gnl::socket m_socket;
        std::shared_ptr<bool> m_exit;
        std::function< bool(socket_type&) > m_function;
    };

    std::thread                         m_ListenThread;
    std::shared_ptr<bool>               m_exit;
    std::mutex                          m_Mutex;

    std::set< std::shared_ptr<client_type> > m_ClientPointers; //<! clients
    size_t                              m_client_id_count;


};

} // namespace gnl

#else
#error This does not work on Windows yet!
#endif

#endif

