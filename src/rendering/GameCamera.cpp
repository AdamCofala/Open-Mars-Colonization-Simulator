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

    // Pan camera (Middle mouse button or OpenTTD style Right mouse button)
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
    }
}

void GameCamera::update_zoom(float dt) {
    float wheel = GetMouseWheelMove();

    if (wheel != 0) {
        targetZoom += wheel * zoomSpeed;
        targetZoom = std::max(minZoom, std::min(maxZoom, targetZoom));
        zoomToMouse = true;
    }

    if (std::abs(targetZoom - camera.zoom) < 0.001f) {
        zoomToMouse = false;
        camera.zoom = targetZoom;
    }

    if (zoomToMouse) {
        Vector2 mouseWorldBefore = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.zoom += (targetZoom - camera.zoom) * zoomLerpSpeed * dt;

        Vector2 mouseWorldAfter = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.target.x += mouseWorldBefore.x - mouseWorldAfter.x;
        camera.target.y += mouseWorldBefore.y - mouseWorldAfter.y;
    }
    else {
        camera.zoom += (targetZoom - camera.zoom) * zoomLerpSpeed * dt;
    }
}

Camera2D GameCamera::getCamera() const {
	return camera;
}

void GameCamera::setTarget(Vector2 target) {
	camera.target = target;
}

void GameCamera::setZoom(float zoom, bool focusOnMouse) {
	targetZoom = std::max(minZoom, std::min(maxZoom, zoom));
	zoomToMouse = focusOnMouse;
}