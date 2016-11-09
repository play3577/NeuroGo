// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de
//
#ifndef GOGROUP
#define GOGROUP

#include <iostream>

enum GoFieldDesc 
  {
    GO_EMPTY,GO_BLACK,GO_WHITE,GO_NBORDER,GO_SBORDER,GO_WBORDER,GO_EBORDER
  };

class GoGroup
{
 private:

  GoGroup **m_pGroupMembers;
  unsigned long *m_pulLiberties;
  
  GoFieldDesc m_xStatus;
  GoGroup *m_pGroupAdress;

  // position of this field on the go-board
  unsigned long m_ulXPos;
  unsigned long m_ulYPos;  
  unsigned long m_ulBoardPos;
  // number of liberties and size of the group to which this field belongs
  unsigned long m_ulLiberties;
  unsigned long m_ulGroupSize;

  // neighbours of this field
  GoGroup *m_WesternField;
  GoGroup *m_EasternField;
  GoGroup *m_NorthernField;
  GoGroup *m_SouthernField;

 public:
  GoGroup(unsigned long ulEdgeLength,
          unsigned long ulXPos, unsigned long ulYPos);
  ~GoGroup();
  void Init();

  void SetNeighbours (GoGroup* North, GoGroup* South,
                      GoGroup* West,  GoGroup* East);
  void Reset ();
  void InitNewGroup (GoFieldDesc Color);
  unsigned long RemoveLiberty(unsigned long ulPos,
                              GoFieldDesc   Color,
                              unsigned long &ulKilledPos);
  unsigned long CheckKills (unsigned long &ulKilledPos, GoFieldDesc NewColor);
  bool CheckSuicide (GoFieldDesc Color);
  void Join (GoGroup* JoinGroup);

  void PrintInfo (unsigned long Length);

  inline void SetGroupPtr (GoGroup* NewGroup) {m_pGroupAdress = NewGroup;}
  inline GoGroup* GetGroup() {return m_pGroupAdress;}
  inline void AddLiberty (unsigned long Pos) { m_pulLiberties[m_ulLiberties++]=Pos;}
  inline void ClearGroup() { m_ulLiberties=0;m_pGroupAdress=this;}
  inline GoFieldDesc GetStatus () { return m_xStatus;}
  inline unsigned long GetPos () {return m_ulBoardPos;}
  inline void GetPos (unsigned long &x, unsigned long &y) { x=m_ulXPos; y=m_ulYPos;}
  inline unsigned long GetGroupSize () {return m_ulGroupSize;}
  inline unsigned long GetLiberties () {return m_ulLiberties;}
  inline GoGroup** GetPosPtr () { return m_pGroupMembers;}
  inline unsigned long* GetLibPtr () { return m_pulLiberties;}  
};
#endif
