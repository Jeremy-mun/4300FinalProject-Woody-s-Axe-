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

class Scene_Level_Editor_Menu : public Scene
{
protected:

	std::string	              m_title;
	std::vector<std::string>  m_menuStrings;
	std::vector<std::string>  m_levelPaths;
	sf::Text				  m_menuText;
	size_t                    m_selectedMenuIndex = 0;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:

	Scene_Level_Editor_Menu(GameEngine* game);

	void sRender();
};
