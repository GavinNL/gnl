#include <gnl/gnl_socket.h>

#include<thread>
#include<future>

using namespace std;

unsigned char buff[512];

int Server()
{

    gnl::Socket s;


    s.create(gnl::Socket::Protocol::TCP);
    s.bind(8810);
    s.listen();

    std::cout << "[Server] Listening on 8810" << std::endl;

   // while (true)
    {


        std::cout << "[Server] Waiting for client" << std::endl;

        //gnl::Socket client;
        gnl::Socket client = s.accept( );
        printf("[Server]: Client connected\n");

        while ( true )
        {
            //if( client.IsError() )
            //{
            //    std::cout << "[Server] client socket detected an error" << std::endl;
            //}
            auto ret = client.recv( buff, 100, false);

            if( ret != 0  )
            {
                buff[ ret] = 0;
                std::cout << "[Server] Received: "<< buff << std::endl;
            }
            else   // Receive will return 0 if the client disonnects, otherwise it will block
            {
               std::cout << "[Server] Client disconnecting gracefully" << std::endl;
               client.close();
               break;
            }
            std::this_thread::sleep_for( std::chrono::milliseconds(10) );
        }
        printf("[Server] Client disconnected\n");
    }

    s.close();

    std::cout << "[Server] exited" << std::endl;

    return 0;

}


int Client()
{

    std::this_thread::sleep_for( std::chrono::seconds(1) );
    gnl::Socket s;

    s.create(gnl::Socket::Protocol::TCP);

    std::cout << "[Client] Connecting " << std::endl;

    if( !s.connect("localhost", 8810) )
    {
        std::cout << "[Client] couldn't connect" << std::endl;
        return 0;
    }


    std::cout << "[Client] Established connection" << std::endl;
    std::this_thread::sleep_for( std::chrono::seconds(1) );
    std::cout << "[Client] Sending Data:" << std::endl;

    //while(1)
    //{
        s.send( "Hello this is a test", 5+4+2+1+4+4);
        std::this_thread::sleep_for( std::chrono::seconds(1) );
        s.send( "Hello this is a test", 5+4+2+1+4+4);
        std::this_thread::sleep_for( std::chrono::seconds(1) );
        s.send( "Hello this is a test", 5+4+2+1+4+4);
        std::this_thread::sleep_for( std::chrono::seconds(1) );
    //}

    std::cout << "[Client] disconnected" << std::endl;
    std::cout << "[Client] exited" << std::endl;
    s.close();
    return 0;

}

int main(int argc, char **argv)
{

    auto s = std::async( std::launch::async, Server);
    //std::this_thread::sleep_for( std::chrono::seconds(1) );

    auto c = std::async( std::launch::async, Client);


    std::this_thread::sleep_for( std::chrono::seconds(20) );
    return 0;

}

