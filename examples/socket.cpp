#include <gnl/gnl_socket.h>

#include<thread>


using namespace std;

unsigned char buff[512];
int Server()
{

    gnl::Socket s;

    s.Create();
    s.Bind(8810);
    s.Listen();

    std::cout << "[Server] Listening on 8810" << std::endl;

    while (true)
    {
        gnl::Socket client;

        std::cout << "[Server] Waiting for client" << std::endl;

       // while( !s.ClientAvailable() )
       // {
       //     std::cout << "Checking if client is available" << std::endl;
       //     std::this_thread::sleep_for( std::chrono::seconds(1) );
       // }

        s.Accept(&client);
        printf("[Server]: Client connected\n");

        while ( true )
        {
            std::cout << "client has bytes: " << client.HasBytes() << std::endl;
           // auto ret = ;

            if( auto ret = client.Receive(buff, 100)  )
            {
                std::cout << " Received " << ret <<" bytes "<< std::endl;
            }
            else   // Receive will return 0 if the client disonnects, otherwise it will block
            {
                std::cout << "[Server] Client error, disconnecting" << std::endl;
                std::cout << "   IsError:" <<client.IsError() << std::endl;
                client.Close();
                break;
            }
        }
        printf("[Server] Client disconnected\n");
    }

    s.Close();

    std::cout << "[Server] exited" << std::endl;

    return 0;

}


int Client()
{

    std::this_thread::sleep_for( std::chrono::seconds(3) );
    gnl::Socket s;

    s.Create();

    std::cout << "[Client] Connecting " << std::endl;

    if( !s.Connect("localhost", 8810) )
    {
        std::cout << "[Client] couldn't connect" << std::endl;
        return 0;
    }


    std::cout << "[Client] Established connection" << std::endl;

    std::cout << "[Client] Sending Data:" << std::endl;

    while(1)
    {
        s.SendRaw( "Hello this is a test", 18);
        std::this_thread::sleep_for( std::chrono::seconds(3) );
    }

    s.Close();
    std::cout << "[Client] disconnected" << std::endl;

    std::cout << "[Client] exited" << std::endl;
    return 0;

}

int main(int argc, char **argv)
{

    if(argc==1)
    {
        Server();
    } else {
        Client();
    }

    return 0;

}

