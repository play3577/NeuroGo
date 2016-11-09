// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
#include <iostream.h>
#include <stdlib.h>
#include <time.h>
#include "GoPlayer.h"


GenBackPropPlayer::GenBackPropPlayer(GoBoard* pBoard, GoFieldDesc Color,const char *FileName)
{
   m_pBoard = pBoard;
   m_Color  = Color;
   m_OppColor = (Color==GO_BLACK?GO_WHITE:GO_BLACK);
   m_ulFieldSize = m_pBoard->GetSize();
   m_pdField = new  double[m_ulFieldSize*m_ulFieldSize];
   m_pdNotField = new  double[m_ulFieldSize*m_ulFieldSize];
   net = NULL;		     

   srandom(time(0)); 
   
   if (FileName)
   {
	net = new myBackpropNet(FileName);	   
      	cout << "net loaded" << endl;
  }
}

void GenBackPropPlayer::SetValues(int Lay,int* Neu, double gain,double spread,double momentum,double seedrand)
{
    Neu[0] = m_ulFieldSize*m_ulFieldSize;
    Neu[Lay-1] = 1;
    if(net!=NULL) delete net;
    net = new myBackpropNet(Lay,Neu,gain,spread,momentum);
}

bool GenBackPropPlayer::MakeMove(bool bFirstMove,unsigned char &ucX,unsigned char &ucY,bool bLearnMode)
{
	unsigned long x,y;
	double dOut[1];
	double dMax = -1.0;
	unsigned long ulMaxXPos;
	unsigned long ulMaxYPos;

	for(y=0; y<m_ulFieldSize; y++)
	   for (x=0; x<m_ulFieldSize; x++)
	   {
		if (m_pBoard->GetField (x,y)==m_Color)
			m_pdField[y*m_ulFieldSize+x] = 1.0;
	        else if (m_pBoard->GetField (x,y)==m_OppColor)
			m_pdField[y*m_ulFieldSize+x] = -1.0;
		else
			m_pdField[y*m_ulFieldSize+x] = 0.0;
	  }

	for(y=0; y<m_ulFieldSize; y++)
		for(x=0; x<m_ulFieldSize; x++)		    
		{
			if (m_pBoard->TestField(x,y)==GO_OK)
			{
				m_pdField[y*m_ulFieldSize+x] = (m_Color==GO_BLACK?-1.0:1.0);
				net->Input(m_pdField);
        	           	net->Calc();
                		net->Output(dOut);
                		if(dMax<dOut[0])
		                {
			        	dMax = dOut[0];
			                ulMaxXPos = x;
		        	        ulMaxYPos = y;
		                }
        	        	m_pdField[y*m_ulFieldSize+x] = 0.0;
			}
              	}

	if(dMax==-1) return false;

	m_pBoard->SetField(ulMaxXPos,ulMaxYPos);
	ucX = (unsigned char)ulMaxXPos;
	ucY = (unsigned char)ulMaxYPos;
		
	return true;	
}

bool GenBackPropPlayer::Learn(long lDiff, GoFieldDesc Color)
{
	double desir[1];
	double Notdesir[1];
	unsigned long x,y;

	desir[0] = 0.0;
	Notdesir[0] = 0.0;
	if(Color==m_Color)
	{
		if(lDiff>=0) desir[0] = 1.0, Notdesir[0] = -0.5;
		else cout << "Fehler!!" << endl;
	}
	else
	{
		if(lDiff>=0) desir[0] = -0.5, Notdesir[0] = 0.5;
		else cout << "Fehler!!" << endl;
	}

	//normales Brett	
	 for(y=0; y<m_ulFieldSize; y++)
            for (x=0; x<m_ulFieldSize; x++)
            {
                      if (m_pBoard->GetField (x,y)==m_Color)
                             m_pdField[y*m_ulFieldSize+x] = 1.0, m_pdNotField[y*m_ulFieldSize+x] = -1.0;
                      else if (m_pBoard->GetField (x,y)==m_OppColor)
                             m_pdField[y*m_ulFieldSize+x] = -1.0, m_pdNotField[y*m_ulFieldSize+x] = 1.0;
                      else
                             m_pdField[y*m_ulFieldSize+x] = 0.0, m_pdNotField[y*m_ulFieldSize+x] = 0.0;;
            }

	 if(lDiff>0)
	 {
	    net->Input(m_pdField);
	    net->Calc();
	    net->Desired(desir);
	    net->Learn();
	 }

	 /*
	 net->Input(m_pdNotField);
	 net->Calc();
	 net->Desired(Notdesir);
	 net->Learn();

	 // an der Winkelhalbierenden gespiegeltes Brett
         for(y=0; y<m_ulFieldSize; y++)
		 for (x=0; x<m_ulFieldSize; x++)
   		 {
			 if (m_pBoard->GetField (x,y)==m_Color)
				 m_pdField[x*m_ulFieldSize+y] = 0.5, m_pdNotField[x*m_ulFieldSize+y] = -0.5;
			 else if (m_pBoard->GetField (x,y)==m_OppColor)
				 m_pdField[x*m_ulFieldSize+y] = -0.5, m_pdNotField[x*m_ulFieldSize+y] = 0.5;
			 else
				 m_pdField[x*m_ulFieldSize+y] = 0.0, m_pdNotField[x*m_ulFieldSize+y] = 0.0;
		 }
 
         net->Input(m_pdField);
	 net->Calc();
         net->Desired(desir);
         net->Learn();
	 net->Input(m_pdNotField);
	 net->Calc();
	 net->Desired(Notdesir);
	 net->Learn();

 	// an der y-Achse gespiegelt	 
	for(y=0; y<m_ulFieldSize; y++)
       		for (x=0; x<m_ulFieldSize; x++)
		{
			if (m_pBoard->GetField (x,y)==m_Color)
				m_pdField[y*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.5, m_pdField[y*m_ulFieldSize+(m_ulFieldSize-x-1)] = -0.5;
			else if (m_pBoard->GetField (x,y)==m_OppColor)
				m_pdField[y*m_ulFieldSize+(m_ulFieldSize-x-1)] = -0.5, m_pdField[y*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.5;
			else
				m_pdField[y*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.0, m_pdField[y*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.0;
		}
       	
         net->Input(m_pdField);
	 net->Calc();
	 net->Desired(desir);
	 net->Learn();
	 net->Input(m_pdNotField);
	 net->Calc(); 
         net->Desired(Notdesir);
	 net->Learn();
	 
	// an der x-Achse gespiegelt
	 for(y=0; y<m_ulFieldSize; y++)
		 for (x=0; x<m_ulFieldSize; x++)
		 {
			 if (m_pBoard->GetField (x,y)==m_Color)
				 m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+x] = 0.5,  m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+x] = -0.5;
			 else if (m_pBoard->GetField (x,y)==m_OppColor)
				 m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+x] = -0.5,  m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+x] = 0.5;
			 else
				 m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+x] = 0.0,  m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+x] = 0.0;
		 }

	 net->Input(m_pdField);
	 net->Calc();
	 net->Desired(desir);
	 net->Learn();
	 net->Input(m_pdNotField);
	 net->Calc();
	 net->Desired(Notdesir);
	 net->Learn();

	 // x- und y-Achse gespiegelt
	 for(y=0; y<m_ulFieldSize; y++)
		 for (x=0; x<m_ulFieldSize; x++)
		 {
			 if (m_pBoard->GetField (x,y)==m_Color)
				 m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.5, m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+(m_ulFieldSize-x-1)] = -0.5;
			 else if (m_pBoard->GetField (x,y)==m_OppColor)
				 m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+(m_ulFieldSize-x-1)] = -0.5, m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.5;
			 else
				 m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.0, m_pdField[(m_ulFieldSize-y-1)*m_ulFieldSize+(m_ulFieldSize-x-1)] = 0.0;
		 }
	 
	 net->Input(m_pdField);
	 net->Calc();
	 net->Desired(desir);
	 net->Learn();
	 net->Input(m_pdNotField);
	 net->Calc();
	 net->Desired(Notdesir);
	 net->Learn();
*/
 	 return true;	 
}

void GenBackPropPlayer::Final (char *FileName)
{
  net->Save(FileName);
}

GenBackPropPlayer::~GenBackPropPlayer()
{
  if(net!=NULL)	delete net;
  delete m_pdField;
  delete m_pdNotField;
}
