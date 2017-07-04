#include <iostream>

#if defined __linux__

#include <thread>

#include <gnl/gnl_socket.h>

#define PORT 30001
#define BUFLEN 256
#define SERVER "/tmp/mysocket123.socket"




int client(void)
{
    gnl::domain_stream_socket     S;                           // The socket

    // Create the socket
    S.create();


    std::cout << "[Client] connecting to " << SERVER << std::endl;

    if( !S.connect(SERVER) )
    {
        std::cout << "[Client] Error connecting" << std::endl;
        return 0;
    }
    std::cout << "[Client] CONNECTED" << std::endl;


    char message[] = "XHello";
    auto size = strlen(message);
    message[0] = 6;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    int i=10;
    while( i-- )
    {
        if( S.send(message, size) == gnl::udp_socket::error)
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
    gnl::domain_stream_socket      S;           // The socket class

    char buf[BUFLEN];            // Buffer to store data

    //S.create();

    S.unlink(SERVER);
    S.bind(SERVER);
    S.listen(5);


    std::cout << "[Server] Waiting for clients" << std::endl;

    auto client = S.accept();

    if( client )
    {
        std::cout << "[Server] Client Connected" << std::endl;
    } else {
        std::cout << "[Server] Client error" << std::endl;
        return 0;
    }

    while( client )
    {
        buf[1] = 0;
        // try to recieve some data. This is a blocking call
        auto message_size = client.recv( buf, 1);

        if( message_size == 0)
        {
            std::cout << "[Server] Client closed the connection! \n";
            break;
        }

        if( message_size != gnl::domain_stream_socket::error)
        {
            auto r = client.recv( &buf[1], buf[0]);

            if( r == 0)
            {
                std::cout << "[Server] Client closed the connection! \n";
                break;
            }
        } else {
            client.close();
        }

        //print details of the client/peer and the data received
        std::cout << "[Server] Received Message. ";
        std::cout << "  Data: " << &buf[1] << std::endl;;


    }

    client.close();
    std::cout << "[Server] Closing Socket" << std::endl;
    S.close();
    S.unlink(SERVER);
    return 0;
}


int main()
{
    //server();
    std::thread S( server );
    S.detach();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    client();

    return 0;
}


#else
int main()
{
    std::cout << "This only works on Linux" << std::endl;

    return 0;
}

#endif
