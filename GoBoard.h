// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de

#ifndef GOBOARD
#define GOBOARD

#include <iostream>
#include "GoGroup.h"

enum GoError
  {
    GO_OK,GO_OCCUPIED,GO_SUICIDE,GO_KO,GO_ERROR,GO_OUT_OF_FIELD
  };

class GoBoard
{
private:
  enum eMarker {CLEAR,MARKED,SET,NOTSET};
      
  struct sField
  {
    GoGroup *Group;
    eMarker Marker;  // für CountAreas
  };
  
  sField *m_pField;

  unsigned long m_ulEdgeLength;
  unsigned long m_ulRealLength;
  unsigned long m_ulBoardSize;
  int           m_ulMoveNumber;

  // needed for ko-check
  unsigned long m_ulLastPos;
  unsigned long m_ulLastKills;
  unsigned long m_ulLastKillPos;

  // needed for finding areas
  bool m_bOwnColUsed;
  bool m_bOppColUsed;
  bool m_bNBorderUsed;
  bool m_bSBorderUsed;
  bool m_bWBorderUsed;
  bool m_bEBorderUsed;  
  unsigned long m_ulAreaSize;

  GoFieldDesc m_xPlayer;

public:
  GoBoard (unsigned long ulEdgeLength);
  ~GoBoard ();
  void Init();
  GoError SetField (unsigned long x, unsigned long y);
  GoError TestField (long x, long y);
  unsigned long GetLocalLib (unsigned long x, unsigned long y);

  void GetGroupStone (unsigned long x, unsigned long y,
                      unsigned long StoneNr,
                      unsigned long &StoneX, unsigned long &StoneY);
  void GetFirstLib (unsigned long x, unsigned long y,
                    unsigned long &LibX, unsigned long &LibY);

  unsigned long CountAreas (GoFieldDesc Color);
  
  void ClearMarker ();
  void MarkArea (unsigned long ulXPos, unsigned long ulYPos,
                 GoFieldDesc Color);
  inline GoFieldDesc GetField (unsigned long x, unsigned long y)
     { return m_pField[(y+1)*m_ulRealLength+x+1].Group->GetStatus();}
  inline bool CheckArea (unsigned long x, unsigned long y)
     { return m_pField[(y+1)*m_ulRealLength+x+1].Marker == SET;}
  
  inline int GetSize() {return m_ulEdgeLength;}
  inline int IsFinish() {return (--m_ulMoveNumber)==0;}
  inline GoFieldDesc GetPlayer() { return m_xPlayer;}
  inline int GetGroupSize (unsigned long x, unsigned long y)
     { return m_pField[(y+1)*m_ulRealLength+x+1].Group->GetGroup()->GetGroupSize(); }
  inline int GetLiberties (unsigned long x, unsigned long y)
     { return m_pField[(y+1)*m_ulRealLength+x+1].Group->GetGroup()->GetLiberties(); }

  void DrawField ();
  void PrintInfo (int X, int Y);
};
#endif
  
