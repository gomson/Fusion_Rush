#include "MainMenu.h"

MainMenu::MainMenu()
	: defaultColor(1.0f, 1.0f, 1.0f), selectColor(0.4f, 0.4f, 1.0f)
{
	playLabel.position = Vector2(40.0f, 400.0f);
	playLabel.color = selectColor;
	playLabel.text = "Play Game";

	signInOutLabel.position = Vector2(40.0f, 350.0f);
	signInOutLabel.color = defaultColor;
	signInOutLabel.text = "Sign In";

	exitLabel.position = Vector2(40.0f, 300.0f);
	exitLabel.color = defaultColor;
	exitLabel.text = "Exit Game";

	exitAsk.color = defaultColor;
	exitAsk.text = "Exit?";
	exitNo.color = selectColor;
	exitNo.text = "No";
	exitNo.scale = 0.8f;
	exitYes.color = defaultColor;
	exitYes.text = "Yes";
	exitYes.scale = 0.8f;

	worldRankLabel.position = Vector2(20.0f, 60.0f);
	worldRankLabel.text = "World Rank: NA";
	worldRankLabel.color = defaultColor;
	worldRankLabel.scale = 0.5f;
	
	bestRecordTime.position = Vector2();
	bestRecordTime.text = "Best Time: NA";
	bestRecordTime.color = defaultColor;
	bestRecordTime.scale = 0.5f;

	currentPlayer.position = Vector2(80.0f, 15.0f);
	currentPlayer.text = "Please sign in";
	currentPlayer.color = defaultColor;
	currentPlayer.scale = 0.6f;

	leaderboardTitle.position = Vector2(500.0f, 500.0f);
	leaderboardTitle.text = "World Top 10";
	leaderboardTitle.color = Vector3(1.0f, 0.2f, 0.3f);
	leaderboardTitle.scale = 0.7f;

	serverStatus.position = Vector2(700, 10);
	serverStatus.color = Vector3(1.0f, 0.0f, 0.0f);
	serverStatus.scale = 0.7f;
	serverStatus.text = "Offline";

	usernameText.color = defaultColor;
	usernameText.scale = 0.8f;
	usernameText.text = "Username";
	passwordText.color = defaultColor;
	passwordText.scale = 0.8f;
	passwordText.text = "Password";
	loginLabel.color = defaultColor;
	loginLabel.text = "Login";
	loginLabel.scale = 0.6f;
	cancelLabel.color = defaultColor;
	cancelLabel.text = "Cancel";
	cancelLabel.scale = 0.6f;
	createAccountLabel.color = defaultColor;
	createAccountLabel.text = "Create account?";
	createAccountLabel.scale  = 0.6f;

	selectPosition = playLabel.position;

	UI_Shader = g_resourceMgr.GetShader(SID("UI_Shader"));
	UI_Bottom = g_resourceMgr.GetTexture(SID("Bottom_UI"));
	UI_Pannal = g_resourceMgr.GetTexture(SID("Menu_UI"));
	UI_Enter = g_resourceMgr.GetTexture(SID("Enter_UI"));
	typeElement = g_resourceMgr.GetTexture(SID("TypeElement"));
	gameTitle = g_resourceMgr.GetTexture(SID("Game_Logo"));

	spriteRenderer = new SpriteRenderer();
}

MainMenu::~MainMenu()
{
	delete UI_Shader;
	delete UI_Bottom;
	delete UI_Pannal;
	delete spriteRenderer;
}

void MainMenu::InitScene()
{
	textRenderer.Load("Resources/Fonts/arial.ttf");

	playLabel.rect.SetRectangle(playLabel.position, 30, 10);
	signInOutLabel.rect.SetRectangle(signInOutLabel.position, 30, 10);
	exitLabel.rect.SetRectangle(exitLabel.position, 30, 10);
}

void MainMenu::UpdateScene(float delatTime, GLsizei screenWidth, GLsizei screenHeight)
{
	if (startSoundTrack)
	{
		menuSoundTrack.Play2D("Resources/Music/Menu_Music.wav", true);
		if (menuSoundTrack.soundEngine != NULL)
			menuSoundTrack.soundEngine->setSoundVolume(1.0f);
		startSoundTrack = false;
	}

	if (GameSparksInfo::GetStatus())
	{
		serverStatus.text = "Online";
		serverStatus.color = Vector3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		serverStatus.text = "Offline";
		serverStatus.color = Vector3(1.0f, 0.0f, 0.0f);
	}

	selectRect.SetRectangle(selectPosition, 200, 50);

	switch (currentMenuState)
	{
		case MenuState::MenuOpitions:
		{
			passwordMatchError = accountSignInError = userNameExistsError = notOnlineError = false;

			if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_UP) 
				|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_UP))
				&& selectPosition.y != playLabel.position.y)
			{
				selectPosition.y += 50.0f;
				PlayMenuNav();
			}
			else if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN) 
				|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_DOWN))
				&& selectPosition.y != exitLabel.position.y)
			{
				selectPosition.y -= 50.0f;
				PlayMenuNav();
			}

			UpdateLable(playLabel);
			if (playLabel.labelSelected)
			{
				currentSelectState = SelectState::PlaySelected;
				playGame = true;
				playLabel.labelSelected = false;

				if(menuSoundTrack.soundEngine != NULL)
					menuSoundTrack.soundEngine->stopAllSounds();
			}
			else
				playGame = false;
			UpdateLable(signInOutLabel);
			if (signInOutLabel.labelSelected)
			{
				currentSelectState = SelectState::SignInOutSeleted;
				currentMenuState = MenuState::SignInOpitions;
				selectPosition = signInOutPannelPos + Vector2(0.0f, 50.0f);
				// Clear the data fields
				signInUserName.clear();
				signInPassword.clear();
				signInPassDisplay.clear();
				InputManager::GetInstance().keyInput.clear();
			}
			UpdateLable(exitLabel);
			if (exitLabel.labelSelected)
			{
				currentSelectState = SelectState::ExitSelected;
				currentMenuState = MenuState::ExitOpitions;
				selectPosition = exitPannelPosition;
			}

			break;
		}

		case MenuState::SignInOpitions:
		{
			usernameRect.SetRectangle(signInOutPannelPos + Vector2(30.0f, 60.0f), 200.0f, 30.0f);
			passwordRect.SetRectangle(signInOutPannelPos + Vector2(30.0f, 160.0f), 200.0f, 30.0f);
			createAccountPannelPos = signInOutPannelPos + Vector2(0.0f, -40.0f);

			loginLabel.rect.SetRectangle(Vector2(loginLabel.position.x, screenHeight - loginLabel.position.y), 30, 50);
			cancelLabel.rect.SetRectangle(Vector2(cancelLabel.position.x, screenHeight - cancelLabel.position.y), 30, 50);
			createAccountLabel.rect.SetRectangle(Vector2(createAccountLabel.position.x - 50.0f, screenHeight - (createAccountLabel.position.y - 150.0f)), 200.0f, 50);

			if (selectRect.Intersects(usernameRect))
			{
				std::string input = InputManager::GetInstance().keyInput;
				if (input.size() < 15)
				{
					signInUserName = input;
					PlayKeyboardEnter();
				}

				typeElementPos = signInOutPannelPos + Vector2(40.0f + (21.0f * signInUserName.size()), 80.0f);

				if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB)
					|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_A))
				{
					InputManager::GetInstance().keyInput.clear();
					selectPosition.y += 120.0f;
					PlayMenuNav();
				}
			}
			else if (selectRect.Intersects(passwordRect))
			{
				std::string input = InputManager::GetInstance().keyInput;
				if (input.size() < 15)
				{
					signInPassword = input;
					PlayKeyboardEnter();
				}

				typeElementPos = signInOutPannelPos + Vector2(40.0f + (15.0f * signInPassword.size()), 180.0f);

				if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB)
					|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_A))
				{
					InputManager::GetInstance().keyInput.clear();
					selectPosition.y += 120.0f;
					PlayMenuNav();
				}
			}

			if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_BACKSPACE))
			{
				if (InputManager::GetInstance().keyInput.size() > 0)
					InputManager::GetInstance().keyInput.pop_back();

				PlayBackspaceSound();
			}

			UpdateLable(loginLabel);
			UpdateLable(cancelLabel);
			UpdateLable(createAccountLabel);

			if (loginLabel.labelSelected)
			{
				SendSignInRequest();
				GameSparksInfo::InitGS();
			}

			if (GameSparksInfo::signInAccount)
			{
				if (!GameSparksInfo::available)
					notOnlineError = true;
				else if (!GameSparksInfo::loginSuccessful)
					accountSignInError = true;
			}

			if (cancelLabel.labelSelected || InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE)
				|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_BACK))
			{
				currentMenuState = MenuState::MenuOpitions;
				currentSelectState = SelectState::NotSelected;
				selectPosition = signInOutLabel.position;
				GameSparksInfo::signInAccount = false;
			}

			if (createAccountLabel.labelSelected)
			{
				currentMenuState = MenuState::CreateAccount;
				selectPosition = createAccountPannelPos + Vector2(30.0f, 60.0f);
				signInUserName.clear();
				signInPassword.clear();
				signInReEnterPass.clear();
			}

			if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_UP) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_UP)) 
				&& !selectRect.Intersects(usernameRect))
			{
				selectPosition.y -= 120.0f;
				PlayMenuNav();
			}
			else if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_DOWN)) 
				&& !selectRect.Intersects(createAccountLabel.rect))
			{
				selectPosition.y += 120.0f;
				PlayMenuNav();
			}

			if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_RIGHT)) 
				&& selectRect.Intersects(loginLabel.rect))
			{
				selectPosition.x += 150.0f;
				PlayMenuNav();
			}
			else if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_LEFT)) 
				&& selectRect.Intersects(cancelLabel.rect))
			{
				selectPosition.x -= 150.0f;
				PlayMenuNav();
			}

			break;
		}

		case MenuState::CreateAccount:
		{
			usernameRect.SetRectangle(signInOutPannelPos + Vector2(30.0f, 60.0f), 200.0f, 30.0f);
			passwordRect.SetRectangle(signInOutPannelPos + Vector2(30.0f, 160.0f), 200.0f, 30.0f);
			reenterPasswordRec.SetRectangle(signInOutPannelPos + Vector2(30.0f, 260.0f), 200.0f, 30.0f);

			cancelLabel.rect.SetRectangle(Vector2(cancelLabel.position.x, screenHeight - cancelLabel.position.y), 30, 50);
			createAccountLabel.rect.SetRectangle(Vector2(createAccountLabel.position.x, screenHeight - createAccountLabel.position.y), 100.0f, 50);

			if (selectRect.Intersects(usernameRect))
			{
				std::string input = InputManager::GetInstance().keyInput;
				if (input.size() < 15)
				{
					PlayKeyboardEnter();
					signInUserName = input;
				}

				typeElementPos = signInOutPannelPos + Vector2(40.0f + (21.0f * signInUserName.size()), 50.0f);

				if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB)
					|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_A))
				{
					InputManager::GetInstance().keyInput.clear();
					selectPosition.y += 100.0f;
					PlayMenuNav();
				}
			}
			else if (selectRect.Intersects(passwordRect))
			{
				std::string input = InputManager::GetInstance().keyInput;
				if (input.size() < 15)
				{
					PlayKeyboardEnter();
					signInPassword = input;
				}

				typeElementPos = signInOutPannelPos + Vector2(40.0f + (15.0f * signInPassword.size()), 150.0f);

				if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB)
					|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_A))
				{
					InputManager::GetInstance().keyInput.clear();
					selectPosition.y += 100.0f;
					PlayMenuNav();
				}
			}
			else if (selectRect.Intersects(reenterPasswordRec))
			{
				std::string input = InputManager::GetInstance().keyInput;
				if (input.size() < 15)
				{
					PlayKeyboardEnter();
					signInReEnterPass = input;
				}

				typeElementPos = signInOutPannelPos + Vector2(40.0f + (15.0f * signInReEnterPass.size()), 250.0f);

				if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN)
					|| InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB)
					|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_A))
				{
					InputManager::GetInstance().keyInput.clear();
					selectPosition.y += 100.0f;
					PlayMenuNav();
				}
			}

			if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_BACKSPACE))
			{
				if (InputManager::GetInstance().keyInput.size() > 0)
					InputManager::GetInstance().keyInput.pop_back();

				PlayBackspaceSound();
			}

			UpdateLable(cancelLabel);
			UpdateLable(createAccountLabel);

			if (GameSparksInfo::registerAccount)
			{
				if (!GameSparksInfo::available)
					notOnlineError = true;
				else if (!GameSparksInfo::loginSuccessful)
					accountSignInError = true;
			}

			if (cancelLabel.labelSelected || InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE)
				|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_BACK))
			{
				currentMenuState = MenuState::MenuOpitions;
				currentSelectState = SelectState::NotSelected;
				selectPosition = signInOutLabel.position;
			}

			if (createAccountLabel.labelSelected)
			{
				if (signInPassword == signInReEnterPass)
				{
					AccountCreateRequest();
					GameSparksInfo::InitGS();
					passwordMatchError = false;
				}
				else
				{
					std::cout << "Password word does not match" << std::endl;
					PlayMenuError();
					passwordMatchError = true;
				}
			}

			if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_UP) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_UP)) 
				&& !usernameRect.Intersects(selectRect))
			{
				selectPosition.y -= 100.0f;
				PlayMenuNav();
			}
			else if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_DOWN))
				&& (!createAccountLabel.rect.Intersects(selectRect)) && (!cancelLabel.rect.Intersects(selectRect)))
			{
				selectPosition.y += 100.0f;
				PlayMenuNav();
			}

			if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_RIGHT))
				&& !cancelLabel.rect.Intersects(selectRect))
			{
				selectPosition.x += 150.0f;
				PlayMenuNav();
			}
			else if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_LEFT))
				&& !createAccountLabel.rect.Intersects(selectRect))
			{
				selectPosition.x -= 150.0f;
				PlayMenuNav();
			}

			break;
		}

		case MenuState::ExitOpitions:
		{
			if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_RIGHT)) 
				&& selectPosition.x != exitPannelPosition.x + 150.0f)
			{
				selectPosition.x += 150.0f;
				PlayMenuNav();
			} 
			else if ((InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT) || InputManager::GetInstance().IsControllerButtonPressed(XBOX360_LEFT)) 
				&& selectPosition.x != exitPannelPosition.x)
			{
				selectPosition.x -= 150.0f;
				PlayMenuNav();
			}

			exitNo.rect.SetRectangle(exitNo.position, 30, 50);
			exitYes.rect.SetRectangle(exitYes.position, 30, 50);

			UpdateLable(exitNo);
			if (exitNo.labelSelected || InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE)
				|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_BACK))
			{
				selectPosition = exitLabel.position;
				currentMenuState = MenuState::MenuOpitions;
				currentSelectState = SelectState::NotSelected;
			}
			UpdateLable(exitYes);
			if (exitYes.labelSelected)
			{
				exitGame = true;
			}

			break;
		}

		default:
			break;
	}

	if (GameSparksInfo::loginSuccessful)
	{
		currentUserName = GameSparksInfo::username;
		GameSparksInfo::loginSuccessful = false;
		currentMenuState = MenuState::MenuOpitions;
		currentSelectState = SelectState::NotSelected;
		selectPosition = signInOutLabel.position;
		accountSignInError = false;
		// Get the current signed in player, world rank and time.
		currentPlayer.text = currentUserName.c_str();
	}

	if (currentUserName != "")
	{
		std::stringstream ss;
		ss << "World Rank :  " << GameSparksInfo::currentPlayer.rank.GetValue();
		playerRank = ss.str();
		worldRankLabel.text = playerRank.c_str();

		ss.str("");
		ss << "Best Time : " << GameSparksInfo::currentPlayer.time.GetValue();
		recordTime = ss.str();
		bestRecordTime.text = recordTime.c_str();
	}

	bestRecordTime.position = Vector2(260.0f, 10.0f);
	worldRankLabel.position = Vector2(50.0f, 10.0f);
	currentPlayer.position = Vector2(50.0f, 60.0f);
}

void MainMenu::SendSignInRequest()
{
	GameSparksInfo::signInAccount = true;
	GameSparksInfo::username = signInUserName;
	GameSparksInfo::password = signInPassword;
}

void MainMenu::AccountCreateRequest()
{
	GameSparksInfo::registerAccount = true;
	GameSparksInfo::username = signInUserName;
	GameSparksInfo::password = signInPassword;
}

void MainMenu::UpdateLable(MenuLabel &label)
{
	if (selectRect.Intersects(label.rect))
	{
		label.color = selectColor;

		if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER) 
			|| InputManager::GetInstance().IsControllerButtonPressed(XBOX360_A))
		{
			label.labelSelected = true;
			PlayMenuSelect();
		}
		else
			label.labelSelected = false;
	}
	else
	{
		label.color = defaultColor;
		label.labelSelected = false;
	}
}

void MainMenu::RenderScene(GLsizei screenWidth, GLsizei screenHeight)
{
	float pannelHeight = screenHeight - playLabel.position.y;
	leaderboardUIPos = Vector2(screenWidth - 500.0f, 30.0f);
	exitPannelPosition = Vector2((screenWidth / 2) - 200.0f, (screenHeight / 2) - 70.0f);
	exitAsk.position = exitPannelPosition + Vector2(150.0f, 90.0f);
	exitNo.position = exitPannelPosition + Vector2(100.0f, 20.0f);
	exitYes.position = exitPannelPosition + Vector2(250.0f, 20.0f);
	signInOutPannelPos = Vector2((screenWidth / 2) - 200.0f, (screenHeight / 2) - 70.0f);
	usernameText.position = signInOutPannelPos + Vector2(20.0f, 90.0f);
	passwordText.position = signInOutPannelPos + Vector2(20.0f, -10.0f);
	loginLabel.position = signInOutPannelPos + Vector2(50.0f, -110.0f);
	cancelLabel.position = signInOutPannelPos + Vector2(250.0f, -110.0f);
	createAccountLabel.position = signInOutPannelPos + Vector2(100.0f, -160.0f);
	leaderboardTitle.position = Vector2(screenWidth - 370.0f, screenHeight - 80.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Matrix4 projection = Matrix4();
	projection = projection.orthographicProjection(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
	UI_Shader->Use();
	glUniformMatrix4fv(glGetUniformLocation(UI_Shader->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	spriteRenderer->Render(*gameTitle, *UI_Shader, Vector2(250.0f, 50.0f), Vector2(550.0f, 250.0f));
	spriteRenderer->Render(*UI_Bottom, *UI_Shader, Vector2(0.0f, screenHeight - 100.0f), Vector2(screenWidth + 40.0f, 100.0f));
	spriteRenderer->Render(*UI_Pannal, *UI_Shader, Vector2(-5.0f, pannelHeight - 80.0f), Vector2(300.0f, 220.0f));

	RenderLabel(playLabel, screenWidth, screenHeight);
	RenderLabel(signInOutLabel, screenWidth, screenHeight);
	RenderLabel(exitLabel, screenWidth, screenHeight);
	RenderLabel(worldRankLabel, screenWidth, screenHeight);
	RenderLabel(bestRecordTime, screenWidth, screenHeight);
	RenderLabel(currentPlayer, screenWidth, screenHeight);
	if (GameSparksInfo::available)
	{
		spriteRenderer->Render(*UI_Pannal, *UI_Shader, leaderboardUIPos, Vector2(470.0f, 620.0f));
		RenderLabel(leaderboardTitle, screenWidth, screenHeight);
		RenderLeaderboardEntry(screenWidth, screenHeight);
	} 
	serverStatus.position.x = screenWidth - 140;
	RenderLabel(serverStatus, screenWidth, screenHeight);

	if (currentSelectState == SelectState::ExitSelected)
	{
		spriteRenderer->Render(*UI_Pannal, *UI_Shader, exitPannelPosition, Vector2(400.0f, 150.0f));

		RenderLabel(exitAsk, screenWidth, screenHeight);
		RenderLabel(exitNo, screenWidth, screenHeight);
		RenderLabel(exitYes, screenWidth, screenHeight);
	}
	else if (currentSelectState == SelectState::SignInOutSeleted)
	{
		spriteRenderer->Render(*typeElement, *UI_Shader, typeElementPos, Vector2(5.0f, 20.0f), 0.0f, Vector3(0.0f, 0.0f, 0.0f));

		if (currentMenuState == MenuState::SignInOpitions)
		{
			// Username
			spriteRenderer->Render(*UI_Enter, *UI_Shader, signInOutPannelPos + Vector2(30.0f, 60.0f), Vector2(350.0f, 50.0f));
			// Password
			spriteRenderer->Render(*UI_Enter, *UI_Shader, signInOutPannelPos + Vector2(30.0f, 160.0f), Vector2(350.0f, 50.0f));
			// UI background pannel
			spriteRenderer->Render(*UI_Pannal, *UI_Shader, signInOutPannelPos, Vector2(400.0f, 320.0f));

			RenderLabel(usernameText, screenWidth, screenHeight);
			passwordText.text = "Password";
			RenderLabel(passwordText, screenWidth, screenHeight);
			RenderLabel(loginLabel, screenWidth, screenHeight);
			RenderLabel(cancelLabel, screenWidth, screenHeight);
			RenderLabel(createAccountLabel, screenWidth, screenHeight);

			textRenderer.RenderText(signInUserName.c_str(), signInOutPannelPos + Vector2(40.0f, 40.0f), 0.8f, Vector3(1.0f, 1.0f, 1.0f), screenWidth, screenHeight);
			std::string hidePass(signInPassword.size(), '*');
			textRenderer.RenderText(hidePass, signInOutPannelPos + Vector2(40.0f, -60.0f), 0.8f, Vector3(1.0f, 1.0f, 1.0f), screenWidth, screenHeight);
		}
		else if (currentMenuState == MenuState::CreateAccount)
		{
			usernameText.position = createAccountPannelPos + Vector2(20.0f, 160.0f);
			passwordText.position = createAccountPannelPos + Vector2(20.0f, 60.0f);

			// Username
			spriteRenderer->Render(*UI_Enter, *UI_Shader, createAccountPannelPos + Vector2(30.0f, 70.0f), Vector2(350.0f, 50.0f));
			// Password
			spriteRenderer->Render(*UI_Enter, *UI_Shader, createAccountPannelPos + Vector2(30.0f, 170.0f), Vector2(350.0f, 50.0f));
			// Re-enter password
			spriteRenderer->Render(*UI_Enter, *UI_Shader, createAccountPannelPos + Vector2(30.0f, 270.0f), Vector2(350.0f, 50.0f));

			spriteRenderer->Render(*UI_Pannal, *UI_Shader, createAccountPannelPos, Vector2(400.0f, 380.0f));

			RenderLabel(usernameText, screenWidth, screenHeight);
			passwordText.text = "Password";
			RenderLabel(passwordText, screenWidth, screenHeight);
			passwordText.text = "Re-Enter Password";
			passwordText.position += Vector2(0.0f, -100.0f);
			RenderLabel(passwordText, screenWidth, screenHeight);
			createAccountLabel.position = createAccountPannelPos + Vector2(20.0f, -140.0f);
			createAccountLabel.text = "Create Account";
			RenderLabel(createAccountLabel, screenWidth, screenHeight);
			cancelLabel.position = createAccountPannelPos + Vector2(290.0f, -140.0f);
			RenderLabel(cancelLabel, screenWidth, screenHeight);

			textRenderer.RenderText(signInUserName.c_str(), signInOutPannelPos + Vector2(40.0f, 70.0f), 0.8f, Vector3(1.0f, 1.0f, 1.0f), screenWidth, screenHeight);
			std::string hidePass(signInPassword.size(), '*');
			textRenderer.RenderText(hidePass, signInOutPannelPos + Vector2(40.0f, -30.0f), 0.8f, Vector3(1.0f, 1.0f, 1.0f), screenWidth, screenHeight);
			std::string hideReEnterPass(signInReEnterPass.size(), '*');
			textRenderer.RenderText(hideReEnterPass, signInOutPannelPos + Vector2(40.0f, -130.0f), 0.8f, Vector3(1.0f, 1.0f, 1.0f), screenWidth, screenHeight);
		}
	}

	glDisable(GL_BLEND);
}

void MainMenu::RenderLeaderboardEntry(GLsizei screenWidth, GLsizei screenHeight)
{
	int index = 0;
	float firstEntryHeight = (screenHeight - leaderboardUIPos.y) - 150.0f;
	float firstEntryWidth = (screenWidth - leaderboardUIPos.x) + 10.0f;

	textRenderer.RenderText("Rank", Vector2(leaderboardUIPos.x + 10.0f, firstEntryHeight + (-50 * -1)), 0.6f, Vector3(0.0f, 1.0f, 0.0f), screenWidth, screenHeight);
	textRenderer.RenderText("Player", Vector2(leaderboardUIPos.x + 160.0f, firstEntryHeight + (-50 * -1)), 0.6f, Vector3(0.0f, 1.0f, 0.0f), screenWidth, screenHeight);
	textRenderer.RenderText("Time", Vector2(leaderboardUIPos.x + 380.0f, firstEntryHeight + (-50 * -1)), 0.6f, Vector3(0.0f, 1.0f, 0.0f), screenWidth, screenHeight);

	for (std::vector<LeaderboardEntry>::iterator it = GameSparksInfo::leaderboardEntry.begin();
		it != GameSparksInfo::leaderboardEntry.end(); it++, index++)
	{
		std::ostringstream oss;
		oss << it->rank.GetValue();
		textRenderer.RenderText(oss.str(), Vector2(leaderboardUIPos.x + 30.0f, firstEntryHeight + (-50 * index)), 0.6f, defaultColor, screenWidth, screenHeight);
		textRenderer.RenderText(it->username.GetValue().c_str(), Vector2(leaderboardUIPos.x + 100.0f, firstEntryHeight + (-50 * index)), 0.6f, defaultColor, screenWidth, screenHeight);
		oss.str("");
		oss << it->time.GetValue();
		textRenderer.RenderText(oss.str(), Vector2(leaderboardUIPos.x + 370.0f, firstEntryHeight + (-50 * index)), 0.6f, defaultColor, screenWidth, screenHeight);
	}
}

void MainMenu::RenderLabel(MenuText &menuLable, GLsizei screenWidth, GLsizei sceenHeight)
{
	textRenderer.RenderText(menuLable.text, menuLable.position, menuLable.scale, menuLable.color, screenWidth, sceenHeight);
}

void MainMenu::PlayMenuError()
{
	menuSound.Play2D("Resources/Sounds/Menu/Menu_Error.wav");
}

void MainMenu::PlayMenuNav()
{
	menuSound.Play2D("Resources/Sounds/Menu/Menu_Nav.wav");
}

void MainMenu::PlayMenuSelect()
{
	menuSound.Play2D("Resources/Sounds/Menu/Menu_Select.wav");
}

void MainMenu::PlayKeyboardEnter()
{
	if (InputManager::GetInstance().charaterKeyPressed)
	{
		menuSound.Play2D("Resources/Sounds/Menu/Keyboard_Enter.wav");
		InputManager::GetInstance().charaterKeyPressed = false;
	}
}

void MainMenu::PlayBackspaceSound()
{
	menuSound.Play2D("Resources/Sounds/Menu/Keyboard_Backspace.wav");
}