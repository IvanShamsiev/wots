#include "../framework/scene.hpp"
#include "../framework/game.hpp"

#include <algorithm>
#include <memory>
#include <vector>
#include <atomic>

#include "vector.h"
#include "airplane.h"
#include "scheduler.h"
#include "ship.h"
#include "params.h"


//-------------------------------------------------------
//	game public interface
//-------------------------------------------------------

namespace game
{
	Scheduler scheduler;
	
	std::shared_ptr<Ship> ship;
	std::vector<std::shared_ptr<Airplane>> airplanes;
	std::atomic<int> airplanes_count; // not important to make it atomic, game works in one thread

	Vector2 currentTarget(0.0f, 0.0f);

	void init()
	{
		ship = std::make_shared<Ship>();
		ship->init();
		Ship::setInstance(ship);
		airplanes_count = 0;
	}


	void deinit()
	{
		if (ship) ship->deinit();
		for (auto& airplane : airplanes)
			airplane->deinit();
		airplanes.clear();
	}


	void update(float dt)
	{
		auto it = std::remove_if(airplanes.begin(), airplanes.end(), [](std::shared_ptr<Airplane>& airplane) {
			if (airplane->state == Airplane::State::END) {
				airplane->deinit();
				scheduler.scheduleTask([](){ --airplanes_count;}, params::aircraft::AIRPLANE_REST_TIME);
				return true;
			}
			return false;
		});
		airplanes.erase(it, airplanes.end());
		scheduler.update(dt);
		if (ship) ship->update( dt );
		for (auto& airplane : airplanes)
			airplane->update(dt);
	}


	void keyPressed( int key )
	{
		if (ship) ship->keyPressed( key );
	}


	void keyReleased( int key )
	{
		if (ship) ship->keyReleased( key );
	}

	void spawnNewAirplane() {
		if (airplanes_count >= params::aircraft::MAX_AIRPLANE_COUNT) return;
		auto airplane = std::make_shared<Airplane>();
		airplane->init(currentTarget);
		airplanes.push_back(airplane);
		++airplanes_count;
	}

	void mouseClicked( float x, float y, bool isLeftButton )
	{
		Vector2 worldPosition( x, y );
		scene::screenToWorld( &worldPosition.x, &worldPosition.y );
		if (ship) ship->mouseClicked( worldPosition, isLeftButton );

		if (isLeftButton) {
			currentTarget = worldPosition;
			for (auto airplane : airplanes) {
				airplane->changeTarget(currentTarget);
			}
		}
		else
			spawnNewAirplane();
	}
}

