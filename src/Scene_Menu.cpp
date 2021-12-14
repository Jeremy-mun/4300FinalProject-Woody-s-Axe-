
#include "Scene_Menu.h"
#include "Scene_MainGame.h"
#include "Scene_Level_Editor.h"
#include "Scene_Level_Editor_Menu.h"
#include "Scene_LoadGame_Menu.h"
#include "Scene_StoryMode_Menu.h"
#include "Scene_Options.h"
#include "Scene_Credits.h"
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
    registerAction(sf::Keyboard::D,     "ENTER");
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
    

    m_title = "Woody's AXE";
    m_menuStrings.push_back("New Game");
    m_menuStrings.push_back("Continue");
    m_menuStrings.push_back("Level  Editor");
    m_menuStrings.push_back("Load Level");
    m_menuStrings.push_back("Options");
    m_menuStrings.push_back("Credits");


    m_menuText.setFont(m_game->assets().getFont("Gypsy"));
    m_menuText.setCharacterSize(64);

    m_game->playSound("MusicTitle");
    m_game->setVolume("MusicTitle", m_musicVolume);


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
        else if (action.name() == "ENTER")
        {
            if (m_menuStrings[m_selectedMenuIndex] == "New Game")
            {
                m_game->changeScene("Overworld", std::make_shared<Scene_StoryMode_Menu>(m_game, "NEW"));
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Continue")
            {
                m_game->changeScene("Overworld", std::make_shared<Scene_StoryMode_Menu>(m_game, "CONTINUE"));
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Level  Editor")
            {
                m_game->changeScene("Level_Editor_Menu", std::make_shared<Scene_Level_Editor_Menu>(m_game));
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Load Level")
            {
                m_game->changeScene("Load_Level_Menu", std::make_shared<Scene_LoadGame_Menu>(m_game));
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Options")
            {
                m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Credits")
            {
                m_game->changeScene("Credits", std::make_shared<Scene_Credits>(m_game));
            }
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
    m_menuText.setFillColor(sf::Color(137, 3, 6));
    m_menuText.setPosition(sf::Vector2f(menuTextPos.x, menuTextPos.y));
    m_game->window().draw(m_menuText);
    
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(137, 3, 6));
        m_menuText.setPosition(sf::Vector2f(menuOptionsPos.x - 32, menuOptionsPos.y + i * 72));
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
