#include <iostream>
#include <thread>
#include <mutex>
#include <gnl/socket.h>


std::mutex m_Mutex;
/**
 * @brief Client_Thread
 * @param ListenSocket
 *
 * Thread called for each client connected.
 */
void Listen(gnl::tcp_socket & C)
{
  // Note! Socket will close() if it's destructor is called.
  // so be careful when making copies of a socket.
  // if you need to access the socket from multiple locations,
  // you should use pointers.
  static int i=0;
  i++;

  unsigned char buffer[1000];
  unsigned char & message_size = buffer[0];
  unsigned char * message      = &buffer[1];

  while( C )
  {
    // read the first byte indicating the
    // length of the next messsage
    std::size_t bytes = C.recv( (char*)&message_size, 1);
    if( bytes != 1) // client closed connetion
    {
      C.close();
      break;
    }

    // read the message
    bytes = C.recv( (char*)message , message_size);
    if( bytes != message_size ) // client closed connetion
    {
      C.close();
      break;
    }

    message[bytes] = 0; // make sure we terminate the string before printing
    std::cout << "Message Recieved (" << i << ") : " << message << std::endl;
  }

  std::cout << "Client disconnected" << std::endl;
}

int main(int argc, char *argv[])
{

  gnl::tcp_socket C;

  C.create();
  C.connect("127.0.0.1", 30000);

  std::thread ListenThread( Listen , std::ref( C ) );

  for(int i=0;i<10;i++)
  {
    char message[] = "XHello!"; // "Hello!" + 1 zero byte at the end
    message[0]     = (char)strlen(message)-1; // set the first byte to be the total bytes sent
    C.send( message, message[0]+1);

    std::this_thread::sleep_for( std::chrono::seconds(1) );
  }

  C.close();

  ListenThread.join();

  return 0;
}
