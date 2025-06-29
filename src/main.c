#include "hex/hexgrid.h"
#include "hex/interact.h"
#include "hexdrawer.h"
#include <GL/freeglut.h>
#include <event2/event.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_STATE_FLAG 0xAABB

typedef struct {
  uint16_t datatype_flag;
  int window_closed;
  int window_size[2];
  struct event_base *event_base;
  HexGrid hexgrid;
  int last_mouse[2];
  int panning_mode;
} WindowState;

WindowState *get_window_state(void *state) {
  if (state == NULL) {
    fprintf(stderr, "State pointer is NULL\n");
    return NULL;
  }
  WindowState *window_state = (WindowState *)state;
  if (window_state->datatype_flag != WINDOW_STATE_FLAG) {
    fprintf(stderr, "Invalid state pointer\n");
    return NULL;
  }
  return (WindowState *)state;
}

#include <time.h>

void display(void *state) {
  WindowState *window_state = get_window_state(state);
  if (window_state == NULL) {
    fprintf(stderr, "Invalid window state\n");
    return;
  }
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  draw_hexgrid(&window_state->hexgrid, window_state->window_size[0],
               window_state->window_size[1]);

  glutSwapBuffers();

  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  // printf("Display function took %.6f seconds\n", elapsed);
}

void reshape(int width, int height, void *state) {
  WindowState *window_state = get_window_state(state);
  if (window_state == NULL) {
    fprintf(stderr, "Invalid window state\n");
    return;
  }
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, height, 0, -1,
          1); // Set 2D coordinate system (0,0 bottom-left)

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  window_state->window_size[0] = width;
  window_state->window_size[1] = height;
}

void on_close(void *state) {
  WindowState *window_state = get_window_state(state);
  if (window_state == NULL) {
    fprintf(stderr, "Invalid window state on close\n");
    return;
  }
  window_state->window_closed = 1;
  glutLeaveMainLoop();
}

void mouse(int button, int state, int x, int y, void *arg) {
  WindowState *window_state = get_window_state(arg);
  if (window_state == NULL) {
    fprintf(stderr, "Invalid window state in mouse click\n");
    return;
  }
  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
    window_state->last_mouse[0] = x;
    window_state->last_mouse[1] = y;
    window_state->panning_mode = 1; // Enable panning mode
  }
  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
    window_state->panning_mode = 0; // Disable panning mode
  }
  if (button == 3 && state == GLUT_DOWN) {
    // Scroll up
    window_state->hexgrid.radius *= 1.1f; // Zoom in
  }
  if (button == 4 && state == GLUT_DOWN) {
    // Scroll down
    window_state->hexgrid.radius /= 1.1f; // Zoom out
  }
  glutPostRedisplay(); // Redraw the window
}

void mouse_motion(int x, int y, void *arg) {
  WindowState *window_state = get_window_state(arg);
  if (window_state == NULL) {
    fprintf(stderr, "Invalid window state in mouse motion\n");
    return;
  }
  if (window_state->panning_mode) {
    int dx = x - window_state->last_mouse[0];
    int dy = y - window_state->last_mouse[1];
    hexgrid_move(&window_state->hexgrid, -dx, -dy);
    window_state->last_mouse[0] = x;
    window_state->last_mouse[1] = y;
    glutPostRedisplay();
  }
}

void check_window_closed(evutil_socket_t fd, short what, void *arg) {
  WindowState *windowState = get_window_state(arg);
  if (windowState == NULL) {
    fprintf(stderr, "Invalid arguments in check_window_closed\n");
    return;
  }
  if (windowState->window_closed) {
    event_base_loopbreak(windowState->event_base);
  }
}

void iterate_event_loop(evutil_socket_t fd, short what, void *arg) {
  glutMainLoopEvent();
}

void hexIterate_loop(evutil_socket_t fd, short what, void *state) {
  // printf("Hex iteration loop started\n");
  WindowState *window_state = get_window_state(state);
  if (window_state == NULL) {
    fprintf(stderr, "Invalid window state in hexIterate_loop\n");
    return;
  }
  for (size_t r = 0; r < window_state->hexgrid.size[1]; r++) {
    for (size_t q = 0; q < window_state->hexgrid.size[0]; q++) {
      Hex *hex = &window_state->hexgrid.coords[r][q];
      if (hex->faction_id >= 0) {
        update_hex(hex);
      }
    }
  }
  // Shuffle (r, q) pairs together instead of individually
  size_t rows = window_state->hexgrid.size[1];
  size_t cols = window_state->hexgrid.size[0];
  size_t num_pairs = rows * cols;
  struct {
    size_t r, q;
  } pairs[num_pairs];
  size_t idx = 0;
  for (size_t r = 0; r < rows; r++) {
    for (size_t q = 0; q < cols; q++) {
      pairs[idx].r = r;
      pairs[idx].q = q;
      idx++;
    }
  }
  // Fisher-Yates shuffle for pairs
  for (size_t i = num_pairs - 1; i > 0; i--) {
    size_t j = rand() % (i + 1);
    // Swap pairs[i] and pairs[j]
    size_t tmp_r = pairs[i].r;
    size_t tmp_q = pairs[i].q;
    pairs[i] = pairs[j];
    pairs[j].r = tmp_r;
    pairs[j].q = tmp_q;
  }
  for (size_t i = 0; i < num_pairs; i++) {
    size_t r = pairs[i].r;
    size_t q = pairs[i].q;
    Hex *neighbours[6];
    Hex *hex = &window_state->hexgrid.coords[r][q];
    HexCoordOffset offset = {.q = q, .r = r}; // Convert to offset coordinates
    HexCoordAxial axial =
        hex_offset_to_axial(&offset); // Convert to axial coordinates
    HexCoordAxial neighbour_coords = {axial.q - 1, axial.r};
    neighbours[0] =
        hexgrid_get_hex_axial(&window_state->hexgrid, &neighbour_coords);
    neighbour_coords = (HexCoordAxial){axial.q, axial.r - 1};
    neighbours[1] =
        hexgrid_get_hex_axial(&window_state->hexgrid, &neighbour_coords);
    neighbour_coords = (HexCoordAxial){axial.q + 1, axial.r - 1};
    neighbours[2] =
        hexgrid_get_hex_axial(&window_state->hexgrid, &neighbour_coords);
    neighbour_coords = (HexCoordAxial){axial.q + 1, axial.r};
    neighbours[3] =
        hexgrid_get_hex_axial(&window_state->hexgrid, &neighbour_coords);
    neighbour_coords = (HexCoordAxial){axial.q, axial.r + 1};
    neighbours[4] =
        hexgrid_get_hex_axial(&window_state->hexgrid, &neighbour_coords);
    neighbour_coords = (HexCoordAxial){axial.q - 1, axial.r + 1};
    neighbours[5] =
        hexgrid_get_hex_axial(&window_state->hexgrid, &neighbour_coords);
    if (hex->faction_id >= 0) {
      interact_with_neighbours(hex, neighbours);
    }
  }

  glutPostRedisplay(); // Redraw the window after updating hexes
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL)); // Seed the random number generator
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Empires");

  glClearColor(1.0, 1.0, 1.0, 1.0); // Black background

  struct event_base *base = event_base_new();
  if (!base) {
    fprintf(stderr, "Could not initialize event base\n");
    return EXIT_FAILURE;
  }
  // Initialize the event base with 3 priority levels
  event_base_priority_init(base, 3);

  WindowState window_state = {.datatype_flag = WINDOW_STATE_FLAG,
                              .event_base = base,
                              .window_closed = 0,
                              .window_size = {800, 600},
                              .last_mouse = {0, 0},
                              .panning_mode = 0};
  hexgrid_init(&window_state.hexgrid, 1000, 500);
  window_state.hexgrid.offset[0] = 0.0f;
  window_state.hexgrid.offset[1] = 0.0f;

  printf("HexGrid initialized with size: %zu x %zu\n",
         window_state.hexgrid.size[0], window_state.hexgrid.size[1]);
  // Register required GLUT callbacks
  glutMotionFuncUcall(mouse_motion, &window_state);
  glutMouseFuncUcall(mouse, &window_state);
  glutDisplayFuncUcall(display, &window_state);
  glutReshapeFuncUcall(reshape, &window_state);
  glutWMCloseFuncUcall(on_close, &window_state);

  struct event *ev =
      event_new(base, -1, EV_PERSIST, check_window_closed, &window_state);
  struct timeval tv = {2, 0}; // Check every 2 seconds
  // Set the priority of the close check event to high
  event_priority_set(ev, 2);
  event_add(ev, &tv);

  float target_fps = 5.0f;
  float frame_time = 1.0f / target_fps;

  struct event *iterate_ev =
      event_new(base, -1, EV_PERSIST, iterate_event_loop, &window_state);
  struct timeval iterate_tv = {
      0, (long)(frame_time * 1000000)}; // Convert to microseconds
  event_priority_set(iterate_ev, 2); // Mark the visual event as high priority
  event_add(iterate_ev, &iterate_tv);

  struct event *hex_iterate_ev =
      event_new(base, -1, EV_PERSIST, hexIterate_loop, &window_state);
  struct timeval hex_iterate_tv = {0, (long)(frame_time * 1000000)};
  event_priority_set(hex_iterate_ev,
                     2); // Mark the hex iteration event as medium priority
  event_add(hex_iterate_ev, &hex_iterate_tv);

  event_base_dispatch(base);

  event_free(ev);
  event_free(iterate_ev);
  event_base_free(base);

  return EXIT_SUCCESS;
}