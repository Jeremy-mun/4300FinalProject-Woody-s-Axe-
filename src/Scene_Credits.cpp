
#include "GameEngine.h"
#include "Action.h"
#include "Common.h"
#include "Assets.h"
#include "Scene_Credits.h"
#include "Scene_MainGame.h"

Scene_Credits::Scene_Credits(GameEngine* game)
	:Scene(game)
{
	init();
}

void Scene_Credits::init()
{
	registerAction(sf::Keyboard::D, "ENTER");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "Woody's AXE";

	loadCredits();

	m_menuText.setFont(m_game->assets().getFont("Arial"));
	m_menuText.setCharacterSize(64);
}

void Scene_Credits::loadCredits()
{
	std::ifstream credits("credits.txt");
	std::string line;
	if (credits.is_open())
	{
		while (credits.good())
		{
			credits >> line;
			m_menuStrings.push_back(line);
		}
	}
}

void Scene_Credits::update()
{
	m_entityManager.update();
}

void Scene_Credits::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Credits::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "ENTER" || action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

Vec2 Scene_Credits::getPosition(int rx, int ry, int tx, int ty) const
{
	float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
	float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
	return Vec2(x, y);
}

void Scene_Credits::sRender()
{
	// clear the window to a black
	m_game->window().setView(m_game->window().getDefaultView());
	m_game->window().clear(sf::Color(0, 0, 0));

	// draw the game title 
	m_menuText.setCharacterSize(48);
	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color(137, 3, 6));
	m_menuText.setPosition(sf::Vector2f(menuTextPos.x, menuTextPos.y));
	m_game->window().draw(m_menuText);

	// draw the Credits
	for (size_t i = 0; i < m_menuStrings.size(); i++)
	{
		m_menuText.setString(m_menuStrings[i]);
		m_menuText.setPosition(sf::Vector2f(menuOptionsPos.x, menuOptionsPos.y + i * 72));
		m_game->window().draw(m_menuText);
	}
}