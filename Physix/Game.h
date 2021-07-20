/*
   Copyright (C) 2018 Pharap (@Pharap)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include "Physics.h"

#include <Arduboy2.h>

class Game
{

public:
	// Simulates friction
	// Not actually how a real coefficient of friction works
	static constexpr Number coefficientOfFriction = 0.95;

	// Simulates gravity
	// Earth's gravitational pull is 9.8 m/s squared
	// But that's far too powerful for the tiny screen
	// So I picked something small
	static constexpr Number coefficientOfGravity = 0.5;

	// Simulates bounciness
	// Again, not quite like the real deal
	static constexpr Number coefficientOfRestitution = 0.3;

	// Prevents never-ending bounciness
	static constexpr Number restitutionThreshold = Number::Epsilon * 16;

	// Amount of force the player exerts
	static constexpr Number inputForce = 0.25;

private:
	Arduboy2 arduboy;

	RigidBody objects[8];

	// playerObject always points to objects[0]
	// The two can be considered interchangeable
	RigidBody & playerObject = objects[0];

	bool gravityEnabled = false;
	Vector2 gravitationalForce { 0, coefficientOfGravity };

	bool statRenderingEnabled = true;

public:
	void randomiseObjects()
	{
		for(uint8_t i = 0; i < arrayLength(objects); ++i)
		{
			RigidBody & object = objects[i];

			object.position = Point2(Number(random(arduboy.width())), Number(random(arduboy.height())));

			// If gravity is enabled, only affect y
			if(gravityEnabled)
			{
				const auto integer = random(-8, 8);
				const auto fraction = random(1 << Number::FractionSize);

				object.velocity.y += Number(integer, fraction);
			}
			// If gravity not enabled, affect both dimensions
			else
			{
				const auto xInteger = random(-8, 8);
				const auto xFraction = random(1 << Number::FractionSize);
				const auto xOffset = Number(xInteger, xFraction);

				const auto yInteger = random(-8, 8);
				const auto yFraction = random(1 << Number::FractionSize);
				const auto yOffset = Number(yInteger, yFraction);

				object.velocity += Vector2(xOffset, yOffset);
			}
		}
	}

	void setup()
	{
		arduboy.begin();

		randomiseObjects();

		constexpr auto centreScreen = Point2(Number(arduboy.width() / 2), Number(arduboy.height() / 2));

		playerObject.position = centreScreen;
		playerObject.velocity = Vector2(0, 0);
	}

	void loop()
	{
		if(!arduboy.nextFrame())
			return;

		arduboy.pollButtons();

		updateInput();
		simulatePhysics();

		arduboy.clear();

		renderObjects();

		if(statRenderingEnabled)
			renderDisplay();

		arduboy.display();
	}

	void renderObjects()
	{
		for(uint8_t index = 0; index < arrayLength(objects); ++index)
		{
			RigidBody & object = objects[index];

			if(index > 0)
				arduboy.fillRect(static_cast<int8_t>(object.getX()), static_cast<int8_t>(object.getY()), 8, 8);
			else
				arduboy.drawRect(static_cast<int8_t>(object.getX()), static_cast<int8_t>(object.getY()), 8, 8);
		}
	}

	void renderDisplay()
	{
		arduboy.println(F("Gravity"));
		arduboy.println(gravityEnabled ? F("ON") : F("OFF"));
		arduboy.println((gravitationalForce.y < 0) ? F("UP") : F("DOWN"));

		arduboy.print(F("G: "));
		arduboy.println(static_cast<float>(coefficientOfGravity));
		arduboy.print(F("F: "));
		arduboy.println(static_cast<float>(coefficientOfFriction));
		arduboy.print(F("R: "));
		arduboy.println(static_cast<float>(coefficientOfRestitution));
	}

	void updateInput()
	{
		// Input tools for playing around
		if(arduboy.pressed(B_BUTTON))
		{
			// A - shake up the other objects by applying random force
			if(arduboy.justPressed(A_BUTTON))
				randomiseObjects();

			// Down - toggle gravity on/off
			if(arduboy.justPressed(DOWN_BUTTON))
				gravityEnabled = !gravityEnabled;

			// Up - invert gravity
			if(arduboy.justPressed(UP_BUTTON))
				gravitationalForce = -gravitationalForce;

			// Left - toggle statRenderingEnabled on/off
			if(arduboy.justPressed(LEFT_BUTTON))
				statRenderingEnabled = !statRenderingEnabled;
		}
		// Input for normal object control
		else
		{
			Vector2 playerForce = Vector2(0, 0);

			if(arduboy.pressed(LEFT_BUTTON))
				playerForce.x += -inputForce;

			if(arduboy.pressed(RIGHT_BUTTON))
				playerForce.x += inputForce;

			if(arduboy.pressed(UP_BUTTON))
				playerForce.y += -inputForce;

			if(arduboy.pressed(DOWN_BUTTON))
				playerForce.y += inputForce;

			// The player's input can be thought of as a force
			// to be enacted on the object that the player is controlling
			playerObject.velocity += playerForce;

			// Emergency stop
			if(arduboy.justPressed(A_BUTTON))
				playerObject.velocity = Vector2(0, 0);
		}
	}

	void simulatePhysics()
	{
		// Update objects
		for(uint8_t index = 0; index < arrayLength(objects); ++index)
		{
			// object refers to the given item in the array
			RigidBody & object = objects[index];

			// First, simulate gravity
			if(gravityEnabled)
				object.velocity += gravitationalForce;

			// Then, simulate friction
			if(gravityEnabled)
				// If gravity is enabled, just simulate horizontal friction
				object.velocity.x *= coefficientOfFriction;
			else
				// If gravity isn't enabled, simulate top-down friction
				object.velocity *= coefficientOfFriction;

			// Then, keep the objects onscreen
			// (A sort of cheaty way of keeping the objects onscreen)

			// They're literally bouncing off the walls :P
			if(object.position.x < 0)
			{
				object.position.x = 0;
				object.velocity.x = -object.velocity.x;
			}

			if(object.position.x > arduboy.width() - 8)
			{
				object.position.x = (arduboy.width() - 8);
				object.velocity.x = -object.velocity.x;
			}

			// If gravity is enabled, gradually have the object come to a halt
			// This would be easier to understand with a diagram
			if(gravityEnabled)
			{
				if(object.position.y < 0)
				{
					object.position.y = 0;

					if(object.velocity.y > restitutionThreshold)
						object.velocity.y = -object.velocity.y * coefficientOfRestitution;
					else
						object.velocity.y = 0;
				}
				if(object.position.y > arduboy.height() - 8)
				{
					object.position.y = (arduboy.height() - 8);

					if(object.velocity.y > restitutionThreshold)
						object.velocity.y = -object.velocity.y * coefficientOfRestitution;
					else
						object.velocity.y = 0;
				}
			}
			// If gravity isn't enabled, bounce off the y sides as well
			else
			{
				if(object.position.y < 0)
				{
					object.position.y = 0;
					object.velocity.y = -object.velocity.y;
				}

				if(object.position.y > arduboy.height() - 8)
				{
					object.position.y = (arduboy.height() - 8);
					object.velocity.y = -object.velocity.y;
				}
			}

			// Finally, update position using velocity
			object.position += object.velocity;
		}
	}
};
