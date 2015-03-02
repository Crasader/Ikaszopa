#ifndef __GAMEPLAY_SCENE_H__
#define __GAMEPLAY_SCENE_H__

#include "../GameModels/LetterCoords.h"
#include "../Constants/CommonProtocols.h"
#include "../Constants/Constants.h"
#include "../CustomGUI/SpriteButton.h"
#include "cocos2d.h"
#include "../GameModels/MainCategory.h"
#include "../GameModels/SubCategory.h"
#include "../GameModels/Option.h"
#include "../Singletons/ZipManager.h"

using namespace cocos2d;
using namespace std;

enum class TouchState
{
    START,
    MOVING,
    FINISH
};

class GamePlay : public Layer, public GamePlayDelegate
{
    Layer *layerPause, *layerWin, *layerLose;
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(GamePlay);
    
    EventListenerTouchOneByOne *addEvents();
    
    virtual bool touchBegan(Touch *touch, Event *pEvent);
    virtual void touchEnded(Touch *touch, Event *pEvent);
    virtual void touchMoved(Touch *touch, Event *pEvent);
    virtual void touchCancelled(Touch *pTouch, Event *pEvent);
    
    //GamePlayDelegate
    virtual void loadNextGamePlay();
    virtual void resumeGamePlay();
    virtual void restartGame();
    virtual void updateScreenGameStats();
    // Time control
    void timeElapsed();
private:
    vector<MainCategory*> _categories;
    void setupLevelSettings();
    void resetTimer();
    void pauseGame(Ref* sender);
    
    Label *_labelCategory, *_labelPoints, *_labelLevel;
    
    Layer *_gameLayer, *_touchLayer, *_hudLayer, *_menuLayer;
    DrawNode *_drawTouch, *_drawSuccess, *_drawError;
    TouchState _touchState;
    Vec2 _initialTouch, _endTouch;
    
    Vec2 _lastTouch;
    Vector<Sprite*> _images;
    Vector<Label*> _titles;
    Vector<Integer*> _indexes;//_indexes[0] = 2; -> pos 0 of _indexes has the _images[0] title position. _images[0] -> _titles[2];
    
    ProgressTimer* _progress;
    ProgressFromTo* _action;
    Sequence* _sequence;
    
    float _elapsedTime, _currentLevelTime, _maxTime;
    int _totalSuccessScreens;
    
    SpriteButton* _pauseButton;
    
    void checkDrawResult(int index, Vec2 imageTouchLocation, Vec2 titleTouchLocation);
    void checkGameStatus();
    void openSuccessScreen();
    
    void clearOnRowChange(int row, int coords);
    bool checkValidWord();
    string charsBetweenColumns(int row, int currentColumn);
    vector<vector<string>> _map;
    vector<LetterCoords*> _words;
    vector<string> _letters { "a" , "b" , "c" , "d" , "e" , "f" , "g" , "h" , "i" , "j" , "k" , "l" , "m" , "n" , "ñ" , "o" , "p" , "q" , "r" , "s" , "t" , "u" , "v" , "w" , "x" , "y" , "z" };
    vector<string> prepareWord(LetterCoords* letterCoords);
    int _currentTableSize;
    int roll(int min, int max);
    string _touchedWord;
    int _lastTouchedRow, _lastTouchedColumn;
    void prepareTable();
    void labelClicked(Ref* sender);
    Vector<Touch*> _touchVector;
    
};

#endif // __GAMEPLAY_SCENE_H__
