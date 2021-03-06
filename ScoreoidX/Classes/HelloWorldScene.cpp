#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // position the label on the center of the screen
    pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition( ccp(size.width/2, size.height/2) );

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    
    this->scoreoid = Scoreoid::GetInstance();
    
    scoreoid->setDelegate(this);
    scoreoid->initScoreoid("INSERT GAME_ID", "API_KEY");
    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::notificationsCallback(CCArray* notifications,SOResult resultStruct)
{
    CCLog("NOTIFICATIONS CALLBACK");
    CCLog("API call:%s with APIcode:%d",resultStruct.apiCall.c_str(),resultStruct.apiCallCode);
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
    else
    {
        CCLog("%d results received",notifications->count());
    }
 }

void HelloWorld::scoreoidAvailable(SOGame* game,SOResult resultStruct)
{
    CCLog("SCOREOID AVAILABLE");
    CCLog("API call:%s",resultStruct.apiCall.c_str());
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
    else
    {
        CCLog("APICALL result:%d",scoreoid->login("dezebestaatniet1","first_name","last_name",true));
    }
}


void HelloWorld::scoreCallback(SOScore* score,SOResult resultStruct)
{
    CCLog("API call:%s",resultStruct.apiCall.c_str());
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
}

void HelloWorld::scoresCallback(CCArray* scores,SOResult resultStruct)
{
    CCLog("SCORES CALLBACK");
    CCLog("API call:%s",resultStruct.apiCall.c_str());
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
    else
    {
        CCLog("%d results received",scores->count());
    }
}

void HelloWorld::playerCallback(SOPlayer* player,SOResult resultStruct)
{
    CCLOG("PLAYER CALLBACK!!!");
    CCLog("API call:%s",resultStruct.apiCall.c_str());
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
    else if (resultStruct.apiCallCode == SO_LOGIN)
    {
        CCLog("USER LOGGED IN!");
    }
}

void HelloWorld::playersCallback(CCArray* players,SOResult resultStruct)
{
    CCLog("API call:%s",resultStruct.apiCall.c_str());
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
    else
    {
        CCLog("%d results received",players->count());
    }
}

void HelloWorld::gameCallback(SOGame* game,SOResult resultStruct)
{
    CCLog("API call:%s",resultStruct.apiCall.c_str());
    CCLog("Result:%s  Description:%s",resultStruct.field.c_str(),resultStruct.value.c_str());
    if (resultStruct.result == SO_API_FAIL || resultStruct.result == SO_API_ERROR)
    {
        CCLog("Error executing API call");
    }
}

