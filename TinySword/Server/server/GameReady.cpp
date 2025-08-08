#include "Manager.h"


void GameReadyExecute() {

	std::cout << ">>>>>>>>>>>>>>>Entered in Game Ready!<<<<<<<<<<<<<<<" << std::endl;
	// CASTLE 
	//CastleManager castleManager;

	/*SheepManager* sheepManager = sheepManager->getInstance(); 
	CastleManager* castleManager = castleManager->getInstance(); 
	GoldmineManager* goldmineManager = goldmineManager->getInstance();*/


	for (int i = 1; i < 5; i++) {
		castleManager->CreateCastle(i);
	}
	std::cout << "CastleEntry SIZE : " << castleManager->GetCastleEntry().size() << std::endl;

	// GOLDMINE
	/*GoldmineManager goldmineManager;*/
	for (int i = 1; i < 3; i++) {
		goldmineManager->CreateGoldmine(i);
	}

	// SHEEP 
	/*SheepManager sheepManager; */
	for (int i = 1; i < 5; i++) {
		sheepManager->CreateSheep(i);
	}

	std::cout << "Game Ready End." << std::endl;
}
