#include <iostream>
#include <thread>
#include <mutex>
#include <gnl/gnl_socket.h>


#define PORT 30000

int main(int argc, char *argv[])
{
  gnl::Socket S;

  S.Create( gnl::Socket::Protocol::UDP ); // no arguemts = TCP by default


  sockaddr_in addr;
  struct hostent* hostname;

  hostname = gethostbyname("127.0.0.1");

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

    int size = S.SendUDP(data, 6, &addr);

    printf("%d bytes sent\n", size);

    std::this_thread::sleep_for( std::chrono::seconds(1));
  }


  // close all connections and wait for all threads to finish


  return 0;
}
