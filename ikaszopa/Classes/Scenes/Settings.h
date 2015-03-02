//
//  Settings.h
//  IkasGame
//
//  Created by Mikel Eizagirre on 2014-12-24.
//
//

#ifndef __IkasCocosGame__Settings__
#define __IkasCocosGame__Settings__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

using namespace cocos2d;

class Settings : public Layer
{
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(Settings);
    
protected:
    void displaySettingsMenu();
    void displayLanguageMenu();
    void languageChanged(Ref *sender);
    void buttonPressed(Ref *sender);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
    bool _showingLanguageSettings;
    MenuItemLabel* _backButton;
    
    std::vector<MenuItemLabel*> _languageButtons;
};
#endif /* defined(__IkasCocosGame__Settings__) */
