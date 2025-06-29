#include "hexdrawer.h"
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Color {
  float r, g, b;
};

static void draw_hexagon(float cx, float cy, float r, struct Color color,
                         int outline) {
  // Draw filled hexagon (keep existing color)
  // glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
  glColor3f(color.r, color.g, color.b); // Use the provided color
  glBegin(GL_POLYGON);
  float dx = r * cosf(2.0f * M_PI / 12.0f);
  float dy = r * sinf(2.0f * M_PI / 12.0f);
  glVertex2f(cx, cy + r);
  glVertex2f(cx + dx, cy + dy);
  glVertex2f(cx + dx, cy - dy);
  glVertex2f(cx, cy - r);
  glVertex2f(cx - dx, cy - dy);
  glVertex2f(cx - dx, cy + dy);
  glEnd();

  if (!outline) {
    return; // If outline is not requested, return after drawing the filled
            // hexagon
  }
  // Draw hexagon outline in black
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(cx, cy + r);
  glVertex2f(cx + dx, cy + dy);
  glVertex2f(cx + dx, cy - dy);
  glVertex2f(cx, cy - r);
  glVertex2f(cx - dx, cy - dy);
  glVertex2f(cx - dx, cy + dy);
  glEnd();
}

struct Color generate_color(faction_id_t faction_id) {
  // Generate a color based on the faction ID
  struct Color color;
  if (faction_id < 0) {
    // If faction_id is -1, return a default color (e.g., gray)
    color.r = 0.1f;
    color.g = 0.1f;
    color.b = 0.1f;
    return color;
  }
  // Use a more complex hash (32-bit FNV-1a) to scramble the faction_id bits for
  // color generation
  uint32_t x = 2166136261u;
  uint32_t id = (uint32_t)faction_id;
  for (int i = 0; i < 4; ++i) {
    x ^= (id & 0xFF);
    x *= 16777619u;
    id >>= 8;
  }
  // Final avalanche
  x ^= (x >> 13);
  x ^= (x << 7);
  x ^= (x >> 17);
  // Now extract color components from the scrambled value
  color.r = ((x & 0xFF)) / 255.0f;
  color.g = ((x >> 8) & 0xFF) / 255.0f;
  color.b = ((x >> 16) & 0xFF) / 255.0f;
  // Make the color fully saturated and mostly light
  float max_component = fmaxf(fmaxf(color.r, color.g), color.b);
  if (max_component > 0.0f) {
    color.r /= max_component;
    color.g /= max_component;
    color.b /= max_component;
  }
  // Set lightness to the middle (0.5 means "mid lightness")
  float mid_lightness = 0.5f;
  color.r = color.r * (1.0f - mid_lightness) + mid_lightness;
  color.g = color.g * (1.0f - mid_lightness) + mid_lightness;
  color.b = color.b * (1.0f - mid_lightness) + mid_lightness;
  return color;
}

void draw_hexgrid(const HexGrid *grid, int width, int height) {
  if (grid == NULL) {
    fprintf(stderr, "HexGrid is NULL\n");
    return;
  }
  float width_in_radii = (float)width / grid->radius;
  float height_in_radii = (float)height / grid->radius;
  float hexagon_width = 2 * sqrtf(3.0f) / 2.0f;
  float hexagon_height = 1.5f;
  int num_cols = (int)(width_in_radii / hexagon_width);
  int num_rows = (int)(height_in_radii / hexagon_height);
  int center_hex_col = roundf(grid->offset[0]);
  int center_hex_row = roundf(grid->offset[1]);
  int start_col = center_hex_col - num_cols / 2 - 2;
  int start_row = center_hex_row - num_rows / 2 - 2;
  start_col = start_col < 0 ? 0 : start_col;
  start_row = start_row < 0 ? 0 : start_row;
  int end_col = start_col + num_cols + 4;
  int end_row = start_row + num_rows + 4;
  end_col = end_col >= grid->size[0] ? grid->size[0] - 1 : end_col;
  end_row = end_row >= grid->size[1] ? grid->size[1] - 1 : end_row;
  int low_res =
      1; // grid->radius < 6.0f; // Use low resolution for small hexagons
  for (size_t r = start_row; r <= end_row; r++) {
    for (size_t q = start_col; q <= end_col; q++) {
      int parity = r & 1; // Check if row is even or odd
      const Hex *hex = &grid->coords[r][q];
      float radius = grid->radius;
      float cx = (q + parity * 0.5f - grid->offset[0]) * 2 * radius *
                 sqrtf(3.0f) / 2.0f;
      float cy = (r - grid->offset[1]) * (radius * 1.5f);
      float display_radius =
          low_res ? radius : radius * 0.9f; // Use smaller radius for
      struct Color color =
          generate_color(hex->faction_id); // Generate color based on faction ID
      float multiplier = 1; // hex->military / 1000.0f / 2 + 0.5f;
      color.r *= multiplier;
      color.g *= multiplier;
      color.b *= multiplier;
      draw_hexagon(cx + width / 2, cy + height / 2, display_radius, color,
                   !low_res); // Draw hexagon with 90% radius
    }
  }
}
