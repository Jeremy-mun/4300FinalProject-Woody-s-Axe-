
#include "GameEngine.h"
#include "Action.h"
#include "Components.h"
#include "Common.h"
#include "Assets.h"
#include "Scene_GameOver.h"
#include "Scene_MainGame.h"
#include "Scene_LoadGame_Menu.h"
#include "Scene_Overworld.h"

Scene_GameOver::Scene_GameOver(GameEngine* game, const std::string& levelPath, const std::string& saveFile)
	:Scene(game)
	, m_levelPath(levelPath)
	, m_saveFile(saveFile)
{
	init();
}

void Scene_GameOver::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "ENTER");
	registerAction(sf::Keyboard::Escape, "QUIT");

	std::ifstream config("options.txt");
	if (config.is_open())
	{
		while (config.good())
		{
			// Using the getPosition() function below to convert room-tile coords to game world coords
			//set variables equal to their values from the config file.
			config >> ConfigRead;
			if (ConfigRead == "MusicVolume")
			{
				config >> m_musicVolume;
			}
			else if (ConfigRead == "SFXVolume")
			{
				config >> m_effectVolume;
			}
		}
	}

	m_title = "Game Over";
	m_game->playSound("GameOver");
	m_menuStrings.push_back("Continue");
	m_menuStrings.push_back("Main Menu");

	m_menuText.setFont(m_game->assets().getFont("Gypsy"));
	m_menuText.setCharacterSize(64);
}

void Scene_GameOver::update()
{
	m_entityManager.update();
}

void Scene_GameOver::onEnd()
{
	if (m_saveFile == "NONE")
	{
		m_game->playSound("MusicTitle");
		m_game->setVolume("MusicTitle", m_musicVolume);
		m_game->loopSound("MusicTitle");
		m_game->changeScene("Load_Level_Menu", std::make_shared<Scene_LoadGame_Menu>(m_game));
	}
	else
	{
		m_game->playSound("MusicOverworld");
		m_game->setVolume("MusicOverworld", m_musicVolume);
		m_game->loopSound("MusicOverworld");
		m_game->changeScene("Overworld", std::make_shared<Scene_Overworld>(m_game, m_saveFile));
	}
}

void Scene_GameOver::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
			else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "ENTER")
		{
			if (m_selectedMenuIndex == 0)
			{
				m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, m_levelPath, m_saveFile));
			}
			else
			{
				onEnd();
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

Vec2 Scene_GameOver::getPosition(int rx, int ry, int tx, int ty) const
{
	float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
	float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
	return Vec2(x, y);
}

void Scene_GameOver::sRender()
{
	m_game->window().setView(m_game->window().getDefaultView());
	m_game->window().clear(sf::Color(0, 0, 0));

	//draw Game Over onto the screen
	m_menuText.setCharacterSize(256);
	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color(137, 3, 6));
	m_menuText.setPosition(sf::Vector2f(menuTextPos.x, menuTextPos.y));
	m_game->window().draw(m_menuText);

	m_menuText.setCharacterSize(64);

	// draw all of the menu options
	for (size_t i = 0; i < m_menuStrings.size(); i++)
	{
		m_menuText.setString(m_menuStrings[i]);
		m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(137, 3, 6));
		m_menuText.setPosition(sf::Vector2f(menuOptionsPos.x, menuOptionsPos.y + i * 72));
		m_game->window().draw(m_menuText);
	}

	// draw the controls in the bottom-left
	m_menuText.setCharacterSize(36);
	m_menuText.setFillColor(sf::Color(100, 100, 100));
	m_menuText.setString("up: w     down: s    play: d      back: esc");
	m_menuText.setPosition(sf::Vector2f(10, 690));
	m_game->window().draw(m_menuText);
}