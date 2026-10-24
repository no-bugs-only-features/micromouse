#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <poll.h>
#include <netinet/in.h>
#include <packetstructure.h> // in shared/ folder

typedef struct {
  const char* m_port;
  int m_serverSocket;
  bool m_isRunning;

  //
  struct sockaddr_in m_clientAddress;
  socklen_t m_clientLen;
  bool m_hasClient;

  struct pollfd m_pollFd;
} ServerApp;

// TODO: create these commands
bool server_setup(ServerApp* app, const char* port);
void server_run(ServerApp* app);
void server_stop(ServerApp* app);
void server_recvClientData(ServerApp* app);
void server_sendPacket(ServerApp* app, const DataPacket* event);

#endif
