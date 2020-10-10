#include <iostream>
#include <thread>

#include <gnl/socket_old.h>

#undef GNL_NAMESPACE
#define GNL_NAMESPACE gnl2
#include <gnl/socket.h>
#undef GNL_NAMESPACE

#define PORT 30001
#define BUFLEN 256
#define SERVER "127.0.0.1"

#define ENDPOINT "127.0.0.1:3001"


int client(void)
{
    gnl2::socket     S;                           // The socket



    std::cout << "[Client] connecting" << std::endl;

    if( !S.connect( ENDPOINT ) )
    {
        std::cout << "[Client] Error connecting" << std::endl;
        return 0;
    }
    //std::cout << "[Client] CONNECTED to " << S.get_address().ip()  << std::endl;



    char message[10] = "XHello";
    auto size  = 6;
    message[0] = 5;

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

int client2(void)
{
    gnl::tcp_socket     S;                           // The socket

    // Create the socket
    S.create();


    std::cout << "[Client] connecting" << std::endl;

    if( !S.connect(SERVER, PORT) )
    {
        std::cout << "[Client] Error connecting" << std::endl;
        return 0;
    }
    std::cout << "[Client] CONNECTED to " << S.get_address().ip()  << std::endl;



    char message[10] = "XHello";
    auto size  = 6;
    message[0] = 5;

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
    gnl::tcp_socket      S;           // The socket class

    char buf[BUFLEN];            // Buffer to store data

    S.create();

    S.bind(PORT);
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
        buf[0] = 0;
        buf[1] = 0;

        // try to recieve some data. This is a blocking call
        // Recieve the first byte, which indicates the total number
        // of bytes in the next message
        char msg_length=0;
        auto message_size = client.recv( &msg_length, 1);

        if( message_size == 0)
        {
            std::cout << "[Server] Client closed the connection! \n";
            break;
        }

        if( message_size != gnl::tcp_socket::error)
        {
            // recieve the remaining bytes
            auto r = client.recv( buf, msg_length);

            if( r == 0)
            {
                std::cout << "[Server] Client closed the connection! \n";
                break;
            }
            buf[r] = 0; // make sure we terminate the string
        } else {
            client.close();
            break;
        }

        //print details of the client / peer and the data received

        std::cout << "[Server] Received Message from " << client.get_address().ip();
        std::cout << "  Data: " << buf << std::endl;;


    }

    client.close();
    std::cout << "[Server] Closing Socket" << std::endl;
    S.close();
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
