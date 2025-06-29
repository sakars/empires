#ifndef HEXGRID_H
#define HEXGRID_H

#include <stddef.h>
#include <stdint.h>

#include "hexcoords.h"

typedef int32_t faction_id_t;

typedef struct Hex {
  faction_id_t faction_id;
  int32_t production; // How much it produces per turn
  int32_t population; // How many people live in the hex
  int32_t resources;  // How many resources it has currently
  int32_t military;   // How many military units are stationed here
} Hex;

typedef struct HexGrid {
  Hex **coords;
  size_t size[2];
  /// Offset for the grid, marks where in the grid the middle of
  /// the hexes is in radius along x and 1.5 * radius along y.
  float offset[2];
  float radius; // Radius of the hexagons in px
} HexGrid;

void hexgrid_init(HexGrid *grid, size_t width, size_t height);

/// @brief Fetches a hex from the grid based on its coordinates.
/// @param grid Pointer to the HexGrid.
/// @param coord Pointer to the HexCoordXY structure containing the coordinates
/// of the hex.
/// @return Pointer to the Hex structure at the specified coordinates, or NULL
/// if the coordinates are out of bounds.
Hex *hexgrid_get_hex_offset(HexGrid *grid, const HexCoordOffset *coord);

Hex *hexgrid_get_hex_axial(HexGrid *grid, const HexCoordAxial *coord);

/// @brief Moves the hex grid by the specified delta values.
/// @param grid Pointer to the HexGrid.
/// @param dx Delta x value to move the grid.
/// @param dy Delta y value to move the grid.
/// @note dx and dy are in pixels, not in hexes.
void hexgrid_move(HexGrid *grid, float dx, float dy);

enum AttributeId {
  ATTRIBUTE_RESOURCE_EFFICIENCY, // How efficiently resources are used
  ATTRIBUTE_MILITARY_STRENGTH,   // How strong the military is
  ATTRIBUTE_POPULATION_GROWTH,   // How fast the population grows
  ATTRIBUTE_MILITARY_EFFICIENCY, // How many resources are used to maintain
                                 // the military
  ATTRIBUTE_GAMBLERS,            // How willing to run risky attacks
};

float generate_faction_attribute(faction_id_t faction_id,
                                 enum AttributeId attribute_id);

#endif // HEXGRID_H