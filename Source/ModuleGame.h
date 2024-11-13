#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

#include "Box2D/Box2D.h"

class PhysBody;
class PhysicEntity;


class ModuleGame : public Module
{
public:

	b2World* world;

	b2Vec2 leftFlipperPosition = b2Vec2(1.5f, 4.0f);  // Ajusta las posiciones según la mesa
	b2Vec2 rightFlipperPosition = b2Vec2(5.0f, 4.0f);

	b2Body* leftFlipper;
	b2Body* rightFlipper;

	void CreateFlippers();
	void UpdateFlippers();
	void HandleInput();
	void Render();


	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);


public:

	std::vector<PhysicEntity*> entities;
	
	PhysBody* sensor;
	bool sensed;

	Texture2D circle;
	Texture2D box;
	Texture2D rick;
	Texture2D Mapa_Pokemon;

	Texture2D leftFlipperTexture;
	Texture2D rightFlipperTexture;

	uint32 bonus_fx;

	vec2<int> ray;
	bool ray_on;
};
