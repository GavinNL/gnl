#include <iostream>
#include <thread>
#include <mutex>
#include <gnl/gnl_socket.h>


std::mutex m_Mutex;
/**
 * @brief Client_Thread
 * @param ListenSocket
 *
 * Thread called for each client connected.
 */
void Listen(gnl::Socket & C)
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
    int bytes = C.recv(&message_size, 1, true);
    if( bytes != 1) // client closed connetion
    {
      C.close();
      break;
    }

    // read the message
    bytes = C.recv( message , message_size, true);
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

  gnl::Socket C;

  C.create( gnl::Socket::Protocol::TCP);
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
