#ifndef ONEGIN_ERROR_FUNCITONS_H
#define ONEGIN_ERROR_FUNCTIONS_H

#include <stdio.h>
#include <assert.h>


#define LOG_FILE_NAME "log_out.txt"

#define ADD_LOG(err_type, offset) add_log(LOG_FILE_NAME, err_type, __FILE__, __LINE__ - offset)
#define ADD_LOG_WITH_RETURN(err_type, offset)\
        add_log(LOG_FILE_NAME, err_type, __FILE__, __LINE__ - offset);\
        return err_type


enum ERR_CODE
{
  SUCCESS = 0,        // successful finish of function
  ERR_FILE_OPEN,      // error while opening file
  ERR_FILE_OPERATE,   // error while working with file
  ERR_UNEXP_EOF,      // unexpected end of file while reading file
  ERR_ALLOC,          // memory allocation error
  ERR_NULL_PARAM,     // NULL-pointer is given as param to function
  ERR_STAT,           // 'stat()' function failure error
  ERR_FUNC_IMPL       // function implementation error
};

/***
 * Creates new log text file
 *
 * @param const char * log_file_name - log file name
 *
 * @return error code. It can occur because of problems with creating/opening log file.
 */
enum ERR_CODE start_log( const char * log_file_name );

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
enum ERR_CODE add_log( const char * log_file_name, enum ERR_CODE code, const char * source_file_name, int curr_line );


#endif //ONEGIN_ERROR_FUNCITONS_H
