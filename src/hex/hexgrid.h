#ifndef HEXGRID_H
#define HEXGRID_H

#include <stddef.h>
#include <stdint.h>

#include "hexcoords.h"

typedef int16_t faction_id_t;

typedef struct {
  faction_id_t faction_id;
} Hex;

typedef struct {
  Hex coords[200][200];
  size_t size;
} HexGrid;

/// @brief Fetches a hex from the grid based on its coordinates.
/// @param grid Pointer to the HexGrid.
/// @param coord Pointer to the HexCoordXY structure containing the coordinates
/// of the hex.
/// @return Pointer to the Hex structure at the specified coordinates, or NULL
/// if the coordinates are out of bounds.
Hex *hexgrid_get_hex(const HexGrid *grid, const HexCoordOffset *coord);

#endif // HEXGRID_H