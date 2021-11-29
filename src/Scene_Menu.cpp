
#include "Scene_Menu.h"
#include "Scene_MainGame.h"
#include "Scene_Level_Editor.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
                           
Scene_Menu::Scene_Menu(GameEngine * gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    registerAction(sf::Keyboard::W,     "UP");
    registerAction(sf::Keyboard::S,     "DOWN");
    registerAction(sf::Keyboard::D,     "MainGame");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_title = "Woody's AXE";
    m_menuStrings.push_back("Level  1");
    m_menuStrings.push_back("Level  2");
    m_menuStrings.push_back("Level  3");
    m_menuStrings.push_back("Level  Editor");
    m_menuStrings.push_back("Load Level");

    m_levelPaths.push_back("level1.txt");
    m_levelPaths.push_back("level2.txt");
    m_levelPaths.push_back("level3.txt");

    m_menuText.setFont(m_game->assets().getFont("Gypsy"));
    m_menuText.setCharacterSize(64);

    m_game->playSound("MusicTitle");

    m_backgroundSprite.setTexture(m_game->assets().getTexture("MainMenuBackground"));
    m_backgroundSprite.setOrigin(m_game->window().getSize().x / 10, m_game->window().getSize().y / 6);
    m_backgroundSprite.move(sf::Vector2f(130, 125));
}

void Scene_Menu::update()
{
    m_entityManager.update();
}

void Scene_Menu::sDoAction(const Action& action)
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
        else if (action.name() == "MainGame")
        {
            if (m_selectedMenuIndex < 3)
            {
                m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, m_levelPaths[m_selectedMenuIndex]));
            }
            else
            {
                m_game->changeScene("Level_Editor", std::make_shared<Scene_Level_Editor>(m_game, "blankLevel.txt"));
            }
            m_game->assets().getSound("MusicTitle").stop();
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}
Vec2 Scene_Menu::getPosition(int rx, int ry, int tx, int ty) const
{
    float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
    float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
    return Vec2(x, y);
}

void Scene_Menu::sRender()
{
    // clear the window to a black
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(0, 0, 0));
    m_game->window().draw(m_backgroundSprite);

    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(48);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color(5, 220, 250));
    m_menuText.setPosition(sf::Vector2f(menuTextPos.x, menuTextPos.y));
    m_game->window().draw(m_menuText);
    
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(5, 220, 250));
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

void Scene_Menu::onEnd()
{
    m_hasEnded = true;
    m_game->quit();
}
