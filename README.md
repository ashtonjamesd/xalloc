# xalloc

A simple memory allocator library for C that provides safe allocation functions and debugging capabilities.

## Features

- Swap between different allocator implementations easily
- Allocation fails are handled automatically through panic functions
- Track allocations and detect memory leaks

## Quick Start

Include the header in your C files:

```c
#include "mem.h"
```

Compile example:
```bash
gcc -o myprogram myprogram.c mem.c
```

## Allocators

xalloc provides three built-in allocators:

### BasicAllocator
Direct wrappers around standard `malloc`, `free`, and `realloc`.

```c
char *str = BasicAllocator.alloc(100);
if (str == NULL) {
    // handle allocation failure
}
BasicAllocator.free(str);
```

### SafeAllocator
Safe versions that automatically panic on out-of-memory errors. No need for NULL checks!

```c
char *str = SafeAllocator.alloc(100);  // panics if allocation fails
// use str safely - guaranteed to be non-null
SafeAllocator.free(str);
```

### DebugAllocator
Tracks all allocations and can report memory leaks. For development and testing.

```c
char *str = DebugAllocator.alloc(100);
// ... use str ...
DebugAllocator.free(str);

// at program exit:
debugReportLeaks();  // reports any leaked memory
```

## Safe Allocation Functions

The library provides convenient standalone safe allocation functions:

### xalloc(size_t sz)
Allocates memory and panics if allocation fails.

```c
int *numbers = xalloc(10 * sizeof(int));
// numbers is guaranteed to be non-null
for (int i = 0; i < 10; i++) {
    numbers[i] = i * i;
}
```

### xrealloc(void *ptr, size_t sz)
Reallocates memory safely.

```c
int *numbers = xalloc(10 * sizeof(int));
numbers = xrealloc(numbers, 20 * sizeof(int));  // expand array
```

### xfree(void *ptr)
Frees allocated memory. Safe to call with NULL.

```c
xfree(numbers);
xfree(NULL);  // safe - does nothing
```

## String Duplication

Use `xstrdup` with any allocator:

```c
const char *original = "Hello, World!";

// with SafeAllocator (safe, panics on OOM)
char *copy1 = xstrdup(SafeAllocator, original);

// with DebugAllocator (tracked allocation)
char *copy2 = xstrdup(DebugAllocator, original);

// with BasicAllocator (may return NULL)
char *copy3 = xstrdup(BasicAllocator, original);
if (copy3 == NULL) {
    // handle allocation failure
}

SafeAllocator.free(copy1);
DebugAllocator.free(copy2);
BasicAllocator.free(copy3);
```

## Memory Leak Detection

The DebugAllocator tracks all allocations and can detect leaks:

```c
#include "mem.h"

int main(void) {
    // allocate some memory
    char *str1 = debugAlloc(100);
    char *str2 = debugAlloc(50);
    
    // properly free one
    debugFree(str1);
    
    // oops, forgot to free str2
    
    // report leaks at program exit
    debugReportLeaks();
    
    return 0;
}
```

Output:
```
MEMORY LEAKS DETECTED:
Leak: allocation 2 leaked 50 bytes 
Total leaked: 50 bytes
```

## Using Custom Allocators

You can create your own allocator implementations:

```c
void *myCustomAlloc(size_t sz) {
    printf("Allocating %zu bytes\n", sz);
    return malloc(sz);
}

void myCustomFree(void *ptr) {
    printf("Freeing memory\n");
    free(ptr);
}

void *myCustomRealloc(void *ptr, size_t sz) {
    printf("Reallocating to %zu bytes\n", sz);
    return realloc(ptr, sz);
}

IAllocator MyAllocator = {
    .alloc = myCustomAlloc,
    .free = myCustomFree,
    .realloc = myCustomRealloc,
};

// usage
char *data = MyAllocator.alloc(100);
MyAllocator.free(data);
```

## API Reference

### Allocator Interface

```c
typedef struct {
    AllocFunction alloc;      // void *(*alloc)(size_t sz)
    FreeFunction free;        // void (*free)(void *ptr)
    ReallocFunction realloc;  // void *(*realloc)(void *ptr, size_t sz)
} Allocator;
```

### Functions

- `void *xalloc(size_t sz)` - Safe allocation (panics on failure)
- `void *xrealloc(void *ptr, size_t sz)` - Safe reallocation (panics on failure)
- `void xfree(void *ptr)` - Safe free (NULL-safe)
- `void *debugAlloc(size_t sz)` - Debug allocation with tracking
- `void *debugRealloc(void *ptr, size_t sz)` - Debug reallocation
- `void debugFree(void *ptr)` - Free debug allocation
- `void debugReportLeaks(void)` - Report memory leaks
- `char *xstrdup(IAllocator allocator, const char *s)` - Duplicate string

### Built-in Allocators

- `BasicAllocator` - Standard malloc/free/realloc wrappers
- `SafeAllocator` - Safe versions that panic on out-of-memory
- `DebugAllocator` - Tracking allocator for leak detection
