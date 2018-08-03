#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/*

gcc -std=c11 -Wall -o converter.o converter.c
./converter.o utf16_test.txt
*/

void convert_to_utf8_bytes(uint32_t c);

int utf16le_to_utf8(FILE* f);

int main(int argc, char* argv[argc+1]) {
  FILE*     f;
  char*     f_name;

  if (argc > 1) {
    f_name = argv[1];
    f = fopen(f_name, "rb");

    utf16le_to_utf8(f);
    fclose(f);
  } else {
    puts("Missing filename");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void convert_to_utf8_bytes(uint32_t c) {
  if (c >= 0x0 && c <= 0x7F) {
    putchar(c);
  } else if (c >= 0x80 && c <= 0x7FF) {
    putchar((c >> 6) | 0xc0);
    putchar((c & 0x3f) | 0x80);
  } else if (c >= 0x800 && c <= 0xFFFF) {
    putchar((c >> 12) | 0xe0);
    putchar(((c >> 6) & 0x3f) | 0x80);
    putchar((c & 0x3f) | 0x80);
  } else if (c >= 0x10000 && c <= 0x10FFFF) {
    putchar((c >> 18) | 0xf0);
    putchar(((c >> 12) & 0x3f) | 0x80);
    putchar(((c >> 6) & 0x3f) | 0x80);
    putchar((c & 0x3f) | 0x80);
  }
}

int utf16le_to_utf8(FILE* f) {
  uint16_t buf[1];
  uint16_t code;
  uint32_t code_point;

  while (fread(buf, 2, 1, f) != '\0') {
    code_point = (uint32_t) buf[0];

    if ((code_point >= 0x0 && code_point <= 0xD7FF) ||
	(code_point >= 0xE000 && code_point <= 0xFFFF)) {

      convert_to_utf8_bytes(code_point);
    } else if (code_point >= 0xD800 && code_point <= 0xDBFF) {
      code = ((code_point & 0x3FF) << 10);
    } else if (code_point >= 0xDC00 && code_point <= 0xDFFF) {
      code_point = (code | (code_point & 0x3FF)) + 0x10000;
      convert_to_utf8_bytes(code_point);
    } else {
      puts("Invalid encoding");
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
