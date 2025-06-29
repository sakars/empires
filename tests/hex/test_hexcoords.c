#include "hexcoords.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_hex_offset_to_axial(void) {
  HexCoordOffset offset = {-2, -1};
  HexCoordAxial axial = hex_offset_to_axial(&offset);

  TEST_ASSERT_EQUAL(-1, axial.r);
  TEST_ASSERT_EQUAL(-1, axial.q);

  offset.q = 0;
  offset.r = 0;
  axial = hex_offset_to_axial(&offset);
  TEST_ASSERT_EQUAL(0, axial.r);
  TEST_ASSERT_EQUAL(0, axial.q);

  offset.q = 2;
  offset.r = -2;
  axial = hex_offset_to_axial(&offset);
  TEST_ASSERT_EQUAL(3, axial.q);
  TEST_ASSERT_EQUAL(-2, axial.r);

  offset.q = 1;
  offset.r = 2;
  axial = hex_offset_to_axial(&offset);
  TEST_ASSERT_EQUAL(0, axial.q);
  TEST_ASSERT_EQUAL(2, axial.r);
}

void test_hex_axial_to_offset(void) {
  HexCoordAxial axial = {-1, -1};
  HexCoordOffset offset = hex_axial_to_offset(&axial);

  TEST_ASSERT_EQUAL(-2, offset.q);
  TEST_ASSERT_EQUAL(-1, offset.r);

  axial.q = 0;
  axial.r = 0;
  offset = hex_axial_to_offset(&axial);
  TEST_ASSERT_EQUAL(0, offset.q);
  TEST_ASSERT_EQUAL(0, offset.r);

  axial.q = 3;
  axial.r = -2;
  offset = hex_axial_to_offset(&axial);
  TEST_ASSERT_EQUAL(2, offset.q);
  TEST_ASSERT_EQUAL(-2, offset.r);

  axial.q = 0;
  axial.r = 2;
  offset = hex_axial_to_offset(&axial);
  TEST_ASSERT_EQUAL(1, offset.q);
  TEST_ASSERT_EQUAL(2, offset.r);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_hex_offset_to_axial);
  RUN_TEST(test_hex_axial_to_offset);
  return UNITY_END();
}
