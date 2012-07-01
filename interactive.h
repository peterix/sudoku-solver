#ifndef interactive_h
#define interactive_h
#include "sudoku.h"

/*
 * Toto by melo fungovat na vsech operacnich systemech a drtive vetsine kompilatoru. Zadavani prikazu je sice trochu krkolomne,
 * ale vyvazi se to tim, ze pouzijeme pouze standardni C++, zadne zmetky z DOSu, Windows, Linuxu nebo od firmy Borland.
 * 
 * Da se to jednoduse prepsat v cemkoliv...
 */

// interaktivni smycka
int MainLoop ( Sudoku * s );
#endif