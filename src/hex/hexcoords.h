/// @file hexcoords.h
/// @brief Header file defining coordinates in an odd-r offset hexagonal grid
/// system. Referencing https://www.redblobgames.com/grids/hexagons/
#ifndef HEXCOORDS_H
#define HEXCOORDS_H

/// @brief Coordinates in the odd-r offset horizontal hexagonal grid system.
typedef struct {
  int q; ///< Column coordinate (q)
  int r; ///< Row coordinate (r)
} HexCoordOffset;

/// @brief Coordinates in the axial coordinate system for hexagonal grids.
typedef struct {
  int x; ///< Axial x coordinate (q)
  int y; ///< Axial y coordinate (r)
} HexCoordAxial;

HexCoordAxial hex_offset_to_axial(const HexCoordOffset *offset) {
  HexCoordAxial axial;
  axial.x = offset->q;
  axial.y = offset->r - (offset->q + (offset->q & 1)) / 2;
  return axial;
}

#endif // HEXCOORDS_H
