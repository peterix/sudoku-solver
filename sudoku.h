#ifndef sudoku_h
#define sudoku_h
#include <iostream>

// struktura pro stav policka
struct sudoku_policko {
     unsigned int zapsane:4;
     unsigned int pevne:1;
     unsigned int error:1;
};

// struktura pro ulozeni stavu sudoku - vhodna pro prenos stavu mezi dvemi sudoku
struct stav
{
// stav jednotlivych policek
  sudoku_policko pole[81];
// urcuji co je uz vyplneo v radku/sloupci/ctverci. jedna se o bitove mapy
  unsigned int sloupce[9];
  unsigned int radky[9];
  unsigned int ctverce[3][3];
// globalni stav sudoku
  unsigned int stav;
// kolik je uz uspesne doplneno
  unsigned int doplnenych;
// je v sudoku spatne vyplnene pole?
  unsigned int rozbite;
};

// mozne stavy, ne vsechny jsou nakonec pouzity ~_~
enum
{
  _nenastavene = 0,
  _chyba,
  _nastavene,
  _vyhrane
};

using namespace std;

// trida pro hrani, generovani a reseni sudoku
class Sudoku
{
public:
// konstruktory a destruktory
  Sudoku(string soubor); // ze souboru
  Sudoku(Sudoku* kopiruj); // z existujiciho sudoku
// generovani sudoku
// specialni hodnoty: 0 = z nahodneho cisla (systemovy cas)
//                   -1 = prazdne sudoku
//                 jine = vzdy stejne sudoku podle zadaneho cisla
  Sudoku(int seed);
  ~Sudoku();
// uplne vynulovani hraciho pole
  void initPole( void );
// odstraneni vsech hracem doplnenych cisel
  void restart( void );
// vypis stav
  void print( void );
// reseni pomoci jednoduche metody
  int jednoducheReseni( void );
// reseni rekurzivnim zanorovani a hadanim cisel
  int rekurzivniReseni( void );
// krok jednoduche metody
  int krok ( void );
// smazani cisla z poxice x,y
  int smazat ( int x, int y );
// krok rekurzivni metody
  int rKrok( int index );

// kolik cisel jiz neni mozne vyplnit na urcite pozici
  int kolikBitu (int x, int y);
// ktere jedine cislo je mozne na pozici vyplnit?
  int ziskejBit (int x, int y);

// jaky je stav sudoku? (prevazne k detekci chyb pri nacitani ze souboru)
  int getStav (void);

// skryj vsechny cisla az na 30. techto 30 se stava pevnymi. jejich konfigurace je takove, ze je mozne sudoku vyresit jednoduchou metodou
  void skryjNa30();
// dopln cislo na pozici
  int doplnM (int x, int y, int cislo); // vraci 0 pokud nelze cislo vyplnit, 1 pokud se to podari
// dopln cislo na pozici s kontrolou spravnosti
  int doplnK (int x, int y, int cislo); // vraci 0 pokud nelze cislo vyplnit, 1 pokud se to podari
  sudoku_policko policko (int x,int y);// co je na pozici?
// stav hraciho pole.
  stav s;
};
#endif