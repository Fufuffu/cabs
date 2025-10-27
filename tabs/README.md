# tabs.h - Thread Abstraction Library

A simple, cross-platform, single-header thread abstraction library for C (Windows threads and pthreads).

## Supported Platforms

- Windows
- macOS (untested)
- Linux

## Usage

In exactly one C file, define the implementation before including the header:

```c
#define TABS_IMPLEMENTATION
#include "tabs.h"
```

In all other files, just include the header:

```c
#include "tabs.h"
```

## Examples

### counter.c

Demonstrates mutex usage with multiple threads incrementing a shared counter. 

**Build and run**:
```bash
gcc counter.c -o counter
./counter
```

### sum.c

Demonstrates how to create threads and run logic on them

**Build and run**:
```bash
gcc sum.c -o sum
./sum
```

## License

MIT. See project license for details.
