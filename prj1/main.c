#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"
struct bitmap bit_bit[20];
struct hash hashhash[20];
unsigned hhash_func(const struct hash_elem *e, void *aux){
  int num = e->data;
  return hash_int(num);
}
bool hless_func(const struct hash_elem *a, const struct hash_elem *b, void *aux) {
  return a->data < b->data;
}
/* Performs some operation on hash element E, given auxiliary
   data AUX. */
void triple_func (struct hash_elem *e, void *aux) {
  int triple_data = e->data;
  e->data = e->data * triple_data;
  e->data = e->data * triple_data; 
}
void square_func (struct hash_elem *e, void *aux) {
  e->data = e->data * e->data;
}
void clear_func (struct hash_elem *e, void *aux) {
  e->list_elem.prev->next = e->list_elem.next;
  e->list_elem.next->prev = e->list_elem.prev;
}

struct list listlist[20];
bool sort_func(const struct list_elem *a, const struct list_elem *b, void *aux) {
    struct list_item *a_temp = list_entry(a, struct list_item, elem);
    struct list_item *b_temp = list_entry(b, struct list_item, elem);
    return a_temp->data < b_temp->data;
}
bool maxmin_func(const struct list_elem *a, const struct list_elem *b, void *aux) {
  struct list_item *maxmin_temp = list_entry(a, struct list_item, elem);
  struct list_item *maxmin_temp2 = list_entry(b, struct list_item, elem);
  return maxmin_temp->data < maxmin_temp2->data;
}
bool insertordered_func(const struct list_elem *a, const struct list_elem *b, void *aux) {
  struct list_item *insertordered_temp = list_entry(a, struct list_item, elem);
  struct list_item *insertordered_temp2 = list_entry(b, struct list_item, elem);
  return insertordered_temp->data < insertordered_temp2->data;
}
bool unique_func(const struct list_elem *a, const struct list_elem *b, void *aux) {
  struct list_item *unique_temp = list_entry(a, struct list_item, elem);
  struct list_item *unique_temp2 = list_entry(b, struct list_item, elem);
  return unique_temp->data < unique_temp2->data;
}
int main(){
  char input[100];
  while(1){
    fgets(input, sizeof(input), stdin); //한줄씩 받기
    input[strlen(input)-1] = '\0'; //\n제거
    char *word = strtok(input, " "); //단어별로 끊기

//list
    if (strcmp(word, "list_front") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      struct list_elem *front;
      front = list_front(&listlist[num]);
      struct list_item *front_temp = list_entry(front, struct list_item, elem);
      printf("%d\n", front_temp->data);
    }

    else if (strcmp(word, "list_back") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      struct list_elem *back;
      back = list_back(&listlist[num]);
      struct list_item *back_temp = list_entry(back, struct list_item, elem);
      printf("%d\n", back_temp->data);
    }

   else if (strcmp(word, "list_pop_back") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      list_pop_back(&listlist[num]);
    }

    else if (strcmp(word, "list_pop_front") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      list_pop_front(&listlist[num]);
    }

    else if (strcmp(word, "list_push_front") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어: 숫자
      int newnum = atoi(word);
      struct list_item *pushf_new = (struct list_item *)malloc(sizeof(struct list_item));
      pushf_new->data = newnum;
      list_push_front(&listlist[num], &pushf_new->elem);
    }

    else if (strcmp(word, "list_push_back") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어: 숫자
      int newnum = atoi(word);
      struct list_item *pushb_new = (struct list_item *)malloc(sizeof(struct list_item));
      pushb_new->data = newnum;
      list_push_back(&listlist[num], &pushb_new->elem);
    }

    else if (strcmp(word, "list_empty") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      if (list_empty(&listlist[num])) printf("true\n");
      else printf("false\n");
    }

    else if (strcmp(word, "list_size") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      printf("%zu\n", list_size(&listlist[num]));
    }

    else if (strcmp(word, "list_remove") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어: 번쨰
      int newnum = atoi(word);
      struct list_elem *remove_find = listlist[num].head.next;
      for (int i = 0; i < newnum; i++) {
          remove_find = remove_find->next;
      }
      list_remove(remove_find);
    } 

    else if (strcmp(word, "list_reverse") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      list_reverse(&listlist[num]);
    }

    else if (strcmp(word, "list_shuffle") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      list_shuffle(&listlist[num]);
    }
    
    else if (strcmp(word, "list_swap") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어: 몇번째 
      int newnum1 = atoi(word);
      struct list_elem *swap_find1 = listlist[num].head.next;
      for (int i = 0; i < newnum1; i++) {
          swap_find1 = swap_find1->next;
      }
      word = strtok(NULL, " "); //네번째 단어: 몇번쨰
      int newnum2 = atoi(word);
      struct list_elem *swap_find2 = listlist[num].head.next;
      for (int i = 0; i < newnum2; i++) {
          swap_find2 = swap_find2->next;
      }
      list_swap(swap_find1, swap_find2);
    }

    else if (strcmp(word, "list_insert") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어
      int newnum1 = atoi(word);
      word = strtok(NULL, " "); //네번째 단어: 몇을 넣을지
      int newnum2 = atoi(word);
      struct list_elem *insert_find;
      insert_find = listlist[num].head.next;
      for (int i = 0 ; i < newnum1; i++) {
        insert_find = insert_find -> next;
      }
      struct list_item *insert_new = (struct list_item *)malloc(sizeof(struct list_item));
      insert_new->data = newnum2;
      list_insert(insert_find, &insert_new->elem);
    }
    
    else if (strcmp(word, "list_insert_ordered") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어:몇을 넣을지
      int newnum = atoi(word);
      struct list_item *insertordered_new = (struct list_item *)malloc(sizeof(struct list_item));
      insertordered_new->data = newnum;
      list_insert_ordered(&listlist[num], &insertordered_new->elem, insertordered_func, NULL);
    }

    else if (strcmp(word, "list_splice") == 0) {
      word = strtok(NULL, " "); //두번째 단어 ex.list0
      int num1 = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어: 몇번째 2
      int newnum1 = atoi(word);
      struct list_elem *splice_find1 = listlist[num1].head.next;
      for (int i = 0 ; i < newnum1; i++) splice_find1 = splice_find1 ->next;
      word = strtok(NULL, " ");//4번째 단어 ex.list 1
      int num2 = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //5번쨰 단어
      int newnum2 = atoi(word);
      struct list_elem *splice_find2 = listlist[num2].head.next;
      for (int i = 0 ; i < newnum2; i++) splice_find2 = splice_find2 ->next;
      struct list_item *splice_temp1 = (struct list_item *)malloc(sizeof(struct list_item));
      splice_temp1 = list_entry(splice_find2, struct list_item, elem);
      word = strtok(NULL, " "); //6번쨰 단어
      int newnum3 = atoi(word);
      struct list_elem *splice_find3 = listlist[num2].head.next;
      for (int i = 0 ; i < newnum3; i++) splice_find3 = splice_find3 -> next;
      struct list_item *splice_temp2 = (struct list_item *)malloc(sizeof(struct list_item));
      splice_temp2 = list_entry(splice_find3, struct list_item, elem);
      list_splice(splice_find1, &splice_temp1->elem, &splice_temp2->elem);
    } 
      
    else if (strcmp(word, "list_sort") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      list_sort(&listlist[num], sort_func, NULL);
    }

    else if (strcmp(word, "list_max") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      struct list_elem *maxdata = list_max(&listlist[num], maxmin_func, NULL);
      struct list_item *max_temp = list_entry(maxdata, struct list_item, elem);
      printf("%d\n", max_temp -> data);
    }

    else if (strcmp(word, "list_min") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      struct list_elem *mindata = list_min(&listlist[num], maxmin_func, NULL);
      struct list_item *min_temp = list_entry(mindata, struct list_item, elem);
      printf("%d\n", min_temp -> data);
    }

    else if (strcmp(word, "list_unique") == 0) {
      word = strtok(NULL, " "); // 두번째 단어: 리스트 이름
      int num = word[strlen(word) - 1] - '0';
      word = strtok(NULL, " ");
      if (word != NULL) {// duplicate가 있는 경우
        int num2 = word[strlen(word)-1] -'0';
        list_unique(&listlist[num], &listlist[num2] ,unique_func, NULL);
        // 중복된 요소를 출력하는 등의 추가적인 작업 수행 가능
      }
      else {
        list_unique(&listlist[num], NULL, unique_func, NULL);
      }
    }

//hash
    else if (strcmp(word, "hash_insert") == 0) {
      word = strtok(NULL, " ");
      int num = word[strlen(word) -1] - '0';
      word = strtok(NULL, " ");
      int newnum = atoi(word);
      struct hash_elem *insert_temp;
      insert_temp = (struct hash_elem *)malloc(sizeof(struct hash_elem));
      insert_temp -> data = newnum;
      hash_insert(&hashhash[num], insert_temp);
    }

    else if (strcmp(word, "hash_apply") == 0) {
      word = strtok(NULL, " ");
      int num = word[strlen(word) -1] - '0'; //hash0
      word = strtok(NULL, " "); //action
      if (strcmp(word, "triple") == 0) hash_apply(&hashhash[num], triple_func);
      else if (strcmp(word, "square") == 0) hash_apply(&hashhash[num], square_func);
    }

    else if (strcmp(word, "hash_delete") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번쨰 단어: 번쨰
      int newnum = atoi(word);
      struct list_elem *delete_find;
      struct hash_elem *delete_temp;
      int flag = 0;
      for (int i = 0 ; i < hashhash[num].bucket_cnt; i++) {
        struct list *delete_bucket = &hashhash[num].buckets[i];
        for (delete_find = delete_bucket->head.next ; delete_find != &(delete_bucket->tail); delete_find = delete_find -> next) {
          delete_temp = list_elem_to_hash_elem(delete_find);
          if (delete_temp -> data == newnum) {
            flag = 1;
            goto outer_loop_exit;
          }
        }
      }
      outer_loop_exit:
      if (flag) hash_delete(&hashhash[num], delete_temp);
    } 

    else if (strcmp(word, "hash_empty") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      if (hash_empty(&hashhash[num])) printf("true\n");
      else printf("false\n");
    }

    else if (strcmp(word, "hash_size") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      printf("%zu\n", hash_size(&hashhash[num]));
    }

    else if (strcmp(word, "hash_clear") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      hash_clear(&hashhash[num], clear_func);
    }

    else if (strcmp(word, "hash_find") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      int newnum = atoi(word); //찾기
      struct hash_elem *find_temp= (struct hash_elem *)malloc(sizeof(struct hash_elem));
      find_temp ->data = newnum;
      if (hash_find(&hashhash[num], find_temp)) printf("%d\n", newnum);
    } 

    else if (strcmp(word, "hash_replace") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      int newnum = atoi(word); //찾기
      struct hash_elem *replace_temp= (struct hash_elem *)malloc(sizeof(struct hash_elem));
      replace_temp ->data = newnum;
      hash_replace(&hashhash[num], replace_temp);
    }

//bitmap
    else if (strcmp(word, "bitmap_mark") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      size_t newnum = atoi(word); //몇번째
      bitmap_mark(&bit_bit[num], newnum);
    }

    else if (strcmp(word, "bitmap_all") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      size_t all_start = atoi(word); //몇번째
      word = strtok(NULL, " ");
      size_t all_count = atoi(word);
      if (bitmap_all(&bit_bit[num], all_start, all_count)) printf("true\n");
      else printf("false\n");
    }

    else if (strcmp(word, "bitmap_any") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      size_t any_start = atoi(word); //몇번째
      word = strtok(NULL, " ");
      size_t any_count = atoi(word);
      if (bitmap_any(&bit_bit[num], any_start, any_count)) printf("true\n");
      else printf("false\n");
    }

    else if (strcmp(word, "bitmap_contains") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      size_t contains_start = atoi(word); //몇번째
      word = strtok(NULL, " ");
      size_t contains_count = atoi(word);
      word = strtok(NULL, " ");
      bool contains_value = 0;
      if (strcmp(word, "true") ==0) contains_value = 1;
      if (bitmap_contains(&bit_bit[num], contains_start, contains_count, contains_value)) printf("true\n");
      else printf("false\n");
    }

    else if (strcmp(word, "bitmap_count") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      size_t count_start = atoi(word); //몇번째
      word = strtok(NULL, " ");
      size_t count_count = atoi(word);
      word = strtok(NULL, " ");
      bool count_value = 0;
      if (strcmp(word, "true") ==0) count_value = 1;
      printf("%zu\n", bitmap_count(&bit_bit[num], count_start, count_count, count_value));
    }

    else if (strcmp(word, "bitmap_dump") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      bitmap_dump(&bit_bit[num]);
    }

    else if (strcmp(word, "bitmap_set_all") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //세번째 단어
      bool set_all_value = 0;
      if (strcmp(word, "true") ==0) set_all_value = 1;
      bitmap_set_all(&bit_bit[num], set_all_value);
    }

    else if (strcmp(word, "bitmap_set") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: 몇번쨰
      size_t set_num = atoi(word);
      word = strtok(NULL, " "); //4번째 단어
      bool set_value = 0;
      if (strcmp(word, "true") ==0) set_value = 1;
      bitmap_set(&bit_bit[num], set_num, set_value); 
    }

    else if (strcmp(word, "bitmap_set_multiple") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t set_multiple_start = atoi(word);
      word = strtok(NULL, " "); //4번쨰
      size_t set_multiple_count = atoi(word);
      word = strtok(NULL, " "); //5번째 단어
      bool set_multiple_value = 0;
      if (strcmp(word, "true") ==0) set_multiple_value = 1;
      bitmap_set_multiple(&bit_bit[num], set_multiple_start, set_multiple_count, set_multiple_value); 
    }

    else if (strcmp(word, "bitmap_flip") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t set_num = atoi(word);
      bitmap_flip(&bit_bit[num], set_num); 
    }

    else if (strcmp(word, "bitmap_size") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      printf("%zu\n", bit_bit[num].bit_cnt); 
    }

    else if (strcmp(word, "bitmap_none") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t none_start = atoi(word);
      word = strtok(NULL, " ");
      size_t none_count = atoi(word);
      if (bitmap_none(&bit_bit[num], none_start, none_count)) printf("true\n");
      else printf("false\n"); 
    }

    else if (strcmp(word, "bitmap_reset") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t reset_num = atoi(word);
      bitmap_reset(&bit_bit[num], reset_num);
    }

    else if (strcmp(word, "bitmap_scan") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t scan_start = atoi(word);
      word = strtok(NULL, " ");
      size_t scan_count = atoi(word);
      word = strtok(NULL, "");
      bool scan_value = 0;
      if (strcmp(word, "true")==0) scan_value = 1;
      printf("%zu\n", bitmap_scan(&bit_bit[num], scan_start, scan_count, scan_value));
    }

    else if (strcmp(word, "bitmap_scan_and_flip") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t scan_and_flip_start = atoi(word);
      word = strtok(NULL, " ");
      size_t scan_and_flip_count = atoi(word);
      word = strtok(NULL, "");
      bool scan_and_flip_value = 0;
      if (strcmp(word, "true")==0) scan_and_flip_value = 1;
      printf("%zu\n", bitmap_scan_and_flip(&bit_bit[num], scan_and_flip_start, scan_and_flip_count, scan_and_flip_value));
    }

    else if (strcmp(word, "bitmap_test") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: start
      size_t test_num = atoi(word);
      if (bitmap_test(&bit_bit[num], test_num)) printf("true\n");
      else printf("false\n");
    }

    else if (strcmp(word, "bitmap_expand") == 0) {
      word = strtok(NULL, " "); //두번째 단어
      int num = word[strlen(word)-1] - '0';
      word = strtok(NULL, " "); //3번째 단어: 몇개
      int expand_num = atoi(word);
      bit_bit[num] = *bitmap_expand(&bit_bit[num], expand_num);
    }

    else if (strcmp(word, "create") == 0){
	    word = strtok(NULL, " "); //두번째 단어
        if (strcmp(word, "list") == 0) {
            word = strtok(NULL, " "); //list name
            int num = word[strlen(word)-1] - '0';
            list_init(&listlist[num]);
        }
        else if (strcmp(word, "hashtable") == 0) {
          word = strtok(NULL, " "); //hashtable name
          int num = word[strlen(word)-1] - '0';
          hash_init(&hashhash[num], hhash_func, hless_func, NULL);
        }
        else if (strcmp(word, "bitmap") == 0) {
          word = strtok(NULL, " ");
          int num = word[strlen(word)-1] - '0';
          word = strtok(NULL, " ");
          size_t count = atoi(word);
          bit_bit[num] = *bitmap_create(count);
        }
        else printf("wrong command\n");
    }

    else if (strcmp(word, "delete") == 0) {
	      word = strtok(NULL, " ");
        int num = word[strlen(word)-1] - '0';
        if (strncmp(word, "list", 4) == 0) {
          list_init(&listlist[num]);
        }
        else if (strncmp(word, "hash", 4) == 0) {
          hash_init(&hashhash[num], hhash_func, hless_func, NULL);
        }
        else if (strncmp(word, "bm", 2) == 0) {
          bit_bit[num].bit_cnt = 0;
        }
        
        else printf("wrong command\n");
    }

    else if (strcmp(word, "dumpdata") == 0) {
	      word = strtok(NULL, " ");
        int num = word[strlen(word)-1] - '0';
        if(strncmp(word, "list", 4) == 0) {
          if(!list_empty(&listlist[num])) {
            struct list_elem *dumpdata_find;
            for (dumpdata_find = listlist[num].head.next; dumpdata_find != &(listlist[num].tail); dumpdata_find = dumpdata_find -> next) {
              struct list_item *dumpdata_temp = list_entry(dumpdata_find, struct list_item, elem);
              printf("%d ", dumpdata_temp->data);
            }
            printf("\n");
          }
        }
        else if (strncmp(word, "hash", 4) == 0) {
          if(!hash_empty(&hashhash[num])){
            for (int i = 0 ; i < hashhash[num].bucket_cnt; i++) {
              struct list_elem *dumpdata_find;
              struct list *dumpdata_bucket = &hashhash[num].buckets[i];
              for (dumpdata_find = dumpdata_bucket->head.next ; dumpdata_find != &(dumpdata_bucket->tail); dumpdata_find = dumpdata_find -> next) {
                struct hash_elem *dumpdata_temp = list_elem_to_hash_elem(dumpdata_find);
                printf("%d ", dumpdata_temp -> data);
              }
            }
          printf("\n");
          }
        }

        else if (strncmp(word, "bm", 2) == 0) {
          int count = bit_bit[num].bit_cnt;
          if (count) {
            for (int i = 0; i < count; i++) {
              printf("%d", bitmap_test(&bit_bit[num], i));
            }
            printf("\n");
          }
        }
        else printf("wrong command\n");
    }
    if (strcmp(input, "quit") == 0) break;
  }

  return 0;
}
