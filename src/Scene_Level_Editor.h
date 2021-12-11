
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
    struct MovingTileConfig
    {
        float RX, RY, TX, TY, S, N;
        std::vector<Vec2> XnY;
        bool BM, BV;
        std::string Name, AI;
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
    EntityManager            m_editorManager;
    std::vector<std::string> m_editStrings;
    std::shared_ptr<Entity>  m_selected;
    std::shared_ptr<Entity>  m_editor;
    std::shared_ptr<Entity>  m_player;
    std::string              m_levelPath;
    sf::Text                 m_entityText;
    sf::Text                 m_editText;
    int                      m_select = 0;
    int                      m_max = 1;
    int                      m_editIndex = 0;
    PlayerConfig             m_playerConfig;
    std::string              configRead;
    const Vec2               m_gridSize = { 64, 64 };
    Vec2                     m_mPos = Vec2(0.0f, 0.0f);
    bool                     m_drawGrid = true;
    TileConfig               m_tileConfig;
    NPCConfig                m_npcConfig;
    ItemConfig               m_itemConfig;
    bool                     m_editing = false;
    bool                     m_addingPoint = false;
    bool                     m_removingPoint = false;
    bool                     m_editingPoint = false;
    bool                     m_adding = false;
    bool                     m_drawTextures = true;
    bool                     m_drawCollision = false;
    bool                     m_follow = false;
    bool                     m_teleported = false;
    bool                     m_weaponSwitch = false;
    int                      m_roomX = 0;
    int                      m_roomY = 0;
    sf::Text                 m_gridText;

    void init(const std::string& levelPath);

    void loadLevel(const std::string& filename);
    void saveLevel(const std::string& filename);
    void templateEntities(const std::string& filename);

    void onEnd();
    void update();
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    void drawLine(const Vec2& p1, const Vec2& p2);
    void sDragAndDrop();
    void sEditor();
    void grab();
    void spawnPlayer();
    void editor();
    void snap(std::shared_ptr<Entity> e);
    void draggable();
    void remove();
    void copy();
    void select(float s);
    void text();
    void edit();
    void editText();
    void change(int d);
    void addPoint(Vec2& pos);
    void removePoint();
    void editPoint(Vec2& pos);
    void snapPoint(Vec2& pos);
    void mode();

public:

    Scene_Level_Editor(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);

};