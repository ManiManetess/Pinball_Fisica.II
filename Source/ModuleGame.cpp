#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

const float SCALE = 100.0f;         // Factor de escala, ajusta según sea necesario
const float RAD_TO_DEG = 57.2958f;  // 180 / π, convierte radianes a grados

// Declaración de texturas para los flippers
Texture2D leftFlipper;
Texture2D rightFlipper;

	int leftFlipperX = 100;
	int leftFlipperY = 400;
	int rightFlipperX = 300;
	int rightFlipperY = 400;

	void ModuleGame::CreateFlippers() {
		// Definición del flipper izquierdo
		b2BodyDef leftFlipperDef;
		leftFlipperDef.type = b2_dynamicBody;
		leftFlipperDef.position = leftFlipperPosition;
		leftFlipper = world->CreateBody(&leftFlipperDef);

		b2PolygonShape flipperShape;
		flipperShape.SetAsBox(0.5f, 0.1f);  // Tamaño del flipper, ajústalo según sea necesario

		b2FixtureDef leftFixtureDef;
		leftFixtureDef.shape = &flipperShape;
		leftFixtureDef.density = 1.0f;
		leftFlipper->CreateFixture(&leftFixtureDef);

		// Definición del flipper derecho
		b2BodyDef rightFlipperDef;
		rightFlipperDef.type = b2_dynamicBody;
		rightFlipperDef.position = rightFlipperPosition;
		rightFlipper = world->CreateBody(&rightFlipperDef);

		b2FixtureDef rightFixtureDef;
		rightFixtureDef.shape = &flipperShape;
		rightFixtureDef.density = 1.0f;
		rightFlipper->CreateFixture(&rightFixtureDef);
	}

	void ModuleGame::UpdateFlippers() {
		// Calcula la posición de los flippers en la pantalla
		Vector2 leftPosition = { leftFlipper->GetPosition().x * SCALE, leftFlipper->GetPosition().y * SCALE };
		Vector2 rightPosition = { rightFlipper->GetPosition().x * SCALE, rightFlipper->GetPosition().y * SCALE };

		// Dibuja las texturas de los flippers
		DrawTextureEx(leftFlipperTexture, leftPosition, leftFlipper->GetAngle() * RAD_TO_DEG, 1.0f, WHITE);
		DrawTextureEx(rightFlipperTexture, rightPosition, rightFlipper->GetAngle() * RAD_TO_DEG, 1.0f, WHITE);
	}


class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

protected:
	PhysBody* body;
	Module* listener;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateCircle(_x, _y, 2), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f};
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

private:
	Texture2D texture;

};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 50), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f}, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};

class Shape : public PhysicEntity
{
public:
	// Pivot 0, 0

	Shape(ModulePhysics* physics, int _x, int _y, int Points[], int num_points, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, Points, num_points), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};



ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	b2Vec2 gravity(0.0f, -9.8f);  // Ajusta la gravedad según sea necesario
	world = new b2World(gravity);  // Inicializa el mundo con la gravedad
	ray_on = false;
	sensed = false;
	leftFlipperTexture = LoadTexture("Assets/leftFlipper.png");
	rightFlipperTexture = LoadTexture("Assets/rightFlipper.png");
}

ModuleGame::~ModuleGame()
{
	UnloadTexture(leftFlipperTexture);
	UnloadTexture(rightFlipperTexture);
}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = LoadTexture("Assets/baseball.png"); 
	box = LoadTexture("Assets/crate.png");
	rick = LoadTexture("Assets/rick_head.png");

	Mapa_Pokemon = LoadTexture("Assets/staticPritesWindowSize.png");
	

	// Inicializa las texturas
	Texture2D leftFlipperTexture = LoadTexture("Assets/leftFlipper.png");
	Texture2D rightFlipperTexture = LoadTexture("Assets/rightFlipper.png");

	// Crea los cuerpos físicos en CreateFlippers()
	b2BodyDef leftFlipperDef;
	leftFlipperDef.type = b2_dynamicBody;
	leftFlipperDef.position = leftFlipperPosition;
	leftFlipper = world->CreateBody(&leftFlipperDef);

	b2BodyDef rightFlipperDef;
	rightFlipperDef.type = b2_dynamicBody;
	rightFlipperDef.position = rightFlipperPosition;
	rightFlipper = world->CreateBody(&rightFlipperDef);


	bonus_fx = App->audio->LoadFx("Assets/bonus.wav");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	CreateFlippers();

		int Pokemon_Map[94] = {
	471, 821,
	470, 164,
	443, 88,
	408, 47,
	369, 20,
	309, 0,
	250, -2,
	161, 8,
	123, 42,
	113, 83,
	121, 126,
	130, 149,
	141, 170,
	119, 139,
	107, 20,
	95, 6,
	77, 12,
	67, 61,
	70, 127,
	80, 176,
	91, 214,
	69, 143,
	69, 84,
	59, 31,
	34, 20,
	4, 31,
	3, 145,
	13, 250,
	38, 332,
	55, 376,
	61, 399,
	39, 457,
	27, 486,
	24, 534,
	3, 567,
	-1, 734,
	140, 821,
	-1, 824,
	-2, -2,
	486, 0,
	486, 824,
	257, 820,
	405, 733,
	402, 573,
	363, 521,
	431, 365,
	435, 819
	};

	int collisions[38] = {
		43, 167,
		69, 259,
		239, 343,
		242, 364,
		296, 385,
		276, 466,
		268, 463,
		288, 414,
		278, 399,
		266, 388,
		246, 391,
		231, 432,
		165, 395,
		162, 350,
		114, 340,
		120, 385,
		99, 377,
		33, 166,
		34, 57
	};

	int collisions2[16] = {
		345, 377,
		389, 323,
		377, 306,
		419, 229,
		412, 289,
		402, 327,
		392, 361,
		313, 487
	};

	int collisions3[12] = {
		305, 82,
		311, 43,
		344, 54,
		374, 76,
		396, 101,
		417, 152
	};

	int collisions4[6] = {
	113, 685,
	66, 661,
	67, 581
	};

	int collisions5[6] = {
	286, 684,
	330, 578,
	335, 661
	};

	int collisions6[8] = {
	373, 603,
	374, 688,
	291, 734,
	365, 681
	};

	int collisions7[8] = {
	35, 679,
	111, 731,
	27, 684,
	28, 601
	};

	int lil[6] = {
	200, 80,
	203, 46,
	206, 81
	};

	int lil2[6] = {
	233, 75,
	237, 41,
	243, 73
	};

	int lil3[6] = {
	269, 76,
	273, 39,
	276, 74
	};


	// Obtén la posición de los cuerpos físicos y conviértela a píxeles
	Vector2 leftPosition = { leftFlipper->GetPosition().x * SCALE, leftFlipper->GetPosition().y * SCALE };
	Vector2 rightPosition = { rightFlipper->GetPosition().x * SCALE, rightFlipper->GetPosition().y * SCALE };

	// Dibuja las texturas en las posiciones de los cuerpos físicos
	DrawTextureEx(leftFlipperTexture, leftPosition, leftFlipper->GetAngle()* RAD_TO_DEG, 1.0f, WHITE);
	DrawTextureEx(rightFlipperTexture, rightPosition, rightFlipper->GetAngle()* RAD_TO_DEG, 1.0f, WHITE);


	entities.emplace_back(new Shape(App->physics,  0,  0, Pokemon_Map,  94, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions,   38, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions2,  16, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions3,  12, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions4,   6, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions5,   6, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions6,   8, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, collisions7,   8, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, lil,   		  6, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, lil2,		  	  6, this, Mapa_Pokemon));
	entities.emplace_back(new Shape(App->physics,  0,  0, lil3,		  	  6, this, Mapa_Pokemon));

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleGame::HandleInput() {
	if (IsKeyPressed(KEY_A)) {
		// Gira el flipper izquierdo hacia arriba
		leftFlipper->SetTransform(leftFlipper->GetPosition(), -0.3f);  // Ajusta el ángulo según sea necesario
	}
	else if (IsKeyReleased(KEY_A)) {
		// Devuelve el flipper izquierdo a su posición inicial
		leftFlipper->SetTransform(leftFlipper->GetPosition(), 0.0f);
	}

	if (IsKeyPressed(KEY_D)) {
		// Gira el flipper derecho hacia arriba
		rightFlipper->SetTransform(rightFlipper->GetPosition(), 0.3f);  // Ajusta el ángulo según sea necesario
	}
	else if (IsKeyReleased(KEY_D)) {
		// Devuelve el flipper derecho a su posición inicial
		rightFlipper->SetTransform(rightFlipper->GetPosition(), 0.0f);
	}
}



// Update: draw background
update_status ModuleGame::Update()
{
	UpdateFlippers();

	if(IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}

	if(IsKeyPressed(KEY_ONE))
	{
		entities.emplace_back(new Circle(App->physics, GetMouseX(), GetMouseY(), this, circle));
		
	}

	if(IsKeyPressed(KEY_TWO))
	{
		entities.emplace_back(new Box(App->physics, GetMouseX(), GetMouseY(), this, box));
	}

	// Prepare for raycast ------------------------------------------------------
	
	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}
	

	// ray -----------------
	if(ray_on == true)
	{
		vec2f destination((float)(mouse.x-ray.x), (float)(mouse.y-ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonus_fx);
}
