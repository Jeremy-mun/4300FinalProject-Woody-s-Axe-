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
                config >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.HEALTH;
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
        spawnPlayer();
    }
    editor();
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
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CState>("StandDown");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CDraggable>();
}

void Scene_Level_Editor::editor()
{
    m_editor = m_entityManager.addEntity("editor");
    m_editor->addComponent<CInput>();
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
    }
    else if (action.type() == "END")
    {
             if (action.name() == "UP")               { m_editor->getComponent<CInput>().up = false; }
        else if (action.name() == "DOWN")             { m_editor->getComponent<CInput>().down = false; }
        else if (action.name() == "LEFT")             { m_editor->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT")            { m_editor->getComponent<CInput>().right = false; }
    }

    if (action.name() == "MOUSE_MOVE")                { m_mPos = action.pos() + Vec2(m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2, m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2); }
}

void Scene_Level_Editor::grab()
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
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

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

