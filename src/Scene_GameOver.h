#pragma once

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"
#include <map>
#include <memory>
#include <deque>

class Scene_GameOver : public Scene
{

protected:

	std::string               m_title;
	std::string               m_levelPath;
	std::string               m_saveFile;
	std::vector<std::string>  m_menuStrings;
	sf::Text				  m_shopText;
	sf::Text                  m_menuText;
	size_t                    m_selectedMenuIndex = 0;
	Vec2                      menuTextPos = getPosition(0, 0, 4, 1);
	Vec2                      menuOptionsPos = getPosition(0, 0, 8, 7);

	//options
	std::string                 ConfigRead;
	int                         m_musicVolume = 100;
	int                         m_effectVolume = 100;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:

	Scene_GameOver(GameEngine* game, const std::string& levelPath, const std::string& saveFile);
	Vec2 getPosition(int sx, int sy, int tx, int ty) const;
	void sRender();

};