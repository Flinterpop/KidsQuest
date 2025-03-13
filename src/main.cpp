
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <vector>
#include <algorithm>



enum ACTION_STATE { IDLE, WALK, RUN};
enum DIRECTION { LEFT = -1, RIGHT = 1 };
enum SCENE_ENTRY_POSITIONS { POSITION_ONE, POSITION_TWO, POSITION_THREE, POSITION_FOUR, POSITION_FIVE };
#define IMGBLUE CLITERAL(Color) { 115, 140, 153, 255 }

#define SCENE_WIDTH 1200
#define SCENE_HEIGHT 800



class _Explosion
{
	static const int NUM_FRAMES_PER_LINE = 5;
	static const int NUM_LINES = 5;

private: 
	// Init variables for animation
	static float frameWidth;  // Sprite one frame rectangle width
	static float frameHeight;  // Sprite one frame rectangle height
	static Rectangle frameRec; // = {0, 0, frameWidth, frameHeight};

	int currentFrame = 0;
	int currentLine = 0;


	Vector2 position;

	bool active = false;
	int framesCounter = 0;

	static Sound fxBoom;
	static Texture2D explosion;

public:

	static bool Initialize()
	{
		fxBoom = LoadSound("audio/boom.wav");
		explosion = LoadTexture("explosion.png");

		frameWidth = (float)(explosion.width / NUM_FRAMES_PER_LINE);   // Sprite one frame rectangle width
		frameHeight = (float)(explosion.height / NUM_LINES);           // Sprite one frame rectangle height
		frameRec = { 0, 0, frameWidth, frameHeight };
		return false;
	};

	
	~_Explosion()
	{
		UnloadTexture(explosion);
	}

	_Explosion(Vector2 _position)
	{
		active = true;

		position.x = _position.x - (frameWidth / 2.0f);
		position.y = _position.y - (frameHeight / 2.0f);

		PlaySound(fxBoom);
		printf("New Explosion at %f %f\r\n", _position.x, _position.y);

	}

	void Draw()
	{
		if (!active) return; 
			framesCounter++;
			if (framesCounter > 2)
			{
				currentFrame++;
				if (currentFrame >= NUM_FRAMES_PER_LINE)
				{
					currentFrame = 0;
					currentLine++;
					if (currentLine >= NUM_LINES)
					{
						currentLine = 0;
						active = false;
					}
				}
				framesCounter = 0;
			}
		frameRec.x = frameWidth * currentFrame;
		frameRec.y = frameHeight * currentLine;
		DrawTextureRec(explosion, frameRec, position, WHITE);
	}

};

Sound _Explosion::fxBoom;
Texture2D _Explosion::explosion;
float _Explosion::frameWidth = 0;  // Sprite one frame rectangle width
float _Explosion::frameHeight = 0;  // Sprite one frame rectangle height
Rectangle _Explosion::frameRec = {0, 0, frameWidth, frameHeight};

std::vector<_Explosion*> bombList;

class _Wabbit
{
	static const int NUM_FRAMES_PER_LINE = 1;
	static const int NUM_LINES = 1;

private:
	// Init variables for animation
	static float frameWidth;  // Sprite one frame rectangle width
	static float frameHeight;  // Sprite one frame rectangle height
	static Rectangle frameRec; // = {0, 0, frameWidth, frameHeight};

	int currentFrame = 0;
	int currentLine = 0;


	Vector2 position;

	bool active = false;
	int framesCounter = 0;

	static Sound fxWaah;
	static Texture2D wabbit;

	float vel = 0;
	float mass = 0.5f;
	float accel = 1.0f;

public:

	static bool Initialize()
	{
		fxWaah = LoadSound("audio/rabbit-falling-in-gopher-s-underground-tunnel.wav");
		wabbit = LoadTexture("wabbit_alpha.png");

		frameWidth = (float)(wabbit.width / NUM_FRAMES_PER_LINE);   // Sprite one frame rectangle width
		frameHeight = (float)(wabbit.height / NUM_LINES);           // Sprite one frame rectangle height
		frameRec = { 0, 0, frameWidth, frameHeight };
		return false;
	};


	~_Wabbit()
	{
		UnloadTexture(wabbit);
	}

	_Wabbit(Vector2 _position)
	{
		active = true;

		position.x = _position.x - (frameWidth / 2.0f);
		position.y = _position.y - (frameHeight / 2.0f);

		PlaySound(fxWaah);
		printf("New Wabbit at %f %f\r\n", _position.x, _position.y);

	}

	void Draw()
	{
		if (!active) return;
		framesCounter++;
		if (framesCounter > 2)
		{
			currentFrame++;
			if (currentFrame >= NUM_FRAMES_PER_LINE)
			{
				currentFrame = 0;
				currentLine++;
				if (currentLine >= NUM_LINES)
				{
					currentLine = 0;
					//active = false;
				}
			}
			framesCounter = 0;
		}
		frameRec.x = frameWidth * currentFrame;
		frameRec.y = frameHeight * currentLine;
		DrawTextureRec(wabbit, frameRec, position, VIOLET);

		vel += mass * accel;
		float dy = vel;
		position.y += dy;
		if (position.y > 700.0f)
		{
			//StopSound(fxWaah);
			active = false;
			_Explosion *e = new _Explosion(position);
			bombList.push_back(e);

		}

	}

};


Sound _Wabbit::fxWaah;
Texture2D _Wabbit::wabbit;
float _Wabbit::frameWidth = 0;  // Sprite one frame rectangle width
float _Wabbit::frameHeight = 0;  // Sprite one frame rectangle height
Rectangle _Wabbit::frameRec = { 0, 0, frameWidth, frameHeight };




class _Scene
{
	//SCENE size is 1200 x 800 => SCENE_WIDTH ,SCENE_HEIGHT
private:
	Rectangle SourceRect;
	Rectangle DestRect{ 0, 0, SCENE_WIDTH ,SCENE_HEIGHT};

	//Sound fxBoom;
	Texture2D sceneTx;


public:
	float MaxScale = 1.2; //size of character at bottom of scene
	float MinScale = 0.25;  ////size of character at top of scene
	float TopLimit = 550;  //typically 0
	float BottomLimit = SCENE_HEIGHT;
	float LeftLimit = 100;
	float RightLimit = SCENE_WIDTH;

	void SetSceneLimits(float maxScale, float minScale, float leftLimit, float rightLimit, float topLimit, float bottomLimit)
	{
		MaxScale = maxScale;
		MinScale = minScale;
		TopLimit = topLimit;
		BottomLimit = bottomLimit;
		LeftLimit = leftLimit;
		RightLimit = rightLimit;
	}

	float getScale(float Y)  //Y in range 0 to 800 (but less based on size of characer) 
	{
		float SceneBndBox = BottomLimit - TopLimit;  
		float ScaleRange = MaxScale - MinScale;

		float deltaY_BndBox = Y - TopLimit;
		float yPoportion = deltaY_BndBox / SceneBndBox;
		float scale = ScaleRange * yPoportion + MinScale;

		

		return scale;
	}




public:

	_Scene(char* fname)
	{
		//fxBoom = LoadSound("audio/boom.wav");
		sceneTx = LoadTexture(fname);
		SourceRect = { 0.0f,  0.0f, (float)sceneTx.width,(float)sceneTx.height };
	}


	~_Scene()
	{
		UnloadTexture(sceneTx);
	}

	void Draw()
	{
		DrawTexturePro(sceneTx, SourceRect, DestRect, { 0,0 }, 0, RAYWHITE);
		//DrawTexture(sceneTx, 0,0 , RAYWHITE);
	}

	Vector2 GetStartPos(SCENE_ENTRY_POSITIONS pos)
		//enum SCENE_ENTRY_POSITIONS { POSITION_ONE, POSITION_TWO, POSITION_THREE, POSITION_FOUR, POSITION_FIVE };
	{
		switch (pos)
		{
		case POSITION_ONE:
			return { 681,600 };
			break;
		case POSITION_TWO :
			return { 854,790 };
			break;
		case POSITION_THREE:
			return { 600,700 };
			break;
		}

	}
};





class _Character
{
	//const int NUM_FRAMES_PER_LINE = 4;
	int NUM_FRAMES_PER_LINE = 9;
	int NUM_LINES = 1;

private:
	// Init variables for animation
	float characterWidth;  // Sprite one frame rectangle width
	float characterHeight;  // Sprite one frame rectangle height
	Rectangle frameRec; // = {0, 0, frameWidth, frameHeight};

	int currentFrame = 0;
	int currentLine = 0;

	DIRECTION direction = LEFT;
	ACTION_STATE actionState = IDLE;

	Vector2 position;
	Vector2 positionTL = { 0.0,0.0 };
	Vector2 positionOffset;
	float scale = 1.0;

	float TopLimit;
	float BottomLimit;
	float LeftLimit;
	float RightLimit;

	Vector2 GoToPosition;

	bool active = false;
	int framesCounter = 0;
	Sound fxBoom;
	Texture2D character_idle, character_walk, character_run;
	Texture2D* character;

	int IDLE_NUM_FRAMES_PER_LINE = 4;
	int WALK_NUM_FRAMES_PER_LINE = 8;
	int RUN_NUM_FRAMES_PER_LINE = 9;

	bool isInAScene = false;
	_Scene *MyScene = NULL;


public:
	int framesSpeed = 8;            // Number of spritesheet frames shown by second

	~_Character()
	{
		UnloadTexture(character_idle);
		UnloadTexture(character_walk);
		UnloadTexture(character_run);
	}

	_Character(Vector2 _position) //constructor
	{
		position = _position;
		GoToPosition = position;

		fxBoom = LoadSound("audio/boom.wav");
		//character = LoadTexture("scarfy.png");
		character_idle = LoadTexture("knight_idle.png");
		character_walk = LoadTexture("knight_walk.png");
		character_run = LoadTexture("knight_run.png");
		character = &character_idle;
		NUM_FRAMES_PER_LINE = IDLE_NUM_FRAMES_PER_LINE;

		characterWidth = (float)(character->width / NUM_FRAMES_PER_LINE);   // Sprite one frame rectangle width
		characterHeight = (float)(character->height / NUM_LINES);           // Sprite one frame rectangle height
		frameRec = { 0, 0, characterWidth, characterHeight };
		positionOffset.x = (characterWidth / 2.0f);
		positionOffset.y = (characterHeight / 2.0f);

		printf("New scarfy centred on %f %f\r\n", position.x, position.y);
		printf("New scarfy offset  %f %f\r\n", positionOffset.x, positionOffset.y);
		//printf("New scarfy positionTL  %f %f\r\n", positionTL.x, positionTL.y);
		printf("scarfy size %f %f\r\n", characterWidth, characterHeight);

		TopLimit = (scale * characterHeight) / 2;
		BottomLimit = SCENE_HEIGHT - characterHeight / 2;
		LeftLimit = (scale * characterHeight) / 2;
		RightLimit = SCENE_WIDTH - (scale * characterHeight) / 2;

		printf("Limits: %f %f %f %f\r\n", TopLimit, BottomLimit, LeftLimit, RightLimit);
	}

	void EnterScene(_Scene * p, Vector2 newPos)
	{
		MyScene = p;
		isInAScene = true;
		position = newPos;
		GoToPosition = position;

		scale = 1.0;  //reset
		TopLimit = MyScene->TopLimit;
		BottomLimit = MyScene->BottomLimit;
		LeftLimit = MyScene->LeftLimit;
		RightLimit = MyScene->RightLimit;

		printf("New Scene Limits: %f %f %f %f\r\n", TopLimit, BottomLimit, LeftLimit, RightLimit);
	}

	Vector2 lastPosition = { 0.0f,0.0f };
	void Draw()
	{
		if ((lastPosition.x == position.x) && (lastPosition.y == position.y)) actionState = IDLE;

		switch (actionState)
		{
		case IDLE:
			NUM_FRAMES_PER_LINE = IDLE_NUM_FRAMES_PER_LINE;
			character = &character_idle;
			//puts("IDLE");
			break;
		case WALK:
			NUM_FRAMES_PER_LINE = WALK_NUM_FRAMES_PER_LINE;
			character = &character_walk;
			puts("WALK");
			break;
		case RUN:
			NUM_FRAMES_PER_LINE = RUN_NUM_FRAMES_PER_LINE;
			character = &character_run;
			puts("RUN");
			break;

		}
		framesCounter++;
		if (framesCounter >= (60 / framesSpeed))
		{
			framesCounter = 0;
			currentFrame++;
			if (currentFrame > (NUM_FRAMES_PER_LINE-1)) currentFrame = 0;
			frameRec.x = (float)currentFrame * (float)character->width / NUM_FRAMES_PER_LINE;
		}
		
		positionOffset.x = scale * (characterWidth / 2.0f);
		positionOffset.y = scale *(characterHeight / 2.0f);
		positionTL.x = position.x - positionOffset.x;
		positionTL.y = position.y - positionOffset.y;

		float heightInScene = positionTL.y / 800.0f; //0 is top,  SCENE_HEIGHT - frameHeight)'

		if (isInAScene)
		{
			scale = MyScene->getScale(position.y);
		}
		else scale = heightInScene;
		Rectangle dest{ positionTL.x, positionTL.y, scale * characterWidth, scale * characterHeight };
		DrawTexturePro(*character, {frameRec.x,frameRec.y,static_cast<float>(direction) * frameRec.width,frameRec.height }, dest, { 0,0 }, 0.0, WHITE);

		lastPosition = position;
	}

	void MoveBy(Vector2 deltaPos)
	{
		//printf("Pre scarfy pos %f %f\r\n", position.x, position.y);
		position.x += deltaPos.x;
		position.y += deltaPos.y;
		position.x = std::clamp((float)position.x, (float)LeftLimit, (float)RightLimit);
		position.y = std::clamp((float)position.y, (float)TopLimit, (float)BottomLimit);

		//printf("Post scarfy pos %f %f\r\n", position.x, position.y);
	}

	void MoveTo(Vector2 newPos)
	{
		GoToPosition.x= newPos.x - positionOffset.x;
		GoToPosition.y = newPos.y - positionOffset.y;
	}

	void HandleInput()
	{
		float accel = 1.0f;
		if (IsKeyDown(KEY_LEFT_SHIFT)) accel = 4;
		if (IsKeyDown(KEY_RIGHT_SHIFT)) accel = 4;
		
		if (IsKeyDown(KEY_UP))
		{
			MoveBy({ 0.0f,  -1.0f * accel / 2.0f});
			actionState = WALK;
		}

		if (IsKeyDown(KEY_DOWN))
		{
			MoveBy({ 0.0f, 1.0f * accel /2.0f});
			actionState = WALK;
		}

		if (IsKeyDown(KEY_LEFT))
		{
			MoveBy({ -1.0f * accel,0.0f });
			direction = LEFT;
			actionState = WALK;
		}

		if (IsKeyDown(KEY_RIGHT))
		{
			MoveBy({ 1.0f * accel,0.0f });
			direction = RIGHT;
			actionState = WALK;
		}
		if (WALK == actionState)
		{
			framesSpeed = 8;
			if (IsKeyDown(KEY_LEFT_SHIFT)) actionState = RUN;
			if (IsKeyDown(KEY_RIGHT_SHIFT)) actionState = RUN;
			framesSpeed = 12;
		}
		else framesSpeed = 4;

	}


};


std::vector<_Wabbit*> wabbitList;


std::vector<_Scene*> sceneList;


#define MAX_FRAME_SPEED     15
//#define MIN_FRAME_SPEED      1

int g_vert = 0;
int g_hori = 0;


void DrawSpeedBar(_Character *c)
{
	float top = 800 + 13;// +g_vert;
	float left = 175;// +g_hori;
	//speed bar
	DrawText("FRAME SPEED: ", left, top, 10, DARKGRAY);
	DrawText(TextFormat("%02i FPS", c->framesSpeed), left + 410, top, 10, DARKGRAY);
	DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", left + 125, top + 20, 10, DARKGRAY);

	for (int i = 0; i < MAX_FRAME_SPEED; i++)
	{
		if (i < c->framesSpeed) DrawRectangle(left + 85 + 21 * i, top -5, 20, 20, RED);
		DrawRectangleLines(left + 85 + 21 * i, top -5, 20, 20, MAROON);
	}
}



static Vector2 m_mousePos;
Font m_verdana;
int m_mouseCount = 0;
bool m_hover = true;
int m_CurScene = 0;

void DrawBorderControls()
{
	char buf[100];
	sprintf(buf, "MP xy: %0.0f  %0.0f  MCount: %d, g_Vert: %d  g_Hori: %d", m_mousePos.x, m_mousePos.y, m_mouseCount, g_vert, g_hori);
	DrawTextEx(m_verdana, buf, { 850.0f + g_hori, 825.0f + g_vert },  15.0f,0,  BLACK);
	//DrawTextPro(m_verdana, "Yippee!", { 200,25 }, { 100,100 }, 0, 25.0f, 0.0f, BLACK);
}


int main ()
{
	const char* CWD = GetWorkingDirectory();
	printf("CurWD: %s\r\n", CWD);

	_Explosion* e = NULL;
	_Wabbit* w = NULL;
	bool showMessageBox = false;

	

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI); // Tell the window to use vsync and work on high DPI displays
	InitWindow(SCENE_WIDTH, SCENE_HEIGHT + 100, "Kids Quest - V0.2 BMG");// Create the window and OpenGL context
	InitAudioDevice();

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	Texture wabbit = LoadTexture("wabbit_alpha.png");
	Texture2D scarfy = LoadTexture("scarfy.png");        


	_Scene* scene;

	//SetSceneLimits(float maxScale, float minScale, float topLimit, float leftLimit, float rightLimit, float bottomLimit, float leftLimit, float rightLimit)

	scene = new _Scene("scene/scene1_large.jpg");
	scene->SetSceneLimits(0.5, 0.08, 150, SCENE_WIDTH, 600, SCENE_HEIGHT-120);
	sceneList.push_back(scene);
	
	scene = new _Scene("scene/scene2.png"); //house
	scene->SetSceneLimits(0.5, .30, 145, 1040, 400, 595);
	sceneList.push_back(scene);
	
	scene = new _Scene("scene/scene3.jpg");
	scene->SetSceneLimits(1.0, 0.7,440, 1080, 400, 675);
	sceneList.push_back(scene);

	scene = new _Scene("scene/sceneKrustycrab.jpg");
	scene->SetSceneLimits(0.3, .20, 145, 1140, 400, 595);
	sceneList.push_back(scene);

	scene = new _Scene("scene/sceneLiveRoom.jpg");
	scene->SetSceneLimits(0.4, .30, 145, 1040, 400, 595);
	sceneList.push_back(scene);


	m_verdana = LoadFont("fonts/verdana.ttf");

	_Explosion::Initialize();
	_Wabbit::Initialize();


	_Character* scarfyChar = new _Character({ 500, 600 });

	Vector2 sp = sceneList[m_CurScene]->GetStartPos(POSITION_ONE);
	scarfyChar->EnterScene(sceneList[m_CurScene], sp);

	
	

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{


		if (IsKeyPressed(KEY_KP_ADD)) scarfyChar->framesSpeed++;
		else if (IsKeyPressed(KEY_KP_SUBTRACT)) scarfyChar->framesSpeed--;
		scarfyChar->framesSpeed = std::clamp(scarfyChar->framesSpeed, 1, 15);
		
		scarfyChar->HandleInput();


		if (IsKeyPressed(KEY_E)) --g_vert;
		if (IsKeyPressed(KEY_D)) ++g_vert;
		if (IsKeyPressed(KEY_S)) --g_hori;
		if (IsKeyPressed(KEY_F)) ++g_hori;

		if (IsKeyPressed(KEY_N))
		{
			m_CurScene++;
			if (m_CurScene >= sceneList.size()) m_CurScene = 0;

			Vector2 sp = sceneList[m_CurScene]->GetStartPos(POSITION_ONE);
			scarfyChar->EnterScene(sceneList[m_CurScene], sp);
		}

		if (IsKeyPressed(KEY_R))
		{
			scarfyChar->MoveTo({ 500,600 });
			puts("Reset Scarfy");
		}

		BeginDrawing();
		ClearBackground(IMGBLUE);

		m_mousePos = GetMousePosition();


		sceneList[m_CurScene]->Draw();
		//DrawTexture(wabbit, 400, 400, GREEN);


		DrawTextPro(m_verdana, "Yippee!", { 150,100 }, { 10,10 }, 0, 25.0f, 0.0f, BLACK);
		DrawSpeedBar(scarfyChar);
		DrawBorderControls();



		Rectangle BN{ 20, 810, 120, 30 };
		if (GuiButton(BN, "#191#Show Message")) showMessageBox = true;
		if (CheckCollisionPointRec(m_mousePos, BN)) m_hover = false;

		if (showMessageBox)
		{
			Rectangle MSGBOX{ 85, 70, 250, 100 };
			int result = GuiMessageBox(MSGBOX, "#191#Message Box", "Hi! This is a message!", "Nice;Cool");
			if (CheckCollisionPointRec(m_mousePos, MSGBOX)) m_hover = false;

			if (result >= 0) {
				showMessageBox = false;
				m_hover = true;
			}
		}



		if (true == m_hover)
		{
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				++m_mouseCount;
				scarfyChar->MoveTo(GetMousePosition());
			}

			if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				++m_mouseCount;
				w = new _Wabbit(GetMousePosition());
				wabbitList.push_back(w);

				//e = new _Explosion(GetMousePosition());
				//bombList.push_back(e);
			}

		}

		scarfyChar->Draw();

		for (auto x : bombList) x->Draw();
		for (auto W : wabbitList) W->Draw();



		
		EndDrawing(); 	// end the frame and get ready for the next one  (display frame, poll input, etc...)
	}



	// cleanup
	UnloadTexture(wabbit);
	delete e;
	delete scarfyChar;


	CloseWindow();
	return 0;
}
