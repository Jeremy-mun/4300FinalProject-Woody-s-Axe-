
#include "Scene_Options.h"
#include "Scene_MainGame.h"
#include "Scene_Level_Editor.h"
#include "Scene_Level_Editor_Menu.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_Options::Scene_Options(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Options::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "ENTER");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::Right, "RIGHT");
    registerAction(sf::Keyboard::Left, "LEFT");

    m_title = "Options";
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
            else if (ConfigRead == "Difficulty")
            {
                config >> m_difficulty;
            }
            else if (ConfigRead == "Jump")
            {
                int keyval;
                config >> keyval;
                m_jumpKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Left")
            {
                int keyval;
                config >> keyval;
                m_leftKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Right")
            {
                int keyval;
                config >> keyval;
                m_rightKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Down")
            {
                int keyval;
                config >> keyval;
                m_downKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Pause")
            {
                int keyval;
                config >> keyval;
                m_pauseKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Interact")
            {
                int keyval;
                config >> keyval;
                m_interactKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Attack")
            {
                int keyval;
                config >> keyval;
                m_attackKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "WeaponSwitch")
            {
                int keyval;
                config >> keyval;
                m_weaponSwitchKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "ZoomMap")
            {
                int keyval;
                config >> keyval;
                m_zoomMapKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "MiniMap")
            {
                int keyval;
                config >> keyval;
                m_miniMapKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "Inventory")
            {
                int keyval;
                config >> keyval;
                m_inventoryKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "SelectRight")
            {
                int keyval;
                config >> keyval;
                m_selectRightKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "SelectLeft")
            {
                int keyval;
                config >> keyval;
                m_selectLeftKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "UseItem")
            {
                int keyval;
                config >> keyval;
                m_useItemKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "ToggleTexture")
            {
                int keyval;
                config >> keyval;
                m_toggleTextureKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "ToggleCollision")
            {
                int keyval;
                config >> keyval;
                m_toggleCollisionKey = sf::Keyboard::Key(keyval);
            }
            else if (ConfigRead == "ToggleGrid")
            {
                int keyval;
                config >> keyval;
                m_toggleGridKey = sf::Keyboard::Key(keyval);
            }
        }
    }
    m_menuText.setFont(m_game->assets().getFont("Gypsy"));
    m_menuText.setCharacterSize(64);

    m_menuStrings.push_back("Music Volume:");
    m_menuStrings.push_back("Sound Effects Volume:");
    m_menuStrings.push_back("Difficulty:");
    m_menuStrings.push_back("Jump:");
    m_menuStrings.push_back("Move Left:");
    m_menuStrings.push_back("Move Right:");
    m_menuStrings.push_back("Crouch:");
    m_menuStrings.push_back("Pause:");
    m_menuStrings.push_back("Interact:");
    m_menuStrings.push_back("Attack:");
    m_menuStrings.push_back("Weapon Switch:");
    m_menuStrings.push_back("Zoom Map:");
    m_menuStrings.push_back("Open Mini Map:");
    m_menuStrings.push_back("Open Inventory:");
    m_menuStrings.push_back("Inventory Select Right:");
    m_menuStrings.push_back("Inventory Select Left:");
    m_menuStrings.push_back("Use Item:");
    m_menuStrings.push_back("Toggle Texture:");
    m_menuStrings.push_back("Toggle Collision:");
    m_menuStrings.push_back("Toggle Grid:");

    m_menuStrings2.push_back(std::to_string(m_musicVolume));
    m_menuStrings2.push_back(std::to_string(m_effectVolume));
    m_menuStrings2.push_back(m_difficulty);
    m_menuStrings2.push_back(m_game->keyToString(m_jumpKey));
    m_menuStrings2.push_back(m_game->keyToString(m_leftKey));
    m_menuStrings2.push_back(m_game->keyToString(m_rightKey));
    m_menuStrings2.push_back(m_game->keyToString(m_downKey));
    m_menuStrings2.push_back(m_game->keyToString(m_pauseKey));
    m_menuStrings2.push_back(m_game->keyToString(m_interactKey));
    m_menuStrings2.push_back(m_game->keyToString(m_attackKey));
    m_menuStrings2.push_back(m_game->keyToString(m_weaponSwitchKey));
    m_menuStrings2.push_back(m_game->keyToString(m_zoomMapKey));
    m_menuStrings2.push_back(m_game->keyToString(m_miniMapKey));
    m_menuStrings2.push_back(m_game->keyToString(m_inventoryKey));
    m_menuStrings2.push_back(m_game->keyToString(m_selectRightKey));
    m_menuStrings2.push_back(m_game->keyToString(m_selectLeftKey));
    m_menuStrings2.push_back(m_game->keyToString(m_useItemKey));
    m_menuStrings2.push_back(m_game->keyToString(m_toggleTextureKey));
    m_menuStrings2.push_back(m_game->keyToString(m_toggleCollisionKey));
    m_menuStrings2.push_back(m_game->keyToString(m_toggleGridKey));



    m_backgroundSprite.setTexture(m_game->assets().getTexture("MainMenuBackground"));
    m_backgroundSprite.setOrigin(m_game->window().getSize().x / 10, m_game->window().getSize().y / 6);
    m_backgroundSprite.move(sf::Vector2f(130, 125));
}

void Scene_Options::update()
{
    m_entityManager.update();
}

void Scene_Options::sDoAction(const Action& action)
{
    if (action.name() == "REMAP")
    {
        if (m_menuStrings[m_selectedMenuIndex] == "Jump:")
        {
            m_jumpKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_jumpKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Move Left:")
        {
            m_leftKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_leftKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Move Right:")
        {
            m_rightKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_rightKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Crouch:")
        {
            m_downKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_downKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Pause:")
        {
            m_pauseKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_pauseKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Interact:")
        {
            m_interactKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_interactKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Attack:")
        {
            m_attackKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_attackKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Weapon Switch:")
        {
            m_weaponSwitchKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_weaponSwitchKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Zoom Map:")
        {
            m_zoomMapKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_zoomMapKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Open Mini Map:")
        {
            m_miniMapKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_miniMapKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Open Inventory:")
        {
            m_inventoryKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_inventoryKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Inventory Select Right:")
        {
            m_selectRightKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_selectRightKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Inventory Select Left:")
        {
            m_selectLeftKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_selectLeftKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Use Item:")
        {
            m_useItemKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_useItemKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Toggle Texture:")
        {
            m_toggleTextureKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_toggleTextureKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Toggle Collision:")
        {
            m_toggleCollisionKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_toggleCollisionKey);
        }
        else if (m_menuStrings[m_selectedMenuIndex] == "Toggle Grid:")
        {
            m_toggleGridKey = action.key();
            m_menuStrings2[m_selectedMenuIndex] = m_game->keyToString(m_toggleGridKey);
        }
        m_selected = false;
        m_game->remap();
    }
    else if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
            if (m_selectedMenuIndex < startnum)
            {
                startnum--;
            }
            if (m_selectedMenuIndex == m_menuStrings.size() - 1)
            {
                startnum = m_menuStrings.size() - numToDisplay;
            }
        }
        else if (action.name() == "DOWN")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
            if (m_selectedMenuIndex > startnum + numToDisplay - 1)
            {
                startnum++;
            }
            if (m_selectedMenuIndex == 0)
            {
                startnum = 0;
            }
        }
        else if (action.name() == "ENTER")
        {
            if (m_selectedMenuIndex > 2)
            {
                m_game->remap();
                m_selected = true;
            }
        }
        else if (action.name() == "RIGHT")
        {
            if (m_menuStrings[m_selectedMenuIndex] == "Music Volume:")
            {
                m_musicVolume = (m_musicVolume + 1) % 101;
                m_menuStrings2[m_selectedMenuIndex] = std::to_string(m_musicVolume);
                m_game->setVolume("MusicTitle", m_musicVolume);
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Sound Effects Volume:")
            {
                m_effectVolume = (m_effectVolume + 1) % 101;
                m_menuStrings2[m_selectedMenuIndex] = std::to_string(m_effectVolume);
            }
            else if (m_menuStrings[m_selectedMenuIndex] == "Difficulty:")
            {
                if (m_difficulty == "Easy")
                {
                    m_difficulty = "Normal";
                    m_menuStrings2[m_selectedMenuIndex] = m_difficulty;
                }
                else if (m_difficulty == "Normal")
                {
                    m_difficulty = "Hard";
                    m_menuStrings2[m_selectedMenuIndex] = m_difficulty;
                }
                else if (m_difficulty == "Hard")
                {
                    m_difficulty = "Easy";
                    m_menuStrings2[m_selectedMenuIndex] = m_difficulty;
                }
            }
        }
        else if (action.name() == "LEFT")
        {
            if (m_selectedMenuIndex == 0)
            {
                if (m_musicVolume > 0) { m_musicVolume--; }
                else { m_musicVolume = 100; }
                m_menuStrings2[0] = std::to_string(m_musicVolume);
                m_game->setVolume("MusicTitle", m_musicVolume);
            }
            else if (m_selectedMenuIndex == 1)
            {
                if (m_effectVolume > 0) { m_effectVolume--; }
                else { m_effectVolume = 100; }
                m_menuStrings2[1] = std::to_string(m_effectVolume);
            }
            else if (m_selectedMenuIndex == 2)
            {
                if (m_difficulty == "Easy")
                {
                    m_difficulty = "Hard";
                    m_menuStrings2[2] = m_difficulty;
                }
                else if (m_difficulty == "Normal")
                {
                    m_difficulty = "Easy";
                    m_menuStrings2[2] = m_difficulty;
                }
                else if (m_difficulty == "Hard")
                {
                    m_difficulty = "Normal";
                    m_menuStrings2[2] = m_difficulty;
                }
            }
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
            saveOptions();
        }
    }
}

void Scene_Options::saveOptions()
{
    std::ofstream config("options.txt");
    if (config.is_open())
    {
        config << "MusicVolume     ";
        config << m_musicVolume << " ";
        config << std::endl;
        config << "SFXVolume       ";
        config << m_effectVolume << " ";
        config << std::endl;
        config << "Difficulty      ";
        config << m_difficulty << " ";
        config << std::endl;
        config << "Jump            ";
        config << m_jumpKey << " ";
        config << std::endl;
        config << "Left            ";
        config << m_leftKey << " ";
        config << std::endl;
        config << "Right           ";
        config << m_rightKey << " ";
        config << std::endl;
        config << "Down            ";
        config << m_downKey << " ";
        config << std::endl;
        config << "Pause           ";
        config << m_pauseKey << " ";
        config << std::endl;
        config << "Interact        ";
        config << m_interactKey << " ";
        config << std::endl;
        config << "Attack          ";
        config << m_attackKey << " ";
        config << std::endl;
        config << "WeaponSwitch    ";
        config << m_weaponSwitchKey << " ";
        config << std::endl;
        config << "ZoomMap         ";
        config << m_zoomMapKey << " ";
        config << std::endl;
        config << "MiniMap         ";
        config << m_miniMapKey << " ";
        config << std::endl;
        config << "Inventory       ";
        config << m_inventoryKey << " ";
        config << std::endl;
        config << "SelectRight     ";
        config << m_selectRightKey << " ";
        config << std::endl;
        config << "SelectLeft      ";
        config << m_selectLeftKey << " ";
        config << std::endl;
        config << "UseItem         ";
        config << m_useItemKey << " ";
        config << std::endl;
        config << "ToggleTexture   ";
        config << m_toggleTextureKey << " ";
        config << std::endl;
        config << "ToggleCollision ";
        config << m_toggleCollisionKey << " ";
        config << std::endl;
        config << "ToggleGrid      ";
        config << m_toggleGridKey << " ";
        config << std::endl;
    }
    std::cout << "Options are saved" << std::endl;
}

Vec2 Scene_Options::getPosition(int rx, int ry, int tx, int ty) const
{
    float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
    float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
    return Vec2(x, y);
}

void Scene_Options::sRender()
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
    for (size_t i = startnum; i < startnum + numToDisplay; i++)
    {
        if (i >= m_menuStrings.size())
        {
            break;
        }
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(137, 3, 6));
        m_menuText.setPosition(sf::Vector2f(menuOptionsPos.x, menuOptionsPos.y + (i - startnum) * 72));
        m_game->window().draw(m_menuText);
    }
    for (size_t i = startnum; i < startnum + numToDisplay; i++)
    {
        if (i >= m_menuStrings2.size())
        {
            break;
        }
        m_menuText.setString(m_menuStrings2[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(137, 3, 6));
        m_menuText.setPosition(sf::Vector2f(menuOptionsPos2.x, menuOptionsPos2.y + (i - startnum) * 72));
        m_game->window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(36);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("up: w     down: s    select: d      back: esc");
    m_menuText.setPosition(sf::Vector2f(10, 690));
    m_game->window().draw(m_menuText);
}

void Scene_Options::onEnd()
{
    m_game->changeScene("MENU", nullptr, true);
}
