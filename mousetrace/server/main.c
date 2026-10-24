#include "server.h"
int main() {

  ServerApp app = {0};

  server_setup(&app, "8080");
  return 0;
}
