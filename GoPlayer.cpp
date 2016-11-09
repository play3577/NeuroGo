// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de

#include "GoPlayer.h"
#include <time.h>
#include <stdlib.h>

/*****************************************************************************
 * Parent Class
 ****************************************************************************/

GoPlayer::GoPlayer()
{
}

GoPlayer::~GoPlayer()
{
}

/*****************************************************************************
 * Random
 ****************************************************************************/

GoRandomPlayer::GoRandomPlayer(GoBoard* pBoard, GoFieldDesc Color)
{
  m_pBoard = pBoard; 
  m_Color  = Color;
  m_ulFieldSize = m_pBoard->GetSize();
	    
  m_xFreeFields = new sField [m_ulFieldSize*m_ulFieldSize];
     
  srandom(time(0));
}

GoRandomPlayer::~GoRandomPlayer()
{
    delete[] m_xFreeFields;
}

bool GoRandomPlayer::MakeMove(bool bFirstMove, 
			      unsigned char &ucX,
			      unsigned char &ucY,
			      bool bLearnMode = false)  
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

//  cout << m_xFreeFields[ulNewField].x << "," << m_xFreeFields[ulNewField].y << endl;
  m_pBoard->SetField(m_xFreeFields[ulNewField].x,m_xFreeFields[ulNewField].y);
  ucX = (unsigned char)m_xFreeFields[ulNewField].x;
  ucY = (unsigned char)m_xFreeFields[ulNewField].y;
  return true;
}

/*****************************************************************************
 * Back Propagation
 ****************************************************************************/

GoBackPropPlayer9x9A::GoBackPropPlayer9x9A (GoBoard* pBoard, GoFieldDesc Color,const char *FileName)
{
   m_pBoard = pBoard;
   m_Color  = Color;
   m_OppColor = (Color==GO_BLACK?GO_WHITE:GO_BLACK);
   m_ulFieldSize = m_pBoard->GetSize();

   if (FileName)
   {
       if(net.load(FileName)==1)
       {
	       cout << "net loaded" << endl;
       }
   }
}

void GoBackPropPlayer9x9A::SetValues(double gain,double spread,double momentum,double seedrand)
{
  net.gain = gain;
  net.spread = spread;
  net.momentum = momentum;
  if(seedrand!=0)
  {
	  net.seedrand = seedrand;
	  net.reset();
  }
}

bool GoBackPropPlayer9x9A::MakeMove(bool bFirstMove, 
				   unsigned char &ucX,
				   unsigned char &ucY,
				   bool bLearnMode = false)
{
  double dMax = -1;
  unsigned long ulMaxXPos;
  unsigned long ulMaxYPos;
  long          lDiff;

  if (!bFirstMove && bLearnMode)
    {
      
      lDiff = (long)m_pBoard->CountAreas (m_Color) - m_ResultAreas;
 
      if(lDiff > 0)    //spreading area
      {
	net.desired (1);
	net.backpropagate ();
      }
      
      if(lDiff < 0)  //decreasing area
      {
	net.desired (0);
	net.backpropagate ();
      }
    }

    m_ResultAreas = m_pBoard->CountAreas (m_Color);

    // mache Zug
    
    for (unsigned long y=0; y<m_ulFieldSize; y++)
      for (unsigned long x=0; x<m_ulFieldSize; x++)
	{
	  if (m_pBoard->GetField (x,y)==m_Color)
	    net.input (y*m_ulFieldSize+x) = 1;
	  else if (m_pBoard->GetField (x,y)==m_OppColor)
	    net.input (y*m_ulFieldSize+x) = -1;
	  else
	    net.input (y*m_ulFieldSize+x) = 0;
	}
    
    for (unsigned long y=0; y<m_ulFieldSize; y++)
       for (unsigned long x=0; x<m_ulFieldSize; x++)
	      if (m_pBoard->TestField(x,y)==GO_OK)
	      {
		net.input(y*m_pBoard->GetSize()+x) = (m_Color==GO_BLACK?-1:1);
		net.feedforward();
		double dOut = net.output(0); 
		if(dMax<dOut) 
		  {
		    dMax = dOut;
		    ulMaxXPos = x;
		    ulMaxYPos = y;
		  }
		net.input(y*m_ulFieldSize+x) = 0;
	      }
    
    if (dMax==-1)
      return false;

    m_pBoard->SetField(ulMaxXPos,ulMaxYPos);
    net.input(ulMaxYPos*m_ulFieldSize+ulMaxXPos) = m_Color;
    net.feedforward();
    ucX = (unsigned char)ulMaxXPos;
    ucY = (unsigned char)ulMaxYPos;
//    cout << "X: " << ulMaxXPos << "Y: " << ulMaxYPos << endl;

  return true;
}

void GoBackPropPlayer9x9A::Final (char *FileName)
{
  net.save(FileName);
}

GoBackPropPlayer9x9A::~GoBackPropPlayer9x9A()
{
}

	      
