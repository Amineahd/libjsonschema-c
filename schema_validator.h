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
#include <ctype.h>
#include "json.h"
#include "linkhash.h"
#include "utils.h"


//all reserved keywords
char* keywords[] = {"multipleOf","maximum","minimum","exclusiveMinimum","exclusiveMaximum",
                 "maxLength","minLength","pattern",
                 "additionalItems","items","minItems","maxItems","uniqueItems",
                 "maxProperties","minProperties","required","properties","patternProperties","additionalProperties","dependencies",
                 "enum","type","allOf","anyOf","oneOf","not","definitions",
                 "title","description","default","format"
                };

/**
 * @brief Array contains all constraints of each keyword in the V4 spec
 *         {keyword,type,parent_type,dependencies,allowed values}
 *        allowed values = "-1" => no specific rules, just verify the value is of the required type
 *                         "0" => strictly greater than 0
 *                         "1" => greater than or equal to 0
 *                         "2" => array of objects
 *                         "3" => array of unique string items and at least one item is provided
 *                         "4" => a valid regex
 *                         "5" => name of the object should be a valid regex
 *                         "6" => array of unique items and at least one item is provided
 *                         "7" => value must be in a given array of values
 */
char* keywords_constraints[][5] = {  {"multipleOf","2 | 3","2 | 3", "-1","0"},
                                    {"maximum","2 | 3","2 | 3", "-1","-1"},
                                    {"minimum","2 | 3","2 | 3", "-1","-1"},
                                    {"exclusiveMinimum","1","2 | 3", "2","-1"},
                                    {"exclusiveMaximum","1","2 | 3", "1","-1"},
                                    {"maxLength","3","6", "-1","1"},
                                    {"minLength","3","6", "-1","1"},
                                    {"pattern","6","6", "-1","4"},
                                    {"additionalItems","1 | 4","5", "-1","-1"},
                                    {"items","4 | 5","5", "-1","2"},
                                    {"minItems","3","5", "-1","1"},
                                    {"maxItems","3","5", "-1","1"},
                                    {"uniqueItems","1","5", "-1","-1"},
                                    {"maxProperties","3","4", "-1","1"},
                                    {"minProperties","3","4", "-1","1"},
                                    {"required","5","4", "-1","3"},
                                    {"properties","4","4", "-1","-1"},
                                    {"patternProperties","4","4", "-1","5"},
                                    {"additionalProperties","1 | 4","4", "-1","-1"},
                                    {"dependencies","4","4", "-1","3"},
                                    {"enum","5","-1", "-1","6"},
                                    {"type","6","-1", "-1","7"},
                                    {"allOf","5","-1", "-1","2"},
                                    {"anyOf","5","-1", "-1","2"},
                                    {"oneOf","5","-1", "-1","2"},
                                    {"not","4","-1", "-1","-1"},
                                    {"definitions","4","-1", "-1","-1"},
                                    {"title","6","-1", "-1","-1"},
                                    {"description","6","-1", "-1","-1"},
                                    {"default","-1","-1", "-1","-1"},
                                    {"format","6","-1", "-1","7"},
                                };

/**
 * @brief array containing mapping between type values and their IDs in the json-c lib
 */
char * json_types_id[7] = {"null","boolean","double","integer","object","array","string"};

/**
 * @brief Use this array to check for case 7 in the allowed values
 */
char * values[] = { "date-time","email","hostname","ipv4","ipv6","uri", //format keyword
                    "null","boolean","double","integer","object","array","string" // 7 primitive types of json-c
                    };
/**
 * @brief Checks that array items are unique
 */
int 
json_validate_array_items_uniqueness(json_object *jobj,int type);

/**
 * @brief Checks that array items are objects with valid JSON schema
 */
int
json_validate_array_items( json_object *jobj);

/**
 * @brief Checks if the given string is a reserved v4 keyword
 */
int 
is_keyword(char* keyword);

/**
 * @brief Gets all valid types of a keyword from the keywords_constraints array
 */
void
get_allowed_types(int keyword_position, int * allowed_types, int parent);

/**
 * @brief Validates a regular expression against the ECMA 262 spec
 */
int validate_regex(const char * regex);

/**
 * @brief Validates the parent's type of a keyword
 */
int
json_validate_parent_type(char * type,int keyword_position);

/**
 * @brief Validates the type of a keyword
 */
int 
json_validate_type(int type,int keyword_position);

/**
 * @brief Validates the dependencies of a keyword
 */
int check_dependencies(int keyword_position, lh_table* parent);

/**
 * @brief Validates the supplied value of a keyword
 */
int check_allowed_values(int keyword_position,struct lh_entry* keyword, struct lh_table* parent);

/**
 * @brief Validates a keyword, by checking the provided keyword against the rules of the V4 draft. check schema_rules.txt file
 */
int
json_validate_keyword(struct lh_entry* keyword, struct lh_table* parent_type);

/**
 * @brief Validates a schema, by checking the provided schema against the rules of the V4 draft. check schema_rules.txt file
 */
int 
json_validate_schema(const char* filename);