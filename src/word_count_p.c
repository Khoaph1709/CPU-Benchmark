/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 * Optimized with Local/Global Merge strategy.
 */

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void init_words(word_count_list_t* wclist) {
  list_init(&wclist->lst);
  pthread_mutex_init(&wclist->lock, NULL);
}

size_t len_words(word_count_list_t* wclist) {
  pthread_mutex_lock(&wclist->lock);
  size_t length = list_size(&wclist->lst);
  pthread_mutex_unlock(&wclist->lock);
  return length;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  pthread_mutex_lock(&wclist->lock);
  struct list_elem* e;
  word_count_t* wc_found = NULL;
  
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    word_count_t* wc = list_entry(e, word_count_t, elem);
    if (strcmp(wc->word, word) == 0) {
      wc_found = wc;
      break;
    }
  }
  pthread_mutex_unlock(&wclist->lock);
  return wc_found;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  pthread_mutex_lock(&wclist->lock);

  struct list_elem *e;
  word_count_t *wc = NULL;
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    word_count_t *current_wc = list_entry(e, word_count_t, elem);
    if (strcmp(current_wc->word, word) == 0) {
      wc = current_wc;
      break;
    }
  }

  if (wc != NULL) {
    wc->count++;
    free(word);
  } else {
    wc = (word_count_t*) malloc(sizeof(word_count_t));
    if (wc != NULL) {
        wc->word = strdup(word); // Make a copy
        wc->count = 1;
        if (wc->word != NULL) {
            list_push_back(&wclist->lst, &wc->elem);
        } else {
            free(wc);
            wc = NULL;
        }
    }
    free(word); // Always free the input word
  }
  
  pthread_mutex_unlock(&wclist->lock);
  return wc;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *e;
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    word_count_t *wc = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%8d\t%s\n", wc->count, wc->word);
  }
  pthread_mutex_unlock(&wclist->lock);
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  bool (*less_func)(const word_count_t*, const word_count_t*) = aux;
  word_count_t* wc1 = list_entry(ewc1, word_count_t, elem);
  word_count_t* wc2 = list_entry(ewc2, word_count_t, elem);
  return less_func(wc1, wc2);
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  pthread_mutex_lock(&wclist->lock);
  list_sort(&wclist->lst, less_list, less);
  pthread_mutex_unlock(&wclist->lock);
}

/* * HÀM MỚI: Merge list (Hợp nhất danh sách)
 * Hàm này chỉ khóa mutex của danh sách đích ĐÚNG MỘT LẦN
 * và thực hiện toàn bộ việc hợp nhất.
 */
void wordcount_merge(word_count_list_t* target, word_count_list_t* source) {
  // 1. Khóa danh sách đích (Global List)
  pthread_mutex_lock(&target->lock);

  // Không cần khóa source vì luồng này sở hữu độc quyền source list tại thời điểm này

  while (!list_empty(&source->lst)) {
    // Lấy phần tử đầu tiên ra khỏi danh sách nguồn
    struct list_elem *e = list_pop_front(&source->lst);
    word_count_t *wc_source = list_entry(e, word_count_t, elem);

    // Tìm xem từ này đã có trong danh sách đích chưa
    // Lưu ý: Ta phải tự viết vòng lặp tìm kiếm vì find_word() sẽ cố gắng khóa mutex lần nữa (deadlock)
    word_count_t *wc_target = NULL;
    struct list_elem *te;
    for (te = list_begin(&target->lst); te != list_end(&target->lst); te = list_next(te)) {
       word_count_t *t = list_entry(te, word_count_t, elem);
       if (strcmp(t->word, wc_source->word) == 0) {
         wc_target = t;
         break;
       }
    }

    if (wc_target != NULL) {
       // Trường hợp 1: Từ đã tồn tại trong đích
       // Cộng dồn số lượng
       wc_target->count += wc_source->count;
       
       // Giải phóng phần tử của nguồn (vì dữ liệu đã được cộng vào đích)
       free(wc_source->word);
       free(wc_source);
    } else {
       // Trường hợp 2: Từ chưa có trong đích
       // Chuyển cả node từ nguồn sang đích (Splicing - Rất nhanh)
       list_push_back(&target->lst, &wc_source->elem);
    }
  }

  // 2. Mở khóa danh sách đích
  pthread_mutex_unlock(&target->lock);
}