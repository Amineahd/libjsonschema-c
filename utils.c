#include "utils.h"

void
printf_colored(char* color,char* buff) {
    printf("%s %s \n"ANSI_COLOR_RESET, color, buff);
}

int 
sort_fn (const void *j1, const void *j2)
{
    json_object * const *jso1, * const *jso2;
    char* i1;
    char* i2;
    

    jso1 = (json_object* const*)j1;
    jso2 = (json_object* const*)j2;
    if (!*jso1 && !*jso2)
            return 0;
    if (!*jso1)
            return -1;
    if (!*jso2)
            return 1;

    i1 = (char *)json_object_get_string(*jso1);
    i2 = (char *)json_object_get_string(*jso2);
    return strcmp(i1,i2);
}