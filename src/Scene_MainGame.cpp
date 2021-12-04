                          
#include "Scene_MainGame.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

Scene_MainGame::Scene_MainGame(GameEngine* game, const std::string& levelPath)
    : Scene(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_MainGame::init(const std::string& levelPath)
{
    loadLevel(levelPath);
    //m_game->playSound("MusicLevel");

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");       // Toggle follow camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");         // Toggle drawing (G)rid Boxes
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Space, "ATTACK");
    registerAction(sf::Keyboard::Tab, "WEAPON_SWITCH");
    registerAction(sf::Keyboard::O, "Zoom Map");
    registerAction(sf::Keyboard::M, "MiniMap");
    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));
    m_levelText.setFont(m_game->assets().getFont("Arial"));
    m_levelText.setFillColor(sf::Color::White);
    m_tutorialText.setFont(m_game->assets().getFont("Arial"));
    m_tutorialText.setCharacterSize(20);
    m_tutorialText.setFillColor(sf::Color::White);
    m_walletText.setFont(m_game->assets().getFont("Arial"));
    m_walletText.setCharacterSize(12);
    m_walletText.setFillColor(sf::Color::White);
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
                if (m_tileConfig.Name == "Heart")
                {
                    auto heart = m_entityManager.addEntity("hearts");
                    heart->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                    heart->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                    heart->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                }
                else if (m_tileConfig.Name == "Black")
                {
                    auto teleport = m_entityManager.addEntity("teleport");
                    teleport->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                    teleport->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                    teleport->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                }
                else if (m_tileConfig.Name == "Rupee" || m_tileConfig.Name == "RupeeBlue")
                {
                    auto rupee = m_entityManager.addEntity("rupee");
                    rupee->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                    rupee->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                    rupee->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                }
                else if (m_tileConfig.Name != "Heart" || m_tileConfig.Name != "Black")
                {
                    auto tile = m_entityManager.addEntity("tile");
                    tile->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                    tile->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                    tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                    tile->addComponent<CDraggable>();
                }
                continue;
            }
            if (configRead == "NPC")
            {
                config >> m_npcConfig.Name >> m_npcConfig.RX >> m_npcConfig.RY >> m_npcConfig.TX >> m_npcConfig.TY >> m_npcConfig.BM >> m_npcConfig.BV >> m_npcConfig.H >> m_npcConfig.D >> m_npcConfig.AI >> m_npcConfig.S;
                auto npc = m_entityManager.addEntity("npc");
                npc->addComponent<CDraggable>();
                if (m_npcConfig.AI == "Follow")
                {
                    npc->addComponent<CTransform>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY));
                    npc->addComponent<CAnimation>(m_game->assets().getAnimation(m_npcConfig.Name), true);
                    npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_npcConfig.Name).getSize(), m_npcConfig.BM, m_npcConfig.BV);
                    npc->addComponent<CFollowPlayer>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY), m_npcConfig.S);
                    npc->addComponent<CHealth>(m_npcConfig.H, m_npcConfig.H);
                    npc->addComponent<CDamage>(m_npcConfig.D);
                    if (m_npcConfig.Name == "OctorokUp" || m_npcConfig.Name == "OctorokRight")
                    {
                        npc->addComponent<CState>(m_npcConfig.Name);
                    }
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
                    npc->addComponent<CHealth>(m_npcConfig.H, m_npcConfig.H);
                    npc->addComponent<CDamage>(m_npcConfig.D);
                    if (m_npcConfig.Name == "OctorokUp" || m_npcConfig.Name == "OctorokRight")
                    {
                        npc->addComponent<CState>(m_npcConfig.Name);
                    }
                    continue;
                }
            }
        }
    }
                           
    spawnPlayer();
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
    m_player->addComponent<CState>("StandDown");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CDraggable>();
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
        spawnPlayer();
    }
    // When the game is paused
    if (m_paused)
    {
        m_levelText.setString("Paused"); 
        m_levelText.setCharacterSize(24);
        m_walletText.setString("\n  Coins: x" + std::to_string(m_wallet));
        m_game->pauseSound("MusicLevel");
        
        return;
    } 
    if (m_player->getComponent<CInput>().attack)
    {
        m_frameSinceAttack++;
    }
    sAI();
    sMovement();
    sDragAndDrop();
    sArrowMovement();
    sStatus();
    sCollision();
    sAnimation();
    sCamera();
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
            pTransform.velocity.y = -7 * m_playerConfig.SPEED;
            pTransform.facing = Vec2(0, -1);
            pState.state = "Jump";
            pTransform.scale = Vec2(1, 1);
        }
        else
        {
            pTransform.velocity.y = 0;
        }
   
        // if only one x directional key is pressed move in that direction otherwise stop.
        if (pInput.left && !pInput.right)
        {
            pTransform.velocity.x = -1 * m_playerConfig.SPEED;
            pTransform.facing = Vec2(-1, 0);
            pState.state = "RunRight";
            pTransform.scale = Vec2(-1, 1);
        }
        else if (!pInput.left && pInput.right)
        {
            pTransform.velocity.x = m_playerConfig.SPEED;
            pTransform.facing = Vec2(1, 0);
            pState.state = "RunRight";
            pTransform.scale = Vec2(1, 1);
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
        pTransform.velocity.y += m_FrameSinceGrounded * m_player->getComponent<CGravity>().gravity;

        //if (pInput.up == false)
        //{
        //    //m_playerHitTile = true;
        //}
    }
    else
    {
        m_FrameSinceGrounded = 0;
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
        if (action.name() == "PAUSE") {  m_levelText.setPosition(m_levelText.getPosition().x, m_levelText.getPosition().y - 64);
        // m_game->playSound("MusicLevel");
        sf::View view = m_game->window().getView();
        view.zoom(2.0f);
        m_game->window().setView(view);
        }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW") { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision;}
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; m_playerOnGround = false; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true;}
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
        else if (action.name() == "LEFT_CLICK") { grab(); }
        else if (action.name() == "ATTACK") { if (!m_player->getComponent<CInput>().attack && !m_paused) { startAttack(m_player);  m_player->getComponent<CInput>().attack = true; } }
        else if (action.name() == "WEAPON_SWITCH") { if (!m_player->getComponent<CInput>().attack) { m_weaponTextClock.restart(); if (m_weaponSwitch < 2) { m_weaponSwitch++; } else { m_weaponSwitch = 0; } } }
        else if (action.name() == "Zoom Map") 
        {
            sf::View view = m_game->window().getView();
            view.zoom(0.5f);
            m_game->window().setView(view);
        }
        else if (action.name() == "MiniMap") { m_minimap = !m_minimap; }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
    }

    if (action.name() == "MOUSE_MOVE") { m_mPos = action.pos() + Vec2(m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2, m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2); }
}

void Scene_MainGame::sAI()
{
    for (auto e : m_entityManager.getEntities())
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
                    e->getComponent<CTransform>().pos.x += e->getComponent<CFollowPlayer>().speed * desired.x;
                    e->getComponent<CTransform>().pos.y += e->getComponent<CFollowPlayer>().speed * desired.y;
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
                if (e->hasComponent<CState>())
                {
                    if (desired.x != 0)
                    {
                        e->getComponent<CState>().state = "OctorokRight";
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
                    {
                        e->getComponent<CState>().state = "OctorokUp";
                        if (desired.y < 0)
                        {
                            e->getComponent<CTransform>().scale.y = 1;
                        }
                        else
                        {
                            e->getComponent<CTransform>().scale.y = -1;
                        }
                    }
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
    sHeartCollision();
    sRupeeCollision();
    sTeleportCollision();
}

void Scene_MainGame::sTileCollision()
{
    // Tile collisions with entities are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    m_playerOnGround = false;
    for (auto tile : m_entityManager.getEntities("tile"))
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
                        m_playerOnGround = true;
                    }
                }
                else
                {
                    if (playerTransform.pos.x > tileTransform.pos.x)
                    {
                        playerTransform.pos.x += playerTileOverlap.x;
                    }
                    else
                    {
                        playerTransform.pos.x -= playerTileOverlap.x;
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
                m_player->addComponent<CInvincibility>(30);
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
    if (m_player->getComponent<CAnimation>().animation.getName() == "Axe" && m_frameSinceAttack == 10)
    {
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& axe = m_entityManager.addEntity("weapon");
        axe->addComponent<CBoundingBox>(m_player->getComponent<CAnimation>().animation.getSize());
        axe->addComponent<CTransform>(Vec2(playerTransform.pos.x + playerTransform.scale.x * 5, playerTransform.pos.y), Vec2(5 * playerTransform.scale.x, 0), playerTransform.scale, 0);
        axe->addComponent<CDamage>(5);
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
    if (m_player->getComponent<CAnimation>().animation.getName() == "Dagger" && (m_frameSinceAttack == 2 || m_frameSinceAttack == 9 || m_frameSinceAttack == 16))
    {
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& axe = m_entityManager.addEntity("weapon");
        axe->addComponent<CBoundingBox>(Vec2(m_player->getComponent<CAnimation>().animation.getSize().x*2/3, m_player->getComponent<CAnimation>().animation.getSize().y));
        axe->addComponent<CTransform>(Vec2(playerTransform.pos.x + playerTransform.scale.x * 5, playerTransform.pos.y), Vec2(5 * playerTransform.scale.x, 0), playerTransform.scale, 0);
        axe->addComponent<CDamage>(1);
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
}
void Scene_MainGame::sArrowCollision()
{
    if (m_player->getComponent<CAnimation>().animation.getName() == "Bow" && m_frameSinceAttack == 10)
    {
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& arrow = m_entityManager.addEntity("arrow");
        arrow->addComponent<CAnimation>(m_game->assets().getAnimation("Arrow"), true);
        arrow->addComponent<CBoundingBox>(m_game->assets().getAnimation("Arrow").getSize());
        arrow->addComponent<CTransform>(Vec2(playerTransform.pos.x + playerTransform.scale.x * 5,playerTransform.pos.y),Vec2(5 * playerTransform.scale.x,0),playerTransform.scale,0);
        arrow->addComponent<CDamage>(1);
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
void Scene_MainGame::sRupeeCollision()
{
    //Rupee collisions with entities are implemented here
    auto& playerTransform = m_player->getComponent<CTransform>();
    for (auto rupee : m_entityManager.getEntities("rupee"))
    {
        auto& rupeeBoundingBox = rupee->getComponent<CBoundingBox>();
        auto& rupeeAnim = rupee->getComponent<CAnimation>().animation;

        // Checking for player and rupee collision
        auto playerHeartsOverlap = Physics::GetOverlap(rupee, m_player);
        if (playerHeartsOverlap.x > rupeeBoundingBox.halfSize.x && playerHeartsOverlap.y > rupeeBoundingBox.halfSize.y)
        {
            //m_game->playSound("GetItem");
            if (rupeeAnim.getName() == "RupeeBlue")
            {
                m_wallet += 5;
                m_walletText.setString("    +5 coins\n  Coins: x" + std::to_string(m_wallet));
            }
            else
            {
                m_wallet++;
                m_walletText.setString("    +1 coin\n   Coins: x" + std::to_string(m_wallet));
            }
            m_walletClock.restart();
            rupee->destroy();
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

    view.setSize(520, 520);
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    Vec2 playerPos = m_player->getComponent<CTransform>().pos;
    sf::Vector2f newCamPos(playerPos.x,playerPos.y);
    if (newCamPos.x < view.getSize().x / 2)
    {
        newCamPos.x = view.getSize().x / 2;
    }

    auto mDiff = center - newCamPos;
    //m_mPos += Vec2(mDiff.x, mDiff.y);
    view.setCenter(newCamPos);
    
    
#pragma region Setting Up strings and positions of UI Texts
   
    m_levelText.setCharacterSize(12);
    Vec2 levelTextPos = Vec2(m_player->getComponent<CTransform>().pos.x - 40, m_player->getComponent<CTransform>().pos.y - 96);
    m_tutorialText.setString(" Move:W, A, S D Weapon Swap: TAB\n Zoom In: O   Zoom Out: P");
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
        //m_tutorialText.setString("");
    }
    if (m_walletClock.getElapsedTime().asSeconds() > 1)
    {
        m_walletText.setString("");
    }

    m_levelText.setPosition(sf::Vector2f(levelTextPos.x, levelTextPos.y + 128));
    m_tutorialText.setPosition(sf::Vector2f(levelTextPos.x - 50, levelTextPos.y - 50));
    m_walletText.setPosition(sf::Vector2f(levelTextPos.x + 5, levelTextPos.y + 15));
#pragma endregion

    // then set the window view
    m_game->window().setView(view);
}

void Scene_MainGame::drawMinimap()
{
    sf::View minimapView = m_game->window().getView();
    minimapView.setViewport(sf::FloatRect(0.74f, 0.01f, 0.25f, 0.25f));
    minimapView.zoom(2.0f);
    m_game->window().setView(minimapView);

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
            std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
            m_gridText.setString("(" + xCell + "," + yCell + ")");
            m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
            m_game->window().draw(m_gridText);
        }
    }

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
     //m_game->stopSound("MusicLevel");
     m_game->playSound("MusicTitle");
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
    tick.setFillColor(sf::Color::Black);
    
    
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        // draw entity animations
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            sf::Color c = sf::Color::White;
            if (e->hasComponent<CInvincibility>())
            {
                c = sf::Color(255, 255, 255, 128);
            }
                           
            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                animation.getSprite().setColor(c);
                m_game->window().draw(animation.getSprite());
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
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
    
    m_game->window().draw(m_tutorialText);
    m_game->window().draw(m_walletText);
    m_game->window().draw(m_levelText);
    if (m_minimap)
    {
        drawMinimap();

    } 
}