#ifndef STACKCPP_STACK_FUNCTIONS_H
#define STACKCPP_STACK_FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <utility>

#include "error_functions.h"

/***
 * Stack verification
 *
 * @param case - condition to be verified. If it is 'false' - the error is logged and returned as return value
 * @param ERR_CODE err_code - error code to be returned in case of 'false' value of verified condition
 */
#define STACK_VERIFY_CHECK(case, err_code)	\
	if (case)				\
	{					\
	  ADD_LOG_WITH_RETURN(err_code, 0);	\
	}

/***
 * Stack dump check
 *
 * @param ERR_CODE code - variable to receive stack error value
 *
 * @return ERR_CODE code - error code
 */
#define STACK_DUMP_CHECK(code)	    \
	code = StackOK();	    \
	if (code != SUCCESS)   	    \
	{			    \
 	  StackDump(CURR_LOCATION); \
 	  return code;		    \
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************************
 * Stack canary protection implementation.
 *
 * To activate stack canary protection it is necessary to place the following code before including library:
 * #define STACK_CANARY_PROTECTION
 */

#ifdef STACK_CANARY_PROTECTION
#define ON_STACK_CANARY_PROTECTION(code) code
#else
#define ON_STACK_CANARY_PROTECTION(code)
#endif //STACK_CANARY_PROTECTION

typedef unsigned long long StkCanaryT;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************************
 * Stack hash protection implementation.
 *
 * To activate stack hash protection it is necessary to place the following code before including library:
 * #define STACK_HASH_PROTECTION
 */

#ifdef STACK_HASH_PROTECTION
#define ON_STACK_HASH_PROTECTION(code) code
#else
#define ON_STACK_HASH_PROTECTION(code)
#endif //STACK_HASH_PROTECTION

typedef unsigned long long HashT;
typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Contains information about some location in the program
 *
 * @attrib std::string func_name - function name, where variable was created
 * @attrib std::string file_name - file name, where variable was created
 * @attrib std::string file_line - line in the file, where variable was created
 */
struct LocationInfo
{
public:
  std::string func_name;
  std::string file_name;
  int file_line = -1;

  LocationInfo()
  = default;

  LocationInfo(std::string new_func_name, std::string new_file_name, int new_file_line) :
  	func_name(std::move(new_func_name)), file_name(std::move(new_file_name)), file_line(new_file_line)
  {
  }

  LocationInfo(LocationInfo const &new_loc)
  = default;
};

/***
 *
 */
#define CURR_LOCATION LocationInfo(__FUNCTION__, __FILE__, __LINE__)


/***
 * Contains information about variable in the program
 *
 * @attrib std::string var_name - variable name
 * @attribstd::string func_name	- function name, where variable was created
 * @std::string file_name	- file name, where variable was created
 * @std::string file_line	- line in the file, where variable was created
 */
struct VarInfo : public LocationInfo
{
public:
  std::string var_name;

  VarInfo()
  = default;

  VarInfo(std::string new_var_name, const LocationInfo& new_var_location) :
  	var_name(std::move(new_var_name))
  {
    func_name = new_var_location.func_name;
    file_name = new_var_location.file_name;
    file_line = new_var_location.file_line;
  }

  VarInfo(std::string new_var_name, std::string new_func_name, std::string new_file_name, int new_file_line) :
    var_name(std::move(new_var_name))
  {
    func_name = std::move(new_func_name);
    file_name = std::move(new_file_name);
    file_line = new_file_line;
  }
};


/***
 * Stack template class
 *
 * @attrib size_t size     - the number of elements
 * @attrib size_t capacity - the number of elements that can be held in currently allocated storage
 * @attrib StkElem *data   - pointer to the buffer with stack data
 *
 * @attrib[D] struct VarInfo debug_info - debug information about stack
 * @attrib
 */
template <typename StkElemT = int>
class StackT
{
private:
  ON_STACK_CANARY_PROTECTION(StkCanaryT canaryFront;)

  size_t size = 0;
  size_t capacity = 0;

  StkElemT *data = nullptr;

  VarInfo debug_info;

  ON_STACK_HASH_PROTECTION(HashT hash = 0;)

  ON_STACK_CANARY_PROTECTION(StkCanaryT canaryBack;)

  /***
   * Allocates memory for stack data buffer
   *
   * @param size_t Capacity - the size of buffer which is asked by user
   *
   * @return ERR_CODE - error code
   */
  ERR_CODE MemAlloc(size_t new_capacity)
  {
    if (capacity != 0)
    {
      return ERR_EXCESS_ALLOC;
    }

    // if stack protection is activated it is necessary to allocate memory for two canaries in the stack buffer
    data = (StkElemT *)malloc(new_capacity * sizeof(StkElemT)
			      ON_STACK_CANARY_PROTECTION(+ sizeof(StkCanaryT) * 2) );

    if (data == nullptr)
    {
      return ERR_ALLOC;
    }
    capacity = new_capacity;

    memset(data, '0', new_capacity * sizeof(StkElemT)
		      ON_STACK_CANARY_PROTECTION(+ sizeof(StkCanaryT) * 2) );

    //setting canaries to the head and end of stack buffer
    ON_STACK_CANARY_PROTECTION(
      *((StkCanaryT *)data) = 0xDEADBEEFBADF00D;
      *((StkCanaryT *)((char *)data + sizeof(StkCanaryT) + sizeof(StkElemT) * capacity)) = 0xDEADBEEFBAD00D;
      data = (StkElemT *)((char *)data + sizeof(StkCanaryT));
    )

    //calculating hash function
    ON_STACK_HASH_PROTECTION(
      ReCalcHash();
      )

    return SUCCESS;
  }

public:
  /***
   * Default stack constructor
   */
  StackT(VarInfo var_info = {})
  {
    debug_info = std::move(var_info);

    enum ERR_CODE code = MemAlloc(1);
    if (code != SUCCESS)
    {
      ADD_LOG(code, 3);
    }

    ON_STACK_CANARY_PROTECTION(canaryFront = 0xDEADBEEFBADF00D;)
    ON_STACK_CANARY_PROTECTION(canaryBack = 0xDEADBEEFBADF00D;)
  }

  /***
   * Stack constructor with given capacity
   *
   * @param size_t new_capacity - capacity of stack
   */
  StackT(size_t new_capacity, VarInfo var_info = {}) : debug_info(std::move(var_info))
  {
    enum ERR_CODE code = MemAlloc(new_capacity);
    if (code != SUCCESS)
    {
      ADD_LOG(code, 3);
    }

    ON_STACK_CANARY_PROTECTION(canaryFront = 0xDEADBEEFBADF00D;)
    ON_STACK_CANARY_PROTECTION(canaryBack = 0xDEADBEEFBADF00D;)
  }

  /***
   * Reallocates memory for stack data buffer and clears the cut part of stack if new capacity is less than current
   *
   * @param size_t new_capacity  - new stack data buffer capacity
   *
   * @return ERR_CODE - error code.
   *
   * @note If 'new_capacity' is less than 'size', then 'size - new_capacity' elements are set to zero.
   *       Even if 'realloc' fails data will be available and elements will be set to zero.
   */
  ERR_CODE MemRealloc(size_t new_capacity)
  {
    ERR_CODE code;
    STACK_DUMP_CHECK(code)

    if (new_capacity < size)
    {
      memset(&(this->data[new_capacity]), '0', sizeof(StkElemT) * (this->size - new_capacity));
      this->size = new_capacity;
    }

    ON_STACK_CANARY_PROTECTION(data = (StkElemT *)((char *)data - sizeof(StkCanaryT));)

    auto ptr = (StkElemT *)realloc((void *)this->data, new_capacity * sizeof(StkElemT)
                                                            ON_STACK_CANARY_PROTECTION(+ sizeof(StkCanaryT) * 2) );

    if (ptr == nullptr)
    {
      ADD_LOG_WITH_RETURN(ERR_ALLOC, 3);
    }
    this->data = ptr;
    this->capacity = new_capacity;

    //setting canaries to the head and end of stack buffer
    ON_STACK_CANARY_PROTECTION(
      *((StkCanaryT *)data) = 0xDEADBEEFBADF00D;
      *((StkCanaryT *)((char *)data + sizeof(StkCanaryT) + sizeof(StkElemT) * capacity)) = 0xDEADBEEFBADF00D;
      data = (StkElemT *)((char *)data + sizeof(StkCanaryT));
    )

    //calculating hash function
    ON_STACK_HASH_PROTECTION(
      ReCalcHash();
    )

    STACK_DUMP_CHECK(code)

    return SUCCESS;
  }

  /***
   * Checks if all stack parameters have correct value and stack protection is not harmed
   *
   * @param StackT *stk - pointer to the stack to check
   *
   * @return ERR_CODE - error code
   */
  ERR_CODE StackOK( )
  {
    STACK_VERIFY_CHECK(data == nullptr, ERR_NULL_ATTRIB);
    STACK_VERIFY_CHECK(size > capacity, ERR_OVERFLOW);
    STACK_VERIFY_CHECK(size < 0,        ERR_UNDERFLOW);
    ON_STACK_CANARY_PROTECTION(
      STACK_VERIFY_CHECK(canaryFront != 0xDEADBEEFBADF00D, ERR_FRONT_CANARY);
      STACK_VERIFY_CHECK(canaryBack != 0xDEADBEEFBADF00D,  ERR_FRONT_CANARY);
      STACK_VERIFY_CHECK(*((StkCanaryT *)((char *)data - sizeof(StkCanaryT))) != 0xDEADBEEFBADF00D, ERR_FRONT_CANARY);
      STACK_VERIFY_CHECK(*((StkCanaryT *)((char *)data - sizeof(StkCanaryT))) != 0xDEADBEEFBADF00D, ERR_FRONT_CANARY);
    )
    ON_STACK_HASH_PROTECTION(
      STACK_VERIFY_CHECK(ReCalcHash() != hash, ERR_HASH_BREAK);
    )

    return SUCCESS;
  }

  /***
   * Adds element to the stack
   *
   * @param StkElemT value - adding element
   *
   * @return ERR_CODE - error code
   */
  ERR_CODE Push(StkElemT value)
  {
    ERR_CODE code;

    STACK_DUMP_CHECK(code)

    if (size == capacity)
    {
      code = MemRealloc(capacity * 2);
      if (code != SUCCESS)
      {
        ADD_LOG_WITH_RETURN(code, 3);
      }
    }

    data[size] = value;
    size++;

    ON_STACK_HASH_PROTECTION(ResetHash();)

    STACK_DUMP_CHECK(code)

    return SUCCESS;
  }

  /***
   * Deletes element from the stack and returns it to the pointer given as a parameter
   *
   * @param StkElemT *receiver - pointer to the variable to receive the getting stack element
   *
   * @return ERR_CODE - error code
   */
  ERR_CODE Pop(StkElemT *receiver)
  {
    ERR_CODE code;

    STACK_DUMP_CHECK(code)

    if (size == 0)
    {
      return FAILURE;
    }

    *receiver = data[size - 1];
    data[size - 1] = 0;
    size--;

    ON_STACK_HASH_PROTECTION(ResetHash();)

    STACK_DUMP_CHECK(code)

    return SUCCESS;
  }

  /***
   * Deletes all stack elements
   */
  ERR_CODE Clear()
  {
    ERR_CODE code;
    STACK_DUMP_CHECK(code)

    memset(&(data[0]), '0', sizeof(StkElemT) * size);
    size = 0;

    ON_STACK_HASH_PROTECTION(ResetHash();)

    STACK_DUMP_CHECK(code)

    return SUCCESS;
  }

  /***
   * Stack destructor
   */
  ~StackT()
  {
    if (!StackOK())
    {
      StackDump(CURR_LOCATION, "stack_destr_dump.txt");
    }

    ON_STACK_CANARY_PROTECTION(
    data = (StkElemT *)((char *)data - sizeof(StkCanaryT));
    )
    memset(&(data[0]), '0', sizeof(StkElemT) * capacity
    			     ON_STACK_CANARY_PROTECTION(+ 2 * sizeof(StkCanaryT)) );
    free(data);
    size = 0;
    capacity = 0;
  }

  /***
   * Prints all elements of stack
   *
   * @param std::ostream &os - reference to the output stream
   */
  ERR_CODE StackPrint(std::ostream &os)
  {
    ERR_CODE code;
    STACK_DUMP_CHECK(code)

    for (int i = 0; i < size; i++)
    {
      os << "[" << i << "]" << " : " << data[i] << std::endl;
    }

    return SUCCESS;
  }

  /***
   * Writes stack diagnostic information to the file
   *
   * std::string out_file_name - name of the file to write the diagnostic information
   */
  void StackDump(const LocationInfo& call_loc, const std::string &out_file_name = "stack_dump_file.txt")
  {
    std::ofstream os(out_file_name);

    if (!os.is_open())
    {
      assert(!"Dump function could not open file to write information there");
    }

    char buf[5000];
    sprintf(buf,
    "Stack name : %s\n"
    "Stack variable location in the program : \n"
    "	-function name : %s\n"
    "	-file name : %s\n"
    "	-file line : %i\n"
    "\n", debug_info.var_name.c_str(), debug_info.func_name.c_str(), debug_info.file_name.c_str(), debug_info.file_line);
    os << buf;

    sprintf(buf,
    "Called from : \n"
    "	-function name : %s\n"
    "	-file name : %s\n"
    "	-file line : %i\n"
    "\n", call_loc.func_name.c_str(), call_loc.file_name.c_str(), call_loc.file_line);
    os << buf;

    sprintf(buf,
    "%s\n"
    "{\n", debug_info.var_name.c_str());
    os << buf;

    ON_STACK_CANARY_PROTECTION(os << "canaryFront - " << canaryFront << std::endl;)
    ON_STACK_HASH_PROTECTION(os << "Hash - " << hash << std::endl;)

    sprintf(buf,
    "  size - %zu\n"
    "  capacity - %zu\n"
    "  data[%p]\n"
    "    {\n", size, capacity, data);
    os << buf;

    ON_STACK_CANARY_PROTECTION(os << "Buffer canaryFront - " << *(StkCanaryT *)((char *)data - sizeof(StkCanaryT)) << std::endl;)

    for (int i = 0; i < size; i++)
    {
      os << "      *[" << i << "] = " << data[i] << std::endl;
    }

    for (int i = size; i < capacity; i++)
    {
      os << "      [" << i << "] = " << data[i] << std::endl;
    }

    ON_STACK_CANARY_PROTECTION(os << "Buffer canaryBack - " << *(StkCanaryT *)((char *)data + sizeof(StkElemT) * capacity) << std::endl;)

    os << "    }\n";
    ON_STACK_CANARY_PROTECTION(os << "canaryBack - " << canaryBack << std::endl;)
    os << "}\n";
  }

  #ifdef STACK_HASH_PROTECTION
  /***
   * Calculates hash for stack buffer
   */
private:

  /***
 * Makes bitwise shift of value to the left (RotateLeft) and the leading bit goes to the end and not lost
 *
 * @param HashT value - value to be rotated
 *
 * @return HashT - rotated value
 */
  static HashT RoL(HashT value)
  {
    return (value << 1u) | (value >> (8u * sizeof(value) - 1u));
  }

  /***
 * Calculates buffer hash
 *
 * @param void *buffer - buffer to calculate hash
 * @param size_t size  - size of buffer
 *
 * @return HashT - calculated hash value
 */
  static HashT Hash(void *buffer, const size_t &buf_size)
  {
    auto buf_ptr = (uchar *)buffer;
    HashT value = 0;

    for (size_t i = 0; i < buf_size * sizeof(StkElemT); i++)
    {
      value = RoL(value) ^ buf_ptr[i];
    }

    return value;
  }

  HashT ReCalcHash( )
  {
    return Hash((void *)data, size);
  }

  void ResetHash( )
  {
    hash = ReCalcHash();
  }
  #endif // STACK_HASH_PROTECTION

  /************************
   * List of friend functions.
   * Used to get the private attributes of stack while testing
   */
  friend void StackTester();
};

/***
   * Tests 'text_functions.h' file functions.
   */
void StackTester( )
{
  ERR_CODE code1;
  ERR_CODE code2;

  std::cout << "Stack tester\n";
  std::cout << "Note: estimated values will be print in the braces after the real value\n\n";
  std::cout << "Stack list:\n" << "StackT<int> stk1 = { }\n" << "StackT<double> stk2(200)\n" << std::endl;

  std::cout << "///////////////////// Constructor and memory allocation/reallocation /////////////////////\n";

  StackT<int> stk1 = {VarInfo("stk1", CURR_LOCATION)};
  StackT<double> stk2(200, VarInfo("stk2", CURR_LOCATION));

  std::cout << "Stack variables creation :" << std::endl;
  std::cout << "stk1.size = " << stk1.size << " (0)" << std::endl;
  std::cout << "stk2.size = " << stk2.size << " (0)" << std::endl;
  std::cout << "stk1.capacity = " << stk1.capacity << " (1)" << std::endl;
  std::cout << "stk2.capacity = " << stk2.capacity << " (200)" << std::endl;
  std::cout << std::endl;

  std::cout << "Reallocation :" << std::endl;
  stk1.MemRealloc(1000);
  stk2.MemRealloc(5);
  std::cout << "stk1.size = " << stk1.size << " (0)" << std::endl;
  std::cout << "stk2.size = " << stk2.size << " (0)" << std::endl;
  std::cout << "stk1.capacity = " << stk1.capacity << " (1000)" << std::endl;
  std::cout << "stk2.capacity = " << stk2.capacity << " (5)" << std::endl;
  std::cout << std::endl;

  code1 = stk1.StackOK();
  code2 = stk2.StackOK();

  std::cout << "Error code for stk1:";
  print_err(std::cout, code1);
  std::cout << "Error code for stk2:";
  print_err(std::cout, code2);

  std::cout << "///////////////////// Push /////////////////////\n";

  for (int i = 0; i < 10; i++)
  {
    stk1.Push(i);
  }

  for (int i = 0; i < 15; i++)
  {
    stk2.Push(i);
  }

  std::cout << "stk1 : " << std::endl;
  std::cout << "size - " << stk1.size << " (10) " << std::endl;
  std::cout << "capacity - " << stk1.capacity << " (1000) " << std::endl;
  std:: cout << "data - numbers from 0 to 9:" << std::endl;
  stk1.StackPrint(std::cout);
  std::cout << std::endl;

  std::cout << "stk2 : " << std::endl;
  std::cout << "size - " << stk2.size << " (15) " << std::endl;
  std::cout << "capacity - " << stk2.capacity << " (20) " << std::endl;
  std:: cout << "data - numbers from 0 to 14:" << std::endl;
  stk2.StackPrint(std::cout);
  std::cout << std::endl;

  code1 = stk1.StackOK();
  code2 = stk2.StackOK();

  std::cout << "Error code for stk1 : ";
  print_err(std::cout, code1);
  std::cout << "Error code for stk2 : ";
  print_err(std::cout, code2);
  std::cout << std::endl;

  std::cout << "///////////////////// Dump /////////////////////\n";
  std::string stk1_dump_file_name("stk1_dump.txt"), stk2_dump_file_name("stk2_dump.txt");

  std::cout << "Check file '" << stk1_dump_file_name << "' to watch 'stk1' diagnostic data" << std::endl;
  std::cout << "Check file '" << stk2_dump_file_name << "' to watch 'stk2' diagnostic data" << std::endl;

  stk1.StackDump(CURR_LOCATION, stk1_dump_file_name);
  stk2.StackDump(CURR_LOCATION, stk2_dump_file_name);

  std::cout << std::endl;

  std::cout << "///////////////////// Hash protection check /////////////////////\n";
  #ifndef STACK_HASH_PROTECTION
  #define STACK_HASH_PROTECTION
  #endif //STACK_HASH_PROTECTION

  std::cout << "Hash of 'stk1' - " << stk1.hash << " (39093746760)" << std::endl;
  std::cout << "Hash of 'stk2' - " << stk2.hash << " (8647433863363066984)" << std::endl;

  stk1.data[stk1.size / 2] = 0;
  stk2.data[stk2.size / 2] = 0;

  std::cout << "Hash after changing one element of 'stk1' - " << stk1.ReCalcHash() << " (39093746760)" << std::endl;
  std::cout << "Hash after changing one element of 'stk2' - " << stk2.ReCalcHash() << " (8647433863363066984)" << std::endl;

  code1 = stk1.StackOK();
  code2 = stk2.StackOK();

  std::cout << "Error code for stk1 : ";
  print_err(std::cout, code1);
  std::cout << "Error code for stk2 : ";
  print_err(std::cout, code2);

  std::cout << std::endl;

  std::cout << "///////////////////// Pop /////////////////////\n";

  int tmp1;
  double tmp2;

  std::cout << "stk1 : " << std::endl;
  std::cout << "Popping data - numbers from 9 to 4 :" << std::endl;
  for (int i = 0; i < 6; i++)
  {
    code1 = stk1.Pop(&tmp1);
    std::cout << "[" << stk1.size << "]" << " : " << tmp1 << ". Success flag : ";
    print_err(std::cout, code1) ;
  }

  std::cout << "stk2 : " << std::endl;
  std::cout << "Popping data - numbers from 14 to 0, then 0 is repeated 5 times, "
	       "with 'failure' flag as stk2 is already empty :" << std::endl;
  for (int i = 0; i < 20; i++)
  {
    code2 = stk2.Pop(&tmp2);
    std::cout << "[" << stk2.size << "]" << " : " << tmp2 << ". Success flag : ";
    print_err(std::cout, code2) ;
  }

  std::cout << "stk1 : " << std::endl;
  std::cout << "size - " << stk1.size << " (4) " << std::endl;
  std::cout << "capacity - " << stk1.capacity << " (1000) " << std::endl;
  std:: cout << "data - numbers from 0 to 3:" << std::endl;
  stk1.StackPrint(std::cout);
  std::cout << std::endl;

  std::cout << "stk2 : " << std::endl;
  std::cout << "size - " << stk2.size << " (0) " << std::endl;
  std::cout << "capacity - " << stk2.capacity << " (20) " << std::endl;
  std:: cout << "data - none:" << std::endl;
  stk2.StackPrint(std::cout);
  std::cout << std::endl;

  code1 = stk1.StackOK();
  code2 = stk2.StackOK();

  std::cout << "Error code for stk1 : ";
  print_err(std::cout, code1);
  std::cout << "Error code for stk2 : ";
  print_err(std::cout, code2);
  std::cout << std::endl;

  std::cout << "///////////////////// Clear /////////////////////\n";
  stk1.Clear();

  std::cout << "stk1 : " << std::endl;
  std::cout << "size - " << stk1.size << " (0) " << std::endl;
  std::cout << "capacity - " << stk1.capacity << " (1000) " << std::endl;
  std:: cout << "data - none:" << std::endl;
  stk1.StackPrint(std::cout);
  std::cout << std::endl;

  code1 = stk1.StackOK();

  std::cout << "Error code for stk1 : ";
  print_err(std::cout, code1);
  std::cout << std::endl;

  std::cout << "///////////////////// Canary protection check /////////////////////\n";
  #ifndef STACK_CANARY_PROTECTION
  #define STACK_CANARY_PROTECTION
  #endif //STACK_CANARY_PROTECTION

  auto canaryFront_buf = (StkCanaryT *)((char *)stk1.data - sizeof(StkCanaryT));
  auto canaryBack_buf  = (StkCanaryT *)((char *)stk1.data + sizeof(int) * stk1.capacity);

  std::cout << std::hex << std::uppercase;
  std::cout << "Front buffer canary value : " << *canaryFront_buf << " (0xDEADBEEFBADF00D)" << std::endl;
  std::cout << "Back  buffer canary value : " << *canaryBack_buf <<  " (0xDEADBEEFBADF00D)" << std::endl;
  std::cout << std::endl;

  *canaryFront_buf = 0x8BADF00D;

  code1 = stk1.StackOK();
  stk1.StackDump(CURR_LOCATION);

  std::cout << "Error code for stk1 : ";
  print_err(std::cout, code1);
  std::cout << std::endl;

  std::cout << "///////////////////// End of test /////////////////////\n";
}

#endif //STACKCPP_STACK_FUNCTIONS_H
