#pragma once

#ifndef GNL_UNIX_SOCKET
#define GNL_UNIX_SOCKET


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include <sys/ioctl.h>

class Unix_Socket
{
public:

  bool Create()
  {
    m_fd = __create(AF_UNIX, SOCK_STREAM, 0);//socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (m_fd == -1)
    {
      perror("socket");
      exit(EXIT_FAILURE);
      return false;
    }
    return true;
  }

  int __create(int __domain, int __type, int __protocol)
  {
    return ::socket(__domain, __type, __protocol);
  }

  bool Bind(const char * name)
  {
    //============
    memset(&m_name, 0, sizeof(struct sockaddr_un) );

    /* Bind socket to socket name. */

    m_name.sun_family = AF_UNIX;
    strcpy(m_name.sun_path, name);

    int ret = ::bind(m_fd, (const struct sockaddr *) &m_name,
                   sizeof(struct sockaddr_un));

    if (ret == -1)
    {
      printf("Error no: %d\n", errno );
      perror("bind");
      exit(EXIT_FAILURE);
      return false;
    }
    return true;
  }

  bool Listen()
  {
    int ret = listen(m_fd, 20);
    if (ret == -1) {
      perror("listen");
      exit(EXIT_FAILURE);
      return false;
    }
    return true;
  }

  Unix_Socket Accept()
  {
    Unix_Socket c;
    c.m_fd = ::accept(m_fd, NULL, NULL);
    if (c.m_fd == -1)
    {
      c.m_fd = -1;
      perror("accept");
      exit(EXIT_FAILURE);
      return c;
    }
    return c;
  }

  size_t Read(void * buffer, size_t size )
  {
    //ssize_t ret = read(m_fd, buffer, size);
    ssize_t ret = recv(m_fd, buffer, size,  0);
    if(ret == -1 || ret == 0)
    {
      m_fd = -1;
    }
    return ret;
  }

  size_t Write( void const * buffer , size_t size)
  {
    return write(m_fd, buffer, size);
  }

  bool Close()
  {
    close(m_fd);
    return true;
  }

  operator bool()
  {
      return m_fd != -1;
  }

private:
  struct sockaddr_un m_name;
  int                m_fd;
};

#endif
