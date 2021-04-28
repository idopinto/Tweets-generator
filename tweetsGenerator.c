#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
int run_tests ();

typedef struct WordStruct {
    char *word; //pointer to word content
    struct WordProbability *prob_list; // dynamic array of WordProbability which contains all possible following word from the text
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

}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{

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
/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
 Node *find_in_dictionary(LinkList *dict,char *word){
  Node* p = dict->first;
  while(p != NULL){
      if(strcmp (p->data->word,word) == 0){
        return p;
      }
      p = p->next;
    }
  return NULL;
 }
WordStruct *find_in_probability_list(WordStruct *first_word, WordStruct *second_word){
  if (first_word->prob_list_len != 0){
    WordProbability *prob_lst = first_word->prob_list;
    for (int i = 0; i<first_word->prob_list_len; i++){
      if (strcmp (prob_lst[i].word_struct_ptr->word,second_word->word) == 0){
        return prob_lst[i].word_struct_ptr;
      }
    }
  }
  return NULL;
}
int add_word_to_probability_list(WordStruct *first_word, WordStruct *second_word)
{
  WordStruct *ptr = find_in_probability_list (first_word,second_word);
  if ((ptr != NULL)&&(ptr->prob_list !=NULL)){
    first_word->prob_list->num_of_occurrences_after_word++;
    return 0;
  }
  else{
    if (first_word->prob_list_capacity == first_word->prob_list_len){
      first_word->prob_list_capacity++;
      WordProbability *temp = (WordProbability*) realloc(first_word->prob_list, (first_word->prob_list_capacity) * sizeof (WordProbability));
      if(temp==NULL){
        free(first_word->prob_list);
        printf("Allocation failure:");
        exit(EXIT_FAILURE);
      }
        first_word->prob_list = temp;

    }
    if (first_word->prob_list != NULL){
      first_word->prob_list[first_word->prob_list_len].word_struct_ptr = second_word;
      first_word->prob_list[first_word->prob_list_len].num_of_occurrences_after_word = 1;
      first_word->prob_list_len++;
    }

    return 1;
  }
}


void print_dict(LinkList *dictionary){
  Node* p = dictionary->first;
  while(p != NULL){
      printf("( %s : %d ) -> Probability list: ",p->data->word,p->data->num_of_occurrence);
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


int word_in_dict_and_update(LinkList *dictionary,char* word){
  Node* p = dictionary->first;
  while(p != NULL){
      if(strcmp (word,p->data->word) == 0)
        {
          p->data->num_of_occurrence ++;
          return 0;
        }// Success
      p = p->next;
    }
  return 1;
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
  cur_word_struct->word = (char*)malloc((int)strlen(cur_word_str) + 1);
  cur_word_struct->prob_list = (WordProbability *) malloc (sizeof (WordProbability));
  //~~VALIDATION CHECK~~//
  if ((cur_word_struct == NULL)||(cur_word_struct->word == NULL)||(cur_word_struct->prob_list == NULL))
    {
      printf ("Allocation failure:");
      // ~~EXISTING WITHOUT FREEING~~//
      exit (EXIT_FAILURE);
    }
  //~~INITIALIZATION~~//
  cur_word_struct->num_of_occurrence = 1;
  cur_word_struct->end_of_sentence = 0;
  cur_word_struct->prob_list_capacity = 1;
  cur_word_struct->prob_list_len = 0;
  //~~COPYING CURRENT STRING INTO WORD STRUCT WORD FIELD~~//
  strcpy (cur_word_struct->word, cur_word_str);
  //~~CHECK IF WORD IS END OF SENTENCE~~//
  if (cur_word_struct->word[(int)strlen (cur_word_struct->word)-1] =='.'){
      cur_word_struct->end_of_sentence = 1;
    }
  return cur_word_struct;

}
void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary)
{
  int count = 0;
  char line[MAX_SENTENCE_LENGTH];
  char *cur_word_str;
  Node* prev_word = dictionary->first;
  while(fgets(line,MAX_SENTENCE_LENGTH,fp)!= NULL){
      cur_word_str = strtok(line, " \n\r");
      while((cur_word_str != NULL) && ((count != words_to_read) || (words_to_read == -1))){
          WordStruct *cur_word_struct =  alloc_and_init (cur_word_str);
          //~~ CHECK IF WORD IN DICTIONARY ~~//
          if (word_in_dict_and_update(dictionary, cur_word_str) == 1)
            {
              //~~FIRST OCCURRENCE OF WORD~~//
              //~~ADD TO THE END OF THE DICTIONARY~~//
              if (add (dictionary, cur_word_struct) == 0)
                {
                  if (prev_word != NULL) // if the previous word is the NULL then the dict is empty.
                    {
                      //~~WORD THAT ENDS WITH '.' SYMBOLIZE END OF SENTENCE~~//
                      //~~THEN WORD'S PROBABILITY LIST POINTS TO NULL~~//
                      if (cur_word_struct->end_of_sentence == 1){
                          cur_word_struct->prob_list = NULL;
                      }
                      //~~ADD WORD TO PROBABILITY LIST WITH THE CURRENT AND THE PREVIOUS WORD STRUCTS~~//
                      //~~0 IF ALREADY IN LIST, 1 OTHERWISE~~//
                      add_word_to_probability_list (prev_word->data, cur_word_struct);
                      //~~LET PREVIOUS WORD BE THE NEXT WORD IN LIST~~//
                      prev_word = prev_word->next;
                    }
                  else // prev word is NULL so put the first node of the list in prev word
                    {
                      prev_word = dictionary->first;
                    }
                }
            }
            //~~THE WORD IS ALREADY IN LIST~~//
          else{
            add_word_to_probability_list (prev_word->data, cur_word_struct);
            prev_word = find_in_dictionary (dictionary,cur_word_struct->word);
          }
          //~~ CONTINUE~~//
          count++;
          cur_word_str = strtok(NULL, " \n\r");
        }
    }
  //~~PRINT THIS SHIT~~//
  print_dict(dictionary);
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary)
{
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
  if ((argc != 4) && (argc!=5)){
    printf("Usage: <seed> < #sentences to generate> <path> Optional: < #words to read>");
    return EXIT_FAILURE;
  }
  int seed, words_to_read;
  //justdoit_tweets.txt
  seed = (int) strtol(argv[1],NULL,10);
  srand(seed); //time(NULL)
  // TODO Read entire file if number of words in text is < number of words to read
  words_to_read = (argc == 5) ? (int) strtol(argv[4],NULL,10): -1;
  FILE *fp = fopen (argv[3], "r");
  if (fp == NULL)
    {
      printf ("Error: invalid file! existing with code 1...\n");
      return EXIT_FAILURE;
    }

  LinkList *dictionary = (LinkList *) malloc (sizeof (LinkList));
  *dictionary = (LinkList) {NULL, NULL, 0};
  fill_dictionary (fp, words_to_read, dictionary);
  return 0;
}