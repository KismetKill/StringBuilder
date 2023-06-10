#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char *value;
    size_t length;
    size_t capacity;
} StringBuilder;

StringBuilder sb_create(void);
StringBuilder sb_create_and_allocate(size_t capacity);
void sb_destroy(StringBuilder *sb);
void sb_init(StringBuilder *sb);
bool sb_ensure_capacity(StringBuilder *sb, size_t min_capacity);
void sb_append(StringBuilder *sb, const char *value);
void sb_append_ex(StringBuilder *sb, const char *value, size_t length);
void sb_append_line(StringBuilder *sb, const char *value);
void sb_append_format(StringBuilder *sb, const char *format, ...);
void sb_append_format_list(StringBuilder *sb, const char *format, va_list args);
void sb_append_char(StringBuilder *sb, char value);
void sb_insert(StringBuilder *sb, size_t index, const char *value);
void sb_insert_ex(StringBuilder *sb, size_t index, const char *value, size_t length);
void sb_remove(StringBuilder *sb, size_t index, size_t length);
void sb_replace(StringBuilder *sb, const char *old_value, const char *new_value);
void sb_clear(StringBuilder *sb);

#ifdef __cplusplus
}
#endif
#endif // STRING_BUILDER_H


#ifdef STRING_BUILDER_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

#ifndef STRING_BUILDER_NEWLINE
#define STRING_BUILDER_NEWLINE "\n"
#endif

StringBuilder sb_create(void)
{
    StringBuilder sb;
    sb_init(&sb);
    return sb;
}

StringBuilder sb_create_and_allocate(size_t capacity)
{
    StringBuilder sb;
    sb_init(&sb);

    if (capacity > 0 && sb_ensure_capacity(&sb, capacity))
        sb.value[0] = '\0';

    return sb;
}

void sb_destroy(StringBuilder *sb)
{
    free(sb->value);
}

void sb_init(StringBuilder *sb)
{
    sb->value = NULL;
    sb->length = 0;
    sb->capacity = 0;
}

bool sb_ensure_capacity(StringBuilder *sb, size_t min_capacity)
{
    if (sb->capacity < min_capacity) {
        size_t capacity = 16;
        while (capacity < min_capacity && capacity > 0)
            capacity *= 2;

        if (capacity == 0)
            return false;

        char *value = (char *)realloc(sb->value, capacity);
        if (value == NULL)
            return false;

        sb->value = value;
        sb->capacity = capacity;
    }
    return true;
}

void sb_append(StringBuilder *sb, const char *value)
{
    if (value)
        sb_append_ex(sb, value, strlen(value));
}

void sb_append_ex(StringBuilder *sb, const char *value, size_t length)
{
    if (length > 0 && sb_ensure_capacity(sb, sb->length + length + 1)) {
        memcpy(sb->value + sb->length, value, length);
        sb->length += length;
        sb->value[sb->length] = '\0';
    }
}

void sb_append_line(StringBuilder *sb, const char *value)
{
    sb_append(sb, value);
    sb_append(sb, STRING_BUILDER_NEWLINE);
}

void sb_append_format(StringBuilder *sb, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    sb_append_format_list(sb, format, args);
    va_end(args);
}

void sb_append_format_list(StringBuilder *sb, const char *format, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    char buffer[4096];
    int needed_length = vsnprintf(buffer, sizeof(buffer), format, args_copy);

    va_end(args_copy);

    if (needed_length < 0) {
        return;
    }

    size_t length = (size_t)needed_length;
    if (length < sizeof(buffer)) {
        sb_append_ex(sb, buffer, length);
    } else {
        if (sb_ensure_capacity(sb, sb->length + length + 1)) {
            vsnprintf(sb->value + sb->length, length + 1, format, args);
            sb->length += length;
        }
    }
}

void sb_append_char(StringBuilder *sb, char value)
{
    if (value != '\0')
        sb_append_ex(sb, &value, 1);
}

void sb_insert(StringBuilder *sb, size_t index, const char *value)
{
    if (value)
        sb_insert_ex(sb, index, value, strlen(value));
}

void sb_insert_ex(StringBuilder *sb, size_t index, const char *value, size_t length)
{
    if (index < sb->length) {
        if (length > 0 && sb_ensure_capacity(sb, sb->length + length + 1)) {
            char *offset = sb->value + index;
            memmove(offset + length, offset, sb->length - index);
            memmove(offset, value, length);

            sb->length += length;
            sb->value[sb->length] = '\0';
        }
    } else {
        sb_append_ex(sb, value, length);
    }
}

void sb_remove(StringBuilder *sb, size_t index, size_t length)
{
    if (index >= sb->length || length == 0)
        return;

    if (index + length < sb->length) {
        size_t num_chars_to_move = sb->length - length - index;
        char *offset = sb->value + index;
        memmove(offset, offset + length, num_chars_to_move);

        sb->length -= length;
    } else {
        sb->length = index;
    }
    sb->value[sb->length] = '\0';
}

void sb_replace(StringBuilder *sb, const char *old_value, const char *new_value)
{
    if (sb->length == 0 || old_value == NULL || new_value == NULL)
        return;

    size_t old_length = strlen(old_value);
    size_t new_length = strlen(new_value);
    char *match = strstr(sb->value, old_value);

    if (old_length == 0 || match == NULL)
        return;

    if (old_length == new_length) {
        while (match) {
            memcpy(match, new_value, new_length);
            match = strstr(match + old_length, old_value);
        }
    } else {
        StringBuilder tmp = sb_create();
        char *start = sb->value;

        while (match) {
            sb_append_ex(&tmp, start, match - start);
            sb_append_ex(&tmp, new_value, new_length);
            start = match + old_length;
            match = strstr(start, old_value);
        }

        if (*start != '\0')
            sb_append_ex(&tmp, start, sb->value + sb->length - start);

        free(sb->value);
        *sb = tmp;
    }
}

void sb_clear(StringBuilder *sb)
{
    if (sb->value)
        sb->value[0] = '\0';

    sb->length = 0;
}

#endif // STRING_BUILDER_IMPLEMENTATION
