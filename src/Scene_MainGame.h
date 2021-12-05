
#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_MainGame : public Scene
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

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    std::string             configRead;
    const Vec2              m_gridSize = { 64, 64 };
    Vec2                    m_mPos = Vec2(0.0f, 0.0f);
    bool                    m_drawGrid = true;
    TileConfig              m_tileConfig;
    NPCConfig               m_npcConfig;
    ItemConfig              m_itemConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = true;
    bool                    m_teleported = false;
    int                     m_weaponSwitch = 0;
    bool                    m_playerOnGround = false;
    bool                    m_minimap = true;
    int                     m_roomX = 0;
    int                     m_roomY = 0;
    int                     m_wallet = 0;
    int                     m_FrameSinceGrounded = 0;
    sf::Text                m_gridText;
    sf::Text                m_tutorialText;
    sf::Text                m_levelText;
    sf::Text                m_walletText;
    sf::Clock               m_weaponTextClock;
    sf::Clock               m_tutorialTextClock;
    sf::Clock               m_walletClock;
    sf::Clock               m_InventoryClock;
    int                     m_frameSinceAttack;
    

    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void onEnd();
    void update();
    void spawnPlayer();
    void startAttack(std::shared_ptr<Entity> entity);
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    void drawLine(const Vec2& p1, const Vec2& p2);
    void sMovement();
    void sAI();
    void sArrowMovement();
    void sStatus();
    void sAnimation();
    void sCollision();
    void sTileCollision();
    void sPlayerCollision();
    void sMeleeCollision();
    void sArrowCollision();
    void sHeartCollision();
    void sCoinCollision();
    void sItemCollision();
    void sTeleportCollision();
    void sCamera();
    void drawWeaponHolder();
    void sHUD();
    void sAddToInventory(std::shared_ptr<Entity> entity, std::string Item);
    void drawInventory();
    void drawMinimap();
    void sDragAndDrop();
    void grab();
    void snap(std::shared_ptr<Entity> e);
    void sInteract();
    void sUseItem(std::shared_ptr<Entity> entity);

public:

    Scene_MainGame(GameEngine* gameEngine, const std::string& levelPath);
                           
    void sRender();
    void sDoAction(const Action& action);
};

