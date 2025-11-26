/*
 * Word count application with one thread per input file.
 * Supports two modes via compile-time flags:
 * 1. PWORDS_V1: Global Mutex (High Contention)
 * 2. Default:   Local Aggregation (Low Contention)
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"

/* * Cấu trúc tham số truyền vào cho luồng.
 * Nếu là V1: Cần con trỏ tới danh sách toàn cục để ghi trực tiếp.
 * Nếu là V2: Chỉ cần tên file, danh sách tự tạo cục bộ.
 */
typedef struct {
  char* filename;
#ifdef PWORDS_V1
  word_count_list_t* global_list;
#endif
} thread_arg_t;

bool less_count_then_word(const word_count_t* a, const word_count_t* b) {
  if (a->count < b->count) return true;
  if (a->count > b->count) return false;
  return strcmp(a->word, b->word) < 0;
}

static void* thread_word_count(void* arg) {
  thread_arg_t* t_arg = (thread_arg_t*)arg;
  char* filename = t_arg->filename;
  FILE* infile = fopen(filename, "r");

  if (infile == NULL) {
    perror("fopen failed");
    free(t_arg);
    return NULL;
  }

#ifdef PWORDS_V1
  count_words(t_arg->global_list, infile);
  
  fclose(infile);
  free(t_arg);
  return NULL;

#else
  word_count_list_t* local_counts = (word_count_list_t*) malloc(sizeof(word_count_list_t));
  if (local_counts == NULL) { 
      fclose(infile);
      free(t_arg);
      return NULL;
  }
  init_words(local_counts);

  count_words(local_counts, infile);

  fclose(infile);
  free(t_arg);
  
  return (void*) local_counts;
#endif
}

int main(int argc, char* argv[]) {
  word_count_list_t word_counts;
  init_words(&word_counts);

  if (argc <= 1) {
    count_words(&word_counts, stdin);
  } else {
    pthread_t threads[argc - 1];
    
    for (int i = 1; i < argc; i++) {
      thread_arg_t* arg = (thread_arg_t*)malloc(sizeof(thread_arg_t));
      if (arg == NULL) { perror("malloc failed"); exit(1); }
      
      arg->filename = argv[i];
#ifdef PWORDS_V1
      arg->global_list = &word_counts;
#endif

      if (pthread_create(&threads[i - 1], NULL, thread_word_count, arg) != 0) {
        perror("pthread_create failed");
        exit(1);
      }
    }

    for (int i = 0; i < argc - 1; i++) {
#ifdef PWORDS_V1
      pthread_join(threads[i], NULL);
#else
      word_count_list_t* local_result = NULL;
      
      if (pthread_join(threads[i], (void**)&local_result) != 0) {
        perror("pthread_join failed");
      }

      if (local_result != NULL) {
          wordcount_merge(&word_counts, local_result);
          
          free(local_result);
      }
#endif
    }
  }

  wordcount_sort(&word_counts, less_count_then_word);
  fprint_words(&word_counts, stdout);
  return 0;
}