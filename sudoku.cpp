#include "sudoku.h"
#include <string.h> // textovy vystup
#include <fstream> // prace se soubory
#include <ctime> // cas jako zdroj pro pseudonahodny generator
#include <cstdlib> // pseudonahodny generator cisel
#include <algorithm> // kvuli algoritmu pro nahodne prehazeni poradi cisel v poli

void Sudoku::initPole ( void )
{
  memset ( &s, 0, sizeof(stav) );
}

// cti ze souboru
Sudoku::Sudoku(string soubor)
{
  char znak;
  int index = 0;
  initPole();// vynulujeme pole
  // otevreme soubor pro cteni
  ifstream souborKeCteni;
  souborKeCteni.open(soubor.c_str());

  if (!souborKeCteni) // nepodarilo se?
  {
    cerr << "Soubor " << soubor << " nelze precist." << endl;
    s.stav= _chyba; // nastavime chybovy stav a vratime se
    return;
  }
  while ((souborKeCteni >> znak) && (index < 81)) // cteni ze souboru po znacich dokud nemame cele sudoku prectene
  {
    if(znak >= '1' && znak <= '9')// je to cislo?
    {
      if (doplnK(index %9, index/9, znak - '1' + 1)) // zkusime ho doplnit do sudoku
      {
        s.pole[index].pevne = 1; // vsechna policka nactena za souboru jsou pevna
        index++;
        continue;// kdyz se povede, pokracujeme dalsim znakem
      }
      // pokud se nepodari doplnit, propadne to sem, dojde k chybe
      souborKeCteni.close();
      s.stav= _chyba;
      cerr << "Soubor neodpovida pravidlum sudoku." << endl;
      return;
    }
    else if(znak == '.' || znak == ',')// je to tecka nebo carka?
    {
      index++;// prazdne policko -> posuneme se o policko dal
    }
    // jinak se nic nedeje.
  }
  if(index != 81)// pokud jsme nedosli na konec sudoku, znamena to, ze ho kus chybi
  {
    // uklidime a nastavime chybovy stav
    souborKeCteni.close();
    s.stav= _chyba;
    cerr << "Soubor neobsahuje cele sudoku." << endl;
    return;
  }
  s.stav = _nastavene; // vse OK
}

// jednoducha metoda pro ziskani stavu ...
int Sudoku::getStav (void)
{
  return s.stav;
}

// vytvoreni noveho sudoku pomoci kopirovani stareho
Sudoku::Sudoku(Sudoku* kopiruj)
{
  s = kopiruj->s;
}

// metoda pro zruseni vsech nepevnych vyplnenych cisel
void Sudoku::restart( void )
{
  Sudoku temp(-1); // operace probiha kopirovanim do docasneho sudoku
  int x,y;
  for(int i = 0; i< 81; i++)// projdeme vse
  {
    if(s.pole[i].pevne)// narazime-li na pevne cislo
    {
      // prekopirujeme jeho stav
      temp.s.pole[i] = s.pole[i];
      x = i % 9;
      y = i / 9;
      // a nastavime zakazane hodnoty pro radky/sloupce/ctverce
      temp.s.radky[y] |= (1 << s.pole[i].zapsane);
      temp.s.sloupce[x] |= (1 << s.pole[i].zapsane);
      temp.s.ctverce[x/3][y/3] |= (1 << s.pole[i].zapsane);
      temp.s.doplnenych ++; // mame dalsi doplnene cislo
    }
  }
  // preneseni stavu z docasneho sudoku do puvodniho (tohoto)
  s = temp.s;
}

// generuj
Sudoku::Sudoku( int seed )
{
  initPole();
  //stav temp;
  if (seed == -1) // prazdne puzzle
    return;
  if (seed == 0)// ciste nahodne puzzle
  {
    srand(time(0));
  }
  else// puzzle dane zadanou ciselnou hodnotou
  {
    srand(seed);
  }
  rekurzivniReseni(); // vyresime
  skryjNa30(); // nechame 30 odkrytych cisel tak, aby bylo mozne sudoku vyresit zakladnimi metodami
  restart(); // zrus vsechny ne-pevna cisla
}

void Sudoku::skryjNa30()
{
  Sudoku temp(-1); // vytvorime prazdne sudoku
  int x,y;
  do
  {
    temp.initPole(); // uplne vyprazdnime docasne sudoku
    // hledame 30 poli
    for (int n=0; n<30; n++)
    {
      int i;
      do
      {
        i = rand() % 81;
      } while (temp.s.pole[i].zapsane); // najdeme v docasnem sudoku nahodne prazdne policko
      // preneseme z naseho sudoku cislo do docasneho
      temp.s.pole[i].zapsane = s.pole[i].zapsane;
      temp.s.pole[i].pevne = true;// generujeme, vse pevne
      x = i % 9;
      y = i / 9;
      // zakazeme pouzit v radcich/sloupcich/ctvercich vyplnene pole
      temp.s.radky[y] |= (1 << s.pole[i].zapsane);
      temp.s.sloupce[x] |= (1 << s.pole[i].zapsane);
      temp.s.ctverce[x/3][y/3] |= (1 << s.pole[i].zapsane);
    }
    temp.s.doplnenych = 30; // vyplnili jsme 30 policek
  } while (!temp.jednoducheReseni()); // dokud se neda sudoku vyresit jednoduchym doplnovanim
  s = temp.s;// preneseni stavu z docasneho sudoku
}

// smazani policka probiha tak, ze se zkopiruje cele sudoku az na pole, ktere mazeme. je to nutne kvuli obnove zakazanyvh poli v radcich/sloupcich/ctvercich
int Sudoku::smazat ( int x, int y )
{
  int u,v, smazano = false;
  if(!s.pole[y*9+x].zapsane || s.pole[y*9+x].pevne) // pokud na pozici nic neni nebo je tam pevne cislo, nic se nedeje
    return false;
  Sudoku temp (-1); // prazdne sudoku pro doplnovani
  // projdeme vse
  for (int i = 0;i < 81; i++)
  {
    // vynechavame pri kopirovani mazane policko
    if(i/9 == y && i%9 == x)
    {
      smazano = true;
      continue;
    }
    // nezapisujeme prazdna policka, doslo by k zaseknuti
    if (!s.pole[i].zapsane)
      continue;
    // doplnime zpet policko
    temp.doplnM (i%9, i/9, s.pole[i].zapsane);
    temp.s.pole[i].pevne = s.pole[i].pevne;
  }
  s = temp.s; // preneseni stavu do tohoto sudoku z docasneho
  return smazano;
}

// uklid
Sudoku::~Sudoku()
{
  // neni co uklizet... vsechno jede v pohode bez dynamicke pameti.
}
// vypis stav - zacni nahore vlevo, po radcich
void Sudoku::print( void )
{
  for(int y=0;y<9;y++)
  {
    for(int x=0;x<9;x++)
    {
      if (s.pole[y*9+x].zapsane)
        cout<< s.pole[y*9+x].zapsane;
      else
        cout<< ".";
    } 
    cout << endl;
  }
   cout << endl;
}

// kolik bitu v celociselne promenne je nastaveno
int Sudoku::kolikBitu (int x, int y)
{
  // sjednoceni mnozin - zjistime co je zakazano pouzit na nasi pozci
  unsigned int zakazano = s.radky[y] | s.sloupce[x] | s.ctverce[x/3][y/3];
  // akumulator
  unsigned int akum;
  // provadime bitovy posun doprava dokud neni zakazano = 0
  for (akum = 0; zakazano; zakazano >>= 1)
  {
    // kdyz je na nejnizsim bitu 1, zvysime akumulator
    akum += zakazano & 1;
  }
  return akum; // vratime kolik bitu je nastaveno
}

// ziskej pozici nastaveneho bitu. platne jen pro jeden nastaveny bit
int Sudoku::ziskejBit (int x, int y)
{
  // obdobne jako v kolokBitu...
  unsigned int zakazano = s.radky[y] | s.sloupce[x] | s.ctverce[x/3][y/3];
  unsigned r = 0;
  // ktere hodnoty jsou povoleny? 0x000003FE je maska pouzita kvuli negaci - bez ni by vzniklo mnoho spatne nastavenych bitu
  unsigned int povoleno = ~zakazano & 0x000003FE;
  while (povoleno >>= 1) // posun doprava dokud neni povoleno == 0
  {
    r++;
  }
  return r; // pozice nejvyssiho nastaveneho bitu
}

// krok jednoducheho reseni
// jedna se o proste doplnovani podle pravidel sudoku
int Sudoku::krok()
{
  int doplneno = false;
  int z;
  // projdeme vse
  for(int y=0;y<9;y++)
  {
    for(int x=0;x<9;x++)
    {
      if(s.pole[y*9+x].zapsane) // ignorujeme jiz zapsana cisla
        continue;
      // pokud je na pozici zakazano jiz 8 cisel
      if((z = kolikBitu(x,y)) == 8)
      {
        // zjistime ktere cislo muzeme pouzit
        int r = ziskejBit(x,y);
        // a zkusime jej doplnit
        if(doplnK(x,y,r))
        {
          doplneno = true;
        }
      }
    }
  }
  // true kdyz se podarilo doplnit, false kdyz ne
  return doplneno;
}
// vstupni bod jednoduche doplnovaci metody
int Sudoku::jednoducheReseni()
{
  while (krok());// projdeme zakladni metodu reseni dokud je co doplnovat
  if(s.doplnenych == 81)
    return true;// vratime kladnou odpoved pokud jsme jiz vse doplnili
  return false;// jinak zapornou
}

// rekurzivni reseni - vstup  - pouzito tam, kde jednoducha metoda selhava
int Sudoku::rekurzivniReseni()
{
  Sudoku novy(this); // okopirujeme si sudoku
  int vysledek = novy.rKrok(0); // zkusime na nem rekurzivni metodu
  if(vysledek == true)
  {
    s = novy.s;// v pripade uspechu z nej prebereme jeho stav
  }
  return vysledek;// vratime vysledek pokusu, at je jakykoliv
}


// krok rekurzivni metody reseni
// zkousime doplnovat cisla dokud se to nepovede. muze selhat jedine v pripade, ze je sudoku spatne specifikovane (nema platne reseni)
int Sudoku::rKrok (int index)
{
  int vysledek;
  int cisla[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // seznam cisel na vyzkouseni
  // funguje to tak, ze si udelame tento seznam a ten pak pokazde nahodne prehazime
  // pro nedostatecne specifikovane sudoku (majici vice reseni) to znamena, ze reseni jsou nahodna

  // projdeme doplnovaci metodu, pokud to jde, vratime kladny vysledek
  // tato motoda diky tomu jak je postavena odchyti nektere kriticke stavy jako pokusy o pristup mimo pridelenou pamet apod.
  if (jednoducheReseni()) // povedlo se...
  {
    return true;
  }
  // nepovedlo se :/
  // zkusime to nasilnejsi metodou... hadanim :)
  else // budeme pokracovat v rekurzi
  {
    while (s.pole[index].zapsane) index++; // hledame dalsi prazdne policko
    random_shuffle(cisla, cisla + 9);// prehazime seznam cisel k vyzkoseni - soucast <algorithm>
    for (int i = 0; i< 9;i++)// vyzkousime vsechny mozna cisla v seznamu
    {
      Sudoku novy(this); // zkopirujeme si stavajici sudoku
      
      if(novy.doplnK(index%9,index/9,cisla[i])) // zkusime cislo doplnit
      {
        // slo to
        if(novy.rKrok(index+1)) // prokud se podari doplnit i dalsi pozici, je vse OK
        {
          s = novy.s; // okopirujeme stav noveho sudoku do puvodniho (tohoto)
          return true; // vse OK
        }
      }
    }
  }
  return false; // rekurze nad neplatnym sudoku vede k chybam...
}

// dopln cislo na pozici manualne
int Sudoku::doplnM (int x, int y, int cislo)
{
  // jsme v mezich hraciho pole?
  if(0 <= x && x < 9)
    if(0 <= y && y < 9)
    {
      if(s.pole[y*9+x].pevne)// neprepisujeme pevna cisla
        return false;
      unsigned int zakazano = s.radky[y] | s.sloupce[x] | s.ctverce[x/3][y/3]; // prusecik mnozin zakazanych hodnot
      if(!((1 << cislo) & ~zakazano)) // je zakazano doplnit cislo na policko?
      {
        s.rozbite = true; // doplnujeme manualne, tudiz toto vede k doplneni a neplatnosti hraci plochy
      }
      s.pole[y*9+x].zapsane = cislo;// zapiseme cislo na uvedenou pozici
      // zakazeme pouzit cislo v radku, sloupci a ctverci
      s.radky[y] |= (1 << cislo);
      s.sloupce[x] |= (1 << cislo);
      s.ctverce[x/3][y/3] |= (1 << cislo);
      s.doplnenych++;
    }
  return false;
}
// dopln cislo na pozici s kontrolou
int Sudoku::doplnK (int x, int y, int cislo)
{
  if(0 <= x && x < 9)
  {
    if(0 <= y && y < 9)
    {
      if(s.pole[y*9+x].pevne)// pevna cisla nelze prepisovat
        return false;
      unsigned int zakazano = s.radky[y] | s.sloupce[x] | s.ctverce[x/3][y/3];
      if((1 << cislo) & ~zakazano) // pokud nelze doplnit, koncime
      {
        s.pole[y*9+x].zapsane = cislo;
        // zakazeme pouzit cislo v radku, sloupci a ctverci
        s.radky[y] |= (1 << cislo);
        s.sloupce[x] |= (1 << cislo);
        s.ctverce[x/3][y/3] |= (1 << cislo);
        s.doplnenych++;
        return true;
      }
    }
  }
  return false;
}

// co je na pozici?
sudoku_policko Sudoku::policko (int x,int y)
{
  if(0 <= x && x < 9)
    if(0 <= y && y < 9)
      return s.pole[y*9+x];
  sudoku_policko err;
  err.error = true;
  return err;
}