#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <errno.h>

#define TYPE_INACCURACY (1e-14)

enum RootsNumber
{
  INF_ROOTS = -1,
  NO_ROOTS  = 0,
  ONE_ROOT  = 1,
  TWO_ROOTS = 2
};

enum BooleanExpression
{
  FALSE_EXP,
  TRUE_EXP
};

struct SquareTestType
{
  double a, b, c;
  enum RootsNumber num_of_roots;
  double x1, x2;
};

/***
 * Functions prototypes
 ***/
enum RootsNumber SolveSquare( double a, double b, double c, double* x1, double* x2 );

void SquareTester( void );

enum RootsNumber SolveLinear( double a, double b, double* x );

enum BooleanExpression is_double_zero( double num );

/***
 * Program main function
 *
 * @return  finish code
 */

int main()
{
  printf("# Square equation solver\n"
         "# (c) Fayzullin Zafar, 2019\n\n");
  /*printf("# Enter equation coefficients through a space (a, b, c) without any other symbols: ");

  double a = 0, b = 0, c = 0;
  scanf ("%lg %lg %lg", &a, &b, &c);
  double x1 = 0, x2 = 0;
  enum RootsNumber NumOfRoots = SolveSquare(a, b, c, &x1, &x2);


  switch (NumOfRoots)
  {
    case NO_ROOTS:
      printf("No roots\n");
      break;

    case ONE_ROOT:
      printf("X = %lg\n", x1);
      break;

    case TWO_ROOTS:
      printf("X1 = %lg, X2 = %lg\n", x1, x2);
      break;

    case INF_ROOTS:
      printf("Any number");
      break;

    default:
      printf ("main(): ERROR: NumOfRoots = %d\n", NumOfRoots);
      return 1;
  }

  printf("\n");
  printf("DBL_MAX : %lg\n", DBL_MAX);
  printf("HUGE_VAL : %lg\n", HUGE_VAL);*/
  SquareTester();

  return 0;
}
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Solves a square equation ax2 + bx + c = 0
//!
//! @param [in] a    a‐coefficient
//! @param [in] b    b‐coefficient
//! @param [in] c    c‐coefficient
//! @param [out] x1  Pointer to the 1st root
//! @param [out] x2  Pointer to the 2nd root
//!
//! @return Number of roots
//!
//! @note In case of infinite number of roots,
//! returns INF_ROOTS
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
enum RootsNumber SolveSquare( double a, double b, double c, double* x1, double* x2 )
{
  assert(isfinite(a));
  assert(isfinite(b));
  assert(isfinite(c));

  assert(fabs(b) < sqrt(DBL_MAX));
  assert(fabs(4 * a * c) < sqrt(DBL_MAX));

  assert (x1 != NULL);
  assert (x2 != NULL);
  assert (x1 != x2);

  enum RootsNumber num_of_roots;

  if (is_double_zero(a))
  {
    /* Linear equation */
    num_of_roots = SolveLinear(b, c, x1);
    *x2 = *x1;
  }
  else /* if (a != 0) */
  {
    double D = b * b - 4 * a * c;

    if (D < 0)
    {
      num_of_roots = NO_ROOTS;
    }
    else if (is_double_zero(D))
    {
      *x1 = *x2 = -b / (2 * a);
      num_of_roots = ONE_ROOT;
    }
    else /* if (D > 0) */
    {
      double sqrt_D = sqrt(D);

      *x1 = (-b - sqrt_D) / (2 * a);
      *x2 = (-b + sqrt_D) / (2 * a);
      num_of_roots = TWO_ROOTS;
    }
  }

  /* Type inaccuracy and -0.0 correction */
  if (num_of_roots > 0)
  {
    *x1 = (fabs(*x1) < TYPE_INACCURACY) ? 0.0 : *x1;
    *x2 = (fabs(*x2) < TYPE_INACCURACY) ? 0.0 : *x2;
  }

  return num_of_roots;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! SolveSquare testing function.
//! The function reads test coefficients from "test_in.txt" file,
//! solves equation and print test information to "test_out.txt" file.
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void SquareTester( void )
{
  FILE *in_file, *out_file;
  if ((in_file = fopen("test_in.txt", "rt")) == NULL)
  {
    printf("Test input file \"test_in.txt\" is broken or does not exist\n");
    return;
  }

  if ((out_file = fopen("test_out.txt", "wt")) == NULL)
  {
    printf("Test output file \"test_out.txt\" is broken or does not exist\n");
    fclose(in_file);
    return;
  }

  unsigned long int num_of_tests;
  char line_buffer[2048];
  char *curr_pos_in_buffer = line_buffer;

  errno = 0;

  fgets(line_buffer, sizeof(line_buffer), in_file);
  num_of_tests = strtoul(line_buffer, &curr_pos_in_buffer, 10);
  if (errno == ERANGE)
  {
    fprintf(out_file, "Test input file has wrong format : "
                      "there is no number of tests specified in the beginning \n");
    fclose(in_file);
    fclose(out_file);
    return;
  }
  else
  {
    fprintf(out_file, "Number of tests : %lu \n", num_of_tests);
  }

  struct SquareTestType* test_coeffs = (struct SquareTestType *)calloc(num_of_tests, sizeof(struct SquareTestType));
  if (test_coeffs == NULL)
  {
    fprintf(out_file, "Memory allocation failure\n");
    fclose(in_file);
    fclose(out_file);
    return;
  }

  /* Reading coefficients, number of roots and solutions from file */
  for (unsigned long int i = 0; i < num_of_tests; i++)
  {
    errno = 0;
    curr_pos_in_buffer = line_buffer;
    fgets(line_buffer, sizeof(line_buffer), in_file);
    test_coeffs->a = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
    test_coeffs->b = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
    test_coeffs->c = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
    test_coeffs->num_of_roots = (enum RootsNumber)strtol(curr_pos_in_buffer, &curr_pos_in_buffer, 10);

    switch(test_coeffs->num_of_roots)
    {
      case INF_ROOTS:
      case NO_ROOTS:
        break;

      case ONE_ROOT:
        test_coeffs->x1 = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
        test_coeffs->x2 = test_coeffs->x1;
        break;

      case TWO_ROOTS:
        test_coeffs->x1 = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
        test_coeffs->x2 = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
        break;

      default:
        fprintf(out_file, "Something is wrong with the testing file."
                          "There is wrong number of roots on the line number %lu\n", i + 2);
        return;
    }

    if (errno == ERANGE)
    {
      fprintf(out_file, "%lu : could not read coefficients for this test, "
                        "because they are either out of range, or wrong formatted\n", i);
    }
    else
    {
      enum RootsNumber num_of_roots;
      double x1 = NAN, x2 = NAN;

      num_of_roots = SolveSquare(test_coeffs->a, test_coeffs->b, test_coeffs->c, &x1, &x2);

      fprintf(out_file, "%lu\n",  i + 1);
      fprintf(out_file, "Test coeffincients: a = %lg, b = %lg, c = %lg\n", test_coeffs->a, test_coeffs->b, test_coeffs->c);
      fprintf(out_file, "Real number of roots: %i\n", test_coeffs->num_of_roots);
      fprintf(out_file, "Test number of roots: %i\n", num_of_roots);

      fprintf(out_file, "Real solution: ");
      switch (test_coeffs->num_of_roots)
      {
        case NO_ROOTS:
          fprintf(out_file, "No roots\n");
          break;

        case ONE_ROOT:
          fprintf(out_file, "X = %lg\n", test_coeffs->x1);
          break;

        case TWO_ROOTS:
          fprintf(out_file, "X1 = %lg, X2 = %lg\n", test_coeffs->x1, test_coeffs->x2);
          break;

        case INF_ROOTS:
          fprintf(out_file, "Any number\n");
          break;

        default:
          fprintf(out_file, "SquareTester() : ERROR: test_coeffs->num_of_roots = %d\n", test_coeffs->num_of_roots);
          return;
      }

      fprintf(out_file, "Test solution: ");
      switch (num_of_roots)
      {
        case NO_ROOTS:
          fprintf(out_file, "No roots\n");
          break;

        case ONE_ROOT:
          fprintf(out_file, "X = %lg\n", x1);
          break;

        case TWO_ROOTS:
          fprintf(out_file, "X1 = %lg, X2 = %lg\n", x1, x2);
          break;

        case INF_ROOTS:
          fprintf(out_file, "Any number\n");
          break;

        default:
          fprintf(out_file, "SquareTester(): ERROR: num_of_roots = %d\n", num_of_roots);
          return;
      }

      enum BooleanExpression test_failure_flag = FALSE_EXP;
      if (num_of_roots != test_coeffs->num_of_roots)
      {
        test_failure_flag = TRUE_EXP;
      }
      else /* if (num_of_roots == test_coeffs->num_of_roots) */
      {
        switch (num_of_roots) {
          case NO_ROOTS:
          case INF_ROOTS:
            break;

          case ONE_ROOT:
            if (test_coeffs->x1 != x1)
              test_failure_flag = TRUE_EXP;
            break;

          case TWO_ROOTS:
            if ((test_coeffs->x1 != x1 && test_coeffs->x2 != x2) && (test_coeffs->x1 != x2 && test_coeffs->x2 != x1))
              test_failure_flag = FALSE_EXP;
            break;

          default:
            /* unreachable code */
            return;
        }
      }
      fprintf(out_file, "Test : %s\n", test_failure_flag ? "Failed" : "OK");
    }
  }

  fclose(in_file);
  fclose(out_file);

  free(test_coeffs);
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Solves linear equation ax + b = 0
//!
//! @param [in] a   a-coefficient
//! @param [in] b   b-coefficient
//!
//! @param [out] Number of roots
//!
//! @note In case of infinite number of roots,
//! returns INF_ROOTS
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
enum RootsNumber SolveLinear( double a, double b, double *x )
{
  assert(isfinite(a));
  assert(isfinite(b));
  assert(x != NULL);

  if (a == 0)
  {
    *x = NAN;
    return (b == 0) ? INF_ROOTS : NO_ROOTS;
  }
  else /* if (a != 0) */
  {
    assert(isfinite(-b / a));
    *x = (fabs(-b / a) < TYPE_INACCURACY) ? 0.0 : (-b / a);

    return ONE_ROOT;
  }
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Checks if number is zero by comparing it with small number
//!
//! @param [in] a   a-coefficient
//! @param [in] b   b-coefficient
//!
//! @param [out] Number of roots
//!
//! @note In case of infinite number of roots,
//! returns INF_ROOTS
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
enum BooleanExpression is_double_zero( double num )
{
  assert(isfinite(num));

  return (fabs(num) < TYPE_INACCURACY) ? TRUE_EXP : FALSE_EXP;
}