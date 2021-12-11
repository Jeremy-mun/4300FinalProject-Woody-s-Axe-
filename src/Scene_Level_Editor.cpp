#include "Scene_Level_Editor.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

Scene_Level_Editor::Scene_Level_Editor(GameEngine* game, const std::string& levelPath)
    : Scene(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Level_Editor::init(const std::string& levelPath)
{
    loadLevel(levelPath);
    templateEntities("entities.txt");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");       // Toggle follow camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");         // Toggle drawing (G)rid Boxes
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Q, "PLACE_BLOCK");
    registerAction(sf::Keyboard::V, "SAVE");
    registerAction(sf::Keyboard::B, "REMOVE");
    registerAction(sf::Keyboard::Z, "COPY");
    registerAction(sf::Keyboard::E, "EDIT");
    registerAction(sf::Keyboard::Left, "DECREASE");
    registerAction(sf::Keyboard::Right, "INCREASE");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));
    m_entityText.setCharacterSize(16);
    m_entityText.setFont(m_game->assets().getFont("Arial"));
    m_editText.setCharacterSize(16);
    m_editText.setFont(m_game->assets().getFont("Arial"));
}

void Scene_Level_Editor::loadLevel(const std::string& filename)
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

                auto tile = m_entityManager.addEntity("Tile");
                tile->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                tile->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                continue;
            }
            if (configRead == "Item")
            {
                config >> m_itemConfig.Name >> m_itemConfig.RX >> m_itemConfig.RY >> m_itemConfig.TX >> m_itemConfig.TY >> m_itemConfig.BM >> m_itemConfig.BV;
                if (m_itemConfig.Name == "Coin")
                {
                    auto coin = m_entityManager.addEntity("Coin");
                    coin->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    coin->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    coin->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
                else if (m_itemConfig.Name == "Heart")
                {
                    auto heart = m_entityManager.addEntity("Heart");
                    heart->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    heart->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    heart->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
                else if (m_itemConfig.Name == "BluePotion" || m_itemConfig.Name == "PurplePotion" || m_itemConfig.Name == "GreenPotion" || m_itemConfig.Name == "GoldPotion" || m_itemConfig.Name == "RedPotion")
                {
                    auto potion = m_entityManager.addEntity("Potion");
                    potion->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    potion->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    potion->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    continue;
                }
            }
            if (configRead == "NPC")
            {
                config >> m_npcConfig.Name >> m_npcConfig.RX >> m_npcConfig.RY >> m_npcConfig.TX >> m_npcConfig.TY >> m_npcConfig.BM >> m_npcConfig.BV >> m_npcConfig.H >> m_npcConfig.D >> m_npcConfig.AI >> m_npcConfig.S;
                auto npc = m_entityManager.addEntity("NPC");
                if (m_npcConfig.AI == "Follow")
                {
                    npc->addComponent<CTransform>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY));
                    npc->addComponent<CAnimation>(m_game->assets().getAnimation(m_npcConfig.Name), true);
                    npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_npcConfig.Name).getSize(), m_npcConfig.BM, m_npcConfig.BV);
                    npc->addComponent<CFollowPlayer>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY), m_npcConfig.S);
                    npc->addComponent<CHealth>(m_npcConfig.H, m_npcConfig.H);
                    npc->addComponent<CDamage>(m_npcConfig.D);
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
                    continue;
                }
            }
        }
    }
    spawnPlayer();
    editor();
    draggable();
}

void Scene_Level_Editor::saveLevel(const std::string& filename)
{
    std::ofstream config(filename);
    if (config.is_open())
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->tag() == "editor") continue;

            if (e->tag() == "Tile")
            {
                m_tileConfig.Name = e->getComponent<CAnimation>().animation.getName();
                auto pos = e->getComponent<CTransform>().pos;
                m_tileConfig.RX = floor(pos.x / m_game->window().getView().getSize().x);
                m_tileConfig.RY = floor(pos.y / m_game->window().getView().getSize().y);
                m_tileConfig.TX = ((pos.x - 32) - (m_tileConfig.RX * m_game->window().getSize().x)) / 64.0;
                m_tileConfig.TY = ((pos.y - 32) - (m_tileConfig.RY * m_game->window().getSize().y)) / 64.0;
                m_tileConfig.BM = e->getComponent<CBoundingBox>().blockMove;
                m_tileConfig.BV = e->getComponent<CBoundingBox>().blockVision;

                config << e->tag() << " ";
                config << m_tileConfig.Name << "    ";
                config << m_tileConfig.RX << "  ";
                config << m_tileConfig.RY << "  ";
                config << m_tileConfig.TX << "  ";
                config << m_tileConfig.TY << "  ";
                config << m_tileConfig.BM << "  ";
                config << m_tileConfig.BV << "  ";
                config << std::endl;
                continue;
            }
            else if (e->tag() == "Coin" ||
                     e->tag() == "Heart" ||
                     e->tag() == "Potion")
            {
                if (e->tag() == "Potion")
                {
                    m_itemConfig.Name = e->getComponent<CAnimation>().animation.getName();
                }
                else
                {
                    m_itemConfig.Name = e->tag();
                }
                auto pos = e->getComponent<CTransform>().pos;
                m_itemConfig.RX = floor(pos.x / m_game->window().getView().getSize().x);
                m_itemConfig.RY = floor(pos.y / m_game->window().getView().getSize().y);
                m_itemConfig.TX = ((pos.x - 32) - (m_itemConfig.RX * m_game->window().getSize().x)) / 64.0;
                m_itemConfig.TY = ((pos.y - 32) - (m_itemConfig.RY * m_game->window().getSize().y)) / 64.0;
                m_itemConfig.BM = e->getComponent<CBoundingBox>().blockMove;
                m_itemConfig.BV = e->getComponent<CBoundingBox>().blockVision;

                config << "Item ";
                config << m_itemConfig.Name << "    ";
                config << m_itemConfig.RX << "  ";
                config << m_itemConfig.RY << "  ";
                config << m_itemConfig.TX << "  ";
                config << m_itemConfig.TY << "  ";
                config << m_itemConfig.BM << "  ";
                config << m_itemConfig.BV << "  ";
                config << std::endl;
                continue;
            }
            else if (e->tag() == "NPC")
            {
                m_npcConfig.Name = e->getComponent<CAnimation>().animation.getName();
                auto pos = e->getComponent<CTransform>().pos;
                m_npcConfig.RX = floor(pos.x / m_game->window().getView().getSize().x);
                m_npcConfig.RY = floor(pos.y / m_game->window().getView().getSize().y);
                m_npcConfig.TX = ((int(pos.x) - 32) - (m_npcConfig.RX * m_game->window().getSize().x)) / 64;
                m_npcConfig.TY = ((int(pos.y) - 32) - (m_npcConfig.RY * m_game->window().getSize().y)) / 64;
                m_npcConfig.BM = e->getComponent<CBoundingBox>().blockMove;
                m_npcConfig.BV = e->getComponent<CBoundingBox>().blockVision;
                m_npcConfig.H = e->getComponent<CHealth>().max;
                m_npcConfig.D = e->getComponent<CDamage>().damage;
                if (e->hasComponent<CFollowPlayer>())
                {
                    m_npcConfig.AI = "Follow";
                    m_npcConfig.S = e->getComponent<CFollowPlayer>().speed;
                }
                else if (e->hasComponent<CPatrol>())
                {
                    m_npcConfig.AI = "Patrol";
                    m_npcConfig.S = e->getComponent<CPatrol>().speed;
                    m_npcConfig.N = e->getComponent<CPatrol>().positions.size();
                }

                config << e->tag() << " ";
                config << m_npcConfig.Name << "     ";
                config << m_npcConfig.RX << "  ";
                config << m_npcConfig.RY << "  ";
                config << m_npcConfig.TX << "  ";
                config << m_npcConfig.TY << "  ";
                config << m_npcConfig.BM << "  ";
                config << m_npcConfig.BV << "  ";
                config << m_npcConfig.H  << "  ";
                config << m_npcConfig.D  << "  ";
                config << m_npcConfig.AI << "  ";
                config << m_npcConfig.S << "  ";
                if (e->hasComponent<CPatrol>())
                {
                    config << m_npcConfig.N << "  ";
                    for (auto v : e->getComponent<CPatrol>().positions)
                    {
                        config << ((int(v.x) - 32) - (m_npcConfig.RX * m_game->window().getSize().x)) / 64 << "  ";
                        config << ((int(v.y) - 32) - (m_npcConfig.RY * m_game->window().getSize().y)) / 64 << "  ";
                    }
                }
                config << std::endl;
                continue;
            }
            else if (e->tag() == "Player")
            {
                m_playerConfig.X = m_player->getComponent<CTransform>().pos.x;
                m_playerConfig.Y = m_player->getComponent<CTransform>().pos.y;
                config << e->tag() << " ";
                config << m_playerConfig.X << " ";
                config << m_playerConfig.Y << " ";
                config << m_playerConfig.CX << " ";
                config << m_playerConfig.CY << " ";
                config << m_playerConfig.SPEED << " ";
                config << m_playerConfig.HEALTH << " ";
                config << m_playerConfig.GRAVITY << " ";
                config << std::endl;
                continue;
            }
        }
        std::ifstream ifs(filename);
        std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        str.erase(std::remove(str.end() - 3, str.end(), ' '), str.end());
        str.erase(std::remove(str.end() - 1, str.end(), '\n'), str.end());
        std::ofstream ofs(filename);
        ofs << str;
        std::cout << "Level is saved" << std::endl;
    }
}

void Scene_Level_Editor::templateEntities(const std::string& filename)
{
    m_editorManager = EntityManager();

    std::ifstream config(filename);
    if (config.is_open())
    {
        while (config.good())
        {
            config >> configRead;

            if (configRead == "None")
            {
                auto blank = m_editorManager.addEntity("None");
            }
            if (configRead == "Tile")
            {
                config >> m_tileConfig.Name >> m_tileConfig.RX >> m_tileConfig.RY >> m_tileConfig.TX >> m_tileConfig.TY >> m_tileConfig.BM >> m_tileConfig.BV;

                auto tile = m_editorManager.addEntity("Tile");
                tile->addComponent<CTransform>(getPosition(m_tileConfig.RX, m_tileConfig.RY, m_tileConfig.TX, m_tileConfig.TY));
                tile->addComponent<CAnimation>(m_game->assets().getAnimation(m_tileConfig.Name), true);
                tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_tileConfig.Name).getSize(), m_tileConfig.BM, m_tileConfig.BV);
                tile->addComponent<CDraggable>();
                continue;
            }
            if (configRead == "Item")
            {
                config >> m_itemConfig.Name >> m_itemConfig.RX >> m_itemConfig.RY >> m_itemConfig.TX >> m_itemConfig.TY >> m_itemConfig.BM >> m_itemConfig.BV;
                if (m_itemConfig.Name == "Coin")
                {
                    auto coin = m_editorManager.addEntity("Coin");
                    coin->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    coin->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    coin->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    coin->addComponent<CDraggable>();
                    continue;
                }
                else if (m_itemConfig.Name == "Heart")
                {
                    auto heart = m_editorManager.addEntity("Heart");
                    heart->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    heart->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    heart->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    heart->addComponent<CDraggable>();
                    continue;
                }
                else if (m_itemConfig.Name == "BluePotion" || m_itemConfig.Name == "PurplePotion" || m_itemConfig.Name == "GreenPotion" || m_itemConfig.Name == "GoldPotion" || m_itemConfig.Name == "RedPotion")
                {
                    auto potion = m_editorManager.addEntity("Potion");
                    potion->addComponent<CTransform>(getPosition(m_itemConfig.RX, m_itemConfig.RY, m_itemConfig.TX, m_itemConfig.TY));
                    potion->addComponent<CAnimation>(m_game->assets().getAnimation(m_itemConfig.Name), true);
                    potion->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_itemConfig.Name).getSize(), m_itemConfig.BM, m_itemConfig.BV);
                    potion->addComponent<CDraggable>();
                    continue;
                }
            }
            if (configRead == "NPC")
            {
                config >> m_npcConfig.Name >> m_npcConfig.RX >> m_npcConfig.RY >> m_npcConfig.TX >> m_npcConfig.TY >> m_npcConfig.BM >> m_npcConfig.BV >> m_npcConfig.H >> m_npcConfig.D >> m_npcConfig.AI >> m_npcConfig.S;
                auto npc = m_editorManager.addEntity("NPC");
                npc->addComponent<CDraggable>();
                if (m_npcConfig.AI == "Follow")
                {
                    npc->addComponent<CTransform>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY));
                    npc->addComponent<CAnimation>(m_game->assets().getAnimation(m_npcConfig.Name), true);
                    npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_npcConfig.Name).getSize(), m_npcConfig.BM, m_npcConfig.BV);
                    npc->addComponent<CFollowPlayer>(getPosition(m_npcConfig.RX, m_npcConfig.RY, m_npcConfig.TX, m_npcConfig.TY), m_npcConfig.S);
                    npc->addComponent<CHealth>(m_npcConfig.H, m_npcConfig.H);
                    npc->addComponent<CDamage>(m_npcConfig.D);
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
                    continue;
                }
            }
        }
    }
    m_editorManager.update();
    m_max = m_editorManager.getEntities().size();
}

void Scene_Level_Editor::spawnPlayer()
{
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CState>("StandDown");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CDamage>(1);
    m_player->addComponent<CInventory>();
}

void Scene_Level_Editor::editor()
{
    m_editor = m_entityManager.addEntity("editor");
    m_editor->addComponent<CInput>();
}

void Scene_Level_Editor::draggable()
{
    m_entityManager.update();
    for (auto& e : m_entityManager.getEntities())
    {
        e->addComponent<CDraggable>();
    }
}

void Scene_Level_Editor::update()
{
    m_entityManager.update();
    m_editorManager.update();
    sEditor();
    sDragAndDrop();
    m_currentFrame++;
}

Vec2 Scene_Level_Editor::getPosition(int rx, int ry, int tx, int ty) const
{
    float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
    float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
    return Vec2(x, y);
}

void Scene_Level_Editor::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
             if (action.name() == "QUIT")             { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW")    { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_TEXTURE")   { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID")      { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "UP")               { m_editor->getComponent<CInput>().up = true; }
        else if (action.name() == "DOWN")             { m_editor->getComponent<CInput>().down = true; }
        else if (action.name() == "LEFT")             { m_editor->getComponent<CInput>().left = true; }
        else if (action.name() == "RIGHT")            { m_editor->getComponent<CInput>().right = true; }
        else if (action.name() == "LEFT_CLICK")       { if (!m_adding && !m_editing) { grab(); } else if (m_adding) { copy(); } else if (m_editing) { mode(); if (m_addingPoint) { addPoint(m_mPos); } else if (m_editingPoint) { editPoint(m_mPos); } } }
        else if (action.name() == "SAVE")             { saveLevel(m_levelPath); }
        else if (action.name() == "REMOVE")           { mode(); if (m_removingPoint) { removePoint(); } else if (!m_adding && !m_editing) { remove(); } }
        else if (action.name() == "SCROLL")           { if (m_editing) { if (action.pos().x < 0) { m_editIndex = (m_editIndex + 1) % m_editStrings.size(); } else if (action.pos().x > 0) { if (m_editIndex > 0) { m_editIndex--; } else { m_editIndex = m_editStrings.size() - 1; } } } else { select(action.pos().x); } }
        else if (action.name() == "COPY")             { copy(); }
        else if (action.name() == "EDIT")             { if (!m_adding && m_selected != nullptr) { m_editing = !m_editing; if (m_editing) { m_editIndex = 0; edit(); } else { m_editStrings.clear(); } } }
        else if (action.name() == "INCREASE")         { if (m_editing) { change(1); } }
        else if (action.name() == "DECREASE")         { if (m_editing) { change(-1); } }
    }
    else if (action.type() == "END")
    {
             if (action.name() == "UP")               { m_editor->getComponent<CInput>().up = false; }
        else if (action.name() == "DOWN")             { m_editor->getComponent<CInput>().down = false; }
        else if (action.name() == "LEFT")             { m_editor->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT")            { m_editor->getComponent<CInput>().right = false; }
    }

    if (action.name() == "MOUSE_MOVE")                { m_mPos = action.pos() + Vec2(m_game->window().getView().getCenter().x - m_game->window().getView().getSize().x / 2, m_game->window().getView().getCenter().y - m_game->window().getView().getSize().y / 2); }
}

void Scene_Level_Editor::grab()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() && 
            Physics::IsInside(m_mPos, e))
        {
            if (!m_editor->getComponent<CDraggable>().dragging)
            {
                m_editor->getComponent<CDraggable>().dragging = !m_editor->getComponent<CDraggable>().dragging;
                e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
                m_selected = e;
                break;
            }
            else if (e->getComponent<CDraggable>().dragging)
            {
                m_editor->getComponent<CDraggable>().dragging = !m_editor->getComponent<CDraggable>().dragging;
                e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
                m_selected = nullptr;
                snap(e);
                break;
            }
        }
    }
}

void Scene_Level_Editor::snap(std::shared_ptr<Entity> e)
{
    auto& pos = e->getComponent<CTransform>().pos;
    pos.x = (floor(pos.x / m_gridSize.x) * m_gridSize.x) + m_gridSize.x / 2;
    pos.y = (floor(pos.y / m_gridSize.y) * m_gridSize.y) + m_gridSize.y / 2;
    if (e->hasComponent<CPatrol>())
    {
        e->getComponent<CPatrol>().positions[0] = pos;
    }
}

void Scene_Level_Editor::sEditor()
{
    auto center = m_game->window().getView().getCenter();

    sf::View view = m_game->window().getView();
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

    sf::Vector2f newView = sf::Vector2f(0.0, 0.0);

    if (m_editor->getComponent<CInput>().up)
    {
        newView.y -= 64.0f;
        m_mPos.y  -= 64.0f;
        m_editor->getComponent<CInput>().up = false;
    }
    if (m_editor->getComponent<CInput>().down)
    {
        newView.y += 64.0f;
        m_mPos.y  += 64.0f;
        m_editor->getComponent<CInput>().down = false;
    }
    if (m_editor->getComponent<CInput>().left)
    {
        newView.x -= 64.0f;
        m_mPos.x  -= 64.0f;
        m_editor->getComponent<CInput>().left = false;
    }
    if (m_editor->getComponent<CInput>().right)
    {
        newView.x += 64.0f;
        m_mPos.x  += 64.0f;
        m_editor->getComponent<CInput>().right = false;
    }

    view.setCenter(newView + center);

    m_game->window().setView(view);
}

void Scene_Level_Editor::onEnd()
{
    m_game->changeScene("Level_Editor_Menu", nullptr, true);
}

void Scene_Level_Editor::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
    m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Level_Editor::sRender()
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
        if (m_selected != nullptr && m_adding)
        {
            m_selected->getComponent<CAnimation>().animation.getSprite().setPosition(m_selected->getComponent<CTransform>().pos.x, m_selected->getComponent<CTransform>().pos.y);
            m_game->window().draw(m_selected->getComponent<CAnimation>().animation.getSprite());
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
        dot.setFillColor(sf::Color::Yellow);
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
    lines[0].color = sf::Color::Green;
    lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
    lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
    lines[1].color = sf::Color::Green;
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

    text();
    editText();
}

void Scene_Level_Editor::sDragAndDrop()
{
    if (m_selected != nullptr)
    {
        if (m_selected->hasComponent<CDraggable>() && 
            m_selected->getComponent<CDraggable>().dragging)
        {
            m_selected->getComponent<CTransform>().pos = m_mPos;
            if (m_selected->hasComponent<CPatrol>())
            {
                m_selected->getComponent<CPatrol>().positions[0] = m_mPos;
            }
        }
    }
}

void Scene_Level_Editor::remove()
{
    if (m_selected != nullptr && !m_adding)
    {
        m_selected->destroy();
        m_editor->getComponent<CDraggable>().dragging = false;
        m_selected = nullptr;
    }
}

void Scene_Level_Editor::copy()
{
    if (m_selected != nullptr)
    {
        auto copy = m_entityManager.addEntity(m_selected->tag());
        if (m_selected->hasComponent<CAnimation>())
        {
            copy->addComponent<CAnimation>(m_selected->getComponent<CAnimation>().animation, m_selected->getComponent<CAnimation>().repeat);
        }
        if (m_selected->hasComponent<CTransform>())
        {
            copy->addComponent<CTransform>(m_selected->getComponent<CTransform>().pos);
        }
        if (m_selected->hasComponent<CBoundingBox>())
        {
            copy->addComponent<CBoundingBox>(m_selected->getComponent<CBoundingBox>().size, m_selected->getComponent<CBoundingBox>().blockMove, m_selected->getComponent<CBoundingBox>().blockVision);
        }
        if (m_selected->hasComponent<CLifeSpan>())
        {
            copy->addComponent<CLifeSpan>(m_selected->getComponent<CLifeSpan>().lifespan, m_selected->getComponent<CLifeSpan>().frameCreated);
        }
        if (m_selected->hasComponent<CDamage>())
        {
            copy->addComponent<CDamage>(m_selected->getComponent<CDamage>().damage);
        }
        if (m_selected->hasComponent<CInvincibility>())
        {
            copy->addComponent<CInvincibility>(m_selected->getComponent<CInvincibility>().iframes);
        }
        if (m_selected->hasComponent<CHealth>())
        {
            copy->addComponent<CHealth>(m_selected->getComponent<CHealth>().max, m_selected->getComponent<CHealth>().current);
        }
        if (m_selected->hasComponent<CGravity>())
        {
            copy->addComponent<CGravity>(m_selected->getComponent<CGravity>().gravity);
        }
        if (m_selected->hasComponent<CInput>())
        {
            copy->addComponent<CInput>();
        }
        if (m_selected->hasComponent<CState>())
        {
            copy->addComponent<CState>(m_selected->getComponent<CState>().state);
        }
        if (m_selected->hasComponent<CInventory>())
        {
            copy->addComponent<CInventory>(m_selected->getComponent<CInventory>().money);
        }
        if (m_selected->hasComponent<CFollowPlayer>())
        {
            copy->addComponent<CFollowPlayer>(m_selected->getComponent<CFollowPlayer>().home, m_selected->getComponent<CFollowPlayer>().speed);
        }
        if (m_selected->hasComponent<CPatrol>())
        {
            copy->addComponent<CPatrol>(m_selected->getComponent<CPatrol>().positions, m_selected->getComponent<CPatrol>().speed);
        }
        copy->addComponent<CDraggable>();
        snap(copy);
    }
}

void Scene_Level_Editor::select(float s)
{
    if (s > 0)
    {
        if (m_selected == nullptr)
        {
            m_select = (m_select + 1) % m_max;
            if (m_select == 0)
            {
                m_adding = false;
                m_editor->getComponent<CDraggable>().dragging = false;
            }
            else
            {
                m_adding = true;
                m_editor->getComponent<CDraggable>().dragging = true;
                m_selected = m_editorManager.getEntities().at(m_select);
                m_selected->getComponent<CDraggable>().dragging = true;
            }
        }
        else if (m_adding == true)
        {
            m_select = (m_select + 1) % m_max;
            m_selected->getComponent<CDraggable>().dragging = false;
            if (m_select == 0)
            {
                m_adding = false;
                m_editor->getComponent<CDraggable>().dragging = false;
                m_selected = nullptr;
            }
            else
            {
                m_selected = m_editorManager.getEntities().at(m_select);
                m_selected->getComponent<CDraggable>().dragging = true;
            }
        }
    }
    else if (s < 0)
    {
        if (m_selected == nullptr)
        {
            m_select = m_max - 1;

            if (m_select == 0)
            {
                m_adding = false;
            }
            else
            {
                m_adding = true;
                m_editor->getComponent<CDraggable>().dragging = true;
                m_selected = m_editorManager.getEntities().at(m_select);
                m_selected->getComponent<CDraggable>().dragging = true;
            }
        }
        else if (m_adding == true)
        {
            if (m_select > 0) { m_select--; }
            else { m_select = m_max - 1; }

            m_selected->getComponent<CDraggable>().dragging = false;
            if (m_select == 0)
            {
                m_adding = false;
                m_editor->getComponent<CDraggable>().dragging = false;
                m_selected = nullptr;
            }
            else
            {
                m_selected = m_editorManager.getEntities().at(m_select);
                m_selected->getComponent<CDraggable>().dragging = true;
            }
        }
    }
}

void Scene_Level_Editor::text()
{
    if (m_selected != nullptr && m_adding)
    {
        std::string name = "";
        name.append(m_selected->getComponent<CAnimation>().animation.getName());
        if (m_selected->tag() == "NPC")
        {
            if (m_selected->hasComponent<CPatrol>())
            {
                name.append(" Patrol");
            }
            else if (m_selected->hasComponent<CFollowPlayer>())
            {
                name.append(" Follow");
            }
        }
        m_entityText.setPosition(m_mPos.x + m_selected->getComponent<CBoundingBox>().halfSize.x + 16, m_mPos.y);
        m_entityText.setString(name);
    }
    else
    {
        m_entityText.setString("");
    }
    m_game->window().draw(m_entityText);
}

void Scene_Level_Editor::edit()
{
    m_editStrings.clear();
    std::string line = "";
    line = "BlockMove ";
    line.append(m_selected->getComponent<CBoundingBox>().blockMove ? "true" : "false");
    m_editStrings.push_back(line);
    line = "BlockVision ";
    line.append(m_selected->getComponent<CBoundingBox>().blockVision ? "true" : "false");
    m_editStrings.push_back(line);
    if (m_selected->hasComponent<CHealth>())
    {
        line = "Health " + std::to_string(m_selected->getComponent<CHealth>().max);
        m_editStrings.push_back(line);
    }
    if (m_selected->hasComponent<CDamage>())
    {
        line = "Damage " + std::to_string(m_selected->getComponent<CDamage>().damage);
        m_editStrings.push_back(line);
    }
    if (m_selected->hasComponent<CGravity>())
    {
        line = "Gravity " + std::to_string(m_selected->getComponent<CGravity>().gravity);
        m_editStrings.push_back(line);
    }
    if (m_selected->hasComponent<CFollowPlayer>())
    {
        line = "Follow ";
        m_editStrings.push_back(line);
        line = "FollowSpeed " + std::to_string(m_selected->getComponent<CFollowPlayer>().speed);
        m_editStrings.push_back(line);
    }
    if (m_selected->hasComponent<CPatrol>())
    {
        line = "Patrol ";
        m_editStrings.push_back(line);
        line = "PatrolSpeed " + std::to_string(m_selected->getComponent<CPatrol>().speed);
        m_editStrings.push_back(line);
        line = "PatrolPositions Add";
        m_editStrings.push_back(line);
        line = "PatrolPositions Remove";
        m_editStrings.push_back(line);
        for (int i = 0; i < int(m_selected->getComponent<CPatrol>().positions.size()); i++)
        {
            int rx = floor(int(m_selected->getComponent<CPatrol>().positions[i].x) / m_game->window().getView().getSize().x);
            int ry = floor(int(m_selected->getComponent<CPatrol>().positions[i].y) / m_game->window().getView().getSize().y);;
            int x = ((int(m_selected->getComponent<CPatrol>().positions[i].x) - 32) - (rx * m_game->window().getSize().x)) / 64;
            int y = ((int(m_selected->getComponent<CPatrol>().positions[i].y) - 32) - (ry * m_game->window().getSize().y)) / 64;
            line = "PatrolPositions Point " + std::to_string(i) + " (" + std::to_string(x) + ", " + std::to_string(y) + ")";
            m_editStrings.push_back(line);
        }
    }
}

void Scene_Level_Editor::editText()
{
    if (m_editing)
    {
        m_editText.setString(m_editStrings[m_editIndex]);
        m_editText.setPosition(m_mPos.x + m_selected->getComponent<CBoundingBox>().halfSize.x + 16, m_mPos.y);
    }
    else
    {
        m_editText.setString("");
    }
    m_game->window().draw(m_editText);
}

void Scene_Level_Editor::change(int d)
{
    std::stringstream ss;
    std::string word;
    ss << m_editStrings[m_editIndex];
    ss >> word;

    if (word == "BlockMove")
    {
        m_selected->getComponent<CBoundingBox>().blockMove = !m_selected->getComponent<CBoundingBox>().blockMove;
    }
    else if (word == "BlockVision")
    {
        m_selected->getComponent<CBoundingBox>().blockVision = !m_selected->getComponent<CBoundingBox>().blockVision;
    }
    else if (word == "Health")
    {
        if (d > 0)
        {
            m_selected->getComponent<CHealth>().max++;
            m_selected->getComponent<CHealth>().current = m_selected->getComponent<CHealth>().max;
        }
        else if (d < 0)
        {
            if (m_selected->getComponent<CHealth>().max > 0)
            {
                m_selected->getComponent<CHealth>().max--;
                m_selected->getComponent<CHealth>().current = m_selected->getComponent<CHealth>().max;
            }
        }
    }
    else if (word == "Damage")
    {
        if (d > 0)
        {
            m_selected->getComponent<CDamage>().damage++;
        }
        else if (d < 0)
        {
            if (m_selected->getComponent<CDamage>().damage > 0)
            {
                m_selected->getComponent<CDamage>().damage--;
            }
        }
    }
    else if (word == "Gravity")
    {
        if (d > 0)
        {
            m_selected->getComponent<CGravity>().gravity++;
        }
        else if (d < 0)
        {
            if (m_selected->getComponent<CGravity>().gravity > 0)
            {
                m_selected->getComponent<CGravity>().gravity--;
            }
        }
    }
    else if (word == "Follow")
    {
        std::vector<Vec2> home;
        home.push_back(m_selected->getComponent<CFollowPlayer>().home);
        m_selected->addComponent<CPatrol>(home, m_selected->getComponent<CFollowPlayer>().speed);
        m_selected->removeComponent<CFollowPlayer>();
    }
    else if (word == "FollowSpeed")
    {
        if (d > 0)
        {
            m_selected->getComponent<CFollowPlayer>().speed++;
        }
        else if (d < 0)
        {
            if (m_selected->getComponent<CFollowPlayer>().speed > 0)
            {
                m_selected->getComponent<CFollowPlayer>().speed--;
            }
        }
    }
    else if (word == "Patrol")
    {
        m_selected->addComponent<CFollowPlayer>(m_selected->getComponent<CPatrol>().positions[0], m_selected->getComponent<CPatrol>().speed);
        m_selected->removeComponent<CPatrol>();
    }
    else if (word == "PatrolSpeed")
    {
        if (d > 0)
        {
            m_selected->getComponent<CPatrol>().speed++;
        }
        else if (d < 0)
        {
            if (m_selected->getComponent<CPatrol>().speed > 0)
            {
                m_selected->getComponent<CPatrol>().speed--;
            }
        }
    }
    edit();
}

void Scene_Level_Editor::addPoint(Vec2& pos)
{
    snapPoint(pos);
    m_selected->getComponent<CPatrol>().positions.push_back(pos);
    edit();
}

void Scene_Level_Editor::removePoint()
{
    m_selected->getComponent<CPatrol>().positions.pop_back();
    edit();
}

void Scene_Level_Editor::editPoint(Vec2& pos)
{
    snapPoint(pos);
    m_selected->getComponent<CPatrol>().positions[m_selected->getComponent<CPatrol>().currentPosition] = pos;
    edit();
}

void Scene_Level_Editor::snapPoint(Vec2& pos)
{
    pos.x = (floor(pos.x / m_gridSize.x) * m_gridSize.x) + m_gridSize.x / 2;
    pos.y = (floor(pos.y / m_gridSize.y) * m_gridSize.y) + m_gridSize.y / 2;
}

void Scene_Level_Editor::mode()
{
    m_addingPoint = false;
    m_removingPoint = false;
    m_editingPoint = false;
    std::stringstream ss;
    std::string word;
    ss << m_editStrings[m_editIndex];
    ss >> word;

    if (word == "PatrolPositions")
    {
        ss >> word;

        if (word == "Add")
        {
            m_addingPoint = true;
        }
        else if (word == "Remove")
        {
            m_removingPoint = true;
        }
        else if (word == "Point")
        {
            m_editingPoint = true;
            ss >> word;
            int i = stoi(word);
            m_selected->getComponent<CPatrol>().currentPosition = i;
        }
    }
}