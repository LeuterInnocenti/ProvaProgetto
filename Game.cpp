//
// Created by Athos Innocenti on 30/10/2018.
//

#include "Game.h"
#include <chrono>
#include <thread>
#include <iostream>

const float Game::g = 0.7;
const float Game::jump = 1.8;
const float Game::shootTime = 1.3f;
const float Game::bulletSpeed = 1.7f;
const float Game::levelGround = 63.0f;
const float Game::speedIncreaser = 0.1;
const float Game::rateIncreaser = 0.120;

Game::Game() : window("FlyingJoyride", sf::Vector2u(1080, 720)), windowSize(window.getWindowSize()), player(),
               isCreated(false), isPowerUpOn(false),
               counter(1), blockX(100), creationRate(1.4f), speed(sf::Vector2f(0.7, 0.8)), tollerance(2),
               playerClock(), objectClock(), speedClock(), controlPowerUp(), enemyClock(), factoryB(), factoryE() {
    reset();
   
    backgroundTexture.loadFromFile("Background.png");
    backgroundTexture.setRepeated(true);
    background.setTexture(backgroundTexture);
    background.setTextureRect(sf::IntRect(0, 0, (500 * windowSize.x), windowSize.y + static_cast<int>(levelGround)));
    background.setScale(0.675, 0.95);

    playerTexture1.loadFromFile("frame-1.png");
    playerTexture2.loadFromFile("frame-3.png");
    playerTexture3.loadFromFile("frame-2.png");
    puPlayerTexture1.loadFromFile("puframe-1.png");
    puPlayerTexture2.loadFromFile("puframe-2.png");
    puPlayerTexture3.loadFromFile("puframe-3.png");
    player.setPlayerTexture(playerTexture1);

    fEnemyTexture.loadFromFile("fenemy1.png");
    sEnemyTexture.loadFromFile("senemy1.png");

    srand((unsigned) time(nullptr));
    createObjects();
    maxY = static_cast<int>(windowSize.y - (levelGround + blockX));
}

Game::~Game() {
    blocks.clear();
}

void Game::update() {
    window.update();
    background.move(-speed.x, 0);

    shoot();
    moveObject();
    movePlayer();
    deleteObject();
    createObjects();
    getBoundEnemy();
    getBoundBullet();

    collision();

    if (player.getDeath()) {
        // std::cout<<"Il tuo punteggio è: "<<score<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(110));
        window.setDone();
    }
    if (speedClock.getElapsedTime().asSeconds() >= 10) {
        speed.x += speedIncreaser;
        creationRate -= rateIncreaser;
        speedClock.restart();
    }
}

void Game::increaseScore() {}

// gestione del testo
void Game::handleText() {
    font.loadFromFile("arial.ttf");
    text.setFont(font);
    text.setString("");
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(25);
    text.setPosition(15200, 50);
}

void Game::reset() {
    //handleText();
    score = 0;
}

void Game::render() {
    window.beginDraw();
    window.draw(background);
    player.render(*window.getRenderWindow());
    for (auto &block : blocks)
        window.draw(*block);
    for (auto &enemy : enemies)
        window.draw(*enemy);
    for (auto &b : bullets)
        window.draw(b);
    for (auto &b : enemyBullets)
        window.draw(b);
    window.endDraw();
}

void Game::movePlayer() {
    player.setPlayerPosition(player.getPlayerPosition().x, player.getPlayerPosition().y + g);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        player.setPlayerPosition(player.getPlayerPosition().x, player.getPlayerPosition().y - jump);
        if (isPowerUpOn)
            player.setPlayerTexture(puPlayerTexture2);
        else
            player.setPlayerTexture(playerTexture2);
        playerTexture2.setSmooth(true);
    } else if (!sf::Keyboard::isKeyPressed((sf::Keyboard::Space))) {
        if (isPowerUpOn)
            player.setPlayerTexture(puPlayerTexture1);
        else
            player.setPlayerTexture(playerTexture1);
        playerTexture1.setSmooth(true);
    }
    if (player.getPlayerPosition().y + player.getPlayerSize().y >= window.getWindowSize().y - levelGround)
        player.setPlayerPosition(player.getPlayerPosition().x,
                                 window.getWindowSize().y - player.getPlayerSize().y - levelGround);
    if (player.getPlayerPosition().y <= 0)
        player.setPlayerPosition(player.getPlayerPosition().x, 0);
}

void Game::shoot() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && playerClock.getElapsedTime().asSeconds() >= shootTime) {
        createBullet();
        playerClock.restart();
    }
    moveBullet();
    if (enemyClock.getElapsedTime().asSeconds() >= (shootTime / 2)) {
        createEnemyBullet();
        enemyClock.restart();
    }
    moveEnemyBullet();
}

void Game::createBullet() {
    bullet.setRadius(10);
    bullet.setFillColor(sf::Color::Black);
    bullet.setPosition(player.getPlayerPosition().x + player.getPlayerSize().x,
                       player.getPlayerPosition().y + player.getPlayerSize().y / 2);
    bullets.emplace_back(sf::CircleShape(bullet));
}

void Game::moveBullet() {
    for (size_t j = 0; j < bullets.size(); ++j) {
        bullets[j].move(bulletSpeed, 0);
        if (bullets[j].getPosition().x >= window.getWindowSize().x + 2 * player.getPlayerSize().x)
            bullets.erase(bullets.begin() + j);
    }
}

void Game::createEnemyBullet() {
    for (auto &j : enemies) {
        if (j->getCanShoot()) {
            bullet.setRadius(10);
            bullet.setFillColor(sf::Color::Red);
            bullet.setPosition(j->getPosition().x, j->getPosition().y + j->getGlobalBounds().height / 2);
            enemyBullets.emplace_back(sf::CircleShape(bullet));
        }
    }
}

void Game::moveEnemyBullet() {
    for (size_t j = 0; j < enemyBullets.size(); ++j) {
        enemyBullets[j].move(-bulletSpeed, 0);
        if (enemyBullets[j].getPosition().x >= window.getWindowSize().x + 2 * player.getPlayerSize().x)
            enemyBullets.erase(enemyBullets.begin() + j);
    }
}

sf::FloatRect Game::getBoundBullet() {
    sf::FloatRect bulletshape;
    for (int i = 0; i < bullets.size(); i++) {
        bulletshape = bullets[i].getGlobalBounds();
        ind = i;
    }
    return bulletshape;
}

sf::FloatRect Game::getBoundEnemy() {
    sf::FloatRect enemyshape;
    for (int i = 0; i < enemies.size(); i++) {
        enemyshape = enemies[i]->getGlobalBounds();
        iter = i;
    }
    return enemyshape;
}

void Game::eraseBullet() {
    bullets.erase(bullets.begin() + ind);
}

void Game::eraseEnemy() {
    enemies.erase(enemies.begin() + iter);
}

void Game::createObjects() {
    if (objectClock.getElapsedTime().asSeconds() >= creationRate) {
        if (counter % 4 == 0 && randomCreation() == 1) {
            std::unique_ptr<Enemy> enemy = factoryE.createEnemy(EnemyType::FlyingEnemy);
            randomPos();
            if (randomCreation() % 2 != 0)
                enemy->setEnemySpeedY(speed.y);
            else
                enemy->setEnemySpeedY(-speed.y);
            enemy->setPosition(sf::Vector2f(2 * windowSize.x, randomY));
            enemies.emplace_back(move(enemy));
            isCreated = true;
            objectClock.restart();
            counter++;
        }
        if (counter % 7 == 0 && randomCreation() == 1) {
            std::unique_ptr<Enemy> enemy = factoryE.createEnemy(EnemyType::ShootingEnemy);
            randomPos();
            if (randomCreation() % 2 != 0)
                enemy->setEnemySpeedY(speed.y);
            else
                enemy->setEnemySpeedY(-speed.y);
            enemy->setPosition(sf::Vector2f(2 * windowSize.x, randomY));
            enemies.emplace_back(move(enemy));
            isCreated = true;
            objectClock.restart();
            counter++;
        }
        // !isPowerUpOn = se sono attivi PowerUp non crea PowerUpBlock
        if (counter % 6 == 0 && randomCreation() == 1 && !isPowerUpOn) {
            std::unique_ptr<Block> block = factoryB.createBlock(BlockType::PowerUpBlock);
            randomPos();
            block->setPosition(sf::Vector2f(2 * windowSize.x, randomY));
            blocks.emplace_back(move(block));
            isCreated = true;
            objectClock.restart();
            counter++;
        }
        if (!isCreated) {
            std::unique_ptr<Block> block = factoryB.createBlock(BlockType::NormalBlock);
            randomPos();
            block->setPosition(sf::Vector2f(2 * windowSize.x, randomY));
            blocks.emplace_back(move(block));
            objectClock.restart();
            counter++;
        }
        isCreated = false;
    }
}

void Game::moveObject() {
    for (auto &i : blocks) {
        i->move(-speed.x, 0);
    }
    for (auto &j : enemies) {
        if (j->getPosition().y + j->getGlobalBounds().height >= window.getWindowSize().y - levelGround ||
            j->getPosition().y <= 0) {
            j->setEnemySpeedY(-j->getEnemySpeedY());
        }
        j->move(-speed.x, j->getEnemySpeedY());
    }
}

void Game::deleteObject() {
    for (int i = 0; i < blocks.size(); ++i) {
        if (blocks[i]->getPosition().x + blocks[i]->getGlobalBounds().width < 0) {
            eraseB(i); // se esce dallo schermo lo cancella
        }
    }
    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->getPosition().x + enemies[i]->getGlobalBounds().width < 0) {
            eraseF(i); // se esce dallo schermo lo cancella
        }
    }
}

void Game::collision() {
    for (auto &i : blocks) {
        class NormalBlock *test = dynamic_cast<class NormalBlock *>(i.get());
        if (i->getGlobalBounds().intersects(player.getBound())) {
            // se character interseca con block muore e gameover
            if (test != nullptr) {
                if (isPowerUpOn)
                    player.setPlayerTexture(puPlayerTexture3);
                else
                    player.setPlayerTexture(playerTexture3);
                player.gameOver(true);
            }
            // se character interseca PowerUpBlock si attiva il potenziamento
            if (test == nullptr) {
                PowerUpBlock::activePowerUp();
                isPowerUpOn = true;
            }
        }
        // se bullet interseca un block viene eliminato
        if (i->getGlobalBounds().intersects(getBoundBullet()))
            eraseBullet();
        // se bullet interseca un enemy viene eliminato enemy
        if (i->getGlobalBounds().intersects(getBoundEnemy())) { // NON FUNZIONA
            std::cout << "intersezione" << std::endl;
            eraseEnemy();
        }
    }
    for (auto &j : enemies) {
        if (j->getGlobalBounds().intersects(player.getBound())) {
            // se character ha PowerUp e interseca enemy perde il potenziamento
            if (isPowerUpOn) {
                isPowerUpOn = false;
                controlPowerUp.restart();
            } else if (controlPowerUp.getElapsedTime().asSeconds() >= tollerance) {
                // se character non ha PowerUp e interseca enemy muore
                player.setPlayerTexture(playerTexture3);
                player.gameOver(true);
            }
        }
    }
}

// funzione randomica per settare coordinata Y del blocco
int Game::randomPos() {
    randomY = rand() % maxY;
    return randomY;
}

int Game::randomCreation() {
    return (rand() % 2);
}

// funzioni getter
const float Game::getShootTime() { return shootTime; }

const float Game::getLevelGround() { return levelGround; }

const float Game::getG() { return g; }

const float Game::getJump() { return jump; }

const float Game::getBulletSpeed() { return bulletSpeed; }

const sf::Vector2f &Game::getSpeed() const { return speed; }

float Game::getCreationRate() const { return creationRate; }

int Game::getMaxY() const { return maxY; }

const std::vector<sf::CircleShape> &Game::getBullets() const { return bullets; }