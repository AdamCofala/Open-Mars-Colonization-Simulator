#include "raylib.h"

class GameCamera {
private:
		Vector2 position;
		float moveSpeed = 500.0f;
		float zoomSpeed = 1.0f;
		float zoomLerpSpeed = 15.0f;
		float targetZoom = 1.0f;
		Camera2D camera;

public:

	GameCamera() = default;
	GameCamera(Vector2 startPosition, float moveSpeed=500.0f, float zoomSpeed=0.1f)
		: position(startPosition), moveSpeed(moveSpeed), zoomSpeed(zoomSpeed)
	{
		init();
	}

	void init() {
		camera.target = position;
		camera.offset = {
			(float)GetScreenWidth() / 2,
			(float)GetScreenHeight() / 2
		};
		camera.rotation = 0.0f;
		camera.zoom = 1.0f;
	}

	void update(float dt) {

		// TODO - this is bad, make it better.

		// MOVE CAMERA (WASD)
		if (IsKeyDown(KEY_A)) camera.target.x -= moveSpeed * dt;
		if (IsKeyDown(KEY_D)) camera.target.x += moveSpeed * dt;
		if (IsKeyDown(KEY_W)) camera.target.y -= moveSpeed * dt;
		if (IsKeyDown(KEY_S)) camera.target.y += moveSpeed * dt;

		// ZOOM
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			targetZoom += wheel * zoomSpeed;

			if (targetZoom < 0.2f) targetZoom = 0.2f;
			if (targetZoom > 3.0f) targetZoom = 3.0f;
		}

		// SMOOTH INTERPOLATION
		camera.zoom += (targetZoom - camera.zoom) * zoomLerpSpeed * dt;
				
		// DRAG (MMB)
		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			Vector2 delta = GetMouseDelta();
			camera.target.x -= delta.x / camera.zoom;
			camera.target.y -= delta.y / camera.zoom;
		}
	}


	Camera2D getCamera() {
		return this->camera;
	}

};