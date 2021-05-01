#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ~~MAX VALUES~~//
#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

// ~~MAGIC NUMBERS~~//
#define ARG_NUM 5
#define BASE 10
#define TRUE 1

// ~~ERRORS AND USAGE MSG~~//
#define USG_ERR "Usage: <seed> < #sentences to generate> "\
      "<path> Optional: < #words to read>"
#define FILE_ERR "Error: invalid file! existing with code 1...\n"
#define ALLOC_ERR "Allocation failure: exiting with code 1...\n"

//~~PRINTING MSG~~//
#define NEW_LINE "\n"
#define TWEET_MSG "Tweet %d: "
#define DELIMITERS " \n\r"


typedef struct WordStruct {
    char *word;
    struct WordProbability *prob_list;
    int prob_list_capacity;
    int prob_list_len;
    int num_of_occurrence;
    int end_of_sentence;
} WordStruct;

typedef struct WordProbability {
    struct WordStruct *word_struct_ptr;
    int num_of_occurrences_after_word;
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct *data;
    struct Node *next;
} Node;

typedef struct LinkList {
    Node *first;
    Node *last;
    int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
    {
      return 1;
    }
  *new_node = (Node){data, NULL};

  if (link_list->first == NULL)
    {
      link_list->first = new_node;
      link_list->last = new_node;
    }
  else
    {
      link_list->last->next = new_node;
      link_list->last = new_node;
    }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
  int rand_num = rand() % max_number;
  return rand_num;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */


WordStruct *get_first_random_word(LinkList *dictionary)
{
  Node *ptr = dictionary->first;
  while(TRUE)
    {
      int x = get_random_number (dictionary->size);
      for (int i = 0; i < x; i++)
        {
          ptr = ptr->next;
        }
      if (ptr->data->end_of_sentence == 0)
        {
          return ptr->data;
        }
      ptr = dictionary->first;
    }

}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{
  int random_number = get_random_number(word_struct_ptr->num_of_occurrence);
  int i=-1;
  while(random_number < word_struct_ptr->num_of_occurrence){
      random_number += word_struct_ptr->prob_list[i+1].num_of_occurrences_after_word;
      i++;
    }
  return word_struct_ptr->prob_list[i].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{
  int i;
  WordStruct *word = get_first_random_word (dictionary);
  for(i=1 ;i<=MAX_WORDS_IN_SENTENCE_GENERATION;i++)
    {
      printf("%s ",word->word);
      word = get_next_random_word(word);
      if (word->end_of_sentence == 1){
          printf("%s",word->word);
          return i;
        }
    }
  return i;
}

WordStruct *find_in_dictionary(LinkList *dict,char *word){
  Node* p = dict->first;
  while(p != NULL){
      if(strcmp (p->data->word,word) == 0){
          return p->data;
        }
      p = p->next;
    }
  return NULL;
}

WordProbability *find_in_probability_list\
(WordStruct *first_word, WordStruct *second_word){
  if (first_word->prob_list_len != 0){
      WordProbability *prob_lst = first_word->prob_list;
      for (int i = 0; i<first_word->prob_list_len; i++){
          if (strcmp (prob_lst[i].word_struct_ptr->word,second_word->word) == 0){
              return &prob_lst[i];
            }
        }
    }
  return NULL;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word, WordStruct *second_word)
{
  WordProbability *ptr = find_in_probability_list (first_word,second_word);
  if (ptr != NULL){
      ptr->num_of_occurrences_after_word++;
      return 0;
    }
  else{
      if (first_word->prob_list_capacity == first_word->prob_list_len){
          first_word->prob_list_capacity++;
          WordProbability *temp = (WordProbability*) \
          realloc(first_word->prob_list, \
          (first_word->prob_list_capacity) * sizeof (WordProbability));
          if(temp == NULL){
              printf(ALLOC_ERR);
              exit(EXIT_FAILURE);
            }
          first_word->prob_list = temp;
        }
      first_word->prob_list[first_word->prob_list_len].word_struct_ptr =\
          second_word;
      first_word->prob_list\
          [first_word->prob_list_len].num_of_occurrences_after_word = 1;
      first_word->prob_list_len++;
      return 1;
    }
}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
WordStruct *alloc_and_init(char* cur_word_str){
  //~~CURRENT WORD STRUCT ALLOCATION~~//
  WordStruct *cur_word_struct = (WordStruct*)malloc(sizeof(WordStruct));
  if (cur_word_struct == NULL)
    {
      printf (ALLOC_ERR);
      exit (EXIT_FAILURE);
    }
  cur_word_struct->word = (char*)malloc((int)strlen(cur_word_str)+1);
  if (cur_word_struct->word == NULL)
    {
      printf(ALLOC_ERR);
      exit (EXIT_FAILURE);
    }
  //~~COPYING CURRENT STRING INTO WORD STRUCT WORD FIELD~~//
  strcpy (cur_word_struct->word, cur_word_str);
  //~~CHECK IF WORD IS END OF SENTENCE~~//
  cur_word_struct->num_of_occurrence = 1;
  if (cur_word_struct->word[(int)strlen (cur_word_struct->word)-1] =='.'){
      cur_word_struct->end_of_sentence = 1;
      cur_word_struct->prob_list = NULL;
    }
  else{
      cur_word_struct->end_of_sentence = 0;
      cur_word_struct->prob_list = (WordProbability *) malloc (sizeof (WordProbability));
      if(cur_word_struct->prob_list == NULL){
          printf(ALLOC_ERR);
          exit (EXIT_FAILURE);
        }
      cur_word_struct->prob_list_capacity = 1;
      cur_word_struct->prob_list_len = 0;
  }
  return cur_word_struct;
}

void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary)
{
  int count = 0;
  char buffer[MAX_SENTENCE_LENGTH], *cur_word_str;
  WordStruct * prev_word = NULL;
  while(fgets(buffer, MAX_SENTENCE_LENGTH, fp) != NULL)
    {
      cur_word_str = strtok (buffer, DELIMITERS);
      while ((cur_word_str != NULL) && \
      ((count != words_to_read) || (words_to_read == -1)))
        {
          WordStruct *cur_word_struct = find_in_dictionary (dictionary, cur_word_str);
          //~~ CHECK IF WORD IN DICTIONARY ~~//
          if (cur_word_struct == NULL)
            {
              cur_word_struct = alloc_and_init (cur_word_str);
              if (add (dictionary, cur_word_struct) == 0)
                {
                  if ((prev_word!=NULL)&&(prev_word->prob_list != NULL))
                    {
                      add_word_to_probability_list (prev_word, cur_word_struct);
                    }
                }
              else{
                printf(ALLOC_ERR);
                exit (EXIT_FAILURE);
              }
            }
          else
            {
              cur_word_struct->num_of_occurrence++;
              if(prev_word->prob_list != NULL){
                  add_word_to_probability_list (prev_word, cur_word_struct);
                }
            }
          //~~ CONTINUE~~//
          prev_word = cur_word_struct;
          count++;
          cur_word_str = strtok (NULL, DELIMITERS);
        }
    }
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary)
{
  Node *first = dictionary->first;
  Node *cur;
  while ((cur =first) != NULL){
      first = first->next;
      free (cur->data->word);
      free(cur->data->prob_list);
      free(cur->data);
      free(cur);
    }
  free(dictionary);
  dictionary = NULL;
}


void print_prob_list(WordStruct *some_word)
{
  printf("[");
  for (int i = 0; i < some_word->prob_list_len; i++)
    {
      printf ("(%s ", some_word->prob_list[i].word_struct_ptr->word);
      printf("#%d)",some_word->prob_list[i].num_of_occurrences_after_word);
    }
  printf("] Length: %d",some_word->prob_list_len);
}

void print_dict(LinkList *dictionary){
  Node* p = dictionary->first;
  while(p != NULL){
      printf("( %s : %d ) -> Probability list: ",\
      p->data->word,p->data->num_of_occurrence);
      if(p->data->prob_list_len != 0){
          print_prob_list (p->data);
        }
      else{
          printf("EMPTY");
        }

      printf("\n");
      p = p->next;
    }
  printf("END\n");
}

int argument_validation(int argc){
  if ((argc != ARG_NUM-1) && (argc!= ARG_NUM)){
      printf(USG_ERR);
      return EXIT_FAILURE;
    }
  return 0;
}

void set_seed(char* seed){
  int int_seed = (int) strtol(seed,NULL,BASE);
  srand(int_seed);
}

LinkList * new_dictionary(){
  LinkList *dictionary = (LinkList *) malloc (sizeof (LinkList));
  *dictionary = (LinkList) {NULL, NULL, 0};
  return dictionary;
}

void print_tweets(int sentences_to_generate,LinkList *dictionary){
  for(int i =1; i<=sentences_to_generate;i++){
      printf(TWEET_MSG,i);
      generate_sentence(dictionary);
      printf(NEW_LINE);
    }
}

FILE * open_tweets_file(char *file_path){
  FILE *fp = fopen (file_path, "r");
  if (fp == NULL)
    {
      printf (FILE_ERR);
      return NULL;
    }
  return fp;
}

void run(FILE *fp,int words_to_read,int sentences_to_generate){
  LinkList *dictionary = new_dictionary();
  fill_dictionary (fp, words_to_read, dictionary);
  print_tweets (sentences_to_generate,dictionary);
  free_dictionary (dictionary);
}
/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */

int main(int argc, char *argv[])
{
  if (argument_validation(argc) == 1){return EXIT_FAILURE;}
  int words_to_read = (argc == ARG_NUM) ? (int) strtol(argv[4],NULL,BASE): -1;
  int sentences_to_generate = (int) strtol(argv[2],NULL,BASE);
  FILE *fp = open_tweets_file (argv[3]);
  if (fp == NULL){return EXIT_FAILURE;}
  set_seed(argv[1]);
  run(fp,words_to_read,sentences_to_generate);
  fclose(fp);
  return EXIT_SUCCESS;
}