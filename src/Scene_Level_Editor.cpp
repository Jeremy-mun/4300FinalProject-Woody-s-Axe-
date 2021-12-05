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
    registerAction(sf::Keyboard::Q, "PLACE_BLOCK");
    registerAction(sf::Keyboard::V, "SAVE");
    registerAction(sf::Keyboard::B, "REMOVE");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));
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
    editor();
    draggable();
}

void Scene_Level_Editor::saveLevel(const std::string& filename)
{
    std::ofstream config(filename);
    if (config.is_open())
    {
        std::cout << "File is open" << std::endl;
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
                m_npcConfig.TX = ((pos.x - 32) - (m_npcConfig.RX * m_game->window().getSize().x)) / 64.0;
                m_npcConfig.TY = ((pos.y - 32) - (m_npcConfig.RY * m_game->window().getSize().y)) / 64.0;
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
                        config << v.x << "  ";
                        config << v.y << "  ";
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
    }
}

void Scene_Level_Editor::placeTile(Animation animation)
{
    float viewX = m_game->window().getView().getCenter().x - (m_game->window().getSize().x / 2);
    float viewY = m_game->window().getView().getCenter().y - (m_game->window().getSize().y / 2);
    auto mpos = sf::Mouse::getPosition(m_game->window());
    Vec2 gridPos( mpos.x + viewX, mpos.y + viewY);
    
    auto& fakeTile = m_entityManager.addEntity("tile");
    fakeTile->addComponent<CAnimation>(animation, false);
    fakeTile->addComponent<CTransform>(gridPos);
    fakeTile->addComponent<CBoundingBox>(Vec2(animation.getSize().x, animation.getSize().y));
    
    for (auto& e : m_entityManager.getEntities())
    {
        auto overlap = Physics::GetOverlap(fakeTile, e);
        if (overlap.x > 0 && overlap.y > 0)
        {
            fakeTile->destroy();
        }
    }

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
        std::cout << e->tag() << std::endl;
        e->addComponent<CDraggable>();
    }
}

void Scene_Level_Editor::update()
{
    m_entityManager.update();
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
             if (action.name() == "PAUSE")            { }
        else if (action.name() == "QUIT")             { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW")    { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_TEXTURE")   { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID")      { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "UP")               { m_editor->getComponent<CInput>().up = true; }
        else if (action.name() == "DOWN")             { m_editor->getComponent<CInput>().down = true; }
        else if (action.name() == "LEFT")             { m_editor->getComponent<CInput>().left = true; }
        else if (action.name() == "RIGHT")            { m_editor->getComponent<CInput>().right = true; }
        else if (action.name() == "LEFT_CLICK")       { grab(); }
        else if (action.name() == "PLACE_BLOCK")      { }
        else if (action.name() == "SAVE")             { saveLevel(m_levelPath); }
        else if (action.name() == "REMOVE")           { remove(); }
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

}

void Scene_Level_Editor::sDragAndDrop()
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

void Scene_Level_Editor::remove()
{
    if (m_selected != nullptr)
    {
        m_selected->destroy();
        m_editor->getComponent<CDraggable>().dragging = !m_editor->getComponent<CDraggable>().dragging;
    }
}