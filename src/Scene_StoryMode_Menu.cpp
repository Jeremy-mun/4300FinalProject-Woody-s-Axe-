
#include "Scene_StoryMode_Menu.h"
#include "GameEngine.h"
#include "Scene_Overworld.h"
#include "Action.h"
#include "Components.h"
#include "Common.h"
#include "Assets.h"

Scene_StoryMode_Menu::Scene_StoryMode_Menu(GameEngine* game, const std::string continueOrnew)
    :Scene(game)
    , m_continueOrNew(continueOrnew)
{
    init();
}

void Scene_StoryMode_Menu::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "ENTER");
    registerAction(sf::Keyboard::Escape, "QUIT");
    if (m_continueOrNew == "CONTINUE")
    {
        m_title = "Woody's AXE: Load Save File";
    }
    else if (m_continueOrNew == "NEW")
    {
        m_title = "Woody's AXE: New Save File";
    }

    size_t num = 1;
    for (auto& p : std::filesystem::directory_iterator("saves"))
    {
        std::string name = p.path().string();

        std::stringstream ss;
        ss << ((num < 10) ? " " : "") << (num) << ") " << name.substr(7);
        num++;

        m_menuStrings.push_back(ss.str());
        m_savePaths.push_back(name);
    }

    m_menuText.setFont(m_game->assets().getFont("Gypsy"));
    m_menuText.setCharacterSize(64);

    m_backgroundSprite.setTexture(m_game->assets().getTexture("MainMenuBackground"));
    m_backgroundSprite.setOrigin(m_game->window().getSize().x / 10, m_game->window().getSize().y / 6);
    m_backgroundSprite.move(sf::Vector2f(130, 125));
}

void Scene_StoryMode_Menu::update()
{
    m_entityManager.update();
}

void Scene_StoryMode_Menu::sDoAction(const Action& action)
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
            if (m_continueOrNew == "NEW")
            {
                std::ofstream save(m_savePaths[m_selectedMenuIndex]);
                if (save.is_open())
                {
                    save.clear();
                }
            }
            m_game->stopSound("MusicTitle");
            m_game->changeScene("Overworld", std::make_shared<Scene_Overworld>(m_game, m_savePaths[m_selectedMenuIndex]));
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

Vec2 Scene_StoryMode_Menu::getPosition(int rx, int ry, int tx, int ty) const
{
    float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
    float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
    return Vec2(x, y);
}

void Scene_StoryMode_Menu::sRender()
{

    // clear the window to a blue
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

void Scene_StoryMode_Menu::onEnd()
{
    m_game->changeScene("MENU", nullptr, true);
}