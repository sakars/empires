#include "hexcoords.h"
#include <stdlib.h>

HexCoordAxial hex_offset_to_axial(const HexCoordOffset *offset) {
  HexCoordAxial axial;
  int parity = offset->r & 1;
  axial.q = offset->q - (offset->r - parity) / 2;
  axial.r = offset->r;
  return axial;
}

HexCoordOffset hex_axial_to_offset(const HexCoordAxial *axial) {
  HexCoordOffset offset;
  int parity = axial->r & 1;
  offset.q = axial->q + (axial->r - parity) / 2;
  offset.r = axial->r;
  return offset;
}

HexCoordCube hex_axial_to_cube(const HexCoordAxial *axial) {
  HexCoordCube cube;
  cube.x = axial->q;
  cube.y = axial->r;
  cube.z = -cube.x - cube.y;
  return cube;
}

HexCoordAxial hex_cube_to_axial(const HexCoordCube *cube) {
  HexCoordAxial axial;
  axial.q = cube->x;
  axial.r = cube->y;
  return axial;
}

HexCoordCube hex_offset_to_cube(const HexCoordOffset *offset) {
  HexCoordAxial axial = hex_offset_to_axial(offset);
  return hex_axial_to_cube(&axial);
}

HexCoordOffset hex_cube_to_offset(const HexCoordCube *cube) {
  HexCoordAxial axial = hex_cube_to_axial(cube);
  return hex_axial_to_offset(&axial);
}

void hex_axial_shift(HexCoordAxial *axial, int dx, int dy, int dz) {
  axial->q += dx;
  axial->r += dy;
  axial->q += dz;
  axial->r -= dz;
}

HexCoordAxial hex_subtract(const HexCoordAxial *a, const HexCoordAxial *b) {
  HexCoordAxial result;
  result.q = a->q - b->q;
  result.r = a->r - b->r;
  return result;
}

HexCoordAxial hex_add(const HexCoordAxial *a, const HexCoordAxial *b) {
  HexCoordAxial result;
  result.q = a->q + b->q;
  result.r = a->r + b->r;
  return result;
}

HexCoordAxial hex_multiply(const HexCoordAxial *a, int scalar) {
  HexCoordAxial result;
  result.q = a->q * scalar;
  result.r = a->r * scalar;
  return result;
}

int hex_cube_magnitude(const HexCoordCube *cube) {
  return (abs(cube->x) + abs(cube->y) + abs(cube->z)) / 2;
}

int hex_axial_magnitude(const HexCoordAxial *axial) {
  HexCoordCube cube = hex_axial_to_cube(axial);
  return hex_cube_magnitude(&cube);
}
