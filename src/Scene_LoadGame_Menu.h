#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <map>
#include <deque>
#include <memory>

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"

class Scene_LoadGame_Menu : public Scene
{
protected:

	std::string	              m_title;
	std::vector<std::string>  m_menuStrings;
	std::vector<std::string>  m_levelPaths;
	sf::Text				  m_menuText;
	sf::Sprite                m_backgroundSprite;
	size_t                    m_selectedMenuIndex = 0;
	Vec2                      menuTextPos = getPosition(0, 0, 8, 1);
	Vec2                      menuOptionsPos = getPosition(0, 0, 8, 3);

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:

	Scene_LoadGame_Menu(GameEngine* game);
	Vec2 getPosition(int sx, int sy, int tx, int ty) const;
	void sRender();
};
