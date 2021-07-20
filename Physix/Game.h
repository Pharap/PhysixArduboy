//
//  Copyright (C) 2018-2021 Pharap (@Pharap)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#pragma once

#include "Physics.h"

#include <Arduboy2.h>

class Game
{

public:
	/// Used for simulating friction.
	///
	/// Note: this is not how a real coefficient of friction works.
	static constexpr Number coefficientOfFriction = 0.95;

	/// Used for simulating gravity.
	///
	/// Earth's gravitational pull is (roughly) 9.8 m/s squared,
	/// but that's far too powerful for the tiny screen,
	/// so I picked something small.
	static constexpr Number coefficientOfGravity = 0.5;

	/// Used for simulating bounciness.
	///
	/// Note: this is not how a real coefficient of restitution works.
	static constexpr Number coefficientOfRestitution = 0.3;

	/// A threshold to ensure the bouncing stops at some point.
	static constexpr Number restitutionThreshold = (Number::Epsilon * 16);

	/// The amount of force the player exerts.
	static constexpr Number inputForce = 0.25;

private:
	/// An instance of the Arduboy2 API.
	Arduboy2 arduboy;

	/// The objects floating around the screen.
	RigidBody objects[8];

	/// A reference to the object that will represent the player.
	///
	/// `playerObject` always points to `objects[0]`,
	/// so the two can be considered interchangeable.
	RigidBody & playerObject = objects[0];

	/// Indicates whether gravity should be simulated or not.
	bool gravityEnabled = false;

	/// A vector representing the force of gravity.
	Vector2 gravitationalForce { 0, coefficientOfGravity };

	/// Indicates whether diagnostics should be rendered or not.
	bool statRenderingEnabled = true;

public:
	/// Performs necessary set up procedures.
	void setup()
	{
		// Initialise the Arduboy.
		arduboy.begin();

		// Randomise the objects.
		randomiseObjects();

		// Calculate the point at the centre of the screen.
		constexpr auto centreScreen = Point2(Number(arduboy.width() / 2), Number(arduboy.height() / 2));

		// Move the player's object to the centre of the screen, with zero velocity.
		playerObject.position = centreScreen;
		playerObject.velocity = Vector2(0, 0);
	}

	/// Loops continually.
	void loop()
	{
		// If it's not time to draw the next frame.
		if(!arduboy.nextFrame())
			// Exit the loop.
			return;

		// Update the Arduboy's button state variables.
		arduboy.pollButtons();

		// React to player input.
		updateInput();

		// Simulate physics.
		simulatePhysics();

		// Clear the screen.
		arduboy.clear();

		// Draw all objects (to the frame buffer).
		renderObjects();

		// If the diagnostics should be displayed.
		if(statRenderingEnabled)
			// Draw diagnostics.
			renderDisplay();

		// Update the screen.
		arduboy.display();
	}

	/// Randomises the positions and velocities of all objects.
	void randomiseObjects()
	{
		// For each object in objects...
		for(RigidBody & object : objects)
		{
			// Give the obejct a random on screen position.
			object.position = Point2(Number(random(arduboy.width())), Number(random(arduboy.height())));

			// Calculate a random x value.
			const auto xInteger = random(-8, 8);
			const auto xFraction = random(1 << Number::FractionSize);
			const auto xOffset = Number(xInteger, xFraction);

			// Calculate a random y value.
			const auto yInteger = random(-8, 8);
			const auto yFraction = random(1 << Number::FractionSize);
			const auto yOffset = Number(yInteger, yFraction);

			// If gravity is enabled...
			if(gravityEnabled)
			{
				// Adjust the object's vertical velocity.
				object.velocity += Vector2(0, yOffset);
			}
			// If gravity not enabled...
			else
			{
				// Adjust the object's full velocity.
				object.velocity += Vector2(xOffset, yOffset);
			}
		}
	}

	/// Renders all objects
	void renderObjects()
	{
		// Note that this time the index is being used to identify the player object.
		for(uint8_t index = 0; index < arrayLength(objects); ++index)
		{
			RigidBody & object = objects[index];

			// If the object isn't the player...
			if(index > 0)
				// Draw it as a filled rectangle.
				arduboy.fillRect(static_cast<int8_t>(object.getX()), static_cast<int8_t>(object.getY()), 8, 8);
			// If the object is the player...
			else
				// Draw it as an empty rectangle.
				arduboy.drawRect(static_cast<int8_t>(object.getX()), static_cast<int8_t>(object.getY()), 8, 8);
		}
	}

	/// Draws diagnostic information.
	void renderDisplay()
	{
		// Print whether gravity is enabled, and its direction.
		arduboy.println(F("Gravity"));
		arduboy.println(gravityEnabled ? F("ON") : F("OFF"));
		arduboy.println((gravitationalForce.y < 0) ? F("UP") : F("DOWN"));

		// Print the various coefficients.
		arduboy.print(F("G: "));
		arduboy.println(static_cast<float>(coefficientOfGravity));
		arduboy.print(F("F: "));
		arduboy.println(static_cast<float>(coefficientOfFriction));
		arduboy.print(F("R: "));
		arduboy.println(static_cast<float>(coefficientOfRestitution));
	}

	/// Updates the simulation state in reaction to player input.
	void updateInput()
	{
		// When the B button is held...
		if(arduboy.pressed(B_BUTTON))
		{
			// A - Shake up the other objects by applying random force.
			if(arduboy.justPressed(A_BUTTON))
				randomiseObjects();

			// Down - Toggle gravity on or off.
			if(arduboy.justPressed(DOWN_BUTTON))
				gravityEnabled = !gravityEnabled;

			// Up - Invert gravity.
			if(arduboy.justPressed(UP_BUTTON))
				gravitationalForce = -gravitationalForce;

			// Left - toggle statRenderingEnabled on/off
			if(arduboy.justPressed(LEFT_BUTTON))
				statRenderingEnabled = !statRenderingEnabled;
		}
		// When the B button isn't held...
		else
		{
			// Calculate the force to apply to the 'player' object
			// based upon which buttons the player is pressing.
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
			// to be enacted on the object that the player is controlling.
			playerObject.velocity += playerForce;

			// If A is pressed...
			if(arduboy.justPressed(A_BUTTON))
				// Perform an 'emergency stop' by zeroing the velocity.
				playerObject.velocity = Vector2(0, 0);
		}
	}

	/// Simulates the physics.
	void simulatePhysics()
	{
		// Precalculate the boundaries for the sides of the screen.
		constexpr int16_t screenLeft = 0;
		constexpr int16_t screenRight = (Arduboy2::width() - 8);
		constexpr int16_t screenTop = 0;
		constexpr int16_t screenBottom = (Arduboy2::height() - 8);

		// For each object in objects...
		for(RigidBody & object : objects)
		{
			// Simulate gravity.
			if(gravityEnabled)
				object.velocity += gravitationalForce;

			// Simulate friction.

			// If gravity is enabled...
			if(gravityEnabled)
				// Simulate only horizontal friction.
				object.velocity.x *= coefficientOfFriction;
			// If gravity isn't enabled...
			else
				// Simulate full friction.
				object.velocity *= coefficientOfFriction;

			// Keep the objects on screen by bouncing them off the walls.

			// If the object has gone off the left side of the screen...
			if(object.position.x < screenLeft)
			{
				// Move the object to the left hand side of the screen.
				object.position.x = screenLeft;

				// Reverse the object's horizontal velocity.
				object.velocity.x = -object.velocity.x;
			}

			// If the object has gone off the right side of the screen...
			if(object.position.x > screenRight)
			{
				// Move the object to the right hand side of the screen.
				object.position.x = screenRight;

				// Reverse the object's horizontal velocity.
				object.velocity.x = -object.velocity.x;
			}

			// If gravity is enabled...
			if(gravityEnabled)
			{
				// If the object has gone off the top side of the screen...
				if(object.position.y < screenTop)
				{
					// Move the object to the top side of the screen.
					object.position.y = screenTop;

					// If the object is moving faster (vertically) than the restitution threshold...
					if(object.velocity.y > restitutionThreshold)
					{
						// Reverse the object's vertical velocity.
						object.velocity.y = -object.velocity.y;

						// Reduce the object's velocity by the coefficient of restitution.
						object.velocity.y *= coefficientOfRestitution;
					}
					// If the object is moving slower (vertically) than the restitution threshold...
					else
					{
						// Set the object's vertical velocity to zero.
						// I.e. bring it to a vertical halt.
						object.velocity.y = 0;
					}
				}

				// If the object has gone off the bottom side of the screen...
				if(object.position.y > screenBottom)
				{
					// Move the object to the bottom side of the screen.
					object.position.y = screenBottom;

					// If the object is moving faster (vertically) than the restitution threshold...
					if(object.velocity.y > restitutionThreshold)
					{
						// Reverse the object's vertical velocity.
						object.velocity.y = -object.velocity.y;

						// Reduce the object's velocity by the coefficient of restitution.
						object.velocity.y *= coefficientOfRestitution;
					}
					// If the object is moving slower (vertically) than the restitution threshold...
					else
					{
						// Set the object's vertical velocity to zero.
						// I.e. bring it to a vertical halt.
						object.velocity.y = 0;
					}
				}
			}
			// If gravity isn't enabled...
			else
			{
				// If the object has gone off the top side of the screen...
				if(object.position.y < screenTop)
				{
					// Move the object to the top side of the screen.
					object.position.y = screenTop;

					// Reverse the object's vertical velocity.
					object.velocity.y = -object.velocity.y;
				}

				// If the object has gone off the bottom side of the screen...
				if(object.position.y > screenBottom)
				{
					// Move the object to the bottom side of the screen.
					object.position.y = screenBottom;

					// Reverse the object's vertical velocity.
					object.velocity.y = -object.velocity.y;
				}
			}

			// Finally, the object's update position using the object's velocity.
			object.position += object.velocity;
		}
	}
};
