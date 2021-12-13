#pragma once

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"
#include <map>
#include <memory>
#include <deque>

class Scene_Overworld : public Scene
{

protected:

	std::string               m_title;
	std::string               m_levelPath;
	std::vector<std::string>  m_menuStrings;
	sf::Text                  m_menuText;
	size_t                    m_selectedMenuIndex = 0;
	Vec2                      menuTextPos = getPosition(0, 0, 4, 1);
	Vec2                      menuOptionsPos = getPosition(0, 0, 8, 7);
	bool					  m_level1Completion = false, m_level2Completion = false, m_level3Completion = false;
	int						  m_select = 1;
	std::shared_ptr<Entity>	  m_player;

	//options
	std::string                 ConfigRead;
	int                         m_musicVolume = 100;
	int                         m_effectVolume = 100;

	sf::Sprite             parallax1, parallax2, parallax3, parallax4, parallax5, parallax6, parallax7, parallax8, parallax9, parallax10, parallax11, parallax12, parallax13, parallax14;
	std::vector<sf::String> m_parallaxBackgroundTextures = { "TexOverworldBackground1" , "TexOverworldBackground2" ,"TexOverworldBackground3" ,"TexOverworldBackground4"
													,"TexOverworldBackground5" ,"TexOverworldBackground6","TexOverworldBackground7" 
													,"TexOverworldBackground8" ,"TexOverworldBackground9" ,"TexOverworldBackground10"
													,"TexOverworldBackground11" ,"TexOverworldBackground12","TexParallax3", "TexParallax5"};
	std::vector<sf::Sprite> m_parallaxBackgroundSprites = { parallax1, parallax2, parallax3, parallax4, parallax5, parallax6 , parallax7 , parallax8 , parallax9, parallax10, parallax11, parallax12, parallax13, parallax14 };


	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	void loadBackground();
	void drawBackground();
	void playerMove();

public:

	Scene_Overworld(GameEngine* game, const std::string& levelPath);
	Vec2 getPosition(int sx, int sy, int tx, int ty) const;
	void sRender();

};