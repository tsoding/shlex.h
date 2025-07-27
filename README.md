# Simple lexical analysis for Shell Syntax

Python's [shlex](https://docs.python.org/3/library/shlex.html) but in C.

## Quick Start

### Splitting

```c
#include <stdio.h>
#define SHLEX_IMPLEMENTATION
#include "shlex.h"

int main(void)
{
    const char *source = "-C link-args=\"-lm -L.\"";
    Shlex s = {0};
    shlex_init(&s, source, source + strlen(source));
    while (shlex_next(&s)) {
        printf("%s\n", s.string);
    }
    shlex_free(&s);
    return 0;
}
```

### Joining

```c
#include <stdio.h>
#define SHLEX_IMPLEMENTATION
#include "shlex.h"

int main(void)
{
    Shlex s = {0};
    shlex_append_quoted(&s, "-C");
    shlex_append_quoted(&s, "link-args=-lm -L.");
    printf("%s\n", shlex_join(&s));
    shlex_free(&s);
    return 0;
}
```

## References

- https://docs.python.org/3/library/shlex.html
- https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html
