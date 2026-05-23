#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <atomic>

struct Config {

    static constexpr int windowWidth = 1000;
    static constexpr int windowHeight = 800;
    static constexpr float frameRate = 60.0f;

    static constexpr float scale = 0.7f;
    static constexpr int ogWidth = 1095;
    static constexpr int ogHeight = 1062;

    static constexpr double micTreshold = 5000.0;
    static constexpr float blinkDuration = 0.5f;
    static constexpr float betweenBlinkDuration = 3.0f;

};


class CustomRecorder : public sf::SoundRecorder {

public:

    CustomRecorder() : m_isSpeaking(false) {}

    bool isSpeaking(){
        return m_isSpeaking.load();
    }

protected:

    virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
    {
        if (sampleCount == 0) return true;

        double sum = 0;
        for (std::size_t i = 0; i < sampleCount; ++i) {
            sum += samples[i] * samples[i];
        }
        double level = sum / sampleCount;

        m_isSpeaking.store(level > Config::micTreshold);
        //std::cout << level << std::endl;

        return true;
    }

private:
    std::atomic<bool> m_isSpeaking = false;
 
};

class Avatar {
public: 
    Avatar() : m_nextBlinkTime(Config::betweenBlinkDuration) {

        m_xCenter = (1000-Config::ogWidth*Config::scale)/2;
        m_yCenter = (800-Config::ogHeight*Config::scale)/2;

        loadResources();
        initSprites();
    }

    void update(bool isSpeaking, sf::RenderWindow& window){
        updateMouth(isSpeaking);
        updateBlink();
        updateMouseHandMovement(window);
    }

    void draw(sf::RenderWindow& window){

        window.draw(m_sBody);
        window.draw(m_sMouth);
        window.draw(m_sEyes);

        window.draw(m_sL2);
        window.draw(m_sR2);

        window.draw(m_sDesk);

        window.draw(m_sL1);
        window.draw(m_sR1);

    }
private:
    void loadResources() {

        if (!m_tBody.loadFromFile("assets/body.png") ||
            !m_tMouthClosed.loadFromFile("assets/mouth_closed.png") ||
            !m_tMouthOpen.loadFromFile("assets/mouth_open.png") ||
            !m_tEyesOpen.loadFromFile("assets/eyes_open.png") ||
            !m_tEyesClosed.loadFromFile("assets/eyes_closed.png") ||
            !m_tDesk.loadFromFile("assets/desk.png") ||
            !m_tR1.loadFromFile("assets/r1.png") ||
            !m_tR2.loadFromFile("assets/r2.png") ||
            !m_tL1.loadFromFile("assets/l1.png") ||
            !m_tL2.loadFromFile("assets/l2.png")) 
        {
            throw std::runtime_error("Can't find all assets");
        }
    }

    void initSprites(){

        std::vector<sf::Sprite*> allSprites = {
            &m_sBody, &m_sMouth, &m_sEyes, 
            &m_sDesk, &m_sL1, &m_sL2, &m_sR1, &m_sR2
        };

        m_sBody.setTexture(m_tBody);
        m_sMouth.setTexture(m_tMouthClosed);
        m_sEyes.setTexture(m_tEyesOpen);
        m_sDesk.setTexture(m_tDesk);
        m_sL1.setTexture(m_tL1);
        m_sL2.setTexture(m_tL2);
        m_sR1.setTexture(m_tR1);
        m_sR2.setTexture(m_tR2);

        for (auto* sprite : allSprites) {
            sprite->setScale(Config::scale, Config::scale);
            sprite->setPosition(m_xCenter, m_yCenter);
        }
    }

    void updateBlink(){

        float seconds = m_blinkClock.getElapsedTime().asSeconds();

        if (!m_isBlinking && seconds > m_nextBlinkTime){
            m_isBlinking = true;
            m_sEyes.setTexture(m_tEyesClosed);
            m_blinkClock.restart();
        } else if (m_isBlinking && seconds > Config::blinkDuration) {
            m_isBlinking = false;
            m_sEyes.setTexture(m_tEyesOpen);
            m_blinkClock.restart();
        }

    }

    void updateMouseHandMovement(sf::RenderWindow& window){

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        float x_delta = mousePos.x * 0.03f;
        float y_delta = mousePos.y * 0.005f;
        float y_hand_delta = 0.0f;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            y_hand_delta += 2.0f;
            y_delta += 1.0f;
        }

        m_sL2.setPosition(m_xCenter + x_delta, m_yCenter + y_delta - 20.0f);
        m_sL1.setPosition(m_xCenter + x_delta, m_yCenter + y_delta + y_hand_delta);
    }

    void updateMouth(bool isSpeaking) {
        if (isSpeaking) {
            m_sMouth.setTexture(m_tMouthOpen);
        } else {
            m_sMouth.setTexture(m_tMouthClosed);
        }
    }

    sf::Texture m_tBody, m_tMouthClosed, m_tMouthOpen, m_tEyesOpen, m_tEyesClosed, m_tDesk, m_tR1, m_tR2, m_tL1, m_tL2;
    sf::Sprite m_sBody, m_sMouth, m_sEyes, m_sDesk, m_sL1, m_sL2, m_sR1, m_sR2;
    
    float m_xCenter;
    float m_yCenter;

    sf::Clock m_blinkClock;

    float m_nextBlinkTime;
    bool m_isBlinking;

};


int main() {
    try {

        sf::RenderWindow window(sf::VideoMode(Config::windowWidth, Config::windowHeight), "PNGTuber");
        
        window.setFramerateLimit(static_cast<unsigned int>(Config::frameRate));

        if (!sf::SoundBufferRecorder::isAvailable()) {
            std::cout << "Warning: No audio interface found :(" << std::endl;
        }

        CustomRecorder analyzer;
        analyzer.start();

        Avatar avatar;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            avatar.update(analyzer.isSpeaking(), window);

            window.clear(sf::Color(0, 255, 0));
            avatar.draw(window);
            window.display();
        }

        analyzer.stop();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}