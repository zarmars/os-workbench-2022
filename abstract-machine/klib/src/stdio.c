#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stdint.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char digits[] = "0123456789abcdef";

static void
printint(int xx, int base, int sign) {
  char buf[16];
  int i;
  unsigned int x;

  if (sign && (sign = xx < 0)) {
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if (sign) {
    buf[i++] = '-';
  }

  while(--i >= 0) {
    putch(buf[i]);
  }
}

static void
printptr(uint64_t x) {
  int i;
  putch('0');
  putch('x');
  for (i = 0; i < (sizeof(uint64_t) * 2); i++, x <<= 4) {
    putch(digits[x >> (sizeof(uint64_t) * 8 - 4)]);
  }
}

int printf(const char *fmt, ...) {
  va_list ap;
  int i, c;
  char* s;

  // TODO: thread-safe implementation
  // acquire lock

  if (fmt == NULL) {
    panic("null fmt");
    return -1;
  }
  va_start(ap, fmt);
  // check char with low-8 bits not equal 0
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      putch(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0) break;
    switch(c) {
      case 'd':
        printint(va_arg(ap, int), 10, 1);
        break;
      case 'x':
        printint(va_arg(ap, int), 16, 1);
        break;
      case 'p':
        printptr(va_arg(ap, uint64_t));
        break;
      case 's':
        if ((s = va_arg(ap, char*)) == 0) {
          s = "(null)";
        }
        for (; *s; s++) {
          putch(*s);
        }
        break;
      case '%':
        putch('%');
        break;
      default:
        // Print unknown % sequence to draw attention.
        putch('%');
        putch(c);
        break;
    }
  }
  va_end(ap);
  // TODO: thread-safe implementation
  // release lock
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
