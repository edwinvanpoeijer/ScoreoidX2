Welcome to the ScoreoidX for Cocos2d-x wiki!

ScoreoidX is a wrapper for using Scoreoid leaderboards in your project.
The nice thing about Scoreoid is that the interface is using http/https calls and returns xml or json files.
ScoreoidX uses rapidjson (included) to process the json results.
With this api you can also perform a user login, if the user is unknown you can (optionally) automatically create a new one .
There is no need to install any additional sdks for Scoreoid.

Take a look at their website for more information and to register.

[www.scoreoid.net](www.scoreoid.net)

[www.scoreoid.com](www.scoreoid.net)

**INSTALLATION:**

Copy the following folders to the classes folder of your project:
**ScoreoidX**
**rapidjson**

Include the **Scoreoid.h** file in the class where you want to use it, extend this class with ScoreoidDelegate.
For example:

#include "Scoreoid.h"
#include "CrossPlatform.h"

class HelloWorld : public cocos2d::CCLayer, public ScoreoidDelegate
{
};

Add the callbacks to the header file:

    // Scoreoid delegates

    // Returns a single score
    virtual void scoreCallback(SOScore* score,SOResult result) {}
    // Returns an array of scores. !!The getPlayerScores will also callback to this callback!!
    virtual void scoresCallback(CCArray* scores,SOResult result) {}
    
    // Returns player data
    virtual void playerCallback(SOPlayer* player,SOResult result){}
    // Returns an array of players
    virtual void playersCallback(CCArray* players,SOResult result){}
    
    // Returns game information
    virtual void gameCallback(SOGame* game,SOResult result) {}

    // Returns all notifications for this game
    virtual void notificationsCallback(CCArray* notifications,SOResult result) {}
    
    // Will be called after init, check result to see if the init was successful 
    virtual void scoreoidAvailable(SOGame* game,SOResult result) {}

And implement them in the implementation file, see HelloWorldScene.cpp for the examples.
Initialize the scoreoid singleton:

    /*
     * Scoreoid
     */
    
    // Set the delegate 
    Scoreoid::GetInstance()->setDelegate(this);
    // Init scoreoid with your GAME_ID and API_KEY
    Scoreoid::GetInstance()->initScoreoid("INSERT GAME_ID", "API_KEY");

ScoreoidX will try to get the game data and will perform a callback to scoreoidAvailable with the game data.
You should check the result in the SOResult struct to see if the init was successful.

Use the following to do a user login to Scoreoid:
    
    // Login to Scoreoid using an unique identifier
    Scoreoid::GetInstance()->login(USER_ID, false);

This new version of ScoreoidX returns objects related to the API request:

SOScore : A score object
SONotification : notifications
SOPlayer : player info
SOGame : game info
When the API returns a single field the correct field will be filled in the returned object.
If the API returns a unknown field or value this will be put in the SOResult struct.

If the API returns multiple results (players, scores) , the return object will be a CCArray with relevant objects (an array of SOScore, SOPlayer).
