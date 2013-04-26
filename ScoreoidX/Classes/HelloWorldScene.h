#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Scoreoid.h"

class HelloWorld : public cocos2d::CCLayer, public ScoreoidDelegate
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    
    Scoreoid* scoreoid;
    
    // Scoreoid delegates
    void scoreCallback(SOScore* score,SOResult result);
    void scoresCallback(CCArray* scores,SOResult result);
    void playerCallback(SOPlayer* player,SOResult result);
    void playersCallback(CCArray* players,SOResult result);
    void gameCallback(SOGame* game,SOResult result);
    void scoreoidAvailable(SOGame* game,SOResult result);
    void notificationsCallback(CCArray* notifications,SOResult result);
};

#endif // __HELLOWORLD_SCENE_H__
