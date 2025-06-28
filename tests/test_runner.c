#include "unity.h"

void test_init_game_logic(void);

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_init_game_logic);
  return UNITY_END();
}
