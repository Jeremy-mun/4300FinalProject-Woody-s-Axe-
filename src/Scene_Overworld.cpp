
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


	auto& shop1Tile = m_entityManager.addEntity("Leveltiles");
	shop1Tile->addComponent<CTransform>(Vec2(-352, 672));
	auto& shop2Tile = m_entityManager.addEntity("Leveltiles");
	shop2Tile->addComponent<CTransform>(Vec2(-672, 672));
	auto& shop3Tile = m_entityManager.addEntity("Leveltiles");
	shop3Tile->addComponent<CTransform>(Vec2(-992, 672));
	shop1Tile->addComponent<CAnimation>(m_game->assets().getAnimation("ShopLevel"), true);
	shop2Tile->addComponent<CAnimation>(m_game->assets().getAnimation("ShopLevel"), true);
	shop3Tile->addComponent<CAnimation>(m_game->assets().getAnimation("ShopLevel"), true);

	auto& shop1Pedestal = m_entityManager.addEntity("shop");
	shop1Pedestal->addComponent<CTransform>(Vec2(-352, 642));
	auto& shop2Pedestal = m_entityManager.addEntity("shop");
	shop2Pedestal->addComponent<CTransform>(Vec2(-672, 642));
	auto& shop3Pedestal = m_entityManager.addEntity("shop");
	shop3Pedestal->addComponent<CTransform>(Vec2(-992, 642));
	shop1Pedestal->addComponent<CAnimation>(m_game->assets().getAnimation("ShopPedestal"), true);
	shop2Pedestal->addComponent<CAnimation>(m_game->assets().getAnimation("ShopPedestal"), true);
	shop3Pedestal->addComponent<CAnimation>(m_game->assets().getAnimation("ShopPedestal"), true);
	auto& shopkeeper = m_entityManager.addEntity("shop");
	shopkeeper->addComponent<CTransform>(Vec2(-1168, 642));
	shopkeeper->addComponent<CAnimation>(m_game->assets().getAnimation("Shopkeeper"), true);
	shopkeeper->getComponent<CTransform>().scale.x = -1;
	auto& shop1Item = m_entityManager.addEntity("shop");
	shop1Item->addComponent<CTransform>(Vec2(-352, 612));
	auto& shop2Item = m_entityManager.addEntity("shop");
	shop2Item->addComponent<CTransform>(Vec2(-672, 612));
	auto& shop3Item = m_entityManager.addEntity("shop");
	shop3Item->addComponent<CTransform>(Vec2(-992, 612));
	shop1Item->addComponent<CAnimation>(m_game->assets().getAnimation("HealthUp"), true);
	shop2Item->addComponent<CAnimation>(m_game->assets().getAnimation("ArrowsUp"), true);
	shop3Item->addComponent<CAnimation>(m_game->assets().getAnimation("DamageUp"), true);

	auto& shop1Coin = m_entityManager.addEntity("shop");
	shop1Coin->addComponent<CTransform>(Vec2(-357, 662));
	auto& shop2Coin = m_entityManager.addEntity("shop");
	shop2Coin->addComponent<CTransform>(Vec2(-677, 662));
	auto& shop3Coin = m_entityManager.addEntity("shop");
	shop3Coin->addComponent<CTransform>(Vec2(-997, 662));
	shop1Coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
	shop2Coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
	shop3Coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);

	auto& sign = m_entityManager.addEntity("shop");
	sign->addComponent<CTransform>(Vec2(49, 400));
	sign->addComponent<CAnimation>(m_game->assets().getAnimation("ShopSign"), true);

	
	m_shopText.setFont(m_game->assets().getFont("Gypsy"));
	m_shopText.setCharacterSize(20);
	m_shopText.setString("x50");
	m_shopText.setPosition(sf::Vector2f(-352, 647));
	m_shopText.setFillColor(sf::Color::Black);

	m_shopText2.setFont(m_game->assets().getFont("Gypsy"));
	m_shopText2.setCharacterSize(20);
	m_shopText2.setString("x50");
	m_shopText2.setPosition(sf::Vector2f(-672, 647));
	m_shopText2.setFillColor(sf::Color::Black);

	m_shopText3.setFont(m_game->assets().getFont("Gypsy"));
	m_shopText3.setCharacterSize(20);
	m_shopText3.setString("x100");
	m_shopText3.setPosition(sf::Vector2f(-992, 647));
	m_shopText3.setFillColor(sf::Color::Black);

	auto coin = m_entityManager.addEntity("shop");

	coin->addComponent<CTransform>(Vec2(50,50));
	coin->addComponent<CAnimation>(m_game->assets().getAnimation("CoinHUD"), true);
	coin->addComponent<CBoundingBox>(m_game->assets().getAnimation("CoinHUD").getSize());


	m_walletText.setFont(m_game->assets().getFont("Gypsy"));
	m_walletText.setCharacterSize(32);

	m_menuText.setFont(m_game->assets().getFont("Gypsy"));
	m_menuText.setCharacterSize(64);

	m_game->playSound("MusicOverworld");
	m_game->loopSound("MusicOverworld");
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
	if (m_select == -2)
	{
		if (m_player->getComponent<CTransform>().pos == Vec2(-992, 656))
		{
			if (m_player->getComponent<CAnimation>().animation.getName() != "StandUp")
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
	else if (m_select == -1)
	{
		if (m_player->getComponent<CTransform>().pos == Vec2(-672, 656))
		{
			if (m_player->getComponent<CAnimation>().animation.getName() != "StandUp")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("StandUp");
			}
		}
		else
		{
			if (m_player->getComponent<CTransform>().pos.x > -672)
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
	else if (m_select == 0)
	{
		if (m_player->getComponent<CTransform>().pos == Vec2(-352, 656))
		{
			if (m_player->getComponent<CAnimation>().animation.getName() != "StandUp")
			{
				m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("StandUp");
			}
		}
		else
		{
			if (m_player->getComponent<CTransform>().pos.x > -352)
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
	else if (m_select == 1)
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
			if (m_player->getComponent<CTransform>().pos.x > 352)
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
	else if (m_select == 2)
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
	else if (m_select == 3)
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
				if (m_select == -2 && m_coinCount >= 100)
				{
					m_coinCount -= 100;
					m_damage++;
					saveGame();
				}
				else if (m_select == -1 && m_coinCount >= 50)
				{
					m_coinCount -= 50;
					m_arrowCount += 5;
					saveGame();
				}
				else if (m_select == 0 && m_coinCount >= 50)
				{
					m_coinCount -= 50;
					m_maxHealth += 5;
					saveGame();
				}
				else if (m_select == 1)
				{
					saveGame();
					m_game->stopSound("MusicOverworld");
					m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, "levels/level1.txt", m_saveFile));
				}
				else if (m_select == 2 && m_level1Completion)
				{
					saveGame();
					m_game->stopSound("MusicOverworld");
					m_game->changeScene("MainGame", std::make_shared<Scene_MainGame>(m_game, "levels/level2.txt", m_saveFile));
				}
				else if (m_select == 3 && m_level2Completion)
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
			if (m_select > -2)
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
	if (m_player->getComponent<CTransform>().pos.x >= 0)
	{
		m_game->window().setView(m_game->window().getDefaultView());
		m_game->window().clear(sf::Color(12, 215, 224));
	}
	else
	{
		sf::View view = m_game->window().getDefaultView();
		view.setCenter(sf::Vector2f(m_game->window().getDefaultView().getCenter().x - m_game->window().getDefaultView().getSize().x, m_game->window().getDefaultView().getCenter().y));
		m_game->window().setView(view);
		m_game->window().clear(sf::Color(12, 215, 224));
	}


	drawBackground();

	for (auto e : m_entityManager.getEntities("shop"))
	{
		auto& transform = e->getComponent<CTransform>();
		auto& animation = e->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		m_game->window().draw(animation.getSprite());
		if (animation.getName() == "Shopkeeper")
		{
			animation.update();
		}
		else if (animation.getName() == "CoinHUD")
		{
			if (m_player->getComponent<CTransform>().pos.x >= 0)
			{
				transform.pos.x = 50;
			}
			else
			{
				transform.pos.x = 50 - m_game->window().getDefaultView().getSize().x;
			}
		}
	}

	for (auto e : m_entityManager.getEntities("Leveltiles"))
	{
		auto& transform = e->getComponent<CTransform>();
		auto& animation = e->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		m_game->window().draw(animation.getSprite());
	}
	m_game->window().draw(m_shopText);
	m_game->window().draw(m_shopText2);
	m_game->window().draw(m_shopText3);

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
	m_walletText.setString("x" + std::to_string(m_coinCount));
	if (m_player->getComponent<CTransform>().pos.x >= 0)
	{
		m_menuText.setPosition(sf::Vector2f(10, 724));
		m_walletText.setPosition(sf::Vector2f(82, 25));
	}
	else
	{
		m_menuText.setPosition(sf::Vector2f(10 - m_game->window().getDefaultView().getSize().x, 724));
		m_walletText.setPosition(sf::Vector2f(82 - m_game->window().getDefaultView().getSize().x, 25));
	}
	m_game->window().draw(m_menuText);
	m_game->window().draw(m_walletText);
}