
#include "hexgrid.h"
#include <math.h>
#include <stdio.h>

static void hex_init(Hex *hex) {
  hex->production = rand() % 3000 + 6500; // Production between 8500 and 11500
  hex->population = rand() % 1000 + 500;  // Population between 500 and 1500
  hex->resources = rand() % 1000 + 9500;  // Resources between 500 and 1500
  hex->military = rand() % 100 + 50;      // Military between 50 and 150
}

void hexgrid_init(HexGrid *grid, size_t width, size_t height) {
  grid->size[0] = width;
  grid->size[1] = height;

  grid->coords = calloc(height, sizeof(Hex *));
  if (!grid->coords) {
    fprintf(stderr, "Could not allocate memory for hexgrid coordinates\n");
    return;
  }
  for (size_t r = 0; r < height; r++) {
    grid->coords[r] = calloc(width, sizeof(Hex));
    if (!grid->coords[r]) {
      fprintf(stderr, "Could not allocate memory for hex row %zu\n", r);
      for (size_t i = 0; i < r; i++) {
        free(grid->coords[i]);
      }
      free(grid->coords);
      return;
    }
  }

  grid->offset[0] = 200.0f;
  grid->offset[1] = 100.0f;
  grid->radius = 30.0f; // Default radius of the hexagons in pixels

  for (size_t r = 0; r < grid->size[1]; r++) {
    for (size_t q = 0; q < grid->size[0]; q++) {
      grid->coords[r][q].faction_id = q * grid->size[1] + r;
    }
  }

  // set the border and 5% of other hexes to have a faction_id of -1
  for (size_t r = 0; r < grid->size[1]; r++) {
    for (size_t q = 0; q < grid->size[0]; q++) {
      if (r == 0 || r == grid->size[1] - 1 || q == 0 ||
          q == grid->size[0] - 1) {
        grid->coords[r][q].faction_id = -1; // Border hexes
      } else {
        int random_value = rand() % 100;
        if (random_value < 5) {               // 5% chance
          grid->coords[r][q].faction_id = -1; // Random hexes
        }
      }
    }
  }
  for (size_t r = 0; r < grid->size[1]; r++) {
    for (size_t q = 0; q < grid->size[0]; q++) {
      hex_init(&grid->coords[r][q]);
    }
  }
}

Hex invalid_hex = {.faction_id = -1};

Hex *hexgrid_get_hex_offset(HexGrid *grid, const HexCoordOffset *coord) {
  if (coord->r < 0 || coord->r >= grid->size[1] || coord->q < 0 ||
      coord->q >= grid->size[0]) {
    return NULL; // Out of bounds
  }
  return &grid->coords[coord->r][coord->q];
}

const Hex *hexgrid_get_hex_offset_const(const HexGrid *grid,
                                        const HexCoordOffset *coord) {
  if (coord->r < 0 || coord->r >= grid->size[1] || coord->q < 0 ||
      coord->q >= grid->size[0]) {
    return &invalid_hex; // Out of bounds
  }
  return &grid->coords[coord->r][coord->q];
}

Hex *hexgrid_get_hex_axial(HexGrid *grid, const HexCoordAxial *coord) {
  HexCoordOffset offset = hex_axial_to_offset(coord);
  return hexgrid_get_hex_offset(grid, &offset);
}

const Hex *hexgrid_get_hex_axial_const(const HexGrid *grid,
                                       const HexCoordAxial *coord) {
  HexCoordOffset offset = hex_axial_to_offset(coord);
  return hexgrid_get_hex_offset_const(grid, &offset);
}

void hexgrid_move(HexGrid *grid, float dx, float dy) {
  grid->offset[0] += dx / (grid->radius * sqrtf(3.0f) / 2.0f) / 2;
  grid->offset[1] += dy / (grid->radius * 1.5f);
}

float generate_faction_attribute(faction_id_t faction_id,
                                 enum AttributeId attribute_id) {
  // Combine the two parameters into a single 64-bit value
  uint64_t x = ((uint64_t)faction_id << 32) | ((uint32_t)attribute_id);

  // Use a complex hash: mix bits, use non-linear transforms, and avoid simple
  // periods
  x ^= (x >> 33);
  x *= 0xff51afd7ed558ccdULL;
  x ^= (x >> 29);
  x *= 0xc4ceb9fe1a85ec53ULL;
  x ^= (x >> 32);

  // Map to [0,1) using double precision
  double norm = (x & 0x1FFFFFFFFFFFFFULL) / (double)(0x1FFFFFFFFFFFFFULL);

  // Apply a non-linear transformation to break up patterns
  norm = sin(norm * 13.731 + cos(norm * 7.123 + x * 1e-12)) * 0.5 + 0.5;

  // Now map to mean 1, stddev ~0.1 using a normal approximation (Box-Muller)
  double u1 = norm;
  double u2 = fmod(norm * 1.6180339887 + 0.123456789,
                   1.0); // irrational multiplier for decorrelation
  if (u1 < 1e-10)
    u1 = 1e-10; // avoid log(0)
  double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

  // Scale and shift to mean 1, stddev 0.1
  return (float)(1.0 + 0.1 * z);
}
