# Simple lexical analysis for Shell Syntax

Python's shlex.split() but in C

## Quick Start

```c
#include <stdio.h>
#define SHLEX_IMPLEMENTATION
#include "shlex.h"

int main(void)
{
    const char *source = "-C link-args=\"-lm -L.\"";
    Shlex s = {0};
    shlex_init(&s, source, source + strlen(source));
    while (slex_next(&s)) {
        printf("%s\n", s.string);
    }
    shlex_free(&s);
    return 0;
}
```

## References

- https://docs.python.org/3/library/shlex.html
- https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html
