
#include "GameEngine.h"
#include "Assets.h"
#include "Scene_Menu.h"

GameEngine::GameEngine(const std::string & path)
{
    init(path);
}

void GameEngine::init(const std::string & path)
{
    m_assets.loadFromFile(path);

    m_window.create(sf::VideoMode(1280, 768), "Woody's AXE");
    m_window.setFramerateLimit(60);
                           
    changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}

bool GameEngine::isRunning()
{ 
    return m_running & m_window.isOpen();
}

sf::RenderWindow & GameEngine::window()
{
    return m_window;
}

void GameEngine::run()
{
    while (isRunning())
    {
        update();
    }
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            quit();
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            if (m_remap && event.type == sf::Event::KeyPressed)
            {
                currentScene()->doAction(Action("REMAP", event.key.code));
                continue;
            }

            // if the current scene does not have an action associated with this key, skip the event
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }
           
            // determine start or end action by whether it was key pres or release
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
                        
            // look up the action and send the action to the scene
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }

        // Mouse Actions
        auto mpos = sf::Mouse::getPosition(m_window);
        Vec2 pos(mpos.x, mpos.y);
        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.mouseButton.button)
            {
                case sf::Mouse::Left:   {currentScene()->doAction(Action("LEFT_CLICK", "START", pos)); break; }
                case sf::Mouse::Middle: {currentScene()->doAction(Action("MIDDLE_CLICK", "START", pos)); break; }
                case sf::Mouse::Right:  {currentScene()->doAction(Action("RIGHT_CLICK", "START", pos)); break; }
                default: break;
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            switch (event.mouseButton.button)
            {
            case sf::Mouse::Left:   {currentScene()->doAction(Action("LEFT_CLICK", "END", pos)); break; }
            case sf::Mouse::Middle: {currentScene()->doAction(Action("MIDDLE_CLICK", "END", pos)); break; }
            case sf::Mouse::Right:  {currentScene()->doAction(Action("RIGHT_CLICK", "END", pos)); break; }
            default: break;
            }
        }

        if (event.type == sf::Event::MouseMoved)
        {
            currentScene()->doAction(Action("MOUSE_MOVE", Vec2(event.mouseMove.x, event.mouseMove.y)));
        }
        if (event.type == sf::Event::MouseWheelScrolled)
        {
            switch (event.mouseWheelScroll.wheel)
            {
                case sf::Mouse::VerticalWheel: {currentScene()->doAction(Action("SCROLL", "START", Vec2(event.mouseWheel.x, 0))); break; }
                default: break;
            }
        }
    }
    
}

std::string GameEngine::keyToString(sf::Keyboard::Key key)
{
    if (key < 26 && key >= 0)
    {
        char letterOfAlphabet = 'A' + key;
        return std::string(1, letterOfAlphabet);
    }
    else if (key < 36 && key >= 0)
    {
        int number = key - 26;
        return std::to_string(number);
    }
    else if (key == sf::Keyboard::Space)
    {
        return "Space";
    }
    else if (key == sf::Keyboard::BackSpace)
    {
        return "Backspace";
    }
    else if (key == sf::Keyboard::BackSlash)
    {
        return "Backslash";
    }
    else if (key == sf::Keyboard::Tab)
    {
        return "Tab";
    }
    else if (key == sf::Keyboard::LControl)
    {
        return "Left Control";
    }
    else if (key == sf::Keyboard::RControl)
    {
        return "Right Control";
    }
    else if (key == sf::Keyboard::Left)
    {
        return "Left Arrow";
    }
    else if (key == sf::Keyboard::Right)
    {
        return "Right Arrow";
    }
    else if (key == sf::Keyboard::Up)
    {
        return "Up Arrow";
    }
    else if (key == sf::Keyboard::Down)
    {
        return "Down Arrow";
    }
    else if (key == sf::Keyboard::RAlt)
    {
        return "Right Alt";
    }
    else if (key == sf::Keyboard::LAlt)
    {
        return "Left Alt";
    }
    else if (key == sf::Keyboard::RShift)
    {
        return "Right Shift";
    }
    else if (key == sf::Keyboard::LShift)
    {
        return "Left Shift";
    }
    else if (key == sf::Keyboard::RBracket)
    {
        return "[";
    }
    else if (key == sf::Keyboard::LBracket)
    {
        return "]";
    }
    else if (key == sf::Keyboard::SemiColon)
    {
        return ";";
    }
    else if (key == sf::Keyboard::Comma)
    {
        return ",";
    }
    else if (key == sf::Keyboard::Period)
    {
        return ".";
    }
    else if (key == sf::Keyboard::Quote)
    {
        return "'";
    }
    else if (key == sf::Keyboard::Enter)
    {
        return "Enter";
    }
    else if (key == sf::Keyboard::Tilde)
    {
        return "Tilde";
    }
    else if (key == sf::Keyboard::F1)
    {
        return "F1";
    }
    else if (key == sf::Keyboard::F2)
    {
        return "F2";
    }
    else if (key == sf::Keyboard::F3)
    {
        return "F3";
    }
    else if (key == sf::Keyboard::F4)
    {
        return "F4";
    }
    else if (key == sf::Keyboard::F5)
    {
        return "F5";
    }
    else if (key == sf::Keyboard::F6)
    {
        return "F6";
    }
    else if (key == sf::Keyboard::F7)
    {
        return "F7";
    }
    else if (key == sf::Keyboard::F8)
    {
        return "F8";
    }
    else if (key == sf::Keyboard::F9)
    {
        return "F9";
    }
    else if (key == sf::Keyboard::F10)
    {
        return "F10";
    }
    else if (key == sf::Keyboard::F11)
    {
        return "F11";
    }
    else if (key == sf::Keyboard::F12)
    {
        return "F12";
    }
    else if (key == sf::Keyboard::Dash)
    {
        return "-";
    }
    else if (key == sf::Keyboard::Equal)
    {
        return "=";
    }
    else if (key == sf::Keyboard::Num0)
    {
        return "Numpad 0";
    }
    else if (key == sf::Keyboard::Num1)
    {
        return "Numpad 1";
    }
    else if (key == sf::Keyboard::Num2)
    {
        return "Numpad 2";
    }
    else if (key == sf::Keyboard::Num3)
    {
        return "Numpad 3";
    }
    else if (key == sf::Keyboard::Num4)
    {
        return "Numpad 4";
    }
    else if (key == sf::Keyboard::Num5)
    {
        return "Numpad 5";
    }
    else if (key == sf::Keyboard::Num6)
    {
        return "Numpad 6";
    }
    else if (key == sf::Keyboard::Num7)
    {
        return "Numpad 7";
    }
    else if (key == sf::Keyboard::Num8)
    {
        return "Numpad 8";
    }
    else if (key == sf::Keyboard::Num9)
    {
        return "Numpad 9";
    }
    else if (key == sf::Keyboard::Delete)
    {
        return "Delete";
    }
    else if (key == sf::Keyboard::Home)
    {
        return "Home";
    }
    else if (key == sf::Keyboard::Insert)
    {
        return "Insert";
    }
    else if (key == sf::Keyboard::End)
    {
        return "End";
    }
    else if (key == sf::Keyboard::PageUp)
    {
        return "Page Up";
    }
    else if (key == sf::Keyboard::PageDown)
    {
        return "Page Down";
    }
    else
    {
        return "UNKNOWN";
    }
}
                           
void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (scene)
    {
        m_sceneMap[sceneName] = scene;
    }
    else
    {
        if (m_sceneMap.find(sceneName) == m_sceneMap.end())
        {
            std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
            return;
        }
    }

    if (endCurrentScene)
    {
        m_sceneMap.erase(m_sceneMap.find(m_currentScene));
    }
                           
    m_currentScene = sceneName;
}

void GameEngine::update()
{
    if (!isRunning()) { return; }
    
    if (m_sceneMap.empty()) { return; }
    
    sUserInput();
    currentScene()->simulate(m_simulationSpeed);
    currentScene()->sRender();
    m_window.display();
}

void GameEngine::quit()
{
    m_running = false;
}
                           
Assets& GameEngine::assets()
{
    return m_assets;
}
void GameEngine::setVolume(const std::string& soundName, int volume)
{
    m_assets.getSound(soundName).setVolume(volume);
}
void GameEngine::playSound(const std::string& soundName)
{
    m_assets.getSound(soundName).play();
}
void GameEngine::stopSound(const std::string& soundName)
{
    m_assets.getSound(soundName).stop();
}
void GameEngine::pauseSound(const std::string& soundName)
{
    m_assets.getSound(soundName).pause();
}

void GameEngine::remap()
{
    m_remap = !m_remap;
}

