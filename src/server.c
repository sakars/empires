#include "server.h"
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct evconnlistener *listener = NULL;

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                           struct sockaddr *address, int socklen, void *ctx);
static void read_cb(struct bufferevent *bev, void *ctx);
static void event_cb(struct bufferevent *bev, short events, void *ctx);

int init_server(struct event_base *base, int port) {
  struct sockaddr_in sin = {0};

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(port);

  listener = evconnlistener_new_bind(base, accept_conn_cb, NULL,
                                     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                     -1, (struct sockaddr *)&sin, sizeof(sin));

  return listener ? 0 : -1;
}

void cleanup_server(void) {
  if (listener) {
    evconnlistener_free(listener);
    listener = NULL;
  }
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                           struct sockaddr *address, int socklen, void *ctx) {
  struct event_base *base = evconnlistener_get_base(listener);
  struct bufferevent *bev =
      bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

  if (!bev)
    return;

  bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
  bufferevent_write(bev, "Hello\n", 6);
}

static void read_cb(struct bufferevent *bev, void *ctx) {
  struct evbuffer *input = bufferevent_get_input(bev);
  char *line;
  size_t len;

  while ((line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF))) {
    bufferevent_write(bev, "Echo: ", 6);
    bufferevent_write(bev, line, len);
    bufferevent_write(bev, "\n", 1);
    free(line);
  }
}

static void event_cb(struct bufferevent *bev, short events, void *ctx) {
  if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
    bufferevent_free(bev);
  }
}
