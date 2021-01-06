#include <iostream>
#include <cmath>
#include <cctype>

#include "grammar.h"

/* Calculates given expression using grammar rules and returns result and error code */
std::pair<double, ERR_CODE> Grammar::CalcExpr(const char *buffer)
{
  InputBuffer inputBuffer(const_cast<char *>(buffer));
  std::pair<double, ERR_CODE> result;

  result.first = GetG(inputBuffer);
  result.second = inputBuffer.ShowErr();

  return result;
}

/* Implies axiom rule of grammar. G->E$ */
double Grammar::GetG(InputBuffer &inputBuffer)
{
  double result = 0;
  GET_AND_CHECK_WITH_RETURN(result, GetE(inputBuffer), inputBuffer)

  if (inputBuffer.Get() != terminator) { SyntaxError(inputBuffer); }

  return result;
}

/* Implies [+,-] expression reading rule of grammar. E->T{[+,-]T}* */
double Grammar::GetE(InputBuffer &inputBuffer)
{
  double result = 0;
  GET_AND_CHECK_WITH_RETURN(result, GetT(inputBuffer), inputBuffer)

  while (inputBuffer.ShowCurr() == '+' || inputBuffer.ShowCurr() == '-')
  {
    char operation = inputBuffer.Get();

    double tmp = 0;
    GET_AND_CHECK_WITH_RETURN(tmp, GetT(inputBuffer), inputBuffer)

    if (operation == '+') { result += tmp; }
    else                  { result -= tmp; }
  }

  return result;
}

/* Implies [*,/] expression reading rule of grammar. T->D{[*,/]D}* */
double Grammar::GetT(InputBuffer &inputBuffer)
{
  double result = 0;
  GET_AND_CHECK_WITH_RETURN(result, GetD(inputBuffer), inputBuffer)

  while (inputBuffer.ShowCurr() == '*' || inputBuffer.ShowCurr() == '/')
  {
    char operation = inputBuffer.Get();

    double tmp = 0;
    GET_AND_CHECK_WITH_RETURN(tmp, GetD(inputBuffer), inputBuffer)

    if (operation == '*') { result *= tmp; }
    else                  { result /= tmp; }
  }

  return result;
}

/* Implies [^] expression reading rule of grammar. D->P{^D}* */
double Grammar::GetD(InputBuffer &inputBuffer)
{
  double result = 0;
  GET_AND_CHECK_WITH_RETURN(result, GetP(inputBuffer), inputBuffer)

  while (inputBuffer.ShowCurr() == '^')
  {
    inputBuffer.IncOffset();

    double tmp = 0;
    GET_AND_CHECK_WITH_RETURN(tmp, GetD(inputBuffer), inputBuffer)

    result = pow(result, tmp);
  }

  return result;
}

/* Implies parentheses obtain. P->'('E')' | N | Id'('E')' */
double Grammar::GetP(InputBuffer &inputBuffer)
{
  SkipSpace(inputBuffer);

  double result = 0;

  if (inputBuffer.ShowCurr() == '(')
  {
    inputBuffer.IncOffset();
    GET_AND_CHECK_WITH_RETURN(result, GetE(inputBuffer), inputBuffer)
    REQUIRE(')', inputBuffer)
  }
  else if (isdigit(inputBuffer.ShowCurr()) || inputBuffer.ShowCurr() == '+' || inputBuffer.ShowCurr() == '-')
  {
    result = 0;
    GET_AND_CHECK_WITH_RETURN(result, GetN(inputBuffer), inputBuffer)
  }
  else /* Id'('E')' is to be obtained here */
  {
    ID_TYPE idType = NOT_ID;
    GET_AND_CHECK_WITH_RETURN(idType, GetId(inputBuffer), inputBuffer)

    if (idType == NOT_ID)
    {
      SyntaxError(inputBuffer);
    }
    else
    {
      SkipSpace(inputBuffer);
      REQUIRE('(', inputBuffer)

      GET_AND_CHECK_WITH_RETURN(result, GetE(inputBuffer), inputBuffer)
      result = CalcOpId(idType, result);

      REQUIRE(')', inputBuffer)
    }
  }

  SkipSpace(inputBuffer);
  return result;
}

/* Implies number reading rule of grammar. N->[+,-, eps][0,...,9]+ */
double Grammar::GetN(InputBuffer &inputBuffer)
{
  bool is_positive = true;
  if (inputBuffer.ShowCurr() == '+' || inputBuffer.ShowCurr() == '-')
  {
    is_positive = (inputBuffer.Get() == '+');
  }

  double result = 0;
  size_t start_offset = inputBuffer.GetOffset();

  while ('0' <= inputBuffer.ShowCurr() && inputBuffer.ShowCurr() <= '9') /* integer part */
  {
    result = result * 10 + inputBuffer.Get() - '0';
  }

  if (inputBuffer.ShowCurr() == '.' || inputBuffer.ShowCurr() == ',')
  {
    inputBuffer.IncOffset();
    double tenPower = 10;
    while ('0' <= inputBuffer.ShowCurr() && inputBuffer.ShowCurr() <= '9') /* fraction part */
    {
      result += (inputBuffer.Get() - '0') / tenPower;
      tenPower *= 10;
    }
  }

  if (inputBuffer.GetOffset() == start_offset)
  {
    SyntaxError(inputBuffer);
  }

  if (!is_positive) { result *= -1; }

  return result;
}

/* Implies ['a'-'z' | 'A'-'Z']+ reading rule of grammar */
Grammar::ID_TYPE Grammar::GetId(InputBuffer &inputBuffer)
{
  std::string id_word{};

  while('a' <= inputBuffer.ShowCurr() && inputBuffer.ShowCurr() <= 'z' ||
        'A' <= inputBuffer.ShowCurr() && inputBuffer.ShowCurr() <= 'Z')
  {
    id_word.push_back(tolower(inputBuffer.Get()));
  }

  auto id_ptr = ID_map.find(id_word);
  if (id_ptr == ID_map.end())
  {
    return NOT_ID;
  }
  return id_ptr->second;
}

/* Calculates identifier operation */
double Grammar::CalcOpId(Grammar::ID_TYPE idType, double value)
{
  switch (idType)
  {
    case ID_SIN : return sin(value);
    case ID_COS : return cos(value);
    case ID_TAN : return tan(value);
    case ID_COT : return 1.0 / tan(value);
    case ID_SQRT: return sqrt(value);
    case ID_LN  : return log(value);
    case NOT_ID : //fallthrough;
    default     : return 0;
  }
}

/* Sets new error code of input buffer */
void Grammar::SyntaxError(InputBuffer &inputBuffer, ERR_CODE code)
{
  code >= ERR_LAST ? inputBuffer.SetErr(FAILURE) : inputBuffer.SetErr(code);
}

/* Increases offset of 'inputBuffer' until all space characters ' ' are skipped */
void Grammar::SkipSpace(InputBuffer &inputBuffer)
{
  while (inputBuffer.ShowCurr() == ' ')
  {
    inputBuffer.IncOffset();
  }
}
