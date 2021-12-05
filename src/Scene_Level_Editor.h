
#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Level_Editor : public Scene
{

    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, HEALTH, GRAVITY;
        std::string WEAPON;
    };

    struct TileConfig
    {
        float RX, RY, TX, TY;
        bool BM, BV;
        std::string Name;
    };
    struct NPCConfig
    {
        float RX, RY, TX, TY, H, D, S, N;
        std::vector<Vec2> XnY;
        bool BM, BV;
        std::string Name, AI;
    };
    struct ItemConfig
    {
        float RX, RY, TX, TY;
        bool BM, BV;
        std::string Name;
    };

protected:
    EntityManager           m_editorManager;
    std::shared_ptr<Entity> m_selected;
    std::shared_ptr<Entity> m_editor;
    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    std::string             configRead;
    const Vec2              m_gridSize = { 64, 64 };
    Vec2                    m_mPos = Vec2(0.0f, 0.0f);
    bool                    m_drawGrid = true;
    std::string TilesPresent[3] = { "Bush", "BowRight", "AtkUp" };
    TileConfig              m_tileConfig;
    NPCConfig               m_npcConfig;
    ItemConfig              m_itemConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
    bool                    m_teleported = false;
    bool                    m_weaponSwitch = false;
    int                     m_roomX = 0;
    int                     m_roomY = 0;
    int                     m_wallet = 0;
    int                     tileIndex = 0;
    sf::Text                m_gridText;
    sf::Text                m_tutorialText;
    sf::Text                m_levelText;
    sf::Text                m_walletText;
    sf::Clock               m_weaponTextClock;
    sf::Clock               m_tutorialTextClock;
    sf::Clock               m_walletClock;

    void init(const std::string& levelPath);

    void loadLevel(const std::string& filename);
    void saveLevel(const std::string& filename);

    void onEnd();
    void update();
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    void drawLine(const Vec2& p1, const Vec2& p2);
    void placeTile(Animation animation);
    void sDragAndDrop();
    void sEditor();
    void grab();
    void spawnPlayer();
    void editor();
    void snap(std::shared_ptr<Entity> e);
    void draggable();
    void remove();

public:

    Scene_Level_Editor(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);

};