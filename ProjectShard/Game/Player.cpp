#include "Player.h"

Player::Player()
	: rotationSpeed(2.0f), camera(Vector3(3.0f, 2.0f, 8.0f)), position(Vector3(0.0f, 0.0f, 0.0f)), speed(2.5f), recordRace(false)
{
	lastTime = glfwGetTime();

	model = g_resourceMgr.GetModel(SID("PlayerShip"));
	shaderModel = g_resourceMgr.GetShader(SID("PlayerShader"));

	// TODO(Darren): Create some user of yaw, roll pitch for camera angles.
	//				 maybe set pitch etc.
	//camera.Roll(140.0f);

	shaderModel->Use();
	glUniform1f(glGetUniformLocation(shaderModel->Program, "time"), 11.0f);
}

Player::~Player()
{
	delete model;
	delete shaderModel;
}

void Player::Update(float deltaTime)
{
	if (shipDestroyed)
		PlayExplodeAnimation();

	if(updateMovement)
		Movement(deltaTime);

	Vector3 initPos = Vector3(3.0f, 2.0f, 8.0f);
	Vector3 finalPos = position - Vector3(0.0f, -15.0f, -40.0f);
	if (cameraInterpolator < 1.0f)
		cameraInterpolator += 0.8f * deltaTime;
	else
		cameraInterpolator = 1.0f;
	Vector3 transitionVector = transitionVector.Lerp(initPos, finalPos, cameraInterpolator);

	if (interpolator < 1.0f)
		interpolator += 0.1f * deltaTime;
	else
		interpolator = 1.0f;

	float offset = (1.0f - interpolator) * oldPosition.x + (interpolator * position.x);

	Vector3 direction = position - camera.GetPosition();

	camera.SetPosition(transitionVector);

	modelRotate = Matrix4();
	modelRotate = modelRotate.QuaternionToMatrix4(orientation);
	boundingBox.UpdateBoundingBox(position, modelRotate, Vector3(1.0f, 1.0f, 1.0f));

	RecordPosition();

	currentTrackTime += deltaTime;

	oldPosition = position;
}

/*
	TODO(Darren): Implement finished animation.
	The camera is already being set in the update so the interpolation does not matter.
*/
void Player::FinishedAnimation(float deltaTime)
{
	//Vector3 initPos = Vector3(3.0f, 2.0f, 8.0f);
	//Vector3 finalPos = position - Vector3(-10.0f, 0.0f, 0.0f);
	//if (iner < 1.0f)
	//	iner += 0.01f * deltaTime;
	//else
	//	iner = 1.0f;
	//Vector3 transitionVector = transitionVector.Lerp(position, 
	//	position + Vector3(0.0f, 0.0f, -2000.0f), iner);

	//camera.SetPosition(transitionVector);
}

void Player::PlayExplodeAnimation()
{
	// Add time component to geometry shader in the form of a uniform
	shaderModel->Use();
	glUniform1f(glGetUniformLocation(shaderModel->Program, "time"), glfwGetTime() * 5.0f);
}

void Player::Movement(float deltaTime)
{
	if (linearVelocity.Magnitude() < 0.5f)
		linearVelocity = Vector3();

	position += linearVelocity * deltaTime;

	Quaternion targetRotation = Quaternion();
	Quaternion initalRotation = Quaternion();

	if (InputManager::GetInstance().IsKeyDown(GLFW_KEY_UP)
		|| InputManager::GetInstance().IsControllerButtonDown(XBOX360_RB)
		|| InputManager::GetInstance().GetRightTrigger() > 0.4f)
		linearVelocity.z -= speed;

	if (InputManager::GetInstance().IsKeyDown(GLFW_KEY_DOWN)
		|| InputManager::GetInstance().IsControllerButtonDown(XBOX360_LB)
		|| InputManager::GetInstance().GetLeftTrigger() < -0.4f)
		linearVelocity.z += speed * 2.0f;

	if (InputManager::GetInstance().IsKeyDown(GLFW_KEY_LEFT) 
		|| InputManager::GetInstance().GetLeftJoyStick().x < - JOYSTICK_DEAD_ZONE)
	{
		linearVelocity.x -= speed * 1.1f; // *(-input * 2.0f);

		targetRotation = targetRotation.RotateZ(MathHelper::DegressToRadians(90.0f));
		orientation = orientation.Slerp(orientation, targetRotation, deltaTime * rotationSpeed);
	}
	else
		orientation = orientation.Slerp(orientation, initalRotation, deltaTime * rotationSpeed);

	if (InputManager::GetInstance().IsKeyDown(GLFW_KEY_RIGHT) 
		|| InputManager::GetInstance().GetLeftJoyStick().x > JOYSTICK_DEAD_ZONE)
	{
		linearVelocity.x += speed * 1.1f; // *(-input * 2.0f);

		targetRotation = targetRotation.RotateZ(MathHelper::DegressToRadians(-90.0f));
		orientation = orientation.Slerp(orientation, targetRotation, deltaTime * rotationSpeed);
	}
	else
		orientation = orientation.Slerp(orientation, initalRotation, deltaTime * rotationSpeed);

	if (linearVelocity.x != 0)
	{
		Vector3 i = linearVelocity;
		float friction = 0.003f;

		linearVelocity -= i * friction;
	}

	Vector3 direction = linearVelocity;
	direction.normalise();
	g_debugDrawMgr.AddLine(position, Vector3(0.0f, 10.0f, 0.0), Vector3(1.0f, 0.0f, 0.0f), 1.0f, false);
}

void Player::RecordPosition()
{
	if (recordRace)
	{
		float currentTime = glfwGetTime();
		if (currentTime - lastTime >= 1.0)
		{
			lastTime += 1.0;

			Vector3 pos = position;
			recordPositions.push_back(pos);

			//Quaternion orien = orientation;
			//recordOrientation.push_back(orien);
		}
	}
}

void Player::WriteRecordedPositions()
{
	std::ofstream ghostRacerFile;
	ghostRacerFile.open("Ghost_Racer.txt");
	for (std::vector<Vector3>::iterator it = recordPositions.begin(); it != recordPositions.end(); it++)
	{
		ghostRacerFile << it->x << " " << it->y << " " << it->z << "\n";
	}

	ghostRacerFile << "#" << "\n";

	for (std::vector<Quaternion>::iterator it = recordOrientation.begin(); it != recordOrientation.end(); it++)
	{
		ghostRacerFile << it->w << " " << it->x << " " << it->y << " " << it->z << "\n";
	}
	ghostRacerFile.close();
}

void Player::Spawn()
{
	position = Vector3(0.0f, 0.0f, 0.0f);
	linearVelocity = Vector3();
	cameraInterpolator = 0.0f;
	shaderModel->Use();
	glUniform1f(glGetUniformLocation(shaderModel->Program, "time"), 11.0f);
	shipDestroyed = false;
}

void Player::Render(GLsizei screenWidth, GLsizei screenHeight)
{
	shaderModel->Use();
	Matrix4 modelMatrix = Matrix4();
	Matrix4 viewMatrix = camera.GetViewMatrix();
	Matrix4 projection = camera.GetProjectionMatrix(screenWidth, screenHeight);
	Matrix4 modelRotate = Matrix4();
	modelRotate = modelRotate.QuaternionToMatrix4(orientation);
	Matrix4 modelTranslate = Matrix4();
	modelTranslate = modelTranslate.translate(position);
	modelMatrix = modelRotate * modelTranslate;
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, &modelMatrix.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "view"), 1, GL_FALSE, &viewMatrix.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	model->Draw(*shaderModel);
}

void Player::Reflection(GLsizei screenWidth, GLsizei screenHeight)
{
	shaderModel->Use();
	Matrix4 modelMatrix = Matrix4();
	Matrix4 viewMatrix = camera.GetViewMatrix();
	Matrix4 projection = camera.GetProjectionMatrix(screenWidth, screenHeight);
	Matrix4 modelRotate = Matrix4();
	Quaternion rotateRef = Quaternion();
	rotateRef = rotateRef.RotateZ(MathHelper::DegressToRadians(-180.0f));
	modelRotate = modelRotate.QuaternionToMatrix4(rotateRef * 
		Quaternion(orientation.w, -orientation.x, -orientation.y, -orientation.z));
	Matrix4 modelTranslate = Matrix4();
	modelTranslate = modelTranslate.translate(position + Vector3(0.0f, -3.0f, 0.0));
	modelMatrix = modelRotate * modelTranslate;
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, &modelMatrix.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "view"), 1, GL_FALSE, &viewMatrix.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	model->Draw(*shaderModel);
}

float Player::GetSpeed()
{
	return linearVelocity.Magnitude();
}