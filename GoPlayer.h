// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
#include "GoBoard.h"
#include "backpropnet.h"
#include "myBackpropNet.h"

#ifndef GOPLAYER
#define GOPLAYER

class GoPlayer
{
 protected:
  GoBoard* m_pBoard;
  GoFieldDesc m_Color;
  GoFieldDesc m_OppColor;
  unsigned long m_ulFieldSize;
 public:

  GoPlayer();
  virtual ~GoPlayer();
  virtual bool MakeMove(bool bFirstMove,
			unsigned char &ucX,
			unsigned char &ucY,
			bool bLearnMode = false) = 0;
   bool Learn(long lDiff, GoFieldDesc Color){};
   void Final () {};
};

class GoRandomPlayer : public GoPlayer
{
  struct sField
  {
    unsigned long x,y;
  };

  sField *m_xFreeFields;

  public:

  GoRandomPlayer(GoBoard* pBoard, GoFieldDesc Color);
  ~GoRandomPlayer();
  bool MakeMove(bool bFirstMove,
                unsigned char &ucX,
		unsigned char &ucY,
		bool bLearnMode = false);
};

class GoBackPropPlayer9x9A : public GoPlayer
{
 private:
  unsigned long m_ulBoardSize;
  BackpropNet<81,40,1> net;
  unsigned long m_ResultAreas;
  GoFieldDesc m_Color, m_OppColor;
 public:
  void SetValues(double gain,double spread,double momentum,double seedreset=0);
  GoBackPropPlayer9x9A(GoBoard* pBoard, GoFieldDesc Color,const char* FileName = NULL);
  ~GoBackPropPlayer9x9A();
  bool MakeMove(bool bFirstMove,	
                unsigned char &ucX,
		unsigned char &ucY,
		bool bLearnMode = false);
  void Final (char* FileName);
  
};

class GoBackPropPlayer9x9B : public GoPlayer
{
 private:
   unsigned long m_ulBoardSize;
   myBackpropNet* net;
   unsigned long m_ResultAreas;
   GoFieldDesc m_Color, m_OppColor;
public:
   void SetValues(int middle,double gain,double spread,double momentum,double seedreset=0);
   GoBackPropPlayer9x9B(GoBoard* pBoard, GoFieldDesc Color,const char* FileName = NULL);
   ~GoBackPropPlayer9x9B();
   bool MakeMove(bool bFirstMove,
   unsigned char &ucX,
   unsigned char &ucY,   
   bool bLearnMode = false);
   void Final (char* FileName);                                                             
};

class GoStrategyPlayer : public GoPlayer
{
private:
   unsigned long m_ulNewX, m_ulNewY;
   unsigned long m_ulMoveCount, m_ulOpenDuration;
	       
   struct sField
   {
      unsigned long x,y;
   };
	 
  sField *m_xFreeFields;
  sField *m_xStratoFields;
public:
  GoStrategyPlayer(GoBoard* pBoard, GoFieldDesc Color);
  ~GoStrategyPlayer();
  bool MakeMove(bool bFirstMove,
  unsigned char &ucX,
  unsigned char &ucY,
  bool bLearnMode = false);
  bool Opening();
  bool IncreaseArea();
};

class Paul : public GoPlayer
{
private:
   unsigned long m_ulNewX, m_ulNewY;
	       
   struct sField
   {
      unsigned long x,y;
   };
	 
  struct sAttack
  {
    unsigned long ulLibs;
    unsigned long ulGrpSize;
    sField Pos;
  };

  sAttack *m_pAttackList;
  bool    *m_pTestField;
  sField *m_xFreeFields;

public:
  Paul  (GoBoard* pBoard, GoFieldDesc Color);
  ~Paul ();
  static int PaulSort (const void *A, const void *B);
  bool MakeMove(bool bFirstMove,
                unsigned char &ucX,
                unsigned char &ucY,
                bool bLearnMode = false);
};

class GenBackPropPlayer : public GoPlayer
{
private:
	double* m_pdField;
	double* m_pdNotField;
	myBackpropNet* net;
public:
	GenBackPropPlayer(GoBoard* pBoard, GoFieldDesc Color,const char *FileName = NULL);
	void SetValues(int Lay,int* Neu, double gain,double spread,double momentum,double seedrand);
	bool MakeMove(bool bFirstMove,unsigned char &ucX,unsigned char &ucY,bool bLearnMode = false);
	bool Learn(long lDiff, GoFieldDesc Color);
	void Final (char *FileName);
	~GenBackPropPlayer();
};

#endif
