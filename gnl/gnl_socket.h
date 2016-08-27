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

#ifdef _MSC_VER

    #include <winsock2.h>
    #pragma comment(lib,"wsock32.lib")
    typedef int socklen_t;

#else
    #include <cstring>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <pthread.h>
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

enum class SocketProtocol
{
    TCP,
    UDP
};

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


        /**
         * @brief Socket
         *
         * Construct the socket. Constructing this way requires
         * you to call Create( ) to set the protocol
         */
        Socket();

        /**
         * @brief Socket
         * @param p - the protocol to use
         *
         * Construct the socket and set the protocol to be used. Constructing
         * this way does not require you to call Create( )
         */
        Socket(SocketProtocol p);

        ~Socket();

        /**
         * @brief Create
         * @param p - the protocl to use
         * @return
         *
         * Sets the sock to use the appropriate protocol.
         * Default is TCP
         */
        bool Create(SocketProtocol p = SocketProtocol::TCP);

        /**
         * @brief Bind
         * @param port - port number to bind the socket to.
         * @return
         *
         * Bind the socket to a port so it can start listening. Call Listen after this.
         */
        bool Bind(unsigned short port);

        /**
         * @brief Listen
         * @param MaxConnections
         * @return - true if the socket was able to be put in listen mode
         *
         * Starts listening on the bound port. Retu
         */
        bool Listen(unsigned int MaxConnections=64);

        /**
         * @brief Accept
         * @param socket - pointer to another socket
         * @return
         *
         * Accepts a client connection. Blocks until a client connects.
         */
        bool Accept( Socket* socket );


        /**
         * @brief Connect
         * @param host -  host address to connect to
         * @param port - port to connect to
         * @return - true if connected
         *
         * Connects to a remote socket.
         */
        bool  Connect( const char * host, unsigned short port);


        /**
         * @brief Close
         * Closes the connection to the socket.
         */
        void Close();


        /**
         * @brief IsError
         * @return true if there is an error
         *
         * Checks for any errors on the current socket.
         */
        bool IsError();


        /**
         * @brief IsValid
         * @return
         *
         * Determins if the socket is valid.
         */
        bool IsValid();


        /**
         * @brief HasBytes
         * @return - true if bytes are available to be read.
         *
         * Checks if the socket has bytes available.
         */
        bool HasBytes();


        /**
         * @brief Receive
         * @param buffer - buffer to write the bytes to
         * @param size - number of bytes to read
         * @param spos
         *
         * @return the number of bytes read, or 0 if the client disconnected
         */
        int  Receive(const void* buffer,    int size, int spos = 0);

        /**
         * @brief SendRaw
         * @param data - data to send
         * @param dataSize - amount of bytes to send
         * @return
         */
        int  SendRaw(const void* data,      int dataSize);



        int  SendUDP(const void* buffer,    int size, sockaddr_in* to  );
        int  ReceiveUDP(const void* buffer, int size, sockaddr_in* from);


        long Address();
        bool ClientAvailable();

    private:
        #ifdef _MSC_VER
        WSADATA wsda;
        #endif
        SocketFD           sock;

        bool               blocking;
        bool               Valid;

        struct sockaddr_in addr;
        struct sockaddr_in fromAddr;

        SocketState        __state = SocketState::Disconnected;

        fd_set  __scks;
        timeval __times;

        //unsigned int totaldata;
        bool create(int Protocol, int Type);
        bool check();
};


inline Socket::Socket() : sock(INVALID_SOCKET), blocking(true), Valid(false)
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

inline Socket::Socket(SocketProtocol p) : sock(INVALID_SOCKET), blocking(true), Valid(false)
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

    Create(p);
}

inline Socket::~Socket()
{
	if (this->check())
        Close();

}

inline bool Socket::check()
{
    return sock != INVALID_SOCKET;
}

inline bool Socket::IsValid()
{
    return sock != INVALID_SOCKET;
}

inline bool Socket::Create(SocketProtocol p)
{
    switch (p)
    {
        case SocketProtocol::TCP: return this->create(IPPROTO_TCP, SOCK_STREAM);
        case SocketProtocol::UDP: return this->create(IPPROTO_UDP, SOCK_DGRAM);
        default                 : return false;
    }
}


inline bool Socket::create(int Protocol, int Type)
{
    printf("Socket::Create()\n");
    printf("   Checking\n");

    if ( this->check() )
    {
		return false;
    }

    __state = SocketState::Disconnected;

  //  printf("Creating socket: type: %d,  protocol: %d\n", Type, Protocol);

	sock = ::socket(AF_INET, Type, Protocol);

    //lastCode = sock;

    // if( sock == INVALID_SOCKET )
    // {
    //     printf("Socket create failed\n");
    // } else {
    //     printf("Listen Socket created\n");
    // }

    return sock != INVALID_SOCKET;
}

inline bool Socket::Bind(unsigned short port)
{
    if ( !check() )
    {
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
        throw std::runtime_error("Socket Bind error");
        printf("Error binding socket\n");
    }
    printf("Last code: %d\n", lastCode);
    return lastCode == 0;
}

inline bool Socket::Listen(unsigned int MaxConnections)
{
    auto lastCode = ::listen(sock, MaxConnections);

    if (lastCode == SOCKET_ERROR)
    {
        throw std::runtime_error("Socket error");
        return false;
    }

     __state = SocketState::Listening;

	this->Valid = true;

	return true;
}

inline bool Socket::ClientAvailable()
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);
    timeval timeout;
    timeout.tv_sec = 0;  // Zero timeout (poll)
    timeout.tv_usec = 1;
    if( ::select(sock, &readSet, NULL, NULL, &timeout) == 1)
    {
        return true;
    }
    return false;
}


inline bool Socket::Accept( Socket* socket )
{
    if (!blocking && !HasBytes()) return false;

    int length   = sizeof(socket->addr);
	socket->sock = ::accept(sock, (struct sockaddr*) &socket->addr, (socklen_t*)&length);

    if( fcntl(socket->sock, F_GETFL) & O_NONBLOCK) {
        std::cout << " socket is non-blocking " << std::endl;
    }

	if (socket->sock == SOCKET_ERROR)
    {
        throw std::runtime_error("Socket Accept error");
        return false;
    }

    socket->__state = SocketState::Connected;
	return true;
}

inline void Socket::Close()
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

inline long Socket::Address()
{
	return addr.sin_addr.s_addr;
}

inline bool Socket::Connect(const char* host, unsigned short port)
{
    if ( !IsValid() )
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

	this->Valid = true;
    std::cout << "Socket Connected Successfully" << std::endl;
    return true;
}

inline bool Socket::HasBytes()
{
    FD_ZERO(&__scks);
    FD_SET( (unsigned)sock, &__scks );

    auto d = select( (int)(sock + 1) , &__scks, NULL, NULL, &__times);
    printf("Bytes available? %d \n", d);
    return d > 0;
}

//inline bool Socket::CanRead()
//{
//    FD_ZERO(&__scks);
//    FD_SET( (unsigned)sock, &__scks );
//
//    auto d = select( (int)(sock + 1) , &__scks, NULL, NULL, &__times);
//
//    printf("Bytes available? %d \n", d);
//    return d > 0;
//}

inline bool Socket::IsError()
{
    // char c;
    // ssize_t x = recv(sock, &c, 1, MSG_PEEK);
    // printf("%x\n", (unsigned int)x);
    // return x == -1;


    if (__state == SocketState::Error || sock == -1)
		return true;

    FD_ZERO(&__scks);
    FD_SET( (unsigned)sock, &__scks);

    if( select( (int)(sock + 0*1), NULL, NULL, &__scks, &__times) >= 0)
		return false;

    __state = SocketState::Error;
	return true;
}

inline int Socket::ReceiveUDP(const void* buffer, int size, sockaddr_in* from)
{
#ifdef _MSC_VER
	int client_length = (int)sizeof(struct sockaddr_in);
#else
    socklen_t client_length = (int)sizeof(struct sockaddr_in);
#endif
	return recvfrom(this->sock, (char*)buffer, size, 0, (struct sockaddr*)from, &client_length);
}


inline int Socket::Receive( const void* buffer, int size, int spos )
{
    auto t = recv( sock, (char*)buffer + spos, size, MSG_WAITALL );

    //if( t == 0 )
    //{
    //    std::cout << "Receive got " << t << " bytes" << std::endl;
    //    Close();
    //}

    return t;
}

inline int Socket::SendUDP( const void* buffer, int size, sockaddr_in* to )
{
    return sendto( sock, (char*)buffer, size, 0, (struct sockaddr *)&to, sizeof(struct sockaddr_in) );
}

inline int Socket::SendRaw(const void* data, int dataSize)
{
    return send(sock, (char*)data, dataSize, 0);
}

}

#endif


