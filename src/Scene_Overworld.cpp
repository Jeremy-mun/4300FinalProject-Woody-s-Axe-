
#include "GameEngine.h"
#include "Action.h"
#include "Components.h"
#include "Common.h"
#include "Assets.h"
#include "Scene_Overworld.h"
#include "Scene_MainGame.h"

Scene_Overworld::Scene_Overworld(GameEngine* game, const std::string& saveFile)
	:Scene(game)
	, m_saveFile(saveFile)
{
	init();
}

void Scene_Overworld::init()
{
	registerAction(sf::Keyboard::F, "INTERACT");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Escape, "QUIT");

	std::ifstream config("options.txt");
	if (config.is_open())
	{
		while (config.good())
		{
			// Using the getPosition() function below to convert room-tile coords to game world coords
			//set variables equal to their values from the config file.
			config >> ConfigRead;
			if (ConfigRead == "MusicVolume")
			{
				config >> m_musicVolume;
			}
			else if (ConfigRead == "SFXVolume")
			{
				config >> m_effectVolume;
			}
		}
	}

	loadSaveGame();
	
	loadBackground();

	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true);
	if (m_select == 1)
	{
		m_player->addComponent<CTransform>(Vec2(352, 656));
	}

	auto& level1Tile = m_entityManager.addEntity("Leveltiles");
	level1Tile->addComponent<CTransform>(Vec2(352, 672));
	auto& level2Tile = m_entityManager.addEntity("Leveltiles");
	level2Tile->addComponent<CTransform>(Vec2(672, 672));
	auto& level3Tile = m_entityManager.addEntity("Leveltiles");
	level3Tile->addComponent<CTransform>(Vec2(992, 672));
	if (m_level1Completion)
	{
		if (m_level2Completion)
		{
			if (m_level3Completion)
			{
				level1Tile->addComponent<CAnimation>(m_game->assets().getAnimation("CompletedLevel"), true);
				level2Tile->addComponent<CAnimation>(m_game->assets().getAnimation("CompletedLevel"), true);
				level3Tile->addComponent<CAnimation>(m_game->assets().getAnimation("CompletedLevel"), true);
			}
			else
			{
				level1Tile->addComponent<CAnimation>(m_game->assets().getAnimation("CompletedLevel"), true);
				level2Tile->addComponent<CAnimation>(m_game->assets().getAnimation("CompletedLevel"), true);
				level3Tile->addComponent<CAnimation>(m_game->assets().getAnimation("UnlockedLevel"), true);
			}
		}
		else
		{
			level1Tile->addComponent<CAnimation>(m_game->assets().getAnimation("CompletedLevel"), true);
			level2Tile->addComponent<CAnimation>(m_game->assets().getAnimation("UnlockedLevel"), true);
			level3Tile->addComponent<CAnimation>(m_game->assets().getAnimation("LockedLevel"), true);
		}
		
	}
	else
	{
		level1Tile->addComponent<CAnimation>(m_game->assets().getAnimation("UnlockedLevel"), true);
		level2Tile->addComponent<CAnimation>(m_game->assets().getAnimation("LockedLevel"), true);
		level3Tile->addComponent<CAnimation>(m_game->assets().getAnimation("LockedLevel"), true);
	}

	m_menuText.setFont(m_game->assets().getFont("Gypsy"));
	m_menuText.setCharacterSize(64);

	m_game->playSound("MusicOverworld");
	m_game->setVolume("MusicOverworld", m_musicVolume);

}

void Scene_Overworld::saveGame()
{
	std::ofstream save(m_saveFile);
	if (save.is_open())
	{	
		save << "Level1   ";
		save << m_level1Completion;
		save << std::endl;
		save << "Level2   ";
		save << m_level2Completion;
		save << std::endl;
		save << "Level3   ";
		save << m_level3Completion;
		save << std::endl;
		save << "Coins     ";
		save << m_coinCount;
		save << std::endl;
		save << "MaxHealth ";
		save << m_maxHealth;
		save << std::endl;
		save << "MaxArrows ";
		save << m_arrowCount;
		save << std::endl;
		save << "Damage   ";
		save << m_damage;
		save << std::endl;
	}
}

void Scene_Overworld::loadSaveGame()
{
	std::ifstream save(m_saveFile);
	if (save.is_open())
	{
		while (save.good())
		{
			
			save >> m_saveRead;
			if (m_saveRead == "Level1")
			{
				save >> m_level1Completion;
			}
			else if (m_saveRead == "Level2")
			{
				save >> m_level2Completion;
			}
			else if (m_saveRead == "Level3")
			{
				save >> m_level3Completion;
			}
			else if (m_saveRead == "Coins")
			{
				save >> m_coinCount;
			}
			else if (m_saveRead == "MaxHealth")
			{
				save >> m_maxHealth;
			}
			else if (m_saveRead == "Damage")
			{
				save >> m_damage;
			}
			else if (m_saveRead == "MaxArrows")
			{
				save >> m_arrowCount;
			}
		}
	}
}

void Scene_Overworld::update()
{
	m_entityManager.update();
	playerMove();
}

void Scene_Overworld::onEnd()
{
	m_game->stopSound("MusicOverworld");
	m_game->playSound("MusicTitle");
	m_game->setVolume("MusicTitle", m_musicVolume);
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Overworld::playerMove()
{
	std::cout << m_select << '\n';
	if (m_select == 1)
	{
		if (m_player->getComponent<CTransform>().pos == Vec2(352, 656))
		{
			if(m_player->getComponent<CAnimation>().animation.getName() != "StandUp")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("StandUp");
			}
		}
		else
		{
			m_player->getComponent<CTransform>().pos.x -= 4;
			if (m_player->getComponent<CAnimation>().animation.getName() != "RunRight")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("RunRight");
				m_player->getComponent<CTransform>().scale.x = -1;
			}
			else
			{
				m_player->getComponent<CTransform>().scale.x = -1;
			}
		}
	}
	if (m_select == 2)
	{
		if (m_player->getComponent<CTransform>().pos == Vec2(672, 656))
		{
			if (m_player->getComponent<CAnimation>().animation.getName() != "StandUp")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("StandUp");
			}
		}
		else
		{
			if (m_player->getComponent<CTransform>().pos.x > 672)
			{
				m_player->getComponent<CTransform>().pos.x -= 4;
				if (m_player->getComponent<CAnimation>().animation.getName() != "RunRight")
				{
					m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("RunRight");
					m_player->getComponent<CTransform>().scale.x = -1;
				}
				else
				{
					m_player->getComponent<CTransform>().scale.x = -1;
				}
			}
			else
			{
				m_player->getComponent<CTransform>().pos.x += 4;
				if (m_player->getComponent<CAnimation>().animation.getName() != "RunRight")
				{
					m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("RunRight");
					m_player->getComponent<CTransform>().scale.x = 1;
				}
				else
				{
					m_player->getComponent<CTransform>().scale.x = 1;
				}
			}
		}
	}
	if (m_select == 3)
	{
		if (m_player->getComponent<CTransform>().pos == Vec2(992, 656))
		{
			if (m_player->getComponent<CAnimation>().animation.getName() != "StandUp")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("StandUp");
			}
		}
		else
		{
			
			m_player->getComponent<CTransform>().pos.x += 4;
			if (m_player->getComponent<CAnimation>().animation.getName() != "RunRight")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("RunRight");
				m_player->getComponent<CTransform>().scale.x = 1;
			}
			else
			{
				m_player->getComponent<CTransform>().scale.x = 1;
			}
		}
	}
}

void Scene_Overworld::loadBackground()
{
	for (auto i = 0; i < m_parallaxBackgroundTextures.size(); i++)
	{

		m_parallaxBackgroundSprites[i].setTexture(m_game->assets().getTexture(m_parallaxBackgroundTextures[i]), false);
		m_parallaxBackgroundSprites[i].setTextureRect(sf::IntRect(0, 0, 1000000, 1000000));
		m_parallaxBackgroundSprites[i].setPosition(-6500, -64);
	}
	m_parallaxBackgroundSprites[11].move(sf::Vector2f(0, 40));
	m_parallaxBackgroundSprites[13].move(sf::Vector2f(512, 0));
}

void Scene_Overworld::drawBackground()
{
	m_game->window().draw(m_parallaxBackgroundSprites[0]);// Background
	m_game->window().draw(m_parallaxBackgroundSprites[1]);// Backgroundlayer2
	m_game->window().draw(m_parallaxBackgroundSprites[2]);// Backgroundlayer3
	m_game->window().draw(m_parallaxBackgroundSprites[3]);// Far Forest
	m_game->window().draw(m_parallaxBackgroundSprites[4]);// Mid Forest
	m_game->window().draw(m_parallaxBackgroundSprites[5]);// Light Effect
	m_game->window().draw(m_parallaxBackgroundSprites[6]);// Front Forest
	m_game->window().draw(m_parallaxBackgroundSprites[7]);// Tree Bottoms
	m_game->window().draw(m_parallaxBackgroundSprites[8]);// Near Light Effects
	m_game->window().draw(m_parallaxBackgroundSprites[9]);// Tree Bottoms
	m_game->window().draw(m_parallaxBackgroundSprites[10]);// TreeTops
	m_parallaxBackgroundSprites[12].move(sf::Vector2f(-0.4f, 0.f));
	m_game->window().draw(m_parallaxBackgroundSprites[12]);// FrontBushes
	m_parallaxBackgroundSprites[13].move(sf::Vector2f(-0.4f, 0.f));
	m_game->window().draw(m_parallaxBackgroundSprites[13]);// FrontBushes
	m_game->window().draw(m_parallaxBackgroundSprites[11]);// Grass

}

void Scene_Overworld::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "INTERACT")
		{
			if (m_player->getComponent<CAnimation>().animation.getName() == "StandUp")
			{
				if (m_select == 1)
				{
					saveGame();
					m_game->stopSound("MusicOverworld");
					m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, "levels/level1.txt", m_saveFile));
				}
				if (m_select == 2 && m_level1Completion)
				{
					saveGame();
					m_game->stopSound("MusicOverworld");
					m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, "levels/level2.txt", m_saveFile));
				}
				if (m_select == 3 && m_level2Completion)
				{
					saveGame();
					m_game->stopSound("MusicOverworld");
					m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, "levels/level3.txt", m_saveFile));
				}
			}
		}
		else if (action.name() == "RIGHT")
		{
			if (m_select < 3)
			{
				m_select++;
			}
		}
		else if (action.name() == "LEFT")
		{
			if (m_select > 1)
			{
				m_select--;
			}
		}
		else if (action.name() == "QUIT")
		{
			saveGame();
			onEnd();
		}
	}
}

Vec2 Scene_Overworld::getPosition(int rx, int ry, int tx, int ty) const
{
	float x = rx * (int)m_game->window().getSize().x + (tx * 64) + 32;
	float y = ry * (int)m_game->window().getSize().y + (ty * 64) + 32;
	return Vec2(x, y);
}

void Scene_Overworld::sRender()
{
	m_game->window().setView(m_game->window().getDefaultView());
	m_game->window().clear(sf::Color(12, 215, 224));


	drawBackground();

	for (auto e : m_entityManager.getEntities("Leveltiles"))
	{
		auto& transform = e->getComponent<CTransform>();
		auto& animation = e->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		m_game->window().draw(animation.getSprite());
	}
	auto& ptransform = m_player->getComponent<CTransform>();
	auto& panimation = m_player->getComponent<CAnimation>().animation;
	panimation.getSprite().setRotation(ptransform.angle);
	panimation.getSprite().setPosition(ptransform.pos.x, ptransform.pos.y);
	panimation.getSprite().setScale(ptransform.scale.x, ptransform.scale.y);
	m_game->window().draw(panimation.getSprite());
	panimation.update();

	// draw the controls in the bottom-left
	m_menuText.setCharacterSize(32);
	m_menuText.setFillColor(sf::Color(100, 100, 100));
	m_menuText.setString("Select Level: F     Left: A    Right: D      Back: esc");
	m_menuText.setPosition(sf::Vector2f(10, 724));
	m_game->window().draw(m_menuText);
}