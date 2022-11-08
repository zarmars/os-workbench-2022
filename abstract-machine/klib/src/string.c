#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int n;
  for (n = 0; s[n]; n++)
    ;
  return n;
}

char *strcpy(char *dst, const char *src) {
  char *os = dst;
  while((*dst++ = *src++) != '\0')
    ;
  return os;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *os = dst;
  while(n-- > 0 && (*dst++ = *src++) != '\0')
    ;
  while(n-- > 0)
    *dst++ = 0;
  return os;
}

char *strcat(char *dst, const char *src) {
  panic_on(dst == 0, "null destination string.");
  size_t dest_len = strlen(dst);
  size_t i;
  for (i = 0; src[i]; i++)
    dst[dest_len + i] = src[i];
  dst[dest_len + i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1 && *s1 == *s2)
    s1++, s2++;
  return (uint8_t)*s1 - (uint8_t)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while(n > 0 && *s1 && *s1 == *s2)
    n--, s1++, s2++;
  if (n == 0)
    return 0;
  return (uint8_t)*s1 - (uint8_t)*s2;
}

void *memset(void *s, int c, size_t n) {
  char *cdst = (char *) s;
  for (int i = 0; i < n; i++) {
    cdst[i] = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  const char *s;
  char *d;
  if (n == 0) return dst;

  s = (const char *)src;
  d = (char *)dst;

  if (s < d && s + n > d) {
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else {
    while(n-- > 0)
      *d++ = *s++;
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *v1, *v2;
  v1 = s1;
  v2 = s2;
  while(n > 0 && *v1 == *v2)
    n--, v1++, v2++;
  if (n == 0)
    return 0;
  return *v1 - *v2;
}

#endif
