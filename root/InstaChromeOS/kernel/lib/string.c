#include "string.h"

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while(n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    if(n == (size_t)-1) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while(s[len]) len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    char* original = dest;
    while((*dest++ = *src++));
    return original;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for(i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for(; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}