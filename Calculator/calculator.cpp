#include "calculator.h"
#include "text_colors.h"

/* Prints menu of calculator */
void Calculator::PrintMenu()
{
  auto &os = std::cout;

  os << "Enter \"baranka\" to exit or\n";
  os << "enter expression to calculate:\n";
}

/* Initiates the work of calculator */
void Calculator::Start()
{
  auto &is = std::cin;
  std::string input;

  while (true)
  {
    PrintMenu();
    getline(is, input);

    if (input == "baranka")
    {
      return;
    }

    if (input.back() != '=') { input.push_back('='); }

    Grammar grammar('=');
    std::pair<double, ERR_CODE> result = grammar.CalcExpr(input.c_str());

    result.second != SUCCESS ? std::cout << RED << "Entered expression has wrong format. Try again."
                             : std::cout << MAGENTA << result.first;
    std::cout << RESET << std::endl << std::endl;
  }
}
