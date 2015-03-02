#include "GamePlay.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/AppManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../GameModels/LetterCoords.h"
#include "../GameConstants/GameConstants.h"
#include "../CustomGUI/SpriteButton.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Helpers/DataFileManager.h"

using namespace cocos2d;
using namespace std;

const Value PlayMaxTime = Value(120.0f);
const Value TimeToReduce = Value(5.0f);
const Value TimeMinPercentage = Value(25);
const Value SuccessTime = Value(1.5);

const Color4B BorderColor = IkasGrayLight;
const float BorderWidth = 2;

const float LineRadius = 30;
const Color4F LineColorTouch = Color4F(Color4B(235, 235, 235, 80));
const Color4F LineColorSuccess = Color4F(Color4B(216, 225, 158, 120));
const Color4F LineColorError = Color4F(Color4B(240, 173, 182, 120));

Scene* GamePlay::createScene()
{
    auto *scene = Scene::create();
    auto *layer = GamePlay::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool GamePlay::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    DataFileManager *dataManager = DataFileManager::getInstance();
    _categories = dataManager->getMainCategories();
    if (_categories.empty()) {
        dataManager->parseDataFile();
        _categories = dataManager->getMainCategories();
    }
    
    AppManager::getInstance()->setGamePlayDelegate(this);
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(background);
    
    float marginsHeight = ScreenSizeManager::getHeightFromPercentage(5);
    float marginWidth = ScreenSizeManager::getWidthFromPercentage(5);
    
    Size dataLayerSize = Size(0, 0);
    dataLayerSize.height = ScreenSizeManager::getHeightFromPercentage(100);
    dataLayerSize.height -= marginsHeight * 2;
    dataLayerSize.width = ScreenSizeManager::getWidthFromPercentage(35);
    dataLayerSize.width -= marginWidth;
    
    _gameLayer = LayerColor::create(Color4B(255, 255, 255, 255));
    _hudLayer = Layer::create();
    
    Size gameLayerSize = Size(0, 0);
    float width = ScreenSizeManager::getWidthFromPercentage(100) - 3 * marginWidth - dataLayerSize.width;
    float height = ScreenSizeManager::getHeightFromPercentage(100) - 2 * marginsHeight;
    if (width < height)
    {
        gameLayerSize.width = width;
        gameLayerSize.height = gameLayerSize.width;
    }
    else
    {
        gameLayerSize.height = height;
        gameLayerSize.width = gameLayerSize.height;
    }
    
    _gameLayer->setContentSize(gameLayerSize);
    _gameLayer->setPosition(
                            ScreenSizeManager::getWidthFromPercentage(100) - marginWidth - _gameLayer->getBoundingBox().size.width,
                            this->getBoundingBox().size.height / 2 - _gameLayer->getBoundingBox().size.height / 2);
    
    _hudLayer->setContentSize(dataLayerSize);
    _hudLayer->setPosition(ScreenSizeManager::getWidthFromPercentage(0) + marginWidth, this->getBoundingBox().size.height / 2 - _hudLayer->getBoundingBox().size.height / 2);
    
    Size spriteSize = _hudLayer->getBoundingBox().size;
    
    auto badge = Sprite::create(ImageManager::getImage("category-showoff"));
    badge->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    
    float maxSize = 0;
    float scale = 1.0f;
    
    // Create circle
    if (spriteSize.width < spriteSize.height) {
        maxSize = spriteSize.width;
        scale = spriteSize.width / badge->getBoundingBox().size.width;
    } else {
        maxSize = spriteSize.height;
        scale = spriteSize.height / badge->getBoundingBox().size.height;
    }
    // Append min margin
    scale -= 0.30f;
    badge->setScale(scale);
    badge->setPosition(_hudLayer->getBoundingBox().size.width / 2, _hudLayer->getBoundingBox().size.height);
    _hudLayer->addChild(badge);
    
    int categoryIndex = GamePlayPointsManager::getInstance()->getCurrentCategory();
    MainCategory mainCategory = *_categories.at(categoryIndex);
    
    maxSize = 0;
    scale = 1.0f;
    spriteSize = badge->getBoundingBox().size;
    auto categoryImg = Sprite::create(ZipManager::getInstance()->getDataFolderFullPath("hd/topics/" + mainCategory.getFileName()));
    float maxSizePercentage = 0.85f;
    if (spriteSize.width < spriteSize.height) {
        maxSize = spriteSize.width;
        scale = spriteSize.width * maxSizePercentage / categoryImg->getBoundingBox().size.width;
    } else {
        maxSize = spriteSize.height;
        scale = spriteSize.width * maxSizePercentage / categoryImg->getBoundingBox().size.height;
    }
    categoryImg->setScale(scale);
    categoryImg->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    categoryImg->setPosition(
                             _hudLayer->getBoundingBox().size.width / 2,
                             _hudLayer->getBoundingBox().size.height - ((badge->getBoundingBox().size.width - categoryImg->getBoundingBox().size.width) / 2)
                             );
    
    _hudLayer->addChild(categoryImg);
    
    Size miniLayerSizes = Size(0, 0);
    miniLayerSizes.width = _hudLayer->getBoundingBox().size.width;
    miniLayerSizes.height = _hudLayer->getBoundingBox().size.height / 10;
    
    auto layerCategory = Layer::create();
    layerCategory->setPosition(marginWidth / 2, _hudLayer->getBoundingBox().size.height * 0.45);
    
    auto labelTitleCategory = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "category-title"), MainRegularFont, 45);
    labelTitleCategory->setWidth(miniLayerSizes.width);
    labelTitleCategory->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitleCategory->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitleCategory->setTextColor(IkasBlue);
    layerCategory->addChild(labelTitleCategory);
    
    _labelCategory = Label::createWithTTF(" ", MainBoldFont, 50);
    _labelCategory->setWidth(miniLayerSizes.width);
    _labelCategory->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelCategory->setVerticalAlignment(TextVAlignment::TOP);
    _labelCategory->setTextColor(IkasGreen);
    layerCategory->addChild(_labelCategory);
    
    miniLayerSizes.height = labelTitleCategory->getContentSize().height + _labelCategory->getContentSize().height;
    layerCategory->setContentSize(miniLayerSizes);
    
    labelTitleCategory->setPosition(0, layerCategory->getBoundingBox().size.height);
    _labelCategory->setPosition(0, 0);
    
    _hudLayer->addChild(layerCategory);
    
    auto layerLevel = Layer::create();
    layerLevel->setPosition(marginWidth / 2, _hudLayer->getBoundingBox().size.height * 0.35);
    
    auto labelTitleLevel = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "level-title"), MainRegularFont, 45);
    labelTitleLevel->setWidth(miniLayerSizes.width);
    labelTitleLevel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitleLevel->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitleLevel->setTextColor(IkasBlue);
    layerLevel->addChild(labelTitleLevel);
    
    _labelLevel = Label::createWithTTF("0", MainBoldFont, 50);
    _labelLevel->setWidth(miniLayerSizes.width);
    _labelLevel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelLevel->setVerticalAlignment(TextVAlignment::TOP);
    _labelLevel->setTextColor(IkasGreen);
    layerLevel->addChild(_labelLevel);
    
    miniLayerSizes.height = labelTitleLevel->getContentSize().height + _labelLevel->getContentSize().height;
    layerLevel->setContentSize(miniLayerSizes);
    
    labelTitleLevel->setPosition(0, layerLevel->getBoundingBox().size.height);
    _labelLevel->setPosition(0, 0);
    
    _hudLayer->addChild(layerLevel);
    
    
    auto layerPoints = Layer::create();
    layerPoints->setPosition(marginWidth / 2, _hudLayer->getBoundingBox().size.height * 0.25);
    
    auto labelTitlePoints = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "points-title"), MainRegularFont, 45);
    labelTitlePoints->setWidth(miniLayerSizes.width);
    labelTitlePoints->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitlePoints->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitlePoints->setTextColor(IkasBlue);
    layerPoints->addChild(labelTitlePoints);
    
    _labelPoints = Label::createWithTTF("0", MainBoldFont, 50);
    _labelPoints->setWidth(miniLayerSizes.width);
    _labelPoints->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelPoints->setVerticalAlignment(TextVAlignment::TOP);
    _labelPoints->setTextColor(IkasGreen);
    layerPoints->addChild(_labelPoints);
    
    miniLayerSizes.height = labelTitlePoints->getContentSize().height + _labelPoints->getContentSize().height;
    layerPoints->setContentSize(miniLayerSizes);
    
    labelTitlePoints->setPosition(0, layerPoints->getBoundingBox().size.height);
    _labelPoints->setPosition(0, 0);
    
    _hudLayer->addChild(layerPoints);
    
    // Timer!
    Sprite* timerBackground = Sprite::create(ImageManager::getImage("timer-background"));
    timerBackground->setScale(0.6f);
    timerBackground->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
    Vec2 centerPos = Vec2(_hudLayer->getBoundingBox().size.width, _hudLayer->getBoundingBox().size.height * 0.15);
    timerBackground->setPosition(centerPos);
    
    Sprite* timer = Sprite::create(ImageManager::getImage("timer-in"));
    
    _progress = ProgressTimer::create(timer);
    _progress->setScale(timerBackground->getScale());
    _progress->setAnchorPoint(timerBackground->getAnchorPoint());
    _progress->setPosition(timerBackground->getPosition());
    
    _hudLayer->addChild(timerBackground);
    _hudLayer->addChild(_progress);
    
    _progress->setType(ProgressTimer::Type::RADIAL);
    _progress->setMidpoint(Point::ANCHOR_MIDDLE);

    _pauseButton = SpriteButton::create(ImageManager::getImage("pause"), 0.4f, CC_CALLBACK_1(GamePlay::pauseGame, this));
    _pauseButton->setPosition(_hudLayer->getPosition().x, _hudLayer->getBoundingBox().size.height * 0.10);
    _pauseButton->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
    _hudLayer->addChild(_pauseButton);
    
    auto borderLayer = LayerColor::create(BorderColor);
    Size borderLayerSize = _gameLayer->getBoundingBox().size;
    borderLayerSize.width += 2 * BorderWidth;
    borderLayerSize.height += 2 * BorderWidth;
    borderLayer->setContentSize(borderLayerSize);
    Vec2 borderLayerPosition = _gameLayer->getPosition();
    borderLayerPosition.x -= BorderWidth;
    borderLayerPosition.y -= BorderWidth;
    borderLayer->setPosition(borderLayerPosition);
    
    this->addChild(borderLayer);
    this->addChild(_gameLayer);
    this->addChild(_hudLayer);
    
    _currentTableSize = IKASZOPA_SMALL_TABLE_SIZE;
    
    _drawError = DrawNode::create();
    _drawSuccess = DrawNode::create();
    _drawTouch = DrawNode::create();
    
    _menuLayer = Layer::create();
    _menuLayer->setContentSize(_gameLayer->getBoundingBox().size);
    _menuLayer->setPosition(Point::ZERO);
    _gameLayer->addChild(_menuLayer);
    
    _touchLayer = Layer::create();
    _touchLayer->setContentSize(gameLayerSize);
    _gameLayer->addChild(_touchLayer);
    
    _touchLayer->addChild(_drawError);
    _touchLayer->addChild(_drawSuccess);
    _touchLayer->addChild(_drawTouch);
    
    auto listener = addEvents();
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _touchLayer);
    _touchState = TouchState::FINISH;
    
    this->setupLevelSettings();
    this->restartGame();
    
    return true;
}

void GamePlay::setupLevelSettings()
{
    _totalSuccessScreens = 0;
    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
    switch (difficulty) {
            break;
        case Difficulty::EASY:
            _maxTime = PlayMaxTime.asFloat() * 1.50f;
            break;
        case Difficulty::MEDIUM:
            _maxTime = PlayMaxTime.asFloat() * 1.00f;
            break;
        case Difficulty::HARD:
            _maxTime = PlayMaxTime.asFloat() * 0.50f;
            break;
        default:
            _maxTime = PlayMaxTime.asFloat();
            break;
    }
}

void GamePlay::resetTimer()
{
    _action = NULL;
    _sequence = NULL;
    _progress->stopAllActions();
    
    _action = ProgressFromTo::create(_currentLevelTime - _elapsedTime, _elapsedTime / _currentLevelTime * 100, 100); //10 seconds, from 0 to 100
    CallFunc* callback = CallFunc::create(CC_CALLBACK_0(GamePlay::timeElapsed, this));
    _sequence = Sequence::create(_action, callback, NULL);
    _progress->runAction(_sequence);
}

void GamePlay::pauseGame(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    SceneManager::getInstance()->runSceneWithType(SceneType::PAUSE);
    _elapsedTime += _sequence->getElapsed();
    _progress->stopAction(_sequence);
}

void GamePlay::prepareTable()
{
    _menuLayer->removeAllChildrenWithCleanup(true);
    
    Menu *menu = Menu::create();
    menu->setContentSize(_gameLayer->getBoundingBox().size);
    menu->setPosition(Point::ZERO);
    
    float itemSquareSize = _gameLayer->getContentSize().width / _currentTableSize;
    
    for (int i = 0; i < _map.size(); i++)
    {
        vector<string> innerMap = _map.at(i);
        for (int j = 0; j < innerMap.size(); j++) {
            auto letterLbl = Label::createWithTTF(innerMap.at(j).c_str(), MainRegularFont, 70);
            letterLbl->setName(innerMap.at(j).c_str());
            letterLbl->setVerticalAlignment(TextVAlignment::CENTER);
            letterLbl->setHorizontalAlignment(TextHAlignment::CENTER);
            letterLbl->setAnchorPoint(Point::ZERO);
            letterLbl->setTextColor(IkasGrayDark);
            letterLbl->setWidth(itemSquareSize);
            auto item = MenuItemLabel::create(letterLbl, CC_CALLBACK_1(GamePlay::labelClicked, this));
            item->setTag((i * 100) + j);
            item->setContentSize(Size(itemSquareSize, itemSquareSize));
            item->setAnchorPoint(Point::ZERO);
            item->setPosition(Vec2(itemSquareSize * j, _gameLayer->getBoundingBox().size.height - (itemSquareSize * i) - itemSquareSize));
            menu->addChild(item);
        }
    }
    _menuLayer->addChild(menu);
}

void GamePlay::labelClicked(Ref* sender)
{
    MenuItemLabel *menuItem = static_cast<MenuItemLabel*>(sender);
    ostringstream stream;
    stream << menuItem->getTag();
    string coordsStr = stream.str();
    int coords = menuItem->getTag();
    int row = 0;
    int column = 0;
    if (coords < 100) {
        if (row != _lastTouchedRow)
        {
            clearOnRowChange(0, coords);
        }
        _touchedWord += charsBetweenColumns(0, coords);
    }
    else
    {
        row = std::atoi(coordsStr.substr(0, coordsStr.length() - 2).c_str());
        column = std::atoi(coordsStr.substr(coordsStr.length() - 2, coordsStr.length()).c_str());
        if (row != _lastTouchedRow) {
            clearOnRowChange(row, column);
        }
        _touchedWord += charsBetweenColumns(row, column);
    }
    log("TouchedWord: %s \n", _touchedWord.c_str());
    checkValidWord();
}

void GamePlay::clearOnRowChange(int row, int coords)
{
    _touchedWord = "";
     _lastTouchedRow = row;
    _lastTouchedColumn = coords;
    _drawTouch->clear();
    if (!_touchVector.empty()) {
       Touch *touch = _touchVector.at(0);
        _touchVector.clear();
        _touchVector.insert(0, touch);
    }
}

string GamePlay::charsBetweenColumns(int row, int currentColumn)
{
    int diff = 0;
    bool reverse = false;
    string result = "";
    if (currentColumn < _lastTouchedColumn)
    {
        diff = _lastTouchedColumn - currentColumn;
        reverse = true;
    }
    else
    {
        diff = currentColumn - _lastTouchedColumn;
    }
    if (diff > 1)
    {
        if (reverse) {
            for (int i = 1; i <= diff; i++) {
                result += _map.at(row).at(_lastTouchedColumn - i).c_str();
            }
        }
        else
        {
            for (int i = 1; i <= diff; i++) {
                result += _map.at(row).at(_lastTouchedColumn + i).c_str();
            }
        }
    }
    else
    {
        result += _map.at(row).at(currentColumn).c_str();
    }
    _lastTouchedColumn = currentColumn;
    return result;
}

bool GamePlay::checkValidWord()
{
    string word = _touchedWord;
    string reverseWord = string(_touchedWord.rbegin(), _touchedWord.rend());
    Vec2 endTouch, startTouch;
    if (!_touchVector.empty())
    {
        endTouch = _touchLayer->convertTouchToNodeSpace(_touchVector.at(0));
        startTouch = _touchLayer->convertTouchToNodeSpace(_touchVector.at(_touchVector.size() - 1));
    }
    for (int i = 0; i < _words.size(); i++) {
        if (_words.at(i)->getWord() == word || _words.at(i)->getWord() == reverseWord) {
            log("BINGO!");
            _touchedWord = "";
            _lastTouchedRow = -1;
            _drawTouch->clear();
            if (!_touchVector.empty())
            {
                _drawSuccess->drawSegment(startTouch, endTouch, LineRadius, LineColorSuccess);
            }
            _touchVector.clear();
            GamePlayPointsManager::getInstance()->addSuccessPoints();
            _words.at(i)->setFound(true);
            SoundManager::getInstance()->successPlay();
            checkGameStatus();
            return true;
        } else {
            if (!_touchVector.empty())
            {
                _drawTouch->drawSegment(startTouch, endTouch, LineRadius, LineColorTouch);
            }
        }
    }
    return false;
}

vector<string> GamePlay::prepareWord(LetterCoords* letterCoords)
{
    string defaultValue = "0";
    vector<string> preparedWord;
    if (letterCoords->getWord().length() == _currentTableSize)
    {
        letterCoords->setStartColumn(0);
        int lenght = (int) letterCoords->getWord().length() - 1;
        letterCoords->setEndColumn(lenght);
        for (int i = 0; i < _currentTableSize; i++)
        {
            preparedWord.push_back(&letterCoords->getWord().at(i));
        }
    }
    else if (letterCoords->getWord().length() > 0 && letterCoords->getWord().length() < _currentTableSize)
    {
        int extraCharCount = _currentTableSize - (int) letterCoords->getWord().length();
        int randomStartPosition = roll((int) 0, (int) extraCharCount);
        
        letterCoords->setStartColumn(randomStartPosition);
        letterCoords->setEndColumn(randomStartPosition + (letterCoords->getWord().length() - 1));
        
        int j = 0;
        for (int i = 0; i < _currentTableSize; i++)
        {
            if (i >= randomStartPosition && j < letterCoords->getWord().length())
            {
                string letter = letterCoords->getWord().substr(j, 1);
                preparedWord.push_back(letter);
                j++;
            }
            else
            {
                preparedWord.push_back(_letters.at(roll(0, 26)));
            }
        }
    }
    else
    {
        for (int i = 0; i < _currentTableSize; i++)
        {
            preparedWord.push_back(_letters.at(roll(0, 26)));
        }
    }
    return preparedWord;
}

//GamePlayDelegate
void GamePlay::loadNextGamePlay()
{
    // TODO: Load not used GamePlay
    _drawError->clear();
    _drawSuccess->clear();
    _drawTouch->clear();
    
    _map.clear();
    _words.clear();
    
    MainCategory mainCategory = *_categories.at(GamePlayPointsManager::getInstance()->getCurrentCategory());
    
    vector<SubCategory*> subCategories = mainCategory.getSubCategories();
    vector<Option*> options = subCategories.at(roll(0, subCategories.size() - 1 ))->getOptions();
    
    list<int> indexWithWord;
    while (indexWithWord.size() < options.size()) {
        indexWithWord.push_back(roll(0, _currentTableSize - 1));
        indexWithWord.unique();
    }
    
    int innerIndex = 0;
    for (int i = 0; i < _currentTableSize; i ++) {
        if (find(indexWithWord.begin(), indexWithWord.end(), i) != indexWithWord.end() && innerIndex < options.size()) {
            LetterCoords *letterCoords = new LetterCoords(options.at(innerIndex)->getName());
            letterCoords->setFound(false);
            _words.push_back(letterCoords);
            innerIndex++;
        }
        else
        {
            _words.push_back(new LetterCoords(""));
        }
    }
    
    for (int i = 0; i < _currentTableSize; i++)
    {
        if (i < _words.size())
        {
            _words.at(i)->setRow(i);
            _map.push_back(prepareWord(_words.at(i)));
        }
        else
        {
            _map.push_back(prepareWord(new LetterCoords()));
        }
    }
    prepareTable();
    
    _elapsedTime = 0;
    
    // Reduce 2 seconds on success
    float timeDifficulty =_totalSuccessScreens * TimeToReduce.asFloat();
    _currentLevelTime = _maxTime;
    _currentLevelTime -= timeDifficulty;
    
    float minValue = (_maxTime * TimeMinPercentage.asInt() / 100);
    _currentLevelTime = (minValue > _currentLevelTime ? minValue : _currentLevelTime);
    this->resetTimer();
    
}

EventListenerTouchOneByOne *GamePlay::addEvents()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    
    listener->onTouchBegan = [&](cocos2d::Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            return false;
        }
        return GamePlay::touchBegan(touch, event);
    };
    
    listener->onTouchMoved = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _drawTouch->clear();
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchMoved(touch, event);
    };
    
    listener->onTouchEnded = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _drawTouch->clear();
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchEnded(touch, event);
    };
    
    listener->onTouchCancelled = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _drawTouch->clear();
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchCancelled(touch, event);
    };
    
    return listener;
}

bool GamePlay::touchBegan(Touch *touch, Event *pEvent)
{
    if (_touchState != TouchState::FINISH) {
        return false;
    }
    _touchState = TouchState::START;
    _drawTouch->clear();
    _drawError->clear();
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    _lastTouch = touchLocation;
    _initialTouch = touchLocation;
    _touchVector.insert(0, touch);
    return true;
}

void GamePlay::touchEnded(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    _lastTouch = touchLocation;
    _endTouch = touchLocation;
    //Check valid touch
    _drawTouch->clear();
    _touchState = TouchState::FINISH;
}

void GamePlay::touchMoved(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    _touchState = TouchState::MOVING;
    Vec2 newTouch = _touchLayer->convertTouchToNodeSpace(touch);
    //    Vec2 oldTouch = touch->getPreviousLocationInView();
    
    _drawTouch->drawSegment(_lastTouch, newTouch, LineRadius, LineColorTouch);
    _lastTouch = newTouch;
}

void GamePlay::touchCancelled(Touch *pTouch, Event *pEvent)
{
    _drawTouch->clear();
    _touchState = TouchState::FINISH;
}

void GamePlay::resumeGamePlay()
{
    this->resetTimer();
}

void GamePlay::restartGame()
{
    GamePlayPointsManager::getInstance()->resetCurrentPoints();
    this->loadNextGamePlay();
}

void GamePlay::timeElapsed()
{
    _elapsedTime = 0;
    _progress->stopAction(_sequence);
    SceneManager::getInstance()->runSceneWithType(SceneType::LOSE);
}

void GamePlay::updateScreenGameStats()
{
    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
    std::string levelString = "";
    switch (difficulty) {
        case Difficulty::EASY:
            levelString = LanguageManager::getLocalizedText("Level", "easy");
            break;
        case Difficulty::MEDIUM:
            levelString = LanguageManager::getLocalizedText("Level", "medium");
            break;
        case Difficulty::HARD:
            levelString = LanguageManager::getLocalizedText("Level", "hard");
            break;
        default:
            break;
    }
    _labelLevel->setString(levelString);
    
    float points = GamePlayPointsManager::getInstance()->getCurrentPoints();
    std::ostringstream pointsString;
    pointsString << points;
    _labelPoints->setString(pointsString.str());
    
    MainCategory mainCategory = *_categories.at(GamePlayPointsManager::getInstance()->getCurrentCategory());
    _labelCategory->setString(mainCategory.getName());
    
}

void GamePlay::checkGameStatus()
{
    bool isDone = true;
    for (int x = 0; x < _words.size(); x++) {
        if (_words.at(x)->getWord().length() > 0 && _words.at(x)->getFound() == false) {
            isDone = false;
        }
    }
    
    if (isDone) {
        _pauseButton->setVisible(false);
        _elapsedTime += _sequence->getElapsed();
        _progress->stopAction(_sequence);
        
        //All image success
        DelayTime *delayAction = DelayTime::create(SuccessTime.asFloat());
        CallFunc *mySelector = CallFunc::create(CC_CALLBACK_0(GamePlay::openSuccessScreen, this));
        this->runAction(Sequence::create(delayAction, mySelector, nullptr));
    }
}

void GamePlay::openSuccessScreen()
{
    _totalSuccessScreens++;
    GamePlayPointsManager::getInstance()->addSuccessPoints();
    SceneManager::getInstance()->runSceneWithType(SceneType::WIN);
    _pauseButton->setVisible(true);
}

int GamePlay::roll(int min, int max)
{
    return ( (arc4random() % (max-min+1)) + min );
}
