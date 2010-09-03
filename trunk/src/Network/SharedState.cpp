#include "SharedState.h"
using namespace TLMP;

vector<NetworkEntity*> *TLMP::NetworkSharedEquipment  = new vector<NetworkEntity*>();
vector<NetworkEntity*> *TLMP::NetworkSharedCharacters = new vector<NetworkEntity*>();
vector<NetworkEntity*> *TLMP::ClientTemporaryEquipment = new vector<NetworkEntity*>();
map<SystemAddress, vector<NetworkEntity*>*> *TLMP::Server_ClientCharacterMapping = new map<SystemAddress, vector<NetworkEntity*>*>();
vector<NetworkEntity*> *TLMP::ServerEquipmentOnGround = new vector<NetworkEntity*>();
vector<NetworkEntity*> *TLMP::NetworkSharedLevelItems = new vector<NetworkEntity*>();

vector<CBaseUnit*> *TLMP::OutsideBaseUnits = new vector<CBaseUnit*>();

