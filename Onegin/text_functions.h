#ifndef ONEGIN_TEXT_FUNCTIONS_H
#define ONEGIN_TEXT_FUNCTIONS_H

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "error_functions.h"

/***
 * Checks why the function read less symbols than expected:
 * because of error or unexpected end of file.
 *
 * @param file_ptr - FILE * type pointer to the file.
 */
#define SHORT_READ_CHECK(file_ptr) \
        if (ferror(file_ptr))       \
        {                           \
          fclose(file_ptr);         \
          return ERR_FILE_OPERATE;  \
        }                           \
        if (feof(file_ptr))         \
        {                           \
          fclose(file_ptr);         \
          return ERR_UNEXP_EOF;     \
        }

enum CMP_TYPE
{
  LESS  = -1,
  EQUAL =  0,
  MORE  =  1,
  NOT_DEFINED  =  2
};

enum BOOL_TYPE
{
  FALSE =  0,
  TRUE  =  1
};

/***
 * Basic string structure.
 *
 * @attrib char *head_ptr - pointer to the head of the string
 * @attrib size_t size - size of string
 */
struct my_string
{
  char *head_ptr;
  size_t size;
};

typedef struct my_string my_string;

/***
 * Reads strings from file to given buffer and sets
 *
 * @param const char * in_file_name - input file name
 * @param char * buffer - buffer to read text there
 * @param size_t text_length - length of text to read
 *
 * @return ERR_CODE - error code
 */
enum ERR_CODE read_text_from_file(const char *in_file_name, char *buffer, size_t text_length);

/***
 * Writes text to file from given buffer
 *
 * @param const char *out_file_name - output file name
 * @param const char *buffer - buffer to read text from
 * @param size_t text_length - length of text to write
 * @param enum BOOL_TYPE to_append - append flag:
 *    - if TRUE - function writes text to the end of existing file or creates new one;
 *    - if FALSE - function overwrites existing file or creates new one.
 *
 * @return ERR_CODE - error code
 */
enum ERR_CODE write_text_to_file(const char *out_file_name, const char *buffer,
                                 size_t text_length, enum BOOL_TYPE to_append);

/***
 * Writes text string by string from the array of pointers to the text strings' beginnings.
 *
 * @param const char *out_file_name - output file name
 * @param my_string - array of strings
 * @param size_t strings_number - expected number of strings to write
 * @param enum BOOL_TYPE to_append - append flag:
 *    - if TRUE - function writes text to the end of existing file or creates new one;
 *    - if FALSE - function overwrites existing file or creates new one.
 *
 * @return ERR_CODE - error code
 */
enum ERR_CODE write_text_by_strings(const char *out_file_name, my_string *index,
                                    size_t strings_number, enum BOOL_TYPE to_append);

/***
 * Checks if symbol is ASCII letter: 'a'-'z' or 'A'-'Z'
 *
 * @param const char *sym - checked symbol
 *
 * @return BOOL_TYPE - success flag
 */
enum BOOL_TYPE is_letter(const char *sym);

/***
 * Function compares two letters
 *
 * @param const char *sym1 - first comparing letter
 * @param const char *sym2 - second comparing letter
 *
 * @return CMP_TYPE - comparision result
 */
enum CMP_TYPE letter_cmp(const char *sym1, const char *sym2);

/***
 * Function compares two strings ignoring punctuation.
 * If one string is identical to the other till its end, but the second one is longer,
 * then the first one is considered to be LESS.
 *
 * @param const my_string *str1 - first comparing string
 * @param const my_string *str2 - second comparing string
 *
 * @return CMP_TYPE - comparision result
 */
enum CMP_TYPE string_cmp(const my_string *str1, const my_string *str2);

/***
 * comparator function for two strings
 *
 * @param const void *str1 - first string
 * @param const void *str2 - second string
 *
 * @return int - comparision result: -1 - LESS, 0 - EQUAL, 1 - MORE
 */
int string_comparator(const void *str1, const void *str2);

/***
 * Function compares two strings in reverse order ignoring punctuation.
 * If one string is identical to the other till its beginning, but the second one is longer,
 * then the first one is considered to be LESS.
 *
 * @param const my_string *str1 - first comparing string
 * @param const my_string *str2 - second comparing string
 *
 * @return CMP_TYPE - comparision result
 */
enum CMP_TYPE string_reverse_cmp(const my_string *str1, const my_string *str2);

/***
 * comparator function for two strings to compare them in reverse order
 *
 * @param const void *str1 - first string
 * @param const void *str2 - second string
 *
 * @return int - comparision result: -1 - LESS, 0 - EQUAL, 1 - MORE
 */
int string_reverse_comparator(const void *str1, const void *str2);

/***
 * Splits text to strings and saves the pointers to the beginning of this strings and their sizes.
 * Memory for dynamic array 'index' is allocated in the function.
 *
 * @param char *buffer - splitting text
 * @param my_string **index - pointer on array of pointers to the beginnings of strings
 *
 * @return int - number of strings in text.
 * If error occurred, then -1 is returned.
 */
int split_text_to_strings(char *buffer, my_string **index);

/***
 * Allocates memory to read text from file. Function allocates one more byte in case of missing terminating '\0'.
 *
 * @param const char *text_file_name - name of the file containing text
 * @param char *buf_ptr - pointer to the pointer of buffer start
 *
 * @return size_t - the size of allocated memory. Returns '-1' if memory allocation error occurred.
 */
size_t alloc_for_text( const char *text_file_name, char **buf_ptr );

/***
 * Reads text from given file, sorts its strings and prints sorted and origin text to the new file
 *
 * @param const char * in_file_name  - input text file name
 * @param const char * out_file_name - output text file name
 *
 * @return ERR_CODE - error code.
 */
enum ERR_CODE sort_text(const char * in_file_name, const char * out_file_name);

/***
 * Tests 'text_functions.h' file functions.
 */
void text_functions_tester( void );


#endif //ONEGIN_TEXT_FUNCTIONS_H
