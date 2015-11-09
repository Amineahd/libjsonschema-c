#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "schema_validator.h"


void
printf_colored(char* color,char* buff) {
    printf("%s %s \n"ANSI_COLOR_RESET, color, buff);
}
int 
json_validate_numeric_keywords(struct lh_entry* keyword, struct lh_table* parent) {  
    
    char * keyword_name = (char*)keyword->k;
    struct json_object * value = (struct json_object *)keyword->v;
    enum json_type keyword_type = json_object_get_type(value); // type of the keyword value
    
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
    
    
    //multipleOf
    if (strcmp(keyword_name, "multipleOf") == 0) {
        
        //parent must be a number
        if(strcmp(parent_type_value , "integer") != 0 && strcmp(parent_type_value , "number") != 0) {
            char buff[1024]; 
            sprintf(buff,"%s : parent type must be a number.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //verify that the keyword type is a number
        if(keyword_type != json_type_double && keyword_type != json_type_int) {
            printf_colored(ANSI_COLOR_RED ,"multipleOf: object value type must be a number or an integer. ");
            return 0;
        }
        //must be greater than or equal to 0
        if(keyword_type == json_type_int){
            if(json_object_get_int(value) < 0) {
                printf_colored(ANSI_COLOR_RED , "multipleOf: value type must be greater than or equal to 0.");
                return 0;
            }
        }
        if(keyword_type == json_type_double){
            if(json_object_get_double(value) < 0) {
                printf_colored(ANSI_COLOR_RED , "multipleOf: value type must be greater than or equal to 0.");
                return 0;
            }
        }
        //success
        return 1; 

    }
    //exclusiveMaximum
    if (strcmp(keyword_name, "exclusiveMaximum") == 0) {
        //parent must be a number
        if(strcmp(parent_type_value , "integer") != 0 && strcmp(parent_type_value , "number") != 0) {
            char buff[1024]; 
            sprintf(buff,"%s : parent type must be a number.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //verify that maximum is present
        if (lh_table_lookup_entry(parent,"maximum") == NULL ) {
            printf_colored(ANSI_COLOR_RED , "exclusiveMaximum: maximum keyword not found.");
            return 0;
        }
        //verify that the keyword type is boolean
        if(keyword_type != json_type_boolean) {
            printf_colored(ANSI_COLOR_RED , "exclusiveMaximum: value type must be boolean.");
            return 0;
        }
        //success
        return 1;
    }
    
    //exclusiveMinimum
    if (strcmp(keyword_name , "exclusiveMinimum") == 0) {
                //parent must be a number
        if(strcmp(parent_type_value , "integer") != 0 && strcmp(parent_type_value , "number") != 0) {
            char buff[1024]; 
            sprintf(buff,"%s : parent type must be a number.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //verify that minimum is present
        if (lh_table_lookup_entry(parent,"minimum") == NULL ) {
            printf_colored(ANSI_COLOR_RED ,"exclusiveMinimum: minimum keyword not found.");
            return 0;
        }
        //verify that the keyword type is boolean
        if(keyword_type != json_type_boolean) {
            printf_colored(ANSI_COLOR_RED ,"exclusiveMinimum: value type must be boolean.");
            return 0;
        }
        //success
        return 1;
    }
    //minimum or maximum
    if ( strcmp(keyword_name , "maximum") == 0 || strcmp(keyword_name , "minimum") == 0 ) {
                //parent must be a number
        if(strcmp(parent_type_value , "integer") != 0 && strcmp(parent_type_value , "number") != 0) {
            char buff[1024]; 
            sprintf(buff,"%s : parent type must be a number.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be a number
        if(keyword_type != json_type_int && keyword_type != json_type_double) {
            printf_colored(ANSI_COLOR_RED ,"minimum or maximum: value type must be a number.");
            return 0;
        }
        //success
        return 1;
    }
    return 1;
}

int 
json_validate_string_keywords(struct lh_entry* keyword, struct lh_table* parent) { 
    
    char * keyword_name = (char*)keyword->k;
    struct json_object * value = (struct json_object *)keyword->v;
    enum json_type keyword_type = json_object_get_type(value); // type of the keyword value
    
    //lookup the type keyword
    struct lh_entry* parent_type = lh_table_lookup_entry(parent,"type");
    if(parent_type == NULL ) {
        printf_colored(ANSI_COLOR_RED , "---parent type not found");
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

    
    if(strcmp(keyword_name, "maxLength") == 0 || strcmp(keyword_name , "minLength") == 0) {
        //parent must be a string
        if(strcmp(parent_type_value, "string") != 0) {
                char * buff;
                sprintf(buff, "%s parent type must be a string.",keyword_name);
                printf_colored(ANSI_COLOR_RED,buff);
                return 0;
        }
        //must be a number
        if(keyword_type != json_type_int && keyword_type != json_type_double) {
            
            printf_colored(ANSI_COLOR_RED , "maxLength or minLength: value type must be a number.");
            return 0;
        }
        //must be greater than or equal to 0
        if(keyword_type == json_type_int){
            if(json_object_get_int(value) < 0) {
                printf_colored(ANSI_COLOR_RED , "maxLength or minLength: value type must be greater than or equal to 0.");
                return 0;
            }
        }
        if(keyword_type == json_type_double){
            if(json_object_get_double(value) < 0) {
                printf_colored(ANSI_COLOR_RED , "maxLength or minLength: value type must be greater than or equal to 0.");
                return 0;
            }
        }
        //success
        return 1;
    }
    if(strcmp(keyword_name, "pattern") == 0) {
                //parent must be a string
        if(strcmp(parent_type_value, "string") != 0) {
                char * buff;
                sprintf(buff, "%s parent type must be a string.",keyword_name);
                printf_colored(ANSI_COLOR_RED,buff);
                return 0;
        }
        //value must be a string
        if(keyword_type != json_type_string){
            printf_colored(ANSI_COLOR_RED, "pattern: value type must be a string");
        }
        //pattern must conform the ECMA 262
        regex_t pattern;
        memset(&pattern, 0, sizeof(regex_t));
        int err;
        if((err = regcomp(&pattern, json_object_get_string(value),REG_EXTENDED | REG_NOSUB))){
            char buffer[1024];
            regerror(err,&pattern,buffer,sizeof(buffer)/sizeof(buffer[0]));
            char buff[1024];
            sprintf(buff,"pattern: invalid pattern with error: %s ", buffer);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        return 1;
    }
    return 1;
}

int 
json_validate_array_keywords(struct lh_entry* keyword, struct lh_table* parent) {  
    
    char * keyword_name = (char*)keyword->k;
    struct json_object * value = (struct json_object *)keyword->v;
    enum json_type keyword_type = json_object_get_type(value); // type of the keyword value
    
    //lookup the type keyword
    struct lh_entry* parent_type = lh_table_lookup_entry(parent,"type");
    
    if(parent_type == NULL ) {
        printf_colored(ANSI_COLOR_RED , "---parent type not found. ");
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
    

    if(strcmp(keyword_name,"additionalItems") == 0) {
            //parent must be an array
        if(strcmp(parent_type_value, "array") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be an array.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //type must be boolean or an object
        if(keyword_type != json_type_boolean && keyword_type != json_type_object) {
            printf_colored(ANSI_COLOR_RED,"additionalItems: value type must be a boolean or an object.");
            return 0;
        }
        //if an object, must have a valid schema
        if(keyword_type == json_type_object) {
            if(json_validate_object((struct json_object*)keyword->v) != 1) {
                printf_colored(ANSI_COLOR_RED,"additionalItems: value object must have a valid JSON schema.");
                return 0;
            }
        }
        return 1;
    }
    if(strcmp(keyword_name,"items") == 0) {
                    //parent must be an array
        if(strcmp(parent_type_value, "array") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be an array.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //type must be an array or an object
        if(keyword_type != json_type_array && keyword_type != json_type_object) {
            printf_colored(ANSI_COLOR_RED ,"items: value type must be an array or an object.");
            return 0;
        }
        //if an object, must have a valid schema
        if(keyword_type == json_type_object) {
            if(json_validate_object((struct json_object*)keyword->v) != 1) {
                printf_colored(ANSI_COLOR_RED ,"items: value object must have a valid JSON schema.");
                return 0;
            }
        }
        //if an array, items must be objects with valid schema
        if(keyword_type == json_type_array) {
            //validate items are objects with a valid JSON schema
            if(json_validate_array_items((struct json_object*)keyword->v) != 1) {
                printf_colored(ANSI_COLOR_RED ,"items: array items must be objects with a valid JSON schema.");
                return 0;
            }
        }
        //success
        return 1;
    }
    if(strcmp(keyword_name, "maxItems") == 0 || strcmp(keyword_name , "minItems") == 0) {
                    //parent must be an array
        if(strcmp(parent_type_value, "array") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be an array.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be a number
        if(keyword_type != json_type_int && keyword_type != json_type_double) {
            printf_colored(ANSI_COLOR_RED ,"maxItems or minItems: value type must be a number. ");
            return 0;
        }
        //must be greater than or equal to 0
            if(keyword_type == json_type_int ) {
                if(json_object_get_int(value) < 0) {
                    printf_colored(ANSI_COLOR_RED, "maxItems or minItems: value type must be greater than or equal to 0.");
                    return 0;
                }
            }else if(keyword_type == json_type_double ) {
                if(json_object_get_double(value) < 0) {
                    printf_colored(ANSI_COLOR_RED ,"maxItems or minItems: value type must be greater than or equal to 0.");
                    return 0;
                }
            }
        //success
        return 1; 
    }
    if(strcmp(keyword_name, "uniqueItems") == 0){
                    //parent must be an array
        if(strcmp(parent_type_value, "array") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be an array.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be a boolean
        if(keyword_type != json_type_boolean) {
            printf_colored(ANSI_COLOR_RED ,"maxItems or minItems: value type must be a boolean.");
            return 0;
        }
        //success
        return 1;
    }
    return 1;
}

int 
json_validate_object_keywords(struct lh_entry* keyword, struct lh_table* parent) {  
    
    char * keyword_name = (char*)keyword->k;
    struct json_object * value = (struct json_object *)keyword->v;
    enum json_type keyword_type = json_object_get_type(value); // type of the keyword value
    
    //lookup the type keyword
    struct lh_entry* parent_type = lh_table_lookup_entry(parent,"type");
    
    if(parent_type == NULL ) {
        printf_colored(ANSI_COLOR_RED, "---parent type not found");
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
    

    if(strcmp(keyword_name, "maxProperties") == 0 || strcmp(keyword_name , "minProperties") == 0) {
        
            //parent must be an object
        if(strcmp(parent_type_value, "object") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be a object.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be a number
        if(keyword_type != json_type_int && keyword_type != json_type_double) {
            printf_colored(ANSI_COLOR_RED, "maxProperties or minProperties: value type must be a number.");
            return 0;
        }
        //must be greater than or equal to 0
            if(keyword_type == json_type_int){
                if(json_object_get_int(value) < 0) {
                    printf_colored(ANSI_COLOR_RED ,"maxProperties or minProperties: value type must be greater than or equal to 0.");
                    return 0;
                }
            }
            if(keyword_type == json_type_double){
                if(json_object_get_double(value) < 0) {
                    printf_colored(ANSI_COLOR_RED, "maxProperties or minProperties: value type must be greater than or equal to 0.");
                    return 0;
                }
            }
        //success
        return 1;
    }
    
    if(strcmp(keyword_name, "required") == 0){
        
                    //parent must be an object
        if(strcmp(parent_type_value, "object") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be a object.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be an array
        if(keyword_type != json_type_array ) {
            printf_colored(ANSI_COLOR_RED ,"required: value type must be an array.");
            return 0;
        }
        //at least one item must be present
        if(json_object_array_length((struct json_object*)keyword->v) < 1) {
            printf_colored(ANSI_COLOR_RED ,"required: at least one item must be present in the array.");
            return 0;
        }
        //verify uniqueness 
        if(json_validate_array_items_uniqueness((struct json_object*)keyword->v) != 1){
            printf_colored(ANSI_COLOR_RED ,"required: items must be unique \n");
            return 0;
        }
        //success
        return 1;
    }
    if(strcmp(keyword_name,"additionalProperties") == 0) {
                    //parent must be an object
        if(strcmp(parent_type_value, "object") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be a object.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //type must be boolean or an object
        if(keyword_type != json_type_boolean && keyword_type != json_type_object) {
            printf_colored(ANSI_COLOR_RED ,"additionalProperties: value type must be a boolean or an object.");
            return 0;
        }
        //if an object, must have a valid schema
        if(keyword_type == json_type_object) {
            if(json_validate_object((struct json_object*)keyword->v) != 1) {
                printf_colored(ANSI_COLOR_RED ,"additionalProperties: value object must have a valid JSON schema.");
                return 0;
            }
        }
        return 1;
    }
    if(strcmp(keyword_name,"properties") == 0) {
                    //parent must be an object
        if(strcmp(parent_type_value, "object") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be a object.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //type must be an object
        if(keyword_type != json_type_object) {
            printf_colored(ANSI_COLOR_RED ,"properties: value type must be an object.");
            return 0;
        }
        //if an object, must have a valid schema
        if(keyword_type == json_type_object) {
            if(json_validate_object((struct json_object*)keyword->v) != 1) {
                printf_colored(ANSI_COLOR_RED ,"properties: value object must have a valid JSON schema.");
                return 0;
            }
        }
        return 1;
    }
    if(strcmp(keyword_name,"patternProperties") == 0) {
                    //parent must be an object
        if(strcmp(parent_type_value, "object") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be a object.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be an object
        if(keyword_type != json_type_object) {
            printf_colored(ANSI_COLOR_RED ,"patternProperties: value type must be an array or an object.");
            return 0;
        }
        //each property name must be a valid regex 
        struct lh_table* object_table = json_object_get_object((struct json_object*)keyword->v);
        struct lh_entry* entry = object_table->head; 
       
        while(entry != NULL) {
            //pattern must conform the ECMA 262
            regex_t pattern;
            memset(&pattern, 0, sizeof(regex_t));
            int err;
            if((err = regcomp(&pattern, (char *)entry->k,REG_EXTENDED | REG_NOSUB))){
                char buffer[1024];
                regerror(err,&pattern,buffer,sizeof(buffer)/sizeof(buffer[0]));
                char buff[1024];
                sprintf(buff,"patternProperties: invalid pattern for value: %s with error: %s",(char *) entry->k, buffer);
                printf_colored(ANSI_COLOR_RED , buff);
                return 0;
            }
        }
        //pattern values must be valid JSON schema
        if(json_validate_object((struct json_object*)keyword->v) != 1){
            printf_colored(ANSI_COLOR_RED ,"patternProperties: proprties must have valid JSON schema");
        }
        return 1;
    }
    if(strcmp(keyword_name,"dependencies") == 0) {
                    //parent must be an object
        if(strcmp(parent_type_value, "object") != 0) {
            char * buff;
            sprintf(buff,"%s parent type must be a object.",keyword_name);
            printf_colored(ANSI_COLOR_RED,buff);
            return 0;
        }
        //must be an array or an object
        if(keyword_type != json_type_array && keyword_type != json_type_object) {
            printf_colored(ANSI_COLOR_RED ,"required: value type must be an array or an object.");
            return 0;
        }
        //if an object, must have a valid schema
        if(keyword_type == json_type_object) {
            if(json_validate_object((struct json_object*)keyword->v) != 1) {
                printf_colored(ANSI_COLOR_RED ,"properties: value object must have a valid JSON schema.");
                return 0;
            }
        }
        //if an array, at least one item must be present and all items must be unique
        if(keyword_type == json_type_array) {
            //at least one item must be present
            if(json_object_array_length((struct json_object*)keyword->v) < 1) {
                printf_colored(ANSI_COLOR_RED, "required: at least one item must be present in the array.");
                return 0;
            }
            //verify uniqueness 
            if(json_validate_array_items_uniqueness((struct json_object*)keyword->v) != 1){
                printf_colored(ANSI_COLOR_RED ,"required: items must be unique");
                return 0;
            }
        }
        //success
        return 1;
    }
    return 1;
}

int
json_validate_keyword(struct lh_entry* keyword, struct lh_table* parent) {

    char * keyword_name = (char*)keyword->k;
    struct json_object * value = (struct json_object *)keyword->v;
    enum json_type keyword_type = json_object_get_type(value);
    //validate meta-keywords 
    if(strcmp(keyword_name, "title") == 0 || strcmp(keyword_name, "description") == 0) {
        // value type must be string
        if(keyword_type != json_type_string)
        {
            printf_colored(ANSI_COLOR_RED ,"title or description: value type must be string ");
            return 0;
        }
        return 1;
    }
    if(strcmp(keyword_name, "format") == 0) {
        char *patternstr;
        // value type must be string
        if(keyword_type != json_type_string)
        {
            printf_colored(ANSI_COLOR_RED ,"format: value type must be string");
            return 0;
        }
        //value must be one of: date-time ,email,hostname, ipv4,ipv6, uri
        char * values[] = {"date-time" ,"email","hostname", "ipv4","ipv6", "uri"};
        int len = sizeof(values)/ sizeof(values[0]);
        int i;
        for(i=0; i< len; i++) {
            if(strcmp(json_object_get_string(value),values[i]) == 0)
                return 1;
            printf_colored(ANSI_COLOR_RED, "format: value must be one of : date-time ,email,hostname, ipv4,ipv6, uri");
            return 0;
        }
        if(strcmp(json_object_get_string(value),"date-time") == 0){
            // RFC 3339, section 5.6
            patternstr = "^[0-9][0-9][0-9][0-9](-[0-1][0-9](-[0-3][0-9] \
                        (T[0-9][0-9](:[0-9][0-9](:[0-9][0-9])?)?)?)?)?Z?$";
        }
        if(strcmp(json_object_get_string(value),"email") == 0){
            // RFC 5322, section 3.4.1
         /* patternstr = "'/^(?!(?>(?1)\"?(?>\\\[ -~]|[^"])"?(?1)){255,})(?!(?>(?1)\"?(?>\\\ \
            [ -~]|[^ \"]) \"?(?1)){65,}@)((?>(?>(?>((?>(?>(?>\x0D\x0A)?[\t ])+|(?>[\t ]*\x0D\x0A)? \
            [\t ]+)?)(\((?>(?2)(?>[\x01-\x08\x0B\x0C\x0E-\'*-\[\]-\x7F]|\\\[\x00-\x7F]|(?3)))*(?2)\))) \
            +(?2))|(?2))?)([!#-\'*+\/-9=?^-~-]+|\"(?>(?2)(?>[\x01-\x08\x0B\x0C\x0E-!#-\[\]-\x7F]|\\\[\x00-\x7F]))*(?2)\") \
            (?>(?1)\.(?1)(?4))*(?1)@(?!(?1)[a-z\d-]{64,})(?1)(?>([a-z\d](?>[a-z\d-]*[a-z\d])?)(?>(?1)\.(?!(?1)[a-z\d-]{64,}) \
            (?1)(?5)){0,126}|\[(?:(?>IPv6:(?>([a-f\d]{1,4})(?>:(?6)){7}|(?!(?:.*[a-f\d][:\]]){8,})((?6)(?>:(?6)){0,6})?::(?7)?) \
            )|(?>(?>IPv6:(?>(?6)(?>:(?6)){5}:|(?!(?:.*[a-f\d]:){6,})(?8)?::(?>((?6)(?>:(?6)){0,4}):)?))?(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)(?>\.(?9)){3}))\])(?1)$/isD'";*/
        }
        if(strlen(patternstr) > 0) {
            regex_t pattern;
            memset(&pattern, 0, sizeof(regex_t));
            int err;
            if((err = regcomp(&pattern, patternstr,REG_EXTENDED | REG_NOSUB))){
                char buffer[8192];
                regerror(err,&pattern,buffer,sizeof(buffer)/sizeof(buffer[0]));
                char * buff;
                sprintf(buff,"format: invalid pattern for %s with error: %s",json_object_get_string(value),buffer);
                printf_colored(ANSI_COLOR_RED ,buff);
                return 0;
            }
        }
        return 1;
    }
        
    //keywords available for all types
    if(strcmp(keyword_name, "type") == 0 ){
        if(keyword_type == json_type_array){
            if(json_object_array_length(value)>1) {
                printf_colored(ANSI_COLOR_RED,"array type of more than one item is not allowed");
                return 0;
            }
            struct json_object * item_value= json_object_array_get_idx(value,0);
            //item should be a string
            if(json_object_get_type(item_value) != json_type_string){
                printf_colored(ANSI_COLOR_RED,"type: array item should be a string");
                return 0;
            }
        } else if(keyword_type != json_type_string) {
            printf_colored(ANSI_COLOR_RED,"type: value type should be either a string or an array");
            return 0;
        }
        return 1;
    }

    if(strcmp(keyword_name, "enum") == 0){
        //must be an array
        if(keyword_type != json_type_array ) {
            printf_colored(ANSI_COLOR_RED, "enum: value type must be an array.");
            return 0;
        }
        //at least one item must be present
        if(json_object_array_length((struct json_object*)keyword->v) < 1) {
            printf_colored(ANSI_COLOR_RED, "enum: at least one item must be present in the array.");
            return 0;
        }
        //verify uniqueness 
        if(json_validate_array_items_uniqueness((struct json_object*)keyword->v) != 1){
            printf_colored(ANSI_COLOR_RED, "enum: items must be unique");
            return 0;
        }
        return 1;
    }
    if((strcmp(keyword_name, "allOf") == 0 || strcmp(keyword_name, "anyOf") == 0) || strcmp(keyword_name, "oneOf") == 0) {
        // value type must be an array
        if(keyword_type != json_type_array)
        {
            printf_colored(ANSI_COLOR_RED, "allOf, anyOf or oneOf: value type must be an array ");
            return 0;
        }
        //at least one item must be present
        if(json_object_array_length((struct json_object*)keyword->v) < 1) {
            printf_colored(ANSI_COLOR_RED ,"allOf, anyOf or oneOf: at least one item must be present in the array.");
            return 0;
        }
        //validate items are objects with a valid JSON schema
        if(json_validate_array_items((struct json_object*)keyword->v) != 1) {
            printf_colored(ANSI_COLOR_RED, "allOf, anyOf or oneOf: array items must be objects with a valid JSON schema.");
            return 0;
        }
        return 1;
    }
    if(strcmp(keyword_name, "not") == 0 || strcmp(keyword_name, "definitions") == 0) {
        // value type must be an object
        if(keyword_type != json_type_object)
        {
            printf_colored(ANSI_COLOR_RED, "not or definitions: value type must be an object");
            return 0;
        }
        //validate items are objects with a valid JSON schema
        if(json_validate_object((struct json_object*)keyword->v) != 1) {
            printf_colored(ANSI_COLOR_RED, "not or definitions: array items must be objects with a valid JSON schema.");
            return 0;
        }
        return 1;
    }
    //validation for numeric
    return (json_validate_numeric_keywords(keyword,parent) &&
            //validation for string
            json_validate_string_keywords(keyword,parent)  &&
            //validation for arrays
            json_validate_array_keywords(keyword,parent)   &&
            //validation for objects
            json_validate_object_keywords(keyword,parent)
    );
}

void
print_json_value(json_object *jobj) {
    enum json_type type;
    type = json_object_get_type(jobj);
    switch (type) {
        case json_type_boolean: 
            printf("            value: %s \n", json_object_get_boolean(jobj)? "true": "false");
            break;
        case json_type_double:
            printf("            value: %lf \n", json_object_get_double(jobj));
            break;
        case json_type_int:
            printf("            value: %d \n", json_object_get_int(jobj));
            break;
        case json_type_string:
            printf("            value: %s \n", json_object_get_string(jobj));
            break;
        default:
            printf("unhandled type, sorry. \n");
            break;
    }
}

int 
json_validate_object(json_object *jobj) {
    
    struct lh_table* object_table = json_object_get_object(jobj);
    struct lh_entry* entry = object_table->head; 
    enum json_type type;
    static int res;
    res = 1 ;
    while(entry != NULL) {
        
        char * key = (char*)entry->k;
        struct json_object * value = (struct json_object *)entry->v;
        int result;
        
        printf("**key: %s \n", key);
        type = json_object_get_type(value);
        switch (type) {
            case json_type_object: 
                json_validate_object(value);
                break;
            default:
                //validate the keyword
                result = json_validate_keyword(entry,object_table);
                if (result == 1) {
                }else{
                    res = res && 0; // schema is not valid, but keep checking to get all errors.
                    global_res = global_res && 0;
                }
                break;
                
        }
        entry = entry->next;
    }
    return res;
}

static int sort_fn (const void *j1, const void *j2)
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

int 
json_validate_array_items_uniqueness(json_object *jobj){
    int arraylen = json_object_array_length(jobj); /*Getting the length of the array*/
    int i;
    //sort the array
    json_object_array_sort(jobj,sort_fn);
    //verify that every item is unique
    for (i=0; i< arraylen-1; i++){
        json_object * jvalue = json_object_array_get_idx(jobj, i);
        json_object * jvalue1 = json_object_array_get_idx(jobj, i+1);
        if(strcmp(json_object_get_string(jvalue),json_object_get_string(jvalue1)) == 0){
            //not unique
            return 0;
        }
    }
    return 1;
}
int
json_validate_array_items( json_object *jobj) {
    
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
        }else {
            if (json_validate_object(jvalue) != 1) {
                printf("array item #%d must be an object with a valid JSON schema",i);
                return 0;
            }
        }
    }
    return 1;
}

int 
json_validate_schema(char* filename) {

    int res;
    json_object *schema = json_object_from_file(filename);
    
    if(schema == NULL) {
        printf_colored("could not load schema from file.",ANSI_COLOR_RED);
        return 0;
    }
    json_validate_object(schema);
    if(global_res != 1) {
        printf_colored(ANSI_COLOR_RED,"Invalid schema!");
        return 0;
    }else {
        printf_colored(ANSI_COLOR_GREEN,"Valid schema!");
        return 1;
    }
    
}
