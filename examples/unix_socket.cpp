#if defined __linux__
#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE 12

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <gnl/gnl_unixsocket.h>

int
main(int argc, char *argv[])
{
  struct sockaddr_un name;
  int down_flag = 0;
  int ret;
  // int connection_socket;
  // int data_socket;
  int result;
  char buffer[BUFFER_SIZE];

  Unix_Socket Server;

  /*
            * In case the program exited inadvertently on the last run,
            * remove the socket.
            */

  unlink(SOCKET_NAME);


  assert( Server.Create() );

  /*
            * For portability clear the whole structure, since some
            * implementations have additional (nonstandard) fields in
            * the structure.
            */
  assert( Server.Bind(SOCKET_NAME) );

  /*
            * Prepare for accepting connections. The backlog size is set
            * to 20. So while one request is being processed other requests
            * can be waiting.
            */
  assert( Server.Listen() );

  /* This is the main loop for handling connections. */

  for (;;) {


    auto Client = Server.Accept();


    result = 0;
    for(;;) {

      /* Wait for next data packet. */

      //ret = read(data_socket, buffer, BUFFER_SIZE);
      ret = Client.Read(buffer, BUFFER_SIZE);
      if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }

      printf("bytes read: %d\n", ret);

      /* Ensure buffer is 0-terminated. */

      buffer[BUFFER_SIZE - 1] = 0;

      /* Handle commands. */

      if (!strncmp(buffer, "DOWN", BUFFER_SIZE)) {
        down_flag = 1;
        break;
      }

      if (!strncmp(buffer, "END", BUFFER_SIZE)) {
        break;
      }

      /* Add received summand. */

      result += atoi(buffer);
    }

    /* Send result. */

    sprintf(buffer, "%d", result);
    ret = Client.Write(buffer, BUFFER_SIZE);
    //ret = write(data_socket, buffer, BUFFER_SIZE);

    if (ret == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    /* Close socket. */

    Client.Close();
    //close(data_socket);

    /* Quit on DOWN command. */

    if (down_flag) {
      break;
    }
  }

  Server.Close();
  //close(connection_socket);

  /* Unlink the socket. */

  unlink(SOCKET_NAME);

  exit(EXIT_SUCCESS);
}
#else

int main()
{
    return 0;
}

#endif
