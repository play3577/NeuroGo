// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
//
#include "GoGroup.h"

/*****************************************************************************
 * Constructor
 ****************************************************************************/

GoGroup::GoGroup (unsigned long ulEdgeLength,
                  unsigned long ulXPos, unsigned long ulYPos)
{
  m_ulXPos           = ulXPos;
  m_ulYPos           = ulYPos;
  m_ulBoardPos       = ulYPos*(ulEdgeLength+2)+ulXPos;
  m_pGroupMembers    = new GoGroup* [ulEdgeLength*ulEdgeLength];
  m_pulLiberties     = new unsigned long [ulEdgeLength*ulEdgeLength];

  if (ulXPos==0)
    m_xStatus = GO_WBORDER;
  else if (ulXPos==ulEdgeLength+1)
    m_xStatus = GO_EBORDER;
  else if (ulYPos==0)
    m_xStatus = GO_NBORDER;
  else if (ulYPos==ulEdgeLength+1)
    m_xStatus = GO_SBORDER;
  else
    m_xStatus = GO_EMPTY;
}

/*****************************************************************************
 * Destructor
 ****************************************************************************/

GoGroup::~GoGroup ()
{
  delete[] m_pGroupMembers;
  delete[] m_pulLiberties;
}

/*****************************************************************************
 * Init
 ****************************************************************************/

void GoGroup::Init ()
{
  m_ulLiberties      = 0;
  m_pGroupAdress     = this;
  m_pGroupMembers[0] = this;  
}

/*****************************************************************************
 * SetNeighbours
 ****************************************************************************/

void GoGroup::SetNeighbours (GoGroup* North, GoGroup* South,
                             GoGroup* West,  GoGroup* East)
{
  m_WesternField  = West;
  m_EasternField  = East;
  m_NorthernField = North;
  m_SouthernField = South;
}

/*****************************************************************************
 * Reset
 ****************************************************************************/

void GoGroup::Reset ()
{
  InitNewGroup (GO_EMPTY);

  m_WesternField->GetGroup()->AddLiberty(m_ulBoardPos);

  if (m_EasternField->GetGroup() != m_WesternField->GetGroup())
    m_EasternField->GetGroup()->AddLiberty(m_ulBoardPos);
  
  if ( (m_NorthernField->GetGroup() != m_WesternField->GetGroup()) &&
       (m_NorthernField->GetGroup() != m_EasternField->GetGroup()) )
    m_NorthernField->GetGroup()->AddLiberty(m_ulBoardPos);
  
  if ( (m_SouthernField->GetGroup() != m_WesternField->GetGroup()) &&
       (m_SouthernField->GetGroup() != m_EasternField->GetGroup()) &&
       (m_SouthernField->GetGroup() != m_NorthernField->GetGroup()) )
    m_SouthernField->GetGroup()->AddLiberty(m_ulBoardPos);
  
}

/*****************************************************************************
 * InitNewGroup
 ****************************************************************************/

void GoGroup::InitNewGroup (GoFieldDesc Color)
{
  m_pGroupAdress = this;
  m_xStatus      = Color;  
  m_ulGroupSize  = 1;
}

/*****************************************************************************
 * RemoveLiberty
 *****************************************************************************/

unsigned long GoGroup::RemoveLiberty (unsigned long ulPos,
                                      GoFieldDesc   Color,
                                      unsigned long &ulKilledPos)
{
  unsigned long ulKilled=0;
  unsigned long ulSearchPos=0;

  while (m_pulLiberties[ulSearchPos]!=ulPos)
    ulSearchPos++;
  m_pulLiberties[ulSearchPos]=m_pulLiberties[--m_ulLiberties];
  
  if ( (m_ulLiberties == 0) &&
       (m_xStatus != Color) &&
       (m_xStatus != GO_EMPTY) &&
       (m_xStatus < GO_NBORDER) )
  {
      unsigned long Cntr;

      ulKilled = m_ulGroupSize;
      ulKilledPos = m_ulBoardPos;

      for (Cntr=0; Cntr < m_ulGroupSize; Cntr++)
          m_pGroupMembers[Cntr]->ClearGroup();
      for (Cntr=1; Cntr < m_ulGroupSize; Cntr++)
          m_pGroupMembers[Cntr]->Reset();
      InitNewGroup(GO_EMPTY);
      Reset();
  }
  return ulKilled;
}

/*****************************************************************************
 * CheckKills
 *****************************************************************************/

unsigned long GoGroup::CheckKills (unsigned long &ulKilledPos,
                                   GoFieldDesc   NewColor)
{
  unsigned long ulKilled=0;
  GoFieldDesc OpponentColor = (NewColor==GO_BLACK?GO_WHITE:GO_BLACK);

  if ( (m_WesternField->GetStatus() == OpponentColor) &&
       (m_WesternField->GetGroup()->GetLiberties() == 1) )
  {
      ulKilled   += m_WesternField->GetGroup()->GetGroupSize();
      ulKilledPos = m_WesternField->GetPos();
  }
  
  if ( (m_EasternField->GetStatus() == OpponentColor) &&
       (m_EasternField->GetGroup()->GetLiberties() == 1) )
  {
      ulKilled   += m_EasternField->GetGroup()->GetGroupSize();
      ulKilledPos = m_EasternField->GetPos();
  }
  
  if ( (m_NorthernField->GetStatus() == OpponentColor) &&
       (m_NorthernField->GetGroup()->GetLiberties() == 1) )
  {
      ulKilled   += m_NorthernField->GetGroup()->GetGroupSize();
      ulKilledPos = m_NorthernField->GetPos();
  }
  
  if ( (m_SouthernField->GetStatus() == OpponentColor) &&
       (m_SouthernField->GetGroup()->GetLiberties() == 1) )
  {
      ulKilled   += m_SouthernField->GetGroup()->GetGroupSize();
      ulKilledPos = m_SouthernField->GetPos();
  }
  

  return ulKilled;
}

/*****************************************************************************
 * CheckSuicide
 *****************************************************************************/

bool GoGroup::CheckSuicide (GoFieldDesc Color) 
{
  /* is set to true if move would definitively not be suicide. this is given
     if one of the four adjecent fields
     1) is free
     2) belongs to a group of the own color with more than 1 liberty
        (which would be this field).
     3) belongs to an opponent group which will be killed by this move
 */
  bool bNoSuicide = false;
  GoFieldDesc OpponentColor = (Color==GO_BLACK?GO_WHITE:GO_BLACK);
   
  if ( (m_WesternField->GetStatus() == GO_EMPTY) ||
       ( (m_WesternField->GetStatus() == Color) &&
         (m_WesternField->GetGroup()->GetLiberties() > 1) ) ||
       ( (m_WesternField->GetStatus() == OpponentColor) &&
         (m_WesternField->GetGroup()->GetLiberties() == 1) ) )
    bNoSuicide = true;
  
  if ( (m_EasternField->GetStatus() == GO_EMPTY) ||
       ( (m_EasternField->GetStatus() == Color) &&
         (m_EasternField->GetGroup()->GetLiberties() > 1) ) ||
       ( (m_EasternField->GetStatus() == OpponentColor) &&
         (m_EasternField->GetGroup()->GetLiberties() == 1) ) )
    bNoSuicide = true;

  if ( (m_NorthernField->GetStatus() == GO_EMPTY) ||
       ( (m_NorthernField->GetStatus() == Color) &&
         (m_NorthernField->GetGroup()->GetLiberties() > 1) ) ||
       ( (m_NorthernField->GetStatus() == OpponentColor) &&
         (m_NorthernField->GetGroup()->GetLiberties() == 1) ) )
    bNoSuicide = true;
  
  if ( (m_SouthernField->GetStatus() == GO_EMPTY) ||
       ( (m_SouthernField->GetStatus() == Color) &&
         (m_SouthernField->GetGroup()->GetLiberties() > 1) ) ||
       ( (m_SouthernField->GetStatus() == OpponentColor) &&
         (m_SouthernField->GetGroup()->GetLiberties() == 1) ) )
    bNoSuicide = true;
  return bNoSuicide;
}
/*****************************************************************************
 * Join
 *****************************************************************************/

void GoGroup::Join (GoGroup* JoinGroup)
{
  GoGroup **pulNewPositions = JoinGroup->GetPosPtr();
  unsigned long *pulNewLiberties = JoinGroup->GetLibPtr();
  unsigned long Cntr,i;
  bool DoubleLib;
  
  for (Cntr=JoinGroup->GetGroupSize(); Cntr>0; Cntr--)
  {
      (*pulNewPositions)->SetGroupPtr (this);
      m_pGroupMembers[m_ulGroupSize++] = *pulNewPositions++;
  }
  
  for (Cntr=JoinGroup->GetLiberties(); Cntr>0; Cntr--)
  {
      DoubleLib = false;
      for (i=0; i<m_ulLiberties; i++)
        if (m_pulLiberties[i]==*pulNewLiberties)
          {
              DoubleLib = true;
              break;
          }
      if (!DoubleLib)
        m_pulLiberties[m_ulLiberties++]=*pulNewLiberties;
      pulNewLiberties++;
  }         
}
 
/*****************************************************************************
 * PrintInfo
 ****************************************************************************/

void GoGroup::PrintInfo (unsigned long Length)
{
  cout << "Status : ";
  switch (m_xStatus)
  {
    case GO_EMPTY  : cout << "Leer" << endl;
                     break;
    case GO_WHITE  : cout << "Weiss" << endl;
                     break;
    case GO_BLACK  : cout << "Schwarz" << endl;
                     break;
    default        : break;
  }
  cout << "Gruppengröße : " << m_pGroupAdress->GetGroupSize() << endl;
  cout << "  Felder : ";
  GoGroup** Group=m_pGroupAdress->GetPosPtr();
  for (unsigned long c=0; c<m_pGroupAdress->GetGroupSize();c++)
  {
      cout << "[" << Group[c]->GetPos()%Length-1;
      cout << "," << Group[c]->GetPos()/Length-1 << "]";
  }
  cout << endl;
  cout << "Freiheiten : " << m_pGroupAdress->GetLiberties() << endl;
  cout << "  Felder : ";
  unsigned long* Libs=m_pGroupAdress->GetLibPtr();
  for (int c=0; c<m_pGroupAdress->GetLiberties();c++)
  {
      cout << "[" << Libs[c]%Length-1;
      cout << "," << Libs[c]/Length-1 << "]";
  }
  cout << endl;
}

    
