#include "text_functions.h"

int main()
{
  //text_functions_tester();
  const char *in_file_name = "romeo-and-juliet_Shakespeare.txt",
             *out_file_name = "romeo-and-juliet_Shakespeare_out.txt";

  sort_text(in_file_name, out_file_name);

  return 0;
}
