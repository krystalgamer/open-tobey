#ifndef W32_ALLOC_H
#define W32_ALLOC_H

void *os_malloc(int size);

void *os_malloc32(int size);
void os_free32(void* p);

#endif
