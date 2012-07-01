#include "interactive.h"
#include <stdio.h>
#include <stdlib.h>

#include <string>

void printPrompt ();
void printHelp ();
int printS ( Sudoku * s, int u, int v );


// vypsani stavu Sudoku - detailnejsi nez Sudoku::print()
// u,v = pozice kurzoru
int printS ( Sudoku * s, int u, int v )
{
  for(int y=0;y<9;y++) // projdeme radky
  {
    // formatovani car podle toho, ktery radek se zpracovava
    if(y == 3 || y == 6)
    {
      cout << "---------+--------+---------" << endl;
    }
    else
    {
      cout << "+--+--+--|--+--+--|--+--+--+" << endl;
    }
    for(int x=0;x<9;x++)// projdeme bunky v radku
    {
      if (s->s.pole[y*9+x].zapsane) // je tam neco?
      {
        cout << "|";
        cout<< s->s.pole[y*9+x].zapsane; // vypiseme
        if(s->s.pole[y*9+x].pevne) // pridame za to x, pkud je to pevna hodnota
        {
          if(u == x && v == y)
            cout << "X";
          else cout << ".";
        }
        else cout << " ";
      }
      else
      {
        if(u == x && v == y)
          cout<< "|O<";
        else
          cout<< "|. ";
      }
    } 
    cout << "|";
    if(v == y) cout << "<--"; // kresleni ukazatele polohy
    cout << endl;
  }
  cout << "+--+--+--|--+--+--|--+--+--+" << endl;
  cout << " ";
  // kresleni ukazatele polohy
  for (int i = 0;i< u*3; i+=3)
  {
    cout << "   ";
  }
  cout << "^" << endl;
}

void printHelp ()
{
  cout << "w,s,a,d = pohyb v mrizce" << endl;
  cout << "1-9     = zapsani cisla" << endl;
  cout << "x,0     = smazani cisla" << endl;
  cout << "exit    = ukonceni programu" << endl;
  cout << "solve   = vyreseni sudoku z aktualniho stavu - pri spatnem vyplneni nefunguje" << endl;
  cout << "rsolve  = vyreseni sudoku z vychoziho stavu" << endl;
  cout << "print   = vypise aktualni stav" << endl;
}

void printVitezstvi (Sudoku * s)
{
  if(s->s.doplnenych == 81 )
  {
    if(!s->s.rozbite)
    {
      cout << "Reseni je spravne!" << endl;
    }
    else
    {
      cout << "Neco je opravdu velmi spatne..." << endl;
    }
  }
}
void printPrompt ()
{
  cout << "prikaz >";
}
int MainLoop ( Sudoku * s )
{
  int x = 4,y = 4; // nastavime kurzor doprostred pole
  printS(s,x,y);
  cout << "Pro ziskani napovedy pouzijte prikaz 'help'." << endl;
  printPrompt();
  while (true)
  {
    string prikaz;
    cin >> prikaz;
    cin.clear();
    cin.ignore(1000, '\n');
    if ( prikaz == "help")
    {
      printHelp();
      printPrompt();
    }
    else if (prikaz == "print")
    {
      printS(s,x,y);
      printPrompt();
    }
    else if (prikaz == "solve")
    {
      if (!s->rekurzivniReseni())
        cout << "Neco je velmi spatne..." << endl;
      printVitezstvi(s);
      printS(s,x,y);
      printPrompt();
    }
    else if (prikaz == "rsolve")
    {
      s->restart();
      if (!s->rekurzivniReseni())
        cout << "Neco je velmi spatne..." << endl;
      printVitezstvi(s);
      printS(s,x,y);
      printPrompt();
    }
    else if (prikaz == "w")
    {
      y--;
      if (y < 0) y+=9;
      printS(s,x,y);
      printPrompt();
    }
    else if (prikaz == "s")
    {
      y++;
      if (y > 8) y-=9;
      printS(s,x,y);
      printPrompt();
    }
    else if (prikaz == "a")
    {
      x--;
      if (x < 0) x+=9;
      printS(s,x,y);
      printPrompt();
    }
    else if (prikaz == "d")
    {
      x++;
      if (x > 8) x-=9;
      printS(s,x,y);
      printPrompt();
    }
    // mazani cisla
    else if (prikaz == "." || prikaz == "x" || prikaz == "0")
    {
      s->smazat(x,y);
      printS(s,x,y);
      printPrompt();
    }
    // doplnovani cisla
    else if(prikaz == "1" || prikaz == "2" || prikaz == "3" || prikaz == "4" || prikaz == "5" || prikaz == "6" || prikaz == "7" || prikaz == "8" || prikaz == "9")
    {
      int i = atoi(prikaz.c_str());
      s->doplnM(x,y,i);
      printS(s,x,y);
      printVitezstvi(s);
      printPrompt();
    }
    else if(prikaz == "exit")
    {
      cout << endl;
      return 0;
    }
    else if(prikaz == "reset")
    {
      s->restart();
      printS(s,x,y);
      printPrompt();
    }
    else
    {
      printHelp();
      printPrompt();
    }
  }
  return 0;
} 
