#ifndef SHLEX_H_
#define SHLEX_H_

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *source;
    const char *source_end;
    const char *point;

    char *string;
    size_t string_count;
    size_t string_capacity;
} Shlex;

// Sets the source
void shlex_init(Shlex *s, const char *source, const char *source_end);

// Generally you don't need to free the shlex if you plan to reuse it several times.
// Just do shlex_init() for each input you need to process and it is going to reuse
// the memory it allocated every time.
void shlex_free(Shlex *s);

// Return false means ran out of tokens.
bool shlex_next(Shlex *s);

#endif // SHLEX_H_

#ifdef SHLEX_IMPLEMENTATION

static void shlex__string_append(Shlex *s, char x);

void shlex_init(Shlex *s, const char *source, const char *source_end)
{
    s->source = source;
    s->source_end = source_end;
    s->point = source;
}

void shlex_free(Shlex *s)
{
    free(s->string);
    memset(s, 0, sizeof(*s));
}

bool shlex_next(Shlex *s)
{
    while (s->point < s->source_end && isspace(*s->point)) {
        s->point++;
    }

    if (s->point >= s->source_end) return false;

    s->string_count = 0;
    char strlit = 0;
    while (s->point < s->source_end) {
        switch (strlit) {
        // POSIX.1-2024 - 2.2.2 Single-Quotes
        // > Enclosing characters in single-quotes ('') shall preserve the literal value of each character within the single-quotes. A single-quote cannot occur within single-quotes.
        case '\'':
            if (*s->point == '\'') {
                strlit = 0;
            } else {
                shlex__string_append(s, *s->point);
            }
            s->point++;
            break;
        // POSIX.1-2024 - 2.2.3 Double-Quotes
        // > Enclosing characters in double-quotes ("") shall preserve the literal value of all characters within the double-quotes, with the exception of the characters backquote, <dollar-sign>, and <backslash> ...
        // We only care about <backslash> since <dollar-sign> and backquote is related to semantics of the Shell language. We do a pure lexical analysis.
        case '"':
            switch (*s->point) {
            case '"':
                strlit = 0;
                s->point++;
                break;
            case '\\':
                s->point++;
                // We don't really know what to do with unfinished escape sequences in the context of shlex, so we just interpret the <backslash> literally
                if (s->point >= s->source_end) {
                    shlex__string_append(s, '\\');
                    shlex__string_append(s, '\0');
                    return s->string_count > 0;
                }

                switch (*s->point) {
                // POSIX.1-2024 - 2.2.3 Double-Quotes
                // > <backslash> shall retain its special meaning as an escape character (see 2.2.1 Escape Character (Backslash)) only when immediately followed by one of the following characters $   `   \   <newline> or by a double-quote character that would otherwise be considered special
                case '$':
                case '`':
                case '\\':
                case '\n':
                case '"':
                    shlex__string_append(s, *s->point);
                    break;
                default:
                    shlex__string_append(s, '\\');
                    shlex__string_append(s, *s->point);
                }
                s->point++;
                break;
            default:
                shlex__string_append(s, *s->point);
                s->point++;
            }
            break;
        case '\0':
            switch (*s->point) {
            case '"':
            case '\'':
                strlit = *s->point;
                s->point++;
                break;
            // POSIX.1-2024 - 2.2.1 Escape Character (Backslash)
            // > A <backslash> that is not quoted shall preserve the literal value of the following character, ...
            case '\\':
                s->point++;
                if (s->point < s->source_end) {
                    shlex__string_append(s, *s->point);
                    s->point++;
                }
                break;
            default:
                if (isspace(*s->point)) {
                    shlex__string_append(s, '\0');
                    return true;
                }
                shlex__string_append(s, *s->point);
                s->point++;
            }
            break;
        default:
            assert(0 && "UNREACHABLE");
        }
    }

    shlex__string_append(s, '\0');
    return true;
}

static void shlex__string_append(Shlex *s, char x)
{
    if (s->string_count >= s->string_capacity) {
        if (s->string_capacity == 0) {
            s->string_capacity = 256;
        } else {
            s->string_capacity *= 2;
        }
        s->string = realloc(s->string, s->string_capacity);
    }
    s->string[s->string_count++] = x;
}

#endif // SHLEX_IMPLEMENTATION

#ifdef SHLEX_SELF_TEST

#include <stdio.h>

int main(void)
{
    const char *sources[] = {
        "Foo Bar",
        "Foo\\ Bar",
        "Foo\\ \\ Bar",
        "-foo -bar -baz",
        "'-foo -bar -baz'",
        "\"Hello, World\"     'Foo Bar'",
        "-I\"./raylib/\" -C link-args=\"-L\\\"./hello world\\\" -lm -lc\" -O3",
    };
    size_t sources_count = sizeof(sources)/sizeof(sources[0]);
    Shlex s = {0};

    for (size_t i = 0; i < sources_count; ++i) {
        const char *source = sources[i];
        shlex_init(&s, source, source + strlen(source));
        while (shlex_next(&s)) {
            printf("%s\n", s.string);
        }
        printf("------------------------------\n");
    }
    return 0;
}

#endif // SHLEX_SELF_TEST
