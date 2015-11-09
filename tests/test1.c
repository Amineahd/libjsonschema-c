#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <schema_validator.h>

#ifdef TEST_FORMATTED
#define json_object_to_json_string(obj) json_object_to_json_string_ext(obj,sflags)
#else
/* no special define */
#endif

int main(int argc, char **argv)
{
        printf("Checking schema_validator!\n");
        json_validate_schema("../example_schema.txt");
	return 0;
}
