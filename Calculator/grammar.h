#ifndef CALCULATOR_GRAMMAR_H
#define CALCULATOR_GRAMMAR_H

#include <map>
#include "error_functions.h"

/* Initializes 'result' with 'get_value',
 * checks if 'inputBuffer' contains 'SUCCESS' error code and returns result if not. */
#define GET_AND_CHECK_WITH_RETURN(result, get_value, inputBuffer)  \
        {                                                          \
          result = get_value;                                      \
          if (inputBuffer.ShowErr() != SUCCESS)                    \
          {                                                        \
            return result;                                         \
          }                                                        \
        }

/* Detects syntax error if current character of 'inputBuffer' is not 'char' */
#define REQUIRE(char, inputBuffer) \
        {                                                                   \
          if (inputBuffer.ShowCurr() == char) { inputBuffer.IncOffset(); }  \
          else                                { SyntaxError(inputBuffer); } \
        }

struct InputBuffer
{
private:
  char *buffer;     /* Contains input expression */
  size_t offset;    /* Offset from the head of input buffer */
  ERR_CODE errCode; /* Function using structure can set error code here */

public:
  /* Class constructor */
  explicit InputBuffer(char *init_buffer, size_t init_offset = 0) : buffer(init_buffer), offset(init_offset), errCode(SUCCESS)
  {
  }

  /* Returns current element from buffer without offset increment */
  char ShowCurr() const
  {
    return buffer[offset];
  }

  /* Increments offset from buffer head */
  void IncOffset()
  {
    offset++;
  }

  /* Returns current element from buffer without offset increment */
  char Get()
  {
    char result = buffer[offset];
    offset++;
    return result;
  }

  /* Returns current offset from buffer head */
  size_t GetOffset() const
  {
    return offset;
  }

  /* Returns errCode */
  ERR_CODE ShowErr() const
  {
    return errCode;
  }

  /* Sets new error code given as parameter */
  void SetErr(ERR_CODE code)
  {
    errCode = code;
  }
};

class Grammar
{
public:
  /* Identifier types */
  enum ID_TYPE
  {
    NOT_ID,
    ID_SIN,
    ID_COS,
    ID_TAN,
    ID_COT,
    ID_SQRT,
    ID_LN
  };

private:
  const char terminator; /* Expression terminating symbol */
  std::map<std::string, ID_TYPE> ID_map; /* Identifier map */

public:
  /* Class constructor which requires expression terminating symbol */
  explicit Grammar(char init_terminator = '$') : terminator(init_terminator)
  {
    ID_map["sin"]  = ID_SIN;
    ID_map["cos"]  = ID_COS;
    ID_map["tan"]  = ID_TAN;
    ID_map["cot"]  = ID_COT;
    ID_map["sqrt"] = ID_SQRT;
    ID_map["ln"]   = ID_LN;
  }

  /* Calculates given expression using grammar rules and returns result and error code */
  std::pair<double, ERR_CODE> CalcExpr(const char *buffer);

private:
  double GetG(InputBuffer &inputBuffer);       /* Implies axiom rule of grammar. G->E'$' */
  double GetE(InputBuffer &inputBuffer);       /* Implies [+,-] expression reading rule of grammar. E->T{[+,-]T}* */
  double GetT(InputBuffer &inputBuffer);       /* Implies [*,/] expression reading rule of grammar. T->D{[*,/]D}* */
  double GetD(InputBuffer &inputBuffer);       /* Implies [^] expression reading rule of grammar. D->P{^D}* */
  double GetP(InputBuffer &inputBuffer);       /* Implies parentheses obtain. P->'('E')' | N | Id'('E')' */
  double GetN(InputBuffer &inputBuffer);       /* Implies number reading rule of grammar. N->[+,-, eps][0,...,9]+ */
  ID_TYPE GetId(InputBuffer &inputBuffer);     /* Implies ['a'-'z' | 'A'-'Z']+ reading rule of grammar */

  double CalcOpId(ID_TYPE idType, double value);                              /* Calculates identifier operation */
  static void SyntaxError(InputBuffer &inputBuffer, ERR_CODE code = FAILURE); /* Sets new error code of input buffer */
  void SkipSpace(InputBuffer &inputBuffer);                                   /* Increases offset of 'inputBuffer'
                                                                               * until all space characters ' ' are skipped */
};

#endif //CALCULATOR_GRAMMAR_H
