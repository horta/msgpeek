#include "c_toolbelt/c_toolbelt.h"
#include "lite_pack/file/read_bool.h"
#include "lite_pack/lite_pack.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static inline bool fpeek(FILE *fp)
{
    int ch = fgetc(fp);
    ungetc(ch, fp);
    return ch != EOF;
}

#ifndef __always_inline
#define __always_inline inline
#endif

__attribute__((const)) static __always_inline int bug_on_reach(void)
{
    assert(0);
    return 0;
}

static void echo(char const *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}
static void indent(unsigned lvl)
{
    for (unsigned i = 0; i < lvl; ++i)
        echo("  ");
}

static void newline(void) { echo("\n"); }

static char *str = 0;
static unsigned str_length = 0;

static struct lip_file f = {0};
struct lip_object obj = {0};

static void echo_nil(void) { echo("null"); }

static void echo_bool(bool v)
{
    if (v)
        echo("true");
    else
        echo("false");
}

bool visit(unsigned lvl)
{
    if (!lip_read_object(&f, &obj)) return false;

    enum lip_format fmt = obj.format;
    enum lip_format_family family = obj.family;

    unsigned size = 0;
    switch (family)
    {
    case LIP_FMT_FAMILY_NIL:

        echo_nil();
        break;

    case LIP_FMT_FAMILY_BOOL:

        echo_bool(obj.value.b);
        break;

    case LIP_FMT_FAMILY_INT:

        if (fmt == LIP_FMT_POSITIVE_FIXINT) echo("%u", obj.value.u8);

        if (fmt == LIP_FMT_INT_8) echo("%d", obj.value.i8);
        if (fmt == LIP_FMT_INT_16) echo("%d", obj.value.i16);
        if (fmt == LIP_FMT_INT_32) echo("%ld", obj.value.i32);
        if (fmt == LIP_FMT_INT_64) echo("%lld", obj.value.i64);

        if (fmt == LIP_FMT_UINT_8) echo("%u", obj.value.u8);
        if (fmt == LIP_FMT_UINT_16) echo("%u", obj.value.u16);
        if (fmt == LIP_FMT_UINT_32) echo("%lu", obj.value.u32);
        if (fmt == LIP_FMT_UINT_64) echo("%llu", obj.value.u64);

        if (fmt == LIP_FMT_NEGATIVE_FIXINT) echo("%d", obj.value.i8);
        break;

    case LIP_FMT_FAMILY_FLOAT:

        if (fmt == LIP_FMT_FLOAT_32) echo("%f", obj.value.f32);
        if (fmt == LIP_FMT_FLOAT_64) echo("%f", obj.value.f64);
        break;

    case LIP_FMT_FAMILY_STR:

        if (!(str = ctb_realloc(str, obj.value.size + 1))) return false;
        str_length = obj.value.size;
        if (!lip_read_str_data(&f, str_length, str)) return false;
        str[str_length] = 0;
        echo("\"%s\"", str);
        break;

    case LIP_FMT_FAMILY_BIN:

        return !bug_on_reach();

    case LIP_FMT_FAMILY_ARRAY:

        echo("[");
        size = obj.value.size;
        for (unsigned i = 0; i < size; ++i)
        {
            newline();
            indent(lvl + 1);
            if (!visit(lvl + 1)) return false;
            if (i + 1 < size)
                echo(",");
            else
            {
                newline();
                indent(lvl);
            }
        }
        echo("]");
        break;

    case LIP_FMT_FAMILY_MAP:

        echo("{");
        size = obj.value.size;
        for (unsigned i = 0; i < size; ++i)
        {
            newline();
            indent(lvl + 1);
            if (!visit(lvl + 1)) return false;
            echo(": ");
            if (!visit(lvl + 1)) return false;
            if (i + 1 < size)
                echo(",");
            else
            {
                newline();
                indent(lvl);
            }
        }
        echo("}");
        break;

    case LIP_FMT_FAMILY_EXT:

        echo("%u", obj.value.size);
        break;

    case LIP_FMT_FAMILY_NEVER_USED:

        return !bug_on_reach();

    default:
        return !bug_on_reach();
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        echo("Usage: msgpeek FILE\n");
        return 1;
    }

    char const *filepath = argv[1];
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        echo("failed to open %s file\n", filepath);
        return 1;
    }

    lip_object_init(&obj);
    lip_file_init(&f, file);

    bool ok = true;
    while (fpeek(file))
    {
        if (!(ok = visit(0))) break;
    }

    fclose(file);
    free(str);

    return !ok;
}
