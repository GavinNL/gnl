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

#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <cstdio>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <cstdint>

#ifdef _MSC_VER

    #include <winsock2.h>
    #pragma comment(lib,"wsock32.lib")
    typedef int socklen_t;

#else
    #include <cstring>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <pthread.h>
    #include <sys/select.h>
#endif

#if !defined(SOCKET_ERROR)
    #define SOCKET_ERROR -1
#endif

#if !defined(SOCKET_NONE)
    #define SOCKET_NONE 0
#endif

#if !defined(INVALID_SOCKET)
    #define INVALID_SOCKET -1
#endif

namespace gnl
{


enum class SocketState
{
    Disconnected = 0,
    Listening,
    Connected,
    Error
};

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



/**
 * @brief The Socket class
 *
 * A socket wrapper class around linux sockets and winsock.
 *
 * Implements very simple blocking calls.
 */
class Socket
{
    #ifdef _MSC_VER
    using SocketFD = SOCKET;
    #else
    using SocketFD = int;
    #endif

    public:

        enum class Protocol
        {
            TCP,
            UDP
        };

        struct Address
        {
            sockaddr_in addr;
          //  SOCKADDR_IN addr;
        };

        /**
         * @brief Socket
         *
         * Construct the socket. Constructing this way requires
         * you to call Create( ) to set the protocol
         */
        Socket();

        Socket(const Socket & S)
        {
            if( &S != this)
            {
                #ifdef _MSC_VER
                wsda = S.wsda;
                #endif
                sock     = S.sock      ;
                addr     = S.addr      ;
                fromAddr = S.fromAddr  ;
                __state  = S.__state   ;
                __scks   = S.__scks    ;
                __times  = S.__times   ;
            }
        }


        Socket( Socket && S )
        {
            #ifdef _MSC_VER
            wsda = S.wsda;
            #endif
            sock     = S.sock      ;
            addr     = S.addr      ;
            fromAddr = S.fromAddr  ;
            __state  = S.__state   ;
            __scks   = S.__scks    ;
            __times  = S.__times   ;

            S.sock = INVALID_SOCKET;
        }

        Socket& operator = (const Socket & S)
        {
            if( &S != this)
			{
		        #ifdef _MSC_VER
		        wsda = S.wsda;
		        #endif
		        sock     = S.sock      ;
		        addr     = S.addr      ;
		        fromAddr = S.fromAddr  ;
		        __state  = S.__state   ;
		        __scks   = S.__scks    ;
		        __times  = S.__times   ;
			}
            return *this;
        }

        Socket& operator = ( Socket && S)
        {
			if( &S != this)
			{
		        #ifdef _MSC_VER
		        wsda = S.wsda;
		        #endif
		        sock     = S.sock      ;
		        addr     = S.addr      ;
		        fromAddr = S.fromAddr  ;
		        __state  = S.__state   ;
		        __scks   = S.__scks    ;
		        __times  = S.__times   ;

		        S.sock    = INVALID_SOCKET;
			}
            return *this;
        }

        /**
         * @brief Socket
         * @param p - the protocol to use
         *
         * Construct the socket and set the protocol to be used. Constructing
         * this way does not require you to call Create( )
         */
        Socket(Protocol p);

        ~Socket();

        /**
         * @brief Create
         * @param p - the protocl to use
         * @return
         *
         * Sets the sock to use the appropriate protocol.
         * Default is TCP
         */
        bool create(Protocol p  );

        /**
         * @brief Bind
         * @param port - port number to bind the socket to.
         * @return
         *
         * Bind the socket to a port so it can start listening. Call Listen after this.
         */
        bool bind(unsigned short port);

        /**
         * @brief Listen
         * @param MaxConnections
         * @return - true if the socket was able to be put in listen mode
         *
         * Starts listening on the bound port. Retu
         */
        bool listen(unsigned int MaxConnections=64);

        /**
         * @brief Accept
         * @param socket - pointer to another socket
         * @return
         *
         * Accepts a client connection. Blocks until a client connects.
         */
        bool   accept(Socket *socket) const;

        /**
         * @brief Accept
         * @return A socket to the connected client.
         */
        Socket accept() const;


        /**
         * @brief Connect
         * @param host -  host address to connect to
         * @param port - port to connect to
         * @return - true if connected
         *
         * Connects to a remote socket.
         */
        bool  connect( const char * host, unsigned short port);


        /**
         * @brief Close
         * Closes the connection to the socket.
         */
        void close();


        /**
         * @brief IsError
         * @return true if there is an error
         *
         * Checks for any errors on the current socket.
         */
        bool is_error();


        /**
         * @brief operator bool
         * Determines if the socket is valid
         */
        operator bool() const
        {
            return ( sock!=SOCKET_ERROR) && (sock!=INVALID_SOCKET);
        }

        /**
         * @brief BytesAvailable
         * @return the number of bytes available to read
         *
         */
        std::size_t has_bytes() { return bytes_available(); }

        int bytes_available()
        {


            #ifdef _MSC_VER
                u_long bytes_available;
                auto ret = ioctlsocket(sock, FIONREAD , &bytes_available);
            #else
                int bytes_available;
                auto ret = ioctl(sock,FIONREAD, &bytes_available);
            #endif

            if(ret == -1)
            {
                std::cout << "Error collecting byte information" << std::endl;
                exit(1);
            }

            return bytes_available;

        }

        /**
         * @brief Receive
         * @param buffer - buffer to write the bytes to
         * @param size - number of bytes to read
         * @param spos
         *
         * @return the number of bytes read, or 0 if the client disconnected
         */
        int  recv( void * buffer,    int size, bool wait_for_all = false );

        /**
         * @brief SendRaw
         * @param data - data to send
         * @param dataSize - amount of bytes to send
         * @return
         */
        int  send(const void* data,      int dataSize);


        int  SendUDP(   const void * buffer,    int size, sockaddr_in* to  );
        int  ReceiveUDP(      void * buffer,    int size, sockaddr_in* from);


        std::uint32_t Address();

        /**
         * @brief ClientAvailable
         * @return
         * Not sure if this works yet
         */
        bool client_available();

        SocketState state() const { return __state; }
    private:
        #ifdef _MSC_VER
        WSADATA            wsda;
        #endif
        SocketFD           sock;

        struct sockaddr_in addr;
        struct sockaddr_in fromAddr;

        SocketState        __state = SocketState::Disconnected;

        fd_set             __scks;
        timeval            __times;


        bool create(int Protocol, int Type);
};


inline Socket::Socket() : sock(INVALID_SOCKET)
{
	memset(&addr, 0, sizeof(addr));

#ifdef _MSC_VER
    auto result = WSAStartup(MAKEWORD(2, 2), &wsda);
    if( result != NO_ERROR)
    {
        printf("Error! starting WinSock\n");
    }

#endif

    __state = SocketState::Disconnected;
    __times.tv_sec  = 0;
    __times.tv_usec = 0;
}

inline Socket::Socket(Protocol p) : sock(INVALID_SOCKET)
{
    memset(&addr, 0, sizeof(addr));

#ifdef _MSC_VER
    auto result = WSAStartup(MAKEWORD(2, 2), &wsda);
    if( result != NO_ERROR)
    {
        printf("Error! starting WinSock\n");
    }

#endif

    __state = SocketState::Disconnected;
    __times.tv_sec  = 0;
    __times.tv_usec = 0;

    create(p);
}

inline Socket::~Socket()
{
    if (*this)
        close();

}

//inline bool Socket::check()
//{
//    return sock != INVALID_SOCKET;
//}
//
//inline bool Socket::IsValid()
//{
//    return sock != INVALID_SOCKET;
//}

inline bool Socket::create(Protocol p)
{
    __state = SocketState::Disconnected;
    switch (p)
    {
        case Protocol::TCP: return this->create(IPPROTO_TCP, SOCK_STREAM);
        case Protocol::UDP: return this->create(IPPROTO_UDP, SOCK_DGRAM);
        default                 : return false;
    }
}


inline bool Socket::create(int protocol, int Type)
{
    __state = SocketState::Disconnected;

    sock = ::socket(AF_INET, Type, protocol);

    return sock != INVALID_SOCKET;
}

inline bool Socket::bind(unsigned short port)
{
    if ( !(*this) )
    {
        // Socket has not been created.
        return false;
    }

    addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

    #ifdef _MSC_VER
    auto lastCode           = ::bind(sock, (SOCKADDR *)&addr, sizeof(addr));
    #else
    auto lastCode           = ::bind(sock, (sockaddr  *)&addr, sizeof(addr));
    #endif

    if(lastCode == SOCKET_ERROR)
    {
        //throw std::runtime_error("Socket Bind error");
        return false;
    }
    //printf("Last code: %d\n", lastCode);
    return lastCode == 0;
}

inline bool Socket::listen(unsigned int MaxConnections)
{
    auto lastCode = ::listen(sock, MaxConnections);

    if (lastCode == SOCKET_ERROR)
    {
        //throw std::runtime_error("Socket error");
        return false;
    }

     __state = SocketState::Listening;



	return true;
}

inline bool Socket::client_available()
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);
    timeval timeout;

    timeout.tv_sec = 0;  // Zero timeout (poll)
    timeout.tv_usec = 1;

    if( ::select( sock, &readSet, NULL, NULL, &timeout) == 1)
    {
        return true;
    }
    return false;
}

inline Socket Socket::accept( ) const
{
    Socket socket;
    int length   = sizeof(socket.addr);

    socket.sock = ::accept(sock, (struct sockaddr*) &socket.addr, (socklen_t*)&length);

    if (socket.sock == SOCKET_ERROR)
    {
        //throw std::runtime_error("Socket Accept error");
        return socket;
    }

    socket.__state = SocketState::Connected;
    return socket;
}

inline bool Socket::accept( Socket * socket ) const
{

    int length   = sizeof(socket->addr);

    socket->sock = ::accept(sock, (struct sockaddr*) &socket->addr, (socklen_t*)&length);

    if (socket->sock == SOCKET_ERROR)
    {
        //throw std::runtime_error("Socket Accept error");
        return false;
    }

    socket->__state = SocketState::Connected;
    return true;
}

inline void Socket::close()
{
    __state = SocketState::Disconnected;

#ifdef _MSC_VER
	::closesocket(sock);
#else
	::shutdown(sock, SHUT_RDWR);
	::close(sock);
#endif

	sock = INVALID_SOCKET;
}

inline uint32_t Socket::Address()
{
#ifdef _MSC_VER
    return static_cast<std::uint32_t>(addr.sin_addr.S_un.S_addr);
#else
    return static_cast<std::uint32_t>(addr.sin_addr.s_addr);
#endif
}

inline bool Socket::connect(const char* host, unsigned short port)
{
    if ( ! (*this) )
        return false;

    struct hostent* hostname;

    hostname = gethostbyname(host);

    if (hostname == NULL)
        return 1;

    memcpy(&addr.sin_addr, hostname->h_addr, sizeof(struct in_addr));

	addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    if( ::connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR )
        return false;

    __state = SocketState::Connected;

    return true;
}


inline bool Socket::is_error()
{

    if (__state == SocketState::Error || sock == -1)
		return true;

    FD_ZERO(&__scks);
    FD_SET( (unsigned)sock, &__scks);

    if( select( (int)(sock + 0*1), NULL, NULL, &__scks, &__times) >= 0)
		return false;

    __state = SocketState::Error;
	return true;
}

inline int Socket::ReceiveUDP(void *buffer, int size, sockaddr_in* from)
{
#ifdef _MSC_VER
	int client_length = (int)sizeof(struct sockaddr_in);
#else
    socklen_t client_length = (int)sizeof(struct sockaddr_in);
#endif
	return recvfrom(this->sock, (char*)buffer, size, 0, (struct sockaddr*)from, &client_length);
}


inline int Socket::recv(void * buffer, int size , bool wait_for_all)
{
    auto t = ::recv( sock, (char*)buffer, size, wait_for_all ? MSG_WAITALL : 0 );
    if( t == 0)
    {
        std::cout << "Socket error" << std::endl;
        sock = INVALID_SOCKET;
        __state = SocketState::Disconnected;
    }
    return t;
}

inline int Socket::SendUDP( const void * buffer, int size, sockaddr_in* to )
{
#ifdef _MSC_VER
    return sendto( sock, static_cast<const char*>(buffer), size, 0, (struct sockaddr *)&to, sizeof(struct sockaddr_in) );
#else
    return sendto( sock, buffer, size, 0, (struct sockaddr *)&to, sizeof(struct sockaddr_in) );
#endif
}

inline int Socket::send(const void* data, int dataSize)
{
    return ::send(sock, static_cast<const char*>(data), dataSize, 0);
}

}

#endif


