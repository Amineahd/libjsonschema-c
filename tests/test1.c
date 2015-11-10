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

int main(int argc, char *argv[])
{
        char schema_file[] = "example_schema.txt";
        printf("Checking schema_validator!\n");
        if(argc > 1)
            json_validate_schema(argv[1]);
        else 
            json_validate_schema("example_schema.txt");
	return 0;
}
