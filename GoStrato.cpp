// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
#include "GoPlayer.h"
#include <time.h>
#include <stdlib.h>


/*****************************************************************************
*  strategy: make groups and areas bigger
*****************************************************************************/

GoStrategyPlayer::GoStrategyPlayer(GoBoard* pBoard, GoFieldDesc Color)
{
  m_pBoard = pBoard;
  m_Color  = Color;
  m_ulMoveCount = 0;
  m_ulFieldSize = m_pBoard->GetSize();
       
  m_xFreeFields = new sField [m_ulFieldSize*m_ulFieldSize];
  m_xStratoFields = new sField [m_ulFieldSize*m_ulFieldSize];
  m_ulOpenDuration = m_ulFieldSize/2;

  srandom(time(0));
		 
}

/*****************************************************************************
 *  strategy: make groups and areas bigger  
 *****************************************************************************/

GoStrategyPlayer::~GoStrategyPlayer()
{
	    delete[] m_xFreeFields;
	    delete[] m_xStratoFields;
}

/*****************************************************************************
 *  Opening  
 *****************************************************************************/
bool GoStrategyPlayer::Opening()
{
  unsigned long ;     
  double Area;

  do
  {
    Area = 0.2*(double)random()/RAND_MAX+0.2;
    if ((random()>>10)&1==1)
      Area+=0.4;

    m_ulNewX = (unsigned long)(Area*m_ulFieldSize);

    Area = 0.2*(double)random()/RAND_MAX+0.2;

    if ((random()>>10)&1==1)
      Area+=0.4;

    m_ulNewY = (unsigned long)(Area*m_ulFieldSize);

  }while(m_pBoard->TestField((long)m_ulNewX, (long)m_ulNewY)!=GO_OK);
  
  return true;

}

	
/*****************************************************************************
 *  Increase Areas
 *****************************************************************************/
 
bool GoStrategyPlayer::IncreaseArea()
{
  unsigned long ulNumOfFreeFields=0;
  unsigned long ulNumOfStratoFields=0;
  unsigned long ulNewField;
  unsigned long max_lib=2;
  long          DeltaX, DeltaY;
  sField        Dir[4];
  unsigned long DirCntr=0;

  for (unsigned long y=0; y<m_ulFieldSize; y++)  
    for (unsigned long x=0; x<m_ulFieldSize; x++)  
      if (m_pBoard->GetField(x,y)==m_Color)
	{
	  if (m_pBoard->GetLocalLib(x,y) > max_lib)
	    {
	      max_lib = m_pBoard->GetLocalLib(x,y);
	      ulNumOfStratoFields=0;
	    }
	  if (m_pBoard->GetLocalLib(x,y)== max_lib)
	    {
	      m_xStratoFields[ulNumOfStratoFields].x=x;
	      m_xStratoFields[ulNumOfStratoFields].y=y;
	      ulNumOfStratoFields++;
	    }
	}
      else if (m_pBoard->TestField(x,y)== GO_OK)
	{
	  m_xFreeFields[ulNumOfFreeFields].x=x;
	  m_xFreeFields[ulNumOfFreeFields].y=y;
	  ulNumOfFreeFields++;
	}

  if ( (ulNumOfStratoFields==0) || ( (long)(m_ulOpenDuration*(double)random()/RAND_MAX)==0) )
    {
         if (ulNumOfFreeFields==0)
	   return false;

	 ulNewField = random()%ulNumOfFreeFields;
	 m_ulNewX = (unsigned char)m_xFreeFields[ulNewField].x;
	 m_ulNewY = (unsigned char)m_xFreeFields[ulNewField].y;
	 return true;
     
    }
  else
    {
      ulNewField = random()%ulNumOfStratoFields;

      m_ulNewX = (unsigned char)m_xStratoFields[ulNewField].x;
      m_ulNewY = (unsigned char)m_xStratoFields[ulNewField].y;

      if (m_pBoard->TestField(m_ulNewX+1,m_ulNewY)== GO_OK)
	{
	  Dir[DirCntr].x=m_ulNewX+1;
	  Dir[DirCntr].y=m_ulNewY;
	  DirCntr++;
	}
      if (m_pBoard->TestField(m_ulNewX-1,m_ulNewY)== GO_OK)
	{
	  Dir[DirCntr].x=m_ulNewX-1;
	  Dir[DirCntr].y=m_ulNewY;
	  DirCntr++;
	}
      if (m_pBoard->TestField(m_ulNewX,m_ulNewY+1)== GO_OK)
	{
	  Dir[DirCntr].x=m_ulNewX;
	  Dir[DirCntr].y=m_ulNewY+1;
	  DirCntr++;
	}
      if (m_pBoard->TestField(m_ulNewX,m_ulNewY-1)== GO_OK)
	{
	  Dir[DirCntr].x=m_ulNewX;
	  Dir[DirCntr].y=m_ulNewY-1;
	}

      DirCntr = (long)(max_lib*(double)random()/RAND_MAX);
      m_ulNewX = Dir[DirCntr].x;
      m_ulNewY = Dir[DirCntr].y;

      /*
      DeltaX=1-2*(long)(2*(double)random()/RAND_MAX);
      DeltaY=1-2*(long)(2*(double)random()/RAND_MAX);
      cout << DeltaX << "/" << DeltaY << endl;

      m_ulNewX = (unsigned char)m_xStratoFields[ulNewField].x;
      m_ulNewY = (unsigned char)m_xStratoFields[ulNewField].y;

      if (m_pBoard->TestField((long)m_ulNewX+DeltaX, (long)m_ulNewY)==GO_OK)
	  m_ulNewX+=DeltaX;
      else
	if (m_pBoard->TestField((long)m_ulNewX-DeltaX,(long) m_ulNewY)==GO_OK)
	  m_ulNewX-=DeltaX;
      else
	if (m_pBoard->TestField((long)m_ulNewX, (long)m_ulNewY+DeltaY)== GO_OK)
	  m_ulNewY+=DeltaY;
      else
	if (m_pBoard->TestField((long)m_ulNewX,(long) m_ulNewY-DeltaY) == GO_OK)
	  m_ulNewY-=DeltaY;
      */
      return true;
    }  
} 

     
/*****************************************************************************
 *  strategy: make groups and areas bigger  
 *****************************************************************************/
 
bool GoStrategyPlayer::MakeMove(bool bFirstMove,
                                unsigned char &ucX,
		                unsigned char &ucY,
		                bool bLearnMode)
{
  bool result; 
  
  m_ulMoveCount++;
  
  if(m_ulMoveCount <= m_ulOpenDuration)
  {
      result = Opening();
      if (result == false)
	result = IncreaseArea();
  }
  else
  {
      result = IncreaseArea();
  }

  if (result)
    m_pBoard->SetField (m_ulNewX, m_ulNewY);
  
  return result;
}
