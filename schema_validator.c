
#include "schema_validator.h"

int 
json_validate_array_items_uniqueness(json_object *jobj,int type){
    
    if(json_object_get_type(jobj) != json_type_array)
    {
        printf_colored(ANSI_COLOR_RED,"type must be array.");
        return 0;
    }
    int arraylen = json_object_array_length(jobj); /*Getting the length of the array*/
    int i;
    //sort the array
    json_object_array_sort(jobj,sort_fn);
    //verify that every item is unique
    for (i=0; i< arraylen-1; i++){
        json_object * jvalue = json_object_array_get_idx(jobj, i);
        json_object * jvalue1 = json_object_array_get_idx(jobj, i+1);
        if(type != -1)
            if(json_object_get_type(jvalue) != type)
                // not valid type
                return 0;
        if(strcmp(json_object_to_json_string(jvalue),json_object_to_json_string(jvalue1)) == 0){
            //not unique
            return 0;
        }
    }
    return 1;
}

int
json_validate_array_items( json_object *jobj) {
    
    if(json_object_get_type(jobj) != json_type_array)
    {
        printf_colored(ANSI_COLOR_RED,"type must be array.");
        return 0;
    }
    enum json_type type;

    int arraylen = json_object_array_length(jobj); /*Getting the length of the array*/
    int i;
    json_object * jvalue;

    //loop through array items
    for (i=0; i< arraylen; i++){
        
        jvalue = json_object_array_get_idx(jobj, i); /*Getting the array element at position i*/
        type = json_object_get_type(jvalue);
        if (type != json_type_object) {
            printf("array item #%d must be an object",i);
            return 0;
        }
    }
    return 1;
}

int 
is_keyword(char* keyword){
    int array_size = sizeof(keywords)/sizeof(keywords[0]);
    int i;
    
    for(i=0 ; i < array_size ; i++){
        if(strcmp(keyword,keywords[i]) == 0){
            return i; // return the position of the keyword
        }
    }
    return -1; // not found
}
void
get_allowed_types(int keyword_position, int * allowed_types, int parent){
    //find the keyword type
    int i=0;
    char *p = keywords_constraints[keyword_position][parent+1];
    while (*p) { // While there are more characters to process...
        if (isdigit(*p)) { // Upon finding a digit, ...
            int val = strtol(p, &p, 10); // Read a number, ...
            allowed_types[i++] = val;
        }else if(*p == '-' || *p == '+') {
            int val = strtol(p, &p, 10); // Read a number, ...
            allowed_types[i++] = val;
        }
        else {
            p++;
        }
    } 
    
}

int validate_regex(const char * regex){
    //pattern must conform the ECMA 262
    regex_t pattern;
    memset(&pattern, 0, sizeof(regex_t));
    int err;
    if((err = regcomp(&pattern, regex,REG_EXTENDED | REG_NOSUB))){
        char buffer[256];
        regerror(err,&pattern,buffer,sizeof(buffer)/sizeof(buffer[0]));
        char buff[256];
        sprintf(buff,"invalid pattern with error: %s", buffer);
        printf_colored(ANSI_COLOR_RED , buff);
        return 0;
    }
    return 1;
}

int
json_validate_parent_type(char * type,int keyword_position){
    //there should be at maximum two types for the parent
    int parent_type[2] = {7,7};
    int i;
    
    get_allowed_types(keyword_position,parent_type,1);
    //if a parent type validation is not required, return directly
    if(parent_type[0] == -1)
        return 1;
    //get parent type id
    for(i=0; i < 7; i++){
        if(strcmp(type,json_types_id[i]) == 0)
            if( i == parent_type[0] || i == parent_type[1])
                return 1;
    }
    return 0;
}

int 
json_validate_type(int type,int keyword_position){
    int allowed_types[7] = {7,7,7,7,7,7,7}; //max 7 types
    int i;
    get_allowed_types(keyword_position,allowed_types,0);
    for(i=0;i<7;i++){
        if(allowed_types[i] == -1)
            return 1;
        if(type == allowed_types[i]) {
            return 1;
        }else if(allowed_types[i] > 6)
            //w've reached the number of types
            break;
    }
    return 0;
}
int check_dependencies(int keyword_position, lh_table* parent){
    int dep = atoi(keywords_constraints[keyword_position][3]); // position of the required keyword
    if(dep > -1){
        char * dep_keyword = keywords_constraints[dep][0] ; // the keyword our keyword depends on
        if (lh_table_lookup_entry(parent,dep_keyword) == NULL ) 
            return 0;
    }
    return 1;
}

int check_allowed_values(int keyword_position,struct lh_entry* keyword, struct lh_table* parent){
    int allowed_values = atoi(keywords_constraints[keyword_position][4]); // get allowed values ID
    struct json_object * value = (struct json_object *)keyword->v;
    const char *key = (char*) keyword->k;
    int i;
    const int array_length = sizeof(values)/sizeof(values[0]);
/*
* allowed values = "-1" => no specific rules, just verify the value is of the required type
*                  "0" => strictly greater than 0
*                  "1" => greater than or equal to 0
*                  "2" => array of objects
*                  "3" => array of unique string items and at least one item is provided
*                  "4" => a valid regex
*                  "5" => name of the object should be a valid regex
*/
    switch(allowed_values) {
        case 0:
            if(json_object_get_int(value) < 1){
                printf("value: %d\n",json_object_get_int(value));
                printf_colored(ANSI_COLOR_RED ,"value must be strictly greater than 0. ");
                return 0;
            }
        break;
        case 1:
            if(json_object_get_int(value) < 0){
                printf("value: %d\n",json_object_get_int(value));
                printf_colored(ANSI_COLOR_RED ,"value must be greater than or equal to 0. ");
                return 0;
            }
        break;
        case 2:
            if(json_object_get_type(value) == json_type_array)
                return json_validate_array_items(value);
            break;
        case 3:
            if(json_object_get_type(value) == json_type_array){
                if(json_object_array_length(value) < 1){
                    printf_colored(ANSI_COLOR_RED,"at least one item must be present in the array");
                    return 0;
                }
                return json_validate_array_items_uniqueness(value,6);
            }
            break;
        case 4:
            if(json_object_get_type(value) == json_type_string)
                return validate_regex(json_object_get_string(value));
            break;
        case 5:
            return validate_regex(key);
            break;
        case 6:
            if(json_object_get_type(value) == json_type_array){
                if(json_object_array_length(value) < 1){
                    printf_colored(ANSI_COLOR_RED,"at least one item must be present in the array");
                    return 0;
                }
                return json_validate_array_items_uniqueness(value,-1); // no specific type is required
            }
            break;
        case 7:
            if(json_object_get_type(value) == json_type_string)
                for(i=0; i<array_length; i++)
                    if(strcmp(json_object_get_string(value),values[i]) == 0)
                        return 1;
            return 0;
            break;
        default:
            return 1 ;
    }
    return 1;
}

int
json_validate_keyword(struct lh_entry* keyword, struct lh_table* parent) {

    char * keyword_name = (char*)keyword->k;
    int keyword_position; // position in the keywords array
    struct json_object * value = (struct json_object *)keyword->v;
    enum json_type keyword_type = json_object_get_type(value);
    
    //check if it is a keyword or not
    if((keyword_position = is_keyword(keyword_name)) != -1)
    {
        //if we don't have a type, return an error
        //lookup the type keyword
        struct lh_entry* parent_type = lh_table_lookup_entry(parent,"type");
        if(parent_type == NULL ) {
            printf_colored(ANSI_COLOR_RED,"---parent type not found" );
            return 0;
        }
        
        struct json_object * parent_type_object = (struct json_object * ) parent_type->v;
        char * parent_type_value;
        //if the type is an array with more than an element, exit with an error.
        if(json_object_get_type(parent_type_object) == json_type_array){
            if(json_object_array_length(parent_type_object)>1) {
                printf_colored(ANSI_COLOR_RED,"array type of more than one item is not allowed");
                return 0;
            }
            struct json_object * item_value= json_object_array_get_idx(parent_type_object,0);
            //item should be a string
            if(json_object_get_type(item_value) != json_type_string){
                printf_colored(ANSI_COLOR_RED,"type: array item should be a string");
                return 0;
            }
            parent_type_value = (char*) json_object_get_string(item_value);
        } else if(json_object_get_type(parent_type_object) == json_type_string) {
            parent_type_value = (char*) json_object_get_string(parent_type_object);
        }else {
            printf_colored(ANSI_COLOR_RED,"type: value type should be either a string or an array");
            return 0;
        }
        
        //check parent type is valid
        if(json_validate_parent_type(parent_type_value,keyword_position) != 1){
            printf_colored(ANSI_COLOR_RED,"Invalid parent type");
            return 0;
        }
        
        //check type is valid
        if(json_validate_type(keyword_type,keyword_position) != 1){
            printf_colored(ANSI_COLOR_RED,"Invalid type");
            return 0;
        }
        if(check_dependencies(keyword_position,parent) != 1){
            printf_colored(ANSI_COLOR_RED,"dependencies missing");
            return 0;
        }
        if(check_allowed_values(keyword_position,keyword,parent) != 1)
        {
            printf_colored(ANSI_COLOR_RED,"bad values");
            return 0;
        }
    }
    return 1;
}

int 
json_validate_object(json_object *jobj,int last_pos) {
    
    struct lh_table* object_table = json_object_get_object(jobj);
    struct lh_entry* entry = object_table->head; 
    enum json_type type;
    static int res = 1;
    int obj_pos = 1 + last_pos;
    while(entry != NULL) {
        
        char * key = (char*)entry->k;
        struct json_object * value = (struct json_object *)entry->v;
        int result;
        
        printf("**key: %*s\n", (int) strlen(key)+(obj_pos*2),key);
        type = json_object_get_type(value);
        //validate the keyword of the object first
        result = json_validate_keyword(entry,object_table);
        if (result != 1) 
            res = res && 0; // schema is not valid, but keep checking to get all errors.
        if(type == json_type_object) 
            json_validate_object(value,obj_pos);

        entry = entry->next;
    }
    return res;
}



int 
json_validate_schema(const char* filename) {

    int res;
    json_object *schema = json_object_from_file(filename);
    
    if(schema == NULL) {
        printf_colored("could not load schema from file.",ANSI_COLOR_RED);
        return 0;
    }
    res = json_validate_object(schema,0);
    if(res != 1) {
        printf_colored(ANSI_COLOR_RED,"Invalid schema!");
        return 0;
    }else {
        printf_colored(ANSI_COLOR_GREEN,"Valid schema!");
        return 1;
    }
    
}
