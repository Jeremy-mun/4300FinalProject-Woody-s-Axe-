
#pragma once

#include "Common.h"
#include "Scene.h"
#include "Assets.h"

#include <memory>
                           
typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;
                           
class GameEngine
{

protected:

    sf::RenderWindow    m_window;
    Assets              m_assets;
    std::string         m_currentScene;
    SceneMap            m_sceneMap;
    size_t              m_simulationSpeed = 1;
    bool                m_running = true;
    bool                m_remap = false;

    void init(const std::string & path);
    void update();

    void sUserInput();

    std::shared_ptr<Scene> currentScene();
                           
public:
    
    GameEngine(const std::string & path);
                           
    void changeScene(const std::string & sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);

    void quit();
    void run();
                           
    void setVolume(const std::string& soundName, int volume);
    void playSound(const std::string& soundName);
    void stopSound(const std::string& soundName);
    void pauseSound(const std::string& soundName);
    void loopSound(const std::string& soundName);

    sf::RenderWindow & window();
    Assets& assets();
    bool isRunning();
    void remap();
    std::string keyToString(sf::Keyboard::Key key);
};

