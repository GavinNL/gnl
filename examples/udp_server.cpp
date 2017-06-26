#include <iostream>
#include <thread>
#include <mutex>
#include <gnl/gnl_socket.h>

#define PORT 30001
#define BUFLEN 256
#define SERVER "127.0.0.1"
int client1()
{
  gnl::Socket S;

  S.Create( gnl::Socket::Protocol::UDP ); // no arguemts = TCP by default


  sockaddr_in addr;
  struct hostent* hostname;

  hostname = gethostbyname("localhost");

  if (hostname == NULL)
      return 1;

  memcpy(&addr.sin_addr, hostname->h_addr, sizeof(struct in_addr));

  addr.sin_family = AF_INET;
  addr.sin_port   = htons(PORT);


  std::cout << "Client Started" << std::endl;

  while( S )
  {
    int i = 0;

    char data[] = "Gavin";

    int size = S.SendUDP(data, 5, &addr);

    printf("%d bytes sent\n", size);

    std::this_thread::sleep_for( std::chrono::seconds(1));
  }


  // close all connections and wait for all threads to finish


  return 0;
}



int server1( )
{
  gnl::Socket S;

  S.Create( gnl::Socket::Protocol::UDP ); // no arguemts = TCP by default

  while( !S.Bind(PORT) )
  {
    std::cout << "Cannot bind. Retrying in 3 seconds" << std::endl;
    std::this_thread::sleep_for( std::chrono::seconds(3) );
  }

  std::cout << "Server Started" << std::endl;

  // Listen for new clients
  char data[256];
  while( S )
  {
    int i=0;

    sockaddr_in addr;
    int size = S.ReceiveUDP( data, 256,  &addr);

    S.SendUDP(data, size, &addr);
    printf("%d bytes rec\n", size);

  }


  // close all connections and wait for all threads to finish


  return 0;
}





class address
{
public:
    address()
    {
        memset((char *) &si_other, 0, sizeof(si_other));
    }

    address(uint16_t port)
    {
        memset((char *) &si_other, 0, sizeof(si_other));
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(port);
        si_other.sin_addr.s_addr = INADDR_ANY;
    }

    address(char const * address, uint16_t port)
    {
        //setup address structure
        memset((char *) &si_other, 0, sizeof(si_other));
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(port);
        si_other.sin_addr.S_un.S_addr = inet_addr(address);
    }

    char const * ip() const
    {
        return inet_ntoa(si_other.sin_addr);
    }

    uint16_t port() const
    {
        return ntohs(si_other.sin_port);
    }

    struct sockaddr_in si_other;
};

class udp_socket
{
public:
    using socket_t = SOCKET;

    struct packet
    {
        address addr;
        size_t  size;
    };

    void close()
    {
        closesocket(m_fd);
        m_fd = SOCKET_ERROR;
    }

    void create()
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
        {
            printf("Failed. Error Code : %d",WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        if ( (m_fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
        {
            printf("socket() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }

    int bind(address const & addr)
    {
        int ret = ::bind(m_fd ,(struct sockaddr *)&addr.si_other , sizeof(addr.si_other));
        if( ret == SOCKET_ERROR)
        {
            printf("Bind failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        return ret;
    }

    int send(char const * data, size_t length, address const & addr)
    {
        int ret = sendto(m_fd, data, length , 0 , (struct sockaddr *)&addr.si_other, sizeof(struct sockaddr_in ));
        if ( ret == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        return ret;
    }

    int recv(char * buf, int length, address & addr)
    {
        int slen =  sizeof(struct sockaddr_in);
        int ret  = recvfrom( m_fd, buf, length, 0, (struct sockaddr *) &addr.si_other, &slen);

        if (ret == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }

    protected:

    socket_t m_fd = SOCKET_ERROR;
};

int client(void)
{
    udp_socket S;
    S.create();
    address addr("127.0.0.1", PORT);

    while(1)
    {
        char message[] = "gavin";

        S.send(message, 5, addr);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

int server()
{
    udp_socket S;
    address   in;

    address  addr(PORT);

    char buf[BUFLEN];


    //Create a socket
    S.create();

    printf("Socket created.\n");

    S.bind(addr);


    printf("Bind done\n");

    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);

        int ret = S.recv(buf, BUFLEN, in);
        //try to receive some data, this is a blocking call
        if (ret == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", in.ip(), in.port() );
        printf("Data: %s\n" , buf);


        //now reply the client with the same data
        if ( S.send(buf, ret, in) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }


    S.close();
    return 0;
}

int server2()
{
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen , recv_len;
    char buf[BUFLEN];
    WSADATA wsa;

    slen = sizeof(si_other) ;

    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");

    //Create a socket
    if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );

    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);

        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }

    closesocket(s);
    WSACleanup();

    return 0;
}


int client2(void)
{
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    WSADATA wsa;

    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");

    //create socket
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

    //start communication
    while(1)
    {
        char message[] = "gavin";
        //send the message
        if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }

       // puts(buf);
    }

    closesocket(s);
    WSACleanup();

    return 0;
}


int main()
{
    std::thread S( server );
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::thread C( client);

    std::this_thread::sleep_for(std::chrono::seconds(300));
    return 0;
}
