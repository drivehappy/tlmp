#include "Common.h"

void TLMP::Helper_BuildInventoryTabIndexSize(NetworkMessages::Character & msgNewCharacter, CCharacter* character)
{
  multiplayerLogger.WriteLine(Info, L"Building inventory tab sizes for character: %p (%s)", character, character->characterName.c_str());
  log(L"Building inventory tab sizes for character: %p (%s)", character, character->characterName.c_str());

  // Build the inventory tab indicies/tab size
  if (character->pCInventory) {
    CInventory *inv = character->pCInventory;

    multiplayerLogger.WriteLine(Info, L"  Tab index count: %i", inv->tabIndices.size());
    log(L"  Tab index count: %i", inv->tabIndices.size());

    for (u32 i = 0; i < inv->tabIndices.size(); ++i) {
      NetworkMessages::InventoryTabSize *msgTabSize = msgNewCharacter.add_inventory_tabsize();
      msgTabSize->set_tabindex(inv->tabIndices[i]);

      multiplayerLogger.WriteLine(Info, L"    Tab[%i] = %i", inv->tabIndices[i], inv->tabSizes[i]);
      log(L"    Tab[%i] = %i", inv->tabIndices[i], inv->tabSizes[i]);

      u32 index = inv->tabIndices[i];
      if (index + 1 < inv->tabIndices.size()) {
        // Use the next index to compute size
        msgTabSize->set_tabsize(inv->tabSizes[i + 1] - inv->tabSizes[i]);
      } else {
        // Use the inventory's max size to compute the remaineder
        msgTabSize->set_tabsize(inv->maxSize - inv->tabSizes[i]);
      }
    }
  }
}

void TLMP::Helper_ExtractInventoryTabIndexSize(const NetworkMessages::Character * msgNewCharacter, CCharacter* character)
{
  u32 invTabCount = msgNewCharacter->inventory_tabsize_size();
  multiplayerLogger.WriteLine(Info, L"  Retrieved %i tab sizes:", invTabCount);
  log(L"  Retrieved %i tab sizes:", invTabCount);

  character->pCInventory->maxSize = 0x13;   // Default size observed
  character->pCInventory->tabIndices.clear();
  character->pCInventory->tabSizes.clear();

  for (u32 i = 0; i < invTabCount; ++i) {
    const NetworkMessages::InventoryTabSize msgInvTabSize = msgNewCharacter->inventory_tabsize().Get(i);
    multiplayerLogger.WriteLine(Info, L"    [%i]: index = %i, size = %i", i, msgInvTabSize.tabindex(), msgInvTabSize.tabsize());
    log(L"    [%i]: index = %i, size = %i", i, msgInvTabSize.tabindex(), msgInvTabSize.tabsize());

    character->pCInventory->AddTabSize(msgInvTabSize.tabindex(), msgInvTabSize.tabsize());
  }
}

void TLMP::HelperCharacterPositioning(CCharacter* character, const Vector3& position)
{
  // If the current positioning is off, fix it - don't adjust our own character though
  const int ALLOWED_SQUARED_ERROR = 100;
  vector<CCharacter*>::iterator itr;
  vector<CCharacter*> *ignoredCharacters = gameClient->pCPlayer->GetMinions();
  ignoredCharacters->push_back(gameClient->pCPlayer);
  bool bFound = false;

  for (itr = ignoredCharacters->begin(); itr != ignoredCharacters->end(); itr++) {
    if (character == (*itr)) {
      bFound = true;
      break;
    }
  }

  if (!bFound) {
    if (position.length() > 0.001f) {
      if ((character->GetPosition() - position).squaredLength() > ALLOWED_SQUARED_ERROR) {
        // Turn off for now
        log(L"Setting character explicit position: %f %f %f", position.x, position.y, position.z);
        character->SetPosition(&position);
      }
    }
  }
}
