
#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Options : public Scene
{

protected:

    std::string                 m_title;
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_menuStrings2;
    sf::Text                    m_menuText;
    sf::Sprite                  m_backgroundSprite;
    size_t                      m_selectedMenuIndex = 0;
    Vec2                        menuTextPos = getPosition(0, 0, 8, 1);
    Vec2                        menuOptionsPos = getPosition(0, 0, 4, 3);
    Vec2                        menuOptionsPos2 = getPosition(0, 0, 12, 3);
    std::string                 ConfigRead;
    int                         numToDisplay = 6;
    int                         startnum = 0;

    bool                        m_selected = false;

    int                         m_musicVolume = 100;
    int                         m_effectVolume = 100;
    std::string                 m_difficulty = "Normal";
    sf::Keyboard::Key           m_jumpKey = sf::Keyboard::W;
    sf::Keyboard::Key           m_leftKey = sf::Keyboard::A;
    sf::Keyboard::Key           m_rightKey = sf::Keyboard::D;
    sf::Keyboard::Key           m_downKey = sf::Keyboard::S;
    sf::Keyboard::Key           m_pauseKey = sf::Keyboard::P;
    sf::Keyboard::Key           m_interactKey = sf::Keyboard::F;
    sf::Keyboard::Key           m_attackKey = sf::Keyboard::Space;
    sf::Keyboard::Key           m_weaponSwitchKey = sf::Keyboard::Tab;
    sf::Keyboard::Key           m_zoomMapKey = sf::Keyboard::O;
    sf::Keyboard::Key           m_miniMapKey = sf::Keyboard::M;
    sf::Keyboard::Key           m_inventoryKey = sf::Keyboard::I;
    sf::Keyboard::Key           m_selectRightKey = sf::Keyboard::Right;
    sf::Keyboard::Key           m_selectLeftKey = sf::Keyboard::Left;
    sf::Keyboard::Key           m_useItemKey = sf::Keyboard::E;
    sf::Keyboard::Key           m_toggleTextureKey = sf::Keyboard::T;
    sf::Keyboard::Key           m_toggleGridKey = sf::Keyboard::G;
    sf::Keyboard::Key           m_toggleCollisionKey = sf::Keyboard::C;

    void init();
    void update();
    void onEnd();
    void sDoAction(const Action& action);
    void saveOptions();
public:

    Scene_Options(GameEngine* gameEngine = nullptr);
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    void sRender();

};

