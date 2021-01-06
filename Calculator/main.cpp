#include <iostream>
#include <string>
#include <vector>

#include "calculator.h"
#include "grammar.h"

/* Calculator testing function */
void CalcTester()
{
  std::cout << "This is test function";
  std::cout << std::endl << std::endl;

  std::vector<std::string> buffer = {"1250="   , "05="                     ,
                                     "2 + 3 =" , "3 + +2="                 , "-3+4++5+(-3)=", "45-(-10)+100-45+3=",
                                     "5*120="  , "2+3*10+20/10+5="         ,
                                     "5+(3*5)=", "2+3+10*(2+(3+20)*30/15)=",
                                     "2^3^2="  , "2^(2^3^2-(2^2+1)*100)="  , "256^-3",
                                     "sqrt(225)=", "sqrt(13+3^(2-1))="     , "sin(3.14/6)=", "ln(2.71^5)=",
                                     "ln   (2.71    ^ 15     ) = "};

  for (auto &expr : buffer)
  {
    Grammar grammar('=');
    std::pair<double, ERR_CODE> result = grammar.CalcExpr(expr.c_str());
    std::cout << expr << result.first << std::endl;
  }
}

int main()
{
  Calculator::Start();
  //CalcTester();

  return 0;
}
