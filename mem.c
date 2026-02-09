#include "mem.h"

#include <stdlib.h>
#include <string.h>

typedef struct DebugHeader {
    size_t size;
    size_t allocNumber;
    struct DebugHeader *next;
    struct DebugHeader *prev;
} DebugHeader;

static DebugHeader *dbgHead = NULL;
static size_t debugBytesAlive = 0;
static size_t debugAllocCount = 0;

IAllocator BasicAllocator = {
    .alloc = malloc,
    .free = free,
    .realloc = realloc,
};

IAllocator SafeAllocator = {
    .alloc = xalloc,
    .free = xfree,
    .realloc = xrealloc,
};

IAllocator DebugAllocator = {
    .alloc = debugAlloc,
    .free = debugFree,
    .realloc = debugRealloc,
};

static inline void checkPtr(void *ptr) {
    if (!ptr) {
        panic("out of memory");
    }
}

void *xalloc(size_t sz) {
    if (!sz) return NULL;

    void *ptr = malloc(sz);
    checkPtr(ptr);

    return ptr;
}

void *xrealloc(void *ptr, size_t sz) {
    void *newPtr = realloc(ptr, sz);
    checkPtr(newPtr);

    return newPtr;
}

void xfree(void *ptr) {
    if (!ptr) return;

    DebugHeader *h = ((DebugHeader *)ptr) - 1;

    if (h->prev) h->prev->next = h->next;
    else dbgHead = h->next;

    if (h->next) h->next->prev = h->prev;

    debugBytesAlive -= h->size;
    free(h);
}

void *debugAlloc(size_t sz) {
    if (!sz) return NULL;

    DebugHeader *dh = malloc(sizeof(DebugHeader) + sz);
    checkPtr(dh);
    
    debugAllocCount += 1;

    dh->size = sz;
    dh->allocNumber = debugAllocCount;
    dh->next = dbgHead;
    dh->prev = NULL;

    if (dbgHead) dbgHead->prev = dh;
    dbgHead = dh;

    debugBytesAlive += sz;

    return (void *)(dh + 1);
}

void *debugRealloc(void *ptr, size_t sz) {
    if (!ptr) return debugAlloc(sz);

    if (sz == 0) {
        debugFree(ptr);
        return NULL;
    }

    DebugHeader *old = ((DebugHeader *)ptr) - 1;
    size_t oldSize = old->size;

    DebugHeader *new =
        realloc(old, sizeof(DebugHeader) + sz);
    checkPtr(new);

    if (new != old) {
        if (new->prev) new->prev->next = new;
        else dbgHead = new;

        if (new->next) new->next->prev = new;
    }

    new->size = sz;

    debugBytesAlive -= oldSize;
    debugBytesAlive += sz;

    return (void *)(new + 1);
}


void debugFree(void *ptr) {
    if (!ptr) return;

    DebugHeader *h = ((DebugHeader *)ptr) - 1;

    if (h->prev) h->prev->next = h->next;
    else dbgHead = h->next;

    if (h->next) h->next->prev = h->prev;

    debugBytesAlive -= h->size;
    free(h);
}

void debugReportLeaks(void) {
    DebugHeader *h = dbgHead;

    if (!h) {
        fprintf(stderr, "No memory leaks detected.\n");
        return;
    }

    fprintf(stderr, "MEMORY LEAKS DETECTED:\n");

    while (h) {
        fprintf(stderr,
            "Leak: allocation %zu leaked %zu bytes \n", h->allocNumber, h->size
        );
        h = h->next;
    }

    fprintf(stderr, "Total leaked: %zu bytes\n", debugBytesAlive);
}

char *xstrdup(IAllocator allocator, const char *s) {
    if (!s) return NULL;

    size_t len = strlen(s);
    char *dup = allocator.alloc(len + 1);

    for (size_t i = 0; i < len; i++) {
        dup[i] = s[i];
    }
    dup[len] = '\0';

    return dup;
}