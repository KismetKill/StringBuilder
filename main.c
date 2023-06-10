#define STRING_BUILDER_IMPLEMENTATION
#include "stringbuilder.h"
#include <stdio.h>

char *assemble_some_string(void)
{
    StringBuilder sb = sb_create();

    sb_append_line(&sb, "String builder test");
    sb_append_line(&sb, "===================");
    sb_append_line(&sb, "");

    for (int i = 1; i <= 5; i++)
        sb_append_format(&sb, "%d. Some item\n", i);

    return sb.value;
}

int main(void)
{
    StringBuilder sb = sb_create();

    sb_append(&sb, "Some content...");
    sb_append_line(&sb, " More content.");
    sb_append_format(&sb, "Some number: %d and a string: \"%s\".\n", 69, "nice");
    sb_insert(&sb, 0, "First! ");

    printf("%s\n", sb.value);
    printf("length: %zu, capacity: %zu\n", sb.length, sb.capacity);

    sb_destroy(&sb);

    char *str = assemble_some_string();
    if (str) {
        printf("%s\n", str);
        free(str);
    }

    return 0;
}
