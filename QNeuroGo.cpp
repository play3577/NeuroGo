// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de

#include <iostream.h>
#include <qapplication.h>
#include <qmenubar.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qwidget.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qmessagebox.h>
//#include <qimageio.h>
#include "GoBoard.h"
#include "GoPlayer.h"
#include "AsciiNeuroGo.h"
#define FIELDSIZE 9
#define FIELDSCAL 40
#define PICOFFSET 30

enum MenuIDs {
	COLOR_MENU_ID_HVH=0,
	COLOR_MENU_ID_HVN=1,
	COLOR_MENU_ID_HVS1=2,
	COLOR_MENU_ID_HVS2=3,
	COLOR_MENU_ID_NVN=4,
	COLOR_MENU_ID_NVR=5,
	COLOR_MENU_ID_NVS1=6,
	COLOR_MENU_ID_NVS2=7
	};

class QNeuroGo : public QWidget
{
  Q_OBJECT

public:
  QNeuroGo();
  ~QNeuroGo();

public slots:
  void slotLoad();
  void slotSave();
protected:
  virtual void mousePressEvent(QMouseEvent*);
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);

private slots:
  void slotAbout();
  void slotAboutQt();
  void slotColorMenu(int);
private:
  GoFieldDesc m_iPlayer;
  int         m_iXSize;
  int         m_iYSize;
  long        m_lOXDim;
  long        m_lOYDim;
  QPoint      _last;
  QColor      _currentcolor;
  QPixmap     m_PicBuffer;
  QImage      m_MyPic;
  QMenuBar*   m_pxMenubar;
  QPopupMenu* m_pxFilemenu;
  QPopupMenu* m_pxColormenu;
  QPopupMenu* m_pxHelpmenu;
  QImage      m_xImage;
  GoBoard*    m_pxGoBoard;
  void        Draw(bool bMark);
  GoPlayer*   m_pxPlayer1;
  GoPlayer*   m_pxPlayer2;
  int         m_iItem;
};

#include "QNeuroGo.moc"

QNeuroGo::QNeuroGo()
{
  setBackgroundMode(NoBackground);

  m_pxFilemenu = new QPopupMenu;
  m_pxFilemenu->insertItem("&Load",this,SLOT(slotLoad()));
  m_pxFilemenu->insertItem("&Save",this,SLOT(slotSave()));
  m_pxFilemenu->insertItem("&Quit",qApp,SLOT(quit())); 
													  
  m_pxColormenu = new QPopupMenu;
  m_pxColormenu->insertItem("Human vs Human",  COLOR_MENU_ID_HVH);
  m_pxColormenu->insertItem("Human vs Neuro",  COLOR_MENU_ID_HVN);
  m_pxColormenu->insertItem("Human vs Strato", COLOR_MENU_ID_HVS1);
  m_pxColormenu->insertItem("Human vs Paul",   COLOR_MENU_ID_HVS2);
  m_pxColormenu->insertItem("Neuro vs Neuro",  COLOR_MENU_ID_NVN);
  m_pxColormenu->insertItem("Neuro vs Random", COLOR_MENU_ID_NVR);
  m_pxColormenu->insertItem("Neuro vs Strato", COLOR_MENU_ID_NVS1);
  m_pxColormenu->insertItem("Neuro vs Paul",   COLOR_MENU_ID_NVS2);
  QObject::connect(m_pxColormenu,SIGNAL(activated(int)),this,SLOT(slotColorMenu(int)));

  m_pxHelpmenu = new QPopupMenu;
  m_pxHelpmenu->insertItem("&About QNeuroGo",this,SLOT(slotAbout()));
  m_pxHelpmenu->insertItem("About &Qt",this,SLOT(slotAboutQt()));

  m_pxMenubar = new QMenuBar(this);
  m_pxMenubar->insertItem("&File",m_pxFilemenu);
  m_pxMenubar->insertItem("&Game",m_pxColormenu);
  m_pxMenubar->insertSeparator();
  m_pxMenubar->insertItem("&Help",m_pxHelpmenu);

  this->setMinimumSize(FIELDSIZE*FIELDSCAL,FIELDSIZE*FIELDSCAL+PICOFFSET);
  this->setMaximumSize(FIELDSIZE*FIELDSCAL,FIELDSIZE*FIELDSCAL+PICOFFSET);

  m_pxPlayer1 = NULL;
  m_pxPlayer2 = NULL;
  m_pxGoBoard = new GoBoard(FIELDSIZE);
  m_pxGoBoard->Init();
  slotColorMenu (COLOR_MENU_ID_HVH);
  m_iPlayer = GO_BLACK;
  Draw (false);
}

QNeuroGo::~QNeuroGo()
{
  delete m_pxGoBoard;
}

void QNeuroGo::mousePressEvent(QMouseEvent* event)
{
  int iXPos = event->x() / FIELDSCAL;
  int iYPos = (event->y()-PICOFFSET) / FIELDSCAL;
  int iHmFldTst = 1;
  QString Ausgabe;

  if (event->button()==RightButton)
  {
      cout << "--------------------------" << endl;
      cout << "Infos für Feld " << iXPos << "/" << iYPos << ":" << endl;
      m_pxGoBoard->PrintInfo (iXPos, iYPos);      
  }
  else
  if (event->button()==MidButton) 
  {
    if (m_pxGoBoard->GetField(iXPos,iYPos)!=GO_EMPTY)
    {
        cout << "--------------------------" << endl;
        if (m_pxGoBoard->GetField(iXPos,iYPos)==GO_BLACK)
          cout << "Von Schwarz";
        else
          cout << "Von Weiss";
        cout << " umschlossene Gebiete : " << m_pxGoBoard->CountAreas (m_pxGoBoard->GetField(iXPos,iYPos)) << endl;
        Draw (true);
    }
  }
  else
  {
      
      /*
      switch (Result)
      {
      case GO_OK       : cout << "OK" << endl;
        break;
      case GO_OCCUPIED : cout << "Besetzt" << endl;
        break;
      case GO_SUICIDE  : cout << "Selbstmord" << endl;
        break;
      case GO_KO       : cout << "KO" << endl;
        break;
      }
      */

    bool bRes=true;

    if (m_iItem<=COLOR_MENU_ID_HVS2)
      {
	if(m_pxGoBoard->TestField(iXPos,iYPos)==GO_OK)
	  {
	    m_pxGoBoard->SetField(iXPos,iYPos);
	    if(m_iItem!=COLOR_MENU_ID_HVH)
	      {
		unsigned char ucX,ucY;
		bRes = m_pxPlayer2->MakeMove(0,ucX,ucY);
		if (!bRes)
		  m_iPlayer = (m_iPlayer==GO_BLACK?GO_WHITE:GO_BLACK);
	      }
	    else
	      m_iPlayer = (m_iPlayer==GO_BLACK?GO_WHITE:GO_BLACK);
	   	    
	    int iTestPosX;
	    int iTestPosY;
	    iHmFldTst = 0;
	    for(iTestPosX=0;iTestPosX<m_pxGoBoard->GetSize();iTestPosX++)
	      {
		for(iTestPosY=0;iTestPosY<m_pxGoBoard->GetSize();iTestPosY++)
		  {
		    if(m_pxGoBoard->TestField(iTestPosX,iTestPosY)==GO_OK)
		      {
			iHmFldTst = 1;
		      }
		  }
	      }
	  

	    Draw(false);
	  }
      }
    else
      {
	//	    m_pxGoBoard->SetField(iXPos,iYPos);
	    unsigned char ucX,ucY;
            if(m_iPlayer==GO_BLACK)
	      bRes = m_pxPlayer1->MakeMove(0,ucX,ucY);
	    else
	      bRes = m_pxPlayer2->MakeMove(0,ucX,ucY);

	    if (bRes)
	      m_iPlayer = (m_iPlayer==GO_BLACK?GO_WHITE:GO_BLACK);

	    Draw(false);
      }
    
    if (!bRes||!iHmFldTst)
      {
        if(m_iPlayer==GO_BLACK)
	  Ausgabe = "Weiß hat gewonnen!";
	else
	  Ausgabe = "Schwarz hat gewonnen!";
	QMessageBox::information(this,"Game Over",Ausgabe); 
	m_pxGoBoard->Init(); 
	m_iPlayer = GO_BLACK;

      }
  }
}

void QNeuroGo::paintEvent(QPaintEvent* event)
{
  bitBlt(this,0,PICOFFSET,&m_PicBuffer);
}

void QNeuroGo::resizeEvent(QResizeEvent* event)
{
}

void QNeuroGo::slotLoad()
{
//   QString xFileName = QFileDialog::getOpenFileName("Load Net","",this);
//   if(!xFileName.isNull())
//   {
//     delete m_pxPlayer;
//     m_pxPlayer1 = new GoBackPropPlayer9x9A(m_pxGoBoard,GO_WHITE,(const char*)xFileName);
//     if(m_pxPlayer==NULL)
//     {
// 	   cout << "Could not load net" << endl; 
//     }
//   }
}

void QNeuroGo::slotSave() 
{
}

void QNeuroGo::slotAbout()
{
  QMessageBox::information( this, "Über QNeuroGo","(C) 2001              \n" 
		  	          "Melanie Sippl melanie.sippl@web.de    \n"
		  	          "Ingo Schasiepen ingo.schasiepen@web.de\n"
			          "Andre Betz andre.betz@gmx.de            ");
		 
}

void QNeuroGo::slotAboutQt()
{
  QMessageBox::aboutQt(this,"KNeuroGo 2001");
}

void QNeuroGo::Draw(bool bMark)
{ 
  int iCount;
  QColor Red(255,0,0);
  QColor Pink(255,100,50);
  QColor Blue(0,0,255);
  QColor Green(0,255,0);
  QColor White(255,255,255);
  QColor Black(0,0,0);
  QColor Yellow(150,150,255);
  QColor Gray(128,128,128);
  m_PicBuffer = QPixmap(FIELDSIZE*FIELDSCAL,FIELDSIZE*FIELDSCAL);
  QPainter painter(&m_PicBuffer);
  m_PicBuffer.fill(Yellow);
  painter.setPen(_currentcolor);
  for(iCount=0;iCount<FIELDSIZE;iCount++)
  {
    painter.drawLine(0,iCount*FIELDSCAL+FIELDSCAL/2,FIELDSIZE*FIELDSCAL,iCount*FIELDSCAL+FIELDSCAL/2);
    painter.drawLine(iCount*FIELDSCAL+FIELDSCAL/2,0,iCount*FIELDSCAL+FIELDSCAL/2,FIELDSIZE*FIELDSCAL);
  }

  for(int iY=0;iY<FIELDSIZE;iY++)
    for(int iX=0;iX<FIELDSIZE;iX++)
    {
        int Stein = m_pxGoBoard->GetField(iX,iY);
        if(Stein==GO_BLACK)
        {
            painter.setPen(Red);
            painter.setBrush(White);
            //         painter.setBrush(SolidPattern);
        }
        if(Stein==GO_WHITE) 
        {
            painter.setPen(Blue);
            painter.setBrush(Pink);
            //painter.setBrush(SolidPattern);
        }
        
        if(Stein!=GO_EMPTY)
        {
            for (long x=0; x<FIELDSCAL/2;x++)
            {
                if (Stein==GO_WHITE)
                {
                    QColor Col(128+127*2*x/FIELDSCAL,128+127*2*x/FIELDSCAL,128+127*2*x/FIELDSCAL);
                    painter.setPen(Col);
                    painter.setBrush(Col);
                    painter.drawEllipse(iX*FIELDSCAL+x+1*x/4,iY*FIELDSCAL+x-1*x/4,FIELDSCAL-2*x,FIELDSCAL-2*x);
                }
                else
                {
                    QColor Col(32+127*2*x/FIELDSCAL,32+127*2*x/FIELDSCAL,32+127*2*x/FIELDSCAL);
                    painter.setPen(Col);
                    painter.setBrush(Col);
                    painter.drawEllipse(iX*FIELDSCAL+x+1*x/4,iY*FIELDSCAL+x-1*x/4,FIELDSCAL-2*x,FIELDSCAL-2*x);
                }
            }
            //      painter.drawEllipse(iX*FIELDSCAL,iY*FIELDSCAL,FIELDSCAL,FIELDSCAL);
        }
        if (bMark)
        {
            if (m_pxGoBoard->CheckArea(iX,iY))
            {
                painter.setPen(Red);
                painter.drawLine(iX*FIELDSCAL,iY*FIELDSCAL,(iX+1)*FIELDSCAL-1,(iY+1)*FIELDSCAL-1);
                painter.drawLine(iX*FIELDSCAL,(iY+1)*FIELDSCAL-1,(iX+1)*FIELDSCAL-1,iY*FIELDSCAL);
            }
        }
    }
  
  repaint(FALSE);
}

void QNeuroGo::slotColorMenu(int item)
{
  if (m_pxPlayer1)
    {
      delete m_pxPlayer1;
      m_pxPlayer1=0;
    }
  if (m_pxPlayer2)
    {
      delete m_pxPlayer2;
      m_pxPlayer2=0;
    }
                      
  m_iItem = item;
  
  if(item==COLOR_MENU_ID_HVS2)
    { 
      m_pxPlayer2 = new Paul(m_pxGoBoard,GO_WHITE);
    }
  
  if(item==COLOR_MENU_ID_HVS1)
    { 
      m_pxPlayer2 = new GoStrategyPlayer(m_pxGoBoard,GO_WHITE);
    }

  if(item == COLOR_MENU_ID_NVR)
    {  
      m_pxPlayer2 = new GoRandomPlayer (m_pxGoBoard,GO_WHITE);
    }
  
  if(item == COLOR_MENU_ID_NVS1) 
    {
       m_pxPlayer2 = new GoStrategyPlayer(m_pxGoBoard,GO_WHITE);
    }
  
  if(item == COLOR_MENU_ID_NVS2)
    {
      m_pxPlayer2 = new Paul (m_pxGoBoard,GO_WHITE);
    }
  
  if ( (item==COLOR_MENU_ID_HVN) || (item>=COLOR_MENU_ID_NVN) )
    {
      // Lade Netz  
      QString xFileName = QFileDialog::getOpenFileName("Load Net1","",this);
      cout << xFileName << endl;
      if(!xFileName.isNull())
	{
	  if (item==COLOR_MENU_ID_HVN)	  
	    m_pxPlayer2 = new GoBackPropPlayer9x9A(m_pxGoBoard,GO_WHITE,(const char*)xFileName);
	  else
	    m_pxPlayer1 = new GoBackPropPlayer9x9A(m_pxGoBoard,GO_BLACK,(const char*)xFileName);
	}
      else
	{
	  QMessageBox::critical(this,"File Panic!","Could not load net1!"); 
	  m_pxPlayer1 = new GoRandomPlayer (m_pxGoBoard,GO_BLACK);
	}
      
      if (item==COLOR_MENU_ID_NVN)
	{
	  // Lade Netz  
	  QString xFileName = QFileDialog::getOpenFileName("Load Net2","",this);
	  if(!xFileName.isNull())
	    {
	      m_pxPlayer2 = new GoBackPropPlayer9x9A(m_pxGoBoard,GO_WHITE,(const char*)xFileName);
	    }
	  else
	    {
	      QMessageBox::critical(this,"File Panic!","Could not load net2!"); 
	      m_pxPlayer2 = new GoRandomPlayer (m_pxGoBoard,GO_BLACK);
	    }
	}
    }
}

int main(int argc,char** argv)
{
  //  qInitImageIO();
  if(argc==1)
  {
    QApplication myKEApp(argc,argv);
    QNeuroGo* myKEWidget = new QNeuroGo();
    myKEWidget->setGeometry(50,50,400,400);
    myKEApp.setMainWidget(myKEWidget);
    myKEWidget->show();
    return myKEApp.exec();
  }
  else
  {
    return AsciiNeuroGo(argc,argv);
  }
}
