#include "MainMenu.h"

MainMenu::MainMenu()
	: defaultColor(1.0f, 1.0f, 1.0f), selectColor(0.4f, 0.4f, 1.0f)
{
	playLabel.position = Vector2(40.0f, 400.0f);
	playLabel.color = selectColor;
	playLabel.text = "Play Game";

	signInOutLabel.position = Vector2(40.0f, 350.0f);
	signInOutLabel.color = defaultColor;
	signInOutLabel.text = "Sign Out";

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
	worldRankLabel.text = "World Rank: #12";
	worldRankLabel.color = defaultColor;
	worldRankLabel.scale = 0.5f;

	currentPlayer.position = Vector2(80.0f, 15.0f);
	currentPlayer.text = "SpyroTheDragon";
	currentPlayer.color = defaultColor;
	currentPlayer.scale = 0.5f;

	leaderboardTitle.position = Vector2(500.0f, 500.0f);
	leaderboardTitle.text = "World Top 10";
	leaderboardTitle.color = defaultColor;
	leaderboardTitle.scale = 0.7f;

	serverStatus.position = Vector2(700, 10);
	serverStatus.color = Vector3(1.0f, 0.0f, 0.0f);
	serverStatus.scale = 0.7f;
	serverStatus.text = "Offline";

	selectPosition = playLabel.position;

	UI_Shader = g_resourceMgr.GetShader(SID("UI_Shader"));
	UI_Bottom = g_resourceMgr.GetTexture(SID("Bottom_UI"));
	UI_Pannal = g_resourceMgr.GetTexture(SID("Menu_UI"));

	spriteRenderer = new Sprite();
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
	selectRect.SetRectangle(selectPosition, 100, 50);
}

void MainMenu::UpdateScene(float delatTime)
{
	gameSparksInfo.Update();

	if (gameSparksInfo.GetStatus())
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
			if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_UP) && selectPosition.y != playLabel.position.y)
				selectPosition.y += 50.0f;
			else if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN) && selectPosition.y != exitLabel.position.y)
				selectPosition.y -= 50.0f;

			UpdateLable(playLabel);
			if (playLabel.labelSelected)
				currentSelectState = SelectState::PlaySelected;
			UpdateLable(signInOutLabel);
			if (signInOutLabel.labelSelected)
				currentSelectState = SelectState::SignInOutSeleted;
			UpdateLable(exitLabel);
			if (exitLabel.labelSelected)
			{
				currentSelectState = SelectState::ExitSelected;
				currentMenuState = MenuState::ExitOpitions;
				selectPosition = exitPannelPosition;
			}

			break;
		}

		case MenuState::ExitOpitions:
		{
			if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT) && selectPosition.x != exitPannelPosition.x + 150.0f)
				selectPosition.x += 150.0f;
			else if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT) && selectPosition.x != exitPannelPosition.x)
				selectPosition.x -= 150.0f;

			exitNo.rect.SetRectangle(exitNo.position, 30, 50);
			exitYes.rect.SetRectangle(exitYes.position, 30, 50);

			UpdateLable(exitNo);
			if (exitNo.labelSelected)
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
}

void MainMenu::UpdateLable(MenuLabel &label)
{
	if (selectRect.Intersects(label.rect))
	{
		label.color = selectColor;

		if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER))
			label.labelSelected = true;
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
	leaderboardUIPos = Vector2(screenWidth - 330.0f, 30.0f);
	exitPannelPosition = Vector2((screenWidth / 2) - 200.0f, (screenHeight / 2) - 70.0f);
	exitAsk.position = exitPannelPosition + Vector2(150.0f, 90.0f);
	exitNo.position = exitPannelPosition + Vector2(100.0f, 20.0f);
	exitYes.position = exitPannelPosition + Vector2(250.0f, 20.0f);

	Matrix4 projection = Matrix4();
	projection = projection.orthographicProjection(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
	UI_Shader->Use();
	glUniformMatrix4fv(glGetUniformLocation(UI_Shader->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	spriteRenderer->Render(*UI_Bottom, *UI_Shader, Vector2(0.0f, screenHeight - 100.0f), Vector2(screenWidth + 40.0f, 100.0f));
	spriteRenderer->Render(*UI_Pannal, *UI_Shader, Vector2(-5.0f, pannelHeight - 80.0f), Vector2(300.0f, 220.0f));
	spriteRenderer->Render(*UI_Pannal, *UI_Shader, leaderboardUIPos, Vector2(300.0f, 500.0f));
	if (currentSelectState == SelectState::ExitSelected)
	{
		spriteRenderer->Render(*UI_Pannal, *UI_Shader, exitPannelPosition, Vector2(400.0f, 150.0f));
	}
	leaderboardTitle.position = Vector2(screenWidth - 300.0f, screenHeight - 80.0f);

	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderLabel(playLabel, screenWidth, screenHeight);
	RenderLabel(signInOutLabel, screenWidth, screenHeight);
	RenderLabel(exitLabel, screenWidth, screenHeight);
	RenderLabel(worldRankLabel, screenWidth, screenHeight);
	RenderLabel(currentPlayer, screenWidth, screenHeight);
	RenderLabel(leaderboardTitle, screenWidth, screenHeight);
	RenderLeaderboardEntry(screenWidth, screenHeight);
	serverStatus.position.x = screenWidth - 110;
	RenderLabel(serverStatus, screenWidth, screenHeight);

	if (currentSelectState == SelectState::ExitSelected)
	{
		RenderLabel(exitAsk, screenWidth, screenHeight);
		RenderLabel(exitNo, screenWidth, screenHeight);
		RenderLabel(exitYes, screenWidth, screenHeight);
	}
	glDisable(GL_BLEND);
}

void MainMenu::RenderLeaderboardEntry(GLsizei screenWidth, GLsizei screenHeight)
{
	int index = 0;
	float firstEntryHeight = (screenHeight - leaderboardUIPos.y) - 100.0f;
	float firstEntryWidth = (screenWidth - leaderboardUIPos.x) + 10.0f;

	for (std::vector<LeaderboardEntry>::iterator it = gameSparksInfo.leaderboardEntry.begin();
		it != gameSparksInfo.leaderboardEntry.end(); it++, index++)
	{
		std::ostringstream oss;
		oss << it->rank.GetValue();
		textRenderer.RenderText(oss.str(), Vector2(leaderboardUIPos.x + 10.0f, firstEntryHeight + (-50 * index)), 0.6f, defaultColor, screenWidth, screenHeight);
		textRenderer.RenderText(it->username.GetValue().c_str(), Vector2(leaderboardUIPos.x + 50.0f, firstEntryHeight + (-50 * index)), 0.6f, defaultColor, screenWidth, screenHeight);
		oss.str("");
		oss << it->time.GetValue();
		textRenderer.RenderText(oss.str(), Vector2(leaderboardUIPos.x + 250.0f, firstEntryHeight + (-50 * index)), 0.6f, defaultColor, screenWidth, screenHeight);
	}
}

void MainMenu::RenderLabel(MenuText &menuLable, GLsizei screenWidth, GLsizei sceenHeight)
{
	textRenderer.RenderText(menuLable.text, menuLable.position, menuLable.scale, menuLable.color, screenWidth, sceenHeight);
}