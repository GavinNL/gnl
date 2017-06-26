#include <iostream>
#include <thread>

#include <gnl/gnl_socket.h>

#define PORT 30001
#define BUFLEN 256
#define SERVER "127.0.0.1"




int client(void)
{
    gnl::udp_socket     S;                           // The socket
    gnl::socket_address addr("127.0.0.1", PORT);     // The address want to send data to

    // Create the socket
    S.create();

    static char d = '0';

    char message[] = "Hello_X";
    auto size = strlen(message);
    message[6] = d++;

    int i=10;
    while( i-- )
    {
        if( S.send(message, size, addr) == gnl::udp_socket::error)
        {
            std::cout << "[Client] Could not send data! Closing Socket" << std::endl;
            break;
        }   //

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "[Client] Closing socket" << std::endl;
    S.close();
    return 0;
}




int server()
{
    gnl::udp_socket      S;           // The socket class
    gnl::socket_address  addr(PORT);  // The port we want to listen for packets on

    gnl::socket_address  in;          // address of the client that sent the message

    char buf[BUFLEN];            // Buffer to store data

    std::size_t packet_count = 0;// total number of packets
    S.create();

    std::cout << "Socket created" << std::endl;

    S.bind(addr);

    std::cout << "Bind done" << std::endl;;

    //keep listening for data
    std::cout << "Waiting for data..." << std::endl;

    while( packet_count < 50 )
    {

        //clear the buffer by filling null,
        //it might have previously received data
        memset(buf,'\0', BUFLEN);

        // try to recieve some data. This is a blocking call
        auto ret = S.recv(buf, BUFLEN, in);

        if( ret == gnl::udp_socket::error)
        {
            std::cout << "[Server] Error recieving data! \n";
            continue;
        }

        //print details of the client/peer and the data received
        std::cout << "(" << packet_count++ << ") Received packet from " << in.ip() << ":" << in.port();
        std::cout << "  Data: " << buf << std::endl;;

        //now reply the client with the same data
        if ( S.send( buf, ret, in) == gnl::udp_socket::error )
        {
            std::cout << "[Server] Error sending data" << std::endl;
            //printf("sendto() failed with error code : %d" , WSAGetLastError());
            continue;
        }
    }

    std::cout << "[Server] Closing Socket" << std::endl;
    S.close();
    return 0;
}


int main()
{
    for(int i=0;i<10;i++)
    {
        std::thread C( client);
        C.detach();
    }

    server();

    return 0;
}
