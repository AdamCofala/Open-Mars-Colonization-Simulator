#include "GameCamera.h"
#include <algorithm>


GameCamera::GameCamera(Vector2 startPosition, float startZoom,
    float moveSpeed, float zoomSpeed)
    :   moveSpeed(moveSpeed),
        zoomSpeed(zoomSpeed),
		zoomLerpSpeed(10.0f),
		targetZoom(startZoom)
{
    init(startPosition, startZoom);
}


void GameCamera::init(Vector2 startPosition, float startZoom) {
    camera.target = startPosition;

    camera.offset = {
        (float)GetScreenWidth() / 2,
        (float)GetScreenHeight() / 2
    };

    camera.rotation = 0.0f;
    camera.zoom = startZoom;

    targetZoom = startZoom;
}

void GameCamera::update(float dt) {
    update_movement(dt);
	update_zoom(dt);
}


void GameCamera::update_movement(float dt) {
    if (IsKeyDown(KEY_A)) camera.target.x -= moveSpeed * dt;
    if (IsKeyDown(KEY_D)) camera.target.x += moveSpeed * dt;
    if (IsKeyDown(KEY_W)) camera.target.y -= moveSpeed * dt;
    if (IsKeyDown(KEY_S)) camera.target.y += moveSpeed * dt;

    Vector2 mouseDelta = GetMouseDelta();

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
    }

    // Openttd RMB Logic
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		HideCursor();
        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
	}
	else { 
        ShowCursor();
	}
}

void GameCamera::update_zoom(float dt) {
    float wheel = GetMouseWheelMove();

    if (wheel != 0) {
        targetZoom += wheel * zoomSpeed;
		targetZoom = std::max(minZoom, std::min(maxZoom, targetZoom));
    }
    Vector2 mouseWorldBefore = GetScreenToWorld2D(GetMousePosition(), camera);
    camera.zoom += (targetZoom - camera.zoom) * zoomLerpSpeed * dt;

    Vector2 mouseWorldAfter = GetScreenToWorld2D(GetMousePosition(), camera);
    camera.target.x += mouseWorldBefore.x - mouseWorldAfter.x;
    camera.target.y += mouseWorldBefore.y - mouseWorldAfter.y;
}


Camera2D GameCamera::getCamera() const {
    return camera;
}