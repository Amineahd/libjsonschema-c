/**
 * @file schema_validator.h
 * @author Amine Aouled Hamed
 * @date 04 Nov. 2015
 * @brief Used to validate a schema against the V4 draft specification
 * 
 * @see http://json-schema.org/latest/json-schema-validation.html
 */

#include "json.h"
#include "linkhash.h"

//terminal colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static int global_res = 1;
/**
 * @brief Prints a colored message in the terminal.
 */
void
printf_colored(char* color,char* buff);

/**
 * @brief Sorts items of a JSON array. copied from test1.c
 */
static int 
sort_fn (const void *j1, const void *j2);

/**
 * @brief Validation handler for numeric types
 */
int 
json_validate_numeric_keywords(struct lh_entry* keyword, struct lh_table* parent);

/**
 * @brief Validation handler for the string type
 */
int 
json_validate_string_keywords(struct lh_entry* keyword, struct lh_table* parent);

/**
 * @brief Validates an array's keywords
 */
int 
json_validate_array_keywords(struct lh_entry* keyword, struct lh_table* parent);
/**
 * @brief Validates an object's keywords
 */
int 
json_validate_object_keywords(struct lh_entry* keyword, struct lh_table* parent);

/**
 * @brief Validates an object
 */
int
json_validate_object(json_object *jobj);

/**
 * @brief Checks that array items are unique
 */
int 
json_validate_array_items_uniqueness(json_object *jobj);

/**
 * @brief Checks that array items are objects with valid JSON schema
 */
int
json_validate_array_items( json_object *jobj);

void 
json_parse_object(json_object *jobj);

void 
json_parse_array( json_object *jobj, char *key);

/**
 * @brief Validates a keyword, by checking the provided keyword against the rules of the V4 draft. check schema_rules.txt file
 */
int
validate_keyword(struct lh_entry* keyword, struct lh_table* parent_type);

/**
 * @brief Validates a schema, by checking the provided schema against the rules of the V4 draft. check schema_rules.txt file
 */
int 
json_validate_schema(char* filename);