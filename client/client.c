#include "http/http-api.h"

#include "json-c/json_object.h"
#include "json-c/json_tokener.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    struct http serv;
    char *body = NULL;
    int body_size = 0;
    // http_init(&serv, argv[1], atoi(argv[2]));
    // http_get(&serv, argv[3], &body, &body_size);

    // printf("%d\n", body_size);
    // printf("%s\n\n\n", body);

    // json_object *jobj = NULL;
    // jobj = json_tokener_parse(body);

    json_object *final = NULL;
    json_object *jobj = NULL;
    json_object *val = NULL;
    val = json_object_new_string("Jalabiston");
    jobj = json_object_new_array();
    json_object_array_add(jobj, json_object_get(val));
    val = json_object_new_string("Dolbabeb");
    json_object_array_add(jobj, json_object_get(val));

    final = json_object_new_object();
    json_object_object_add(final, "ara", jobj);

    char *string = json_object_to_json_string_ext(final, JSON_C_TO_STRING_PRETTY);
    printf("%s", string);

    json_object_put(final);

    // char post_body[] = "{   \
	// \"username\": \"Vorzakon\", \
	// \"password\": \"123\"       \
    // }";

    // char *response_body = NULL;
    // int response_body_size = 0;

    // http_post(&serv, "/user/login", post_body, sizeof(post_body), &response_body, &response_body_size);
    // printf("%d\n", response_body_size);
    // printf("%s", response_body);
}