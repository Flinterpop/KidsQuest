/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir



int main ()
{
	const char *CWD = GetWorkingDirectory();
	printf("CurWD: %s\r\n", CWD);

	bool showMessageBox = false;

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Hello Raylib");

	InitAudioDevice();


	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	Sound fxBoom = LoadSound("audio/boom.wav");
	Texture2D explosion = LoadTexture("explosion.png");


	struct Explosion
	{
		#define NUM_FRAMES_PER_LINE     5
		#define NUM_LINES               5

		// Init variables for animation
		float frameWidth;// = (float)(explosion.width / NUM_FRAMES_PER_LINE);   // Sprite one frame rectangle width
		float frameHeight;// = (float)(explosion.height / NUM_LINES);           // Sprite one frame rectangle height
		int currentFrame = 0;
		int currentLine = 0;

		Rectangle frameRec = { 0, 0, frameWidth, frameHeight };
		Vector2 position = { 0.0f, 0.0f };

		bool active = false;
		int framesCounter = 0;

		Explosion()
		{
			float frameWidth = (float)(explosion.width / NUM_FRAMES_PER_LINE);   // Sprite one frame rectangle width
			float frameHeight = (float)(explosion.height / NUM_LINES);           // Sprite one frame rectangle height
		}

	};


	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		DrawText("Hello Raylib", 200,200,20,WHITE);

		DrawTexture(wabbit, 400, 200, WHITE);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !active)
		{
			position = GetMousePosition();
			active = true;

			position.x -= frameWidth / 2.0f;
			position.y -= frameHeight / 2.0f;

			PlaySound(fxBoom);
		}

		// Compute explosion animation frames
		if (active)
		{
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
		}

		frameRec.x = frameWidth * currentFrame;
		frameRec.y = frameHeight * currentLine;

		// Draw explosion required frame rectangle
		if (active) DrawTextureRec(explosion, frameRec, position, WHITE);


		if (GuiButton((Rectangle) { 24, 24, 120, 30 }, "#191#Show Message"))
		{
			PlaySound(fxBoom);
			showMessageBox = true;
		}


		if (showMessageBox)
		{
			int result = GuiMessageBox((Rectangle) { 85, 70, 250, 100 },
				"#191#Message Box", "Hi! This is a message!", "Nice;Cool");

			if (result >= 0) showMessageBox = false;
		}





		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
