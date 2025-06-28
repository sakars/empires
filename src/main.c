#include "game_logic.h"
#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  printf("Empires Game Server\n");

  struct event_base *base = event_base_new();
  if (!base) {
    fprintf(stderr, "Could not initialize libevent!\n");
    return 1;
  }

  if (init_server(base, 8080) != 0) {
    fprintf(stderr, "Failed to initialize server\n");
    event_base_free(base);
    return 1;
  }

  printf("Server listening on port 8080\n");
  event_base_dispatch(base);

  cleanup_server();
  event_base_free(base);
  return 0;
}
