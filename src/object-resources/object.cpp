#include "object.hpp"

#include <iostream>

using namespace mage;

GameObject::GameObject(unsigned int id) : object_id{id}{
	// placeholder constructor
}

GameObject GameObject::create_game_object(){
	std::cout << " - creating game object and assigning id..." << std::endl;
	static unsigned int current_num = 0;
	return GameObject{current_num++};
}

GameObject::~GameObject(){
	// placeholder deconstructor
}