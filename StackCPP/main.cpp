#include <iostream>

#define STACK_CANARY_PROTECTION
#define STACK_HASH_PROTECTION

#include <cstdlib>
#include <cstring>

#include "stack_functions.h"

int main()
{
  StackTester();

  return 0;
}
