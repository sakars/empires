/// @file hexcoords.h
/// @brief Header file defining coordinates in an odd-r offset hexagonal grid
/// system. Referencing https://www.redblobgames.com/grids/hexagons/
#ifndef HEXCOORDS_H
#define HEXCOORDS_H

#include <stdint.h>
#include <stdlib.h>

/// @brief Coordinates in the odd-r offset horizontal hexagonal grid system.
typedef struct {
  int q; ///< Column coordinate (q)
  int r; ///< Row coordinate (r)
} HexCoordOffset;

/// @brief Coordinates in the axial coordinate system for hexagonal grids.
typedef struct {
  int q; ///< Axial x coordinate (q)
  int r; ///< Axial y coordinate (r)
} HexCoordAxial;

typedef struct {
  int x;
  int y;
  int z;
} HexCoordCube;

// Function declarations
HexCoordAxial hex_offset_to_axial(const HexCoordOffset *offset);
HexCoordOffset hex_axial_to_offset(const HexCoordAxial *axial);
HexCoordCube hex_axial_to_cube(const HexCoordAxial *axial);
HexCoordAxial hex_cube_to_axial(const HexCoordCube *cube);
HexCoordCube hex_offset_to_cube(const HexCoordOffset *offset);
HexCoordOffset hex_cube_to_offset(const HexCoordCube *cube);

/// @brief Shifts the axial coordinates by the specified delta values.
/// @verbatim
/// -y   +z
///
/// -x   +x
///
/// -z   +y
/// @endverbatim
void hex_axial_shift(HexCoordAxial *axial, int dx, int dy, int dz);

HexCoordAxial hex_subtract(const HexCoordAxial *a, const HexCoordAxial *b);
HexCoordAxial hex_add(const HexCoordAxial *a, const HexCoordAxial *b);
HexCoordAxial hex_multiply(const HexCoordAxial *a, int scalar);
int hex_cube_magnitude(const HexCoordCube *cube);
int hex_axial_magnitude(const HexCoordAxial *axial);

#endif // HEXCOORDS_H
