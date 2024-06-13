#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

using namespace std;

// WORDS FILES PATHS
string wordFilePath_en = "..\\assets\\words_eng.txt";
string wordFilePath_pl = "..\\assets\\words_pl.txt";

// METHODS
void authorDisplay();
int wordsCounter(const string& path);
vector<string> wordsToArray(const string& path);
vector<string> wordsToArray(const string& path1, const string& path2);
bool isLetter(sf::Uint32 input);
sf::Text generateText(const vector<string>& wordsArray, int wordsCount, const sf::Font& font, int previousY);
int generateRandomNumber(int start, int limit);
void loadFonts();
sf::Font fontList[10];
bool isClickable(sf::Text& text);
bool isClickableVol(const sf::Sprite& sprite);

// DIFFICULTY SETTINGS
enum DIFFICULTY {
    EASY, MEDIUM, HARD, MAD
};
void difficultySetup(DIFFICULTY difficulty);

DIFFICULTY currentDifficulty;
string difficultyLabel;
int fontBound;
int speed;
int livesLeft;
float scoreMultiplier;
bool eng;
bool pl;

// MOUSE COORDINATES (RELATIVE TO WINDOW)
int mouseX;
int mouseY;

// WINDOW COORDINATES
int winX = -1;
int winY = -1;

// PLAYER STATS
struct Player{

    string name;
    int score;

    Player(string n, int s) : name(std::move(n)), score(s){
    }

    void serialize(ostream& save) const {
        save << name << " " << score << endl;
    }

    void deserialize(istream& load) {
        load >> name >> score;
    }

};
map<int, Player> scoreboard;
void updateScoreboard(const Player& player);

//GAME SAVE
void autoSave();
void loadSave();

// STATISTICS VARIABLES
int score;
float tries;
float goodTries;

// TRUE - PROGRAM IS RUNNING
bool isRunning = true;

// SOUNDTRACK
sf::Sound musicBg;

// WINDOWS
void menuWindow();
void gameWindow(DIFFICULTY difficulty);
string saveScoreWindow();
void leaderBoardWindow();

auto main() -> int {

    ////   ALL GRAPHICS AND BACKGROUND MUSIC ARE SELF-MADE. THE FONTS USED ARE BUILT INTO THE WINDOWS SYSTEM :)

    // ASCII ART :)
    authorDisplay();

    // AUTO LOADING SAVED STATS
    loadSave();

    // FONTS TO ARRAY LOADER
    loadFonts();

    // BACKGROUND MUSIC SETUP
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("..\\assets\\music.mp3")){
        cerr << "Audio file not found!";
        return -1;
    }

        musicBg.setBuffer(buffer);
        musicBg.play();
        musicBg.setLoop(true);

    // GAME WINDOWS SWITCH PROCESS LOOP
    while (isRunning) {

        menuWindow();

        if (!isRunning)
            break;

        gameWindow(currentDifficulty);

        if (!isRunning)
            break;

        string nick = saveScoreWindow();

        if (!isRunning)
            break;

        leaderBoardWindow();

        if (!isRunning)
            break;

    }

}

// WINDOWS
void menuWindow() {

    // WORDS
    eng = false;
    pl = false;

    auto window = sf::RenderWindow(
            sf::VideoMode(1000, 700),
            "Ninja typer | Maciej Sepeta s30518 19c | Main menu",
            sf::Style::Close
    );

    if (winX != -1 && winY != -1)
        window.setPosition(sf::Vector2i(winX, winY));

    auto icon = sf::Image{};
    if (!icon.loadFromFile("..\\assets\\icon.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        window.setIcon(400, 400, icon.getPixelsPtr());
    }

    auto volumeIcon = sf::Texture{};
    auto volumeSprite = sf::Sprite{};
    if (!volumeIcon.loadFromFile("..\\assets\\volumeIcon.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        volumeSprite.setTexture(volumeIcon);
        volumeSprite.setScale(sf::Vector2f(0.3, 0.3));
        volumeSprite.setPosition(5, 5);
    }

    auto volumeIconOff = sf::Texture{};
    auto volumeSpriteOff = sf::Sprite{};
    if (!volumeIconOff.loadFromFile("..\\assets\\volumeIconOff.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        volumeSpriteOff.setTexture(volumeIconOff);
        volumeSpriteOff.setScale(sf::Vector2f(0.3, 0.3));
        volumeSpriteOff.setPosition(5, 5);
    }

    auto bg = sf::Texture{};
    auto bgSprite = sf::Sprite{};
    if (!bg.loadFromFile("..\\assets\\bg.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        bgSprite.setTexture(bg);
    }

    window.setFramerateLimit(60);

    sf::Clock blinkerClock;
    sf::Clock mouseDelayClock;

    sf::Text titleLabel;
    titleLabel.setString("NINJA TYPER");
    titleLabel.setFillColor(sf::Color::White);
    titleLabel.setFont(fontList[5]);
    titleLabel.setCharacterSize(50);
    titleLabel.setPosition((window.getSize().x-titleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-140);

    sf::Text subTitleLabel;
    subTitleLabel.setString("WTyper copy c++ project");
    subTitleLabel.setFillColor(sf::Color(192, 192, 192));
    subTitleLabel.setFont(fontList[5]);
    subTitleLabel.setCharacterSize(20);
    subTitleLabel.setPosition((window.getSize().x-subTitleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-80);

    sf::Text anyKeyLabel;
    anyKeyLabel.setString("Press any key to continue...");
    anyKeyLabel.setFillColor(sf::Color::White);
    anyKeyLabel.setFont(fontList[5]);
    anyKeyLabel.setCharacterSize(15);
    anyKeyLabel.setPosition((window.getSize().x-anyKeyLabel.getLocalBounds().width)*0.5, window.getSize().y/2 - 30);

    sf::Text easyLabel;
    easyLabel.setString("Easy");
    easyLabel.setFillColor(sf::Color::White);
    easyLabel.setFont(fontList[5]);
    easyLabel.setCharacterSize(20);
    easyLabel.setPosition((window.getSize().x-easyLabel.getLocalBounds().width)*0.5, window.getSize().y/2 - 20);

    sf::Text mediumLabel;
    mediumLabel.setString("Medium");
    mediumLabel.setFillColor(sf::Color::White);
    mediumLabel.setFont(fontList[5]);
    mediumLabel.setCharacterSize(20);
    mediumLabel.setPosition((window.getSize().x-mediumLabel.getLocalBounds().width)*0.5, window.getSize().y/2 + 30);

    sf::Text hardLabel;
    hardLabel.setString("Hard");
    hardLabel.setFillColor(sf::Color::White);
    hardLabel.setFont(fontList[5]);
    hardLabel.setCharacterSize(20);
    hardLabel.setPosition((window.getSize().x-hardLabel.getLocalBounds().width)*0.5, window.getSize().y/2 + 80);

    sf::Text madLabel;
    madLabel.setString("MAD EXAM");
    madLabel.setFillColor(sf::Color::White);
    madLabel.setFont(fontList[5]);
    madLabel.setCharacterSize(20);
    madLabel.setPosition((window.getSize().x-madLabel.getLocalBounds().width)*0.5, window.getSize().y/2 + 130);

    sf::Text engLabel;
    engLabel.setString("English");
    engLabel.setFillColor(sf::Color::White);
    engLabel.setFont(fontList[5]);
    engLabel.setCharacterSize(20);
    engLabel.setPosition((window.getSize().x-engLabel.getLocalBounds().width)*0.5, window.getSize().y/2);

    sf::Text plLabel;
    plLabel.setString("Polish");
    plLabel.setFillColor(sf::Color::White);
    plLabel.setFont(fontList[5]);
    plLabel.setCharacterSize(20);
    plLabel.setPosition((window.getSize().x-plLabel.getLocalBounds().width)*0.5, window.getSize().y/2 + 50);

    sf::Text startLabel;
    startLabel.setString("Start");
    startLabel.setFillColor(sf::Color::White);
    startLabel.setFont(fontList[5]);
    startLabel.setCharacterSize(20);
    startLabel.setPosition((window.getSize().x-startLabel.getLocalBounds().width)*0.5, window.getSize().y/2 + 100);

    int currentMenuNumber = 0;

    while(window.isOpen()){

        mouseX = sf::Mouse::getPosition(window).x;
        mouseY = sf::Mouse::getPosition(window).y;

        if (isClickable(easyLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if (currentMenuNumber == 1) {
                    currentMenuNumber++;
                    currentDifficulty = EASY;
                }

            }
        } else if (isClickable(mediumLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if (currentMenuNumber == 1) {
                    currentMenuNumber++;
                    currentDifficulty = MEDIUM;
                }

            }
        } else if (isClickable(hardLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if (currentMenuNumber == 1) {
                    currentMenuNumber++;
                    currentDifficulty = HARD;
                }

            }
        } else if (isClickable(madLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if (currentMenuNumber == 1) {
                    currentMenuNumber++;
                    currentDifficulty = MAD;
                }

            }
        } else if (isClickable(engLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if(currentMenuNumber >= 2) {
                    if (mouseDelayClock.getElapsedTime().asMilliseconds() >= 200) {
                        mouseDelayClock.restart();
                        if (eng) {
                            eng = false;
                        } else {
                            eng = true;
                        }
                    }
                }

            }
        } else if (isClickable(plLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if (currentMenuNumber >= 2) {
                    if (mouseDelayClock.getElapsedTime().asMilliseconds() >= 200) {
                        mouseDelayClock.restart();
                        if (pl) {
                            pl = false;
                        } else {
                            pl = true;
                        }
                    }
                }

            }
        } else if (isClickable(startLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if ((eng || pl) && currentMenuNumber >= 2) {
                    winX = window.getPosition().x;
                    winY = window.getPosition().y;
                    window.close();
                }

            }
        } else if (isClickableVol(volumeSprite) || isClickableVol(volumeSpriteOff)){

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                if(mouseDelayClock.getElapsedTime().asMilliseconds() > 200) {
                    if (musicBg.getStatus() == sf::SoundSource::Playing) {

                        musicBg.stop();
                    } else {
                        musicBg.play();
                    }

                    mouseDelayClock.restart();

                }

            }

        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {

            if (currentMenuNumber == 0 && (!isClickableVol(volumeSprite) || !isClickableVol(volumeSpriteOff)))
                currentMenuNumber++;

        }

        for(auto event = sf::Event(); window.pollEvent(event); ){

            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;
            }

            if (event.type == sf::Event::KeyPressed){

                switch (event.key.code) {
                    case sf::Keyboard::S:
                        if (musicBg.getStatus() == sf::SoundSource::Playing) {
                            musicBg.stop();
                        } else {
                            musicBg.play();
                        }
                        break;
                }

                if (currentMenuNumber == 0){
                    currentMenuNumber++;
                } else if (currentMenuNumber == 1) {
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            currentMenuNumber--;
                            blinkerClock.restart();
                            break;
                        case sf::Keyboard::Num1:
                            currentMenuNumber++;
                            currentDifficulty = EASY;
                            break;
                        case sf::Keyboard::Num2:
                            currentMenuNumber++;
                            currentDifficulty = MEDIUM;
                            break;
                        case sf::Keyboard::Num3:
                            currentMenuNumber++;
                            currentDifficulty = HARD;
                            break;
                        case sf::Keyboard::Num4:
                            currentMenuNumber++;
                            currentDifficulty = MAD;
                            break;
                    }
                } else{
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            currentMenuNumber--;
                            blinkerClock.restart();
                            break;
                        default:
                            window.close();
                    }
                }
            }
        }

        window.clear();

        window.draw(bgSprite);

        if (musicBg.getStatus() == sf::SoundSource::Playing) {
            window.draw(volumeSprite);
        } else {
            window.draw(volumeSpriteOff);
        }

        window.draw(titleLabel);

        if (currentMenuNumber == 0){
            subTitleLabel.setString("WTyper copy c++ project");
            subTitleLabel.setPosition((window.getSize().x-subTitleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-80);


            if (blinkerClock.getElapsedTime().asMilliseconds() < 800){
                anyKeyLabel.setFillColor(sf::Color(128, 128, 128));
            } else {
                anyKeyLabel.setFillColor(sf::Color::White);
                if (blinkerClock.getElapsedTime().asMilliseconds() > 1600)
                    blinkerClock.restart();
            }
            window.draw(anyKeyLabel);
        } else if (currentMenuNumber == 1){
            subTitleLabel.setString("Pick a difficulty...");
            subTitleLabel.setPosition((window.getSize().x-subTitleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-80);

            window.draw(easyLabel);
            window.draw(mediumLabel);
            window.draw(hardLabel);
            window.draw(madLabel);
        } else if (currentMenuNumber >= 2) {
            currentMenuNumber = 2;

            subTitleLabel.setString("Pick words language...");
            subTitleLabel.setPosition((window.getSize().x-subTitleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-80);

            window.draw(engLabel);
            window.draw(plLabel);

            if (pl || eng)
                window.draw(startLabel);

        }

        window.draw(subTitleLabel);

        window.display();

    }
}

void gameWindow(DIFFICULTY difficulty) {

    score = 0;
    tries = 0;
    goodTries = 0;

    int wordsCount = 0;

    if (eng && pl){
        wordsCount = wordsCounter(wordFilePath_en) + wordsCounter(wordFilePath_pl);
    } else if (eng){
        wordsCount = wordsCounter(wordFilePath_en);
    } else if (pl){
        wordsCount = wordsCounter(wordFilePath_pl);
    }

    auto wordsList = vector<string>();

    if (eng && pl){
        wordsList = wordsToArray(wordFilePath_en, wordFilePath_pl);
    } else if (eng){
        wordsList = wordsToArray(wordFilePath_en);
    } else if (pl){
        wordsList = wordsToArray(wordFilePath_pl);
    }

    difficultySetup(difficulty);

    auto window = sf::RenderWindow(
            sf::VideoMode(1000, 700),
            "Ninja typer | Maciej Sepeta s30518 19c | Difficulty: " + difficultyLabel,
            sf::Style::Close
    );
    window.setPosition(sf::Vector2i(winX, winY));
    window.setFramerateLimit(60);


    auto bg = sf::Texture{};
    auto bgSprite = sf::Sprite{};

    switch (currentDifficulty) {
        case EASY:
            if (!bg.loadFromFile("..\\assets\\bgEasy.png")){
                cerr << "Error loading icon!" << endl;
            } else {
                bgSprite.setTexture(bg);
            }
            break;
        case MEDIUM:
            if (!bg.loadFromFile("..\\assets\\bgMedium.png")){
                cerr << "Error loading icon!" << endl;
            } else {
                bgSprite.setTexture(bg);
            }
            break;
        case HARD:
            if (!bg.loadFromFile("..\\assets\\bgHard.png")){
                cerr << "Error loading icon!" << endl;
            } else {
                bgSprite.setTexture(bg);
            }
            break;
        case MAD:
            if (!bg.loadFromFile("..\\assets\\bgMad.png")){
                cerr << "Error loading icon!" << endl;
            } else {
                bgSprite.setTexture(bg);
            }
            break;
    }

    auto icon = sf::Image{};
    if (!icon.loadFromFile("..\\assets\\icon.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        window.setIcon(400, 400, icon.getPixelsPtr());
    }

    auto hpIcon = sf::Texture{};
    auto hp1 = sf::Sprite{};
    auto hp2 = sf::Sprite{};
    auto hp3 = sf::Sprite{};
    if (!hpIcon.loadFromFile("..\\assets\\hp.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        hp1.setTexture(hpIcon);
        hp1.setScale(sf::Vector2f(0.06, 0.06));
        hp1.setPosition(window.getSize().x-30, window.getSize().y-30);

        hp2.setTexture(hpIcon);
        hp2.setScale(sf::Vector2f(0.06, 0.06));
        hp2.setPosition(window.getSize().x-60, window.getSize().y-30);

        hp3.setTexture(hpIcon);
        hp3.setScale(sf::Vector2f(0.06, 0.06));
        hp3.setPosition(window.getSize().x-90, window.getSize().y-30);
    }

    sf::RectangleShape bottomBar(sf::Vector2f(window.getSize().x, 50));
    bottomBar.setFillColor(sf::Color(102, 102, 102));
    bottomBar.setPosition(0, window.getSize().y-50);

    sf::RectangleShape textInputFrame(sf::Vector2f(280, 20));
    textInputFrame.setOutlineColor(sf::Color::White);
    textInputFrame.setFillColor(sf::Color(102, 102, 102));
    textInputFrame.setOutlineThickness(5);
    textInputFrame.setPosition(15, window.getSize().y - 35);

    sf::RectangleShape bottomBarCover(sf::Vector2f(window.getSize().x, 50));
    bottomBarCover.setFillColor(sf::Color(102, 102, 102));
    bottomBarCover.setPosition(textInputFrame.getPosition().x + textInputFrame.getLocalBounds().width - 5, window.getSize().y-50);

    sf::Clock timer;
    sf::Clock delay;

    sf::Text timerDisplay;
    timerDisplay.setString("time: 000");
    timerDisplay.setFillColor(sf::Color::White);
    timerDisplay.setPosition(textInputFrame.getPosition().x + textInputFrame.getLocalBounds().width+20, textInputFrame.getPosition().y-10);
    timerDisplay.setFont(fontList[5]);
    timerDisplay.setCharacterSize(15);

    sf::Text livesDisplay;
    livesDisplay.setString("Lives:");
    livesDisplay.setFillColor(sf::Color::White);
    livesDisplay.setFont(fontList[5]);
    livesDisplay.setCharacterSize(15);
    livesDisplay.setPosition(window.getSize().x-livesDisplay.getLocalBounds().width-5, window.getSize().y-50);

    sf::Text scoreDisplay;
    scoreDisplay.setFillColor(sf::Color::White);
    scoreDisplay.setPosition(timerDisplay.getPosition().x + timerDisplay.getLocalBounds().width + 180, timerDisplay.getPosition().y);
    scoreDisplay.setFont(fontList[5]);
    scoreDisplay.setCharacterSize(30);

    sf::Text accuracyDisplay;
    accuracyDisplay.setFillColor(sf::Color::White);
    accuracyDisplay.setPosition(timerDisplay.getPosition().x, scoreDisplay.getPosition().y + scoreDisplay.getLocalBounds().height + 20);
    accuracyDisplay.setFont(fontList[5]);
    accuracyDisplay.setCharacterSize(15);

    sf::String textInput;

    sf::Text textInputDisplay;
    textInputDisplay.setFillColor(sf::Color::White);
    textInputDisplay.setPosition(20, window.getSize().y - 38);
    textInputDisplay.setFont(fontList[5]);
    textInputDisplay.setCharacterSize(20);

    sf::Text firstText = generateText(wordsList, wordsCount, fontList[generateRandomNumber(0, fontBound)], -1);

    map<string, sf::Text> playgroundTextMap;
    playgroundTextMap[firstText.getString().toAnsiString()] = firstText;

    int previousY = firstText.getPosition().y;

    while(window.isOpen()){

        srand(time(NULL));

        vector<map<string, sf::Text>::iterator> iterToClear;

        for(auto event = sf::Event(); window.pollEvent(event); ){

            if (event.type == sf::Event::TextEntered && textInput.getSize() <= 20) {
                if (isLetter(event.text.unicode)){

                    string currentText;
                    currentText += event.text.unicode;

                    std::transform(currentText.begin(), currentText.end(), currentText.begin(), [](unsigned char letter) {
                        return tolower(letter);
                    });

                    textInput += currentText;
                }
            }

            if (event.type == sf::Event::KeyPressed) {

                switch (event.key.code) {
                    case sf::Keyboard::BackSpace:
                        textInput = textInput.substring(0, textInput.getSize()-1);
                        break;
                    case sf::Keyboard::Enter:

                        tries++;

                        for (auto iterator = playgroundTextMap.begin(); iterator != playgroundTextMap.end(); iterator++) {

                            if (iterator->first == textInput.toAnsiString()) {

                                goodTries++;

                                iterator->second.setFillColor(sf::Color::Green);
                                iterToClear.push_back(iterator);
                                score += ceil(iterator->first.size()*scoreMultiplier);
                                break;
                            }

                        }

                        textInput = "";
                }

            }

            textInputDisplay.setString(textInput);

            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;
            }
        }

        timerDisplay.setString("Time: " + to_string(round(timer.getElapsedTime().asSeconds())).substr(0, to_string(round(timer.getElapsedTime().asSeconds())).find('.')));

        scoreDisplay.setString("Score: " + to_string(score));

        int accuracy = ceil((goodTries / tries) * 100);

        if (tries != 0) {
            accuracyDisplay.setString("Accuracy: " + to_string(accuracy) + " %");
        } else {
            accuracyDisplay.setString("Accuracy: 0 %");
        }

        window.clear();

        window.draw(bgSprite);
        window.draw(bottomBar);
        window.draw(textInputFrame);
        window.draw(textInputDisplay);
        window.draw(bottomBarCover);
        window.draw(timerDisplay);
        window.draw(livesDisplay);
        window.draw(scoreDisplay);
        window.draw(accuracyDisplay);

        if (livesLeft == 0) {
            winX = window.getPosition().x;
            winY = window.getPosition().y;
            window.close();
        } else {

            switch (livesLeft) {
                case 3:
                    window.draw(hp3);
                case 2:
                    window.draw(hp2);
                case 1:
                    window.draw(hp1);
            }

        }

        int currentDelay = 5000;

        if (timer.getElapsedTime().asSeconds() > 400){
            currentDelay = 200;
        } else if (timer.getElapsedTime().asSeconds() > 200){
            currentDelay = 400;
        } else if (timer.getElapsedTime().asSeconds() > 100){
            currentDelay = 500;
        } else if (timer.getElapsedTime().asSeconds() > 50) {
            currentDelay = 1000;
        } else if (timer.getElapsedTime().asSeconds() > 35){
            currentDelay = 2000;
        } else if (timer.getElapsedTime().asSeconds() > 20){
            currentDelay = 3000;
        } else if (timer.getElapsedTime().asSeconds() > 5){
            currentDelay = 4000;
        }

        if (delay.getElapsedTime().asMilliseconds() >= currentDelay){
            delay.restart();

            sf::Text newText = generateText(wordsList, wordsCount, fontList[generateRandomNumber(0, fontBound)], previousY);

            previousY = newText.getPosition().y;

            if (currentDifficulty == MEDIUM || currentDifficulty == HARD || currentDifficulty == MAD)
                newText.setFillColor(sf::Color(generateRandomNumber(20, 256), generateRandomNumber(20, 256),generateRandomNumber(20, 256)));

            playgroundTextMap[newText.getString().toAnsiString()] = newText;
        }

        for (auto iterator = playgroundTextMap.begin(); iterator != playgroundTextMap.end(); iterator++) {

            iterator->second.setPosition(iterator->second.getPosition().x + speed, iterator->second.getPosition().y);

            if(currentDifficulty == MAD)
                iterator->second.rotate(5);


            if (iterator->second.getPosition().x <= window.getSize().x) {
                window.draw(iterator->second);
            } else {
                iterToClear.push_back(iterator);
                livesLeft--;
            }

        }

        for (auto i : iterToClear) {
            playgroundTextMap.erase(i);
        }

        window.display();

    }

}

string saveScoreWindow() {

    auto window = sf::RenderWindow(
            sf::VideoMode(1000, 700),
            "Ninja typer | Maciej Sepeta s30518 19c | Score: " + to_string(score),
            sf::Style::Close
    );

    window.setPosition(sf::Vector2i(winX, winY));

    window.setFramerateLimit(60);

    auto icon = sf::Image{};
    if (!icon.loadFromFile("..\\assets\\icon.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        window.setIcon(400, 400, icon.getPixelsPtr());
    }

    auto bg = sf::Texture{};
    auto bgSprite = sf::Sprite{};
    if (!bg.loadFromFile("..\\assets\\bg.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        bgSprite.setTexture(bg);
    }

    sf::Text titleLabel;
    titleLabel.setString("CONGRATULATIONS!");
    titleLabel.setFillColor(sf::Color::White);
    titleLabel.setFont(fontList[5]);
    titleLabel.setCharacterSize(50);
    titleLabel.setPosition((window.getSize().x-titleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-140);

    sf::Text subTitleLabel;
    subTitleLabel.setString("SCORE: " + to_string(score));
    subTitleLabel.setFillColor(sf::Color(192, 192, 192));
    subTitleLabel.setFont(fontList[5]);
    subTitleLabel.setCharacterSize(20);
    subTitleLabel.setPosition((window.getSize().x-subTitleLabel.getLocalBounds().width)*0.5, window.getSize().y/2-80);

    sf::RectangleShape underline(sf::Vector2f(300, 5));
    underline.setFillColor(sf::Color::White);
    underline.setPosition((window.getSize().x-underline.getLocalBounds().width)*0.5, 350);

    sf::Text nameLabel;
    nameLabel.setString("Name:");
    nameLabel.setFillColor(sf::Color(192, 192, 192));
    nameLabel.setFont(fontList[5]);
    nameLabel.setCharacterSize(15);
    nameLabel.setPosition((window.getSize().x-underline.getLocalBounds().width)*0.5 - 75, window.getSize().y/2-25);

    sf::Text inputNameLabel;
    inputNameLabel.setString("Name:");
    inputNameLabel.setFillColor(sf::Color(192, 192, 192));
    inputNameLabel.setFont(fontList[5]);
    inputNameLabel.setCharacterSize(15);
    inputNameLabel.setPosition((window.getSize().x-underline.getLocalBounds().width)*0.5, window.getSize().y/2-25);

    sf::String textInput;

    while(window.isOpen()){

        for(auto event = sf::Event(); window.pollEvent(event); ){

            if (event.type == sf::Event::TextEntered && textInput.getSize() <= 10) {
                if (isLetter(event.text.unicode)){

                    string currentText;
                    currentText += event.text.unicode;

                    std::transform(currentText.begin(), currentText.end(), currentText.begin(), [](unsigned char letter) {
                        return tolower(letter);
                    });

                    textInput += currentText;
                }
            }

            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;
            }

            if (event.type == sf::Event::KeyPressed){
                switch (event.key.code) {
                    case sf::Keyboard::Enter:
                        if (inputNameLabel.getString().toAnsiString().size() > 0){

                            updateScoreboard(Player(textInput.toAnsiString(), score));

                            autoSave();

                            winX = window.getPosition().x;
                            winY = window.getPosition().y;
                            return inputNameLabel.getString().toAnsiString();
                        }
                        break;
                    case sf::Keyboard::BackSpace:
                        textInput = textInput.substring(0, textInput.getSize()-1);
                        break;
                }
            }
        }

        inputNameLabel.setString(textInput);

        window.clear();

        window.draw(bgSprite);

        window.draw(titleLabel);
        window.draw(subTitleLabel);
        window.draw(underline);
        window.draw(nameLabel);
        window.draw(inputNameLabel);

        window.display();

    }
}

void leaderBoardWindow() {

    auto window = sf::RenderWindow(
            sf::VideoMode(1000, 700),
            "Ninja typer | Maciej Sepeta s30518 19c | Leaderboard",
            sf::Style::Close
    );

    window.setPosition(sf::Vector2i(winX, winY));

    window.setFramerateLimit(60);

    auto icon = sf::Image{};
    if (!icon.loadFromFile("..\\assets\\icon.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        window.setIcon(400, 400, icon.getPixelsPtr());
    }

    auto bg = sf::Texture{};
    auto bgSprite = sf::Sprite{};
    if (!bg.loadFromFile("..\\assets\\bg.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        bgSprite.setTexture(bg);
    }

    // GitHub LOGO FROM GITHUB WEBSITE
    auto gitIcon = sf::Texture{};
    auto gitSprite = sf::Sprite{};
    if (!gitIcon.loadFromFile("..\\assets\\gitLogo.png")){
        cerr << "Error loading icon!" << endl;
    } else {
        gitSprite.setTexture(gitIcon);
        gitSprite.setScale(sf::Vector2f(0.05, 0.05));
        gitSprite.setPosition(10, window.getSize().y-35);
    }

    sf::Text gitNameLabel;
    gitNameLabel.setString("Macieksep");
    gitNameLabel.setFillColor(sf::Color::White);
    gitNameLabel.setFont(fontList[5]);
    gitNameLabel.setCharacterSize(20);
    gitNameLabel.setPosition(52, window.getSize().y-35);

    sf::Text nameLabel;
    nameLabel.setString("name");
    nameLabel.setFillColor(sf::Color::White);
    nameLabel.setFont(fontList[5]);
    nameLabel.setCharacterSize(30);
    nameLabel.setPosition(60, 10);

    sf::Text scoreLabel;
    scoreLabel.setString("score");
    scoreLabel.setFillColor(sf::Color::White);
    scoreLabel.setFont(fontList[5]);
    scoreLabel.setCharacterSize(30);
    scoreLabel.setPosition(360, 10);

    sf::Text nr1Label;
    nr1Label.setString("1");
    nr1Label.setFillColor(sf::Color::White);
    nr1Label.setFont(fontList[5]);
    nr1Label.setCharacterSize(30);
    nr1Label.setPosition(10, 60);

    sf::Text nr2Label;
    nr2Label.setString("2");
    nr2Label.setFillColor(sf::Color::White);
    nr2Label.setFont(fontList[5]);
    nr2Label.setCharacterSize(30);
    nr2Label.setPosition(10, 110);

    sf::Text nr3Label;
    nr3Label.setString("3");
    nr3Label.setFillColor(sf::Color::White);
    nr3Label.setFont(fontList[5]);
    nr3Label.setCharacterSize(30);
    nr3Label.setPosition(10, 160);

    sf::Text nr4Label;
    nr4Label.setString("4");
    nr4Label.setFillColor(sf::Color::White);
    nr4Label.setFont(fontList[5]);
    nr4Label.setCharacterSize(30);
    nr4Label.setPosition(10, 210);

    sf::Text nr5Label;
    nr5Label.setString("5");
    nr5Label.setFillColor(sf::Color::White);
    nr5Label.setFont(fontList[5]);
    nr5Label.setCharacterSize(30);
    nr5Label.setPosition(10, 260);

    sf::Text nr6Label;
    nr6Label.setString("6");
    nr6Label.setFillColor(sf::Color::White);
    nr6Label.setFont(fontList[5]);
    nr6Label.setCharacterSize(30);
    nr6Label.setPosition(10, 310);

    sf::Text nr7Label;
    nr7Label.setString("7");
    nr7Label.setFillColor(sf::Color::White);
    nr7Label.setFont(fontList[5]);
    nr7Label.setCharacterSize(30);
    nr7Label.setPosition(10, 360);

    sf::Text nr8Label;
    nr8Label.setString("8");
    nr8Label.setFillColor(sf::Color::White);
    nr8Label.setFont(fontList[5]);
    nr8Label.setCharacterSize(30);
    nr8Label.setPosition(10, 410);

    sf::Text nr9Label;
    nr9Label.setString("9");
    nr9Label.setFillColor(sf::Color::White);
    nr9Label.setFont(fontList[5]);
    nr9Label.setCharacterSize(30);
    nr9Label.setPosition(10, 460);

    sf::Text nr10Label;
    nr10Label.setString("10");
    nr10Label.setFillColor(sf::Color::White);
    nr10Label.setFont(fontList[5]);
    nr10Label.setCharacterSize(30);
    nr10Label.setPosition(10, 510);

    sf::Text name1Label;
    name1Label.setString(scoreboard.find(0)->second.name);
    name1Label.setFillColor(sf::Color::White);
    name1Label.setFont(fontList[5]);
    name1Label.setCharacterSize(30);
    name1Label.setPosition(60, 60);

    sf::Text name2Label;
    name2Label.setString(scoreboard.find(1)->second.name);
    name2Label.setFillColor(sf::Color::White);
    name2Label.setFont(fontList[5]);
    name2Label.setCharacterSize(30);
    name2Label.setPosition(60, 110);

    sf::Text name3Label;
    name3Label.setString(scoreboard.find(2)->second.name);
    name3Label.setFillColor(sf::Color::White);
    name3Label.setFont(fontList[5]);
    name3Label.setCharacterSize(30);
    name3Label.setPosition(60, 160);

    sf::Text name4Label;
    name4Label.setString(scoreboard.find(3)->second.name);
    name4Label.setFillColor(sf::Color::White);
    name4Label.setFont(fontList[5]);
    name4Label.setCharacterSize(30);
    name4Label.setPosition(60, 210);

    sf::Text name5Label;
    name5Label.setString(scoreboard.find(4)->second.name);
    name5Label.setFillColor(sf::Color::White);
    name5Label.setFont(fontList[5]);
    name5Label.setCharacterSize(30);
    name5Label.setPosition(60, 260);

    sf::Text name6Label;
    name6Label.setString(scoreboard.find(5)->second.name);
    name6Label.setFillColor(sf::Color::White);
    name6Label.setFont(fontList[5]);
    name6Label.setCharacterSize(30);
    name6Label.setPosition(60, 310);

    sf::Text name7Label;
    name7Label.setString(scoreboard.find(6)->second.name);
    name7Label.setFillColor(sf::Color::White);
    name7Label.setFont(fontList[5]);
    name7Label.setCharacterSize(30);
    name7Label.setPosition(60, 360);

    sf::Text name8Label;
    name8Label.setString(scoreboard.find(7)->second.name);
    name8Label.setFillColor(sf::Color::White);
    name8Label.setFont(fontList[5]);
    name8Label.setCharacterSize(30);
    name8Label.setPosition(60, 410);

    sf::Text name9Label;
    name9Label.setString(scoreboard.find(8)->second.name);
    name9Label.setFillColor(sf::Color::White);
    name9Label.setFont(fontList[5]);
    name9Label.setCharacterSize(30);
    name9Label.setPosition(60, 460);

    sf::Text name10Label;
    name10Label.setString(scoreboard.find(9)->second.name);
    name10Label.setFillColor(sf::Color::White);
    name10Label.setFont(fontList[5]);
    name10Label.setCharacterSize(30);
    name10Label.setPosition(60, 510);

    sf::Text score1Label;
    score1Label.setString(to_string(scoreboard.find(0)->second.score));
    score1Label.setFillColor(sf::Color::White);
    score1Label.setFont(fontList[5]);
    score1Label.setCharacterSize(30);
    score1Label.setPosition(360, 60);

    sf::Text score2Label;
    score2Label.setString(to_string(scoreboard.find(1)->second.score));
    score2Label.setFillColor(sf::Color::White);
    score2Label.setFont(fontList[5]);
    score2Label.setCharacterSize(30);
    score2Label.setPosition(360, 110);

    sf::Text score3Label;
    score3Label.setString(to_string(scoreboard.find(2)->second.score));
    score3Label.setFillColor(sf::Color::White);
    score3Label.setFont(fontList[5]);
    score3Label.setCharacterSize(30);
    score3Label.setPosition(360, 160);

    sf::Text score4Label;
    score4Label.setString(to_string(scoreboard.find(3)->second.score));
    score4Label.setFillColor(sf::Color::White);
    score4Label.setFont(fontList[5]);
    score4Label.setCharacterSize(30);
    score4Label.setPosition(360, 210);

    sf::Text score5Label;
    score5Label.setString(to_string(scoreboard.find(4)->second.score));
    score5Label.setFillColor(sf::Color::White);
    score5Label.setFont(fontList[5]);
    score5Label.setCharacterSize(30);
    score5Label.setPosition(360, 260);

    sf::Text score6Label;
    score6Label.setString(to_string(scoreboard.find(5)->second.score));
    score6Label.setFillColor(sf::Color::White);
    score6Label.setFont(fontList[5]);
    score6Label.setCharacterSize(30);
    score6Label.setPosition(360, 310);

    sf::Text score7Label;
    score7Label.setString(to_string(scoreboard.find(6)->second.score));
    score7Label.setFillColor(sf::Color::White);
    score7Label.setFont(fontList[5]);
    score7Label.setCharacterSize(30);
    score7Label.setPosition(360, 360);

    sf::Text score8Label;
    score8Label.setString(to_string(scoreboard.find(7)->second.score));
    score8Label.setFillColor(sf::Color::White);
    score8Label.setFont(fontList[5]);
    score8Label.setCharacterSize(30);
    score8Label.setPosition(360, 410);

    sf::Text score9Label;
    score9Label.setString(to_string(scoreboard.find(8)->second.score));
    score9Label.setFillColor(sf::Color::White);
    score9Label.setFont(fontList[5]);
    score9Label.setCharacterSize(30);
    score9Label.setPosition(360, 460);

    sf::Text score10Label;
    score10Label.setString(to_string(scoreboard.find(9)->second.score));
    score10Label.setFillColor(sf::Color::White);
    score10Label.setFont(fontList[5]);
    score10Label.setCharacterSize(30);
    score10Label.setPosition(360, 510);

    sf::Text continueLabel;
    continueLabel.setString("Continue >");
    continueLabel.setFillColor(sf::Color::White);
    continueLabel.setFont(fontList[5]);
    continueLabel.setCharacterSize(50);
    continueLabel.setPosition(550, 130);

    sf::RectangleShape cover(sf::Vector2f(100, 560));
    cover.setFillColor(sf::Color::Black);
    cover.setPosition(355, 60);

    while(window.isOpen()){

        mouseX = sf::Mouse::getPosition(window).x;
        mouseY = sf::Mouse::getPosition(window).y;

        if (isClickable(continueLabel)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                winX = window.getPosition().x;
                winY = window.getPosition().y;
                window.close();

            }
        }

        for(auto event = sf::Event(); window.pollEvent(event); ){
            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;
            }

            if (event.type == sf::Event::KeyPressed){
                switch (event.key.code) {

                }
            }
        }

        window.clear();

        window.draw(bgSprite);

        window.draw(gitSprite);
        window.draw(gitNameLabel);

        window.draw(nameLabel);
        window.draw(scoreLabel);

        window.draw(nr1Label);
        window.draw(nr2Label);
        window.draw(nr3Label);
        window.draw(nr4Label);
        window.draw(nr5Label);
        window.draw(nr6Label);
        window.draw(nr7Label);
        window.draw(nr8Label);
        window.draw(nr9Label);
        window.draw(nr10Label);

        window.draw(name1Label);
        window.draw(name2Label);
        window.draw(name3Label);
        window.draw(name4Label);
        window.draw(name5Label);
        window.draw(name6Label);
        window.draw(name7Label);
        window.draw(name8Label);
        window.draw(name9Label);
        window.draw(name10Label);

        window.draw(cover);

        window.draw(score1Label);
        window.draw(score2Label);
        window.draw(score3Label);
        window.draw(score4Label);
        window.draw(score5Label);
        window.draw(score6Label);
        window.draw(score7Label);
        window.draw(score8Label);
        window.draw(score9Label);
        window.draw(score10Label);

        window.draw(continueLabel);

        window.display();

    }
}

//METHODS
auto wordsCounter(const string& path) -> int {

    ifstream wordsFile;
    wordsFile.open(path);

    string line;
    auto lineCounter = 0;
    while (getline(wordsFile, line)){

        lineCounter++;

    }

    wordsFile.close();

    return lineCounter;

}

auto wordsToArray(const string& path) -> vector<string> {

    ifstream wordsFile;
    wordsFile.open(path);

    auto wordsList = vector<string>();

    string currentLine;

    while (getline(wordsFile, currentLine)){
        wordsList.push_back(currentLine);
    }

    wordsFile.close();

    return wordsList;

}

auto wordsToArray(const string& path1, const string& path2) -> vector<string> {

    ifstream wordsFile1;
    wordsFile1.open(path1);

    auto wordsList = vector<string>();

    string currentLine;

    while (getline(wordsFile1, currentLine)){
        wordsList.push_back(currentLine);
    }

    wordsFile1.close();

    ifstream wordsFile2;
    wordsFile2.open(path2);

    while (getline(wordsFile2, currentLine)){
        wordsList.push_back(currentLine);
    }

    wordsFile2.close();

    return wordsList;

}

auto loadFonts() -> void {

    // 0 - easy
    // 0 - 3 - medium
    // 0 - 6 - hard
    // 0 - 9 - MAD exam

    if (!fontList[0].loadFromFile("..\\assets\\fonts\\arial.ttf")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[1].loadFromFile("..\\assets\\fonts\\Baloo.ttf")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[2].loadFromFile("..\\assets\\fonts\\Bauhs.ttf")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[3].loadFromFile("..\\assets\\fonts\\BERNHC.TTF")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[4].loadFromFile("..\\assets\\fonts\\ELEPHNT.TTF")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[5].loadFromFile("..\\assets\\fonts\\HOOG.ttf")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[6].loadFromFile("..\\assets\\fonts\\COLONNA.ttf")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[7].loadFromFile("..\\assets\\fonts\\Lemon.ttf")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[8].loadFromFile("..\\assets\\fonts\\CURLZ.TTF")) {
        cerr << "Error loading font!" << std::endl;
    }

    if (!fontList[9].loadFromFile("..\\assets\\fonts\\BROADW.TTF")) {
        cerr << "Error loading font!" << std::endl;
    }

}

bool isLetter(sf::Uint32 input) {

    // CHECKS IF INPUT CONTAINS LETTERS ONLY
    return isalpha(input) != 0;

}

sf::Text generateText(const vector<string>& wordsArray, int wordsCount, const sf::Font& font, int previousY) {

    sf::Text text;

    srand(time(NULL));

    int randIndex = generateRandomNumber(0, wordsCount-1);

    text.setString(wordsArray[randIndex]);

    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);

    int newY;

    do {
        if (currentDifficulty != MAD) {
            newY = generateRandomNumber(0, 10) * 60;
        } else {
            newY = generateRandomNumber(1, 9) * 60;
        }
    } while (newY == previousY);

    text.setPosition(text.getLocalBounds().width*(-1)-30, newY);

    return text;

}

auto generateRandomNumber(int start, int limit) -> int {

    if (limit == 0)
        return 0;

    return rand() % limit + start;

}

void difficultySetup(DIFFICULTY difficulty) {

    currentDifficulty = difficulty;

    switch (difficulty) {
        case EASY:
            difficultyLabel = "Easy";
            scoreMultiplier = 1.0;
            speed = 1;
            fontBound = 0;
            livesLeft = 3;
            break;
        case MEDIUM:
            difficultyLabel = "Medium";
            scoreMultiplier = 1.5;
            speed = 2;
            fontBound = 3;
            livesLeft = 3;
            break;
        case HARD:
            difficultyLabel = "Hard";
            scoreMultiplier = 2;
            speed = 3;
            fontBound = 6;
            livesLeft = 3;
            break;
        case MAD:
            difficultyLabel = "MAD exam";
            scoreMultiplier = 4;
            speed = 5;
            fontBound = 9;
            livesLeft = 1;
            break;
    }

}

bool isClickable(sf::Text& text){

    // CHECKS IF MOUSE HOOVER OVER BUTTON
    if (mouseX >= text.getPosition().x
        && mouseX <= (text.getPosition().x+text.getLocalBounds().width)
        && mouseY >= text.getPosition().y
        && mouseY <= (text.getPosition().y+text.getLocalBounds().height+5)) {

        if ((text.getString().toAnsiString() == "English" && eng) || (text.getString().toAnsiString() == "Polish" && pl)){
            text.setFillColor(sf::Color(0, 102, 0));
        } else {
            text.setFillColor(sf::Color(192, 192, 192));
        }

        return true;
    } else {

        if ((text.getString().toAnsiString() == "English" && eng) || (text.getString().toAnsiString() == "Polish" && pl)){
            text.setFillColor(sf::Color::Green);
        } else {
            text.setFillColor(sf::Color::White);
        }

        return false;
    }
}

bool isClickableVol(const sf::Sprite& sprite){

    // CHECKS IF MOUSE HOOVER OVER BUTTON
    if (mouseX >= sprite.getPosition().x
        && mouseX <= (sprite.getPosition().x+(sprite.getLocalBounds().width/3))
        && mouseY >= sprite.getPosition().y
        && mouseY <= (sprite.getPosition().y+((sprite.getLocalBounds().height+5)/3))) {

        return true;
    } else {

        return false;
    }
}

void updateScoreboard(const Player& player){

    scoreboard.insert(std::make_pair(11, player));

    vector<std::pair<int, Player>> sortVec(scoreboard.begin(), scoreboard.end());

    // SORTS DESCENDING USING COMPARATOR
    sort(sortVec.begin(), sortVec.end(), [](const auto& left, const auto& right) {
        return left.second.score > right.second.score;
    });

    int i = 0;

    scoreboard = map<int, Player>();

    // ASSINGS UPDATED SCOREBOARD
    for (const auto& pair : sortVec) {
        if (i != 10){
            scoreboard.insert(std::make_pair(i++, Player(pair.second.name, pair.second.score)));
        } else {
            break;
        }
    }

}

// SAVE AND LOAD USING SERIALIZATION
void autoSave() {

    ofstream saveFile("..\\assets\\save\\gamesave");

    if (saveFile.is_open()) {

        for (const auto& pair : scoreboard) {
            saveFile << pair.first << " ";
            pair.second.serialize(saveFile);
        }
        saveFile.close();

    } else {
        cerr << "Failed to save!";
    }

}

void loadSave() {

    map<int, Player> loadMap;

    ifstream saveFile("..\\assets\\save\\gamesave");

    if (saveFile.is_open()) {

        int key;

        while (saveFile >> key) {

            Player loadPlayer("", 0);
            loadPlayer.deserialize(saveFile);
            scoreboard.insert(std::make_pair(key++, loadPlayer));

        }

        saveFile.close();

    } else {
        cerr << "Failed to save!";
    }

}

void authorDisplay() {

    cout << "# #  #   ## ### ###  ##      ## ### ##  ### ###  #          ### ### ###  #  ###      #  ###         ##   ##  ##" << endl;
    cout << "### # # #    #  #     #     #   #   # # #    #  # #      ##   # # # #   ##  # #     ##  # # ###     # #   # #  " << endl;
    cout << "### ### #    #  ##    #      #  ##  ##  ##   #  ###      #   ## # # ###  #  ###      #  ### #       ##    # #  " << endl;
    cout << "# # # # #    #  #   # #       # #   #   #    #  # #     ##    # # #   #  #  # #      #    # ###     #   # # #  " << endl;
    cout << "# # # #  ## ### ###  #      ##  ### #   ###  #  # #         ### ### ### ### ###     ### ###         #    #   ##" << endl;

}

////  $$\      $$\  $$$$$$\   $$$$$$\  $$$$$$\ $$$$$$$$\   $$$$$\        $$$$$$\  $$$$$$$$\ $$$$$$$\  $$$$$$$$\ $$$$$$$$\  $$$$$$\                   $$$$$$\   $$$$$$\  $$$$$$$\    $$\   $$$$$$\          $$\   $$$$$$\                  $$$$$$$\    $$$$$\  $$$$$$\
////  $$$\    $$$ |$$  __$$\ $$  __$$\ \_$$  _|$$  _____|  \__$$ |      $$  __$$\ $$  _____|$$  __$$\ $$  _____|\__$$  __|$$  __$$\                 $$ ___$$\ $$$ __$$\ $$  ____| $$$$ | $$  __$$\       $$$$ | $$  __$$\                 $$  __$$\   \__$$ |$$  __$$\
////  $$$$\  $$$$ |$$ /  $$ |$$ /  \__|  $$ |  $$ |           $$ |      $$ /  \__|$$ |      $$ |  $$ |$$ |         $$ |   $$ /  $$ |       $$$$$$$\ \_/   $$ |$$$$\ $$ |$$ |      \_$$ | $$ /  $$ |      \_$$ | $$ /  $$ | $$$$$$$\       $$ |  $$ |     $$ |$$ /  \__|
////  $$\$$\$$ $$ |$$$$$$$$ |$$ |        $$ |  $$$$$\         $$ |      \$$$$$$\  $$$$$\    $$$$$$$  |$$$$$\       $$ |   $$$$$$$$ |      $$  _____|  $$$$$ / $$\$$\$$ |$$$$$$$\    $$ |  $$$$$$  |        $$ | \$$$$$$$ |$$  _____|      $$$$$$$  |     $$ |$$ |
////  $$ \$$$  $$ |$$  __$$ |$$ |        $$ |  $$  __|  $$\   $$ |       \____$$\ $$  __|   $$  ____/ $$  __|      $$ |   $$  __$$ |      \$$$$$$\    \___$$\ $$ \$$$$ |\_____$$\   $$ | $$  __$$<         $$ |  \____$$ |$$ /            $$  ____/$$\   $$ |$$ |
////  $$ |\$  /$$ |$$ |  $$ |$$ |  $$\   $$ |  $$ |     $$ |  $$ |      $$\   $$ |$$ |      $$ |      $$ |         $$ |   $$ |  $$ |       \____$$\ $$\   $$ |$$ |\$$$ |$$\   $$ |  $$ | $$ /  $$ |        $$ | $$\   $$ |$$ |            $$ |     $$ |  $$ |$$ |  $$\
////  $$ | \_/ $$ |$$ |  $$ |\$$$$$$  |$$$$$$\ $$$$$$$$\\$$$$$$  |      \$$$$$$  |$$$$$$$$\ $$ |      $$$$$$$$\    $$ |   $$ |  $$ |      $$$$$$$  |\$$$$$$  |\$$$$$$  /\$$$$$$  |$$$$$$\\$$$$$$  |      $$$$$$\\$$$$$$  |\$$$$$$$\       $$ |     \$$$$$$  |\$$$$$$  |
////  \__|     \__|\__|  \__| \______/ \______|\________|\______/        \______/ \________|\__|      \________|   \__|   \__|  \__|      \_______/  \______/  \______/  \______/ \______|\______/       \______|\______/  \_______|      \__|      \______/  \______/



