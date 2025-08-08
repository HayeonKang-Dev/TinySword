#pragma once

#ifndef MANAGER_H

#define MANAGER_H

#include "CastleManager.h"
#include "GoldmineManager.h"
#include "SheepManager.h"
#include "ItemManager.h"
//#include "BombManager.h"
class BombManager;


extern CastleManager* castleManager; 
extern GoldmineManager* goldmineManager; 
extern SheepManager* sheepManager; 
extern ItemManager* itemManager;
extern BombManager* bombManager; 

#endif // !MANAGER_H
