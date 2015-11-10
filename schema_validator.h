/**
 * @file schema_validator.h
 * @author Amine Aouled Hamed
 * @date 04 Nov. 2015
 * @brief Used to validate a schema against the V4 draft specification
 * 
 * @see http://json-schema.org/latest/json-schema-validation.html
 */

#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "json.h"
#include "linkhash.h"
#include "utils.h"


//all reserved keywords
char* keywords[] = {"multipleOf","maximum","minimum","exclusiveMinimum","exclusiveMaximum",
                 "maxLength","minLength","pattern",
                 "additionalItems","items","minItems","maxItems","uniqueItems",
                 "maxProperties","minProperties","required","properties","patternProperties","additionalProperties","dependencies" 
                };

static int global_res = 1;


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
json_validate_object(json_object *jobj, int last_pos);

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