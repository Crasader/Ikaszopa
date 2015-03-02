//
//  Constants.h
//  IkasGame
//
//  Created by Sergio Garcia on 30/01/15.
//
//

#ifndef __IkasCocosGame__Constants
#define __IkasCocosGame__Constants

#include "cocos2d.h"
using namespace cocos2d;

/**
 * Custom callbacks
 */
typedef std::function<void(Ref*)> IkasHandler;
typedef std::function<void(Ref*,bool)> IkasResultHandler;

/**
 * URL´s
 */
static std::string ApiURL = "http://ikastek.net/ikastek-klear/apps-data/ikaszopa";
static std::string InfoURL = "http://ikastek.net/aplikazioak/umeentzako/ikazopa/";

/**
 * Main fonts names
 */
static std::string MainRegularFont = "fonts/Oxygen-Regular.ttf";
static std::string MainBoldFont = "fonts/Oxygen-Bold.ttf";
static std::string MainLightFont = "fonts/Oxygen-Light.ttf";

/**
 * Sound settings filename
 */
static std::string SoundEnableImage = "sound-on";
static std::string SoundDisableImage = "sound-off";

/**
 * App colors
 */
//static cocos2d::Color4B IkasPink = cocos2d::Color4B(231, 58, 82, 255);
//static cocos2d::Color4B IkasRed = cocos2d::Color4B(190, 22, 34, 255);

static cocos2d::Color4B IkasGrayLight = cocos2d::Color4B(173, 173, 173, 255);
static cocos2d::Color4B IkasGrayDark = cocos2d::Color4B(87, 87, 87, 255);

//static cocos2d::Color4B IkasPinkAlpha = cocos2d::Color4B(231, 58, 82, 130);
static cocos2d::Color4B IkasBlue = cocos2d::Color4B(26, 96, 132, 255);
static cocos2d::Color4B IkasGreen = cocos2d::Color4B(145, 165, 16, 255);

/**
 * Different scenes used in common games
 */
enum class SceneType
{
	NONE,
    SPLASH,
    SETTINGS,
    ABOUT,
	MAIN,
    CATEGORIES,
    LEVEL,
    GAMEPLAY,
    PAUSE,
    WIN,
    LOSE
};

/**
 * Game difficuty options
 */
enum class Difficulty
{
    EMPTY = -1,
    EASY,
    MEDIUM,
    HARD
};

/**
 * Different checkBox types used in Game Settings Scene
 */
enum class CheckBoxType
{
    NONE,
    MUSIC,
    SFX
};

/**
 * Different checkBox types used in Game Settings Scene
 */
enum class CustomLanguage
{
    EUSKARA = 0,
    SPANISH
};

#endif
