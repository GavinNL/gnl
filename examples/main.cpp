
#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE 256

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <gnl/gnl_unixsocket.h>
#define GNL

int
main(int argc, char *argv[])
{
    int ret;
    int result;
    char buffer[BUFFER_SIZE];
    int down_flag = 0;
#if !defined(GNL)
  struct sockaddr_un name;
   int connection_socket;
   int data_socket;
#else
  Unix_Socket Server;
#endif

  /*
            * In case the program exited inadvertently on the last run,
            * remove the socket.
            */

  unlink(SOCKET_NAME);

  /* Create local socket. */
#if defined(GNL)
#else
  connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (connection_socket == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
  }
#endif

  /*
            * For portability clear the whole structure, since some
            * implementations have additional (nonstandard) fields in
            * the structure.
            */
#if defined(GNL)
    Server.Create();

    Server.Bind(SOCKET_NAME);
#else

  memset(&name, 0, sizeof(struct sockaddr_un));

  /* Bind socket to socket name. */

  name.sun_family = AF_UNIX;
  strcpy(name.sun_path, SOCKET_NAME);
  //strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

  ret = bind(connection_socket, (const struct sockaddr *) &name,
             sizeof(struct sockaddr_un));
  if (ret == -1) {
      perror("bind");
      exit(EXIT_FAILURE);
  }

  /*
            * Prepare for accepting connections. The backlog size is set
            * to 20. So while one request is being processed other requests
            * can be waiting.
            */
#endif

#if defined(GNL)
  assert( Server.Listen() );
#else
  ret = listen(connection_socket, 20);
  if (ret == -1) {
      perror("listen");
      exit(EXIT_FAILURE);
  }
#endif

  /* This is the main loop for handling connections. */

  for (;;) {

    /* Wait for incoming connection. */
    #if defined(GNL)
      printf("Accepting new clients\n");
    auto Client = Server.Accept();
    printf("New Client accepted\n");
    #else
    data_socket = accept(connection_socket, NULL, NULL);
    if (data_socket == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    result = 0;
    #endif

    for(;;) {

      /* Wait for next data packet. */
      #if defined(GNL)
      ret = Client.Read(buffer, BUFFER_SIZE);
#else
      ret = read(data_socket, buffer, BUFFER_SIZE);
      if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
#endif
buffer[ret]=0;
      printf("Recv: -->%s<--\n", buffer);
      //printf("bytes read: %d\n", ret);
      Client.Write(buffer, ret);
      if( ret==-1 )
      {
          Client.Close();
          break;
      }
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
    #if defined(GNL)
    ret = Client.Write(buffer, BUFFER_SIZE);
#else
    ret = write(data_socket, buffer, BUFFER_SIZE);
#endif

    if (ret == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    /* Close socket. */

#if defined(GNL)
    Client.Close();
#else
    close(data_socket);
#endif

    /* Quit on DOWN command. */

    if (down_flag) {
      break;
    }
  }

#if defined(GNL)
  Server.Close();
#else
  close(connection_socket);
#endif

  /* Unlink the socket. */

  unlink(SOCKET_NAME);

  exit(EXIT_SUCCESS);
}
