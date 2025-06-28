#include "hexcoords.h"
#include "unity.h"

void test_hex_offset_to_axial(void) {
  HexCoordOffset offset = {2, 3};
  HexCoordAxial axial = hex_offset_to_axial(&offset);

  TEST_ASSERT_EQUAL(2, axial.x);
  TEST_ASSERT_EQUAL(2, axial.y); // 3 - (2 + (2 & 1)) / 2 = 2
}