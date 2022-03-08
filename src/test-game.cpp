#include "test-game.hpp"

using namespace mage;

TestGame::TestGame() {
// Placeholder constructor
}

// Run window until user wants to close it
void TestGame::run() {
	while(!test_game.close_window()){
		glfwPollEvents();
	}
}

TestGame::~TestGame() {
// Placeholder destructor
}
