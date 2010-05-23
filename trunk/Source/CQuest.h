#pragma once

struct CQuest : CRunicCore
{
  PVOID unk0;

  CLevelTemplateData   *pCLevelTemplateData;

  u32 unk1[7];

  CString             name0;    // "RANDOMDUNGEON"
  CString             unk2;     // ""

  u32 unk2[15];

  CString             location; // "MEDIA/QUESTS/VASMANRANDOM/ANOTHERPIECECAVES.DAT"
  CString             name1;    // "ANOTHERPIECECAVES"
  CString             name2;    // "|cFFFFBA00Another Piece|u"

  vector<CQuestDialog *>    *pCQuestDialogList0;
  vector<CQuestDialog *>    *pCQuestDialogList1;

  // And some more...


};
