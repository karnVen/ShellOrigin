#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "ai_module.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "kaiVen: suggest: out of memory\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int lsh_suggest(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "kaiVen: suggest: expected argument\n");
        return 1;
    }

    char *api_key = getenv("GEMINI_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "kaiVen: suggest: GEMINI_API_KEY environment variable missing\n");
        return 1; 
    }

    size_t prompt_len = 0;
    for (int i = 1; args[i] != NULL; i++) {
        prompt_len += strlen(args[i]) + 1;
    }
    
    char *prompt = malloc(prompt_len + 1);
    if (!prompt) {
        fprintf(stderr, "kaiVen: suggest: prompt allocation failed\n");
        return 1;
    }
    prompt[0] = '\0'; 
    
    for (int i = 1; args[i] != NULL; i++) {
        strcat(prompt, args[i]);
        if (args[i+1] != NULL) {
            strcat(prompt, " ");
        }
    }

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=%s", api_key);

        const char *json_template = "{\"contents\": [{\"parts\":[{\"text\": \"%s\"}]}]}";
        size_t payload_len = strlen(json_template) + prompt_len + 1;
        
        char *payload = malloc(payload_len);
        if (!payload) {
            fprintf(stderr, "kaiVen: suggest: payload allocation failed\n");
            free(prompt);
            free(chunk.memory);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }
        snprintf(payload, payload_len, json_template, prompt);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "kaiVen: suggest: API call failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *json = cJSON_Parse(chunk.memory);
            if (json == NULL) {
                fprintf(stderr, "kaiVen: suggest: error parsing JSON response\n");
            } else {
                cJSON *candidates = cJSON_GetObjectItemCaseSensitive(json, "candidates");
                cJSON *first_candidate = cJSON_IsArray(candidates) ? cJSON_GetArrayItem(candidates, 0) : NULL;
                cJSON *content = cJSON_GetObjectItemCaseSensitive(first_candidate, "content");
                cJSON *parts = cJSON_GetObjectItemCaseSensitive(content, "parts");
                cJSON *first_part = cJSON_IsArray(parts) ? cJSON_GetArrayItem(parts, 0) : NULL;
                cJSON *text = cJSON_GetObjectItemCaseSensitive(first_part, "text");

                if (cJSON_IsString(text) && (text->valuestring != NULL)) {
                    /* Strict, silent execution. Just prints the raw answer. */
                    printf("%s\n", text->valuestring);
                } else {
                    fprintf(stderr, "kaiVen: suggest: unexpected API response format\n");
                }
                cJSON_Delete(json);
            }
        }
        
        curl_slist_free_all(headers);
        free(payload);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "kaiVen: suggest: failed to initialize cURL\n");
    }
    
    free(prompt);
    free(chunk.memory);
    curl_global_cleanup();
    
    return 1;
}