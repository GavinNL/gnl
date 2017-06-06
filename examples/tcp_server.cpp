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
void Client_Thread(gnl::Socket & C)
{
  static int i=0;
  i++;

  int I = i;
  std::cout << "Client Connected (" << I << ")" << std::endl;
  // Note! Socket will close() if it's destructor is called.
  // so be careful when makign copies of a socket.
  // if you need to access the socket from multiple locations,
  // you should use pointers.
  unsigned char buffer[1000];

  unsigned char & message_size = buffer[0]; // store the message size in teh first byte of the buffer
  unsigned char * message      = &buffer[1];
  while( C )
  {
    // read the first byte indicating the
    // length of the next messsage
    int bytes = C.Receive(&message_size, 1, true);
    if( bytes != 1) // client closed connetion
    {
      C.Close();
      break;
    }

    // read the message
    bytes = C.Receive(message, message_size, true);
    if( bytes != message_size ) // client closed connetion
    {
      C.Close();
      break;
    }

    message[bytes] = 0; // make sure we have a terminating byte
    std::cout << "Message Recieved (" << I << ") :" << message << std::endl;

    // Send the message back to the client.
    C.SendRaw( buffer, message_size+1 );
  }

  C.Close();
  std::cout << "Client disconnected ( " << I << ")" << std::endl;
}

int main(int argc, char *argv[])
{
  gnl::Socket S;

  S.Create( gnl::Socket::Protocol::TCP ); // no arguemts = TCP by default

  while( !S.Bind(30000) )
  {
    std::cout << "Cannot bind. Retrying in 3 seconds" << std::endl;
    std::this_thread::sleep_for( std::chrono::seconds(3) );
  }
  S.Listen();


  // Each client needs a socket and a thread
  using Client_Data_t = std::pair<gnl::Socket, std::thread>;

  std::vector<Client_Data_t> m_Clients(10);

  std::cout << "Server Started" << std::endl;

  // Listen for new clients
  // spawn a new thread for each client connected
  while( S )
  {
    int i=0;

    auto NewClient = S.Accept();         // Accept the client connection (blocking)

    for(auto & c : m_Clients)            //  Find an appropriate spot in the vector to place the client
    {
      if( !c.first )                     // If the socket is invalid, that means nothing is connected, so we can use this spot
      {
        if( c.second.joinable() )        // if it's joinable, it means an old thread hasn't been cleaned up yet.
          c.second.join();               // ie: a previous client's thread wasn't joined back up with the main thread
                                         // make sure we join() so we can reuse the thread handle.

        c.first  = std::move(NewClient); // MOVE the client, do not copy, otherwise the destructor of NewClient
                                         // will close() the connection

        // call Client_Thread function and pass the socket handle as a reference to the function.
        c.second = std::thread( Client_Thread, std::ref( c.first ) );

        std::cout << "Client added to index " << i << std::endl;
        break;
      }
      i++;
    }

    NewClient.Close(); // no more available spots, so close any new connections.

  }


  // close all connections and wait for all threads to finish
  for(auto & c : m_Clients)
  {
    if(c.first)
      c.first.Close();

    if( c.second.joinable())
      c.second.join();
  }

  return 0;
}
