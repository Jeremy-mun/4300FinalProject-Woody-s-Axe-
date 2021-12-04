
#include "Scene_Level_Editor_Menu.h"
#include "GameEngine.h"
#include "Scene_Level_Editor.h"
#include "Action.h"
#include "Components.h"
#include "Common.h"
#include "Assets.h"

Scene_Level_Editor_Menu::Scene_Level_Editor_Menu(GameEngine* game)
	:Scene(game)
{
	init();
}

void Scene_Level_Editor_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "ENTER");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "Woody's AXE: Level Editor Menu";

	size_t num = 1;
	for (auto& p : std::filesystem::directory_iterator("levels"))
	{
		std::string name = p.path().string();

		std::stringstream ss;
		ss << ((num < 10) ? " " : "") << (num) << ") " << name;
		num++;

		m_menuStrings.push_back(ss.str());
		m_levelPaths.push_back(name);
	}

	m_menuText.setFont(m_game->assets().getFont("Gypsy"));
	m_menuText.setCharacterSize(16);
}

void Scene_Level_Editor_Menu::update()
{

}

void Scene_Level_Editor_Menu::sDoAction(const Action& action)
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
            m_game->changeScene("Level_Editor", std::make_shared<Scene_Level_Editor>(m_game, m_levelPaths[m_selectedMenuIndex]));
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void Scene_Level_Editor_Menu::sRender()
{
    // clear the window to a blue
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(0, 0, 0));

    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(32);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setPosition(sf::Vector2f(12, 5));
    m_game->window().draw(m_menuText);

    m_menuText.setCharacterSize(16);
    const int filesPerLine = 38;
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::Yellow : sf::Color(127, 127, 127));
        m_menuText.setPosition(sf::Vector2f(32.0f + (float)(i / filesPerLine) * 450, 50.0f + (i % filesPerLine) * (float)m_menuText.getCharacterSize() + 2));
        m_game->window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(20);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setString("up: w   down: s   run: d   back: esc");
    m_menuText.setPosition(sf::Vector2f(10, 690));
    m_game->window().draw(m_menuText);

    m_game->window().display();
}

void Scene_Level_Editor_Menu::onEnd()
{
    m_game->changeScene("MENU", nullptr, true);
}