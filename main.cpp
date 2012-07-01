#include <iostream> // vstup/vystup
#include <string> // prace s retezci
#include <set> // pro zpracovani parametru pomoci mnoziny
#include "sudoku.h"
#include "interactive.h"
using namespace std; // pouzijeme standardni jmenny prostor knihovny C++

enum { INTERAKTIVNI, FILTR, RESITEL, GENERATOR};


/**
 * fce pro vypsani navodu k pouziti
 */
void printUsage( void )
{
  cout << "Program pro generovani a reseni sudoku." << endl
  << "  Parametry:" << endl
  << "  i - interaktivni hra. pouzijte pokud nechcete aby program okamzite vyresil sudoku" << endl
  << "  f - filtr - pouze kontroluje spravnost vstupu" << endl
  << "  g - generator - pouze generuje nahodne sudoku" << endl
  << "  s - vyresi sudoku - vychozi rezim" << endl
  << "  pri uvedeni kombinace techto parametru je chovani programu nedefinovano" << endl
  << "  Format parametru:" << endl
  << "  Parametry se zapisuji tak, ze pred ne uvedete pomlcku. Bez uvedeni pomlcky jsou brany jako nazvy souboru. " << endl
  << "  Prvni nazev souboru se bere jeko vstupni. pokud neni uveden, program vygeneruje nahodne sudoku." << endl;
}
/**
 * Vstupni bod, ziskani parametru programu
 * argc - pocet parametru
 * argv - pole nulou ukoncenych retezcu - parametry
 */
int main(int argc, char* argv[])
{
  // soubory - vstup a vystup
  string strSouborVstup;
  int iSouborIndex = 0;
  // mnozina parametru extrahovanych z prikazove radky
  set <char> setMnozinaParametru;
  int rezim = RESITEL; //bez parametru program pouze vyresi sudoku
  // jediny parametr - nazev volaneho programu. vypiseme navod k pouziti
  if(argc == 1)
  {
    printUsage();
    return 1;
  }
  else // vice parametru, projdeme a zjistime co se po nas chce
  {
    for (int i = 1; i< argc; i++)
    {
      string zpracuj (argv[i]);
      if(zpracuj[0] == '-') // jedna se o parametry
      {
        string::iterator paramIterator = zpracuj.begin()+1; // nebereme v potaz pomlcku na zacatku
        while (paramIterator != zpracuj.end()) // projdeme retezec iteratorem, zjistime pismena parametru
        {
          setMnozinaParametru.insert(*paramIterator); // vlozime parametr do mnoziny
          paramIterator++;// prejdeme na dalsi
        }
      }
      else // jedna se o soubor
      {
        if(iSouborIndex == 0)
        {
          strSouborVstup = zpracuj;
          iSouborIndex++;
        }
        // bereme jen jeden nazev souboru, zbytek je balast
      }
    }
    /*
     * projdeme nasbirane parametry v mnozine a nastavime podle nich beh programu
     */
    for(set<char>::iterator paramIterator = setMnozinaParametru.begin();paramIterator != setMnozinaParametru.end(); paramIterator++)
    {
      switch (*paramIterator)
      {
      case 'i':
        rezim = INTERAKTIVNI;
        break;
      case 'f':
        rezim = FILTR;
        break;
      case 's':
        rezim = RESITEL;
        break;
      case 'g':
        rezim = GENERATOR;
        break;
      default:
        cerr << "Neznamy parametr: '" << *paramIterator << "'" << endl;
      }
    }
  }
  // podle rezimu pokracujeme
  Sudoku * sa;
  switch(rezim)
  {
    case FILTR:
      if(strSouborVstup != "") // je vstupni soubor?
      {
        // nacteme soubor se vstupem
        Sudoku s (strSouborVstup);
        if( s.getStav() != _chyba) 
        {
          if(s.rekurzivniReseni()) // zkusime vyresit
          {
            cout << "Sudoku je v poradku" << endl;
            return 0;
          }
          else
          {
            cout << "Sudoku neni resitelne" << endl;
            return 2;
          }
        }
        else
        {
          cerr << "chyba pri cteni souboru." << endl;
          return 1;
        }
      }
      else
      {
        cerr << "Filtr potrebuje vstupni soubor." << endl;
        return 1;
      }
      break;
    // interakce s uzivatelem
    case INTERAKTIVNI:
      sa = new Sudoku(-1); // pouzita dynamicka pamet na pracovni sudoku
      if(strSouborVstup != "")// mame urcen vstupni soubor?
      {
        // ano, pouzijeme sudoku v nem jako zadani
        Sudoku s (strSouborVstup);
        if( s.getStav() != _chyba)  // pokud se soubor dal nacist
        {
          if(s.rekurzivniReseni())// zkusime sudoku vyresit
          {
            s.restart();// zbavime se resenim vyplnenych policek
            sa->s = s.s;// a okopirujeme do sudoku v dynamicke pameti
            
            return MainLoop(sa);// predani rizeni interaktivni smycce v interactive.cpp
          }
          else// neresitelne - nekde je neco spatne
          {
            cerr << "Spatne zadane sudoku." << endl;
            delete sa;
            return 1;
          }
        }
        else// nepovedlo se nacist soubor
        {
          cerr << "Spatne zadane sudoku." << endl;
          delete sa;
          return 1;
        }
      }
      else
      {
        // vstupni soubor nemame -> generujeme jednoduche sudoku aby si uzivatel mohl zahrat :)
        sa = new Sudoku( 0 );// 0 = pouzijeme systemovy cas jako zdroj 'nahody'
        return MainLoop(sa);// a jedeme :)
      }
      delete sa;// uklid
      break;
    // automaticke reseni
    case RESITEL:
      if(strSouborVstup != "")// mame soubor?
      {
        Sudoku s (strSouborVstup);// nacteme
        if( s.getStav() != _chyba) // zkontrolujeme zna nedoslo k chybe
        {
          if(s.rekurzivniReseni()) // nedoslo, je resitelne?
          {
            s.print();// ano, vyhodime na konzoli reseni
            return 0;
          }
          else// nejde to
          {
            cout << "Sudoku neni resitelne" << endl;
            return 1;
          }
        }
        else // soubor je podivny
        {
          cerr << "chyba pri cteni souboru." << endl;
          return 1;
        }
      }
      else
      {
        cerr << "Resitel potrebuje vstupni soubor." << endl;
        return 1;
      }
      break;
      // generovani jednoducheho sudoku, je mozne presmerovat do souboru
    case GENERATOR:
      Sudoku s ( 0 );
      s.print();
      break;
  }
  return 0;
} 
