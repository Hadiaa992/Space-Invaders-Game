#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include<iostream>
using namespace std;

//  Window
const int WIN_W = 1200;
const int WIN_H = 1000;

// Power-up type constants
const int powerUpSpread = 1;
const int powerUpPierce = 2;
const int powerUpShield = 3;
const int powerUpEMP = 4;

// Game state constants
const int stateIntro = 0;
const int stateInstructions = 1;
const int stateMenu = 2;
const int statePlaying = 3;
const int statePaused = 4;
const int stateGameOver = 5;
const int stateWin = 6;

// Game mode constants
const int modeArcade = 1;
const int modeSurvival = 2;


struct AssetManager
{
   sf::Texture texPlayer;
   sf::Texture texDrone, texViper, texSeeker;
   sf::Texture texCruiser, texTwinCannons, texMothership, texTurret;
   sf::Texture texBackground;
   sf::Texture texPlayerBullet, texEnemyBullet;
   sf::Texture texSpread, texPierce, texShield, texEMP;
   sf::Texture texAsteroid;
   sf::Font    font;

   // SFML 3: sf::Image constructor takes size + colour directly
   static sf::Texture makeColour(unsigned w, unsigned h, sf::Color c)
   {
       // SFML 3: sf::Image(sf::Vector2u, sf::Color)
       sf::Image img(sf::Vector2u(w, h), c);
       sf::Texture t;
       // SFML 3: loadFromImage returns bool (same API, just confirm usage)
       if (!t.loadFromImage(img)) { /* fallback already is the colour */ }
       return t;
   }

   static sf::Texture makeStarfield()
   {
       // SFML 3: sf::Image(sf::Vector2u, sf::Color)
       sf::Image img(sf::Vector2u(WIN_W, WIN_H), sf::Color(5, 5, 20));
       for (int i = 0; i < 300; ++i)
       {
           int sx = rand() % WIN_W;
           int sy = rand() % WIN_H;
           std::uint8_t br = static_cast<std::uint8_t>(100 + rand() % 156);
           // SFML 3: setPixel takes sf::Vector2u
           img.setPixel(sf::Vector2u(static_cast<unsigned>(sx),
               static_cast<unsigned>(sy)),
               sf::Color(br, br, br));
       }
       sf::Texture t;
       t.loadFromImage(img);
       return t;
   }

   void load()
   {
       if (!texPlayer.loadFromFile("Assets/Images/Player.png"))
           texPlayer = makeColour(30, 40, sf::Color::Cyan);

       if (!texDrone.loadFromFile("Assets/Images/Drone.png"))
           texDrone = makeColour(28, 28, sf::Color::Red);

       if (!texViper.loadFromFile("Assets/Images/Viper.png"))
           texViper = makeColour(28, 28, sf::Color(255, 120, 0));

       if (!texSeeker.loadFromFile("Assets/Images/Seeker.png"))
           texSeeker = makeColour(24, 24, sf::Color::Yellow);

       if (!texCruiser.loadFromFile("Assets/Images/Cruiser.png"))
           texCruiser = makeColour(80, 50, sf::Color(180, 0, 0));

       if (!texTwinCannons.loadFromFile("Assets/Images/Twincannons.png"))
           texTwinCannons = makeColour(120, 60, sf::Color(200, 80, 0));

       if (!texMothership.loadFromFile("Assets/Images/Mothership.png"))
           texMothership = makeColour(140, 70, sf::Color(120, 0, 200));

       if (!texTurret.loadFromFile("Assets/Images/Turret.png"))
           texTurret = makeColour(20, 20, sf::Color(150, 150, 150));

    /* if (!texBackground.loadFromFile("Assets/Images/BG.png"))
           texBackground = makeStarfield();  */ 
           
       if (!gAssets.texBackground.loadFromFile("assets/BG.png"))
       {
           std::cout << "BG load failed!" << std::endl;
       }
       else
       {
           std::cout << "BG loaded successfully!" << std::endl;
       }

       // SFML 3: setRepeated is still valid
       texBackground.setRepeated(true);

       if (!texPlayerBullet.loadFromFile("Assets/Images/Playerbullet.png"))
           texPlayerBullet = makeColour(4, 50, sf::Color(0, 255, 160));

       if (!texEnemyBullet.loadFromFile("Assets/Images/Enemybullet.png"))
           texEnemyBullet = makeColour(6, 50, sf::Color(255, 50, 50));

       if (!texSpread.loadFromFile("Assets/Images/pu_spread.png"))
           texSpread = makeColour(20, 70, sf::Color(0, 180, 255));

       if (!texPierce.loadFromFile("Assets/Images/pu_pierce.png"))
           texPierce = makeColour(20, 70, sf::Color(255, 0, 200));

       if (!texShield.loadFromFile("Assets/Images/pu_shield.png"))
           texShield = makeColour(20, 70, sf::Color(0, 220, 220));

       if (!texEMP.loadFromFile("Assets/Images/pu_emp.png"))
           texEMP = makeColour(20, 70, sf::Color(255, 230, 0));

       if (!texAsteroid.loadFromFile("Assets/Images/Asteroid.png"))
           texAsteroid = makeColour(35, 35, sf::Color(120, 100, 70));

       // SFML 3: sf::Font::openFromFile (replaces loadFromFile)
       if (!font.openFromFile("Assets/Font/ByteBounce.ttf")) {}
   }
} gAssets;

struct AudioManager
{
   sf::Music bgMusic, bossMusic;

   sf::SoundBuffer bufPlayerFire, bufEnemyFire, bufExplosion;
   sf::SoundBuffer bufPlayerHit, bufPowerUp, bufEMP, bufDash, bufShieldBreak;

   sf::Sound sndPlayerFire, sndEnemyFire, sndExplosion;
   sf::Sound sndPlayerHit, sndPowerUp, sndEMP, sndDash, sndShieldBreak;

   // SFML 3: sf::Sound constructor requires a buffer reference
   AudioManager()
       : sndPlayerFire(bufPlayerFire),
       sndEnemyFire(bufEnemyFire),
       sndExplosion(bufExplosion),
       sndPlayerHit(bufPlayerHit),
       sndPowerUp(bufPowerUp),
       sndEMP(bufEMP),
       sndDash(bufDash),
       sndShieldBreak(bufShieldBreak)
   {
   }

   void load()
   {
       // SFML 3: sf::Music::openFromFile is unchanged
       bgMusic.openFromFile("Assets/Audio/BG.ogg");
       bgMusic.setLooping(true);          // SFML 3: setLooping (was setLoop)
       bossMusic.openFromFile("Assets/Audio/BossBG.ogg");
       bossMusic.setLooping(true);

       // SFML 3: SoundBuffer::loadFromFile unchanged; Sound must be re-bound
       //         after buffer load because buffers may move in memory.
       bufPlayerFire.loadFromFile("Assets/Audio/playerfire.wav");
       sndPlayerFire = sf::Sound(bufPlayerFire);

       bufEnemyFire.loadFromFile("Assets/Audio/enemyfire.wav");
       sndEnemyFire = sf::Sound(bufEnemyFire);

       bufExplosion.loadFromFile("Assets/Audio/explosion.wav");
       sndExplosion = sf::Sound(bufExplosion);

       bufPlayerHit.loadFromFile("Assets/Audio/playerhit.wav");
       sndPlayerHit = sf::Sound(bufPlayerHit);

       bufPowerUp.loadFromFile("Assets/Audio/powerup.wav");
       sndPowerUp = sf::Sound(bufPowerUp);

       bufEMP.loadFromFile("Assets/Audio/emp.wav");
       sndEMP = sf::Sound(bufEMP);

       bufDash.loadFromFile("Assets/Audio/dash.wav");
       sndDash = sf::Sound(bufDash);

       bufShieldBreak.loadFromFile("Assets/Audio/shieldbreak.wav");
       sndShieldBreak = sf::Sound(bufShieldBreak);
   }

   void playNormalMusic()
   {
       bossMusic.stop();
       // SFML 3: getStatus() returns sf::SoundSource::Status enum (same values)
       if (bgMusic.getStatus() != sf::SoundSource::Status::Playing)
       {
           bgMusic.setVolume(50);
           bgMusic.play();
       }
   }
   void playBossMusic()
   {
       bgMusic.stop();
       if (bossMusic.getStatus() != sf::SoundSource::Status::Playing)
       {
           bossMusic.play();
           bossMusic.setVolume(50);
       }
   }
   void stopAll() { bgMusic.stop(); bossMusic.stop(); }

   void playPlayerFire() { sndPlayerFire.play(); }
   void playEnemyFire() { sndEnemyFire.play(); }
   void playExplosion() { sndExplosion.play(); }
   void playPlayerHit() { sndPlayerHit.play(); }
   void playPowerUp() { sndPowerUp.play(); }
   void playEMP() { sndEMP.play(); }
   void playDash() { sndDash.play(); }
   void playShieldBreak() { sndShieldBreak.play(); }
} gAudio;

struct Particle
{
   float    x = 0.f, y = 0.f, vx = 0.f, vy = 0.f;
   float    lifetime = 0.f, maxLife = 0.f, radius = 0.f;
   sf::Color color = sf::Color::White;
   bool     active = false;
};

const int MAX_PARTICLES = 800;
Particle  gParticles[MAX_PARTICLES];

void spawnExplosion(float cx, float cy, int count = 20,
   sf::Color baseColor = sf::Color(255, 130, 0))
{
   for (int i = 0; i < count; ++i)
   {
       for (int j = 0; j < MAX_PARTICLES; ++j)
       {
           if (!gParticles[j].active)
           {
               float angle = static_cast<float>(rand() % 628) / 100.f;
               float speed = 60.f + rand() % 220;
               gParticles[j] = {
                   cx, cy,
                   cosf(angle) * speed, sinf(angle) * speed,
                   0.35f + (rand() % 50) / 100.f,
                   0.35f + (rand() % 50) / 100.f,
                   2.f + static_cast<float>(rand() % 5),
                   baseColor, true
               };
               break;
           }
       }
   }
}

void spawnShieldBreak(float cx, float cy)
{
   spawnExplosion(cx, cy, 28, sf::Color(0, 200, 255));
}

void updateParticles(float dt)
{
   for (int i = 0; i < MAX_PARTICLES; ++i)
   {
       if (!gParticles[i].active) continue;
       gParticles[i].x += gParticles[i].vx * dt;
       gParticles[i].y += gParticles[i].vy * dt;
       gParticles[i].vx *= 0.97f;
       gParticles[i].vy *= 0.97f;
       gParticles[i].lifetime -= dt;
       if (gParticles[i].lifetime <= 0.f) gParticles[i].active = false;
   }
}

void drawParticles(sf::RenderWindow& win)
{
   // SFML 3: CircleShape constructor takes a float radius (unchanged)
   sf::CircleShape c(1.f);
   for (int i = 0; i < MAX_PARTICLES; ++i)
   {
       if (!gParticles[i].active) continue;
       float     alpha = gParticles[i].lifetime / gParticles[i].maxLife;
       sf::Color col = gParticles[i].color;
       col.a = static_cast<std::uint8_t>(alpha * 255);
       c.setRadius(gParticles[i].radius);
       // SFML 3: setOrigin takes sf::Vector2f
       c.setOrigin(sf::Vector2f(gParticles[i].radius, gParticles[i].radius));
       // SFML 3: setPosition takes sf::Vector2f
       c.setPosition(sf::Vector2f(gParticles[i].x, gParticles[i].y));
       c.setFillColor(col);
       win.draw(c);
   }
}

float gShakeIntensity = 0.f, gShakeDuration = 0.f, gShakeTimer = 0.f;

void triggerScreenShake(float intensity, float duration)
{
   gShakeIntensity = intensity;
   gShakeDuration = duration;
   gShakeTimer = duration;
}

void updateShake(float dt)
{
   if (gShakeTimer > 0.f) gShakeTimer -= dt;
}

sf::Vector2f getShakeOffset()
{
   if (gShakeTimer <= 0.f) return { 0.f, 0.f };
   float ratio = gShakeTimer / gShakeDuration;
   float mag = gShakeIntensity * ratio;
   float ox = ((rand() % 200) - 100) / 100.f * mag;
   float oy = ((rand() % 200) - 100) / 100.f * mag;
   return { ox, oy };
}

struct ScrollingBg
{
   sf::Sprite s1, s2;
   float      speed = 80.f;

   // SFML 3: sf::Sprite constructor requires a texture reference
   ScrollingBg() : s1(gAssets.texBackground), s2(gAssets.texBackground) {}

   void init()
   {
       // Re-set texture (assets may have been (re)loaded after construction)
       s1.setTexture(gAssets.texBackground);
       s2.setTexture(gAssets.texBackground);

       // SFML 3: getSize() returns sf::Vector2u (unchanged)
       auto sz = gAssets.texBackground.getSize();
       float sx = static_cast<float>(WIN_W) / sz.x;
       float sy = static_cast<float>(WIN_H) / sz.y;
       // SFML 3: setScale takes sf::Vector2f
       s1.setScale(sf::Vector2f(sx, sy));
       s2.setScale(sf::Vector2f(sx, sy));
       // SFML 3: setPosition takes sf::Vector2f
       s1.setPosition(sf::Vector2f(0.f, 0.f));
       s2.setPosition(sf::Vector2f(0.f, -static_cast<float>(WIN_H)));
   }

   void update(float dt)
   {
       // SFML 3: move takes sf::Vector2f
       s1.move(sf::Vector2f(0.f, speed * dt));
       s2.move(sf::Vector2f(0.f, speed * dt));
       if (s1.getPosition().y >= WIN_H)
           s1.setPosition(sf::Vector2f(0.f, s2.getPosition().y - WIN_H));
       if (s2.getPosition().y >= WIN_H)
           s2.setPosition(sf::Vector2f(0.f, s1.getPosition().y - WIN_H));
   }

   void draw(sf::RenderWindow& win) { win.draw(s1); win.draw(s2); }
} gBg;

static void drawText(sf::RenderWindow& win, const string& str,
   float x, float y, unsigned sz, sf::Color col)
{
   // SFML 3: sf::Text constructor requires (font, string, characterSize)
   sf::Text t(gAssets.font, str, sz);
   t.setFillColor(col);
   t.setPosition(sf::Vector2f(x, y));
   win.draw(t);
}

static void drawTextC(sf::RenderWindow& win, const string& str,
   float y, unsigned sz, sf::Color col)
{
   sf::Text t(gAssets.font, str, sz);
   t.setFillColor(col);
   auto b = t.getLocalBounds();
   // SFML 3: getLocalBounds() returns sf::FloatRect with position + size
   t.setPosition(sf::Vector2f((WIN_W - b.size.x) / 2.f, y));
   win.draw(t);
}

class enemy;
class Boss;

class gameObject {
public:
   virtual void draw(sf::RenderWindow& win) = 0;
   virtual void onCollision() = 0;
   virtual void update(float dt) = 0;
   virtual ~gameObject() {}
};

class entity : public gameObject {
   float x, y, velocity_x, velocity_y, length, width;
   bool  isAlive;
protected:
   sf::Sprite sprite;
public:
   entity(float x, float y, float length, float width)
       : x(x), y(y), velocity_x(0), velocity_y(0),
       length(length), width(width), isAlive(true),
       // SFML 3: Sprite must be constructed with a texture
       sprite(gAssets.texPlayer)   // temporary; overwritten by setupSprite()
   {
   }

   void setupSprite(const sf::Texture& tex)
   {
       // SFML 3: setTexture still exists on Sprite
       sprite.setTexture(tex);
       auto sz = tex.getSize();
       // SFML 3: setScale takes sf::Vector2f
       sprite.setScale(sf::Vector2f(width / sz.x, length / sz.y));
       sprite.setPosition(sf::Vector2f(x, y));
   }

   float getX() { return x; }
   float getY() { return y; }
   float getWidth() { return width; }
   float getLength() { return length; }

   void setX(float v) { x = v; sprite.setPosition(sf::Vector2f(x, y)); }
   void setY(float v) { y = v; sprite.setPosition(sf::Vector2f(x, y)); }

   float getVelocity_X() { return velocity_x; }
   float getVelocity_Y() { return velocity_y; }

   bool getAlive() { return isAlive; }
   void setAlive(bool alive) { isAlive = alive; }

   bool collision(entity* obj)
   {
       float padding = 25.f;
       if ((x + width - padding) < obj->x ||
           (x + padding) > (obj->x + obj->width) ||
           (y + length - padding) < obj->y ||
           (y + padding) > (obj->y + obj->length))
           return false;
       return true;
   }

   virtual ~entity() {}
};

class player : public entity {
   int   lives, score, weaponType, shieldHits, empCount, multiplier;
   float killChainTimer;
   float dashCooldown, invincibleTimer, lastMoveDir;
   float hitFlashTimer;
   float shieldGlowTimer;
public:
   bool shieldJustBroke;

   player(int lives, float x, float y, float length, float width)
       : entity(x, y, length, width),
       lives(lives), score(0), weaponType(0), shieldHits(0),
       empCount(1), multiplier(1), killChainTimer(0.f),
       dashCooldown(0.f), invincibleTimer(0.f), lastMoveDir(1.f),
       hitFlashTimer(0.f), shieldGlowTimer(0.f), shieldJustBroke(false)
   {
       setupSprite(gAssets.texPlayer);
   }

   int   getScore() { return score; }
   int   getLives() { return lives; }
   int   getMultiplier() { return multiplier; }
   int   getWeaponType() { return weaponType; }
   int   getShieldHits() { return shieldHits; }
   int   getEmpCount() { return empCount; }
   float getDashCooldown() { return dashCooldown; }
   float getInvincible() { return invincibleTimer; }

   void draw(sf::RenderWindow& win) override
   {
       if (!getAlive()) return;

       if (invincibleTimer > 0.f)
       {
           bool show = (static_cast<int>(invincibleTimer * 12)) % 2 == 0;
           if (!show) return;
       }

       sf::Color tint = sf::Color::White;
       if (hitFlashTimer > 0.f)
           tint = sf::Color(255, 80, 80, 200);
       sprite.setColor(tint);
       win.draw(sprite);
       sprite.setColor(sf::Color::White);

       if (shieldHits > 0)
       {
           float pulse = 0.7f + 0.3f * sinf(shieldGlowTimer * 6.f);
           float radius = 20.f * pulse;
           sf::CircleShape shield(radius);
           // SFML 3: setOrigin / setPosition take sf::Vector2f
           shield.setOrigin(sf::Vector2f(radius, radius));
           shield.setPosition(sf::Vector2f(getX() + getWidth() / 2.f,
               getY() + getLength() / 2.f));
           shield.setFillColor(sf::Color(0, 200, 255, 50));
           shield.setOutlineColor(sf::Color(0, 220, 255, 180));
           shield.setOutlineThickness(2.f);
           win.draw(shield);
       }
   }

   void update(float dt) override
   {
       bool movingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
       bool movingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

       if (movingLeft) { setX(getX() - 350.f * dt); lastMoveDir = -1.f; }
       if (movingRight) { setX(getX() + 350.f * dt); lastMoveDir = 1.f; }

       if (getX() < 0)              setX(0);
       if (getX() > WIN_W - getWidth()) setX(WIN_W - getWidth());

       if (dashCooldown > 0.f) dashCooldown -= dt;
       if (invincibleTimer > 0.f) invincibleTimer -= dt;
       if (dashCooldown < 0.f) dashCooldown = 0.f;
       if (invincibleTimer < 0.f) invincibleTimer = 0.f;
       if (hitFlashTimer > 0.f) hitFlashTimer -= dt;
       if (shieldGlowTimer >= 0.f) shieldGlowTimer += dt;

       // SFML 3: keyboard key enum is sf::Keyboard::Key::E etc.
       if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E) &&
           dashCooldown <= 0.f &&
           (movingLeft || movingRight))
       {
           setX(getX() + 150.f * lastMoveDir);
           if (getX() < 0)          setX(0);
           if (getX() > WIN_W - 30) setX(static_cast<float>(WIN_W - 30));
           dashCooldown = 3.f;
           invincibleTimer = 0.5f;
           gAudio.playDash();
       }
   }

   void onCollision() override
   {
       if (invincibleTimer > 0.f) return;
       if (!absorbByShield())
       {
           lives--;
           multiplier = 1;
           killChainTimer = 0.f;
           hitFlashTimer = 0.4f;
           triggerScreenShake(10.f, 0.35f);
           gAudio.playPlayerHit();
       }
   }

   bool absorbByShield()
   {
       if (shieldHits > 0)
       {
           shieldHits--;
           if (shieldHits == 0)
           {
               shieldJustBroke = true;
               spawnShieldBreak(getX() + 15.f, getY() + 20.f);
               gAudio.playShieldBreak();
           }
           return true;
       }
       return false;
   }

   void applyPowerUp(int type)
   {
       if (type == powerUpSpread)  weaponType = 1;
       else if (type == powerUpPierce)  weaponType = 2;
       else if (type == powerUpShield) { shieldHits = 2; shieldGlowTimer = 0.f; }
       else if (type == powerUpEMP && empCount < 3) empCount++;
       gAudio.playPowerUp();
   }

   void addScore(int basePoints)
   {
       if (killChainTimer > 0.f)
       {
           if (multiplier == 1) multiplier = 2;
           else if (multiplier == 2) multiplier = 4;
       }
       score += basePoints * multiplier;
       killChainTimer = 3.f;
   }

   void updateChainTimer(float dt)
   {
       if (killChainTimer > 0.f)
       {
           killChainTimer -= dt;
           if (killChainTimer <= 0.f) { killChainTimer = 0.f; multiplier = 1; }
       }
   }

   // Defined after enemy + Boss
   void activateEMP(enemy** enemies, int& enemyCount, Boss*& currentBoss);
};
class bullet : public entity {
   bool isPlayerBullet, isPiercing;
   int  damage;
public:
   bullet(float x, float y, float length, float width,
       bool isPlayerBullet, bool isPiercing, int damage)
       : entity(x, y, length, width),
       isPlayerBullet(isPlayerBullet), isPiercing(isPiercing), damage(damage)
   {
       if (isPlayerBullet) setupSprite(gAssets.texPlayerBullet);
       else                setupSprite(gAssets.texEnemyBullet);
   }

   void draw(sf::RenderWindow& win) override { win.draw(sprite); }

   void onCollision() override { if (!isPiercing) setAlive(false); }

   void update(float dt) override
   {
       if (isPlayerBullet) setY(getY() - 400.f * dt);
       else                setY(getY() + 300.f * dt);
       if (getY() < -20.f || getY() > WIN_H + 20.f) setAlive(false);
   }

   bool getIsPlayerBullet() { return isPlayerBullet; }
   int  getDamage() { return damage; }
};
class enemy : public entity {
   int   health, score;
   float fireTimer;
   bool  wasKilled;
public:
   enemy(int health, int score, float x, float y, float length, float width)
       : entity(x, y, length, width), health(health), score(score), wasKilled(false)
   {
       fireTimer = static_cast<float>(rand() % 3 + 1);
   }

   virtual void shoot(bullet** bullets, int* bulletCount) = 0;
   virtual void draw(sf::RenderWindow& win) override { win.draw(sprite); }

   void onCollision() override
   {
       health--;
       if (health <= 0)
       {
           setAlive(false);
           wasKilled = true;
           spawnExplosion(getX() + getWidth() / 2.f, getY() + getLength() / 2.f);
           gAudio.playExplosion();
       }
   }

   virtual void update(float dt) override {}

   void  setfireTimer(float t) { fireTimer = t; }
   float getfireTimer() { return fireTimer; }
   bool  getWasKilled() { return wasKilled; }
   int   getScore() { return score; }
};

class drone : public enemy {
   bullet** bullets; int* bulletCount;
public:
   drone(float x, float y, bullet** b, int* bc)
       : enemy(1, 100, x, y, 90, 90), bullets(b), bulletCount(bc)
   {
       setupSprite(gAssets.texDrone);
   }

   void shoot(bullet** b, int* bc) override
   {
       if (*bc < 100)
       {
           float bx = getX() + (getWidth() / 2.f) - 10.f;
           float by = getY() + getLength();
           b[*bc] = new bullet(bx, by, 40, 25, false, false, 1);
           (*bc)++;
           gAudio.playEnemyFire();
       }
   }

   void update(float dt) override
   {
       setY(getY() + 200.f * dt);
       setfireTimer(getfireTimer() - dt);
       if (getfireTimer() <= 0.f)
       {
           shoot(bullets, bulletCount);
           setfireTimer(static_cast<float>(rand() % 3 + 1));
       }
       if (getY() > WIN_H + 30.f) setAlive(false);
   }
   void onCollision() override { enemy::onCollision(); }
};
class viper : public enemy {
   bullet** bullets; int* bulletCount;
   float time, startX;
public:
   viper(float x, float y, bullet** b, int* bc)
       : enemy(2, 150, x, y, 80, 80), bullets(b), bulletCount(bc), time(0), startX(x)
   {
       setupSprite(gAssets.texViper);
   }

   void shoot(bullet** b, int* bc) override
   {
       if (*bc < 100)
       {
           float bx = getX() + (getWidth() / 2.f) - 10.f;
           float by = getY() + getLength();
           b[*bc] = new bullet(bx, by, 40, 25, false, false, 1);
           (*bc)++;
           gAudio.playEnemyFire();
       }
   }

   void update(float dt) override
   {
       setY(getY() + 150.f * dt);
       time += dt;
       setX(startX + 80.f * sinf(time * 2.f));
       setfireTimer(getfireTimer() - dt);
       if (getfireTimer() <= 0.f)
       {
           shoot(bullets, bulletCount);
           setfireTimer(static_cast<float>(rand() % 3 + 1));
       }
       if (getY() > WIN_H + 30.f) setAlive(false);
   }
   void onCollision() override { enemy::onCollision(); }
};

class seeker : public enemy {
   float lockX, speed;
public:
   seeker(float x, float y, float lockX)
       : enemy(1, 120, x, y, 100, 100), lockX(lockX), speed(100.f)
   {
       setupSprite(gAssets.texSeeker);
   }

   void shoot(bullet**, int*) override {}

   void update(float dt) override
   {
       speed += 50.f * dt;
       setY(getY() + speed * dt);
       if (getX() < lockX) setX(getX() + 100.f * dt);
       else if (getX() > lockX) setX(getX() - 100.f * dt);
       if (getY() > WIN_H + 30.f) setAlive(false);
   }
   void onCollision() override { enemy::onCollision(); }
};
class Turret {
   float x, y, width, length;
   int   health;
   bool  isAlive;
   float fireTimer;
   bullet** bulletArray; int* bulletCount;
   player* targetPlayer;
public:
   Turret(float x, float y, bullet** ba, int* bc, player* p)
       : x(x), y(y), width(85), length(85), health(3),
       isAlive(true), fireTimer(static_cast<float>(rand() % 3 + 1)),
       bulletArray(ba), bulletCount(bc), targetPlayer(p)
   {
   }

   float getX() { return x; }
   float getY() { return y; }
   bool  getAlive() { return isAlive; }

   void setPosition(float nx, float ny) { x = nx; y = ny; }

   void onCollision()
   {
       health--;
       if (health <= 0)
       {
           isAlive = false;
           spawnExplosion(x + 10, y + 10, 70, sf::Color(150, 150, 150));
           gAudio.playExplosion();
       }
   }

   bool collision(entity* obj)
   {
       float ox = obj->getX(), oy = obj->getY();
       if ((x + width) < ox || x > (ox + obj->getWidth()) ||
           (y + length) < oy || y > (oy + obj->getLength()))
           return false;
       return true;
   }

   void shoot()
   {
       if (*bulletCount >= 100 || !targetPlayer) return;
       float playerCX = targetPlayer->getX() + targetPlayer->getWidth() / 2.f;
       bulletArray[*bulletCount] = new bullet(playerCX - 14.f, y + length,
           55, 28, false, false, 1);
       (*bulletCount)++;
       gAudio.playEnemyFire();
   }

   void update(float dt)
   {
       if (!isAlive) return;
       fireTimer -= dt;
       if (fireTimer <= 0.f) { shoot(); fireTimer = static_cast<float>(rand() % 3 + 1); }
   }

   void draw(sf::RenderWindow& win)
   {
       if (!isAlive) return;
       // SFML 3: Sprite must be constructed with a texture
       sf::Sprite s(gAssets.texTurret);
       auto sz = gAssets.texTurret.getSize();
       s.setScale(sf::Vector2f(width / sz.x, length / sz.y));
       s.setPosition(sf::Vector2f(x, y));
       win.draw(s);
   }
};
class Boss : public enemy {
protected:
   int  maxHealth, currentHealth, phase;
   bool inFight;
public:
   Boss(int health, int score, float x, float y, float length, float width)
       : enemy(health, score, x, y, length, width),
       maxHealth(health), currentHealth(health), phase(1), inFight(true)
   {
   }

   virtual void shootPattern(bullet** ba, int* bc) = 0;

   int  getPhase() { return phase; }
   int  getCurrentHP() { return currentHealth; }
   int  getMaxHP() { return maxHealth; }
   bool isFightActive() { return inFight; }

   void onCollision() override
   {
       currentHealth--;
       if (currentHealth <= 0)
       {
           setAlive(false);
           for (int i = 0; i < 4; ++i)
               spawnExplosion(
                   getX() + static_cast<float>(rand() % static_cast<int>(getWidth())),
                   getY() + static_cast<float>(rand() % static_cast<int>(getLength())),
                   25, sf::Color(255, 80, 0));
           gAudio.playExplosion();
       }
   }

   virtual ~Boss() {}
};
class Cruiser : public Boss {
   float sweepSpeed, sweepDir, attackTimer;
   int   safeGap;
   bullet** bulletArray; int* bulletCount;
public:
   Cruiser(float x, float y, bullet** ba, int* bc)
       : Boss(30, 500, x, y, 190, 310),
       sweepSpeed(100.f), sweepDir(1.f), attackTimer(3.5f),
       safeGap(rand() % 5), bulletArray(ba), bulletCount(bc)
   {
       setupSprite(gAssets.texCruiser);
   }

   void shootPattern(bullet** ba, int* bc) override
   {
       const int COLS = 9;
       safeGap = rand() % COLS;
       float spacing = static_cast<float>(WIN_W) / COLS;
       for (int col = 0; col < COLS; ++col)
       {
           if (col == safeGap) continue;
           if (*bc < 100)
           {
               float bx = col * spacing + spacing / 2.f;
               ba[*bc] = new bullet(bx, getY() + 50.f, 18, 20, false, false, 1);
               (*bc)++;
           }
       }
       gAudio.playEnemyFire();
   }

   void shoot(bullet** ba, int* bc) override { shootPattern(ba, bc); }

   void update(float dt) override
   {
       setX(getX() + sweepSpeed * sweepDir * dt);
       if (getX() <= 0) { setX(0);                          sweepDir = 1.f; }
       if (getX() >= WIN_W - 315) { setX(static_cast<float>(WIN_W - 315)); sweepDir = -1.f; }

       attackTimer -= dt;
       if (attackTimer <= 0.f)
       {
           shootPattern(bulletArray, bulletCount);
           attackTimer = 3.f;
       }
   }

   void draw(sf::RenderWindow& win) override { win.draw(sprite); }

   void onCollision() override
   {
       Boss::onCollision();
       if (currentHealth <= maxHealth / 2 && phase == 1)
       {
           phase = 2;
           sweepSpeed = 160.f;
           attackTimer = 2.5f;
       }
   }
};
class TwinCannons : public Boss {
   Turret* leftTurret;
   Turret* rightTurret;
   bool    turretsDead;
   bullet** bulletArray; int* bulletCount;
   float    moveDir;
   player* targetPlayer;
public:
   TwinCannons(float x, float y, bullet** ba, int* bc, player* p)
       : Boss(60, 800, x, y, 150, 220),
       turretsDead(false), bulletArray(ba), bulletCount(bc),
       moveDir(1.f), targetPlayer(p)
   {
       setupSprite(gAssets.texTwinCannons);
       leftTurret = new Turret(x - 90.f, y + 35.f, ba, bc, p);
       rightTurret = new Turret(x + 290.f + 10.f, y + 35.f, ba, bc, p);
   }

   void shootPattern(bullet** ba, int* bc) override
   {
       if (!turretsDead || !targetPlayer) return;
       float playerCX = targetPlayer->getX() + targetPlayer->getWidth() / 2.f;
       float by = getY() + getLength();
       int offsets[] = { -35, 0, 35 };
       for (int k = 0; k < 3; ++k)
           if (*bc < 100)
           {
               ba[*bc] = new bullet(playerCX + offsets[k] - 12.f,
                   by, 55, 28, false, false, 2);
               (*bc)++;
           }
       gAudio.playEnemyFire();
   }

   void shoot(bullet** ba, int* bc) override { shootPattern(ba, bc); }

   void onCollision() override
   {
       if (!turretsDead) return;
       Boss::onCollision();
   }

   void checkBulletVsTurrets(bullet** bullets, int bulletCount)
   {
       for (int i = 0; i < bulletCount; ++i)
       {
           if (!bullets[i]->getIsPlayerBullet()) continue;
           if (leftTurret->getAlive() && leftTurret->collision(bullets[i]))
           {
               spawnExplosion(bullets[i]->getX(), bullets[i]->getY(),
                   8, sf::Color(200, 200, 200));
               leftTurret->onCollision();
               bullets[i]->onCollision();
           }
           if (rightTurret->getAlive() && rightTurret->collision(bullets[i]))
           {
               spawnExplosion(bullets[i]->getX(), bullets[i]->getY(),
                   8, sf::Color(200, 200, 200));
               rightTurret->onCollision();
               bullets[i]->onCollision();
           }
       }
       if (!leftTurret->getAlive() && !rightTurret->getAlive() && !turretsDead)
           turretsDead = true;
   }

   bool areTurretsDead() { return turretsDead; }

   void update(float dt) override
   {
       setX(getX() + 100.f * moveDir * dt);
       if (getX() <= 50.f) { setX(50.f);                          moveDir = 1.f; }
       if (getX() >= WIN_W - 300.f) { setX(static_cast<float>(WIN_W - 300)); moveDir = -1.f; }

       leftTurret->setPosition(getX() - 90.f, getY() + 35.f);
       rightTurret->setPosition(getX() + getWidth() + 10.f, getY() + 35.f);
       leftTurret->update(dt);
       rightTurret->update(dt);

       if (turretsDead)
       {
           setfireTimer(getfireTimer() - dt);
           if (getfireTimer() <= 0.f)
           {
               shootPattern(bulletArray, bulletCount);
               setfireTimer(1.f);
           }
       }
   }

   void draw(sf::RenderWindow& win) override
   {
       win.draw(sprite);
       leftTurret->draw(win);
       rightTurret->draw(win);
   }

   ~TwinCannons() { delete leftTurret; delete rightTurret; }
};
class Mothership : public Boss {
   bullet** bulletArray; int* bulletCount;
   enemy** enemyArray;  int* enemyCount;
   player* playerPos;

   float seekerSpawnTimer, laserWarTimer, laserFireTimer;
   bool  laserWarActive, laserFiringActive;
   float sweepDir;
public:
   Mothership(float x, float y, bullet** ba, int* bc,
       enemy** ea, int* ec, player* p)
       : Boss(100, 2000, x, y, 300, 370),
       bulletArray(ba), bulletCount(bc),
       enemyArray(ea), enemyCount(ec), playerPos(p),
       seekerSpawnTimer(3.f), laserWarTimer(3.f), laserFireTimer(0.f),
       laserWarActive(false), laserFiringActive(false), sweepDir(1.f)
   {
       setupSprite(gAssets.texMothership);
   }

   bool isLaserWarning() { return laserWarActive; }
   bool isLaserFiring() { return laserFiringActive; }

   void shootPattern(bullet** ba, int* bc) override
   {
       const float SAFE_ZONE_WIDTH = 200.f;
       const float available = static_cast<float>(WIN_W) - SAFE_ZONE_WIDTH * 2.f;
       const int   COLS = 12;
       const float colWidth = available / COLS;
       const float laserY = getY() + getLength();

       for (int col = 0; col < COLS; ++col)
       {
           if (*bc >= 100) break;
           float bx = SAFE_ZONE_WIDTH + (col * colWidth);
           ba[*bc] = new bullet(bx, laserY, 65, static_cast<int>(colWidth),
               false, false, 1);
           (*bc)++;
       }
       gAudio.playEnemyFire();
   }

   void shoot(bullet** ba, int* bc) override { shootPattern(ba, bc); }

   void spawnSeeker()
   {
       if (*enemyCount >= 48) return;
       float sx = static_cast<float>(rand() % 700 + 25);
       enemyArray[*enemyCount] = new seeker(sx, getY() + 80.f, playerPos->getX());
       (*enemyCount)++;
   }

   void update(float dt) override
   {
       setX(getX() + 80.f * sweepDir * dt);
       if (getX() <= 50.f) { setX(50.f);                          sweepDir = 1.f; }
       if (getX() >= WIN_W - 380.f) { setX(static_cast<float>(WIN_W - 380)); sweepDir = -1.f; }

       seekerSpawnTimer -= dt;
       if (seekerSpawnTimer <= 0.f) { spawnSeeker(); seekerSpawnTimer = 5.f; }

       laserWarTimer -= dt;
       if (laserWarTimer <= 0.f && !laserWarActive && !laserFiringActive)
       {
           laserWarActive = true;
           laserFireTimer = 1.f;
       }

       if (laserWarActive)
       {
           laserFireTimer -= dt;
           if (laserFireTimer <= 0.f)
           {
               laserWarActive = false;
               laserFiringActive = true;
               shootPattern(bulletArray, bulletCount);
               laserFireTimer = 0.25f;
               laserWarTimer = 6.f;
           }
       }

       if (laserFiringActive)
       {
           laserFireTimer -= dt;
           if (laserFireTimer <= 0.f) laserFiringActive = false;
       }

       if (currentHealth <= maxHealth / 2 && phase == 1)
       {
           phase = 2;
           seekerSpawnTimer = 3.f;
           laserWarTimer = 4.f;
       }
   }

   void draw(sf::RenderWindow& win) override
   {
       const float SAFE_ZONE_WIDTH = 200.f;
       const float laserWidth = static_cast<float>(WIN_W) - SAFE_ZONE_WIDTH * 2.f;
       const float laserY = getY() + getLength();

       if (laserWarActive)
       {
           float alpha = 128.f + 127.f * sinf(laserFireTimer * 20.f);
           // SFML 3: RectangleShape constructor takes sf::Vector2f
           sf::RectangleShape bar(sf::Vector2f(laserWidth, 20.f));
           bar.setPosition(sf::Vector2f(SAFE_ZONE_WIDTH, laserY));
           bar.setFillColor(sf::Color(255, 0, 0, static_cast<std::uint8_t>(alpha)));
           win.draw(bar);
       }

       if (laserFiringActive)
       {
           sf::RectangleShape glow(sf::Vector2f(laserWidth, 40.f));
           glow.setPosition(sf::Vector2f(SAFE_ZONE_WIDTH, laserY - 5.f));
           glow.setFillColor(sf::Color(255, 0, 0, 80));
           win.draw(glow);

           sf::RectangleShape beam(sf::Vector2f(laserWidth, 24.f));
           beam.setPosition(sf::Vector2f(SAFE_ZONE_WIDTH, laserY + 3.f));
           beam.setFillColor(sf::Color(255, 80, 80, 230));
           win.draw(beam);

           sf::RectangleShape core(sf::Vector2f(
               static_cast<float>(WIN_W) - SAFE_ZONE_WIDTH * 2.f, 6.f));
           core.setPosition(sf::Vector2f(SAFE_ZONE_WIDTH, laserY + 12.f));
           core.setFillColor(sf::Color(255, 220, 220, 255));
           win.draw(core);
       }

       win.draw(sprite);
   }

   void onCollision() override { Boss::onCollision(); }
   ~Mothership() {}
};
class powerUp : public entity {
   int   type;
   float fallSpeed;
public:
   powerUp(float x, float y, int type)
       : entity(x, y, 100, 100), type(type), fallSpeed(90.f)
   {
       switch (type)
       {
       case powerUpSpread: setupSprite(gAssets.texSpread); break;
       case powerUpPierce: setupSprite(gAssets.texPierce); break;
       case powerUpShield: setupSprite(gAssets.texShield); break;
       case powerUpEMP:    setupSprite(gAssets.texEMP);    break;
       }
   }

   int getType() { return type; }

   void update(float dt) override
   {
       setY(getY() + fallSpeed * dt);
       if (getY() > WIN_H + 30.f) setAlive(false);
   }

   void onCollision() override { setAlive(false); }

   void draw(sf::RenderWindow& win) override { win.draw(sprite); }
};
class asteroid : public entity {
   float speed;
public:
   asteroid(float x, float y, float size)
       : entity(x, y, size, size), speed(110.f)
   {
       setupSprite(gAssets.texAsteroid);
   }

   void update(float dt) override
   {
       setY(getY() + speed * dt);
       if (getY() > WIN_H + 50.f)
       {
           setX(static_cast<float>(rand() % 700 + 30));
           setY(-50.f);
       }
   }

   void onCollision() override {}
   void draw(sf::RenderWindow& win) override { win.draw(sprite); }
};
void player::activateEMP(enemy** enemies, int& enemyCount, Boss*& currentBoss)
{
   if (empCount <= 0) return;
   empCount--;

   for (int i = 0; i < enemyCount; ++i)
       enemies[i]->setAlive(false);

   if (currentBoss)
       for (int i = 0; i < 10; ++i)
           currentBoss->onCollision();

   gAudio.playEMP();
}
class GameStateManager {
   int state, mode;
public:
   GameStateManager() : state(stateIntro), mode(0) {}

   int  getState() { return state; }
   int  getMode() { return mode; }

   bool isIntro() { return state == stateIntro; }
   bool isInstructions() { return state == stateInstructions; }
   bool isMenu() { return state == stateMenu; }
   bool isPlaying() { return state == statePlaying; }
   bool isPaused() { return state == statePaused; }
   bool isGameOver() { return state == stateGameOver; }
   bool isWin() { return state == stateWin; }

   void goMenu() { state = stateMenu; mode = 0; }
   void goInstructions() { state = stateInstructions; }

   void startArcade()
   {
       mode = modeArcade;
       state = statePlaying;
       gAudio.playNormalMusic();
   }
   void startSurvival()
   {
       mode = modeSurvival;
       state = statePlaying;
       gAudio.playNormalMusic();
   }
   void togglePause()
   {
       if (state == statePlaying) state = statePaused;
       else if (state == statePaused)  state = statePlaying;
   }
   void triggerGameOver() { state = stateGameOver; gAudio.stopAll(); }
   void triggerWin() { state = stateWin;      gAudio.stopAll(); }
   void returnToMenu() { state = stateMenu; mode = 0; gAudio.stopAll(); }
};
class SurvivalManager {
   int   waveNumber, enemiesToSpawn;
   float speedMult, fireMult;
   bool  waveInProgress;
public:
   SurvivalManager()
       : waveNumber(1), enemiesToSpawn(5),
       speedMult(1.f), fireMult(1.f), waveInProgress(false) {
   }

   int   getWaveNumber() { return waveNumber; }
   int   getEnemiesToSpawn() { return enemiesToSpawn; }
   float getSpeedMult() { return speedMult; }
   float getFireMult() { return fireMult; }
   bool  isWaveInProgress() { return waveInProgress; }

   void startWave() { waveInProgress = true; }
   void waveCleared()
   {
       waveInProgress = false;
       waveNumber++;
       enemiesToSpawn += 2;
       speedMult += 0.05f;
       fireMult += 0.10f;
   }
   bool shouldSpawnViper() { return waveNumber >= 5; }
   bool shouldSpawnSeeker() { return waveNumber >= 8; }
};
void drawHUD(sf::RenderWindow& win, player* p, GameStateManager& gsm,
   SurvivalManager& sm, int level, int kills, int killThresh)
{
   sf::RectangleShape hudBg(sf::Vector2f(static_cast<float>(WIN_W), 165.f));
   hudBg.setPosition(sf::Vector2f(0.f, 0.f));
   hudBg.setFillColor(sf::Color(0, 0, 0, 160));
   win.draw(hudBg);

   sf::Color white(255, 255, 255);
   sf::Color yellow(255, 220, 0);
   sf::Color cyan(0, 220, 255);
   sf::Color green(100, 255, 100);

   drawText(win, "SCORE: " + to_string(p->getScore()), 8, 10, 30, white);
   drawText(win, "x" + to_string(p->getMultiplier()), 240, 10, 28, yellow);
   drawText(win, "LIVES: " + to_string(p->getLives()), 380, 10, 30, white);

   string wname = "Standard";
   if (p->getWeaponType() == 1) wname = "Spread";
   if (p->getWeaponType() == 2) wname = "Pierce";
   drawText(win, "WEAPON: " + wname, 8, 55, 25, cyan);

   string shieldStr = (p->getShieldHits() > 0)
       ? "SHIELD: " + to_string(p->getShieldHits()) + " hits"
       : "SHIELD: --";
   drawText(win, shieldStr, 320, 55, 25,
       p->getShieldHits() > 0 ? cyan : sf::Color(120, 120, 120));

   drawText(win, "EMP: " + to_string(p->getEmpCount()), 530, 32, 25, yellow);

   if (gsm.getMode() == modeArcade)
       drawText(win, "LEVEL " + to_string(level) +
           "   KILLS " + to_string(kills) + "/" + to_string(killThresh),
           8, 105, 20, green);
   else
       drawText(win, "WAVE " + to_string(sm.getWaveNumber()), 8, 105, 20, green);

   float dc = p->getDashCooldown();
   drawText(win, "DASH:", 8, 135, 20, white);

   if (dc <= 0.f)
   {
       drawText(win, "[  READY  ]", 62, 78, 20, sf::Color(0, 255, 120));
   }
   else
   {
       float ratio = 1.f - (dc / 3.f);
       const int BAR = 120;
       sf::RectangleShape bg(sf::Vector2f(static_cast<float>(BAR), 14.f));
       bg.setPosition(sf::Vector2f(80.f, 137.f));
       bg.setFillColor(sf::Color(60, 60, 60));
       win.draw(bg);

       sf::RectangleShape fill(sf::Vector2f(BAR * ratio, 14.f));
       fill.setPosition(sf::Vector2f(80.f, 137.f));
       fill.setFillColor(sf::Color(0, 200, 255));
       win.draw(fill);

       int wh = static_cast<int>(dc);
       int fr = static_cast<int>((dc - wh) * 10);
       drawText(win, to_string(wh) + "." + to_string(fr) + "s", 190, 78, 20, white);
   }

   if (p->getInvincible() > 0.f)
       drawText(win, "INVINCIBLE!", 350, 78, 20, sf::Color(255, 255, 0));
}

void drawBossHP(sf::RenderWindow& win, Boss* boss, const string& name)
{
   int   hp = boss->getCurrentHP();
   int   maxhp = boss->getMaxHP();
   float ratio = static_cast<float>(hp) / maxhp;

   sf::RectangleShape sep(sf::Vector2f(static_cast<float>(WIN_W), 2.f));
   sep.setPosition(sf::Vector2f(0.f, 168.f));
   sep.setFillColor(sf::Color(80, 80, 80));
   win.draw(sep);

   sf::RectangleShape bg(sf::Vector2f(WIN_W - 20.f, 20.f));
   bg.setPosition(sf::Vector2f(10.f, 170.f));
   bg.setFillColor(sf::Color(60, 0, 0));
   win.draw(bg);

   std::uint8_t r = 255;
   std::uint8_t g = static_cast<std::uint8_t>(ratio * 160.f);
   sf::RectangleShape fill(sf::Vector2f((WIN_W - 20.f) * ratio, 20.f));
   fill.setPosition(sf::Vector2f(10.f, 170.f));
   fill.setFillColor(sf::Color(r, g, 0));
   win.draw(fill);

   sf::RectangleShape outline(sf::Vector2f(WIN_W - 20.f, 20.f));
   outline.setPosition(sf::Vector2f(10.f, 170.f));
   outline.setFillColor(sf::Color::Transparent);
   outline.setOutlineColor(sf::Color(200, 200, 200));
   outline.setOutlineThickness(1.f);
   win.draw(outline);

   drawText(win, name + "  " + to_string(hp) + "/" + to_string(maxhp),
       15, 172, 20, sf::Color::White);
}
void spawnSurvivalWave(enemy** enemies, int& enemyCount,
   bullet** bullets, int& bulletCount,
   SurvivalManager& sm, player* p)
{
   int n = sm.getEnemiesToSpawn();
   bool laneOccupied[10] = { false };

   for (int i = 0; i < n && enemyCount < 48; ++i)
   {
       int lane = rand() % 10;
       int attempts = 0;
       while (laneOccupied[lane] && attempts < 10)
       {
           lane = (lane + 1) % 10;
           attempts++;
       }
       laneOccupied[lane] = true;

       float sx = (lane * 110.f) + 50.f;
       float sy = -50.f - (i * 100.f);

       int maxT = 1;
       if (sm.shouldSpawnViper())  maxT = 2;
       if (sm.shouldSpawnSeeker()) maxT = 3;
       int t = rand() % maxT;

       if (t == 0) enemies[enemyCount++] = new drone(sx, sy, bullets, &bulletCount);
       else if (t == 1) enemies[enemyCount++] = new viper(sx, sy, bullets, &bulletCount);
       else             enemies[enemyCount++] = new seeker(sx, sy, p->getX());

       if (i % 10 == 9)
           for (int j = 0; j < 10; j++) laneOccupied[j] = false;
   }
}

void spawnOneArcadeEnemy(int level, enemy** enemies, int& enemyCount,
   bullet** bullets, int& bulletCount, player* p)
{
   if (enemyCount >= 48) return;

   int   lane = rand() % 10;
   float sx = (lane * 110.f) + 50.f;
   float sy = -50.f - static_cast<float>(rand() % 150);
   int   roll = rand() % 10;

   if (level == 1)
   {
       if (roll < 7) enemies[enemyCount++] = new drone(sx, sy, bullets, &bulletCount);
       else          enemies[enemyCount++] = new seeker(sx, sy, p->getX());
   }
   else if (level == 2)
   {
       if (roll < 4) enemies[enemyCount++] = new drone(sx, sy, bullets, &bulletCount);
       else if (roll < 8) enemies[enemyCount++] = new seeker(sx, sy, p->getX());
       else               enemies[enemyCount++] = new viper(sx, sy, bullets, &bulletCount);
   }
   else
   {
       int typeRoll = rand() % 3;
       if (typeRoll == 0) enemies[enemyCount++] = new drone(sx, sy, bullets, &bulletCount);
       else if (typeRoll == 1) enemies[enemyCount++] = new viper(sx, sy, bullets, &bulletCount);
       else                    enemies[enemyCount++] = new seeker(sx, sy, p->getX());
   }
}
int main()
{
   srand(static_cast<unsigned>(time(nullptr)));

   // SFML 3: sf::RenderWindow constructor — sf::VideoMode takes sf::Vector2u
   sf::RenderWindow window(
       sf::VideoMode(sf::Vector2u(WIN_W, WIN_H)),
       "Space Invaders",
       sf::Style::Titlebar | sf::Style::Close);
   window.setFramerateLimit(60);

   gAssets.load();
   gAudio.load();
   gBg.init();
   gAudio.playNormalMusic();

   GameStateManager gsm;
   SurvivalManager  sm;

   float playerwidth = 170.f;
   float playerHeight = 250.f;
   player* player1 = new player(50, (WIN_W - playerwidth) / 2.f, 850.f,
       playerHeight, playerwidth);

   bullet** bullets = new bullet * [50];
   int bulletCount = 0;
   float fireTimer = 0.f;

   enemy** enemies = new enemy * [50];
   int enemyCount = 0;

   powerUp** powerUps = new powerUp * [50];
   int powerUpCount = 0;

   const int ASTEROID_COUNT = 4;
   asteroid* ast[ASTEROID_COUNT] = {
       new asteroid(80.f,  80.f,  50.f),
       new asteroid(280.f, 350.f, 65.f),
       new asteroid(500.f, 180.f, 42.f),
       new asteroid(680.f, 520.f, 80.f)
   };

   Boss* currentBoss = nullptr;
   int   currentLevel = 1;
   bool  bossActive = false;

   int   arcadeKillCount = 0;
   const int BOSS_KILL_THRESHOLD = 10;
   float arcadeSpawnTimer = 0.f;
   const float ARCADE_SPAWN_INTERVAL = 2.f;

   float introY = static_cast<float>(WIN_H) + 60.f;
   float introTarget = 600.f;

   float empFlashTimer = 0.f;

   // SFML 3: sf::View constructor is unchanged
   sf::View gameView(sf::FloatRect(sf::Vector2f(0.f, 0.f),
       sf::Vector2f(static_cast<float>(WIN_W),
           static_cast<float>(WIN_H))));

   sf::Clock clock;

   float  levelCompleteTimer = 0.f;
   string levelCompleteMsg = "";

   // ── MAIN LOOP ────────────────────────────────────────────────────────────
   while (window.isOpen())
   {
       float dt = clock.restart().asSeconds();
       if (dt > 0.05f) dt = 0.05f;

       // ── EVENT LOOP ───────────────────────────────────────────────────────
       // SFML 3: pollEvent returns std::optional<sf::Event>
       while (auto evOpt = window.pollEvent())
       {
           const sf::Event& ev = *evOpt;

           // SFML 3: event type checking via std::holds_alternative / is<>
           if (ev.is<sf::Event::Closed>())
               window.close();

           // SFML 3: KeyPressed event data via getIf<>
           if (const auto* kp = ev.getIf<sf::Event::KeyPressed>())
           {
               if (gsm.isInstructions())
                   gsm.goMenu();

               else if (gsm.isMenu())
               {
                   if (kp->code == sf::Keyboard::Key::Num1 ||
                       kp->code == sf::Keyboard::Key::Numpad1)
                   {
                       gsm.startArcade();
                       arcadeKillCount = 0; arcadeSpawnTimer = 0.f;
                       for (int i = 0; i < 5; ++i)
                           spawnOneArcadeEnemy(1, enemies, enemyCount,
                               bullets, bulletCount, player1);
                   }
                   else if (kp->code == sf::Keyboard::Key::Num2 ||
                       kp->code == sf::Keyboard::Key::Numpad2)
                   {
                       gsm.startSurvival();
                       sm.startWave();
                       spawnSurvivalWave(enemies, enemyCount, bullets,
                           bulletCount, sm, player1);
                   }
                   else if (kp->code == sf::Keyboard::Key::Num3 ||
                       kp->code == sf::Keyboard::Key::Numpad3 ||
                       kp->code == sf::Keyboard::Key::I)
                       gsm.goInstructions();
                   else if (kp->code == sf::Keyboard::Key::Escape)
                       window.close();
               }

               else if (gsm.isPlaying() && kp->code == sf::Keyboard::Key::Escape)
                   gsm.togglePause();

               else if (gsm.isPaused())
               {
                   if (kp->code == sf::Keyboard::Key::Escape)
                       gsm.togglePause();
                   else if (kp->code == sf::Keyboard::Key::Q)
                   {
                       for (int i = 0; i < enemyCount; ++i) delete enemies[i];
                       for (int i = 0; i < bulletCount; ++i) delete bullets[i];
                       for (int i = 0; i < powerUpCount;++i) delete powerUps[i];
                       if (currentBoss) { delete currentBoss; currentBoss = nullptr; }
                       enemyCount = bulletCount = powerUpCount = 0;
                       bossActive = false; currentLevel = 1;
                       arcadeKillCount = 0; arcadeSpawnTimer = 0.f;
                       delete player1;
                       player1 = new player(3, 525.f, 820.f, 200, 150);
                       sm = SurvivalManager();
                       gsm.returnToMenu();
                   }
               }

               else if ((gsm.isGameOver() || gsm.isWin()) &&
                   kp->code == sf::Keyboard::Key::R)
               {
                   for (int i = 0; i < enemyCount; ++i) delete enemies[i];
                   for (int i = 0; i < bulletCount; ++i) delete bullets[i];
                   for (int i = 0; i < powerUpCount;++i) delete powerUps[i];
                   if (currentBoss) { delete currentBoss; currentBoss = nullptr; }
                   enemyCount = bulletCount = powerUpCount = 0;
                   bossActive = false; currentLevel = 1;
                   arcadeKillCount = 0; arcadeSpawnTimer = 0.f;
                   delete player1;
                   player1 = new player(3, 525.f, 820.f, 200, 150);
                   sm = SurvivalManager();
                   gsm.returnToMenu();
               }

               else if (gsm.isPlaying() && kp->code == sf::Keyboard::Key::N)
               {
                   player1->activateEMP(enemies, enemyCount, currentBoss);
                   empFlashTimer = 0.2f;
               }
           }
       }

       gBg.update(dt);
       updateParticles(dt);
       updateShake(dt);
       if (empFlashTimer > 0.f) empFlashTimer -= dt;
       if (levelCompleteTimer > 0.f) levelCompleteTimer -= dt;

       if (gsm.isIntro())
       {
           introY -= 100.f * dt;
           player1->setY(introY);
           if (introY <= introTarget)
           {
               player1->setY(850.f);
               gsm.goMenu();
           }
       }

       if (gsm.isPlaying())
       {
           player1->update(dt);
           player1->updateChainTimer(dt);

           if (player1->getLives() <= 0)
           {
               gsm.triggerGameOver();
               spawnExplosion(player1->getX() + 15.f, player1->getY() + 20.f, 30);
               triggerScreenShake(15.f, 0.6f);
           }

           if (player1->shieldJustBroke)
               player1->shieldJustBroke = false;

           fireTimer -= dt;
           // SFML 3: isKeyPressed uses sf::Keyboard::Key enum
           if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && fireTimer <= 0.f)
           {
               int   wt = player1->getWeaponType();
               float bx = player1->getX() + (player1->getWidth() / 2.f) - 2.f;
               float by = player1->getY();

               if (wt == 0 && bulletCount < 100)
                   bullets[bulletCount++] = new bullet(bx, by, 50, 30, true, false, 1);
               else if (wt == 1)
               {
                   if (bulletCount < 100) bullets[bulletCount++] = new bullet(bx, by, 50.f, 25.f, true, false, 1);
                   if (bulletCount < 100) bullets[bulletCount++] = new bullet(bx - 35.f, by, 50.f, 25.f, true, false, 1);
                   if (bulletCount < 100) bullets[bulletCount++] = new bullet(bx + 35.f, by, 50.f, 25.f, true, false, 1);
               }
               else if (wt == 2 && bulletCount < 100)
                   bullets[bulletCount++] = new bullet(bx, by, 70.f, 35.f, true, true, 2);

               fireTimer = 0.25f;
               gAudio.playPlayerFire();
           }

           for (int i = 0; i < enemyCount; ++i) enemies[i]->update(dt);

           for (int i = 0; i < bulletCount; )
           {
               bullets[i]->update(dt);
               if (!bullets[i]->getAlive())
               {
                   delete bullets[i];
                   bullets[i] = bullets[--bulletCount];
               }
               else ++i;
           }

           for (int i = 0; i < ASTEROID_COUNT; ++i) ast[i]->update(dt);

           for (int i = 0; i < powerUpCount; )
           {
               powerUps[i]->update(dt);
               if (powerUps[i]->collision(player1))
               {
                   player1->applyPowerUp(powerUps[i]->getType());
                   powerUps[i]->onCollision();
               }
               if (!powerUps[i]->getAlive())
               {
                   delete powerUps[i];
                   powerUps[i] = powerUps[--powerUpCount];
               }
               else ++i;
           }

           // Collisions
           for (int i = 0; i < bulletCount; ++i)
               for (int j = 0; j < enemyCount; ++j)
                   if (bullets[i]->getIsPlayerBullet() && bullets[i]->collision(enemies[j]))
                   {
                       enemies[j]->onCollision();
                       bullets[i]->onCollision();
                   }

           for (int i = 0; i < bulletCount; ++i)
               if (!bullets[i]->getIsPlayerBullet() && bullets[i]->collision(player1))
               {
                   player1->onCollision();
                   bullets[i]->onCollision();
               }

           for (int i = 0; i < enemyCount; ++i)
               if (enemies[i]->collision(player1))
               {
                   player1->onCollision();
                   enemies[i]->onCollision();
               }

           for (int i = 0; i < bulletCount; ++i)
               for (int j = 0; j < ASTEROID_COUNT; ++j)
                   if (bullets[i]->collision(ast[j]))
                       bullets[i]->onCollision();

           for (int j = 0; j < ASTEROID_COUNT; ++j)
               if (ast[j]->collision(player1))
               {
                   player1->onCollision();
                   ast[j]->setY(static_cast<float>(-50 - rand() % 400));
                   ast[j]->setX(static_cast<float>(rand() % 740 + 30));
               }

           // Dead enemies cleanup
           for (int i = 0; i < enemyCount; )
           {
               if (!enemies[i]->getAlive())
               {
                   if (enemies[i]->getWasKilled())
                   {
                       player1->addScore(enemies[i]->getScore());
                       if (gsm.getMode() == modeArcade && !bossActive)
                           arcadeKillCount++;
                   }
                   float dx = enemies[i]->getX(), dy = enemies[i]->getY();

                   if ((rand() % 100) < 15 && powerUpCount < 50)
                   {
                       int ch = rand() % 3;
                       int dt2 = (ch == 0) ? powerUpSpread :
                           (ch == 1) ? powerUpPierce : powerUpShield;
                       powerUps[powerUpCount++] = new powerUp(dx, dy, dt2);
                   }
                   if ((rand() % 100) < 5 && powerUpCount < 50)
                       powerUps[powerUpCount++] = new powerUp(dx, dy, powerUpEMP);

                   delete enemies[i];
                   enemies[i] = enemies[--enemyCount];
               }
               else ++i;
           }

           // Arcade mode
           if (gsm.getMode() == modeArcade)
           {
               if (!bossActive)
               {
                   if (arcadeKillCount >= BOSS_KILL_THRESHOLD)
                   {
                       for (int i = 0; i < enemyCount; ++i) delete enemies[i];
                       enemyCount = 0;
                       bossActive = true;
                       gAudio.playBossMusic();

                       if (currentLevel == 1)
                           currentBoss = new Cruiser(360.f, 200.f, bullets, &bulletCount);
                       else if (currentLevel == 2)
                           currentBoss = new TwinCannons(360.f, 200.f, bullets, &bulletCount, player1);
                       else
                           currentBoss = new Mothership(360.f, 200.f, bullets, &bulletCount,
                               enemies, &enemyCount, player1);
                   }
                   else
                   {
                       arcadeSpawnTimer -= dt;
                       if (arcadeSpawnTimer <= 0.f)
                       {
                           spawnOneArcadeEnemy(currentLevel, enemies, enemyCount,
                               bullets, bulletCount, player1);
                           arcadeSpawnTimer = ARCADE_SPAWN_INTERVAL;
                       }
                   }
               }

               if (bossActive && currentBoss)
               {
                   currentBoss->update(dt);

                   TwinCannons* tc = dynamic_cast<TwinCannons*>(currentBoss);
                   if (tc) tc->checkBulletVsTurrets(bullets, bulletCount);

                   for (int i = 0; i < bulletCount; ++i)
                       if (bullets[i]->getAlive() && bullets[i]->getIsPlayerBullet() &&
                           bullets[i]->collision(currentBoss))
                       {
                           currentBoss->onCollision();
                           spawnExplosion(bullets[i]->getX() + bullets[i]->getWidth() / 2.f,
                               bullets[i]->getY(), 10, sf::Color(255, 180, 0));
                           bullets[i]->onCollision();
                       }

                   if (!currentBoss->getAlive())
                   {
                       int bScore = (currentLevel == 1) ? 500 :
                           (currentLevel == 2) ? 800 : 2000;
                       player1->addScore(bScore);

                       delete currentBoss; currentBoss = nullptr;
                       bossActive = false;
                       levelCompleteTimer = 3.f;
                       if (currentLevel == 1) levelCompleteMsg = "LEVEL 1 COMPLETE!";
                       else if (currentLevel == 2) levelCompleteMsg = "LEVEL 2 COMPLETE!";
                       else                         levelCompleteMsg = "LEVEL 3 COMPLETE!";

                       if (currentLevel == 3)
                           gsm.triggerWin();
                       else
                       {
                           currentLevel++;
                           arcadeKillCount = 0;
                           arcadeSpawnTimer = 0.f;
                           for (int i = 0; i < 5; ++i)
                               spawnOneArcadeEnemy(currentLevel, enemies, enemyCount,
                                   bullets, bulletCount, player1);
                           gAudio.playNormalMusic();
                       }
                   }
               }
           }
           // Survival mode
           else if (gsm.getMode() == modeSurvival)
           {
               if (enemyCount == 0 && sm.isWaveInProgress())
               {
                   sm.waveCleared();
                   sm.startWave();
                   spawnSurvivalWave(enemies, enemyCount, bullets, bulletCount, sm, player1);
               }
           }
       }

       // ── DRAW ─────────────────────────────────────────────────────────────
       sf::Vector2f shakeOff = getShakeOffset();
       gameView.setCenter(sf::Vector2f(WIN_W / 2.f + shakeOff.x,
           WIN_H / 2.f + shakeOff.y));
       window.setView(gameView);

       window.clear(sf::Color(5, 5, 20));
       gBg.draw(window);

       if (gsm.isIntro())
       {
           player1->draw(window);
           drawTextC(window, "SPACE INVADERS", WIN_H / 2.f - 80.f, 50, sf::Color(0, 220, 255));
           drawTextC(window, "Preparing...", WIN_H / 2.f, 50, sf::Color(180, 180, 180));
       }
       else if (gsm.isInstructions())
       {
           sf::RectangleShape ov(sf::Vector2f(static_cast<float>(WIN_W),
               static_cast<float>(WIN_H)));
           ov.setFillColor(sf::Color(0, 0, 0, 180));
           window.draw(ov);

           sf::Color title(0, 220, 255);
           sf::Color body(220, 220, 220);
           sf::Color key(255, 220, 0);

           drawTextC(window, "CONTROLS", 80, 40, title);
           drawTextC(window, "Move Ship", 160, 30, body);
           drawTextC(window, "Arrow Keys", 185, 30, key);
           drawTextC(window, "Fire", 230, 30, body);
           drawTextC(window, "SPACE", 255, 30, key);
           drawTextC(window, "Evasive Dash", 300, 30, body);
           drawTextC(window, "E + Arrow Key  (3s cooldown, 0.5s invincibility)", 325, 30, key);
           drawTextC(window, "EMP / Screen Nuke", 380, 30, body);
           drawTextC(window, "N", 405, 30, key);
           drawTextC(window, "Pause", 450, 30, body);
           drawTextC(window, "ESC", 475, 30, key);
           drawTextC(window, "Quit to Menu (while paused)", 520, 30, body);
           drawTextC(window, "Q", 545, 30, key);
           drawTextC(window, "Press any key to return", 630, 30, sf::Color(150, 150, 150));
       }
       else if (gsm.isMenu())
       {
           sf::RectangleShape ov(sf::Vector2f(static_cast<float>(WIN_W),
               static_cast<float>(WIN_H)));
           ov.setFillColor(sf::Color(0, 0, 0, 140));
           window.draw(ov);

           drawTextC(window, "SPACE INVADERS", 140, 70, sf::Color(0, 255, 255));
           drawTextC(window, "1.  Arcade Mode", 270, 50, sf::Color::White);
           drawTextC(window, "2.  Survival Mode", 320, 50, sf::Color::White);
           drawTextC(window, "3.  Instructions", 370, 50, sf::Color::White);
           drawTextC(window, "ESC  Quit", 440, 30, sf::Color(140, 255, 255));
       }
       else if (gsm.isPlaying() || gsm.isPaused())
       {
           for (int i = 0; i < ASTEROID_COUNT; ++i) ast[i]->draw(window);
           for (int i = 0; i < enemyCount; ++i)
               if (enemies[i]->getAlive()) enemies[i]->draw(window);

           if (bossActive && currentBoss && currentBoss->getAlive())
               currentBoss->draw(window);

           for (int i = 0; i < powerUpCount; ++i)
               if (powerUps[i]->getAlive()) powerUps[i]->draw(window);

           for (int i = 0; i < bulletCount; ++i)
               if (bullets[i]->getAlive()) bullets[i]->draw(window);

           player1->draw(window);
           drawParticles(window);

           if (levelCompleteTimer > 0.f)
           {
               float    alpha = levelCompleteTimer / 3.f;

               std::uint8_t a = static_cast<std::uint8_t>(alpha * 255.f);

               sf::RectangleShape bg(sf::Vector2f(700.f, 90.f));
               bg.setPosition(sf::Vector2f((WIN_W - 700.f) / 2.f, WIN_H / 2.f - 45.f));
               bg.setFillColor(sf::Color(
                   0,
                   0,
                   0,
                   static_cast<std::uint8_t>(a * 0.8f)
               ));
               bg.setOutlineColor(sf::Color(0, 255, 120, a));
               bg.setOutlineThickness(3.f);
               window.draw(bg);

               drawTextC(window, levelCompleteMsg,
                   WIN_H / 2.f - 30.f, 42,
                   sf::Color(0, 255, 120, a));
           }

           if (empFlashTimer > 0.f)
           {
               sf::RectangleShape flash(sf::Vector2f(static_cast<float>(WIN_W),
                   static_cast<float>(WIN_H)));
               float alpha = (empFlashTimer / 0.2f) * 160.f;
               flash.setFillColor(sf::Color(
                   200, 220, 255,
                   static_cast<std::uint8_t>(alpha)
               ));
               window.draw(flash);
           }

           // HUD uses default (non-shaking) view
           window.setView(window.getDefaultView());

           drawHUD(window, player1, gsm, sm, currentLevel,
               arcadeKillCount, BOSS_KILL_THRESHOLD);

           if (bossActive && currentBoss && currentBoss->getAlive())
           {
               string bname;
               if (currentLevel == 1) bname = "THE CRUISER";
               else if (currentLevel == 2) bname = "THE TWIN CANNONS";
               else                         bname = "THE MOTHERSHIP";
               drawBossHP(window, currentBoss, bname);

               TwinCannons* tc = dynamic_cast<TwinCannons*>(currentBoss);
               if (tc && !tc->areTurretsDead())
                   drawTextC(window, "Destroy both turrets to expose the core!",
                       198, 22, sf::Color(255, 180, 0));
           }

           Mothership* ms = dynamic_cast<Mothership*>(currentBoss);
           if (ms && ms->isLaserWarning())
           {
               static float pulseT = 0.f; pulseT += 0.016f;
              std::uint8_t a = static_cast<std::uint8_t>(
   160 + 90 * std::sinf(pulseT * 15.f)
);
               sf::RectangleShape wb(sf::Vector2f(static_cast<float>(WIN_W), 26.f));
               wb.setPosition(sf::Vector2f(0.f, WIN_H / 2.f));
               wb.setFillColor(sf::Color(255, 0, 0, a));
               window.draw(wb);
               drawTextC(window, "!!! LASER WARNING - MOVE TO EDGE !!!",
                   WIN_H / 2.f + 3.f, 70, sf::Color::White);
           }

           if (gsm.isPaused())
           {
               sf::RectangleShape ov(sf::Vector2f(static_cast<float>(WIN_W),
                   static_cast<float>(WIN_H)));
               ov.setFillColor(sf::Color(0, 0, 0, 170));
               window.draw(ov);
               drawTextC(window, "PAUSED", 260, 48, sf::Color(0, 255, 255));
               drawTextC(window, "ESC  -  Resume", 340, 50, sf::Color::White);
               drawTextC(window, "Q    -  Quit to Menu", 390, 50, sf::Color::White);
           }
       }
       else if (gsm.isGameOver())
       {
           drawParticles(window);
           sf::RectangleShape ov(sf::Vector2f(static_cast<float>(WIN_W),
               static_cast<float>(WIN_H)));
           ov.setFillColor(sf::Color(0, 0, 0, 180));
           window.draw(ov);

           drawTextC(window, "GAME OVER", 220, 70, sf::Color(255, 60, 60));
           drawTextC(window, "Score: " + to_string(player1->getScore()),
               310, 50, sf::Color::White);
           if (gsm.getMode() == modeSurvival)
               drawTextC(window, "Wave reached: " + to_string(sm.getWaveNumber()),
                   355, 70, sf::Color(200, 200, 200));
           drawTextC(window, "Press R to return to Menu",
               440, 50, sf::Color(140, 140, 140));
       }
       else if (gsm.isWin())
       {
           drawParticles(window);
           sf::RectangleShape ov(sf::Vector2f(static_cast<float>(WIN_W),
               static_cast<float>(WIN_H)));
           ov.setFillColor(sf::Color(0, 0, 0, 160));
           window.draw(ov);

           drawTextC(window, "YOU WIN!", 220, 100, sf::Color(0, 255, 120));
           drawTextC(window, "Score: " + to_string(player1->getScore()),
               310, 70, sf::Color::White);
           drawTextC(window, "Press R to return to Menu",
               400, 70, sf::Color(140, 140, 140));
       }

       window.display();
   }

   // ── CLEANUP ───────────────────────────────────────────────────────────────
   for (int i = 0; i < bulletCount; ++i) delete bullets[i];
   for (int i = 0; i < enemyCount; ++i) delete enemies[i];
   for (int i = 0; i < powerUpCount; ++i) delete powerUps[i];
   for (int i = 0; i < ASTEROID_COUNT;++i) delete ast[i];
   if (currentBoss) delete currentBoss;
   delete player1;
   delete[] bullets;
   delete[] enemies;
   delete[] powerUps;

   return 0;
}

