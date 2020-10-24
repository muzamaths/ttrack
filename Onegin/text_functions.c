#include "text_functions.h"

/***
 * Reads strings from file to given buffer and sets
 *
 * @param const char * in_file_name - input file name
 * @param char * buffer - buffer to read text there
 * @param size_t length - length of text to read
 *
 * @return ERR_CODE - error code
 */
enum ERR_CODE read_text_from_file(const char *in_file_name, char *buffer, size_t text_length)
{
  if (in_file_name == NULL || buffer == NULL)
  {
    return ERR_NULL_PARAM;
  }

  FILE *in_file;
  if ((in_file = fopen(in_file_name, "r")) == NULL)
  {
    return ERR_FILE_OPEN;
  }

  if (fread(buffer, sizeof(char), text_length, in_file) < text_length)
  {
    SHORT_READ_CHECK(in_file);
  }

  fclose(in_file);
  return SUCCESS;
}

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
                                 size_t text_length, enum BOOL_TYPE to_append)
{
  if (out_file_name == NULL || buffer == NULL)
  {
    return ERR_NULL_PARAM;
  }

  FILE *out_file;

  if (to_append)
  {
    if ((out_file = fopen(out_file_name, "a")) == NULL)
    {
      return ERR_FILE_OPEN;
    }
  }
  else /* if (!to_append) */
  {
    if ((out_file = fopen(out_file_name, "w")) == NULL) {
      return ERR_FILE_OPEN;
    }
  }

  if (fwrite(buffer, sizeof(char), text_length, out_file) < text_length)
  {
    SHORT_READ_CHECK(out_file);
  }

  fclose(out_file);
  return SUCCESS;
}

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
                                    size_t strings_number, enum BOOL_TYPE to_append)
{
  if (out_file_name == NULL || index == NULL)
  {
    return ERR_NULL_PARAM;
  }

  for (int i = 0; i < strings_number; i++)
  {
    if (index[i].head_ptr == NULL)
    {
      return ERR_NULL_PARAM;
    }
  }

  FILE *out_file;

  if (to_append)
  {
    if ((out_file = fopen(out_file_name, "a")) == NULL)
    {
      return ERR_FILE_OPEN;
    }
  }
  else /* if (!to_append) */
  {
    if ((out_file = fopen(out_file_name, "w")) == NULL) {
      return ERR_FILE_OPEN;
    }
  }

  for (int i = 0; i < strings_number; i++)
  {
    fwrite(index[i].head_ptr, sizeof(char), index[i].size, out_file);
  }

  fclose(out_file);
  return SUCCESS;
}

/***
 * Checks if symbol is ASCII letter: 'a'-'z' or 'A'-'Z'
 *
 * @param const char *sym - checked symbol
 *
 * @return BOOL_TYPE - success flag
 */
enum BOOL_TYPE is_letter(const char *sym)
{
  if (sym == NULL)
    return FALSE;

  if ((*sym >= 'a' && *sym <= 'z') || (*sym >= 'A' && *sym <= 'Z'))
    return TRUE;
  return FALSE;
}

/***
 * Function compares two letters
 *
 * @param const char *sym1 - first comparing letter
 * @param const char *sym2 - second comparing letter
 *
 * @return CMP_TYPE - comparision result
 */
enum CMP_TYPE letter_cmp(const char *sym1, const char *sym2)
{
  int first_sym_pos, second_sym_pos;

  if (*sym1 >= 'a' && *sym1 <= 'z')
  {
    first_sym_pos = *sym1 - 'a' + 1;
  }
  else if (*sym1 >= 'A' && *sym1 <= 'Z')
  {
    first_sym_pos = *sym1 - 'A' + 1;
  }
  else
  {
    return NOT_DEFINED;
  }

  if (*sym2 >= 'a' && *sym2 <= 'z')
  {
    second_sym_pos = *sym2 - 'a' + 1;
  }
  else if (*sym2 >= 'A' && *sym2 <= 'Z')
  {
    second_sym_pos = *sym2 - 'A' + 1;
  }
  else
  {
    return NOT_DEFINED;
  }

  if (first_sym_pos > second_sym_pos)
  {
    return MORE;
  }
  else if (first_sym_pos == second_sym_pos)
  {
    return EQUAL;
  }
  return LESS;
}

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
enum CMP_TYPE string_cmp(const my_string *str1, const my_string *str2)
{
  if (str1 == NULL || str2 == NULL || str1->head_ptr == NULL || str2->head_ptr == NULL)
    return NOT_DEFINED;

  const char *ptr1 = str1->head_ptr,
             *ptr2 = str2->head_ptr;

  while (*ptr1 != '\n' && *ptr2 != '\n' && *ptr1 != '\0' && *ptr2 != '\0')
  {
    if (!is_letter(ptr1))
    {
      ptr1++;
      if (!is_letter(ptr2))
      {
        ptr2++;
      }
      continue;
    }
    if (!is_letter(ptr2))
    {
      ptr2++;
      if (!is_letter(ptr1))
      {
        ptr1++;
      }
      continue;
    }

    enum CMP_TYPE cmp_result = letter_cmp(ptr1, ptr2);

    if (cmp_result == MORE)
    {
      return MORE;
    }
    else if (cmp_result == LESS)
    {
      return LESS;
    }
    else /* if (cmp_result == EQUAL) */
    {
      ptr1++;
      ptr2++;
    }
  }

  if (*ptr1 != '\n' && *ptr1 != '\0')
  {
    return MORE;
  }
  else if (*ptr2 != '\n' && *ptr2 != '\0')
  {
    return LESS;
  }
  return EQUAL;
}

/***
 * comparator function for two strings
 *
 * @param const void *str1 - first string
 * @param const void *str2 - second string
 *
 * @return int - comparision result: -1 - LESS, 0 - EQUAL, 1 - MORE
 */
int string_comparator(const void *str1, const void *str2)
{
  return (int)(string_cmp((my_string *)str1, (my_string *)str2));
}

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
enum CMP_TYPE string_reverse_cmp(const my_string *str1, const my_string *str2)
{
  if (str1 == NULL || str2 == NULL || str1->head_ptr == NULL || str2->head_ptr == NULL)
    return NOT_DEFINED;

  const char *ptr1 = str1->head_ptr + str1->size,
    *ptr2 = str2->head_ptr + str2->size;

  while (ptr1 != str1->head_ptr && ptr2 != str2->head_ptr)
  {
    if (!is_letter(ptr1))
    {
      ptr1--;
      if (!is_letter(ptr2))
      {
        ptr2--;
      }
      continue;
    }
    if (!is_letter(ptr2))
    {
      ptr2--;
      if (!is_letter(ptr1))
      {
        ptr1--;
      }
      continue;
    }

    enum CMP_TYPE cmp_result = letter_cmp(ptr1, ptr2);

    if (cmp_result == MORE)
    {
      return MORE;
    }
    else if (cmp_result == LESS)
    {
      return LESS;
    }
    else /* if (cmp_result == EQUAL) */
    {
      ptr1--;
      ptr2--;
    }
  }

  if (ptr1 != str1->head_ptr)
  {
    return MORE;
  }
  else if (ptr1 != str1->head_ptr)
  {
    return LESS;
  }
  return EQUAL;
}

/***
 * comparator function for two strings to compare them in reverse order
 *
 * @param const void *str1 - first string
 * @param const void *str2 - second string
 *
 * @return int - comparision result: -1 - LESS, 0 - EQUAL, 1 - MORE
 */
int string_reverse_comparator(const void *str1, const void *str2)
{
  return (int)(string_reverse_cmp((my_string *)str1, (my_string *)str2));
}

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
int split_text_to_strings(char *buffer, my_string **index)
{
  if (buffer == NULL)
  {
    return -1;
  }

  char *buf_ptr = buffer;
  int number_of_lines = 0;
  // counting the number of '\n' symbols
  while (*buf_ptr != '\0')
  {
    buf_ptr = strchr(buf_ptr, '\n');
    buf_ptr++;
    number_of_lines++;
  }

  *index = (my_string *)calloc(number_of_lines, sizeof(my_string));
  if (*index == NULL)
  {
    return -1;
  }

  // initializing 'index' with pointers to the starts of strings and their sizes
  int curr_str_num = 0;

  buf_ptr = buffer;
  do
  {
    (*index)[curr_str_num].head_ptr = buf_ptr;                                          // set head of string
    buf_ptr = strchr(buf_ptr, '\n');                                                    // find end of string
    (*index)[curr_str_num].size = (size_t)(buf_ptr - (*index)[curr_str_num].head_ptr);  // find size of string
    buf_ptr++;                                                                          // head of next string or '\0'
    curr_str_num++;
  } while (*buf_ptr != '\0');

  return number_of_lines;
}

/***
 * Allocates memory to read text from file. Function allocates one more byte in case of missing terminating '\0'.
 *
 * @param const char *text_file_name - name of the file containing text
 * @param char *buf_ptr - pointer to the pointer of buffer start
 *
 * @return size_t - the size of allocated memory. Returns '-1' if memory allocation error occurred.
 */
size_t alloc_for_text( const char *text_file_name, char **buf_ptr )
{
  struct stat stat_data;  // structure containing file data

  if (stat(text_file_name, &stat_data) != 0)
  {
    ADD_LOG(ERR_STAT, 2);
    return 0;
  }

  /* Allocating memory for buffer */
  size_t text_length = stat_data.st_size;
  *buf_ptr = (char *)calloc(text_length + 1, 1);          // have not used sizeof(char) here,
                                                          // because 'st_size' is total size in bytes.
  if (*buf_ptr == NULL)
  {
    ADD_LOG(ERR_ALLOC, 4);
    return 0;
  }

  return text_length + 1;
}

/***
 * Reads text from given file, sorts its strings and prints sorted and origin text to the new file
 *
 * @param const char * in_file_name  - input text file name
 * @param const char * out_file_name - output text file name
 *
 * @return enum ERR_CODE - error code.
 */
enum ERR_CODE sort_text(const char * in_file_name, const char * out_file_name)
{
  char *buffer;           // main buffer to save text in it
  my_string *index;       // array of strings with known size
  int err_code;           // initialized by every function call to obtain the return value


  const char *log_file_name = LOG_FILE_NAME;
  err_code = start_log(log_file_name);
  if (err_code != SUCCESS)
  {
    ADD_LOG_WITH_RETURN(err_code, 3);
  }

  if (in_file_name == NULL || out_file_name == NULL)
  {
    ADD_LOG_WITH_RETURN(ERR_NULL_PARAM, 2);
  }

  /* Allocating memory for buffer */
  size_t text_length = alloc_for_text(in_file_name, &buffer) - 1;
  if (text_length == 0)
  {
    ADD_LOG_WITH_RETURN(ERR_FUNC_IMPL, 3);
  }

  /* reading text from file to 'buffer' */
  err_code = read_text_from_file(in_file_name, buffer, text_length);
  if (err_code != SUCCESS)
  {
    free(buffer);
    ADD_LOG_WITH_RETURN(err_code, 4);
  }
  buffer[text_length + 1] = '\0'; //setting terminating '\0' after the last element, which can already be '\0'

  /* splitting text to strings */
  int number_of_lines = split_text_to_strings(buffer, &index);
  if (number_of_lines == -1)
  {
    if (index != NULL)
    {
      free(index);
    }
    free(buffer);
    ADD_LOG_WITH_RETURN(ERR_FUNC_IMPL, 5);
  }

  /* sorting text by strings */
  qsort(index, number_of_lines, sizeof(my_string), string_comparator);
  write_text_by_strings(out_file_name, index, number_of_lines, FALSE); // sorted
  write_text_to_file(out_file_name, "========================\n\n\n\n========================", 4, TRUE);

  /* sorting text by string in reverse order */
  qsort(index, number_of_lines, sizeof(my_string), string_reverse_comparator);
  write_text_by_strings(out_file_name, index, number_of_lines, TRUE);  // reverse sorted
  write_text_to_file(out_file_name, "========================\n\n\n\n========================", 4, TRUE);

  /* writing sorted text and origin text to out_file */
  write_text_to_file(out_file_name, buffer, text_length, TRUE);        // origin

  /* free memory */
  free(index);
  free(buffer);

  return SUCCESS;
}

/***
 * Tests 'text_functions.h' file functions.
 */
void text_functions_tester( void )
{
  FILE *out_file;

  const char *in_file_name = "test_in.2.txt",
             *out_file_name = "test_out.2.txt";

  char *buffer;

  size_t size_of_mem_for_text = alloc_for_text(in_file_name, &buffer); // additional byte is allocated
  size_t text_length = size_of_mem_for_text - 1;

  if (size_of_mem_for_text == 0)
  {
    printf("ERROR. FILE: %s.\n LINE %i : could not allocate data for test text sample\n", __FILE__, __LINE__ - 3);
    return;
  }

  /* Read and write functions test. Only visual check is available at the moment */
  if ((out_file = fopen(out_file_name, "w")) == NULL)
  {
    printf("ERROR: could not open out_file\n");
    return;
  }
  fprintf(out_file, "===============Text read and write functions test section===============\n\n");
  fclose(out_file);

  read_text_from_file(in_file_name, buffer, text_length);
  buffer[text_length] = '\0';

  write_text_to_file(out_file_name, buffer, text_length, TRUE);

  /* splitting and string by string printing functions test */
  if ((out_file = fopen(out_file_name, "a")) == NULL)
  {
    printf("ERROR: could not open out_file\n");
    return;
  }
  fprintf(out_file, "===============Split and by string printing functions test section===============\n\n");
  fclose(out_file);

  my_string *index;

  size_t num_of_lines = split_text_to_strings(buffer, &index);

  if ((out_file = fopen(out_file_name, "a")) == NULL)
  {
    printf("ERROR: could not open out_file\n");
    return;
  }
  for (int i = 0; i < num_of_lines; i += 2)
  {
    fprintf(out_file, "%i : ", i + 1);
    fwrite(index[i].head_ptr, sizeof(char), index[i].size, out_file);
  }
  fclose(out_file);

  /* comparator function test */
  if ((out_file = fopen(out_file_name, "a")) == NULL)
  {
    printf("ERROR: could not open out_file\n");
    return;
  }
  fprintf(out_file, "===============Comparator functions test section===============\n\n");
/*
  for (int i = 0; i < num_of_lines; i++)
  {
    fprintf(out_file, "%i : ", i + 1);
    fwrite(index[i], sizeof(char), strchr(index[i], '\n') - index[i] + 1, out_file);
  }
  fprintf(out_file, "\n");

  for (int i = 0; i < num_of_lines; i++)
    for (int j = i; j < num_of_lines; j++)
    {
      fprintf(out_file, "%i cmp %i = %i\n", i + 1, j + 1, string_comparator((const void *)(index[i]), (const void *)(index[j])));
    }
*/
  fclose(out_file);

  // direct order
  write_text_to_file(out_file_name, "-----direct order-----\n", 1, TRUE);
  qsort(index, num_of_lines, sizeof(my_string), string_comparator);
  write_text_by_strings(out_file_name, index, num_of_lines, TRUE);

  // reverse order
  write_text_to_file(out_file_name, "-----reverse order-----\n", 1, TRUE);
  qsort(index, num_of_lines, sizeof(my_string), string_reverse_comparator);
  write_text_by_strings(out_file_name, index, num_of_lines, TRUE);

  free(buffer);
}