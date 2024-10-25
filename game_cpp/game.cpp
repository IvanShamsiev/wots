#include "../framework/scene.hpp"
#include "../framework/game.hpp"

#include <memory>
#include <vector>

#include "vector.h"
#include "airplane.h"
#include "ship.h"


//-------------------------------------------------------
//	game public interface
//-------------------------------------------------------

namespace game
{
	Ship ship;
	std::vector<std::shared_ptr<Airplane>> airplanes;

	void init()
	{
		ship.init();
		Ship::setShip(ship);
	}


	void deinit()
	{
		ship.deinit();
		airplanes.clear();
	}


	void update( float dt )
	{
		ship.update( dt );
		for (auto airplane : airplanes)
			airplane->update(dt);
	}


	void keyPressed( int key )
	{
		ship.keyPressed( key );
	}


	void keyReleased( int key )
	{
		ship.keyReleased( key );
	}

	void spawnNewAirplane() {
		if (airplanes.size() >= params::aircraft::MAX_AIRPLANE_COUNT) return;
		auto airplane = std::make_shared<Airplane>();
		airplane->init(ship);
		airplanes.push_back(airplane);
	}

	void mouseClicked( float x, float y, bool isLeftButton )
	{
		Vector2 worldPosition( x, y );
		scene::screenToWorld( &worldPosition.x, &worldPosition.y );
		ship.mouseClicked( worldPosition, isLeftButton );

		if (isLeftButton)
			for (auto airplane : airplanes)
				airplane->changeTarget(worldPosition);
		else
			spawnNewAirplane();
	}
}

