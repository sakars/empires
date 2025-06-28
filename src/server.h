#ifndef SERVER_H
#define SERVER_H

#include <event2/event.h>

// Initialize the server
int init_server(struct event_base *base, int port);

// Cleanup server resources
void cleanup_server(void);

#endif // SERVER_H
