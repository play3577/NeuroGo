// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
#include "GoPlayer.h"
#include <time.h>

Paul::Paul (GoBoard* pBoard, GoFieldDesc Color)
{
  m_pBoard = pBoard; 
  m_Color  = Color;
  m_OppColor = (Color==GO_WHITE?GO_BLACK:GO_WHITE);
  m_ulFieldSize = m_pBoard->GetSize();	    
     
  m_pTestField  = new bool    [m_ulFieldSize*m_ulFieldSize];
  m_pAttackList = new sAttack [m_ulFieldSize*m_ulFieldSize];
  m_xFreeFields = new sField  [m_ulFieldSize*m_ulFieldSize];
     
  srandom(time(0));
}

Paul::~Paul()
{
  delete[] m_pTestField;
  delete[] m_pAttackList;
  delete[] m_xFreeFields;
}

int Paul::PaulSort (const void *A, const void *B)
{
  if ((*(sAttack*)A).ulLibs==(*(sAttack*)B).ulLibs)
    return (*(sAttack*)A).ulGrpSize-(*(sAttack*)B).ulGrpSize;
  else
    return (*(sAttack*)A).ulLibs-(*(sAttack*)B).ulLibs;
}

bool Paul::MakeMove(bool bFirstMove, 
                    unsigned char &ucX,
                    unsigned char &ucY,
                    bool bLearnMode = false)  
{
  
  long lX,lY,lMark;
  unsigned long ulStoneX, ulStoneY;
  unsigned long ulLibX, ulLibY;
  unsigned long ulNrOfAttackFields=0;
  bool *pBoardPtr;
  
  pBoardPtr = m_pTestField;
  for (lX=m_ulFieldSize*m_ulFieldSize; lX>0; lX--)
    *pBoardPtr++ = false;

  pBoardPtr = m_pTestField;
  for (lY=0; lY<m_ulFieldSize; lY++)
    for (lX=0; lX<m_ulFieldSize; lX++)
      if ( (m_pBoard->GetField(lX,lY)==m_OppColor) &&
           (!*(pBoardPtr++)) )
      {
          for (lMark=0; lMark<m_pBoard->GetGroupSize(lX,lY); lMark++)
          {
              m_pBoard->GetGroupStone (lX,lY,lMark,ulStoneX,ulStoneY);
              m_pTestField[ulStoneY*m_ulFieldSize+ulStoneX]=true;
          }
          m_pBoard->GetFirstLib (lX,lY,ulLibX,ulLibY);
          if (m_pBoard->TestField (ulLibX,ulLibY)==GO_OK)
          {
              m_pAttackList[ulNrOfAttackFields].ulGrpSize = m_pBoard->GetGroupSize(lX,lY);
              m_pAttackList[ulNrOfAttackFields].ulLibs    = m_pBoard->GetLiberties(lX,lY);
              m_pAttackList[ulNrOfAttackFields].Pos.x     = ulLibX;
              m_pAttackList[ulNrOfAttackFields].Pos.y     = ulLibY;
              ulNrOfAttackFields++;
          }
          
      }
  
  if (ulNrOfAttackFields==0)
  {
      unsigned long ulNumOfFreeFields=0;
      unsigned long ulNewField;
      
      for (unsigned long y=0; y<m_ulFieldSize; y++)  
        for (unsigned long x=0; x<m_ulFieldSize; x++)  
          if (m_pBoard->TestField((long)x,(long)y)==GO_OK)
          {
              m_xFreeFields[ulNumOfFreeFields].x=x;
              m_xFreeFields[ulNumOfFreeFields].y=y;
              ulNumOfFreeFields++;
          }
      
      if (ulNumOfFreeFields==0)
        return false;
      
      ulNewField = random()%ulNumOfFreeFields;
      
      m_pBoard->SetField(m_xFreeFields[ulNewField].x,m_xFreeFields[ulNewField].y);
      ucX = (unsigned char)m_xFreeFields[ulNewField].x;
      ucY = (unsigned char)m_xFreeFields[ulNewField].y;
      return true;
  }
  else
  {
      qsort ((void*)m_pAttackList, ulNrOfAttackFields,sizeof(sAttack),&PaulSort);
      m_pBoard->SetField (m_pAttackList[0].Pos.x,m_pAttackList[0].Pos.y);
      ucX = (unsigned char)m_pAttackList[0].Pos.x;
      ucY = (unsigned char)m_pAttackList[0].Pos.y;
      return true;
  }  
}

