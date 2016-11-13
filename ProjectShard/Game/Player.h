#ifndef PLAYER_H
#define PLAYER_H

#include "..\Renderer\Model.h"
#include "..\Renderer\Shader.h"
#include "..\Core\ResourceManager.h"
#include "..\Math\Matrix4x4.h"
#include "..\Math\Quaternion.h"
#include "..\Input\InputManager.h"
#include "Camera.h"
#include "..\Math\CollisionBox.h"

class Player
{
public:
	Player();
	~Player();

	void Update(float deltaTime);
	void Spawn();
	void Render(GLsizei screenWidth, GLsizei screenHeight);
	void Reflection(GLsizei screenWidth, GLsizei screenHeight);

	Camera camera;
	CollisionBox boundingBox;
	Vector3 linearVelocity;
	float speed;
	bool updateMovement;

private:
	Model *model;
	Shader *shaderModel;
	Vector3 acceleration;
	Quaternion orientation;
	float rotationSpeed;
	Matrix4 modelRotate;
	Vector3 position;

	void Movement(float deltaTime);
};

#endif