//
// Created by Athos Innocenti on 30/10/2018.
//

#ifndef FLYJOYRIDE_GAME_H
#define FLYJOYRIDE_GAME_H

#include <list>
#include <string>

#include "Block.h"
#include "Enemy.h"
#include "Window.h"
#include "Subject.h"
#include "Character.h"
#include "BlockFactory.h"
#include "EnemyFactory.h"
#include "SFML/Graphics.hpp"

class Game : public Subject {
public:
    Game();
    ~Game();
    void update();
    void render();
    void handleText();
    Window *GetWindow() { return &window; };

    void shoot();
    void moveBullet();
    void movePlayer();
    void createBullet();
    void moveEnemyBullet();
    void createEnemyBullet();

    void moveObject();
    void deleteObject();

    void collision();
    void createObjects();

    int randomPos();
    int randomCreation();

    // funzioni getter
    int getMaxY() const;
    int getContainerSize();
    static const float getG();
    static const float getJump();
    float getCreationRate() const;
    static const float getShootTime();
    static const float getLevelGround();
    static const float getBulletSpeed();
    const sf::Vector2f &getSpeed() const;
    const std::vector<sf::CircleShape> &getBullets() const;

    // funzioni per test
    void eraseB(int index) { blocks.erase(blocks.begin() + index); }

    // funzioni per observer
    void setScore(unsigned int score);
    unsigned int getScore() const;
    void notify() override;
    void unsubscribe(Observer *o) override;
    void subscribe(Observer *o) override;
    Window &getWindow() { return window; }

    void setAchievementString();
private:
    int maxY;
    int randomY;

    Window window;
    sf::Clock speedClock;
    sf::Sprite background;
    sf::Vector2i windowSize;
    sf::Texture backgroundTexture;
    static const float levelGround;

    Character player;
    sf::Clock playerClock;
    sf::CircleShape bullet;
    sf::Texture playerTexture1;
    sf::Texture playerTexture2;
    sf::Texture playerTexture3;
    sf::Texture puPlayerTexture1;
    sf::Texture puPlayerTexture2;
    std::vector<sf::CircleShape> bullets;
    std::vector<sf::CircleShape> enemyBullets;

    int n;
    int blockX;
    int counter;
    int tollerance;
    bool isCreated;
    bool isPowerUpOn;
    bool isEnemyCreated;
    float creationRate;
    static const float g;
    static const float jump;
    static const float shootTime;
    static const float bulletSpeed;
    static const float rateIncreaser;
    static const float speedIncreaser;

    sf::Vector2f speed;
    BlockFactory factoryB;
    sf::Clock objectClock;
    sf::Clock controlPowerUp;
    std::vector<std::unique_ptr<Block>> blocks;

    sf::Clock enemyClock;
    EnemyFactory factoryE;
    sf::Texture fEnemyTexture;
    sf::Texture sEnemyTexture;
    std::vector<std::unique_ptr<Enemy>> enemies;

    sf::Font font;
    sf::Text text;
    sf::Text scoreText;
    unsigned int score;
    std::list<Observer*> observers;
    
};

#endif //FLYJOYRIDE_GAME_H