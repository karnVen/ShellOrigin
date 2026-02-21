// ai_module.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "ai_module.h"

// --- Memory Interceptor for cURL ---
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    printf("Not enough memory\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

// --- The Actual Command ---
int lsh_suggest(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "karnVen: expected a question after \"suggest\"\n");
    return 1;
  }

  char *api_key = getenv("GEMINI_API_KEY");
  if (api_key == NULL) {
      fprintf(stderr, "karnVen: API Key missing!\n");
      fprintf(stderr, "Please run: export GEMINI_API_KEY=\"your_key\"\n");
      return 1; 
  }

  char prompt[1024] = "";
  for (int i = 1; args[i] != NULL; i++) {
    strcat(prompt, args[i]);
    strcat(prompt, " ");
  }

  printf("🧠 Thinking...\n");

  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if(curl) {
    char url[512];
    snprintf(url, sizeof(url), "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=%s", api_key);

    char payload[2048];
    snprintf(payload, sizeof(payload), "{\"contents\": [{\"parts\":[{\"text\": \"%s\"}]}]}", prompt);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
      fprintf(stderr, "API Call failed: %s\n", curl_easy_strerror(res));
    } else {
      cJSON *json = cJSON_Parse(chunk.memory);
      if (json == NULL) {
          printf("Error parsing JSON response.\n");
      } else {
          cJSON *candidates = cJSON_GetObjectItem(json, "candidates");
          if (candidates != NULL) {
              cJSON *first_candidate = cJSON_GetArrayItem(candidates, 0);
              cJSON *content = cJSON_GetObjectItem(first_candidate, "content");
              cJSON *parts = cJSON_GetObjectItem(content, "parts");
              cJSON *first_part = cJSON_GetArrayItem(parts, 0);
              cJSON *text = cJSON_GetObjectItem(first_part, "text");

              printf("\n🤖 Gemini:\n%s\n", text->valuestring);
          } else {
              printf("Error: Unexpected API response format.\n");
          }
          cJSON_Delete(json);
      }
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(chunk.memory);
  }
  curl_global_cleanup();
  return 1;
}