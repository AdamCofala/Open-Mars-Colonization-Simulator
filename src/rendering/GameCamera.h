#pragma once
#include "raylib.h"

class GameCamera {
private:
    float moveSpeed;
    float zoomSpeed;
    float zoomLerpSpeed;

    float targetZoom;
	float minZoom = 0.1f;
	float maxZoom = 3.0f;

    Camera2D camera;

public:
    GameCamera() = default;
    GameCamera(Vector2 startPosition, float startZoom = 1.0f, float moveSpeed = 500.0f, float zoomSpeed = 0.07f);

    void init(Vector2 startPosition, float startZoom);
    void update(float dt);
    void update_movement(float dt);
    void update_zoom(float dt);
    Camera2D getCamera() const;
};