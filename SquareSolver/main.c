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
  NO_ROOTS,
  ONE_ROOT,
  TWO_ROOTS
};

enum BooleanExpression
{
  FALSE_EXP,
  TRUE_EXP
};

enum RootsNumber SolveSquare( double a, double b, double c, double* x1, double* x2 );
void SquareTester( void );
enum RootsNumber SolveLinear( double a, double b, double* x );
enum BooleanExpression is_double_zero( double num );

int main()
{
  printf("# Square equation solver\n"
         "# (c) Fayzullin Zafar, 2019\n\n");
  /*printf("# Enter equation coefficients through a space (a, b, c) without any other symbols: ");

  double a = 0, b = 0, c = 0;
  scanf ("%lg %lg %lg", &a, &b, &c);
  double x1 = 0, x2 = 0;
  int NumOfRoots = SolveSquare(a, b, c, &x1, &x2);


  switch (NumOfRoots)
  {
    case 0:
      printf("No roots\n");
      break;

    case 1:
      printf("X = %lg\n", x1);
      break;

    case 2:
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

  if (is_double_zero(a))
  {
    /* Linear equation */
    if (is_double_zero(b))
    {
      return (is_double_zero(c)) ? INF_ROOTS : 0;
    }
    else /* if (b != 0) */
    {
      *x1 = -c / b;
      return ONE_ROOT;
    }
  }
  else /* if (a != 0) */
  {
    double D = b * b - 4 * a * c;

    if (D < 0)
    {
      return NO_ROOTS;
    }
    else if (is_double_zero(D))
    {
      *x1 = *x2 = -b / (2 * a);
      return ONE_ROOT;
    }
    else
    {
      double sqrt_D = sqrt(D);

      *x1 = (-b - sqrt_D) / (2 * a);
      *x2 = (-b + sqrt_D) / (2 * a);

      return TWO_ROOTS;
    }
  }
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
  char line_buffer[1100];
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

  double** test_coeffs = (double **)malloc(sizeof(double *) * num_of_tests);
  for (int i = 0; i < num_of_tests; i++)
  {
    test_coeffs[i] = (double *)malloc(sizeof(double) * 3);
  }

  /* Reading coefficients from file */
  for (unsigned long int i = 0; i < num_of_tests; i++)
  {
    errno = 0;
    curr_pos_in_buffer = line_buffer;
    fgets(line_buffer, sizeof(line_buffer), in_file);
    for (int j = 0; j < 3; j++)
    {
      test_coeffs[i][j] = strtod(curr_pos_in_buffer, &curr_pos_in_buffer);
    }
    if (errno == ERANGE)
    {
      fprintf(out_file, "%lu : could not read coefficients for this test, "
                        "because they are either out of range, or wrong formatted\n", i);
    }
    else
    {
      int num_of_roots;
      double x1 = NAN, x2 = NAN;

      num_of_roots = SolveSquare(test_coeffs[i][0], test_coeffs[i][1], test_coeffs[i][2], &x1, &x2);

      fprintf(out_file, "%lu\n",  i + 1);
      fprintf(out_file, "Test coeffincients: a = %lg, b = %lg, c = %lg\n", test_coeffs[i][0], test_coeffs[i][1], test_coeffs[i][2]);
      fprintf(out_file, "Number of roots: %i\n", num_of_roots);
      fprintf(out_file, "Test solution: ");
      switch (num_of_roots)
      {
        case 0:
          fprintf(out_file, "No roots\n");
          break;

        case 1:
          fprintf(out_file, "X = %lg\n", x1);
          break;

        case 2:
          fprintf(out_file, "X1 = %lg, X2 = %lg\n", x1, x2);
          break;

        case INF_ROOTS:
          fprintf(out_file, "Any number\n");
          break;

        default:
          fprintf(out_file, "main(): ERROR: num_of_roots = %d\n", num_of_roots);
          return;
      }
    }
  }

  fclose(in_file);
  fclose(out_file);
  for (int i = 0; i < num_of_tests; i++)
  {
    free(test_coeffs[i]);
  }
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
int SolveLinear( double a, double b, double* x )
{
  assert(isfinite(a));
  assert(isfinite(b));
  assert(x != NULL);

  if (a == 0)
  {
    return (b == 0) ? INF_ROOTS : NO_ROOTS;
  }
  else /* if (a != 0) */
  {
    assert(isfinite(-b / a));
    *x = -b / a;
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