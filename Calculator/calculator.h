#ifndef CALCULATOR_CALCULATOR_H
#define CALCULATOR_CALCULATOR_H

#include "grammar.h"

class Calculator
{
private:
  static void PrintMenu(); /* Prints menu of calculator */

public:
  /* Class constructor */
  Calculator()
  =default;

  static void Start();  /* Initiates the work of calculator */
};


#endif //CALCULATOR_CALCULATOR_H
