#include "error_functions.h"

/***
 * Creates new log text file
 *
 * @param const char * log_file_name - log file name
 *
 * @return error code. It can occur because of problems with creating/opening log file.
 */
enum ERR_CODE start_log( const char * log_file_name )
{
  FILE *log_file;

  if((log_file = fopen(LOG_FILE_NAME, "w")) == NULL)
  {
    printf("ERROR in '%s' file:\n \tSomething is wrong with creating/opening log file on line %i\n", __FILE__, __LINE__);
    return ERR_FILE_OPEN;
  }

  fprintf(log_file, "Logging start\n\n");

  fclose(log_file);

  return SUCCESS;
}

/***
 * Adds error information to log file
 *
 * @param const char * log_file_name - log file name
 * @param enum ERR_CODE code - code of error
 * @param const char * source_file_name - source code file name where mistake probably occurred
 * @param int curr_line - line of code, where error occurred
 *
 * @return error code. It can occur because of problems with writing/existence of log file.
 */
enum ERR_CODE add_log( const char * log_file_name, enum ERR_CODE code, const char * source_file_name, int curr_line )
{
  FILE *log_file;

  if((log_file = fopen(LOG_FILE_NAME, "w")) == NULL)
  {
    printf("ERROR in '%s' file:\n \tSomething is wrong with opening log file on line %i\n", source_file_name, curr_line);
    return ERR_FILE_OPEN;
  }

  fprintf(log_file, "Error file/line - %s/%i\n\t", source_file_name, curr_line);

  switch(code)
  {
    case SUCCESS:
      fprintf(log_file, "Either everything is OK and '%s' is mistakenly called,"
                        "or wrong code is sent in 'code' param\n\n",  __func__);

    case ERR_FILE_OPEN:
      fprintf(log_file, "Could not open file\n\n");

    case ERR_FILE_OPERATE:
      fprintf(log_file, "Error while working with file\n\n");

    case ERR_UNEXP_EOF:
      fprintf(log_file, "Unexpected end of file while reading file\n\n");

    case ERR_NULL_PARAM:
      fprintf(log_file, "NULL-pointer is given as function in param\n\n");

    case ERR_STAT:
      fprintf(log_file, "'stat()' function failure error\n\n");

    default:
      fprintf(log_file, "Unknown error code\n\n");
  }

  fclose(log_file);

  return SUCCESS;
}