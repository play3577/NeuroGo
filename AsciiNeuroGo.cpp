// (C) 2001 by
// Melanie Sippl melanie.sippl@web.de
// Ingo Schasiepen ingo.schasiepen@web.de
// Andre Betz andre.betz@gmx.de

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include "GoBoard.h"
#include "GoPlayer.h"
#include "AsciiNeuroGo.h"
#define  BOARDSIZE 9

void DrawBoard(GoBoard* GoPlay)
{
  int Size = GoPlay->GetSize();

  for(int X=0;X<Size;X++) printf("%2d ",X);
  printf("\n");

  for(int Y=0;Y<Size;Y++)
  {
    for(int X=0;X<Size;X++) printf(" | ");
    printf("\n");
    for(int X=0;X<Size;X++)
    {
      if(GoPlay->GetField(X,Y)==GO_BLACK) printf("-O-");
      else if(GoPlay->GetField(X,Y)==GO_WHITE)   printf("-X-");
      else printf("-+-");
    }
    printf("%2d ",Y);
    printf("\n");
  }
  for(int X=0;X<Size;X++) printf(" | ");
  printf("\n");
}

void GetInput(int& x,int& y)
{
  char buf[10];
  printf("Eingabe X: ");
  gets(buf);
  x = atoi(buf);
  printf("Eingabe Y: ");   
  gets(buf);
  y = atoi(buf);
}

void SaveGame (char *FileName, unsigned char *Game)
{
  ofstream GameFile (FileName,ios::binary|ios::app);
  if (!GameFile)
    {
      cout << "Fehler beim Öffnen!" << endl;
      exit(1);
    }

  if (!GameFile.write ((char*)Game,162*2))
    {
      cout << "Fehler beim Schreiben!" << endl;
      exit(1);
    }
  
}

int AsciiNeuroGo(int argc, char* argv[])
{
	double gain;
	double spread;
	double moment;
	int    middle;
        int oponent;

  if(argc!=8)
  {
	  cout << "Usage: " 
	       << argv[0] 
	       << " "
	       << "<Gegner> "
	       << "<Spielstand> "
	       << "<Lerndatei> " 
	       << "<Middle> "
	       << "<Gain> "
	       << "<Spread> "
	       << "<Momentum> "
	       << endl;
	  cout << "Gegner: 0=Strato 1=Random 2=Paul" << endl;
	  return 1;
  }
  
  cout << argv[0] << " " 
       << argv[1] << " " 
       << argv[2] << " "
       << argv[3] << " "
       << argv[4] << " "
       << argv[5] << " "
       << argv[6] << " " 
       << argv[7] << " "
       << endl;
  
  int zuege = 0;

  oponent = atoi(argv[3]);  
  middle = atoi(argv[4]);
  gain = atof(argv[5]);
  spread = atof(argv[6]);
  moment = atof(argv[7]);
  
  GoBoard* GoPlay = new GoBoard(BOARDSIZE);

  GoPlayer *Black;

  if(oponent==1)
    Black = new GoRandomPlayer(GoPlay, GO_BLACK);
  else if(oponent==0)
    Black = new GoStrategyPlayer(GoPlay,GO_BLACK);
  else 
    Black = new Paul(GoPlay,GO_BLACK);
 
  char *FileName;
  
//  GenBackPropPlayer White(GoPlay,GO_WHITE);
  GoBackPropPlayer9x9A White (GoPlay, GO_WHITE);
//  GoStrategyPlayer Black (GoPlay, GO_BLACK); 
//  gain, spread, momentum[,reset]
  //   White.SetValues(middle,gain,spread,moment,1);
 White.SetValues(middle,gain,spread,moment);

//   int Net[] = {BOARDSIZE*BOARDSIZE,middle,1};
//   White.SetValues(3,Net,gain,spread,moment,0.1);
   
  bool bFirstMove=true;

  struct Field
  {
      unsigned char X,Y;
  };

  Field Game[162];
  unsigned long Move;
  bool bLastMove;
  int WinWhite = 0;
  long lWin;
  
  while(++zuege)
  {
    GoPlay->Init();
    bLastMove  = true;
    bFirstMove = true;
    Move       = 0;
    
    while( (!GoPlay->IsFinish()) && (bLastMove) )
    {
//    if(GoPlay->GetPlayer()==GO_WHITE) printf("Spieler X\n");
//    else                              printf("Spieler O\n");
 //   DrawBoard(GoPlay);

      if (GoPlay->GetPlayer()==GO_BLACK)
      {
	lWin = GoPlay->CountAreas(GO_BLACK);
        bLastMove=Black->MakeMove(bFirstMove, Game[Move].X, Game[Move].Y);
	lWin = GoPlay->CountAreas(GO_BLACK) - lWin;
	White.Learn(lWin,GO_BLACK);
      }
      else
      {
	lWin = GoPlay->CountAreas(GO_WHITE);      
        bLastMove=White.MakeMove(bFirstMove, Game[Move].X, Game[Move].Y);
	lWin = GoPlay->CountAreas(GO_WHITE) - lWin;
	White.Learn(lWin,GO_WHITE);
      }
      Move++;
//      cout << Move << ". Zug" << endl;
      bFirstMove = false;
    }

    if(GoPlay->CountAreas(GO_BLACK)<GoPlay->CountAreas(GO_WHITE))
    {
	    WinWhite++;
    }
    
    cout << "SpielNr: " << zuege << " Gewonnen Neuro: " << (double)WinWhite/(double)zuege << endl;

    if(zuege%5==0)
    {
 	  char Buf[255];
          sprintf(Buf,"%s.%d",argv[2],zuege);	  
	  SaveGame (argv[1], (unsigned char*)Game);
          White.Final(Buf);
    }	
  }
  return 0;
}

