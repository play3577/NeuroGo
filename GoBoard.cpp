// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
#include "GoBoard.h"

/*****************************************************************************
 * Constructor
 ****************************************************************************/

GoBoard::GoBoard (unsigned long ulEdgeLength)
{
  unsigned long x,y;
  
  m_ulEdgeLength = ulEdgeLength;
  m_ulRealLength = ulEdgeLength+2;
  m_ulBoardSize  = m_ulRealLength*m_ulRealLength;
  m_pField       = new sField [m_ulBoardSize];

  // initialize board. each field is a GoGroup that wants to know the
  // EdgeLength and its own position. 
  for (y = 0; y<m_ulRealLength; y++)
    for (x = 0; x<m_ulRealLength; x++)
      m_pField[y*m_ulRealLength+x].Group = new GoGroup (m_ulEdgeLength, x, y);

  // now that every group exists, some further initializations for the inner
  // fields (without border) can be done. each group wants to know its
  // neighbours, and it is set to EMPTY. 
  for (y = 1; y<=m_ulEdgeLength; y++)
    for (x = 1; x<=m_ulEdgeLength; x++)
    {
        m_pField[y*m_ulRealLength+x].Group->SetNeighbours
          (m_pField[(y-1)*m_ulRealLength+x].Group,
           m_pField[(y+1)*m_ulRealLength+x].Group,
           m_pField[y*m_ulRealLength+x-1].Group,
           m_pField[y*m_ulRealLength+x+1].Group);
	//        m_pField[y*m_ulRealLength+x].Group->Reset();
    }
}

/*****************************************************************************
 * Destructor
 ****************************************************************************/

GoBoard::~GoBoard ()
{
  for (unsigned long i = 0; i<m_ulBoardSize; i++)
  {
      delete m_pField[i].Group;
  }
  delete[] m_pField;
}

/*****************************************************************************
 * Init
 ****************************************************************************/

void GoBoard::Init ()
{
  unsigned long x,y;
  for (y = 0; y<m_ulRealLength; y++)
    for (x = 0; x<m_ulRealLength; x++)
      m_pField[y*m_ulRealLength+x].Group->Init();
  for (y = 1; y<=m_ulEdgeLength; y++)
    for (x = 1; x<=m_ulEdgeLength; x++)
      m_pField[y*m_ulRealLength+x].Group->Reset();

  m_ulMoveNumber = 2*m_ulEdgeLength*m_ulEdgeLength;  
  m_ulLastPos    = 0;
  m_ulLastKills  = 0;
  m_xPlayer      = GO_BLACK;
}

/*****************************************************************************
 * TestField
 ****************************************************************************/

GoError GoBoard::TestField (long x, long y)
{
  // coordinates have to be adjusted due to the border this class uses.
  x++;
  y++;

  if ( (x<1) || (y<1) || (x>m_ulEdgeLength) || (y>m_ulEdgeLength) )
    return GO_OUT_OF_FIELD;

  unsigned long ulBoardPos = y*m_ulRealLength+x;
  unsigned long ulKillPos  = 0;
  unsigned long ulKills;

  // check if field is free
  if (!m_pField[ulBoardPos].Group->GetStatus()==GO_EMPTY)
    return GO_OCCUPIED;
  
  // check if move is suicide
  if (!m_pField[ulBoardPos].Group->CheckSuicide(m_xPlayer))
    return GO_SUICIDE;

  ulKills = m_pField[ulBoardPos].Group->CheckKills (ulKillPos, m_xPlayer);

  // check for ko
  if ( (ulBoardPos==m_ulLastKillPos) && (m_ulLastKills==1) &&
       (ulKillPos ==m_ulLastPos) && (ulKills==1) )
    return GO_KO;

  return GO_OK;
}

/*****************************************************************************
 * SetField
 ****************************************************************************/

GoError GoBoard::SetField (unsigned long x, unsigned long y)
{

  // coordinates have to be adjusted due to the border this class uses.
  x++;
  y++;

  unsigned long ulBoardPos = y*m_ulRealLength+x;

  GoGroup *CenterGroup   = m_pField[ulBoardPos].Group;
  GoGroup *WesternGroup  = m_pField[y*m_ulRealLength+x-1].Group->GetGroup();
  GoGroup *EasternGroup  = m_pField[y*m_ulRealLength+x+1].Group->GetGroup();
  GoGroup *NorthernGroup = m_pField[(y-1)*m_ulRealLength+x].Group->GetGroup();
  GoGroup *SouthernGroup = m_pField[(y+1)*m_ulRealLength+x].Group->GetGroup();
      
  // at first make this field an own group. if adjacent groups of the same
  // color exists they will be combined 
  CenterGroup->InitNewGroup (m_xPlayer);

  m_ulLastKills=0;

  // this field is not EMPTY any longer, so the neighbours lose it as liberty.
  // accumulate all killed stones. if sum is 1, then ko is possible
  m_ulLastKills+=WesternGroup->RemoveLiberty(ulBoardPos,m_xPlayer,m_ulLastKillPos);
  // if the western group has the same color, they are combined.
  if (WesternGroup->GetStatus() == m_xPlayer)
  {
      WesternGroup->Join (CenterGroup);
      // center now belongs to the western group, so its group pointer has changed
      // and must be reloaded
      CenterGroup = CenterGroup->GetGroup();
  }

  // do the same as above, but only, if the eastern field does NOT belong to
  // the same group as the western field. if they'd belong to the same group,
  // all necessary work allready would have be done above.
  if (EasternGroup != WesternGroup)
  {
      m_ulLastKills+=EasternGroup->RemoveLiberty(ulBoardPos,m_xPlayer,m_ulLastKillPos);
      if (EasternGroup->GetStatus() == m_xPlayer)
        // if groups are combined, the smaller group is added to the bigger
        // for speed optimization
        if (CenterGroup->GetGroupSize()<EasternGroup->GetGroupSize() )
          EasternGroup->Join (CenterGroup);
        else
          CenterGroup->GetGroup()->Join (EasternGroup);
      CenterGroup = CenterGroup->GetGroup();
  }
  
  // and so on...
  if ( (NorthernGroup != WesternGroup) && 
       (NorthernGroup != EasternGroup) )
  {
      m_ulLastKills+=NorthernGroup->RemoveLiberty(ulBoardPos,m_xPlayer,m_ulLastKillPos);
      if (NorthernGroup->GetStatus() == m_xPlayer)
        if (CenterGroup->GetGroupSize()<NorthernGroup->GetGroupSize() )
          NorthernGroup->Join (CenterGroup);
        else
          CenterGroup->GetGroup()->Join (NorthernGroup);
      CenterGroup = CenterGroup->GetGroup();
  }
  
  if ( (SouthernGroup != WesternGroup) &&
       (SouthernGroup != EasternGroup) &&
       (SouthernGroup != NorthernGroup) )
  {
      m_ulLastKills+=SouthernGroup->RemoveLiberty(ulBoardPos,m_xPlayer,m_ulLastKillPos);
      if (SouthernGroup->GetStatus() == m_xPlayer)
        if (CenterGroup->GetGroupSize()<SouthernGroup->GetGroupSize() )
          SouthernGroup->Join (CenterGroup);
        else
          CenterGroup->GetGroup()->Join (SouthernGroup);
      CenterGroup = CenterGroup->GetGroup();
  }

  m_ulLastPos = ulBoardPos;
  if(m_xPlayer == GO_WHITE) m_xPlayer = GO_BLACK;
  else                      m_xPlayer = GO_WHITE;

  return GO_OK;
}

/*****************************************************************************
 * GetLocalLib
 ****************************************************************************/

unsigned long GoBoard::GetLocalLib (unsigned long x, unsigned long y)
{
  unsigned long ulResult=0;
  x++;
  y++;

  if (m_pField[(y-1)*m_ulRealLength+x].Group->GetStatus()==GO_EMPTY)
    ulResult++;
  if (m_pField[(y+1)*m_ulRealLength+x].Group->GetStatus()==GO_EMPTY)
    ulResult++;
  if (m_pField[y*m_ulRealLength+x-1].Group->GetStatus()==GO_EMPTY)
    ulResult++;
  if (m_pField[y*m_ulRealLength+x+1].Group->GetStatus()==GO_EMPTY)
    ulResult++;

  return ulResult;
}

/*****************************************************************************
 * GetGroupStone
 ****************************************************************************/

void GoBoard::GetGroupStone (unsigned long x, unsigned long y,
                             unsigned long StoneNr,
                             unsigned long &StoneX, unsigned long &StoneY)
{
  if (StoneNr>GetGroupSize(x,y))
  {
      cout << "Oh oh! Fehler! Und zwar in der Funktion GetGroupStone." << endl;
      cout << "Die Gruppe ist kleiner als Du denkst!" << endl;
      exit(0);
  }
  m_pField[(y+1)*m_ulRealLength+x+1].Group->GetGroup()->GetPosPtr()[StoneNr]->GetPos(StoneX,StoneY);
  StoneX--;
  StoneY--;
}

/*****************************************************************************
 * GetFirstLib
 ****************************************************************************/

void GoBoard::GetFirstLib (unsigned long x, unsigned long y,
                           unsigned long &LibX, unsigned long &LibY)
{
  unsigned long FirstLib;
  FirstLib = m_pField[(y+1)*m_ulRealLength+x+1].Group->GetGroup()->GetLibPtr()[0];
  LibX=(FirstLib%m_ulRealLength)-1;
  LibY=(FirstLib/m_ulRealLength)-1;
}

/*****************************************************************************
 * ClearMarker
 ****************************************************************************/

void GoBoard::ClearMarker ()
{
  for (unsigned long Cntr=0; Cntr<m_ulBoardSize; Cntr++)
    m_pField[Cntr].Marker=CLEAR;
}

/*****************************************************************************
 * MarkArea
 ****************************************************************************/

void GoBoard::MarkArea (unsigned long ulXPos, unsigned long ulYPos,
                        GoFieldDesc Color)
{
  unsigned long ulBoardPos=ulYPos*m_ulRealLength+ulXPos;
  
  m_pField[ulBoardPos].Marker=MARKED;
  m_ulAreaSize++;
  if (ulXPos>1)
  {
      if( (m_pField[ulBoardPos-1].Group->GetStatus()!=Color) &&
          (m_pField[ulBoardPos-1].Marker==CLEAR) )
        MarkArea (ulXPos-1, ulYPos, Color);
  }
  else
    m_bWBorderUsed = true;

  if (ulXPos<m_ulEdgeLength)
  {
      if( (m_pField[ulBoardPos+1].Group->GetStatus()!=Color) &&
          (m_pField[ulBoardPos+1].Marker==CLEAR) )
        MarkArea (ulXPos+1, ulYPos, Color);
  }
  else
    m_bEBorderUsed = true;

  if (ulYPos>1)
  {
      if( (m_pField[ulBoardPos-m_ulRealLength].Group->GetStatus()!=Color) &&
          (m_pField[ulBoardPos-m_ulRealLength].Marker==CLEAR) )
        MarkArea (ulXPos, ulYPos-1, Color);
  }
  else
    m_bNBorderUsed = true;
  if (ulYPos<m_ulEdgeLength)
  {
      if( (m_pField[ulBoardPos+m_ulRealLength].Group->GetStatus()!=Color) &&
          (m_pField[ulBoardPos+m_ulRealLength].Marker==CLEAR) )
        MarkArea (ulXPos, ulYPos+1, Color);
  }
  else
    m_bSBorderUsed = true;
  
  
}


/*****************************************************************************
 * CountAreas
 ****************************************************************************/

unsigned long GoBoard::CountAreas (GoFieldDesc Color)
{
  ClearMarker();
  unsigned long ulXPos;
  unsigned long ulYPos;
  unsigned long ulBoardPos;
  unsigned long ulCountBorders;
  unsigned long ulNumber;

  ulNumber = 0;
  
  for (ulYPos=1; ulYPos<=m_ulEdgeLength; ulYPos++)
  {
      for (ulXPos=1; ulXPos<=m_ulEdgeLength; ulXPos++)
      {
          ulBoardPos = ulYPos*m_ulRealLength+ulXPos;
          
          if ( (m_pField[ulBoardPos].Group->GetStatus()!=Color) &&
               (m_pField[ulBoardPos].Marker==CLEAR ) )
          {
              m_bNBorderUsed = false;
              m_bSBorderUsed = false;
              m_bWBorderUsed = false;
              m_bEBorderUsed = false; 
              m_ulAreaSize = 0;
              MarkArea (ulXPos,ulYPos,Color);
              ulCountBorders = 0;
              if (m_bNBorderUsed)
                ulCountBorders++;
              if (m_bSBorderUsed)
                ulCountBorders++;
              if (m_bWBorderUsed)
                ulCountBorders++;
              if (m_bEBorderUsed)
                ulCountBorders++;
              if (ulCountBorders<3)
              {
                  ulNumber += m_ulAreaSize;
                  for (unsigned long Cntr=0; Cntr<m_ulBoardSize; Cntr++)
                    if (m_pField[Cntr].Marker==MARKED)
                      m_pField[Cntr].Marker=SET;
              }
              else
              {
                  for (unsigned long Cntr=0; Cntr<m_ulBoardSize; Cntr++)
                    if (m_pField[Cntr].Marker==MARKED)
                      m_pField[Cntr].Marker=NOTSET;
              }
          }
      }
      
  }
  return ulNumber;
}

/*****************************************************************************
 * DrawField
 ****************************************************************************/

void GoBoard::DrawField ()
{
  unsigned long x,y;
  for (y=0;y<m_ulRealLength;y++)
  {
      for (x=0;x<m_ulRealLength;x++)
        switch (m_pField[y*m_ulRealLength+x].Group->GetStatus())
        {
        case GO_EMPTY  : cout << ".";
                         break;
        case GO_BLACK  : cout << "X";
                         break;
        case GO_WHITE  : cout << "O";
                         break;
        default        : cout << "=";
                         break;
        }
      cout << endl;
  }
}

/*****************************************************************************
 * PrintInfo
 ****************************************************************************/

void GoBoard::PrintInfo (int X, int Y)
{
  m_pField[(Y+1)*m_ulRealLength+X+1].Group->PrintInfo(m_ulRealLength);
}

    
