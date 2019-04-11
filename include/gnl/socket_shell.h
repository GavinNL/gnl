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

#ifndef _GNL_SOCKET_SHELL_H_
#define _GNL_SOCKET_SHELL_H_

#if defined __linux__

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
#include <string>
#include <thread>
#include <functional>
#include <mutex>

#include <iostream>

#include "socket.h"


class DomainShell;

class client
{
public:
    using socket_t = gnl::domain_stream_socket;

    void run(socket_t * p_Client)
    {
        auto & Client = *m_socket;
        char buffer[256];

        while( Client && !m_exit)
        {
            auto ret = Client.recv(buffer, 255);
            if(ret == -1)
            {
                break;
            }

            buffer[ret] = 0;
            parse(buffer, Client);
        }

        if(m_onDisconnect)
            m_onDisconnect(old_client);

        Client.close();
        __erase_client(p_Client);
        //std::cout << "Client disconnected" << std::endl;
    }

    DomainShell * m_parent = nullptr;
    socket_t      m_socket;
};

class DomainShell
{
public:
    using socket_t = gnl::domain_stream_socket;

    socket_t m_Socket;

    using cmdfunction_t = std::function<std::string(std::vector<std::string>)>;
    using connectfunction_t = std::function<void(socket_t&)>;
    using disconnectfunction_t = std::function<void(socket_t&)>;
    using map_t      = std::map<std::string, cmdfunction_t >;

    DomainShell()
    {
        add_command( "set", std::bind(&DomainShell::set_cmd, this ,std::placeholders::_1) );
        add_command( "set", std::bind(&DomainShell::set_cmd, this ,std::placeholders::_1) );
    }

    std::string set_cmd(std::vector<std::string> args)
    {
        if( args.size() >= 3)
            set_var(args[1], args[2]);
        return "";
    }
    ~DomainShell()
    {
        __disconnect();
        Unlink();
    }

    /**
     * @brief Unlink
     *
     * Unlinks the current socket name
     */
    void Unlink()
    {
        std::cout << "unlinking" << std::endl;
        unlink(m_Name.c_str());
    }

    /**
     * @brief Start
     * @param name - the path of the socket to create in the filesystem
     *
     * Starts the shell and creates a socket in the filesystem which can be
     * connected to via an external application.
     */
    void Start( char const * name)
    {
        m_Name = name;
        unlink(m_Name.c_str());
        m_Socket.create();
        m_Socket.bind(m_Name.c_str());


        std::thread t1(&DomainShell::__listen, this);
        m_ListenThread = std::move(t1);
    }


    /**
     * @brief AddOnConnect
     * @param f
     *
     * Set the function to be called when a client connects.
     * The function signature has the following prototype:
     *
     * void func_name(Unix_Socket & client)
     */
    void AddOnConnect( connectfunction_t f)
    {
        m_onConnect = f;
    }

    /**
     * @brief AddOnDisconnect
     * @param f
     *
     * Set the function to be called when a client disconnects.
     * The function signature has the following prototype:
     *
     * void func_name(Unix_Socket & client)
     */
    void AddOnDisconnect( disconnectfunction_t f)
    {
        m_onDisconnect = f;
    }


    /**
     * @brief AddCommand
     * @param command_name - name of the command
     *
     * Add a command function which will be called whenever a
     * client types it into its terminal.
     *
     * The funciton prototype is:
     *
     * void func_name(Unix_Socket & client, char const * args)
     *
     * Example:
     *
     * shell.AddCommand("exit", exit_function);
     *
     *
     *
     */
    void add_command(std::string const & command_name, cmdfunction_t f)
    {
        m_cmds[command_name] = f;
    }

    /**
     * @brief AddDefaultCommand
     * @param f
     *
     * Adds a command that will be called if the client types in a command
     * that is not handled.
     */
    void AddDefaultCommand(cmdfunction_t f)
    {
        m_Default = f;
    }

    /**
     * @brief Disconnect
     * Disconnects all clients and destroy's the socket
     */
    void Disconnect()
    {
        __disconnect();
    }

private:
    void parse(const char * buffer, socket_t & client)
    {
        std::string S(buffer);
        if(S.size()>0)
        {
            auto printout = call( S );

            client.send(printout.data(), printout.size());
            client.send("\nShell>> ", 9);
        }
    }


    void __client(socket_t * p_Client)
    {
        auto & Client = *p_Client;
        //std::cout << "Client Connected" << std::endl;
        char buffer[256];

        auto old_client = *p_Client;
        if( m_onConnect )
            m_onConnect(*p_Client);

        while( Client && !m_exit)
        {
            auto ret = Client.recv(buffer, 255);
            if(ret == -1)
            {
                break;
            }

            buffer[ret] = 0;
            parse(buffer, Client);
        }

        if(m_onDisconnect)
            m_onDisconnect(old_client);

        Client.close();
        __erase_client(p_Client);
        //std::cout << "Client disconnected" << std::endl;
    }

    void __erase_client(socket_t * p)
    {
        m_Mutex.lock();
        for(int i=0; i < m_Clients.size();i++)
        {
            if(&m_Clients[i]->second == p)
            {
                std::swap( m_Clients[i] , m_Clients.back() );
                m_Clients.pop_back();
            }
        }
        m_Mutex.unlock();
    }

    void __listen()
    {
        m_Socket.listen();

        while(!m_exit)
        {
          auto Client = m_Socket.accept();

          if(!Client)
          {
              //std::cout << "Client is bad" << std::endl;
              continue;
          }
          int result = 0;

          pair_t * client = new pair_t();

          client->second = std::move(Client);

          std::thread tc( &DomainShell::__client, this, &client->second);

          client->first  = std::move(tc);

          m_Mutex.lock();
          m_Clients.push_back( client );
          m_Mutex.unlock();
        }

        //std::cout << "Listen thread exited" << std::endl;

    }


    void __disconnect()
    {
        m_exit = true;

        int i=0;

        for(auto & c : m_Clients)
        {
            //std::cout << "Disconnecting client " << i++ << std::endl;
            c->second.close();
            if(c->first.joinable())
                c->first.join();

            delete(c);
        }

        m_Clients.clear();


        if(m_ListenThread.joinable() )
            m_ListenThread.join();

        Unlink();
    }


    using pair_t = std::pair< std::thread, socket_t >;

    std::thread          m_ListenThread;
    std::string          m_Name;
    bool                 m_exit = false;
    std::mutex           m_Mutex;
    std::vector<pair_t*> m_Clients;
    map_t                m_cmds;
    cmdfunction_t        m_Default;
    connectfunction_t    m_onConnect;
    disconnectfunction_t m_onDisconnect;

public:

    static uint32_t find_closing_bracket(char const * c, char open, char close)
    {
        char const * s = c;
        int b = 0;
        while( c != 0 )
        {
            if( *c == close && b==0)
                return c-s;

            if(*c == open)  ++b;
            if(*c == close) --b;

            c++;
        }

        return 0;
    }

    std::string call(std::string cmd)
    {

        if( cmd.back() == '\n') cmd.pop_back();
        if( cmd.size() == 0)
            return "";

        uint32_t k = 0;
        while( k < cmd.size() )
        {

            if(cmd[k] == '$')
            {
                switch( cmd[k+1])
                {
                    case '(':
                    {
                        uint32_t s = find_closing_bracket( &cmd[k+2], '(', ')');
                        auto new_call = cmd.substr(k+2, s );
                        auto ret = call( new_call );
                        cmd.erase(k, s+3);
                        cmd.insert(k, ret);
                        break;
                    }
                    case '{':
                    {
                        uint32_t s = find_closing_bracket( &cmd[k+2], '{', '}');
                        auto new_call = cmd.substr(k+2, s );
                        auto ret = get_var(new_call);
                        cmd.erase(k, s+3);
                        cmd.insert(k, ret);
                        break;
                    }
                    default:
                        break;
                }
            }
            ++k;
        }
        std::cout << "calling: " << cmd << std::endl;
        auto T = tokenize(cmd);
        return execute( T );
    }

    std::string execute( std::vector< std::string > const & args )
    {
        auto f = m_cmds.find( args[0] );
        if( f != m_cmds.end() )
        {
            auto ret = f->second( args  );
            return ret;
        } else {
            return "Unknown command";
        }
    }

    std::map<std::string, std::string> m_vars;

    void set_var(std::string name, std::string value)
    {
        //std::cout << "setting var: " << name << "= " << value << std::endl;
        m_vars[name] = value;
    }
    void unset_var(std::string name)
    {
        m_vars.erase(name);
    }
    std::string get_var(std::string name)
    {
        //std::cout << "getting var: " << name << std::endl;
        auto f = m_vars.find(name);
        if( f == m_vars.end())
        {
            return "";
        } else {
            return f->second;
        }
    }

    std::vector<std::string> tokenize( std::string const & s)
    {
        std::vector<std::string> tokens;

        std::string T;
        for(int i=0;i<s.size();i++)
        {
           if( s[i]=='\\')
           {
               ++i;
               T += s[i];
               continue;
           }
           else
           {
               if( s[i] == '"')
               {
                   ++i;
                   while(s[i] != '"')
                   {
                       if( s[i] == '\\')
                       {
                            i++;
                            T += s[i];
                       }
                       else
                       {
                        T += s[i++];
                       }
                   }
               }
               else if( s[i] == ' ')
               {
                   if( T.size() != 0)
                   {
                     tokens.push_back(T);
                     T = "";
                   }
               } else {
                T += s[i];
               }
           }
        }
        if( T.size() !=0)
            tokens.push_back(T);
        return tokens;
    }

};
#else
    #error This does not work on Windows yet!
#endif

#endif
