                          
#include "Scene_MainGame.h"
#include "Scene_GameOver.h"
#include "Scene_Overworld.h"
#include "Scene_LoadGame_Menu.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

Scene_MainGame::Scene_MainGame(GameEngine* game, const std::string& levelPath, const std::string& saveFile)
    : Scene(game)
    , m_levelPath(levelPath)
    , m_saveFile(saveFile)
{
    init(m_levelPath);
}

void Scene_MainGame::init(const std::string& levelPath)
{
    loadSaveGame();
    loadOptions();
    loadLevel(levelPath);
    loadParallaxBackground();

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(m_pauseKey, "PAUSE");
    registerAction(m_toggleTextureKey, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(m_toggleCollisionKey, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(m_toggleGridKey, "TOGGLE_GRID");         // Toggle drawing (G)rid Boxes
    registerAction(m_jumpKey, "UP");
    registerAction(m_downKey, "DOWN");
    registerAction(m_leftKey, "LEFT");
    registerAction(m_rightKey, "RIGHT");
    registerAction(m_interactKey, "INTERACT");
    registerAction(m_useItemKey, "USE_ITEM");
    registerAction(m_attackKey, "ATTACK");
    registerAction(m_weaponSwitchKey, "WEAPON_SWITCH");
    registerAction(m_zoomMapKey, "Zoom Map");
    registerAction(m_miniMapKey, "MiniMap");
    registerAction(m_inventoryKey, "OPEN_INVENTORY");
    registerAction(m_selectRightKey, "SELECT_RIGHT");
    registerAction(m_selectLeftKey, "SELECT_LEFT");
    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));
    m_levelText.setFont(m_game->assets().getFont("Gypsy"));
    m_levelText.setFillColor(sf::Color::White);
    m_tutorialText.setFont(m_game->assets().getFont("Gypsy"));
    m_tutorialText.setCharacterSize(20);
    m_tutorialText.setFillColor(sf::Color::White);
    m_walletText.setFont(m_game->assets().getFont("Gypsy"));
    m_walletText.setCharacterSize(36);
    m_walletText.setFillColor(sf::Color(137, 3, 6));

    m_lighting.setTexture(m_game->assets().getTexture("TexTransparent"));
    m_lighting.setTextureRect(sf::IntRect(1, 1, 1280, 762));
    m_lighting.setPosition(0, 0);

    m_game->playSound("MusicGame");
    m_game->setVolume("MusicGame", m_musicVolume);
    m_game->loopSound("MusicGame");

    std::cout << m_levelPath <<  '\n';

}

void Scene_MainGame::levelCompleted()
{
    if (m_saveFile == "NONE")
    {
        m_game->changeScene("Load_Level_Menu", std::make_shared<Scene_LoadGame_Menu>(m_game));
    }
    else
    {
        if (m_levelPath == "levels/level1.txt")
        {
            m_level1Completion = true;
        }
        else if (m_levelPath == "levels/level2.txt")
        {
            m_level1Completion = true;
        }
        else if (m_levelPath == "levels/level3.txt")
        {
            m_level1Completion = true;
        }
        saveGame();
        m_game->changeScene("Overworld", std::make_shared<Scene_Overworld>(m_game, m_saveFile));
    }
}

void Scene_MainGame::saveGame()
{
    std::ofstream save(m_saveFile);
    if (save.is_open())
    {
        save << "Level1    ";
        save << m_level1Completion;
        save << std::endl;
        save << "Level2    ";
        save << m_level2Completion;
        save << std::endl;
        save << "Level3    ";
        save << m_level3Completion;
        save << std::endl;
        save << "Coins     ";
        save << m_player->getComponent<CInventory>().money;
        save << std::endl;
        save << "MaxHealth ";
        save << m_player->getComponent<CHealth>().max;
        save << std::endl;
        save << "MaxArrows ";
        save << m_player->getComponent<CInventory>().maxArrows;
        save << std::endl;
        save << "Damage   ";
        save << m_player->getComponent<CDamage>().damage;
        save << std::endl;
    }
}

void Scene_MainGame::loadSaveGame()
{
    std::ifstream save(m_saveFile);
    if (save.is_open())
    {
        while (save.good())
        {

            save >> m_saveRead;
            if (m_saveRead == "Level1")
            {
                save >> m_level1Completion;
            }
            else if (m_saveRead == "Level2")
            {
                save >> m_level2Completion;
            }
            else if (m_saveRead == "Level3")
            {
                save >> m_level3Completion;
            }
            else if (m_saveRead == "Coins")
            {
                save >> m_coinCount;
            }
            else if (m_saveRead == "MaxHealth")
            {
                save >> m_maxHealth;
            }
            else if (m_saveRead == "Damage")
            {
                save >> m_damage;
            }
            else if (m_saveRead == "MaxArrows")
            {
                save >> m_arrowCount;
            }
        }
    }
}

void Scene_MainGame::loadOptions()
{
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
                if (m_difficulty == "Easy")
                {
                    m_difficultymod = 0.5;
                }
                else if (m_difficulty == "Hard")
                {
                    m_difficultymod = 2;
                }
                else
                {
                    m_difficultymod = 1;
                }
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
}

void Scene_MainGame::loadLevel(const std::string& filename)
{
    m_entityManager = EntityManager();

    std::ifstream config(filename);
    if (config.is_open())
    {
        while (config.good())
        {
            // Using the getPosition() function below to convert room-tile coords to game world coords
            //set variables equal to their values from the config file.
            config >> configRead;

            if (configRead == "Player")
            {
                config >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.HEALTH >> m_playerConfig.GRAVITY;
                continue;
            }   
            if (configRead == "Tile")
            {
                config >> m_tileConfig.Name >> m_tileConfig.RX >> m_tileConfig.RY >> m_tileConfig.TX >> m_tileConfig.TY >> m_tileConfig.BM >> m_tileConfig.BV;

                auto tile = m_entityManager.addEntity("tile");

                if (m_tileConfig.Name == "Ground" || m_tileConfig.Name == "FloatTileSmall" || m_tileConfig.Name == "FloatTileBig" || m_tileConfig.Name == "PlatformMiddle" || m_tileConfig.Name == "Ground2")
                {
                    tile->addComponent<CBoundingBox>(Vec2(m_game->assets().getAnimation(m_tileConfig.Name).getSize().x, m_game->assets().getAnimation(m_tileConfig.Name).getSize().y - 20), m_tileConfig.BM, m_tileConfig.BV);
                }
                else
                {
                    tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                }
                tile->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                tile->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                
                tile->addComponent<CDraggable>();
                continue;
            }
            if (configRead == "MovingTile")
            {
                config >> m_movingTileConfig.Name >> m_movingTileConfig.RX >> m_movingTileConfig.RY >> m_movingTileConfig.TX >> m_movingTileConfig.TY >> m_movingTileConfig.BM >> m_movingTileConfig.BV >> m_movingTileConfig.AI >> m_movingTileConfig.S;
                auto mTile = m_entityManager.addEntity("movingTile");
                mTile->addComponent<CDraggable>();
                if (m_movingTileConfig.AI == "Patrol")
                {
                    std::vector<Vec2> initialPatrolPos;
                    mTile->addComponent<CPatrol>(initialPatrolPos, m_movingTileConfig.S);
                    config >> m_movingTileConfig.N;
                    for (int i = 0; i < m_movingTileConfig.N; i++)
                    {
                        float x, y;
                        config >> x >> y;
                        mTile->getComponent<CPatrol>().positions.push_back(getPosition(m_movingTileConfig.RX, m_movingTileConfig.RY, x, y));
                    }
                }
                mTile->addComponent<CTransform>(getPosition(m_movingTileConfig.RX, m_movingTileConfig.RY, m_movingTileConfig.TX, m_movingTileConfig.TY));
                mTile->addComponent<CAnimation>(m_game->assets().getAnimation(m_movingTileConfig.Name), true);

                if (m_movingTileConfig.Name == "FloatTileSmall" || m_movingTileConfig.Name == "FloatTileBig")
                {
                    mTile->addComponent<CBoundingBox>(Vec2(m_game->assets().getAnimation(m_movingTileConfig.Name).getSize().x, m_game->assets().getAnimation(m_movingTileConfig.Name).getSize().y - 15), m_movingTileConfig.BM, m_movingTileConfig.BV);
                }
                else
                {
                    mTile->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_movingTileConfig.Name).getSize(), m_movingTileConfig.BM, m_movingTileConfig.BV);
                }
                continue;
            }
            if (configRead == "Item")
            {
                config >> m_itemConfig.Name >> m_itemConfig.RX >> m_itemConfig.RY >> m_itemConfig.TX >> m_itemConfig.TY >> m_itemConfig.BM >> m_itemConfig.BV;
                if (m_itemConfig.Name == "Coin")
                {
                    auto coin = m_entityManager.addEntity("coins");
                    coin->addComponent<CDraggable>();
                    coin->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    coin->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    coin->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
                else if (m_itemConfig.Name == "Heart")
                {
                    auto heart = m_entityManager.addEntity("hearts");
                    heart->addComponent<CDraggable>();
                    heart->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    heart->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    heart->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
                else if (m_itemConfig.Name == "BluePotion" || m_itemConfig.Name == "PurplePotion" || m_itemConfig.Name == "GreenPotion" || m_itemConfig.Name == "GoldPotion" || m_itemConfig.Name == "RedPotion")
                {
                    auto potion = m_entityManager.addEntity("potions");
                    potion->addComponent<CDraggable>();
                    potion->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    potion->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    potion->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
                else if (m_itemConfig.Name == "Chest")
                {
                    auto interact = m_entityManager.addEntity("interactable");
                    interact->addComponent<CDraggable>();
                    interact->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    interact->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    interact->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
                else if (m_itemConfig.Name == "JarBig" || m_itemConfig.Name == "JarSmall" || m_itemConfig.Name == "Barrel")
                {
                    auto breakable = m_entityManager.addEntity("breakable");
                    breakable->addComponent<CDraggable>();
                    breakable->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    breakable->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    breakable->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
            }
            if (configRead == "NPC")
            {
                config >> m_npcConfig.Name >> m_npcConfig.RX >> m_npcConfig.RY >> m_npcConfig.TX >> m_npcConfig.TY >> m_npcConfig.BM >> m_npcConfig.BV >> m_npcConfig.H >> m_npcConfig.D >> m_npcConfig.AI >> m_npcConfig.S;
                auto npc = m_entityManager.addEntity("npc");
                npc->addComponent<CDraggable>();
                if (m_npcConfig.AI == "Follow")
                {

                    npc->addComponent<CTransform>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY));
                    if (m_npcConfig.Name == "GhostShriek")
                    {
                        npc->addComponent<CState>("GhostShriek");
                        npc->addComponent<CAnimation>(m_game->assets().getAnimation("GhostShriek"), true);
                    }
                    if (m_npcConfig.Name == "DemonIdle")
                    {
                        npc->addComponent<CState>("DemonIdle");
                        npc->addComponent<CAnimation>(m_game->assets().getAnimation("DemonIdle"), true);
                        npc->addComponent<CBoundingBox>(Vec2(m_game->assets().getAnimation(m_npcConfig.Name).getSize().x, m_game->assets().getAnimation(m_npcConfig.Name).getSize().y), m_npcConfig.BM, m_npcConfig.BV);
                    }
                    else if (m_npcConfig.Name == "WizardIdle")
                    {
                        npc->addComponent<CState>("WizardIdle");
                        npc->addComponent<CAnimation>(m_game->assets().getAnimation("WizardIdle"), true);
                        npc->addComponent<CBoundingBox>(Vec2(m_game->assets().getAnimation(m_npcConfig.Name).getSize().x, m_game->assets().getAnimation(m_npcConfig.Name).getSize().y), m_npcConfig.BM, m_npcConfig.BV);
                    }
                    else
                    {
                        npc->addComponent<CAnimation>(m_game->assets().getAnimation(m_npcConfig.Name), true);
                        npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_npcConfig.Name).getSize(), m_npcConfig.BM, m_npcConfig.BV);
                    }
                    //npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_npcConfig.Name).getSize(), m_npcConfig.BM, m_npcConfig.BV);
                    npc->addComponent<CFollowPlayer>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY), m_npcConfig.S);
                    npc->addComponent<CHealth>(m_npcConfig.H * m_difficultymod, m_npcConfig.H * m_difficultymod);
                    npc->addComponent<CDamage>(ceil(m_npcConfig.D * m_difficultymod));

                    npc->addComponent<CShader>();
                    continue;
                }
                else if (m_npcConfig.AI == "Patrol")
                {
                    std::vector<Vec2> initialPatrolPos;
                    npc->addComponent<CPatrol>(initialPatrolPos, m_npcConfig.S);
                    config >> m_npcConfig.N;
                    for (int i = 0; i < m_npcConfig.N; i++)
                    {
                        float x, y;
                        config >> x >> y;
                        npc->getComponent<CPatrol>().positions.push_back(getPosition(m_npcConfig.RX, m_npcConfig.RY, x, y));
                    }
                    npc->addComponent<CTransform>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY));
                    npc->addComponent<CAnimation>(m_game->assets().getAnimation(m_npcConfig.Name), true);
                    npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_npcConfig.Name).getSize(), m_npcConfig.BM, m_npcConfig.BV);
                    npc->addComponent<CHealth>(m_npcConfig.H* m_difficultymod, m_npcConfig.H* m_difficultymod);
                    npc->addComponent<CDamage>(ceil(m_npcConfig.D * m_difficultymod));
                    continue;
                }
            }
        }
    }           
    spawnPlayer();
    drawWeaponHolder();
}

void Scene_MainGame::loadParallaxBackground()
{
    for (auto i = 0; i < m_parallaxBackgroundTextures.size(); i++)
    {
        
        m_parallaxBackgroundSprites[i].setTexture(m_game->assets().getTexture(m_parallaxBackgroundTextures[i]),false);
        m_parallaxBackgroundSprites[i].setTextureRect(sf::IntRect(1, 1, 1000000, 1000000));
        m_parallaxBackgroundSprites[i].setPosition(-6500, -256);
    }
}

void Scene_MainGame::drawParallaxBackground()
{
    m_game->window().draw(m_parallaxBackgroundSprites[9]);// Sky;

    m_game->window().draw(m_parallaxBackgroundSprites[8]);// Forest
    m_game->window().draw(m_parallaxBackgroundSprites[7]);// Forest
    m_game->window().draw(m_parallaxBackgroundSprites[6]);// Forest


    m_game->window().draw(m_parallaxBackgroundSprites[5]);// Forest
    m_parallaxBackgroundSprites[4].move(sf::Vector2f(-0.4f, 0.f));
    m_game->window().draw(m_parallaxBackgroundSprites[4]);// Particles
    m_game->window().draw(m_parallaxBackgroundSprites[3]);// Forest


    m_game->window().draw(m_parallaxBackgroundSprites[1]);// Bushes

    //m_parallaxBackgroundSprites[0].move(sf::Vector2f(-0.2f, 0.f));
    //m_game->window().draw(m_parallaxBackgroundSprites[0]);// Mist

    
}

Vec2 Scene_MainGame::getPosition(int rx, int ry, int tx, int ty) const
{
    float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
    float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
    return Vec2(x, y);
}

void Scene_MainGame::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CState>("StandRight");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    if (m_saveFile == "NONE")
    {
        m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
        m_player->addComponent<CDamage>(1);
        m_player->addComponent<CInventory>();
        
    }
    else
    {
        m_player->addComponent<CHealth>(m_maxHealth, m_maxHealth);
        m_player->addComponent<CDamage>(m_damage);
        m_player->addComponent<CInventory>(m_coinCount, m_arrowCount);
    }
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CDraggable>();
    m_player->addComponent<CShader>();
}

void Scene_MainGame::startAttack(std::shared_ptr<Entity> entity)
{
    m_frameSinceAttack = 0;
    // check which weapon is selected (dagger, bow, axe) and play the proper animation.
    auto& eState = entity->getComponent<CState>();
    
    // use Dagger Animation
    if (m_weaponSwitch == 0)
    {
         eState.state = "Dagger";
    }

    // use Bow Animation
    else if(m_weaponSwitch == 1)
    {
        eState.state = "Bow";
    }
    else
    {
        eState.state = "Axe";
    }
}
                           
void Scene_MainGame::update()
{
    m_entityManager.update();
    if (!m_player->isActive())
    {
        m_game->stopSound("MusicGame");
        m_game->changeScene("GameOver", std::make_shared<Scene_GameOver>(m_game, m_levelPath, m_saveFile));
    }
    // When the game is paused
    if (m_paused)
    {
        m_levelText.setString("Paused"); 
        m_levelText.setCharacterSize(24);
        /*m_walletText.setString("\n  Coins: x" + std::to_string(m_wallet));*/
        m_game->pauseSound("MusicLevel");
        sCamera();
        sHUD();
        
        return;
    } 
    if (m_player->getComponent<CInput>().attack)
    {
        m_frameSinceAttack++;
    }
    sAI();
    sTilesAI();
    sMovement();
    sDragAndDrop();
    sArrowMovement();
    sStatus();
    sCollision();
    sAnimation();
    sCamera();
    sHUD();
    m_currentFrame++;
}

void Scene_MainGame::sMovement()
{

    auto& pInput = m_player->getComponent<CInput>();
    auto& pTransform = m_player->getComponent<CTransform>();
    auto& pState= m_player->getComponent<CState>();

#pragma region Running State
    // if velocity in x direction was 0 last frame check for movement in y direction else check for movement in x direction

   
        if (pInput.up)
        {
            pTransform.velocity.y = -7 * (m_playerConfig.SPEED) - 3 * pTransform.tempSpeed;
            pTransform.facing = Vec2(0, -1);
            pState.state = "Jump";
            pTransform.scale = Vec2(1, 1);
        }

        else
        {
            pTransform.velocity.y = 0;
        }
        
        if (pInput.right && pInput.up)
        {
            pState.state = "Jump";
        }
        m_player->removeComponent<CBoundingBox>();
        m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
        // if only one x directional key is pressed move in that direction otherwise stop.
        if (pInput.left && !pInput.right)
        {
            pTransform.velocity.x = -1 * (m_playerConfig.SPEED + pTransform.tempSpeed);
            pTransform.facing = Vec2(-1, 0);
            pState.state = "RunRight";
            if (pInput.up == true)
            {
                pState.state = "Jump";
            }
            
            if (pInput.down == true)
            {
                pState.state = "StandDown";
                m_player->removeComponent<CBoundingBox>();
                m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, 20), true, false);
            }
            pTransform.scale = Vec2(-1, 1);
            
            if (!m_collidingWithTile)
            {
                m_parallaxBackgroundSprites[8].move(sf::Vector2f(0.5f, 0.f));
                m_parallaxBackgroundSprites[7].move(sf::Vector2f(0.6f, 0.f));
                m_parallaxBackgroundSprites[6].move(sf::Vector2f(0.7f, 0.f));
                ///
                m_parallaxBackgroundSprites[5].move(sf::Vector2f(1.f, 0.f));
                m_parallaxBackgroundSprites[4].move(sf::Vector2f(1.2f, 0.f));
                m_parallaxBackgroundSprites[3].move(sf::Vector2f(1.3f, 0.f));
                ///
                m_parallaxBackgroundSprites[2].move(sf::Vector2f(1.8f, 0.f));
                m_parallaxBackgroundSprites[1].move(sf::Vector2f(1.9f, 0.f));
                //m_parallaxBackgroundSprites[0].move(sf::Vector2f(3.0f, 0.f));
            }
           
        }
        else if (!pInput.left && pInput.right)
        {
            pTransform.velocity.x = (m_playerConfig.SPEED + pTransform.tempSpeed);
            pTransform.facing = Vec2(1, 0);
            pState.state = "RunRight";
            if (pInput.up == true)
            {
                pState.state = "Jump";
            }
            m_player->removeComponent<CBoundingBox>();
            m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
            if (pInput.down == true)
            {
                pState.state = "StandDown";
                //m_player->getComponent<CBoundingBox>().size.y = 32;
                //m_player->getComponent<CTransform>().pos.y -= m_player->getComponent<CBoundingBox>().halfSize.y - m_playerConfig.CY;
                //pTransform.scale = Vec2(-1, 1);
                m_player->removeComponent<CBoundingBox>();
                m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX,20),true, false);
            }

            pTransform.scale = Vec2(1, 1);
            if (!m_collidingWithTile)
            {
                m_parallaxBackgroundSprites[8].move(sf::Vector2f(-0.5f, 0.f));
                m_parallaxBackgroundSprites[7].move(sf::Vector2f(-0.6f, 0.f));
                m_parallaxBackgroundSprites[6].move(sf::Vector2f(-0.7f, 0.f));
                ///
                m_parallaxBackgroundSprites[5].move(sf::Vector2f(-1.f, 0.f));
                m_parallaxBackgroundSprites[4].move(sf::Vector2f(-1.2f, 0.f));
                m_parallaxBackgroundSprites[3].move(sf::Vector2f(-1.3f, 0.f));
                ///
                m_parallaxBackgroundSprites[2].move(sf::Vector2f(-1.8f, 0.f));
                m_parallaxBackgroundSprites[1].move(sf::Vector2f(-1.9f, 0.f));
                //m_parallaxBackgroundSprites[0].move(sf::Vector2f(-3.0f, 0.f));
            }
            
        }
        else if (!pInput.left && pInput.right)
        {
            pTransform.velocity.x = 0;
        }
        else
        {
            pTransform.velocity.x = 0;
        }
    
  
#pragma endregion

#pragma region Attacking State
    // if the player is attacking give it an attacking state.
    if (pInput.attack)
    {
        if (m_weaponSwitch == 0)
        {
            pState.state = "Dagger";
        }
        else if (m_weaponSwitch == 1)
        {
            pState.state = "Bow";
        }
        else 
        {
            pState.state = "Axe";
        }
    }
#pragma endregion

#pragma region Standing State
    // if the player is not attacking or has no velocity give it a standing state.
    else if (pTransform.velocity.x == 0 && pTransform.velocity.y == 0)
    {
        if (pTransform.facing == Vec2(1, 0))
        {
            pState.state = "StandRight";
            pTransform.scale = Vec2(1, 1);
        }
        else if (pTransform.facing == Vec2(-1, 0))
        {
            pState.state = "StandRight";
            pTransform.scale = Vec2(-1, 1);
        }
        else if (pTransform.facing == Vec2(0, -1))
        {
            pState.state = "StandUp";
            pTransform.scale = Vec2(1, 1);
        }
        else
        {
            pState.state = "StandDown";
            pTransform.scale = Vec2(1, 1);
        }
    }
#pragma endregion


    if (!m_playerOnGround)
    {
        m_FrameSinceGrounded++;
        pTransform.velocity.y += m_FrameSinceGrounded/2 * m_player->getComponent<CGravity>().gravity;
        
    }
    else
    {
        m_FrameSinceGrounded = 0;
    }

    
    if (m_player->getComponent<CTransform>().pos.y > getPosition(0, 0, 0, 12).y)
    {
        m_player->destroy();
    }
    if (pTransform.velocity.y > m_MaxYSpeed)
    {
        pTransform.velocity.y = m_MaxYSpeed;
    }
    else if(pTransform.velocity.y < -1 * m_MaxYSpeed)
    {
        pTransform.velocity.y = -1 * m_MaxYSpeed;
    }

    pTransform.pos += pTransform.velocity;
    

}

void Scene_MainGame::sArrowMovement()
{
    //preforms arrow movement
    for (auto& arrow : m_entityManager.getEntities("arrow"))
    {
        auto& arrowTransform = arrow->getComponent<CTransform>();
        arrowTransform.pos += arrowTransform.velocity;
    }
}

void Scene_MainGame::sDoAction(const Action& action)
{                      
    if (action.type() == "START")
    {
        if (action.name() == "PAUSE") {  setPaused(!m_paused);
        }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW") { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision;}
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "UP") 
        {
            std::cout << m_player->getComponent<CInput>().canJump << ":" << m_playerOnGround << '\n';
            if (m_player->getComponent<CInput>().canJump || !m_playerOnGround) 
            {
                if (m_player->getComponent<CInput>().canJump)
                {
                    m_game->playSound("Jump");
                    m_game->setVolume("Jump", m_effectVolume);
                }
                m_player->getComponent<CInput>().up = true;
                m_player->getComponent<CInput>().canJump = false;
                m_playerOnGround = false; 

            } 
            else{ m_player->getComponent<CInput>().up = false; }
        }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true;}
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
        else if (action.name() == "LEFT_CLICK") { grab(); }
        else if (action.name() == "USE_ITEM") { sUseItem(m_player); }
        else if (action.name() == "INTERACT") { sInteract(); }
        else if (action.name() == "ATTACK") { if (!m_player->getComponent<CInput>().attack && !m_paused) { startAttack(m_player);  m_player->getComponent<CInput>().attack = true; } }
        else if (action.name() == "WEAPON_SWITCH") { if (!m_player->getComponent<CInput>().attack) {  m_weaponTextClock.restart(); if (m_weaponSwitch < 2) { m_weaponSwitch++; } else { m_weaponSwitch = 0; } drawWeaponHolder();
        } }
        else if (action.name() == "SELECT_RIGHT") { select("Right"); }
        else if (action.name() == "SELECT_LEFT") { select("Left"); }
        else if (action.name() == "Zoom Map") 
        {
            sf::View view = m_game->window().getView();
            view.zoom(0.5f);
            m_game->window().setView(view);
        }
        else if (action.name() == "MiniMap") { m_minimap = !m_minimap; }
        else if (action.name() == "OPEN_INVENTORY") { std::cout << "Inventory"; inventoryOpened = true; drawInventory(); }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; m_player->getComponent<CInput>().canJump = true;}
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
    }

    if (action.name() == "MOUSE_MOVE") { m_mPos = action.pos() + Vec2(m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2, m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2); }
}

void Scene_MainGame::sUseItem(std::shared_ptr<Entity> entity)
{
    if (entity->hasComponent<CInventory>())
    {
        auto& inventory = entity->getComponent<CInventory>();
        
        if (inventory.items.empty())
        {
            return;
        }
        m_game->playSound("UseItem");
        m_game->setVolume("UseItem", m_effectVolume);
        if (inventory.items[m_select] == "RedPotion")
        {
            std::cout << "Red Potion";
            inventory.items.erase(inventory.items.begin() + m_select);
            auto& health = entity->getComponent<CHealth>();
            health.current = health.max;
        }
        else if (inventory.items[m_select] == "BluePotion")
        {
            inventory.items.erase(inventory.items.begin() + m_select);
            auto& transform = entity->getComponent<CTransform>();
            transform.tempSpeed = 3;
            transform.duration = 600;
        }
        else if (inventory.items[m_select] == "GreenPotion")
        {
            inventory.items.erase(inventory.items.begin() + m_select);
            auto& damage = entity->getComponent<CDamage>();
            damage.tempDamage = 2;
            damage.duration = 600;
        }
        else if (inventory.items[m_select] == "GoldPotion")
        {
            inventory.items.erase(inventory.items.begin() + m_select);
            
            if (entity->hasComponent<CInvincibility>())
            {
                entity->getComponent<CInvincibility>().iframes = 420;
            }
            else
            {
                entity->addComponent<CInvincibility>(420);
            }
            if (entity->hasComponent<CShader>())
            {
                entity->getComponent<CShader>().duration = 420;
                entity->getComponent<CShader>().ourShader = m_shaders[1];
            }
        }
        else if (inventory.items[m_select] == "PurplePotion")
        {
            inventory.items.erase(inventory.items.begin() + m_select);
            auto& eTransform = entity->getComponent<CTransform>();
            auto& potion = m_entityManager.addEntity("arrow");
            potion->addComponent<CAnimation>(m_game->assets().getAnimation("PurplePotion"), true);
            potion->addComponent<CBoundingBox>(m_game->assets().getAnimation("PurplePotion").getSize());
            potion->addComponent<CTransform>(Vec2(eTransform.pos.x + eTransform.scale.x * 5, eTransform.pos.y), Vec2(8 * eTransform.scale.x, 0), eTransform.scale, 0);
            potion->addComponent<CDamage>(10);
            potion->addComponent<CLifeSpan>(180, m_currentFrame);
        }

    }
    select("None");
    drawInventory();
}

void Scene_MainGame::sInteract()
{
    for (auto interactable : m_entityManager.getEntities("interactable"))
    {
        auto overlap = Physics::GetOverlap(interactable, m_player);
        if (overlap.x > 2 && overlap.y > 2)
        {
            if (interactable->getComponent<CAnimation>().animation.getName() == "Chest")
            {
                std::srand(time(0));
                int coinAmount = std::rand() % 17 + 3;
                m_player->getComponent<CInventory>().money += coinAmount;
                int itemID = std::rand() % 6;
                if (itemID == 0)
                {
                    m_player->getComponent<CInventory>().items.push_back("RedPotion");
                }
                if (itemID == 1)
                {
                    m_player->getComponent<CInventory>().items.push_back("BluePotion");
                }
                if (itemID == 2)
                {
                    m_player->getComponent<CInventory>().items.push_back("GoldPotion");
                }
                if (itemID == 3)
                {
                    m_player->getComponent<CInventory>().items.push_back("GreenPotion");
                }
                if (itemID == 4)
                {
                    m_player->getComponent<CInventory>().items.push_back("PurplePotion");
                }
                interactable->destroy();
            }
        }
    }
}

void Scene_MainGame::select(std::string direction)
{
    m_InventoryClock.restart();
    if (m_player->getComponent<CInventory>().items.size() == 0)
    {
        m_select = 0;
    }
    else if (direction == "Right")
    {
        if (m_player->getComponent<CInventory>().items.size() - 1 <= m_select)
        {
            m_select = 0;
        }
        else
        {
            m_select++;
        }
    }
    else if (direction == "Left")
    {
        if (m_select == 0)
        {
            m_select = m_player->getComponent<CInventory>().items.size() - 1;
        }
        else
        {
            m_select--;
        }
    }
    else if (direction == "None")
    {
        if (m_select > m_player->getComponent<CInventory>().items.size() - 1)
        {
            m_select = m_player->getComponent<CInventory>().items.size() - 1;
        }
    }
}

void Scene_MainGame::sAI()
{
    srand(time(0));
    for (auto e : m_entityManager.getEntities("npc"))
    {
        if (e->hasComponent<CFollowPlayer>())
        {
           bool Visionblocked = false;

            // Checking for Entity intersection to check if there is a clear line of vision between the enemy and the player 
            for (auto visionBlocker : m_entityManager.getEntities())
            {
                if (visionBlocker->hasComponent<CBoundingBox>())
                {
                    if (visionBlocker->getComponent<CBoundingBox>().blockVision)
                    {
                        bool intersecting = Physics::EntityIntersect(m_player->getComponent<CTransform>().pos, e->getComponent<CTransform>().pos, visionBlocker);
                        if (intersecting)
                        {
                            Visionblocked = true;
                            break;
                        }
                    }
                }
            }
            if (e->getComponent<CAnimation>().animation.getName() == "DemonAttack")
            {
                if (e->getComponent<CAnimation>().animation.hasEnded())
                {
                    e->getComponent<CState>().state = "DemonIdle";
                }
            }
            if (!Visionblocked)
            {
                
                // Performing Steering
                Vec2 target = m_player->getComponent<CTransform>().pos;
                Vec2 desired = target - e->getComponent<CTransform>().pos;
                float length = desired.dist(Vec2(0, 0));
                if (int(length) > 1)
                {
                    desired = desired / length;
                    // when npc is following the player to the right
                    if (desired.x > 0)
                    {
                        e->getComponent<CTransform>().scale.x = -1;
                    }

                    // when npc is following the player to the left
                    else if (desired.x < 0)
                    {
                        e->getComponent<CTransform>().scale.x = 1;
                    }
                    if (e->getComponent<CAnimation>().animation.getName() == "DemonAttack")
                    {
                        if (e->getComponent<CAnimation>().animation.hasEnded())
                        {
                            e->getComponent<CState>().state = "DemonIdle";
                        }
                    }
                    else if (e->getComponent<CAnimation>().animation.getName() == "WizardAttack1" || e->getComponent<CAnimation>().animation.getName() == "WizardAttack2")
                    {
                        if (e->getComponent<CAnimation>().animation.hasEnded())
                        {
                            e->getComponent<CState>().state = "WizardIdle";
                        }
                    }
                    else
                    {
                        e->getComponent<CTransform>().pos.x += e->getComponent<CFollowPlayer>().speed * desired.x;
                        e->getComponent<CTransform>().pos.y += e->getComponent<CFollowPlayer>().speed * desired.y;
                    }
                    if (e->getComponent<CAnimation>().animation.getName() == "WizardRun" || e->getComponent<CAnimation>().animation.getName() == "WizardFall" || e->getComponent<CAnimation>().animation.getName() == "WizardIdle" || e->getComponent<CAnimation>().animation.getName() == "WizardJump")
                    {
                        if (desired.x > 0)
                        {
                            if (e->getComponent<CAnimation>().animation.hasEnded() || e->getComponent<CAnimation>().animation.getName() == "WizardIdle")
                            {
                                e->getComponent<CState>().state = "WizardRun";
                            }
                        }
                        else if (desired.x < 0)
                        {
                            if (e->getComponent<CAnimation>().animation.hasEnded() || e->getComponent<CAnimation>().animation.getName() == "WizardIdle")
                            {
                                e->getComponent<CState>().state = "WizardRun";
                            }
                        }
                        else
                        {
                            if (e->getComponent<CAnimation>().animation.hasEnded() || e->getComponent<CAnimation>().animation.getName() == "WizardIdle")
                            {
                                e->getComponent<CState>().state = "WizardIdle";
                            }
                        }
                    }
                
                }
                if (e->hasComponent<CState>())
                {
                    auto& eState = e->getComponent<CState>();
                    if (eState.state == "WizardRun" || eState.state == "WizardTakeHit" || eState.state == "WizardIdle")
                    {
                        int attacks = rand() % 100;
                        if (attacks < 10)
                        {
                            if (length > 80)
                            {
                                eState.state = "WizardAttack2";
                                auto& enemyProjectile = m_entityManager.addEntity("enemyAttack");
                                enemyProjectile->addComponent<CAnimation>(m_game->assets().getAnimation("WizardProjectile"), true);
                                enemyProjectile->addComponent<CTransform>(e->getComponent<CTransform>().pos);
                                enemyProjectile->addComponent<CBoundingBox>(m_game->assets().getAnimation("WizardProjectile").getSize());
                                enemyProjectile->addComponent<CDamage>(e->getComponent<CDamage>().damage + e->getComponent<CDamage>().tempDamage);
                                enemyProjectile->addComponent<CLifeSpan>(240, currentFrame());
                            }
                            else 
                            {
                                eState.state = "WizardAttack2";
                                auto& enemyProjectile = m_entityManager.addEntity("enemyAttack");
                                enemyProjectile->addComponent<CTransform>(e->getComponent<CTransform>().pos);
                                enemyProjectile->addComponent<CBoundingBox>(m_game->assets().getAnimation("WizardAttack1").getSize());
                                enemyProjectile->addComponent<CDamage>(2*(e->getComponent<CDamage>().damage + e->getComponent<CDamage>().tempDamage));
                                enemyProjectile->addComponent<CLifeSpan>(5, currentFrame());
                            }
                        }
                    }
                }
              
            }
            else if (e->getComponent<CTransform>().pos != e->getComponent<CFollowPlayer>().home && Visionblocked)
            {
                Vec2 target = e->getComponent<CFollowPlayer>().home;
                Vec2 desired = target - e->getComponent<CTransform>().pos;
                float length = desired.dist(Vec2(0, 0));
                if (int(length) > 1)
                {
                    desired = desired / length;
                    e->getComponent<CTransform>().pos.x += e->getComponent<CFollowPlayer>().speed * desired.x;
                    e->getComponent<CTransform>().pos.y += e->getComponent<CFollowPlayer>().speed * desired.y;
                }
            }
        }
        // Implementing Patrol behavior
        if (e->hasComponent<CPatrol>())
        {
            Vec2 target = e->getComponent<CPatrol>().positions[e->getComponent<CPatrol>().currentPosition];
            int nextLoc = e->getComponent<CPatrol>().currentPosition + 1;
            Vec2 desired = target - e->getComponent<CTransform>().pos;
            float length = desired.dist(Vec2(0, 0));
            int numofWaypoints = e->getComponent<CPatrol>().positions.size();
            
            if (int(length) > 1)
            {
                desired = desired / length;
                e->getComponent<CTransform>().pos.x += e->getComponent<CPatrol>().speed * desired.x;
                e->getComponent<CTransform>().pos.y += e->getComponent<CPatrol>().speed * desired.y;
                if (desired.x != 0)
                {
                    if (desired.x < 0)
                    {
                        e->getComponent<CTransform>().scale.x = -1;
                    }
                    else
                    {
                        e->getComponent<CTransform>().scale.x = 1;
                    }
                }
                else
                {/*
                    if (desired.y < 0)
                    {
                        e->getComponent<CTransform>().scale.y = 1;
                    }
                    else
                    {
                        e->getComponent<CTransform>().scale.y = -1;
                    }*/
                }
                
            }
            else
            {
                e->getComponent<CPatrol>().currentPosition++;
                if (e->getComponent<CPatrol>().currentPosition == numofWaypoints)
                {
                    e->getComponent<CPatrol>().currentPosition = 0;
                }
            }
        }
    }
}

void Scene_MainGame::sTilesAI()
{
    for (auto e : m_entityManager.getEntities("movingTile"))
    {
        // Implementing Patrol behavior
        if (e->hasComponent<CPatrol>())
        {
            Vec2 target = e->getComponent<CPatrol>().positions[e->getComponent<CPatrol>().currentPosition];
            int nextLoc = e->getComponent<CPatrol>().currentPosition + 1;
            Vec2 desired = target - e->getComponent<CTransform>().pos;
            float length = desired.dist(Vec2(0, 0));
            int numofWaypoints = e->getComponent<CPatrol>().positions.size();

            if (int(length) > 1)
            {
                desired = desired / length;
                e->getComponent<CTransform>().pos.x += e->getComponent<CPatrol>().speed * desired.x;
                e->getComponent<CTransform>().pos.y += e->getComponent<CPatrol>().speed * desired.y;
               

            }
            else
            {
                e->getComponent<CPatrol>().currentPosition++;
                if (e->getComponent<CPatrol>().currentPosition == numofWaypoints)
                {
                    e->getComponent<CPatrol>().currentPosition = 0;
                }
            }
        }
    }
}
void Scene_MainGame::sStatus()
{
    // Setting up LifeSpan functionality
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CLifeSpan>())
        {
            auto& eLifeSpan = e->getComponent<CLifeSpan>();
            if (currentFrame() >= eLifeSpan.frameCreated + eLifeSpan.lifespan)
            {
                e->destroy();

                // checking if the destroyed animation was of a weapon
                if (e->getComponent<CAnimation>().animation.getName() == "Dagger" || e->getComponent<CAnimation>().animation.getName() == "Axe" || e->getComponent<CAnimation>().animation.getName() == "Bow")
                {
                    m_player->getComponent<CInput>().attack = false;
                }
            }
        }
    }

    // Invincibility frames
    if (m_player->hasComponent<CInvincibility>())
    {
        if (m_player->getComponent<CInvincibility>().iframes > 0)
        {
            m_player->getComponent<CInvincibility>().iframes--;
        }
        else if (m_player->getComponent<CInvincibility>().iframes == 0)
        {
            m_player->removeComponent<CInvincibility>();
        }
    }
}
                           
void Scene_MainGame::sCollision()
{
    sTileCollision();
    sPlayerCollision();
    sMeleeCollision();
    sArrowCollision();
    sBreakableCollision();
    sHeartCollision();
    sCoinCollision();
    sItemCollision();
    sTeleportCollision();
    sEnemyCollision();
}

void Scene_MainGame::sTileCollision()
{
    // Tile collisions with entities are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    auto& playerBoundingBox = m_player->getComponent<CBoundingBox>();
    m_playerOnGround = false;
    m_collidingWithTile = false;
    m_playerOnMovingTile = false;
    for (auto tile : m_entityManager.getEntities("tile"))
    {
        auto& tileBoundingBox = tile->getComponent<CBoundingBox>();
        auto& tileAnimation = tile->getComponent<CAnimation>().animation;
        if (tileBoundingBox.blockMove || tileAnimation.getName() == "Goal")
        {
            auto playerTileOverlap = Physics::GetOverlap(tile, m_player);
            
            auto& tileTransform = tile->getComponent<CTransform>();
            if (playerTileOverlap.x > 0 && playerTileOverlap.y > 0)
            {
                if (tileAnimation.getName() == "Goal")
                {
                    levelCompleted();
                }
                if (playerTileOverlap.x > playerTileOverlap.y)
                {
                    if (playerTransform.pos.y > tileTransform.pos.y)
                    {
                        playerTransform.pos.y += playerTileOverlap.y;
                    }
                    else
                    {
                        playerTransform.pos.y -= playerTileOverlap.y;
                        m_playerOnGround = true;
                    }
                }
                else if (playerTransform.prevPos.y < tileTransform.pos.y && playerTileOverlap.x > playerTileOverlap.y-2)
                {
                    playerTransform.pos.y = tileTransform.pos.y - tileBoundingBox.halfSize.y - playerBoundingBox.halfSize.y;
                    //m_playerOnGround = true;
                }
                else
                {
                    
                    if (playerTransform.pos.x > tileTransform.pos.x)
                    {
                        playerTransform.pos.x += playerTileOverlap.x;
                        m_collidingWithTile = true;
                    }
                    else
                    {
                        playerTransform.pos.x -= playerTileOverlap.x;
                        m_collidingWithTile = true;
                    }
                }
            }
            for (auto npc : m_entityManager.getEntities("npc"))
            {
                bool grounded = false;
                auto& npcTransform = npc->getComponent<CTransform>();
                auto npcTileOverlap = Physics::GetOverlap(tile, npc);
                if (npcTileOverlap.x > 0 && npcTileOverlap.y > 0)
                {
                    if (npcTileOverlap.x > npcTileOverlap.y)
                    {
                        if (npcTransform.pos.y > tileTransform.pos.y)
                        {
                            bool grounded = true;
                            if (npc->getComponent<CAnimation>().animation.getName() == "WizardRun" || npc->getComponent<CAnimation>().animation.getName() == "WizardFall" || npc->getComponent<CAnimation>().animation.getName() == "WizardIdle" || npc->getComponent<CAnimation>().animation.getName() == "WizardJump" || npc->getComponent<CAnimation>().animation.getName() == "WizardAttack1" || npc->getComponent<CAnimation>().animation.getName() == "WizardAttack2")
                            {
                                npc->getComponent<CTransform>().velocity.y = 0;
                            }
                            npcTransform.pos.y += npcTileOverlap.y;
                        }
                        else
                        {
                            npcTransform.pos.y -= npcTileOverlap.y;
                        }              
                    }
                    else
                    {
                        if (npcTransform.pos.x > tileTransform.pos.x)
                        {
                            npcTransform.pos.x += npcTileOverlap.x;
                        }
                        else
                        {
                            npcTransform.pos.x -= npcTileOverlap.x; 
                        }
                    }

                }
            }
        }
        else
        {
            continue;
        }
    }

    for (auto tile : m_entityManager.getEntities("movingTile"))
    {
        auto& tileBoundingBox = tile->getComponent<CBoundingBox>();
        if (tileBoundingBox.blockMove)
        {
            auto playerTileOverlap = Physics::GetOverlap(tile, m_player);

            auto& tileTransform = tile->getComponent<CTransform>();
            if (playerTileOverlap.x > 0 && playerTileOverlap.y > 0)
            {
                if (playerTileOverlap.x > playerTileOverlap.y)
                {
                    if (playerTransform.pos.y > tileTransform.pos.y)
                    {
                        playerTransform.pos.y += playerTileOverlap.y;
                    }
                    else
                    {
                        playerTransform.pos.y -= playerTileOverlap.y;
                        m_playerOnMovingTile = true;
                        
                    }
                }
                else if (playerTransform.prevPos.y < tileTransform.pos.y && playerTileOverlap.x > playerTileOverlap.y - 2)
                {
                    playerTransform.pos.y = tileTransform.pos.y - tileBoundingBox.halfSize.y - playerBoundingBox.halfSize.y;
                    //m_playerOnGround = true;
                }
                else
                {

                    if (playerTransform.pos.x > tileTransform.pos.x)
                    {
                        playerTransform.pos.x += playerTileOverlap.x;
                        m_collidingWithTile = true;
                    }
                    else
                    {
                        playerTransform.pos.x -= playerTileOverlap.x;
                        m_collidingWithTile = true;
                    }
                }
            }
            for (auto npc : m_entityManager.getEntities("npc"))
            {
                auto& npcTransform = npc->getComponent<CTransform>();
                auto npcTileOverlap = Physics::GetOverlap(tile, npc);
                if (npcTileOverlap.x > 0 && npcTileOverlap.y > 0)
                {
                    if (npcTileOverlap.x > npcTileOverlap.y)
                    {
                        if (npcTransform.pos.y > tileTransform.pos.y)
                        {
                            npcTransform.pos.y += npcTileOverlap.y;
                        }
                        else
                        {
                            npcTransform.pos.y -= npcTileOverlap.y;
                        }
                    }
                    else
                    {
                        if (npcTransform.pos.x > tileTransform.pos.x)
                        {
                            npcTransform.pos.x += npcTileOverlap.x;
                        }
                        else
                        {
                            npcTransform.pos.x -= npcTileOverlap.x;
                        }
                    }

                }
            }
        }
        else
        {
            continue;
        }
    }
}
void Scene_MainGame::sPlayerCollision()
{
    // Player collisions with NPC's are implemented here
    auto& playerHealth = m_player->getComponent<CHealth>();
    for (auto npc : m_entityManager.getEntities("npc"))
    {
        auto& npcDamage = npc->getComponent<CDamage>();
        auto npcPlayerOverlap = Physics::GetOverlap(m_player, npc);
        if (npcPlayerOverlap.x > 0 && npcPlayerOverlap.y > 0)
        {
            //m_game->playSound("LinkHurt");
            if (!m_player->hasComponent<CInvincibility>())
            {
                auto& pShader = m_player->getComponent<CShader>();
                pShader.duration = 60;
                pShader.ourShader = m_shaders[0];
                m_player->addComponent<CInvincibility>(60);
                playerHealth.current -= npcDamage.damage;

                if (playerHealth.current <= 0)
                {
                    //m_game->playSound("LinkDie");
                    m_player->destroy();
                    m_tutorialTextClock.restart();
                }
                break;
            }
        }
    }
    
}
void Scene_MainGame::sMeleeCollision()
{
    //Melee collisions with NPC's are implemented here
    
    if (m_player->getComponent<CAnimation>().animation.getName() == "Axe" && m_frameSinceAttack == 24)
    {
        m_game->playSound("Melee");
        m_game->setVolume("Melee", m_effectVolume);
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& playerDamage = m_player->getComponent<CDamage>();
        auto& axe = m_entityManager.addEntity("weapon");
        axe->addComponent<CBoundingBox>(m_player->getComponent<CAnimation>().animation.getSize());
        axe->addComponent<CTransform>(Vec2(playerTransform.pos.x + playerTransform.scale.x * 5, playerTransform.pos.y), Vec2(5 * playerTransform.scale.x, 0), playerTransform.scale, 0);
        axe->addComponent<CDamage>((playerDamage.damage + playerDamage.tempDamage)*3);
        axe->addComponent<CLifeSpan>(0, m_currentFrame);
        for (auto& e : m_entityManager.getEntities("npc"))
        {
            auto& npcHealth = e->getComponent<CHealth>();
            auto npcWeaponOverlap = Physics::GetOverlap(axe, e);
            if (npcWeaponOverlap.x > 0 && npcWeaponOverlap.y > 0)
            {
                npcHealth.current -= axe->getComponent<CDamage>().damage;
                if (npcHealth.current <= 0)
                {
                    //m_game->playSound("EnemyDie");

                    auto ex = m_entityManager.addEntity("explosion");
                    ex->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                    ex->addComponent<CTransform>().pos = e->getComponent<CTransform>().pos;
                    e->destroy();
                    break;
                }
            }
        }

    }
    if (m_player->getComponent<CAnimation>().animation.getName() == "Dagger" && (m_frameSinceAttack == 10 || m_frameSinceAttack == 40 || m_frameSinceAttack == 60))
    {
        m_game->playSound("Melee");
        m_game->setVolume("Melee", m_effectVolume);
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& playerDamage = m_player->getComponent<CDamage>();
        auto& axe = m_entityManager.addEntity("weapon");
        axe->addComponent<CBoundingBox>(Vec2(m_player->getComponent<CAnimation>().animation.getSize().x*2/3, m_player->getComponent<CAnimation>().animation.getSize().y));
        axe->addComponent<CTransform>(Vec2(playerTransform.pos.x + playerTransform.scale.x * 5, playerTransform.pos.y), Vec2(5 * playerTransform.scale.x, 0), playerTransform.scale, 0);
        axe->addComponent<CDamage>((playerDamage.damage + playerDamage.tempDamage));
        axe->addComponent<CLifeSpan>(0, m_currentFrame);
        for (auto& e : m_entityManager.getEntities("npc"))
        {
            auto& npcHealth = e->getComponent<CHealth>();
            auto npcWeaponOverlap = Physics::GetOverlap(axe, e);
            if (npcWeaponOverlap.x > 0 && npcWeaponOverlap.y > 0)
            {
                npcHealth.current -= axe->getComponent<CDamage>().damage;
                if (npcHealth.current <= 0)
                {
                    m_game->playSound("EnemyHit");
                    m_game->setVolume("EnemyHit", m_effectVolume);
                    //m_game->playSound("EnemyDie");
                    auto ex = m_entityManager.addEntity("explosion");
                    ex->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                    ex->addComponent<CTransform>().pos = e->getComponent<CTransform>().pos;
                    e->destroy();
                    break;
                }
                else
                {
                    m_game->playSound("EnemyHit");
                    m_game->setVolume("EnemyHit", m_effectVolume);
                }
            }
        }
    }
}
void Scene_MainGame::sBreakableCollision()
{
    for (auto& weapon : m_entityManager.getEntities("weapon"))
    {
        for (auto& e : m_entityManager.getEntities("breakable"))
        {
            auto breakableWeaponOverlap = Physics::GetOverlap(weapon, e);
            if (breakableWeaponOverlap.x > 0 && breakableWeaponOverlap.y > 0)
            {
                //m_game->playSound("EnemyDie");
                auto ex = m_entityManager.addEntity("explosion");
                ex->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                ex->addComponent<CTransform>().pos = e->getComponent<CTransform>().pos;
                e->destroy();
                break;
                srand(time(0));
                if (e->getComponent<CAnimation>().animation.getName() == "Barrel")
                {
                    m_player->getComponent<CInventory>().money += rand() % 10 + 2;
                }
                if (e->getComponent<CAnimation>().animation.getName() == "JarBig")
                {
                    m_player->getComponent<CInventory>().money += rand() % 5 + 1;
                }
                if (e->getComponent<CAnimation>().animation.getName() == "JarSmall")
                {
                    m_player->getComponent<CInventory>().money += rand() % 3;
                }
            }
        }
    }
}
void Scene_MainGame::sArrowCollision()
{
    if (m_player->getComponent<CAnimation>().animation.getName() == "Bow" && m_frameSinceAttack == 10)
    {
        m_game->playSound("Bow");
        m_game->setVolume("Bow", m_effectVolume);
        auto& playerDamage = m_player->getComponent<CDamage>();
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& arrow = m_entityManager.addEntity("arrow");
        arrow->addComponent<CAnimation>(m_game->assets().getAnimation("Arrow"), true);
        arrow->addComponent<CBoundingBox>(m_game->assets().getAnimation("Arrow").getSize());
        arrow->addComponent<CTransform>(Vec2(playerTransform.pos.x + playerTransform.scale.x * 5,playerTransform.pos.y),Vec2(5 * playerTransform.scale.x,0),playerTransform.scale,0);
        arrow->addComponent<CDamage>(playerDamage.damage + playerDamage.tempDamage);
        arrow->addComponent<CLifeSpan>(180, m_currentFrame);
    }
    //Arrow collisions with NPC's are implemented here
    for (auto& arrow : m_entityManager.getEntities("arrow"))
    {
        auto& arrowDamage = arrow->getComponent<CDamage>();
        for (auto& e : m_entityManager.getEntities("npc"))
        {
            auto& npcHealth = e->getComponent<CHealth>();
            auto npcWeaponOverlap = Physics::GetOverlap(arrow, e);
            if (npcWeaponOverlap.x > 0 && npcWeaponOverlap.y > 0)
            {
                npcHealth.current -= arrowDamage.damage;
                arrow->destroy();
                //m_game->playSound("EnemyHit");
                if (npcHealth.current <= 0)
                {
                    m_game->playSound("EnemyHit");
                    m_game->setVolume("EnemyHit", m_effectVolume);
                    //m_game->playSound("EnemyDie");
                    auto ex = m_entityManager.addEntity("explosion");
                    Vec2 exPos = e->getComponent<CTransform>().pos;
                    
                    if (e->getComponent<CAnimation>().animation.getName() == "GhostShriek")
                    {
                        
                        //ex->addComponent<CAnimation>(m_game->assets().getAnimation("GhostVanish"), false);
                        e->getComponent<CState>().state = "GhostVanish";
                        e->getComponent<CAnimation>().repeat = false;
                    }
                    else
                    {
                        ex->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                        e->destroy();
                    }
                    ex->addComponent<CTransform>().pos = exPos;
                    
                    break;
                }
                else
                {
                    m_game->playSound("EnemyHit");
                    m_game->setVolume("EnemyHit", m_effectVolume);
                }
            }
        }
    }
}
void Scene_MainGame::sHeartCollision()
{
    //Heart collisions with entities are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    auto& playerHealth = m_player->getComponent<CHealth>();
    for (auto hearts : m_entityManager.getEntities("hearts"))
    {
        auto& heartsBoundingBox = hearts->getComponent<CBoundingBox>();
        auto& heartsAnim = hearts->getComponent<CAnimation>().animation;

        // Checking for player and heart collision
        auto playerHeartsOverlap = Physics::GetOverlap(hearts, m_player);
        if (playerHeartsOverlap.x > 0 && playerHeartsOverlap.y > 0)
        {
            // Player can only pick it up if health is below max
            if (playerHealth.current < playerHealth.max)
            {
                playerHealth.current = playerHealth.max;
                //m_game->playSound("GetItem");
                hearts->destroy();
            }
        }

        // Checking for npc and heart collision
        for (auto e : m_entityManager.getEntities("npc"))
        {
            auto& npcTransform = e->getComponent<CTransform>();
            auto npcheartsOverlap = Physics::GetOverlap(hearts, e);
            auto& npcHealth = e->getComponent<CHealth>();
            if (npcheartsOverlap.x > heartsBoundingBox.halfSize.x && npcheartsOverlap.y > heartsBoundingBox.halfSize.y)
            {
                // NPC can only pick it up if health is below max
                if (npcHealth.current < npcHealth.max)
                {
                    npcHealth.current = npcHealth.max;
                    hearts->destroy();
                }
            }
        }
    }
}
void Scene_MainGame::sCoinCollision()
{
    //Rupee collisions with entities are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    for (auto coin : m_entityManager.getEntities("coins"))
    {
        auto& coinBoundingBox = coin->getComponent<CBoundingBox>();
        auto& coinAnim = coin->getComponent<CAnimation>().animation;

        // Checking for player and rupee collision
        auto playerCoinsOverlap = Physics::GetOverlap(coin, m_player);
        if (playerCoinsOverlap.x > coinBoundingBox.halfSize.x && playerCoinsOverlap.y > coinBoundingBox.halfSize.y)
        {
            m_game->playSound("Coin");
            m_game->setVolume("Coin", m_effectVolume);
            //m_game->playSound("GetItem");
            m_player->getComponent<CInventory>().money++;
            drawInventory();
            coin->destroy();
        }
    }
}
void Scene_MainGame::sItemCollision()
{
    //Rupee collisions with entities are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    for (auto potion : m_entityManager.getEntities("potions"))
    {
        auto& potionBoundingBox = potion->getComponent<CBoundingBox>();
        auto& potionAnim = potion->getComponent<CAnimation>().animation;

        // Checking for player and rupee collision
        auto playerPotionsOverlap = Physics::GetOverlap(potion, m_player);
        if (playerPotionsOverlap.x > potionBoundingBox.halfSize.x && playerPotionsOverlap.y > potionBoundingBox.halfSize.y)
        {
            m_player->getComponent<CInventory>().items.push_back(potionAnim.getName());
            drawInventory();
            potion->destroy();
        }
    }
}
void Scene_MainGame::sTeleportCollision()
{
    //player collisions with teleport blocks are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    auto& playerHealth = m_player->getComponent<CHealth>();
    auto& playerAnim = m_player->getComponent<CAnimation>();
    auto& pState = m_player->getComponent<CState>();
    std::vector<Vec2> teleportLocations;

    // Storing all the teleport entity positions in teleport locations array
    for (auto teleports : m_entityManager.getEntities("teleport"))
    {
        auto& teleportTransform = teleports->getComponent<CTransform>();
        teleportLocations.push_back(teleportTransform.pos);
    }

    // Checking for collision with teleport entity
    for (auto teleports : m_entityManager.getEntities("teleport"))
    {
        auto& teleportsBoundingBox = teleports->getComponent<CBoundingBox>();
        auto playerteleportsOverlap = Physics::GetOverlap(teleports, m_player);
        
        // if colliding set the player position to a random teleport location entry
        if (playerteleportsOverlap.x > teleportsBoundingBox.halfSize.x && playerteleportsOverlap.y > teleportsBoundingBox.halfSize.y)
        {
            Vec2 newLocation = teleportLocations[rand() % teleportLocations.size()];
            if (newLocation.x != teleports->getComponent<CTransform>().pos.x)
            {
                playerTransform.pos.x = newLocation.x;
                playerTransform.pos.y = newLocation.y + 64;
            } 
        }
    }

}
void Scene_MainGame::sEnemyCollision() {
    auto& playerHealth = m_player->getComponent<CHealth>();
    for (auto npc : m_entityManager.getEntities("npc"))
    {
        if (npc->getComponent<CAnimation>().animation.getName() == "DemonIdle")
        {
            //npc->getComponent<CAnimation>().animation.get
            auto& npcState = npc->getComponent<CState>();
            auto& npcDamage = npc->getComponent<CDamage>();
            auto& npcTransform = npc->getComponent<CTransform>();

            auto npcPlayerOverlap = Physics::GetOverlap(m_player, npc);
            if (npcPlayerOverlap.x > 0 && npcPlayerOverlap.y > 0)
            {
                //auto frame = m_currentFrame;
                npcState.state = "DemonAttack";
                auto& npcTransform = npc->getComponent<CTransform>();
            }
            
        }
        
    }
}

void Scene_MainGame::sAnimation()
{

#pragma region Player Animation
    auto& playerTransform = m_player->getComponent<CTransform>();
    auto& playerAnimation = m_player->getComponent<CAnimation>();
    auto& playerInput = m_player->getComponent<CInput>();
    auto& playerState = m_player->getComponent<CState>();

    if (playerAnimation.animation.getName() == playerState.state)
    {
        playerAnimation.animation.update();
        if (playerInput.attack)
        {
            if (playerAnimation.animation.hasEnded())
            {
                playerInput.attack = false;
            }
        }
    }
    else
    {
        playerAnimation.animation = m_game->assets().getAnimation(playerState.state);
    }
#pragma endregion

#pragma region NPC Animation
    for (auto& e : m_entityManager.getEntities("npc"))
    {
        if (e->hasComponent<CState>())
        {
            //Animation for special NPC Octorok
            if (e->getComponent<CState>().state == e->getComponent<CAnimation>().animation.getName())
            {
                e->getComponent<CAnimation>().animation.update();
                if (e->getComponent<CAnimation>().repeat == false)
                {
                    if (e->getComponent<CAnimation>().animation.hasEnded())
                    {
                        e->destroy();
                    }
                }
            }
            else
            {
                e->getComponent<CAnimation>().animation = m_game->assets().getAnimation(e->getComponent<CState>().state);
            }
        }
        else
        {
            //Animation for Patrol NPCs
            e->getComponent<CAnimation>().animation.update();
            if (e->getComponent<CAnimation>().repeat == false)
            {
                if (e->getComponent<CAnimation>().animation.hasEnded())
                {
                    e->destroy();
                }
            }
        }
    }

    
#pragma endregion

#pragma region DestructionAnimation(Explosion)
    for (auto& exp : m_entityManager.getEntities("explosion"))
    {
        exp->getComponent<CAnimation>().animation.update();
        if (exp->getComponent<CAnimation>().repeat == false)
        {
            if (exp->getComponent<CAnimation>().animation.hasEnded())
            {
                exp->destroy();
            }
        }
    }
#pragma endregion

    
}

void Scene_MainGame::sCamera()
{
    auto center = m_game->window().getView().getCenter();
    // get the current view, which we will modify in the if-statement below
   
    
    sf::View view = m_game->window().getView();
    /*f*//*loat windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);*/

    view.setSize(1280, 768);
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    Vec2 playerPos = m_player->getComponent<CTransform>().pos;
    sf::Vector2f newCamPos(playerPos.x,m_game->window().getSize().y/2);
    if (newCamPos.x < view.getSize().x / 2)
    {
        newCamPos.x = view.getSize().x / 2;
    }
    
    m_lighting.setPosition(sf::Vector2f(newCamPos.x - m_game->window().getSize().x/2, newCamPos.y - m_game->window().getSize().y / 2));
    auto mDiff = center - newCamPos;
    //m_mPos += Vec2(mDiff.x, mDiff.y);
    view.setCenter(newCamPos);
    
    
#pragma region Setting Up strings and positions of UI Texts
   
    m_levelText.setCharacterSize(12);
    Vec2 levelTextPos = Vec2(m_player->getComponent<CTransform>().pos.x - 40, m_player->getComponent<CTransform>().pos.y - 96);
    
    
    m_tutorialText.setString(" Move:W, A, S D Weapon Swap: TAB");
    if (m_weaponSwitch == 0)
    {
        m_levelText.setString("Dagger Activated");
    }
    else if (m_weaponSwitch == 1)
    {
        m_levelText.setString("  Bow Activated");
    }
    else
    {
        m_levelText.setString("  Axe Activated");
    }

    if (m_weaponTextClock.getElapsedTime().asSeconds() > 2)
    {
        m_levelText.setString("");
    }
    if (m_tutorialTextClock.getElapsedTime().asSeconds() > 3)
    {
        m_tutorialText.setString("");
    }

    m_levelText.setPosition(sf::Vector2f(levelTextPos.x, levelTextPos.y + 128));
    m_tutorialText.setPosition(sf::Vector2f(levelTextPos.x - 50, levelTextPos.y - 50));
    
#pragma endregion

    // then set the window view
    m_game->window().setView(view);
}

void Scene_MainGame::drawWeaponHolder()
{
    for (auto& weaponHolder : m_entityManager.getEntities("weaponHolder"))
    {
        weaponHolder->destroy();
    }

    std::string currentWeaponHUD = "";
    
    if (m_weaponSwitch == 0)
    {
        currentWeaponHUD = "DaggerHUD";
    }

    // use Bow Animation
    else if (m_weaponSwitch == 1)
    {
        currentWeaponHUD = "BowHUD";
    }
    else
    {
        currentWeaponHUD = "AxeHUD";
    }
    auto anim = m_game->assets().getAnimation(currentWeaponHUD);
    Vec2 weaponHolderPos = Vec2(0, 0);
    
    auto& weaponHolder = m_entityManager.addEntity("weaponHolder");
    weaponHolder->addComponent<CAnimation>(anim, false);
    weaponHolder->addComponent<CTransform>(weaponHolderPos);
    //weaponHolder->addComponent<CBoundingBox>(Vec2(anim.getSize().x, anim.getSize().y));
}

void Scene_MainGame::sHUD()
{
    sf::View view = m_game->window().getView();
    //Vec2 InventoryPos = Vec2(m_player->getComponent<CTransform>().pos.x + 0, m_player->getComponent<CTransform>().pos.y + 320);
    Vec2 playerPos = m_player->getComponent<CTransform>().pos;
    Vec2 InventoryPosOffset = Vec2(m_game->window().getSize().x / 2 - (m_gridSize.x * 5 + 32), m_gridSize.y / 2);
    Vec2 InventoryPos = Vec2(playerPos.x - InventoryPosOffset.x, InventoryPosOffset.y);
    Vec2 weaponHolderOffset = Vec2(m_gridSize.x * 4 + m_gridSize.x / 2, 32);
    Vec2 weaponHolderPos = Vec2(InventoryPos.x - m_gridSize.x * 5, InventoryPos.y);


    //sf::Vector2f newCamPos(playerPos.x, playerPos.y);
    if (InventoryPos.x < view.getSize().x / 2 - InventoryPosOffset.x)
    {
        InventoryPos.x = view.getSize().x / 2 - InventoryPosOffset.x;
    }

    if (weaponHolderPos.x < view.getSize().x / 2)
    {
        weaponHolderPos.x = InventoryPos.x - m_gridSize.x * 5;
        weaponHolderPos.y = InventoryPos.y;
    }

    
    
    Vec2 coinHUDPosition = Vec2(weaponHolderPos.x, weaponHolderPos.y + m_gridSize.y);

    
    for (auto& weaponHolder : m_entityManager.getEntities("weaponHolder"))
    {
        weaponHolder->getComponent<CTransform>().pos = weaponHolderPos;
    }

    for (auto& coinHUD : m_entityManager.getEntities("CoinHUD"))
    {
        coinHUD->getComponent<CTransform>().pos = coinHUDPosition;
        m_walletText.setPosition(sf::Vector2f(coinHUDPosition.x + 32, coinHUDPosition.y - 25));
        m_walletText.setString("x" + std::to_string(m_player->getComponent<CInventory>().money));
    }

    // Setting inventory position relative to player
    for (auto& inventory : m_entityManager.getEntities("inventory"))
    {
        inventory->getComponent<CTransform>().pos = InventoryPos;
        if (m_InventoryClock.getElapsedTime().asSeconds() > 4)
        {
            inventory->destroy();
            inventoryOpened = false;
            m_walletText.setString("");
        }
    }
    for (auto& coinHUD : m_entityManager.getEntities("CoinHUD"))
    {
        if (m_InventoryClock.getElapsedTime().asSeconds() > 4)
        {
            coinHUD->destroy();
            m_walletText.setString("");
            
        }
    }
    int inventoryItemPositionOffset = 0;
    // Setting inventory items positions
    for (auto& inventoryItems : m_entityManager.getEntities("inventoryItems"))
    {
        
        inventoryItems->getComponent<CTransform>().pos.x = InventoryPos.x - (m_gridSize.x*4) + inventoryItemPositionOffset;
        inventoryItems->getComponent<CTransform>().pos.y = InventoryPos.y;
        inventoryItemPositionOffset += 64;
        
        if (inventoryItemPositionOffset > m_gridSize.x * 8)
        {
            inventoryItemPositionOffset = 0;
        }
        if (m_InventoryClock.getElapsedTime().asSeconds() > 4)
        {
            inventoryItems->destroy();
            
        }
    }
    inventorySelect.setPosition(InventoryPos.x + 64 * m_select  -(m_gridSize.x * 4), InventoryPos.y);
    
}

void Scene_MainGame::sDrawInventoryItems()
{
    inventoryOpened = true;
    for (auto& inventoryItems : m_entityManager.getEntities("inventoryItems"))
    {
        inventoryItems->destroy();
    }
    for (auto& coinHud : m_entityManager.getEntities("CoinHUD"))
    {
        coinHud->destroy();
    }

    auto coin = m_entityManager.addEntity("CoinHUD");
    
    coin->addComponent<CTransform>(getPosition(0,0,0,0));
    coin->addComponent<CAnimation>(m_game->assets().getAnimation("CoinHUD"), true);
    coin->addComponent<CBoundingBox>(m_game->assets().getAnimation("CoinHUD").getSize(), m_itemConfig.BM, m_itemConfig.BV);


    auto& items = m_player->getComponent<CInventory>().items;
    if (items.size() != 0)
    {
        for (auto item : items)
        {
            auto anim = m_game->assets().getAnimation(item);
            auto& invItem = m_entityManager.addEntity("inventoryItems");
            invItem->addComponent<CAnimation>(anim, false);
            invItem->addComponent<CTransform>();
        }
    }
}

void Scene_MainGame::drawInventory()
{
    m_InventoryClock.restart();
    float viewX = m_game->window().getView().getCenter().x - (m_game->window().getSize().x / 2);
    float viewY = m_game->window().getView().getCenter().y - (m_game->window().getSize().y / 2);
    //auto mpos = sf::Mouse::getPosition(m_game->window());
    Vec2 gridPos(m_player->getComponent<CTransform>().pos.x + viewX, m_player->getComponent<CTransform>().pos.y + viewY);

    auto anim = m_game->assets().getAnimation("SmallInventory");
    auto& invTile = m_entityManager.addEntity("inventory");
    invTile->addComponent<CAnimation>(anim, false);
    invTile->addComponent<CTransform>(gridPos);
    invTile->addComponent<CBoundingBox>(Vec2(anim.getSize().x, anim.getSize().y));
    sDrawInventoryItems();
    
}

void Scene_MainGame::drawMinimap()
{
    sf::View minimapView = m_game->window().getView();
    minimapView.setViewport(sf::FloatRect(0.74f, 0.01f, 0.25f, 0.25f));
    minimapView.zoom(2.5f);
    m_game->window().setView(minimapView);

    for (auto e : m_entityManager.getEntities())
    {
        
        if (e->hasComponent<CAnimation>())
        {
            m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
        }
    }
}

void Scene_MainGame::sDragAndDrop()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() &&
            e->getComponent<CDraggable>().dragging)
        {
            e->getComponent<CTransform>().pos = m_mPos;
        }
    }
}

void Scene_MainGame::grab()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() &&
            Physics::IsInside(m_mPos, e))
        {
            e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
            if (!e->getComponent<CDraggable>().dragging)
            {
                snap(e);
            }
            std::cout << "Entity Clicked: " << e->tag() << std::endl;
            break;
        }
    }
}

void Scene_MainGame::snap(std::shared_ptr<Entity> e)
{
    auto& pos = e->getComponent<CTransform>().pos;
    pos.x = (floor(pos.x / m_gridSize.x) * m_gridSize.x) + m_gridSize.x / 2;
    pos.y = (floor(pos.y / m_gridSize.y) * m_gridSize.y) + m_gridSize.y / 2;
}

void Scene_MainGame::onEnd()
{
     m_game->stopSound("MusicGame");
     m_game->playSound("MusicTitle");
     m_game->setVolume("MusicTitle", m_musicVolume);
     m_game->changeScene("MENU", nullptr, true);
}

void Scene_MainGame::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
    m_game->window().draw(line, 2, sf::Lines);
}

void Scene_MainGame::sRender()
{
    // RENDERING DONE FOR YOU
                           
    m_game->window().clear(sf::Color(0, 0, 0));
    sf::RectangleShape tick({ 1.0f, 6.0f });
    //tick.setFillColor(sf::Color::Black);
    //m_game->window().draw(spriteBG);
    drawParallaxBackground();

    
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        // draw entity animations
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
                           
            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                if (e->hasComponent<CShader>())
                {
                    if (e->getComponent<CShader>().ourShader != "None")
                    {
                        e->getComponent<CShader>().duration--;
                        sf::Shader shader;
                        if (!shader.loadFromFile(e->getComponent<CShader>().ourShader, sf::Shader::Fragment))
                        {
                            std::cerr << "Error while shaders" << std::endl;
                            m_game->window().draw(animation.getSprite());
                        }
                        else
                        {
                            shader.setUniform("time", m_time.getElapsedTime().asSeconds());
                            m_game->window().draw(animation.getSprite(), &shader);
                        }
                        if (e->getComponent<CShader>().duration <= 0)
                        {
                            e->getComponent<CShader>().ourShader = "None";
                        }
                    }
                    else
                    {
                        m_game->window().draw(animation.getSprite());
                    }
                }
                else
                {
                    m_game->window().draw(animation.getSprite());
                }
            }
        }

        // draw entity health bars
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            if (e->hasComponent<CHealth>())
            {
                auto& h = e->getComponent<CHealth>();
                Vec2 size(64, 6);
                sf::RectangleShape rect({ size.x, size.y });
                rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
                rect.setFillColor(sf::Color(96, 96, 96));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(2);
                m_game->window().draw(rect);

                float ratio = (float)(h.current) / h.max;
                size.x *= ratio;
                rect.setSize({ size.x, size.y });
                rect.setFillColor(sf::Color(255, 0, 0));
                rect.setOutlineThickness(0);
                m_game->window().draw(rect);

                for (int i = 0; i < h.max; i++)
                {
                    tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
                    m_game->window().draw(tick);
                }
            }
        }
    }
    

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::Black);
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
                if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
                if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
                if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }

            if (e->hasComponent<CPatrol>())
            {
                auto& patrol = e->getComponent<CPatrol>().positions;
                for (size_t p = 0; p < patrol.size(); p++)
                {
                    dot.setPosition(patrol[p].x, patrol[p].y);
                    m_game->window().draw(dot);
                }
            }

            if (e->hasComponent<CFollowPlayer>())
            {
                sf::VertexArray lines(sf::LinesStrip, 2);
                lines[0].position.x = e->getComponent<CTransform>().pos.x;
                lines[0].position.y = e->getComponent<CTransform>().pos.y;
                lines[0].color = sf::Color::Black;
                lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
                lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
                lines[1].color = sf::Color::Black;
                m_game->window().draw(lines);
                dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
                m_game->window().draw(dot);
            }
        }
    }

    if (inventoryOpened)
    {
        inventorySelect.setRadius(23.5);
        //inventorySelect.setSize(sf::Vector2f(45, 45));
        inventorySelect.setOrigin(sf::Vector2f(45 / 2 + 1, 45 / 2));
        //inventorySelect.setPosition(getPosition(0,0,5,0).x, getPosition(0, 0, 5, 0).y);
        inventorySelect.setFillColor(sf::Color(0, 0, 0, 0));
        inventorySelect.setOutlineColor(sf::Color(196, 70, 70, 255));
        inventorySelect.setOutlineThickness(5);
        m_game->window().draw(inventorySelect);
    }
    
  

    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
               
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string(11 - ((int)y / (int)m_gridSize.y));
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
    sf::Shader shader;
    if (!shader.loadFromFile(m_shaders[2], sf::Shader::Fragment))
    {
        std::cerr << "Error while shaders" << std::endl;
        m_game->window().draw(m_lighting);
    }
    else
    {
        shader.setUniform("positionx", (float)m_player->getComponent<CTransform>().pos.x - m_game->window().getView().getCenter().x + m_game->window().getSize().x/2);
        shader.setUniform("positiony", (float)m_player->getComponent<CTransform>().pos.y - m_game->window().getView().getCenter().y + m_game->window().getSize().y / 2);
        shader.setUniform("radius", 300);
        m_game->window().draw(m_lighting, &shader);
    }
    // Particles on the front
    m_parallaxBackgroundSprites[2].move(sf::Vector2f(0.4f, 0.f));
    m_parallaxBackgroundSprites[2].setPosition(m_parallaxBackgroundSprites[2].getPosition().x, -50);
    m_game->window().draw(m_parallaxBackgroundSprites[2]);// Particles
    m_game->window().draw(m_tutorialText);
    m_game->window().draw(m_walletText);
    m_game->window().draw(m_levelText);

    if (m_minimap)
    {
        drawMinimap();

    } 
}