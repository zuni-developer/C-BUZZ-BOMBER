#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>   // For file handling
#include <cstdlib>   // For random number generation
#include <cmath>     // For sqrt
#include <cstring>   // For strcpy, etc. if needed
using namespace std;
using namespace sf;
//ASAD_ALI_PF_PROJECT_CS-G

// Initializing Dimensions.
const int resolutionX = 960;
const int resolutionY = 640;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX;    // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid



void drawBullet(RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite);
void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock);
void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite);
bool checkCollision(Sprite& sprite1, Sprite& sprite2);
void addHighScore(const char* filename, char names[][50], int scores[], int& count, const char* name, int score);
void moveBees(Sprite bees[], bool directions[], int numBees, float speed, Sprite honeycombs[], int honeycombCount, Sprite hives[], int& hiveCount, Texture& hiveTexture, Clock beeStopClocks[], bool beeStopped[], float beeStopDuration[]);
void moveFastBees(Sprite fastBees[], bool fastBeeDirections[], bool fastBeeActive[], int numFastBees, float fastBeeSpeed) ;
void sortScores(char names[][50], int scores[], int count);
void writeHighScores(const char* filename, char names[][50], int scores[], int count);
int readHighScores(const char* filename, char names[][50], int scores[], int maxScores);
int getTier(float y) ;


 

// Maximum number of scores stored
const int MAX_SCORES = 100;
char playerNames[MAX_SCORES][50]; // Array to store names
int playerScores[MAX_SCORES];     // Array to store scores


int main()
{
    srand(static_cast<unsigned>(time(0)));

    // Game States
const int STATE_MENU = 0;
const int STATE_LEVEL_SELECTION = 1;
const int STATE_GAME = 2;
const int STATE_LEVEL_PASSED = 3;
const int STATE_LEVEL_FAILED = 4;
const int STATE_GAME_COMPLETED = 5;
const int STATE_EXIT = 6;
const int STATE_PAUSE = 7;
const int STATE_HIGH_SCORES = 8; // For showing high scores screen


    RectangleShape groundRectangle(Vector2f(960, 64));
    groundRectangle.setPosition(0, (gameColumns - 2) * boxPixelsY);
    groundRectangle.setFillColor(Color::Green);

    RenderWindow window(VideoMode(resolutionX, resolutionY), "Buzz Bombers", Style::Close | Style::Titlebar);
    window.setPosition(Vector2i(500, 200));

    int gameState = STATE_MENU;

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Textures/lava.png")) {
        return -1;
    }
    Sprite backgroundSprite(backgroundTexture);

    Texture gameBackgroundTexture;
    if (!gameBackgroundTexture.loadFromFile("Textures/download1.jpg")) {
        return -1;
    }
    Sprite gameBackgroundSprite(gameBackgroundTexture);

    Texture gameBackground2Texture;
    if (!gameBackground2Texture.loadFromFile("Textures/download2.jpg")) {
        return -1;
    }
    Sprite gameBackground2Sprite(gameBackground2Texture);

    Texture gameBackground3Texture;
    if (!gameBackground3Texture.loadFromFile("Textures/download3.jpg")) {
        return -1;
    }
    Sprite gameBackground3Sprite(gameBackground3Texture);



    Texture flowerTexture;
    if (!flowerTexture.loadFromFile("Textures/obstacles.png")) {
        return -1;
    }

    Texture playerTexture;
    if (!playerTexture.loadFromFile("Textures/spray.png")) {
        return -1;
    }
    Sprite playerSprite(playerTexture);

    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("Textures/bullet.png")) {
        return -1;
    }
    Sprite bulletSprite(bulletTexture);
    bulletSprite.setScale(3, 3);
    bulletSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    Texture beeTexture;
    if (!beeTexture.loadFromFile("Textures/download4.png")) {
        return -1;
    }

    Texture honeycombTexture;
    if (!honeycombTexture.loadFromFile("Textures/honeycomb.png")) {
        return -1;
    }

    Texture redHoneycombTexture;
    if (!redHoneycombTexture.loadFromFile("Textures/honeycomb_red.png")) {
        return -1;
    }

    Texture yellowHoneycombTexture;
    if (!yellowHoneycombTexture.loadFromFile("Textures/honeycomb.png")) {
        return -1;
    }

    Texture hummingbirdTexture;
    if (!hummingbirdTexture.loadFromFile("Textures/bird.png")) {
        return -1;
    }
    Sprite hummingbird(hummingbirdTexture);
    hummingbird.setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY));
    Clock hummingbirdClock;

    bool hummingbirdOffScreen = false;
    Clock hummingbirdOffScreenClock;
    int hummingbirdHitCount = 0;

    bool hummingbirdColliding[50] = { false }; 
    Clock hummingbirdHoneycombClocks[50];
    Clock hummingbirdRandomMoveClock;
    Vector2f hummingbirdRandomDir(0, 0);
    bool hummingbirdEating = false; 
    int hummingbirdEatingIndex = -1; 

    Texture hiveTexture;
    if (!hiveTexture.loadFromFile("Textures/hive.png")) {
        return -1;
    }

    Texture fastBeeTexture;
    if (!fastBeeTexture.loadFromFile("Textures/Fast_bee.png")) {
        return -1;
    }

    Sprite hives[50];
    int hiveCount = 0;

    Music titleMusic, bgMusic;
    if (!titleMusic.openFromFile("Music/Title.wav")) {
        return -1;
    }
    titleMusic.setLoop(true);
    titleMusic.setVolume(50);
    titleMusic.play();

    bgMusic.setLoop(true);
    bgMusic.setVolume(50);

    Font font;
    if (!font.loadFromFile("font/Arial.ttf")) {
        return -1;
    }

    Text menu[4];
    string options[] = { "Start Game", "Choose Level", "High Scores", "Exit" };
    int selectedOption = 0;
    for (int i = 0; i < 4; i++) {
        menu[i].setFont(font);
        menu[i].setString(options[i]);
        menu[i].setCharacterSize(40);
        menu[i].setPosition(360, 200 + i * 60);
        menu[i].setFillColor(Color::White);
    }
    menu[0].setFillColor(Color::Yellow);

    Text levelMenu[4];
    string levelOptions[] = { "Level 1", "Level 2", "Level 3", "Back to Main Menu" };
    int selectedLevelOption = 0;
    for (int i = 0; i < 4; i++) {
        levelMenu[i].setFont(font);
        levelMenu[i].setString(levelOptions[i]);
        levelMenu[i].setCharacterSize(40);
        levelMenu[i].setPosition(360, 200 + i * 60);
        levelMenu[i].setFillColor(Color::White);
    }
    levelMenu[0].setFillColor(Color::Yellow);

    Text highScoresText;
    highScoresText.setFont(font);
    highScoresText.setCharacterSize(40);
    highScoresText.setFillColor(Color::White);
    highScoresText.setPosition(360, 150);

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    

    int score = 0;
    int totalBeesEliminated = 0;
    int hiveBonus = 0;
    int leftoverHoneycombBonus = 0;
    Clock beeStopClocks[4];
    bool beeStopped[4] = {false, false, false, false};
    float beeStopDuration[4];
     int nextBonusScore = 20000;

    bool hiveBonusApplied = false;

    float player_x = (gameRows / 2) * boxPixelsX;
    float player_y = (gameColumns - 4) * boxPixelsY;
    float bullet_x = player_x, bullet_y = player_y;
    bool bullet_exists = false;
    Clock bulletClock;
    
    int lives = 3;
    int spraysLeft = 56;
     
    Sprite honeycombs[50];
    Clock honeycombTimers[50];
    int honeycombCount = 3;
    for (int i = 0; i < 3; i++) {
        honeycombs[i].setTexture(honeycombTexture);
        honeycombs[i].setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY / 3));
        honeycombTimers[i].restart();
    }

    Sprite bees[4];
    bool beeDirections[4] = { true, true, true, true };
    int totalBeesSpawned = 4;
    for (int i = 0; i < 4; i++) {
        bees[i].setTexture(beeTexture);
        bees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
    }

    Sprite fastBees[10];
    bool fastBeeDirections[10];
    bool fastBeeActive[10];
    float fastBeeSpeed = 7.0f;

    Sprite flowers[50];
    int flowerCount = 0;

    bool firstBeeCollision = true;

    int selectedLevel = 1;
    const int maxBeesPerLevel[] = {20, 15, 20};
    int maxBees = maxBeesPerLevel[selectedLevel - 1];

    float beeSpeed = 3.0f;

    Clock spawnClock;
    int honeycombsSpawned = 0;
    int fastBeesSpawned = 0;

    int totalHoneycombsLevel2 = 9;
    int initialHoneycombsLevel2 = 3;
    int totalHoneycombsLevel3 = 15;
    int totalFastBeesLevel3 = 10;
    int initialFastBeesLevel3 = 5;
    int totalFastBeesLevel2 = 5;
   
 int scoreCount = 0; // To keep track of the number of high scores
    scoreCount = readHighScores("High_score", playerNames, playerScores, MAX_SCORES);

    bool pausedFromGame = false;
    bool showFinalScoresAfterEnd = false;
    bool finalNameAsked = false;

    Text pauseMenu[4];
    string pauseOptions[] = { "Continue", "Start Game", "Choose Level", "Exit" };
    int selectedPauseOption = 0;
    for (int i = 0; i < 4; i++) {
        pauseMenu[i].setFont(font);
        pauseMenu[i].setString(pauseOptions[i]);
        pauseMenu[i].setCharacterSize(40);
        pauseMenu[i].setPosition(360, 200 + i * 60);
        pauseMenu[i].setFillColor(Color::White);
    }
    pauseMenu[0].setFillColor(Color::Yellow);

    Text levelPassedMenu[2];
    string levelPassedOptions[] = { "Next Level", "Main Menu" };
    int selectedLevelPassedOption = 0;
    for (int i = 0; i < 2; i++) {
        levelPassedMenu[i].setFont(font);
        levelPassedMenu[i].setString(levelPassedOptions[i]);
        levelPassedMenu[i].setCharacterSize(40);
        levelPassedMenu[i].setPosition(360, (resolutionY / 2 + 120) + i * 60);
        levelPassedMenu[i].setFillColor(Color::White);
    }
    levelPassedMenu[0].setFillColor(Color::Yellow);

    Text levelFailedMenu[2];
    string levelFailedOptions[] = { "Restart Level", "Main Menu" };
    int selectedLevelFailedOption = 0;
    for (int i = 0; i < 2; i++) {
        levelFailedMenu[i].setFont(font);
        levelFailedMenu[i].setString(levelFailedOptions[i]);
        levelFailedMenu[i].setCharacterSize(40);
        levelFailedMenu[i].setPosition(360, (resolutionY / 2) + 40 + i * 60);
        levelFailedMenu[i].setFillColor(Color::White);
    }
    levelFailedMenu[0].setFillColor(Color::Yellow);

    SoundBuffer hitBuffer;
    if (!hitBuffer.loadFromFile("Sounds/hit.wav")) {
        cerr << "Error loading Sounds/hit.wav" << endl;
        return -1;
    }
    Sound hitSound;
    hitSound.setBuffer(hitBuffer);
    hitSound.setVolume(50);

    SoundBuffer obstacleBuffer;
    if (!obstacleBuffer.loadFromFile("Sounds/Obstacle.wav")) {
        cerr << "Error loading Sounds/Obstacle.wav" << endl;
        return -1;
    }
    Sound obstacleSound;
    obstacleSound.setBuffer(obstacleBuffer);
    obstacleSound.setVolume(50);

    SoundBuffer fireBuffer;
    if (!fireBuffer.loadFromFile("Sounds/fire.wav")) {
        cerr << "Error loading Sounds/fire.wav" << endl;
        return -1;
    }
    Sound fireSound;
    fireSound.setBuffer(fireBuffer);
    fireSound.setVolume(50);

    bool obstacleSoundPlayed = false;

    auto resetGameState = [&](bool resetScoreAndLives) {
        player_x = (gameRows / 2) * boxPixelsX;
        player_y = (gameColumns - 4) * boxPixelsY;

        if (resetScoreAndLives) {
            score = 0;
            lives = 3;  
            spraysLeft = 56;
        }

        maxBees = maxBeesPerLevel[selectedLevel - 1];
        flowerCount = 0;
        totalBeesSpawned = 4;
        totalBeesEliminated = 0;
        hiveCount = 0;
        hummingbirdOffScreen = false;
        hummingbirdHitCount = 0;
        beeSpeed = 3.0f;
        hiveBonusApplied = false;
        leftoverHoneycombBonus = 0;
        firstBeeCollision = true;
        hummingbirdEating = false;
        hummingbirdEatingIndex = -1;

        hummingbird.setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY));
        hummingbirdOffScreen = false;
        hummingbirdRandomDir = Vector2f(0,0);
        hummingbirdRandomMoveClock.restart();
        for (int i = 0; i < 50; i++) {
            hummingbirdColliding[i] = false;
        }

        for (int i = 0; i < 4; i++) {
            bees[i].setTexture(beeTexture);
            bees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
            beeDirections[i] = rand() % 2 == 0;
        }

        for (int i = 0; i < 50; i++) {
            hives[i].setPosition(-100, -100);
        }

        if (bgMusic.getStatus() == Music::Playing) {
            bgMusic.stop();
        }

        string musicFile;
        switch (selectedLevel) {
            case 1:
                musicFile = "Music/Forest.wav";
                break;
            case 2:
                musicFile = "Music/Cave.wav";
                break;
            case 3:
                musicFile = "Music/Lava_Reef.wav";
                break;
            default:
                musicFile = "Music/Forest.wav";
                break;
        }

        if (!bgMusic.openFromFile(musicFile)) {
            cerr << "Error loading " << musicFile << endl;
            gameState = STATE_EXIT;
        } else {
            bgMusic.setLoop(true);
            bgMusic.setVolume(50);
            bgMusic.play();
        }

        if (selectedLevel == 1) {
            honeycombCount = 3;
            for (int i = 0; i < honeycombCount; i++) {
                honeycombs[i].setTexture(honeycombTexture);
                honeycombs[i].setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY / 3));
            }
            for (int i = honeycombCount; i < 50; i++) {
                honeycombs[i].setPosition(-100, -100);
            }

            for (int i = 0; i < 10; i++) {
                fastBees[i].setPosition(-100, -100);
                fastBeeActive[i] = false;
            }

        } else if (selectedLevel == 2) {
            honeycombCount = initialHoneycombsLevel2;
            for (int i = 0; i < honeycombCount; i++) {
                honeycombs[i].setTexture(honeycombTexture);
                honeycombs[i].setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY / 3));
            }
            for (int i = honeycombCount; i < 50; i++) {
                honeycombs[i].setPosition(-100, -100);
            }
            honeycombsSpawned = 0;
            spawnClock.restart();

            for (int i = 0; i < totalFastBeesLevel2; i++) {
                fastBees[i].setTexture(fastBeeTexture);
                fastBees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
                fastBeeDirections[i] = (rand() % 2 == 0);
                fastBeeActive[i] = true;
            }
            for (int i = totalFastBeesLevel2; i < 10; i++) {
                fastBees[i].setPosition(-100, -100);
                fastBeeActive[i] = false;
            }

        } else if (selectedLevel == 3) {
            honeycombCount = totalHoneycombsLevel3;
            for (int i = 0; i < honeycombCount; i++) {
                honeycombs[i].setTexture(yellowHoneycombTexture);
                honeycombs[i].setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY / 3));
            }
            for (int i = honeycombCount; i < 50; i++) {
                honeycombs[i].setPosition(-100, -100);
            }

            for (int i = 0; i < initialFastBeesLevel3; i++) {
                fastBees[i].setTexture(fastBeeTexture);
                fastBees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
                fastBeeDirections[i] = (rand() % 2 == 0);
                fastBeeActive[i] = true;
            }
            for (int i = initialFastBeesLevel3; i < 10; i++) {
                fastBees[i].setPosition(-100, -100);
                fastBeeActive[i] = false;
            }
            fastBeesSpawned = 0;
            spawnClock.restart();
        }
    };

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();

            if (gameState == STATE_MENU) {
                if (e.type == Event::KeyPressed) {
                    if (e.key.code == Keyboard::Up) {
                        menu[selectedOption].setFillColor(Color::White);
                        selectedOption = (selectedOption - 1 + 4) % 4;
                        menu[selectedOption].setFillColor(Color::Yellow);
                    }
                    else if (e.key.code == Keyboard::Down) {
                        menu[selectedOption].setFillColor(Color::White);
                        selectedOption = (selectedOption + 1) % 4;
                        menu[selectedOption].setFillColor(Color::Yellow);
                    }
                    else if (e.key.code == Keyboard::Enter) {
                        if (selectedOption == 0) {
                            // Start Game
                            gameState = STATE_GAME;
                            titleMusic.stop();
                            selectedLevel = 1;
                            resetGameState(true);
                        }
                        else if (selectedOption == 1) {
                            // Choose level
                            gameState = STATE_LEVEL_SELECTION;
                            selectedLevelOption = 0;
                            for (int i = 0; i < 4; i++) {
                                levelMenu[i].setFillColor(Color::White);
                            }
                            levelMenu[selectedLevelOption].setFillColor(Color::Yellow);
                        }
                        else if (selectedOption == 2) {
                            // High Scores
                            gameState = STATE_HIGH_SCORES;
                            showFinalScoresAfterEnd = false;
                        }
                        else if (selectedOption == 3) {
                            window.close();
                        }
                    }
                }
            }

            else if (gameState == STATE_PAUSE) {
                if (e.type == Event::KeyPressed) {
                    if (e.key.code == Keyboard::Up) {
                        pauseMenu[selectedPauseOption].setFillColor(Color::White);
                        selectedPauseOption = (selectedPauseOption - 1 + 4) % 4;
                        pauseMenu[selectedPauseOption].setFillColor(Color::Yellow);
                    } else if (e.key.code == Keyboard::Down) {
                        pauseMenu[selectedPauseOption].setFillColor(Color::White);
                        selectedPauseOption = (selectedPauseOption + 1) % 4;
                        pauseMenu[selectedPauseOption].setFillColor(Color::Yellow);
                    } else if (e.key.code == Keyboard::Enter) {
                        if (selectedPauseOption == 0) {
                            gameState = STATE_GAME;
                            pausedFromGame = false;
                            bgMusic.play();
                        } else if (selectedPauseOption == 1) {
                            gameState = STATE_GAME;
                            resetGameState(true);
                            pausedFromGame = false;
                            titleMusic.stop();
                        } else if (selectedPauseOption == 2) {
                            gameState = STATE_LEVEL_SELECTION;
                            selectedLevelOption = 0;
                            for (int i = 0; i < 4; i++) {
                                levelMenu[i].setFillColor(Color::White);
                            }
                            levelMenu[selectedLevelOption].setFillColor(Color::Yellow);
                            pausedFromGame = false;
                            bgMusic.stop();
                            titleMusic.play();
                        } else if (selectedPauseOption == 3) {
                            window.close();
                        }
                    } else if (e.key.code == Keyboard::Escape) {
                        gameState = STATE_GAME;
                        pausedFromGame = false;
                        bgMusic.play();
                    }
                }
            }

            else if (gameState == STATE_LEVEL_SELECTION && e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Up) {
                    levelMenu[selectedLevelOption].setFillColor(Color::White);
                    selectedLevelOption = (selectedLevelOption - 1 + 4) % 4;
                    levelMenu[selectedLevelOption].setFillColor(Color::Yellow);
                }
                else if (e.key.code == Keyboard::Down) {
                    levelMenu[selectedLevelOption].setFillColor(Color::White);
                    selectedLevelOption = (selectedLevelOption + 1) % 4;
                    levelMenu[selectedLevelOption].setFillColor(Color::Yellow);
                }
                else if (e.key.code == Keyboard::Enter) {
                    if (selectedLevelOption >= 0 && selectedLevelOption <= 2) {
                        selectedLevel = selectedLevelOption + 1;
                        maxBees = maxBeesPerLevel[selectedLevel - 1];
                        gameState = STATE_GAME;
                        titleMusic.stop();
                        resetGameState(true);
                    }
                    else if (selectedLevelOption == 3) {
                        gameState = STATE_MENU;
                        selectedOption = 0;
                        for (int i = 0; i < 4; i++) {
                            menu[i].setFillColor(Color::White);
                        }
                        menu[selectedOption].setFillColor(Color::Yellow);
                    }
                }
            }

            else if (gameState == STATE_LEVEL_PASSED && e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Up) {
                    levelPassedMenu[selectedLevelPassedOption].setFillColor(Color::White);
                    selectedLevelPassedOption = (selectedLevelPassedOption - 1 + 2) % 2;
                    levelPassedMenu[selectedLevelPassedOption].setFillColor(Color::Yellow);
                } else if (e.key.code == Keyboard::Down) {
                    levelPassedMenu[selectedLevelPassedOption].setFillColor(Color::White);
                    selectedLevelPassedOption = (selectedLevelPassedOption + 1) % 2;
                    levelPassedMenu[selectedLevelPassedOption].setFillColor(Color::Yellow);
                } else if (e.key.code == Keyboard::Enter) {
                    if (selectedLevelPassedOption == 0) {
                        // Next Level
                        selectedLevel++;
                        if (selectedLevel > 3) {
                            if (!finalNameAsked) {
                                cout << "Enter your name: ";
                                char playerName[50];
                                cin >> playerName;
                               addHighScore("High_score", playerNames, playerScores, scoreCount, playerName, score);

                                finalNameAsked = true;
                            }
                            gameState = STATE_HIGH_SCORES;
                            showFinalScoresAfterEnd = true;
                        }
                        else {
                            gameState = STATE_GAME;
                            maxBees = maxBeesPerLevel[selectedLevel - 1];
                            bgMusic.play();
                            resetGameState(false);
                        }
                    } else if (selectedLevelPassedOption == 1) {
                        // Main Menu
                        gameState = STATE_MENU;
                        titleMusic.play();
                        for (int i = 0; i < 4; i++) {
                            menu[i].setFillColor(Color::White);
                        }
                        selectedOption = 0;
                        menu[selectedOption].setFillColor(Color::Yellow);
                    }
                }
            }

            else if (gameState == STATE_GAME_COMPLETED && e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Enter) {
                    window.close();
                }
            }

            else if (gameState == STATE_LEVEL_FAILED && e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Up) {
                    levelFailedMenu[selectedLevelFailedOption].setFillColor(Color::White);
                    selectedLevelFailedOption = (selectedLevelFailedOption - 1 + 2) % 2;
                    levelFailedMenu[selectedLevelFailedOption].setFillColor(Color::Yellow);
                } else if (e.key.code == Keyboard::Down) {
                    levelFailedMenu[selectedLevelFailedOption].setFillColor(Color::White);
                    selectedLevelFailedOption = (selectedLevelFailedOption + 1) % 2;
                    levelFailedMenu[selectedLevelFailedOption].setFillColor(Color::Yellow);
                } else if (e.key.code == Keyboard::Enter) {
                    if (selectedLevelFailedOption == 0) {
                        // Restart Level
                        gameState = STATE_GAME;
                        bgMusic.play();
                        resetGameState(true);
                    } else if (selectedLevelFailedOption == 1) {
                        // Main Menu
                        if (!finalNameAsked) {
                            cout << "Enter your name: ";
                            char playerName[50];
                            cin >> playerName;
                           addHighScore("High_score", playerNames, playerScores, scoreCount, playerName, score);

                            finalNameAsked = true;
                        }
                        gameState = STATE_HIGH_SCORES;
                        showFinalScoresAfterEnd = true;
                    }
                }
            }

            else if (gameState == STATE_GAME) {
                if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                    gameState = STATE_PAUSE;
                    pausedFromGame = true;
                    bgMusic.pause();
                }
            }

            else if (gameState == STATE_HIGH_SCORES && e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Enter) {
                    if (showFinalScoresAfterEnd) {
                        // Return to main menu
                        gameState = STATE_MENU;
                        titleMusic.play();
                        for (int i = 0; i < 4; i++) {
                            menu[i].setFillColor(Color::White);
                        }
                        selectedOption = 0;
                        menu[selectedOption].setFillColor(Color::Yellow);
                        showFinalScoresAfterEnd = false;
                    } else {
                        gameState = STATE_MENU;
                        for (int i = 0; i < 4; i++) {
                            menu[i].setFillColor(Color::White);
                        }
                        selectedOption = 0;
                        menu[selectedOption].setFillColor(Color::Yellow);
                    }
                }
            }

        }

        if (gameState == STATE_GAME) {
            if (selectedLevel == 2) {
                if (spawnClock.getElapsedTime().asSeconds() > 5.0f && honeycombCount < 9) {
                    spawnClock.restart();
                    honeycombs[honeycombCount].setTexture(honeycombTexture);
                    honeycombs[honeycombCount].setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY / 3));
                    honeycombCount++;
                }
            }

            if (selectedLevel == 3) {
                if (spawnClock.getElapsedTime().asSeconds() > 5.0f && (5 + fastBeesSpawned) < 10) {
                    spawnClock.restart();
                    int index = 5 + fastBeesSpawned;
                    fastBees[index].setTexture(fastBeeTexture);
                    fastBees[index].setPosition(rand() % (resolutionX - boxPixelsX), 50);
                    fastBeeDirections[index] = (rand() % 2 == 0);
                    fastBeeActive[index] = true;
                    fastBeesSpawned++;
                }
            }
        }

        window.clear();

if (gameState == STATE_MENU || gameState == STATE_HIGH_SCORES || gameState == STATE_PAUSE || gameState == STATE_LEVEL_SELECTION) {
    window.draw(backgroundSprite);  // Draw the main background 
}
       if (gameState == STATE_GAME) {
    if (selectedLevel == 1) {
        window.draw(gameBackgroundSprite);
    } else if (selectedLevel == 2) {
        window.draw(gameBackground2Sprite);
    } else if (selectedLevel == 3) {
         window.draw(gameBackground3Sprite);
    } else {
         window.draw(backgroundSprite);
    }
}


        if (gameState == STATE_MENU) {
            for (int i = 0; i < 4; i++) {
                window.draw(menu[i]);
                 
            }
        }
       else if (gameState == STATE_HIGH_SCORES) {
    
   int count = readHighScores("High_score", playerNames, playerScores, MAX_SCORES);

   sortScores(playerNames, playerScores, count);
 // display scores in ascending order 

    // Display header
    highScoresText.setPosition(300, 100);
    highScoresText.setString("High Scores (Ascending):");
    window.draw(highScoresText);

    // Display sorted high scores
    Text lineText;
    lineText.setFont(font);
    lineText.setCharacterSize(30);
    lineText.setFillColor(Color::White);
    int lineCount = 0;
    for (int i = 0; i < count && i < 10; i++) { // Show only top 10 scores
       lineText.setString(string(playerNames[i]) + " - " + to_string(playerScores[i]));

        lineText.setPosition(360, 200 + lineCount * 40);
        window.draw(lineText);
        lineCount++;
    }

    
    Text promptText;
    promptText.setFont(font);
    promptText.setCharacterSize(30);
    promptText.setFillColor(Color::White);
    promptText.setString("Press Enter to return");
    promptText.setPosition(360, 500);
    window.draw(promptText);
}

        else if (gameState == STATE_PAUSE) {
            for (int i = 0; i < 4; i++) {
                window.draw(pauseMenu[i]);
            }
        }
        else if (gameState == STATE_LEVEL_SELECTION) {
            for (int i = 0; i < 4; i++) {
                window.draw(levelMenu[i]);
            }
        }
        else if (gameState == STATE_GAME) {
            float playerSpeed = 5.0f;
            bool canMoveLeft = true;
            bool canMoveRight = true;
            bool isOverlappingFlower = false;

            playerSprite.setPosition(player_x, player_y);
            for (int i = 0; i < flowerCount; i++) {
                if (checkCollision(playerSprite, flowers[i])) {
                    isOverlappingFlower = true;
                    if (!obstacleSoundPlayed) {
                        obstacleSound.play();
                        obstacleSoundPlayed = true;
                    }
                    break;
                }
            }

            if (!isOverlappingFlower) {
                playerSprite.setPosition(player_x - playerSpeed, player_y);
                for (int i = 0; i < flowerCount; i++) {
                    if (checkCollision(playerSprite, flowers[i])) {
                        canMoveLeft = false;
                        if (!obstacleSoundPlayed) {
                            obstacleSound.play();
                            obstacleSoundPlayed = true;
                        }
                        break;
                    }
                }
                playerSprite.setPosition(player_x, player_y);

                playerSprite.setPosition(player_x + playerSpeed, player_y);
                for (int i = 0; i < flowerCount; i++) {
                    if (checkCollision(playerSprite, flowers[i])) {
                        canMoveRight = false;
                        if (!obstacleSoundPlayed) {
                            obstacleSound.play();
                            obstacleSoundPlayed = true;
                        }
                        break;
                    }
                }
                playerSprite.setPosition(player_x, player_y);
            }

            bool isPlayerStuck = (!canMoveLeft && !canMoveRight && !isOverlappingFlower);
            if (isPlayerStuck) {
                lives--;
                if (lives <= 0) {
                    gameState = STATE_LEVEL_FAILED;
                } else {
                    resetGameState(true);
                }
            }

            if (!isOverlappingFlower && canMoveLeft && canMoveRight) {
                obstacleSoundPlayed = false;
            }

            if (Keyboard::isKeyPressed(Keyboard::Left) && player_x > 0 && (canMoveLeft || isOverlappingFlower)) {
                player_x -= playerSpeed;
            }
            if (Keyboard::isKeyPressed(Keyboard::Right) && player_x < resolutionX - boxPixelsX && (canMoveRight || isOverlappingFlower)) {
                player_x += playerSpeed;
            }

            if (Keyboard::isKeyPressed(Keyboard::Space) && !bullet_exists) {
                bullet_exists = true;
                bullet_x = player_x + boxPixelsX / 2;
                bullet_y = player_y;
                spraysLeft--;

                fireSound.play();

                if (spraysLeft <= 0) {
                    lives--;
                    if (lives <= 0) {
                        gameState = STATE_LEVEL_FAILED;
                    } else {
                        spraysLeft = 56;
                    }
                }
            }

            if (bullet_exists) {
                moveBullet(bullet_y, bullet_exists, bulletClock);
                drawBullet(window, bullet_x, bullet_y, bulletSprite);
            }

            moveBees(bees, beeDirections, 4, beeSpeed, honeycombs, honeycombCount, hives, hiveCount, hiveTexture, beeStopClocks, beeStopped, beeStopDuration);

            if (selectedLevel == 2 || selectedLevel == 3) {
                int maxFastBeesActive = (selectedLevel == 2) ? 5 : 10;
                moveFastBees(fastBees, fastBeeDirections, fastBeeActive, maxFastBeesActive, fastBeeSpeed);
            }

            // Bullet-bee collisions (Regular bees)
            for (int i = 0; i < 4; i++) {
                if (bullet_exists && checkCollision(bulletSprite, bees[i])) {
                    bullet_exists = false;
                    score += 100;
                    totalBeesEliminated++;
                    hitSound.play();

                    if (score >= nextBonusScore) {
    lives++; // Grant a life
    if (nextBonusScore < 80000) {
        nextBonusScore *= 2; // Double the bonus threshold (20k -> 40k -> 80k)
    } else {
        nextBonusScore += 80000; // After 80k, add 80k for each subsequent bonus
    }
}


                    if (honeycombCount < 50) {
                        honeycombs[honeycombCount].setTexture(honeycombTexture);
                        honeycombs[honeycombCount].setPosition(bees[i].getPosition());
                        honeycombCount++;
                    }

                    if (totalBeesSpawned < maxBees ) {
                        bees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
                        totalBeesSpawned++;
                        beeDirections[i] = (rand() % 2 == 0);
                    } else {
                        bees[i].setPosition(-100, -100);
                    }
                }
            }

            // Bullet-fastBee collisions
            if (selectedLevel == 2 || selectedLevel == 3) {
                int maxFastBeesActive = (selectedLevel == 2) ? 5 : 10;
                for (int i = 0; i < maxFastBeesActive; i++) {
                    if (fastBeeActive[i] && bullet_exists && checkCollision(bulletSprite, fastBees[i])) {
                        bullet_exists = false;
                        score += 1000;
                        hitSound.play();
                       
                       if (score >= nextBonusScore) {
    lives++; // Grant a life
    if (nextBonusScore < 80000) {
        nextBonusScore *= 2; // Double the bonus threshold (20k -> 40k -> 80k)
    } else {
        nextBonusScore += 80000; // After 80k, add 80k for each subsequent bonus
    }
}

                        if (honeycombCount < 50) {
                            honeycombs[honeycombCount].setTexture(redHoneycombTexture);
                            honeycombs[honeycombCount].setPosition(fastBees[i].getPosition());
                            honeycombCount++;
                        }

                        fastBees[i].setPosition(-100, -100);
                        fastBeeActive[i] = false;
                    }
                }
            }

            // Fast bee hits spray can
            if (selectedLevel == 2 || selectedLevel == 3) {
                int maxFastBeesActive = (selectedLevel == 2) ? 5 : 10;
                for (int i = 0; i < maxFastBeesActive; i++) {
                    if (fastBeeActive[i] && checkCollision(fastBees[i], playerSprite)) {
                        // Only life decreases
                        lives--;
                        if (lives <= 0) {
                            gameState = STATE_LEVEL_FAILED;
                        }
                        // Remove the fast bee
                        fastBees[i].setPosition(-100, -100);
                        fastBeeActive[i] = false;
                    }
                }
            }

            // Bullet-hummingbird
            if (!hummingbirdOffScreen && bullet_exists && checkCollision(bulletSprite, hummingbird)) {
                bullet_exists = false;
                hummingbirdHitCount++;
                hitSound.play();

                if (hummingbirdHitCount >= 3) {
                    hummingbird.setPosition(-200, -200);
                    hummingbirdOffScreen = true;
                    hummingbirdOffScreenClock.restart();
                }
            }

            // Bullet-honeycomb
            for (int i = 0; i < honeycombCount; i++) {
                if (bullet_exists && honeycombs[i].getPosition().x >=0 && checkCollision(bulletSprite, honeycombs[i])) {
                    hitSound.play();
                    honeycombs[i].setPosition(-100, -100);
                    bullet_exists = false;
                }
            }

            // Bullet-hive
            for (int i = 0; i < hiveCount; i++) {
                if (bullet_exists && checkCollision(bulletSprite, hives[i])) {
                    hitSound.play();
                    hives[i].setPosition(-100, -100);
                    bullet_exists = false;

                    hiveCount--;
                    if (i != hiveCount) {
                        hives[i] = hives[hiveCount];
                    }
                    break;
                }
            }

            // Hummingbird reappear
            if (hummingbirdOffScreen && hummingbirdOffScreenClock.getElapsedTime().asSeconds() > 5.0f) {
                hummingbird.setPosition(rand() % (resolutionX - boxPixelsX), rand() % (resolutionY));
                hummingbirdOffScreen = false;
                hummingbirdHitCount = 0;
            }

            // Hummingbird functionality 
            if (hummingbirdEating && hummingbirdEatingIndex >= 0 && hummingbirdEatingIndex < honeycombCount) {
                if (hummingbirdHoneycombClocks[hummingbirdEatingIndex].getElapsedTime().asSeconds() > 5.0f) {
                    if (honeycombs[hummingbirdEatingIndex].getPosition().x >=0) {
                        float hy = honeycombs[hummingbirdEatingIndex].getPosition().y;
                        int tier = getTier(hy);
                        int points = 0;
                        if (tier < 2) {
                            points = 2000;
                        } else if (tier >= 2 && tier <= 4) {
                            points = 1600;
                        } else {
                            points = 1000;
                        }
                        score += points;
                        honeycombs[hummingbirdEatingIndex].setPosition(-100, -100);
                    if (score >= nextBonusScore) {
    lives++; // Grant a life
    if (nextBonusScore < 80000) {
        nextBonusScore *= 2; // Double the bonus threshold (20k -> 40k -> 80k)
    } else {
        nextBonusScore += 80000; // After 80k, add 80k for each subsequent bonus
    }
}

                    
                    
                    }
                    hummingbirdEating = false;
                    hummingbirdEatingIndex = -1;
                }
            } else {
                int nearestIndex = -1;
                float nearestDist = 1000000.0f;
                Vector2f hbPos = hummingbird.getPosition();
                for (int i = 0; i < honeycombCount; i++) {
                    if (honeycombs[i].getPosition().x >= 0) {
                        float dx = honeycombs[i].getPosition().x - hbPos.x;
                        float dy = honeycombs[i].getPosition().y - hbPos.y;
                        float dist = sqrt(dx*dx + dy*dy);
                        if (dist < nearestDist) {
                            nearestDist = dist;
                            nearestIndex = i;
                        }
                    }
                }

                if (nearestIndex == -1) {
                    if (hummingbirdRandomMoveClock.getElapsedTime().asSeconds() > 2.0f || (hummingbirdRandomDir.x == 0 && hummingbirdRandomDir.y ==0)) {
                        hummingbirdRandomMoveClock.restart();
                        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
                        hummingbirdRandomDir = Vector2f(cos(angle), sin(angle));
                    }

                    float hbSpeed = 1.5f;
                    Vector2f newPos = hbPos + hummingbirdRandomDir * hbSpeed;
                     float maxY = (gameColumns - 2) * boxPixelsY - hummingbird.getGlobalBounds().height;
                    if (newPos.x < 0 || newPos.x > resolutionX - boxPixelsX) {
                        hummingbirdRandomDir.x = -hummingbirdRandomDir.x;
                    }
                    if (newPos.y < 0 || newPos.y > (maxY)) {
                        hummingbirdRandomDir.y = -hummingbirdRandomDir.y;
                    }
                    hummingbird.move(hummingbirdRandomDir * hbSpeed);
                } else {
                    Vector2f hcPos = honeycombs[nearestIndex].getPosition();
                    Vector2f dir = hcPos - hbPos;
                    float length = sqrt(dir.x*dir.x + dir.y*dir.y);
                    if (length > 0) {
                        dir.x /= length;
                        dir.y /= length;
                    }
                    float hbSpeed = 1.5f;
                    if (length > 5.0f) {
                        hummingbird.move(dir * hbSpeed);
                    } else {
                        hummingbirdEating = true;
                        hummingbirdEatingIndex = nearestIndex;
                        hummingbirdHoneycombClocks[nearestIndex].restart();
                    }
                }
            }

            // Bee collides with spray can (regular bees)
            for (int i = 0; i < 4; i++) {
                if (checkCollision(bees[i], playerSprite)) {
                    if (firstBeeCollision) {
                        if (flowerCount < 49) {
                            flowers[flowerCount].setTexture(flowerTexture);
                            flowers[flowerCount].setPosition(0, groundRectangle.getPosition().y - boxPixelsY);
                            flowerCount++;

                            flowers[flowerCount].setTexture(flowerTexture);
                            flowers[flowerCount].setPosition(resolutionX - boxPixelsX, groundRectangle.getPosition().y - boxPixelsY);
                            flowerCount++;
                        }
                        firstBeeCollision = false;
                    }
                    else {
                        if (flowerCount < 50) {
                            flowers[flowerCount].setTexture(flowerTexture);
                            flowers[flowerCount].setPosition(player_x, groundRectangle.getPosition().y - boxPixelsY);
                            flowerCount++;
                        }
                    }



                    float playerCenterX = player_x + boxPixelsX / 2.0f;
                    if (fabs(playerCenterX - (resolutionX / 2.0f)) < (boxPixelsX / 2.0f) && flowerCount < 50) {
                        flowers[flowerCount].setTexture(flowerTexture);
                        flowers[flowerCount].setPosition((resolutionX / 2) - (boxPixelsX / 2), groundRectangle.getPosition().y - boxPixelsY);
                        flowerCount++;
                    }




                    if (!obstacleSoundPlayed) {
                        obstacleSound.play();
                        obstacleSoundPlayed = true;
                    }

                    if (player_x + boxPixelsX < resolutionX) {
                        player_x += boxPixelsX;
                    }
                    else if (player_x - boxPixelsX > 0) {
                        player_x -= boxPixelsX;
                    }

                    if (player_x < 0) player_x = 0;
                    if (player_x > resolutionX - boxPixelsX) player_x = resolutionX - boxPixelsX;

                    bees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
                    beeDirections[i] = (rand() % 2 == 0);
                }
            }

            // Check hive formation (2 honeycomb collisions)
            for (int i = 0; i < 4; i++) {
                if (bees[i].getPosition().x >= 0 && bees[i].getPosition().y >= 0) {
                    int collisionCount = 0;
                    for (int j = 0; j < honeycombCount; j++) {
                        if (honeycombs[j].getPosition().x >= 0 && honeycombs[j].getPosition().y >= 0) {
                            if (checkCollision(bees[i], honeycombs[j])) {
                                collisionCount++;
                            }
                        }
                    }
                    if (collisionCount == 1) {
                        // One honeycomb collision  reflect direction
                        for (int k = 0; k < honeycombCount; k++) {
                            if (honeycombs[k].getPosition().x >= 0 && honeycombs[k].getPosition().y >= 0) {
                                if (checkCollision(bees[i], honeycombs[k])) {
                                    beeDirections[i] = !beeDirections[i];
                                    break;
                                }
                            }
                        }
                    }
                    if (collisionCount >= 2) {
                        // Create hive
                        if (hiveCount < 50) {
                            hives[hiveCount].setTexture(hiveTexture);
                            hives[hiveCount].setPosition(bees[i].getPosition());
                            hiveCount++;
                        }

                        totalBeesEliminated++;
                        if (totalBeesSpawned < maxBees) {
                            bees[i].setPosition(rand() % (resolutionX - boxPixelsX), 50);
                            beeDirections[i] = (rand() % 2 == 0);
                            totalBeesSpawned++;
                        } else {
                            bees[i].setPosition(-100, -100);
                        }

                        int removed = 0;
                        for (int j = 0; j < honeycombCount && removed < 2; j++) {
                            if (honeycombs[j].getPosition().x >= 0 && honeycombs[j].getPosition().y >= 0 &&
                                checkCollision(bees[i], honeycombs[j])) {
                                honeycombs[j].setPosition(-100, -100);
                                removed++;
                            }
                        }
                    }
                }
            }

            // Check if cleared
            if (totalBeesEliminated == maxBees && !hiveBonusApplied) {
                bool allFastBeesGone = true;
                if (selectedLevel == 2) {
                    for (int i = 0; i < 5; i++) {
                        if (fastBeeActive[i]) {
                            allFastBeesGone = false;
                            break;
                        }
                    }
                }
                if (selectedLevel == 3) {
                    for (int i = 0; i < 10; i++) {
                        if (fastBeeActive[i]) {
                            allFastBeesGone = false;
                            break;
                        }
                    }
                }

                if ((selectedLevel == 1) ||
                    (selectedLevel == 2 && allFastBeesGone) ||
                    (selectedLevel == 3 && allFastBeesGone)) {
                    for (int i = 0; i < 4; i++) {
                        bees[i].setPosition(-100, -100);
                    }

                    for (int i = 0; i < honeycombCount; i++) {
                        if (honeycombs[i].getPosition().x >= 0) {
                            float hy = honeycombs[i].getPosition().y;
                            int tier = getTier(hy);
                            int points = 0;
                            if (tier < 2) {
                                points = 2000;
                            } else if (tier >= 2 && tier <= 4) {
                                points = 1600;
                            } else {
                                points = 1000;
                            }
                            leftoverHoneycombBonus += points;
                            honeycombs[i].setPosition(-100, -100);
                        }
                    }
                    score += leftoverHoneycombBonus;

                    hiveBonus = hiveCount * 1000;
                    score += hiveBonus;
                    hiveCount = 0;
                    hiveBonusApplied = true;
                     if (score >= nextBonusScore) {
    lives++; // Grant a life
    if (nextBonusScore < 80000) {
        nextBonusScore *= 2; // Double the bonus threshold (20k -> 40k -> 80k)
    } else {
        nextBonusScore += 80000; // After 80k, add 80k for each subsequent bonus
    }
}

                    gameState = STATE_LEVEL_PASSED;
                    selectedLevelPassedOption = 0;
                    levelPassedMenu[0].setFillColor(Color::Yellow);
                    levelPassedMenu[1].setFillColor(Color::White);

                }
            }

            for (int i = 0; i < hiveCount; i++) {
                window.draw(hives[i]);
            }

            if (!hummingbirdOffScreen) {
                window.draw(hummingbird);
            }

            for (int i = 0; i < flowerCount; i++) {
                window.draw(flowers[i]);
            }

            drawPlayer(window, player_x, player_y, playerSprite);
            for (int i = 0; i < honeycombCount; i++) window.draw(honeycombs[i]);
            for (int i = 0; i < 4; i++) window.draw(bees[i]);

            if (selectedLevel == 2) {
                for (int i = 0; i < 5; i++) {
                    if (fastBeeActive[i]) {
                        window.draw(fastBees[i]);
                    }
                }
            }

            if (selectedLevel == 3) {
                for (int i = 0; i < 10; i++) {
                    if (fastBeeActive[i]) {
                        window.draw(fastBees[i]);
                    }
                }
            }

            scoreText.setString("Score: " + to_string(score));
            window.draw(scoreText);
            window.draw(groundRectangle);
            // Draw lives and sprays 
            
            int displayedLives = (lives - 1 > 0) ? (lives - 1) : 0;
            float baseY = groundRectangle.getPosition().y + (64 - boxPixelsY)/2.0f; 
            float baseX = 10.0f;

            // Draw each life as a spray can icon
            for (int i = 0; i < displayedLives; i++) {
                Sprite lifeSprite(playerTexture);
                lifeSprite.setPosition(baseX + i*(boxPixelsX+8), baseY);
                window.draw(lifeSprite);
            }

            //  Show 7 cells next to spray can icons for each 8 sprays.
           
            int numCells = (spraysLeft + 7) / 8; 

            // Position cells after the lives icons
            float cellsStartX = baseX + displayedLives*(boxPixelsX+8) + 50.0f; 
            float cellsY = groundRectangle.getPosition().y + 22;
            Vector2f cellSize(20, 20);

            for (int i = 0; i < 7; i++) {
                RectangleShape cell(cellSize);
                cell.setPosition(cellsStartX + i*(cellSize.x+5), cellsY);
                if (i < numCells) {
                    // If only one cell left, it turns red
                    if (numCells == 1) {
                        cell.setFillColor(Color::Red);
                    } else {
                        cell.setFillColor(Color::White);
                    }
                } else {
                    cell.setFillColor(Color(100,100,100)); // grey or something to indicate empty
                }
                window.draw(cell);
            }

        

            Text levelIndicator;
            levelIndicator.setFont(font);
            levelIndicator.setCharacterSize(30);
            levelIndicator.setFillColor(Color::White);
            levelIndicator.setString("Level: " + to_string(selectedLevel));
            levelIndicator.setPosition(resolutionX - 150, 10);
            window.draw(levelIndicator);
        }
        else if (gameState == STATE_LEVEL_PASSED) {
            window.draw(backgroundSprite);  
            Text levelPassedText;
            levelPassedText.setFont(font);
            levelPassedText.setCharacterSize(50);
            levelPassedText.setFillColor(Color::Yellow);
            levelPassedText.setString("Level " + to_string(selectedLevel) + " Passed!");
            levelPassedText.setPosition(resolutionX / 2 - 200, resolutionY / 2 - 150);
            window.draw(levelPassedText);

            Text scoreDisplay;
            scoreDisplay.setFont(font);
            scoreDisplay.setCharacterSize(30);
            scoreDisplay.setFillColor(Color::White);
            scoreDisplay.setString("Score: " + to_string(score));
            scoreDisplay.setPosition(resolutionX / 2 - 100, resolutionY / 2 - 50);
            window.draw(scoreDisplay);

            Text hiveBonusText;
            hiveBonusText.setFont(font);
            hiveBonusText.setCharacterSize(30);
            hiveBonusText.setFillColor(Color::White);
            hiveBonusText.setString("Hive Bonus: " + to_string(hiveBonus));
            hiveBonusText.setPosition(resolutionX / 2 - 100, resolutionY / 2);
            window.draw(hiveBonusText);

            Text leftoverBonusText;
            leftoverBonusText.setFont(font);
            leftoverBonusText.setCharacterSize(30);
            leftoverBonusText.setFillColor(Color::White);
            leftoverBonusText.setString("Leftover Honeycomb Bonus: " + to_string(leftoverHoneycombBonus));
            leftoverBonusText.setPosition(resolutionX / 2 - 200, resolutionY / 2 + 40);
            window.draw(leftoverBonusText);

            for (int i = 0; i < 2; i++) {
                window.draw(levelPassedMenu[i]);
            }
        }
        else if (gameState == STATE_GAME_COMPLETED) {
            window.draw(backgroundSprite);  
            Text gameCompletedText;
            gameCompletedText.setFont(font);
            gameCompletedText.setCharacterSize(50);
            gameCompletedText.setFillColor(Color::Green);
            gameCompletedText.setString("Congratulations!\nAll Levels Completed!");
            gameCompletedText.setPosition(resolutionX / 2 - 250, resolutionY / 2 - 100);
            window.draw(gameCompletedText);

            Text finalScore;
            finalScore.setFont(font);
            finalScore.setCharacterSize(30);
            finalScore.setFillColor(Color::White);
            finalScore.setString("Final Score: " + to_string(score));
            finalScore.setPosition(resolutionX / 2 - 100, resolutionY / 2 + 20);
            window.draw(finalScore);

            Text exitPrompt;
            exitPrompt.setFont(font);
            exitPrompt.setCharacterSize(30);
            exitPrompt.setFillColor(Color::White);
            exitPrompt.setString("Press Enter to Exit");
            exitPrompt.setPosition(resolutionX / 2 - 120, resolutionY / 2 + 60);
            window.draw(exitPrompt);
        }
        else if (gameState == STATE_LEVEL_FAILED) {
            Text levelFailedText;
            levelFailedText.setFont(font);
            levelFailedText.setCharacterSize(50);
            levelFailedText.setFillColor(Color::Red);
            levelFailedText.setString("Level Failed!");
            levelFailedText.setPosition(resolutionX / 2 - 150, resolutionY / 2 - 150);
            window.draw(levelFailedText);

            Text scoreDisplay;
            scoreDisplay.setFont(font);
            scoreDisplay.setCharacterSize(30);
            scoreDisplay.setFillColor(Color::White);
            scoreDisplay.setString("Final Score: " + to_string(score));
            scoreDisplay.setPosition(resolutionX / 2 - 100, resolutionY / 2 - 50);
            window.draw(scoreDisplay);

            for (int i = 0; i < 2; i++) {
                window.draw(levelFailedMenu[i]);
            }
        }

        window.display();
    }

    return 0;
}


int getTier(float y) {
    return static_cast<int>(y / boxPixelsY);
}

// Reads high scores from file into array, returns number of scores read
int readHighScores(const char* filename, char names[][50], int scores[], int maxScores) {
    ifstream fin(filename);
    int count = 0;
    if (fin.is_open()) {
        while (count < maxScores && fin >> names[count] >> scores[count]) {
            count++;
        }
        fin.close();
    }
    return count;
}

// Writes scores to file
void writeHighScores(const char* filename, char names[][50], int scores[], int count) {
    ofstream fout(filename, ios::trunc);
    for (int i = 0; i < count; i++) {
        fout << names[i] << " " << scores[i] << "\n";
    }
    fout.close();
}
// Simplly sort scores
void sortScores(char names[][50], int scores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j] > scores[j + 1]) {
                // Swap scores
                int tempScore = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tempScore;

                // Swap names
                char tempName[50];
                strcpy(tempName, names[j]);
                strcpy(names[j], names[j + 1]);
                strcpy(names[j + 1], tempName);
            }
        }
    }
}



void addHighScore(const char* filename, char names[][50], int scores[], int& count, const char* name, int score) {
    if (count < MAX_SCORES) {
        strcpy(names[count], name);
        scores[count] = score;
        count++;
       sortScores(playerNames, playerScores, count);

        writeHighScores(filename, names, scores, count);
    }
}

bool checkCollision(Sprite& sprite1, Sprite& sprite2) {
    return sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds());
}

void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite) {
    playerSprite.setPosition(player_x, player_y);
    window.draw(playerSprite);
}

void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock) {
    if (bulletClock.getElapsedTime().asMilliseconds() < 20)
        return;

    bulletClock.restart();
    bullet_y -= 25;
    if (bullet_y < -32)
        bullet_exists = false;
}

void drawBullet(RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite) {
    bulletSprite.setPosition(bullet_x, bullet_y);
    window.draw(bulletSprite);
}

// Move regular bees
void moveBees(Sprite bees[], bool directions[], int numBees, float speed, Sprite honeycombs[], int honeycombCount, Sprite hives[], int& hiveCount, Texture& hiveTexture, Clock beeStopClocks[], bool beeStopped[], float beeStopDuration[]) {
    for (int i = 0; i < numBees; i++) {
        if (beeStopped[i]) {
            if (beeStopClocks[i].getElapsedTime().asSeconds() >= beeStopDuration[i]) {
                beeStopped[i] = false;
            } else {
                continue;
            }
        }

        float oldX = bees[i].getPosition().x;
        float oldY = bees[i].getPosition().y;

        if (directions[i]) {
            bees[i].move(speed, 0);
        } else {
            bees[i].move(-speed, 0);
        }

        if (bees[i].getPosition().x < 0) {
            bees[i].setPosition(0, bees[i].getPosition().y);
            directions[i] = !directions[i];
            bees[i].move(0, 40);
            if (rand() % 3 == 0 && !beeStopped[i]) {
                beeStopped[i] = true;
                beeStopClocks[i].restart();
                beeStopDuration[i] = 1.5f;
            }
        } else if (bees[i].getPosition().x + boxPixelsX > resolutionX) {
            bees[i].setPosition(resolutionX - boxPixelsX, bees[i].getPosition().y);
            directions[i] = !directions[i];
            bees[i].move(0, 40);
            if (rand() % 3 == 0 && !beeStopped[i]) {
                beeStopped[i] = true;
                beeStopClocks[i].restart();
                beeStopDuration[i] = 1.5f;
            }
        }
    }
}

void moveFastBees(Sprite fastBees[], bool fastBeeDirections[], bool fastBeeActive[], int numFastBees, float fastBeeSpeed) {
    for (int i = 0; i < numFastBees; i++) {
        if (!fastBeeActive[i]) continue;

        if (fastBeeDirections[i]) {
            fastBees[i].move(fastBeeSpeed, 0);
            if (fastBees[i].getPosition().x + boxPixelsX >= resolutionX) {
                fastBeeDirections[i] = false;
                fastBees[i].move(0, 40);
            }
        } else {
            fastBees[i].move(-fastBeeSpeed, 0);
            if (fastBees[i].getPosition().x <= 0) {
                fastBeeDirections[i] = true;
                fastBees[i].move(0, 40);
            }
        }
    }
}
