#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

// test_hexcoords.c
void test_hex_offset_to_axial(void);

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_hex_offset_to_axial);
  return UNITY_END();
}
