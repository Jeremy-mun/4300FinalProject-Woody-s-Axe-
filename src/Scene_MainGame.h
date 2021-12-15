
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
    struct MovingTileConfig
    {
        float RX, RY, TX, TY, S, N;
        std::vector<Vec2> XnY;
        bool BM, BV;
        std::string Name, AI;
    };
    struct ItemConfig
    {
        float RX, RY, TX, TY;
        bool BM, BV;
        std::string Name, PositionInGrid;
    };
                           
protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    std::string             configRead;
    const Vec2              m_gridSize = { 64, 64 };
    Vec2                    m_mPos = Vec2(0.0f, 0.0f);
    bool                    m_drawGrid = false;
    TileConfig              m_tileConfig;
    NPCConfig               m_npcConfig;
    ItemConfig              m_itemConfig;
    MovingTileConfig        m_movingTileConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = true;
    bool                    m_teleported = false;
    bool                    inventoryOpened = false;
    int                     m_weaponSwitch = 0;
    int                     m_select = 0;
    bool                    m_playerOnGround = false;
    bool                    m_playerOnMovingTile = false;
    bool                    m_minimap = true;
    bool                    m_collidingWithTile = false;
    bool                    m_canCrouch = true;
    int                     m_roomX = 0;
    int                     m_roomY = 0;
    int                     m_wallet = 0;
    int                     m_FrameSinceGrounded = 0;
    int                     m_MaxYSpeed = 30;
    sf::Text                m_gridText;
    sf::Text                m_tutorialText;
    sf::Text                m_levelText;
    sf::Text                m_walletText;
    sf::Text                m_arrowHolderText;
    sf::Clock               m_weaponTextClock;
    sf::Clock               m_tutorialTextClock;
    sf::Clock               m_walletClock;
    sf::Clock               m_InventoryClock;
    sf::Clock               m_time;
    sf::Clock               m_gravityClock;
    sf::Clock               m_crouchClock;
    int                     m_frameSinceAttack;
    sf::CircleShape         inventorySelect;

    //options
    std::string                 ConfigRead;
    int                         m_musicVolume = 100;
    int                         m_effectVolume = 100;
    std::string                 m_difficulty = "Normal";
    float                       m_difficultymod = 1;
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

    //saveGame
    std::string               m_saveFile;
    std::string               m_saveRead;
    bool					  m_level1Completion = false, m_level2Completion = false, m_level3Completion = false;
    int						  m_maxHealth = 10, m_damage = 1, m_arrowCount = 10, m_coinCount = 0;

    sf::Sprite             parallax1, parallax2, parallax3, parallax4, parallax5, parallax6 , parallax7 , parallax8 , parallax9, parallax10, m_lighting;
    std::vector<sf::String> m_parallaxBackgroundTextures = { "TexParallax1" , "TexParallax2" ,"TexParallax3" ,"TexParallax4" 
                                                    ,"TexParallax5" ,"TexParallax6" ,"TexParallax7" 
                                                    ,"TexParallax8" ,"TexParallax9" ,"TexParallax10" };
    std::vector<sf::Sprite> m_parallaxBackgroundSprites = { parallax1, parallax2, parallax3, parallax4, parallax5, parallax6 , parallax7 , parallax8 , parallax9, parallax10 };

    std::vector<sf::String> m_shaders =
    {
        "shaders/shaderFade.frag",
        "shaders/shaderInvincibility.frag",
        "shaders/shaderLighting.frag"
    };
     
    

    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);
    void loadParallaxBackground();
    void drawParallaxBackground();
    void onEnd();
    void update();
    void spawnPlayer();
    void startAttack(std::shared_ptr<Entity> entity);
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    void drawLine(const Vec2& p1, const Vec2& p2);
    void sMovement();
    void sAI();
    void sBossAI();
    void sTilesAI();
    void sArrowMovement();
    void sStatus();
    void sAnimation();
    void sCollision();
    void sTileCollision();
    void sPlayerCollision();
    void sMeleeCollision();
    void sBreakableCollision();
    void sArrowCollision();
    void sHeartCollision();
    void sArrowPickCollision();
    void sCoinCollision();
    void sItemCollision();
    void sTeleportCollision();
    void sEnemyCollision();
    void sCamera();
    void drawWeaponSwap();
    void drawWeaponHolder();
    void sHUD();
    void sDrawInventoryItems();
    void drawInventory();
    void drawMinimap();
    void sDragAndDrop();
    void grab();
    void snap(std::shared_ptr<Entity> e);
    void sInteract();
    void sUseItem(std::shared_ptr<Entity> entity);
    void select(std::string direction);
    void loadOptions();
    void loadSaveGame();
    void saveGame();
    void levelCompleted();

    
public:

    Scene_MainGame(GameEngine* gameEngine, const std::string& levelPath, const std::string& saveFile);
                           
    void sRender();
    void sDoAction(const Action& action);
};

