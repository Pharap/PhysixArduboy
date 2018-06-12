#pragma once

#include "Physics.h"

#include <Arduboy2.h>

class Game
{
private:
	Arduboy2 arduboy;
	RigidBody objects[8];

public:

	void setup(void)
	{
		arduboy.begin();
		
		for(uint8_t i = 0; i < arrayLength(objects); ++i)
		{
			RigidBody & object = objects[i];
			
			object.position.x = random(arduboy.width());
			object.position.y = random(arduboy.height());
			
			object.velocity.x = random(-3, 3);
			object.velocity.y = random(-3, 3);
		}
	}
	
	void loop(void)
	{
		if(!arduboy.nextFrame())
			return;
			
		arduboy.pollButtons();
		
		// Update objects
		for(uint8_t i = 0; i < arrayLength(objects); ++i)
		{
			RigidBody & object = objects[i];
			object.position += object.velocity;

			if(object.position.x < 8 || object.position.x > arduboy.width() + 8)
				object.velocity.x = -object.velocity.x;
				
			if(object.position.y < 8 || object.position.y > arduboy.height() + 8)
				object.velocity.y = -object.velocity.y;
		}
		
		arduboy.clear();
		
		// Render objects
		for(uint8_t i = 0; i < arrayLength(objects); ++i)
		{
			RigidBody & object = objects[i];
			arduboy.fillRect(static_cast<int16_t>(object.getX()), static_cast<int16_t>(object.getY()), 8, 8);
		}
		
		arduboy.display();
	}
};