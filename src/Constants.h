#pragma once

// Constants used

namespace TLMP
{
  // Distance constants
  const float EPSILON = 0.01f;
  const u32 ALLOWED_SQUARED_ERROR = 81;      // Used for strict positioning when out of sync characters

  // List of NPCs the client cannot control (e.g. quest cinematics)
  const u64 INVALID_CLIENT_CONTROLLED_GUID[] = {
    0xB2F3B91E6D1611DE,     // Brink the Corrupted
    0x501C13F086BA11DE,     // Alric good
    0x5B1924F288F211DE,     // Alric evil
    0x6A7EB662723311DE,     // Shade
    0xAA0CB4594A3511DE,     // Syl
  };
  const u32 SIZE_CLIENT_CONTROLLED = sizeof(INVALID_CLIENT_CONTROLLED_GUID) / sizeof(INVALID_CLIENT_CONTROLLED_GUID[0]);

  // Character state
  const u32 IDLE = 0x0;
  const u32 ATTACKING = 0x3;
  const u32 AGGRO = 0x4;
  const u32 DYING = 0x5;
  const u32 DEAD = 0x6;
  const u32 CASTING = 0xE;
  const u32 TRADING = 0x14;

  // Unittypes.hie
  const u32 OPENABLE = 0x28;
  const u32 INTERACTABLE = 0x20;
  const u32 MAP_PORTAL = 0xAD;
  const u32 GOLD = 0x22;
  const u32 BREAKABLE = 0x1D;
  const u32 WAYPOINTNODE = 0xAB;
  const u32 TOWNPORTAL = 0x2B;

  // vtables
  const u32 CPLAYER_BASE = 0xA80064;
  const u32 CMONSTER_BASE = 0xA7F97C;

  // GUIDs
  const u64 CAT = 0x5C5BBC74483A11DE;
  const u64 DOG = 0xD3A8F9832FA111DE;
  const u64 DESTROYER = 0xD3A8F9982FA111DE;
  const u64 ALCHEMIST = 0x8D3EE5363F7611DE;
  const u64 VANQUISHER = 0xAA472CC2629611DE;
  const u64 BRINK = 0xBC1E373A723411DE;
  const u64 STASH = 0x258372C33F2411DE;
  const u64 SHAREDSTASH = 0xFC4F7F1F9D8E11DE;

};
