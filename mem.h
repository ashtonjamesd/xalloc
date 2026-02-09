#ifndef mem_h
#define mem_h

#include <stdio.h>

#define panic(msg) do { \
    fprintf(stderr, "panic in %s:%d: %s\n", __FILE__, __LINE__, msg);  \
    exit(1); \
} while (0)

typedef void *(*AllocFunction)(size_t sz);
typedef void (*FreeFunction)(void *ptr);
typedef void *(*ReallocFunction)(void *ptr, size_t sz);

typedef struct {
    AllocFunction alloc;
    FreeFunction free;
    ReallocFunction realloc;
} Allocator;

typedef const Allocator IAllocator;

void *xalloc(size_t sz);
void xfree(void *ptr);
void *xrealloc(void *ptr, size_t sz);

void *debugAlloc(size_t sz);
void debugFree(void *ptr);
void *debugRealloc(void *ptr, size_t sz);
void debugReportLeaks(void);

char *xstrdup(IAllocator allocator, const char *s);

extern IAllocator BasicAllocator;
extern IAllocator SafeAllocator;
extern IAllocator DebugAllocator;

#endif