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

#ifndef GNL_SOCKET_H
#define GNL_SOCKET_H

#ifdef _MSC_VER
#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif


#include <cstdio>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <cstdint>


#if defined _MSC_VER

    #include <winsock2.h>
    #pragma comment(lib,"wsock32.lib")

#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/un.h>
#endif


#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif
namespace GNL_NAMESPACE
{

// for future use
#if 0
#ifdef _MSC_VER
enum class SocketError
{
    ACCES       =  WSAEACCES,
    //PERM      =  WSAEPERM,
    ADDRINUSE   =  WSAEADDRINUSE,
    AFNOSUPPORT =  WSAEAFNOSUPPORT,
    //AGAIN     =  WSAEAGAIN,
    ALREADY     =  WSAEALREADY,
    BADF        =  WSAEBADF,
    CONNREFUSED =  WSAECONNREFUSED,
    FAULT       =  WSAEFAULT,
    INPROGRESS  =  WSAEINPROGRESS,
    INTR        =  WSAEINTR,
    ISCONN      =  WSAEISCONN,
    NETUNREACH  =  WSAENETUNREACH,
    NOTSOCK     =  WSAENOTSOCK  ,
    TIMEDOUT    =  WSAETIMEDOUT
};
#else
enum class SocketError
{
    ACCES       =  EACCES,
    PERM        =  EPERM,
    ADDRINUSE   =  EADDRINUSE,
    AFNOSUPPORT =  EAFNOSUPPORT,
    AGAIN       =  EAGAIN,
    ALREADY     =  EALREADY,
    BADF        =  EBADF,
    CONNREFUSED =  ECONNREFUSED,
    FAULT       =  EFAULT,
    INPROGRESS  =  EINPROGRESS,
    INTR        =  EINTR,
    ISCONN      =  EISCONN,
    NETUNREACH  =  ENETUNREACH,
    NOTSOCK     =  ENOTSOCK  ,
    TIMEDOUT    =  ETIMEDOUT
};
#endif
#endif

////////////////////////////////////////////////////////////////////////////////
/// New Implementations here!
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The socket_address class
 *
 * The socket address class is used to identify the address of the
 * connected socket. It is used by the udp_socket to indicate which
 * client has sent a packet and to indicate which client to send a message
 * to.
 */
class socket_address
{
protected:

#if defined _MSC_VER
    using address_t = struct ::sockaddr_in;
#else
    using address_t = struct sockaddr_in;
#endif

public:
    socket_address()
    {
        memset( reinterpret_cast<char*>(&m_address), 0, sizeof(m_address));
    }

    socket_address(uint16_t _port)
    {
        memset( reinterpret_cast<char *>(&m_address), 0, sizeof(m_address));
        m_address.sin_family = AF_INET;
        m_address.sin_port = htons(_port);
        m_address.sin_addr.s_addr = INADDR_ANY;
    }

    socket_address(char const * ip_address, uint16_t _port)
    {
        //setup address structure
        memset( reinterpret_cast<char *>(&m_address), 0, sizeof(m_address));
        m_address.sin_family = AF_INET;
        m_address.sin_port = htons(_port);


        #if defined _MSC_VER
        m_address.sin_addr.S_un.S_addr = inet_addr(ip_address);
        #else
        m_address.sin_addr.s_addr = inet_addr(ip_address);

        #endif
    }

    operator bool()
    {
        return m_address.sin_port == 0;
    }

    /**
     * @brief native_address
     * @return
     * Returns the native address handle of the address strut
     */
    address_t const & native_address() const
    {
        return m_address;
    }

    /**
     * @brief native_address
     * @return
     * Returns the native address handle of the address strut
     */
    address_t & native_address()
    {
        return m_address;
    }

    /**
     * @brief ip
     * @return
     * Returns the ipaddress as a character string
     */
    char const * ip() const
    {
        return inet_ntoa(m_address.sin_addr);
    }

    /**
     * @brief port
     * @return
     * Returns the port number
     */
    uint16_t port() const
    {
        return ntohs(m_address.sin_port);
    }

    operator address_t()
    {
        return m_address;
        static_assert( sizeof(address_t) == sizeof(socket_address), "struct sizes are not the same");
    }
protected:
    address_t m_address;
};


class socket_base
{
public:
#if defined _WIN32
    using socket_t = SOCKET;
    using native_msg_size_input_t  = int;// the input message length type for send/recv/sendto/recvfrom
    using native_msg_size_return_t = int;// the return type for send/recv/sendto/recvfrom
    using native_raw_buffer_t      = char;
    static const socket_t     invalid_socket = INVALID_SOCKET;
    static const int          socket_error   = SOCKET_ERROR;
    static const int          msg_error      = -1;
#else
    using socket_t                 = int;    //
    using native_msg_size_input_t  = size_t; // the input message length type for send/recv/sendto/recvfrom
    using native_msg_size_return_t = ssize_t;// the return type for send/recv/sendto/recvfrom
    using native_raw_buffer_t      = void;
    static const socket_t     invalid_socket = -1;
    static const int          socket_error   = -1;
    static const ssize_t      msg_error      = -1;
#endif

    static const int          bind_error     = -1;
    static const int          listen_error   = -1;
    static const int          connect_error   = -1;

    typedef std::int32_t   msg_size_t;

    static const msg_size_t error = -1;

    socket_base() : m_fd( invalid_socket )
    {
    }

    socket_base(socket_base const & other) : m_fd(other.m_fd)
    {
    }
    socket_base(socket_base && other) : m_fd(other.m_fd)
    {
        other.m_fd=invalid_socket;
    }
    socket_base & operator=(socket_base && other)
    {
        if( this != &other)
        {
            m_fd = other.m_fd;
            other.m_fd=invalid_socket;
        }
        return *this;
    }

    bool create(int __domain, int __type, int __protocol)
    {
    #ifdef _MSC_VER
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
        {
            //printf("Failed. Error Code : %d",WSAGetLastError());
            return false;
        }
    #endif
        if ( (m_fd=socket(__domain, __type, __protocol)) == invalid_socket)
        {
            #ifdef _MSC_VER
          //  printf("Create failed with error code : %d\n" , WSAGetLastError() );
            #else
            printf("Create failed with error code : %d : %s\n" , errno,  strerror(errno) );
            #endif
            return false;
        }
        return true;
    }

    bool bind(socket_address const & addr)
    {
        auto ret = ::bind(m_fd, reinterpret_cast<struct sockaddr const*>(&addr.native_address()) , sizeof(socket_address));

        if( ret == bind_error)
        {
            #ifdef _MSC_VER
            //printf("Bind failed with error code : %d" , WSAGetLastError() );
            #else
            //printf("Bind failed with error code : %d : %s" , errno,  strerror(errno) );
            #endif
            return false;
        }
        return true;
    }


    /**
     * @brief close
     * Closes the socket. The socket will cast to false after this has been
     * called.
     */
    void close()
    {
        #ifdef _MSC_VER
            ::closesocket(m_fd);
        #else
            ::shutdown(m_fd, SHUT_RDWR);
            ::close(m_fd);
        #endif
        m_fd = invalid_socket;
    }

    /**
     * @brief native_handle
     * @return
     *
     * Returns the native handle of the socket descriptor
     */
    socket_t native_handle() const
    {
        return m_fd;
    }

    protected:
        socket_t m_fd = invalid_socket;
};

/**
 * @brief The udp_socket class
 *
 * A udp socket class to send UDP data to various clients.
 */
class udp_socket : public socket_base
{
public:

    /**
     * @brief create
     *
     * Creates the socket.  This must be called before you can bind it.
     */
    bool create()
    {
        return socket_base::create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }


    /**
     * @brief send
     * @param data - the data packet to send
     * @param length - number of bytes in the packet to send
     * @param addr - the address to send the data to
     * @return Returns socket::error if an error occurs or returns the number
     *         of bytes sent if success
     *
     *
     */
    msg_size_t  send(void const * data, size_t length, socket_address const & addr)
    {
        native_msg_size_return_t ret = ::sendto(m_fd,
                                                reinterpret_cast<native_raw_buffer_t const*>(data),
                                                static_cast<native_msg_size_input_t>(length&0xFFFFFFFF) ,
                                                0 ,
                                                reinterpret_cast<struct sockaddr const *>(&addr.native_address()),
                                                sizeof(struct sockaddr_in ));
        if ( ret == msg_error)
        {
            #ifdef _MSC_VER
            //printf("Send failed with error code : %d" , WSAGetLastError() );
            #else
            //printf("Send failed with error code : %d : %s" , errno,  strerror(errno) );
            #endif
            return msg_size_t(ret);
        }
        return msg_size_t(ret);
    }

    /**
     * @brief recv
     * @param buf - buffer to write the data into
     * @param length - maximum length of the buffer
     * @param addr -  reference to a address struct where the ip/port of teh
     *                client will be stored
     * @return returns udp_socket::error if error or the number of bytes recieved
     *
     * Recieves data from the socket
     */
    msg_size_t recv(void * buf, size_t length, socket_address & addr)
    {
#if defined _MSC_VER
        int slen =  sizeof(struct sockaddr_in);
#else
        socklen_t slen = sizeof(struct sockaddr_in);
#endif
        native_msg_size_return_t ret  = ::recvfrom( m_fd, reinterpret_cast<native_raw_buffer_t*>(buf), static_cast<native_msg_size_input_t>(length&0xFFFFFFFF), 0, reinterpret_cast<struct sockaddr *>(&addr.native_address()), &slen);

        if (ret == msg_error)
        {
            #ifdef _MSC_VER
            //printf("Recv failed with error code : %d" , WSAGetLastError() );
            #else
            //printf("Recv failed with error code : %d : %s" , errno,  strerror(errno) );
            #endif
            //return msg_error;
        }
        return msg_size_t(ret);
    }

    /**
     * @brief operator bool
     *
     * Conversion to bool. Converts to false if the socket is
     * not created or if it has an error
     */
    operator bool()
    {
        return !( ( m_fd == invalid_socket ) );
    }

};



class tcp_socket : public socket_base
{
public:

    tcp_socket() : socket_base()
    {
    }

    tcp_socket( tcp_socket && other)
    {
        m_fd       = other.m_fd;
        m_address  = other.m_address;
        other.m_fd = invalid_socket;
        memset(&other.m_address,0,sizeof(other.m_address));
    }

    tcp_socket( const tcp_socket & other) : socket_base( other)
    {
        m_fd = other.m_fd;
        memcpy(&m_address, &other.m_address, sizeof(m_address) );
    }

    tcp_socket& operator=( tcp_socket const & other)
    {
        m_fd      = other.m_fd;
        memcpy(&m_address, &other.m_address, sizeof(m_address));
        return *this;
    }

    tcp_socket& operator=( tcp_socket && other)
    {
        if( this != &other)
        {
            m_fd      = other.m_fd;
            m_address = other.m_address;
            memset(&other.m_address,0,sizeof(other.m_address));
            other.m_fd = invalid_socket;
        }
        return *this;
    }

    /**
     * @brief operator bool
     *
     * Conversion to bool. Converts to false if the socket is
     * not created or if it has an error
     */
    operator bool()
    {
        return !( ( m_fd == invalid_socket )  );
    }

    /**
     * @brief bind
     * @param port
     * @return
     *
     * Bind the socket to a port so it can start listening for incoming
     * tcp connections.
     */
    bool bind( uint16_t port)
    {
        return socket_base::bind( socket_address(port) );
    }

    /**
     * @brief connect
     * @param server - the server ip address/host name
     * @param port - the port number
     * @return
     *
     * Connect to a server
     */
    bool connect( const char * server, std::uint16_t port)
    {
        socket_address addr(server, port);

        decltype(socket_error) ret = ::connect( m_fd, reinterpret_cast<struct sockaddr*>(&addr.native_address()), sizeof( addr.native_address()));

        m_address = addr;
        if( ret == socket_error)
            return false;

        return true;
    }

    /**
     * @brief create
     * @return
     *
     * Create the socket. THis must be called before you do any actions on
     * the socket.
     */
    bool create()
    {
        return socket_base::create(AF_INET, SOCK_STREAM, IPPROTO_TCP );
    }

    /**
     * @brief listen
     * @param max_connections
     * @return
     *
     * Puts the socket into listening mode so that it can accept client
     * connections
     */
    bool listen( std::size_t max_connections)
    {
        decltype(socket_error) code = ::listen( m_fd, static_cast<int>(max_connections));

        if( code == socket_error)
        {
            return false;
        }
        return true;
    }

    /**
     * @brief accept
     * @return
     *
     * Accept a new connected client. This function blocks until
     * a client connects.
     */
    tcp_socket accept()
    {
        tcp_socket client;

        int length   = sizeof( m_address.native_address() );

#if defined _MSC_VER
        using socklen_t = int;
#endif
        client.m_fd  = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&m_address.native_address()), reinterpret_cast<socklen_t*>(&length));

        ::getpeername(client.m_fd , reinterpret_cast<struct sockaddr *>(&client.get_address().native_address()), reinterpret_cast<socklen_t*>(&length) );

        return client;

    }

    /**
     * @brief send
     * @param data
     * @param size
     * @return returns 0 if the client disconnected, otherwise returns the
     *         number of bytes sent
     *
     * Sends data to this socket.
     */
    msg_size_t send( void const * data, size_t _size)
    {

        native_msg_size_return_t ret = ::send(m_fd, reinterpret_cast<const native_raw_buffer_t*>(data), static_cast<native_msg_size_input_t>(_size&0xFFFFFFFF), 0);

        return msg_size_t(ret);
    }

    /**
     * @brief recv
     * @param data
     * @param size
     * @return  the number of bytes read, or zero if the client disconnected.
     *          returns tcp_socket::error if an error occoured
     *
     * Recieves data from the socket. This function blocks until the total
     * number of bytes have been recieved.
     */
    msg_size_t recv(void * data, size_t _size)
    {
        bool wait_for_all = true; // default for now.

        native_msg_size_return_t t = ::recv( m_fd, reinterpret_cast<native_raw_buffer_t*>(data), static_cast<native_msg_size_input_t>(_size&0xFFFFFFFF), wait_for_all ? MSG_WAITALL : 0 );

        if( t == 0 && _size != 0 ) // gracefully closed
        {
            m_fd = invalid_socket;
        }
        return msg_size_t(t);
    }

    /**
     * @brief size
     * @return
     *
     * Returns the number of bytes waiting in the buffer.
     */
    std::size_t size() const
    {
        #ifdef _MSC_VER
            u_long bytes_available;
            auto ret = ioctlsocket(m_fd, FIONREAD , &bytes_available);
            if( ret == socket_error)
                    return 0;
        #else
            int bytes_available;
            auto ret = ioctl(m_fd,FIONREAD, &bytes_available);
            if( ret == -1)
                return 0;
        #endif
        return std::size_t(bytes_available);
    }

    /**
     * @brief get_address
     * @return
     *
     * Gets the address of the socket.
     */
    socket_address get_address() const
    {
        return m_address;
    }
protected:
    socket_address m_address;
};


#if defined __linux__

/**
 * @brief The domain_stream_socket class
 *
 * Unix domain stream socket.  A unix domain socket exists as a file on the
 * filesystem. This can be opened as a file descriptor in Unix.
 */
class domain_stream_socket : public socket_base
{
public:
    /**
     * @brief operator bool
     *
     * Conversion to bool. Converts to false if the socket is
     * not created or if it has an error
     */
    operator bool()
    {
        return !( ( m_fd == invalid_socket ) );
    }

    /**
     * @brief bind
     * @param port
     * @return
     *
     * Bind the socket to a path so it can start listening for incoming
     * connections.
     */
    bool bind( const char * path )
    {

        if( !(*this) )
            create();

        struct sockaddr_un d_name;
        memset(&d_name, 0 ,sizeof(struct sockaddr_un) );
        d_name.sun_family = AF_UNIX;
        strcpy(d_name.sun_path, path);

        int ret = ::bind(m_fd, reinterpret_cast<const struct sockaddr *>(&d_name),
                       sizeof(struct sockaddr_un));

        if( ret == bind_error)
        {
            #ifdef _MSC_VER
            //printf("Bind failed with error code : %d" , WSAGetLastError() );
            #else
            //printf("Bind failed with error code : %d : %s" , errno,  strerror(errno) );
            #endif
            //exit(EXIT_FAILURE);
            return false;
        }
        return true;
    }


    /**
     * @brief unlink
     * @param path
     * @return
     *
     * Unlinks the path from the filesystem.
     */
    bool unlink(const char * path)
    {
        return ::unlink( path ) == 0;
    }

    /**
     * @brief connect
     * @param server - the server ip address/host name
     * @param port - the port number
     * @return
     *
     * Connect to a server
     */
    bool connect( const char * path)
    {
        struct sockaddr_un d_name;
        memset(&d_name, 0 ,sizeof(struct sockaddr_un) );
        d_name.sun_family = AF_UNIX;
        strcpy(d_name.sun_path, path);

        int ret = ::connect( m_fd, reinterpret_cast<struct sockaddr*>(&d_name), sizeof( d_name));

        if( ret == connect_error)
            return false;

        return true;
    }

    /**
     * @brief create
     * @return
     *
     * Create the socket. THis must be called before you do any actions on
     * the socket.
     */
    bool create()
    {
        return socket_base::create(AF_UNIX, SOCK_STREAM, 0 );
    }

    /**
     * @brief listen
     * @param max_connections
     * @return
     *
     * Puts the socket into listening mode so that it can accept client
     * connections
     */
    bool listen( std::size_t max_connections = 10)
    {
        int code = ::listen( m_fd, static_cast<int>(max_connections) );

        if( code == listen_error)
        {
            return false;
        }
        return true;
    }

    /**
     * @brief accept
     * @return
     *
     * Accept a new connected client. This function blocks until
     * a client connects.
     */
    domain_stream_socket accept()
    {
        domain_stream_socket client;

        client.m_fd = ::accept(m_fd, NULL, NULL);

        return client;

    }

    /**
     * @brief send
     * @param data
     * @param size
     * @return returns 0 if the client disconnected, otherwise returns the
     *         number of bytes sent
     *
     * Sends data to this socket.
     */
    msg_size_t send( void const * data, size_t _size)
    {
        native_msg_size_return_t ret = ::send(m_fd, static_cast<const native_raw_buffer_t*>(data), static_cast<native_msg_size_input_t>(_size), 0);
        return msg_size_t(ret);
    }

    /**
     * @brief recv
     * @param data
     * @param size
     * @return  the number of bytes read, or zero if the client disconnected.
     *          returns tcp_socket::error if an error occoured
     *
     * Recieves data from the socket. This function blocks until the total
     * number of bytes have been recieved.
     */
    msg_size_t recv(void * data, size_t _size)
    {


        native_msg_size_return_t t = ::recv( m_fd, reinterpret_cast<native_raw_buffer_t*>(data), static_cast<native_msg_size_input_t>(_size), 0 );

        if( t == 0 && _size != 0 )
        {
            m_fd = invalid_socket;
        }
        return msg_size_t(t);
    }



};
#endif

}

#endif


