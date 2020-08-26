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

class socket_shell;
class shell_client;

/**
 * @brief The process_type class
 *
 * The Proc class is a container of the input/output streams,
 * and the shell client. A new process is
 */
class Proc_t
{
public:
    Proc_t(shell_client & c ) : user(c){}

    std::vector<std::string>    args;    // input arguments to the process
    std::istringstream          in;      // the input stream
    std::ostringstream          out;     // the output stream.
    shell_client                &user;     // the user's information.
};

/**
 * @brief The shell_client class
 *
 * A shell_client is created for each client connected to the socket.
 * Each shell_client contains its own env variables which are copied
 * from the main shell's env
 */
class shell_client
{
public:
    using socket_type  = gnl::socket;
    using process_type = Proc_t;
    using shell_type   = socket_shell;

    shell_client(socket_type socket, shell_type * parent) : m_socket(socket), m_parent(parent)
    {
    }

    shell_client(shell_client const & c) = delete;
    shell_client& operator = (shell_client const & c) = delete;


    size_t id() const
    {
        return m_id;
    }
    /**
     * @brief send
     * @param msg
     *
     * Send a message directly to the client. Using this inside a
     * command callback function isn't advised since the output
     * is not parsed by the shell ,therefore using ${} and $() will not work
     *
     */
    void send(std::string const & msg)
    {
        m_socket.send(msg.data(), msg.size());
    }

    /**
     * @brief close * Disconnect the client
     */
    void close()
    {
        m_exit = true;
    }

    /**
     * @brief get_var
     * @param name
     * @return
     *
     * Get an env variable
     */
    std::string get_env(std::string name) const
    {
        auto f = m_vars.find(name);
        if( f == m_vars.end())
        {
            return "";
        } else {
            return f->second;
        }
    }

    /**
     * @brief set_var
     * @param var
     * @param value
     *
     * Set an env variable
     */
    void set_env(std::string const & var, std::string const & value)
    {
        m_vars[var] = value;
    }
    void unset_env(std::string const & var)
    {
        auto f = m_vars.find(var);
        if( f != m_vars.end() )
            m_vars.erase(f);
    }
    /**
     * @brief env
     * @return
     *
     * Returns the map of environment variables.
     */
    std::map<std::string, std::string> & env()
    {
        return m_vars;
    }

    void setCustomParse( std::function< void(const char*, socket_type&)> p )
    {
        _parse = p;
    }
protected:
    std::function< void(const char*, socket_type&)> _parse;
    void        parse(const char * buffer, socket_type & client);
    std::string execute( std::string cmd );
    std::string execute( std::vector< std::string > const & args );
    void        run();
    //void        start();

    void start()
    {
        std::thread tc( &shell_client::run, this);
        m_thread = std::move(tc);
    }



    socket_type      m_socket;
    shell_type     * m_parent = nullptr;
    std::map<std::string, std::string> m_vars;
    bool          m_exit = false;
    std::thread   m_thread;
    size_t        m_id;
    friend class socket_shell;
};


class socket_shell
{
public:
    using socket_type  = gnl::socket;
    using client_type  = shell_client;
    using process_type = Proc_t;

    socket_type m_Socket;

    using cmdfunction_t        = std::function<int(process_type&)>;
    using connectfunction_t    = std::function<void(client_type&)>;
    using disconnectfunction_t = std::function<void(client_type&)>;
    using map_t                = std::map<std::string, cmdfunction_t >;

    socket_shell()
    {
        add_command( "env"  , &socket_shell::cmd_env );
        add_command( "wc"   , &socket_shell::cmd_wc );
        add_command( "set"  , &socket_shell::cmd_set );
        add_command( "unset", &socket_shell::cmd_undset );
        add_command( "help" , &socket_shell::cmd_help );

        m_vars["PROMPT"] = "?>";
    }

    //===================================================================================
    // Some default commands to behave similar to how bash works.
    //===================================================================================
    static int cmd_env( process_type  & c)
    {
        auto i = 0u;
        auto s = c.user.env().size();

        for(auto & e : c.user.env() )
        {
            c.out << e.first + '=' + e.second + (++i != s ? "\n" : "");
        }
        return 0;
    }
    static int cmd_wc( process_type  & c)
    {
        std::string line;

        int x=0;

        bool line_count=false;
        if( c.args.size() >= 2 && c.args[1] == "-l")
            line_count = true;

        while( std::getline( c.in, line) )
        {
            if(!line_count)
            {
                c.out << std::to_string(line.size()) ;
                if( !c.in.eof() )
                {
                    c.out << '\n';
                }
            }
            x++;

        }
        if(line_count) c.out << std::to_string(x);
        return 0;
    }

    static int cmd_help( process_type  & c)
    {
        auto s = c.user.m_parent->m_cmds.size();
        for(auto & x : c.user.m_parent->m_cmds)
        {
            c.out << x.first + (--s == 0 ? "" : "\n");
        }
        return 0;
    }


    static int cmd_set(process_type  & c)
    {
        if( c.args.size() >= 3)
        {
            c.user.set_env(c.args[1], c.args[2] );
        }
        return 0;
    }
    static int cmd_undset(process_type  & c)
    {
        if( c.args.size() >= 2)
        {
            c.user.unset_env( c.args[1] );
        }
        return 0;
    }
    //===================================================================================

    ~socket_shell()
    {
        _disconnect();
        unlink();
    }

    /**
     * @brief Unlink
     *
     * Unlinks the current socket name
     */
    void unlink()
    {
        ::unlink(m_Name.c_str());
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
        std::thread t1(&socket_shell::_listen, this);
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
     * @brief AddOnConnect
     * @param f
     *
     * Set the function to be called when a client connects.
     * The function signature has the following prototype:
     *
     * void func_name(Unix_Socket & client)
     */
    void add_connect_function( connectfunction_t f)
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
    void add_disconnect_function( disconnectfunction_t f)
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
    void add_default(cmdfunction_t f)
    {
        m_Default = f;
    }

    void setCustomParse( std::function< void(const char*, socket_type&)> p)
    {
        _customParse = p;
    }

    /**
     * @brief Disconnect
     * Disconnects all clients and destroy's the socket
     */
    void disconnect()
    {
        _disconnect();
    }


    int execute(process_type & c)
    {
        auto f = m_cmds.find( c.args[0] );
        if( f != m_cmds.end() )
        {
            auto ret = f->second( c );
            return ret;
        }
        else
        {
            if( m_Default)
                m_Default(c);
            return 127;
        }
    }

    /**
     * @brief set_var
     * @param name
     * @param value
     *
     * Sets the environment variable
     */
    void set_env(std::string name, std::string value)
    {
        m_vars[name] = value;
    }
    void unset_env(std::string name)
    {
        m_vars.erase(name);
    }
    std::string get_var(std::string name)
    {
        LOUT << "getting var: " << name << std::endl;
        auto f = m_vars.find(name);
        if( f == m_vars.end())
        {
            return "";
        } else {
            return f->second;
        }
    }

protected:

    void _listen()
    {
        m_Socket.listen(10);
        m_client_id_count = 0;
        while(!m_exit)
        {
            m_Socket.set_recv_timeout( std::chrono::seconds(1));
            auto client_socket = m_Socket.accept();

            if(!client_socket)
            {
                //LOUT << "Client is bad" << std::endl;
                continue;
            }

            client_socket.set_recv_timeout( std::chrono::seconds(100000));
            auto c = std::make_shared<client_type>(client_socket, this);
            c->m_vars = this->m_vars;
            c->set_env("ID", std::to_string(m_client_id_count++));
            c->set_env("SOCKET", m_Name);

            if( _customParse )
            {
                c->setCustomParse(_customParse);
            }
            c->start();

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
            c->close();
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
        m_exit = true;

        //m_Socket.close();
        if(m_ListenThread.joinable() )
            m_ListenThread.join();
        //m_Socket.close();
      //  unlink();
    }


    std::thread                         m_ListenThread;
    std::string                         m_Name;
    bool                                m_exit = false;
    std::mutex                          m_Mutex;

    std::map<std::string, std::string>  m_vars;           //<! environment variables
    std::set< std::shared_ptr<client_type> > m_ClientPointers; //<! clients
    map_t                               m_cmds;           //!< list of commands
    size_t                              m_client_id_count;

    cmdfunction_t        m_Default;
    connectfunction_t    m_onConnect;
    disconnectfunction_t m_onDisconnect;


    friend void        shell_client::parse(const char * buffer, socket_type & client);
    friend std::string shell_client::execute( std::string cmd );
    friend std::string shell_client::execute( std::vector< std::string > const & args );
    friend void        shell_client::run();
    friend void        shell_client::start();

    std::function< void(const char*, socket_type&)> _customParse;
//public:

    /**
     * @brief find_closing_bracket
     * @param c
     * @param open - the open bracket type (, <, [, {, etc
     * @param close - the corresponding closing bracket ),>,],}, etc
     * @return
     *
     * Finds the index of the closing bracket in the string
     */
    static uint32_t find_closing_bracket(char const * c, char open, char close)
    {
        char const * s = c;
        int b = 0;
        while( *c != 0 )
        {
            if( *c == close && b==0)
                return c-s;

            if(*c == open)  ++b;
            if(*c == close) --b;

            c++;
        }

        return 0;
    }


    static std::string removeNewlineChars(std::string input)
    {
        return input.erase(input.find_last_not_of(" \n\r\t")+1);
    }

    /**
     * @brief tokenize
     * @param s
     * @return
     *
     * This method could probably be more efficient
     */
    static std::vector<std::string> tokenize( std::string const & input)
    {
        auto s = removeNewlineChars(input);
        std::vector<std::string> tokens;

        std::string token;
        for(size_t i=0;i<s.size();i++)
        {
           if( s[i]=='\\')
           {
               ++i;
               token += s[i];
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
                            token += s[i];
                       }
                       else
                       {
                        token += s[i++];
                       }
                   }
               }
               else if( s[i] == ' ')
               {
                   if( token.size() != 0)
                   {
                     tokens.push_back(token);
                     token = "";
                   }
               } else {
                token += s[i];
               }
           }
        }
        if( token.size() !=0)
            tokens.push_back(token);
        return tokens;
    }

};

/**
 * @brief shell_client::run
 *
 * The main thread command for the client.
 */
inline void shell_client::run()
{
    auto & Client = m_socket;

    #define BUFFER_SIZE 4096
    char buffer[BUFFER_SIZE];

    if(m_parent->m_onConnect )
    {
        m_parent->m_onConnect( *this);
        auto p = std::string("\n") + get_env("PROMPT");
        Client.send(p.c_str(), p.size());
    }

    while( Client && !m_exit)
    {
        auto ret = Client.recv(buffer, BUFFER_SIZE, false);
        if(ret == -1)
        {
            break;
        }

        buffer[ret] = 0;

        parse(buffer, Client);
    }

    if(m_parent->m_onDisconnect)
        m_parent->m_onDisconnect(*this);

    Client.close();
}


inline void shell_client::parse(const char *buffer, shell_client::socket_type &client)
{
    if( _parse )
    {
        _parse(buffer, client);
        return;
    }
    std::string S(buffer);
    if(S.back() == '\n' && S.size() ) S.pop_back();

    if( S.size() > 0 )
    {
        auto printout = execute( S );
        m_vars["LAST_CMD"] = S;
        client.send(printout.c_str(), printout.size());
    }
    auto p = std::string("\n") + get_env("PROMPT");
    client.send(p.c_str(), p.size());
}

/**
 * @brief extract_pipes
 * @param cmd
 * @return
 *
 * Given a command such as:
 *    cmd1 ${var1} | cmd2 $( cmd3 | cmd4 )
 *
 * It will return a vector of strings:
 *
 * cmd1 ${var1}
 * cmd2 $( cmd3 | cmd4)
 *
 */
inline std::vector<std::string> extract_pipes( std::string cmd)
{
    std::vector<std::string> out;

    auto x = std::begin(cmd);

    auto start = x;
    int count=0;
    while( x != std::end(cmd) )
    {
        if( *x == '{' || *x == '(') count++;
        if( *x == '}' || *x == ')') count--;
        if( count == 0 )
        {
            if( *x == '|')
            {
                auto s = std::distance(std::begin(cmd), start);
                auto n = std::distance(start, x);
                out.push_back( std::string(start,x) );
                start = x+1;

                (void)s;
                (void)n;
            }
        }
        x++;
    }
    auto s = std::distance(std::begin(cmd), start);
    auto n = std::distance(start, x);
    out.push_back( cmd.substr(s,n ) );
    return out;
}



inline void replace_with_vars(std::string & c, std::map<std::string, std::string> const & V)
{
    auto start = std::begin(c);

    while(true)
    {
        // find the first occurance of ${
        auto s1 = std::adjacent_find( start,
                                      std::end(c),
                                      [](const char & a, const char &b)
                                      {
                                          return a=='$' && b=='{';
                                      });

        if( s1 == std::end(c) ) // we didn't find anything. break out.
            break;

        {
            int bracket_count=0;

            // find the closing bracket. Making sure to keep track of any other
            // opening brackets.
            auto e1 = std::find_if( s1+1,
                                    std::end(c),
                                    [&bracket_count](char const & a)
                                    {
                                        if(a == '{') bracket_count++;
                                        if(a == '}') bracket_count--;
                                        return bracket_count==0;
                                    }
                          );

            if( e1 != std::end(c) )
            {
                // The expression ${VARIABLE_NAME} exists between iterators
                // s1 and e1;
                auto var = std::string( s1+2 , e1);
                replace_with_vars(var,V);
                auto f = V.find(var);

                auto s = std::distance(std::begin(c),s1);
                auto l = std::distance(s1,e1)+1u;

                c.replace( s, l, f != std::end(V) ? f->second: "" );

                start = std::begin(c) + s;
            }
        }
    }

    return;
}
/**
 * @brief client::execute
 * @param cmd
 * @return
 *
 * Execute a command from a string. This will parse the commands
 * like a bash command line recurively executing anything within $( )
 * and replacing ${ } with environment variables
 */
inline std::string shell_client::execute(std::string cmd)
{

    if( cmd.back() == '\n') cmd.pop_back();
    if( cmd.size() == 0)
        return "";

    // split the command into the separate commands
    // eg:   cmd1 | cmd2 | cmd3
    auto command_list = extract_pipes(cmd);


    process_type process(*this);


    for(auto & c :  command_list)
    {

        // Replace all the ${NAME} with the appropriate variable name
        replace_with_vars(c, this->m_vars);

        // Replace all the $( cmd ) with teh putput of that command
        {
            // find the first occurance of ${
            auto s1 = std::adjacent_find( std::begin(c),
                                          std::end(c),
                                          [](const char & a, const char &b)
                                          {
                                              return a=='$' && b=='(';
                                          });
            if( s1 != std::end(c) )
            {
                int bracket_count=0;

                // find the closing bracket. Making sure to keep track of any other
                // opening brackets.
                auto e1 = std::find_if( s1+1,
                                        std::end(c),
                                        [&bracket_count](char const & a)
                                        {
                                            if(a == '(') bracket_count++;
                                            if(a == ')') bracket_count--;
                                            return bracket_count==0;
                                        }
                              );

                if( e1 != std::end(c) )
                {
                    auto s = std::distance(std::begin(c),s1);
                    auto l = std::distance(s1,e1)+1u;

                    std::string sub_cmd( s1+2, e1);
                    auto output = execute(sub_cmd);
                    c.replace(s, l, output);

                }
            }
        }

        LOUT << c.size() << " Executing: " << c << " - " << static_cast<int32_t>(c.back()) << std::endl;
        process.args = socket_shell::tokenize(c);

        auto exit_code = m_parent->execute(process);
        (void)exit_code;
        // copy the output string into the input string and call the next command

        process.in = std::istringstream(process.out.str());
        process.out= std::ostringstream();
    }

    return process.in.str();
}

} // namespace gnl

#else
#error This does not work on Windows yet!
#endif

#endif

