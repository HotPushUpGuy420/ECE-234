#include <stdio.h>
#include <stdint.h>

void check(uint16_t u16_a1, uint16_t u16_b1, uint16_t u16_c1, uint8_t u8_d1, uint8_t u8_e1) {
  static uint16_t u16_a = 0xE494;
  static uint16_t u16_b = 0x29A5;
  static uint16_t u16_c = 0xA57D;
  static uint8_t u8_d = 0x8F;
  static uint8_t  u8_e = 0;
  static int16_t i16_lastFailure = -1;

  // Indicate start of test
  if (u8_e == 0)
    printf("\n\n\nTest started.\n");

// Print out expected vs. acutal values
  printf("CORRECT a:%04x,  b:%04x,  c: %04x,  d:%02x,  e:%02x \n",
         u16_a,     u16_b,    u16_c,     u8_d,   u8_e);
  printf("OUTPUT  a1:%04x, b1:%04x, c1: %04x, d1:%02x, e1:%02x \n",
         u16_a1,    u16_b1,  u16_c1,     u8_d1,  u8_e1);

  // Print out a pass/fail indication
  if ((u16_a == u16_a1) && (u16_b == u16_b1) &&
      (u16_c == u16_c1) && (u8_d == u8_d1) &&
      (u8_e == u8_e1)) {
    printf("PASS");
    if (i16_lastFailure >= 0)
      printf("; last failure was at loop %02x.\n", i16_lastFailure);
    else
      printf("\n");
  } else {
    printf("FAIL\n");
    i16_lastFailure = u8_e;
  }

  // Execute body of loop
  if (u16_c & 0x0040) {
    u8_d = ~u8_d - u8_e;
    u16_a = u16_b | u16_c;
    u16_b = u16_b - u16_c;
  } else {
   if (u16_a <= u16_b) {
      u16_a = (u16_a << 2) + 0xA500 + (uint16_t) u8_d;
    } else {
      u16_a = u16_a - u16_b;
    }
  }
  u16_c = ~( (u16_c >> 1) - u16_a);
  u8_e++;
}



