local tcc = require 'tcc' ('libtcc')

local state = tcc.new()

state:set_output_type('output_memory')

state:compile_string [[
  #include <stdio.h>

  int fibR(int n)
  {
      if (n < 2) return n;
      return (fibR(n-2) + fibR(n-1));
  }

  int main(int argc, char** argv)
  {
    for (int i = 0; i < argc; i++){
        printf("%s \n", argv[i]);
    }
    int N = 34; //Should return 433494437
    printf("fib: %d\n", fibR(N));
    return 0;
  }
]]

state:run { 1, 2, 3, 'text', false }

state:delete()
