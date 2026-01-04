1. Linux coding style
2. No global variables, only if there is no other way
3. It is the responsibility of the function to print error messages, not the caller (the caller should exit)
4. If we have nested functions, the inner function is responsible for printing error messages for its own work
5. Every error should be written to stderr
6. True Usually means success and and false failure (or simply just exit(without error))
7. User created structs are stack allocated
8. frvCamalCase naming convention
