#pragma once

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"
#include <map>
#include <memory>
#include <deque>

class Scene_Credits : public Scene
{

protected:

	std::string               m_title;
	std::vector<std::string>  m_menuStrings;
	sf::Text                  m_menuText;
	size_t                    m_selectedMenuIndex = 0;
	Vec2                      menuTextPos = getPosition(0, 0, 6, 1);
	Vec2                      menuOptionsPos = getPosition(0, 0, 2, 4);

	void loadCredits();
	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	void scroll();

public:

	Scene_Credits(GameEngine* game);
	Vec2 getPosition(int sx, int sy, int tx, int ty) const;
	void sRender();

};