// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
#include "GoPlayer.h"
#include <time.h>
#include <stdlib.h>

GoBackPropPlayer9x9B::GoBackPropPlayer9x9B (GoBoard* pBoard, GoFieldDesc Color,const char *FileName)
{
   m_pBoard = pBoard;
   m_Color  = Color;
   m_OppColor = (Color==GO_BLACK?GO_WHITE:GO_BLACK);
   m_ulFieldSize = m_pBoard->GetSize();

   net = NULL;

   if (FileName)
   {
/*
       if(net.load(FileName)==1)
       {
               cout << "net loaded" << endl;
       }
*/
   }

}

void GoBackPropPlayer9x9B::SetValues(int middle,double gain,double spread,double momentum,double seedrand)
{
  int lay[] = {81,middle,1};
  if(net!=NULL) delete net;
  net = new myBackpropNet(3,lay,gain,spread,momentum);
  if(seedrand!=0)
  {
  }
}

bool GoBackPropPlayer9x9B::MakeMove(bool bFirstMove, 
                                   unsigned char &ucX,
                                   unsigned char &ucY,
                                   bool bLearnMode = false)
{
  double desir[1];
  double out[1];
  double In[81];
  double dMax = -1;
  unsigned long ulMaxXPos;
  unsigned long ulMaxYPos;
  long          lDiff;

  if (!bFirstMove && bLearnMode)
    {
      
      lDiff = (long)m_pBoard->CountAreas (m_Color) - m_ResultAreas;
 
      if(lDiff > 0)    //spreading area
      {
        desir[0] = 1;
        net->Desired(desir);
        net->Learn();
      }
      
      if(lDiff < 0)  //decreasing area
      {
        desir[0] = 0;
        net->Learn();
      }
    }

    m_ResultAreas = m_pBoard->CountAreas (m_Color);

    // mache Zug
    
    for (unsigned long y=0; y<m_ulFieldSize; y++)
      for (unsigned long x=0; x<m_ulFieldSize; x++)
        {
          if (m_pBoard->GetField (x,y)==m_Color)
            In[y*m_ulFieldSize+x] = 1;
          else if (m_pBoard->GetField (x,y)==m_OppColor)
            In[y*m_ulFieldSize+x] = -1;
          else
            In[y*m_ulFieldSize+x] = 0;
        }
    
    for (unsigned long y=0; y<m_ulFieldSize; y++)
       for (unsigned long x=0; x<m_ulFieldSize; x++)
              if (m_pBoard->TestField(x,y)==GO_OK)
              {
                In[y*m_pBoard->GetSize()+x] = (m_Color==GO_BLACK?-1:1);
		net->Input(In);
                net->Calc();
                net->Output(out);
                if(dMax<out[0]) 
                  {
                    dMax = out[0];
                    ulMaxXPos = x;
                    ulMaxYPos = y;
                  }
                In[y*m_ulFieldSize+x] = 0;
              }
    
    if (dMax==-1)
      return false;

    m_pBoard->SetField(ulMaxXPos,ulMaxYPos);
    In[ulMaxYPos*m_ulFieldSize+ulMaxXPos] = m_Color;
    net->Calc();
    ucX = (unsigned char)ulMaxXPos;
    ucY = (unsigned char)ulMaxYPos;
//    cout << "X: " << ulMaxXPos << "Y: " << ulMaxYPos << endl;

  return true;
}

void GoBackPropPlayer9x9B::Final (char *FileName)
{
  net->Save(FileName);
}

GoBackPropPlayer9x9B::~GoBackPropPlayer9x9B()
{
}
