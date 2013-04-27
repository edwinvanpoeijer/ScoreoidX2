/****************************************************************************
Copyright (c) 2013      Edwin van Poeijer

http://www.vanpoeijer.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "Scoreoid.h"

USING_NS_CC;
USING_NS_CC_EXT;


#define SCOREOID_RESPONSE "json"

Scoreoid* Scoreoid::instance = NULL;


Scoreoid::~Scoreoid()
{
}

Scoreoid* Scoreoid::GetInstance()
{
    if ( instance == NULL ) {
        instance = Scoreoid::create();
        instance->retain();
    }
    
    return instance;
}

Scoreoid::Scoreoid()
{
    this->m_delegate = NULL;
    this->_scoreoidAvailable = false;
    this->_actionRunning = false;
    this->_currentApiCall = SO_INIT;
    this->_gameID = "";
    this->_ApiKey = "";
    this->_localUserLoggedIn = false;
    this->_createUser = false;
    
}

void Scoreoid::initScoreoid(const char *gameID, const char *apiKey)
{
    this->_scoreoidAvailable = false;
    this->_actionRunning = false;
    this->_gameID = gameID;
    this->_ApiKey = apiKey;
    this->_apiCallString = CCString::createWithFormat("api_key=%s&game_id=%s&respsone=json",this->_ApiKey.c_str(),this->_gameID.c_str())->getCString();
    // Call getGame to init
    this->_currentApiCall = SO_INIT;
    this->getGame();
}

/*
 * Remove empty fields from the API string
 */
const char* Scoreoid::removeEmptyFields(const char* value1, std::string delimiter)
{
    std::string value = value1;
    CCString* newString = CCString::create("");
    int currentPosition = 0;
    unsigned foundPosition = 0;
    
    while(foundPosition!=std::string::npos)
    {
        foundPosition = value.find(delimiter,currentPosition);
        std::string substring = value.substr (currentPosition,(foundPosition-currentPosition));
        char lastChar = substring[substring.length()-1];
        if (lastChar != '=')
        {
            CCString* original = newString;
            if (currentPosition ==0)
            {
                newString = CCString::createWithFormat("%s",substring.c_str());
                //CCLog("FIELD:%s",newString->getCString());
            }
            else
            {
                newString = CCString::createWithFormat("%s&%s",original->getCString(),substring.c_str());
                //CCLog("FIELD:%s",newString->getCString());
            }
        }
        currentPosition = foundPosition+1;
    }
    return newString->getCString();
}

Scoreoid * Scoreoid::create(void)
{
	Scoreoid * pRet = new Scoreoid();
	pRet->autorelease();
	return pRet;
}

std::string Scoreoid::getStringValue(const Value& member, const char* field)
{
    std::string returnValue = "";
    if (member.HasMember(field) && member[field].IsString())
    {
            returnValue = member[field].GetString();
    }
    return returnValue;
}

std::string Scoreoid::getStringValue(Value::ConstMemberIterator itr,const char* field)
{
    std::string value = "";
    if (itr->value.IsString())
    {
        value = itr->value.GetString();
    }
    else if (itr->value.IsBool())
    {
        if (itr->value.GetBool())
        {
            value = "1";
        }
        else
        {
            value = "0";
        }
    }
    else if (itr->value.IsDouble())
    {
        value = CCString::createWithFormat("%f",itr->value.GetDouble())->getCString();
    }
    else if (itr->value.IsInt())
    {
        value = CCString::createWithFormat("%d",itr->value.GetInt())->getCString();
    }
    else if (itr->value.IsInt64())
    {
        value = CCString::createWithFormat("%d",itr->value.GetInt64())->getCString();
    }

    return value;
}
bool Scoreoid::getBoolValue(const Value& member, const char* field)
{
    bool returnValue = false;
    if (member.HasMember(field) && member[field].IsBool())
    {
        returnValue = member[field].GetBool();
    }
    return returnValue;
}

int Scoreoid::getIntValue(const Value& member, const char* field)
{
    int returnValue = -1;
    if (member.HasMember(field) && member[field].IsInt())
    {
        returnValue = member[field].GetInt();
    }
    else if (member.HasMember(field) && member[field].IsString())
    {
        returnValue = atoi(member[field].GetString());
    }
    return returnValue;
}

double Scoreoid::getDoubleValue(const Value& member, const char* field)
{
    double returnValue = -1.0f;
    if (member.HasMember(field) && member[field].IsDouble())
    {
        returnValue = member[field].GetDouble();
    }
    else if (member.HasMember(field) && member[field].IsString())
    {
        returnValue = atof(member[field].GetString());
    }
    return returnValue;
}



CCString* Scoreoid::getStringJSON(Value::Member* iterator, const char* field)
{
    if (iterator->value.HasMember(field))
        if (iterator->value[field].IsString())
            return CCString::create(iterator->value[field].GetString());
    
    return CCString::create("");
}

bool Scoreoid::HttpRequest(const char* apiUrl,const char* data, const char* tag,SEL_CallFuncND pSelector)
{
    bool returnValue = false;
    if ( (this->_scoreoidAvailable && !this->_actionRunning) || (this->_currentApiCall == SO_INIT) )
    {
        this->_actionRunning = true;
        returnValue = true;
        CCHttpRequest* request = new CCHttpRequest();
    
        request->setUrl(apiUrl);
        request->setRequestType(CCHttpRequest::kHttpPost);
        std::vector<std::string> headers;
        headers.push_back("application/x-www-form-urlencoded; charset=utf-8");
        request->setHeaders(headers);
        request->setResponseCallback(this, pSelector);
    
        // write the post data
        std::string endData = this->removeEmptyFields(data, "&");
        std::string scoreoid_postdat = this->_apiCallString;
        std::string postData;
        if (strlen(endData.c_str()) > 0)
        {
            postData = scoreoid_postdat + "&" + endData;
        }
        else
        {
            postData = scoreoid_postdat;
        }
        CCLog("Posting:%s",postData.c_str());
        request->setRequestData(postData.c_str(), strlen(postData.c_str()));
        request->setTag(tag);
        CCHttpClient::getInstance()->send(request);
        request->release();
    }
    
    return returnValue;
}

/*
 * Handle scores this includes the getPlayerScores
 */
void Scoreoid::HttpRequestScoresCallback(cocos2d::CCNode *sender, void *data)
{
    CCHttpResponse *response = (CCHttpResponse*)data;
    SOResult resultStruct;
    resultStruct.result = SO_API_FAIL;
    resultStruct.apiCall = response->getHttpRequest()->getTag();
    
    CCArray* results = CCArray::create();
    
    if (response)
    {
        resultStruct.result = SO_API_SUCCES;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    CCLog(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed() || resultStruct.result == SO_API_FAIL)
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        resultStruct.result = SO_API_FAIL;
    }
    else
    {
        // dump data and convert to string
        std::vector<char> *buffer = response->getResponseData();
        std::string s(buffer->begin(), buffer->end());
        CCLog("%s",s.c_str());
        resultStruct.result = SO_API_SUCCES;
        
        // Check if the data was correct
        Document document;
        if(document.Parse<0>(s.c_str()).HasParseError() || (document.IsObject() && document.HasMember("error")))
        {
            resultStruct.result = SO_API_ERROR;
            resultStruct.field = "error";
            resultStruct.value = document["error"].GetString() ;
        }
        else if (document.IsArray())
        {
            for (SizeType i = 0; i < document.Size(); i++)
            {
                SOScore* score = new SOScore();
                score->autorelease();
                bool scoresFound = false;
                if (document[i].HasMember("Player"))
                {
                    scoresFound = true;
                    const Value& member = document[i]["Player"];
                    
                    score->setuserName(getStringValue(member, "username").c_str());
                    score->setemail(getStringValue(member, "email").c_str());
                    score->setfirst_Name(getStringValue(member, "first_name").c_str());
                    score->setlast_Name(getStringValue(member, "last_name").c_str());
                    score->setplatform(getStringValue(member, "platform").c_str());
                    score->setscoreId(getStringValue(member, "id").c_str());
                    score->setplayerId(getStringValue(member, "player_id").c_str());
                    score->setdata(getStringValue(member, "data").c_str());
                    score->setgame_Id(getStringValue(member, "game_id").c_str());
                    
                }
                if (document[i].HasMember("Score"))
                {
                    scoresFound = true;
                    const Value& member = document[i]["Score"];
 
                    score->setscore(getIntValue(member, "score"));
                    score->setdifficulty(getIntValue(member, "difficulty"));
                    score->setplatform(getStringValue(member, "platform").c_str());
                    score->setleaderBoard(getStringValue(member, "leaderboard").c_str());
                    score->setcreated(getStringValue(member, "created").c_str());
                    
                }
                if (scoresFound)
                {
                    results->addObject(score);
                }
            }
            resultStruct.field = "success";
            resultStruct.value = "" ;
            
        }
        else
        {
            resultStruct.field = "error";
            resultStruct.value = "" ;
            resultStruct.result = SO_API_ERROR;
        }
    }
    
    resultStruct.apiCallCode = this->_currentApiCall;
    this->_currentApiCall = SO_NOTHING;
    this->_actionRunning = false;
    // Callback
    if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
        return delegate->scoresCallback(results, resultStruct);
    }
    
}

/*
 * Handle scores this includes the getPlayerScores
 */
void Scoreoid::HttpRequestScoreCallback(cocos2d::CCNode *sender, void *data)
{
    CCHttpResponse *response = (CCHttpResponse*)data;
    SOResult resultStruct;
    resultStruct.result = SO_API_FAIL;
    resultStruct.apiCall = response->getHttpRequest()->getTag();
    
    SOScore* results = new SOScore();
    results->autorelease();
    
    if (response)
    {
        resultStruct.result = SO_API_SUCCES;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    CCLog(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed() || resultStruct.result == SO_API_FAIL)
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        resultStruct.result = SO_API_FAIL;
    }
    else
    {
        // dump data and convert to string
        std::vector<char> *buffer = response->getResponseData();
        std::string s(buffer->begin(), buffer->end());
        CCLog("%s",s.c_str());
        resultStruct.result = SO_API_SUCCES;
        
        // Check if the data was correct
        Document document;
        if(document.Parse<0>(s.c_str()).HasParseError() || (document.IsObject() && document.HasMember("error")))
        {
            resultStruct.result = SO_API_ERROR;
            resultStruct.field = "error";
            resultStruct.value = document["error"].GetString() ;
        }
        else if (document.IsArray())
        {
            for (SizeType i = 0; i < document.Size(); i++)
            {
                SOScore* score = new SOScore();
                score->autorelease();
                if (document[i].HasMember("Player"))
                {
                    const Value& member = document[i]["Player"];
                    
                    results->setuserName(getStringValue(member, "username").c_str());
                    results->setemail(getStringValue(member, "email").c_str());
                    results->setfirst_Name(getStringValue(member, "first_name").c_str());
                    results->setlast_Name(getStringValue(member, "last_name").c_str());
                    results->setplatform(getStringValue(member, "platform").c_str());
                    results->setscoreId(getStringValue(member, "id").c_str());
                    results->setplayerId(getStringValue(member, "player_id").c_str());
                    results->setdata(getStringValue(member, "data").c_str());
                    results->setgame_Id(getStringValue(member, "game_id").c_str());
                    
                }
                if (document[i].HasMember("Score"))
                {
                    const Value& member = document[i]["Score"];
                    
                    results->setscore(getIntValue(member, "score"));
                    results->setdifficulty(getIntValue(member, "difficulty"));
                    results->setplatform(getStringValue(member, "platform").c_str());
                    results->setleaderBoard(getStringValue(member, "leaderboard").c_str());
                    results->setcreated(getStringValue(member, "created").c_str());
                    
                }
            }
            resultStruct.field = "success";
            resultStruct.value = "" ;
            
        }
        else if (document.IsObject())
        {
            const Value& member = document;
            results->setuserName(getStringValue(member, "username").c_str());
            results->setemail(getStringValue(member, "email").c_str());
            results->setfirst_Name(getStringValue(member, "first_name").c_str());
            results->setlast_Name(getStringValue(member, "last_name").c_str());
            results->setplatform(getStringValue(member, "platform").c_str());
            results->setscoreId(getStringValue(member, "id").c_str());
            results->setplayerId(getStringValue(member, "player_id").c_str());
            results->setdata(getStringValue(member, "data").c_str());
            results->setgame_Id(getStringValue(member, "game_id").c_str());
            results->setscore(getIntValue(member, "score"));
            results->setdifficulty(getIntValue(member, "difficulty"));
            results->setplatform(getStringValue(member, "platform").c_str());
            results->setleaderBoard(getStringValue(member, "leaderboard").c_str());
            results->setcreated(getStringValue(member, "created").c_str());

            Value::ConstMemberIterator itr = document.MemberBegin();
            resultStruct.field = itr->name.GetString();
            resultStruct.value = this->getStringValue(itr, itr->name.GetString());

        }
        else
        {
            resultStruct.field = "error";
            resultStruct.value = "" ;
            resultStruct.result = SO_API_ERROR;
        }
    }
    resultStruct.apiCallCode = this->_currentApiCall;
    this->_currentApiCall = SO_NOTHING;
    this->_actionRunning = false;
    
    // Callback
    if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
        return delegate->scoreCallback(results, resultStruct);
    }
    
}

void Scoreoid::HttpRequestPlayerCallback(cocos2d::CCNode *sender, void *data)
{
    CCHttpResponse *response = (CCHttpResponse*)data;
    SOResult resultStruct;
    resultStruct.result = SO_API_FAIL;
    resultStruct.apiCall = response->getHttpRequest()->getTag();
    
    SOPlayer* results = new SOPlayer();
    results->autorelease();
    
    if (response)
    {
        resultStruct.result = SO_API_SUCCES;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    CCLog(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed() || resultStruct.result == SO_API_FAIL)
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        resultStruct.result = SO_API_FAIL;
    }
    else
    {
        // dump data and convert to string
        std::vector<char> *buffer = response->getResponseData();
        std::string s(buffer->begin(), buffer->end());
        CCLog("%s",s.c_str());
        resultStruct.result = SO_API_SUCCES;
        
        // Check if the data was correct
        Document document;
        if(document.Parse<0>(s.c_str()).HasParseError() || (document.IsObject() && document.HasMember("error")))
        {
            resultStruct.result = SO_API_ERROR;
            resultStruct.field = "error";
            resultStruct.value = document["error"].GetString() ;
        }
        else if (document.IsArray())
        {
            for (SizeType i = 0; i < document.Size(); i++)
            {
                if (document[i].HasMember("Player"))
                {
                    const Value& member = document[i]["Player"];
                    
                    results->setusername(getStringValue(member, "username").c_str());
                    results->setunique_id(getStringValue(member, "unique_id").c_str());
                    results->setfirst_name(getStringValue(member, "first_name").c_str());
                    results->setlast_name(getStringValue(member, "last_name").c_str());
                    results->setemail(getStringValue(member, "email").c_str());
                    results->setbonus(getIntValue(member, "bonus"));
                    results->setachievements(getStringValue(member, "achievements").c_str());
                    results->setgold(getIntValue(member, "gold"));
                    results->setmoney(getIntValue(member, "money"));
                    results->setkills(getIntValue(member, "kills"));
                    results->setlives(getIntValue(member, "lives"));
                    results->settime_played(getIntValue(member, "time_played"));
                    results->setunlocked_levels(getStringValue(member, "unlocked_levels").c_str());
                    results->setunlocked_items(getStringValue(member, "unlocked_items").c_str());
                    results->setinventory(getStringValue(member, "inventory").c_str());
                    results->setlast_level(getStringValue(member, "last_level").c_str());
                    results->setcurrent_level(getStringValue(member, "current_level").c_str());
                    results->setcurrent_time(getIntValue(member, "current_time"));
                    results->setcurrent_bonus(getIntValue(member, "current_bonus"));
                    results->setcurrent_kills(getIntValue(member, "current_kills"));
                    results->setcurrent_achievements(getStringValue(member, "current_achievements").c_str());
                    results->setcurrent_gold(getIntValue(member, "current_gold"));
                    results->setcurrent_unlocked_levels(getIntValue(member, "current_unlocked_levels"));
                    results->setcurrent_unlocked_items(getStringValue(member, "current_unlocked_items").c_str());
                    results->setcurrent_lives(getIntValue(member, "current_lives"));
                    results->setxp(getStringValue(member, "xp").c_str());
                    results->setenergy(getStringValue(member, "energy").c_str());
                    results->setboost(getStringValue(member, "boost").c_str());
                    results->setlatitude(getStringValue(member, "latitude").c_str());
                    results->setlongtitude(getStringValue(member, "longtitude").c_str());
                    results->setgame_state(getStringValue(member, "game_state").c_str());
                    results->setplatform(getStringValue(member, "platform").c_str());
                    results->setrank(getIntValue(member, "rank"));
                    results->setbest_score(getIntValue(member, "best_score"));
                    results->setcreated(getStringValue(member, "created").c_str());
                    results->setupdated(getStringValue(member, "updated").c_str());
                    
                }
            }
            resultStruct.field = "success";
            resultStruct.value = "" ;
            
        }
        else if (document.IsObject())
        {
            const Value& member = document;
            
            results->setusername(getStringValue(member, "username").c_str());
            results->setunique_id(getStringValue(member, "unique_id").c_str());
            results->setfirst_name(getStringValue(member, "first_name").c_str());
            results->setlast_name(getStringValue(member, "last_name").c_str());
            results->setemail(getStringValue(member, "email").c_str());
            results->setbonus(getIntValue(member, "bonus"));
            results->setachievements(getStringValue(member, "achievements").c_str());
            results->setgold(getIntValue(member, "gold"));
            results->setmoney(getIntValue(member, "money"));
            results->setkills(getIntValue(member, "kills"));
            results->setlives(getIntValue(member, "lives"));
            results->settime_played(getIntValue(member, "time_played"));
            results->setunlocked_levels(getStringValue(member, "unlocked_levels").c_str());
            results->setunlocked_items(getStringValue(member, "unlocked_items").c_str());
            results->setinventory(getStringValue(member, "inventory").c_str());
            results->setlast_level(getStringValue(member, "last_level").c_str());
            results->setcurrent_level(getStringValue(member, "current_level").c_str());
            results->setcurrent_time(getIntValue(member, "current_time"));
            results->setcurrent_bonus(getIntValue(member, "current_bonus"));
            results->setcurrent_kills(getIntValue(member, "current_kills"));
            results->setcurrent_achievements(getStringValue(member, "current_achievements").c_str());
            results->setcurrent_gold(getIntValue(member, "current_gold"));
            results->setcurrent_unlocked_levels(getIntValue(member, "current_unlocked_levels"));
            results->setcurrent_unlocked_items(getStringValue(member, "current_unlocked_items").c_str());
            results->setcurrent_lives(getIntValue(member, "current_lives"));
            results->setxp(getStringValue(member, "xp").c_str());
            results->setenergy(getStringValue(member, "energy").c_str());
            results->setboost(getStringValue(member, "boost").c_str());
            results->setlatitude(getStringValue(member, "latitude").c_str());
            results->setlongtitude(getStringValue(member, "longtitude").c_str());
            results->setgame_state(getStringValue(member, "game_state").c_str());
            results->setplatform(getStringValue(member, "platform").c_str());
            results->setrank(getIntValue(member, "rank"));
            results->setbest_score(getIntValue(member, "best_score"));
            results->setcreated(getStringValue(member, "created").c_str());
            results->setupdated(getStringValue(member, "updated").c_str());

            Value::ConstMemberIterator itr = document.MemberBegin();
            resultStruct.field = itr->name.GetString();
            resultStruct.value = this->getStringValue(itr, itr->name.GetString());
            
        }
        else
        {
            resultStruct.field = "error";
            resultStruct.value = "" ;
            resultStruct.result = SO_API_ERROR;
        }
    }
    resultStruct.apiCallCode = this->_currentApiCall;
    this->_currentApiCall = SO_NOTHING;
    this->_actionRunning = false;
    if (resultStruct.apiCallCode == SO_LOGIN)
    {
        this->loginPlayerHandler(results, resultStruct);
    }
    else if (resultStruct.apiCallCode == SO_LOGIN_CREATEUSER)
    {
        this->loginCreatePlayerHandler(results, resultStruct);
    }
    else
    {
    // Callback
    if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
        return delegate->playerCallback(results, resultStruct);
    }
    }
    
}

/*
 * Request alle players
 */
void Scoreoid::HttpRequestPlayersCallback(cocos2d::CCNode *sender, void *data)
{
    CCHttpResponse *response = (CCHttpResponse*)data;
    SOResult resultStruct;
    resultStruct.result = SO_API_FAIL;
    resultStruct.apiCall = response->getHttpRequest()->getTag();
    
    CCArray* results = CCArray::create();
    
    if (response)
    {
        resultStruct.result = SO_API_SUCCES;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    CCLog(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed() || resultStruct.result == SO_API_FAIL)
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        resultStruct.result = SO_API_FAIL;
    }
    else
    {
        // dump data and convert to string
        std::vector<char> *buffer = response->getResponseData();
        std::string s(buffer->begin(), buffer->end());
        CCLog("%s",s.c_str());
        resultStruct.result = SO_API_SUCCES;
        
        // Check if the data was correct
        Document document;
        if(document.Parse<0>(s.c_str()).HasParseError() || (document.IsObject() && document.HasMember("error")))
        {
            resultStruct.result = SO_API_ERROR;
            resultStruct.field = "error";
            resultStruct.value = document["error"].GetString() ;
        }
        else if (document.IsArray())
        {
            for (SizeType i = 0; i < document.Size(); i++)
            {
                if (document[i].HasMember("Player"))
                {
                    const Value& member = document[i]["Player"];
                    SOPlayer* player = new SOPlayer();
                    player->autorelease();
                    
                    player->setusername(getStringValue(member, "username").c_str());
                    player->setunique_id(getStringValue(member, "unique_id").c_str());
                    player->setfirst_name(getStringValue(member, "first_name").c_str());
                    player->setlast_name(getStringValue(member, "last_name").c_str());
                    player->setemail(getStringValue(member, "email").c_str());
                    player->setbonus(getIntValue(member, "bonus"));
                    player->setachievements(getStringValue(member, "achievements").c_str());
                    player->setgold(getIntValue(member, "gold"));
                    player->setmoney(getIntValue(member, "money"));
                    player->setkills(getIntValue(member, "kills"));
                    player->setlives(getIntValue(member, "lives"));
                    player->settime_played(getIntValue(member, "time_played"));
                    player->setunlocked_levels(getStringValue(member, "unlocked_levels").c_str());
                    player->setunlocked_items(getStringValue(member, "unlocked_items").c_str());
                    player->setinventory(getStringValue(member, "inventory").c_str());
                    player->setlast_level(getStringValue(member, "last_level").c_str());
                    player->setcurrent_level(getStringValue(member, "current_level").c_str());
                    player->setcurrent_time(getIntValue(member, "current_time"));
                    player->setcurrent_bonus(getIntValue(member, "current_bonus"));
                    player->setcurrent_kills(getIntValue(member, "current_kills"));
                    player->setcurrent_achievements(getStringValue(member, "current_achievements").c_str());
                    player->setcurrent_gold(getIntValue(member, "current_gold"));
                    player->setcurrent_unlocked_levels(getIntValue(member, "current_unlocked_levels"));
                    player->setcurrent_unlocked_items(getStringValue(member, "current_unlocked_items").c_str());
                    player->setcurrent_lives(getIntValue(member, "current_lives"));
                    player->setxp(getStringValue(member, "xp").c_str());
                    player->setenergy(getStringValue(member, "energy").c_str());
                    player->setboost(getStringValue(member, "boost").c_str());
                    player->setlatitude(getStringValue(member, "latitude").c_str());
                    player->setlongtitude(getStringValue(member, "longtitude").c_str());
                    player->setgame_state(getStringValue(member, "game_state").c_str());
                    player->setplatform(getStringValue(member, "platform").c_str());
                    player->setrank(getIntValue(member, "rank"));
                    player->setbest_score(getIntValue(member, "best_score"));
                    player->setcreated(getStringValue(member, "created").c_str());
                    player->setupdated(getStringValue(member, "updated").c_str());
                    
                    results->addObject(player);
                }
            }
            resultStruct.field = "success";
            resultStruct.value = "" ;
            
        }
        else
        {
            resultStruct.field = "error";
            resultStruct.value = "" ;
            resultStruct.result = SO_API_ERROR;
        }
    }
    resultStruct.apiCallCode = this->_currentApiCall;
    this->_currentApiCall = SO_NOTHING;
    this->_actionRunning = false;
    
    // Callback
    if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
        return delegate->playersCallback(results, resultStruct);
    }
    
}

/*
 * Handles all game related callbacks
 */
void Scoreoid::HttpRequestGameCallback(cocos2d::CCNode *sender, void *data)
{
    CCHttpResponse *response = (CCHttpResponse*)data;
    SOResult resultStruct;
    resultStruct.result = SO_API_FAIL;
    resultStruct.apiCall = response->getHttpRequest()->getTag();

    SOGame* results = new SOGame();
    results->autorelease();
    
    if (response)
    {
        resultStruct.result = SO_API_SUCCES;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    CCLog(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed() || resultStruct.result == SO_API_FAIL)
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        resultStruct.result = SO_API_FAIL;
    }
    else
    {
        // dump data and convert to string
        std::vector<char> *buffer = response->getResponseData();
        std::string s(buffer->begin(), buffer->end());
        CCLog("%s",s.c_str());
        resultStruct.result = SO_API_SUCCES;
        
        // Check if the data was correct
        Document document;
        if(document.Parse<0>(s.c_str()).HasParseError() || (document.IsObject() && document.HasMember("error")))
        {
            resultStruct.result = SO_API_ERROR;
            resultStruct.field = "error";
            resultStruct.value = document["error"].GetString() ;
        }
        else if (document.IsArray())
        {
                    for (SizeType i = 0; i < document.Size(); i++)
                    {
                        if (document[i].HasMember("Game"))
                        {
                            const Value& member = document[i]["Game"];
                            
                            results->setuser_id(getStringValue(member, "user_id").c_str());
                            results->setname(getStringValue(member, "name").c_str());
                            results->setshort_description(getStringValue(member, "short_description").c_str());
                            results->setdescription(getStringValue(member, "description").c_str());
                            results->setgame_type(getStringValue(member, "game_type").c_str());
                            results->setversion(getStringValue(member, "version").c_str());
                            results->setlevels(getIntValue(member, "levels"));
                            results->setplatform(getStringValue(member, "platform").c_str());
                            results->setplay_url(getStringValue(member, "play_url").c_str());
                            results->setwebsite_url(getStringValue(member, "website_url").c_str());
                            results->setplayers_count(getIntValue(member, "players_count"));
                            results->setscores_count(getIntValue(member, "scores_count"));
                            results->setstatus(getIntValue(member, "status"));
                            results->setcreated(getStringValue(member, "created").c_str());
                            results->setupdated(getStringValue(member, "updated").c_str());
                            
                        }
                    }
            resultStruct.field = "success";
            resultStruct.value = "" ;
            
        }
        else if (document.IsObject())
        {
            const Value& member = document;
            
            results->setuser_id(getStringValue(member, "user_id").c_str());
            results->setname(getStringValue(member, "name").c_str());
            results->setshort_description(getStringValue(member, "short_description").c_str());
            results->setdescription(getStringValue(member, "description").c_str());
            results->setgame_type(getStringValue(member, "game_type").c_str());
            results->setversion(getStringValue(member, "version").c_str());
            results->setlevels(getIntValue(member, "levels"));
            results->setplatform(getStringValue(member, "platform").c_str());
            results->setplay_url(getStringValue(member, "play_url").c_str());
            results->setwebsite_url(getStringValue(member, "website_url").c_str());
            results->setplayers_count(getIntValue(member, "players_count"));
            results->setscores_count(getIntValue(member, "scores_count"));
            results->setstatus(getIntValue(member, "status"));
            results->setcreated(getStringValue(member, "created").c_str());
            results->setupdated(getStringValue(member, "updated").c_str());

            Value::ConstMemberIterator itr = document.MemberBegin();
            resultStruct.field = itr->name.GetString();
            resultStruct.value = this->getStringValue(itr, itr->name.GetString());
        }
        else
        {
            resultStruct.field = "error";
            resultStruct.value = "" ;
            resultStruct.result = SO_API_ERROR;
        }
    }
    resultStruct.apiCallCode = this->_currentApiCall;
    this->_currentApiCall = SO_NOTHING;
    this->_actionRunning = false;
    
    if(!this->_scoreoidAvailable && resultStruct.apiCallCode == SO_INIT)
    {
        this->_scoreoidAvailable = true;
        // Callback
        if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
            return delegate->scoreoidAvailable(results, resultStruct);
        }
    }
    else
    {
        // Callback
        if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
        return delegate->gameCallback(results, resultStruct);
        }
    }
}

/*
 * Handle notifications
 */
void Scoreoid::HttpRequestNotificationsCallback(cocos2d::CCNode *sender, void *data)
{
    SOResult resultStruct;
    int result = SO_API_FAIL;
    CCHttpResponse *response = (CCHttpResponse*)data;
    CCArray* results = CCArray::create();
    
    if (response)
    {        
        result = SO_API_SUCCES;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    CCLog(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed() || result == SO_API_FAIL)
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        result = SO_API_FAIL;
    }
    else
    {
        // dump data and convert to string
        std::vector<char> *buffer = response->getResponseData();
        std::string s(buffer->begin(), buffer->end());
        
    result = SO_API_SUCCES;

    
    Document document;
    
    // Check if the data was correct
    if(document.Parse<0>(s.c_str()).HasParseError())
    {
        result = SO_API_ERROR;
        resultStruct.field = "error";
        resultStruct.value = document["error"].GetString() ;
    }
    else if (document.IsObject() && document.HasMember("error"))
    {
        result = SO_API_ERROR;
        resultStruct.field = "error";
        resultStruct.value = document["error"].GetString() ;
    }
    else if (document.IsObject() && document.HasMember("notifications"))
    {
        if (document["notifications"].HasMember("game_notification"))
        {            
            if (document["notifications"]["game_notification"].IsArray())
            {
                    for (SizeType i = 0; i < document["notifications"]["game_notification"].Size(); i++)
                    {
                        if (document["notifications"]["game_notification"][i].HasMember("GameNotification"))
                        {
                                const Value& member = document["notifications"]["game_notification"][i]["GameNotification"];
                                
                                SONotification* notification = new SONotification();
                                notification->autorelease();
                                notification->setTile(getStringValue(member, "title").c_str());
                                notification->setContent(getStringValue(member, "content").c_str());
                                notification->setStart_Date(getStringValue(member, "start_date").c_str());
                                notification->setEnd_Date(getStringValue(member, "end_date").c_str());
                                notification->setStatus(getBoolValue(member, "status"));
                                notification->setDebug(getBoolValue(member, "debug"));
                        
                                results->addObject(notification);
                        }
                    }
            }
        }
        resultStruct.field = "success";
        resultStruct.value = "" ;
    }
    else
    {
        resultStruct.field = "error";
        resultStruct.value = "" ;
        result = SO_API_ERROR;
    }
    }

    resultStruct.apiCallCode = this->_currentApiCall;
    this->_currentApiCall = SO_NOTHING;
    this->_actionRunning = false;
    
    resultStruct.result = result;
    resultStruct.apiCall = response->getHttpRequest()->getTag();
    // Callback
    if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
        return delegate->notificationsCallback(results, resultStruct);
    }
}


/*
 Gets notifications for this game
 */
bool Scoreoid::getNotification() //method lets you pull your game’s in game notifications.
{
    this->_currentApiCall = SO_GETNOTIFICATION;
    return this->HttpRequest("http://www.scoreoid.com/api/getNotification", "","getNotification",callfuncND_selector(Scoreoid::HttpRequestNotificationsCallback));
}

/*
 gets the total for the following game field’s bonus, gold, money, kills, lifes, time_played and unlocked_levels.
 field => String Value, (bonus, gold, money, kills, lifes, time_played, unlocked_levels) [Required]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 */
bool Scoreoid::getGameTotal(const char* field, const char* start_date, const char* end_date, const char* platform)
{
    this->_currentApiCall = SO_GETGAMETOTAL;
    CCString* result = CCString::createWithFormat("field=%s&start_date=%s&end_date=%s&platform=%s",field,start_date,end_date,platform);
    return this->HttpRequest("http://www.scoreoid.com/api/getGameTotal", result->getCString(),"getGameTotal",callfuncND_selector(Scoreoid::HttpRequestGameCallback));
}

/*
 gets the lowest value for the following game field’s bonus, gold, money, kills, lifes, time_played and unlocked_levels.
 field => String Value, (bonus,best_score, gold, money, kills, lifes, time_played, unlocked_levels) [Required]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 */
bool Scoreoid::getGameLowest(const char* field, const char* start_date, const char* end_date, const char* platform)
{
    this->_currentApiCall = SO_GETGAMELOWEST;
    CCString* result = CCString::createWithFormat("field=%s&start_date=%s&end_date=%s&platform=%s",field,start_date,end_date,platform);
    return this->HttpRequest("http://www.scoreoid.com/api/getGameLowest", result->getCString(),"getGameLowest",callfuncND_selector(Scoreoid::HttpRequestGameCallback));
}

/*
 gets the top value for the following game field’s bonus, gold, money, kills, lifes, time_played and unlocked_levels.
 field => String Value, (bonus, best_score, gold, money, kills, lifes, time_played, unlocked_levels) [Required]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 */
bool Scoreoid::getGameTop(const char* field, const char* start_date, const char* end_date, const char* platform)
{
    this->_currentApiCall = SO_GETGAMETOTAL;
    CCString* result = CCString::createWithFormat("field=%s&start_date=%s&end_date=%s&platform=%s",field,start_date,end_date,platform);
    return this->HttpRequest("http://www.scoreoid.com/api/getGameTop", result->getCString(),"getGameTop",callfuncND_selector(Scoreoid::HttpRequestGameCallback));
}

/*
 gets the average for the following game field’s bonus, gold, money, kills, lifes, time_played and unlocked_levels.
 field => String Value, (bonus, best_score, gold, money, kills, lifes, time_played, unlocked_levels) [Required]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 */
bool Scoreoid::getGameAverage(const char* field, const char* start_date, const char* end_date, const char* platform)
{
    this->_currentApiCall = SO_GETGAMEAVERAGE;
    CCString* result = CCString::createWithFormat("field=%s&start_date=%s&end_date=%s&platform=%s",field,start_date,end_date,platform);
    return this->HttpRequest("http://www.scoreoid.com/api/getGameAverage", result->getCString(),"getGameAverage",callfuncND_selector(Scoreoid::HttpRequestGameCallback));
}

/*
 API method lets you get all the players for a specif game.
 order_by => String Value, (date or score) [Optional]
 order => String Value, asc or desc [Optional]
 limit => Number Value, the limit, "20" retrieves rows 1 - 20 | "10,20" retrieves 20 scores starting from the 10th [Optional]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => String Value, needs to match the string value that was used when creating the player [Optional]
 */
bool Scoreoid::getPlayers(const char* order_by, const char* order, const char* limit, const char* start_date, const char* end_date, const char* platform)
{
    this->_currentApiCall = SO_GETPLAYERS;
    CCString* result = CCString::createWithFormat("order_by=%s&order=%s&limit=%s&start_date=%s&end_date=%s&platform=%s",order_by,order,limit,start_date,end_date,platform);
    return this->HttpRequest("http://www.scoreoid.com/api/getPlayers", result->getCString(),"getPlayers",callfuncND_selector(Scoreoid::HttpRequestPlayersCallback));
}

/*
 method lets you pull a specific field from your game info.
 field => String Value, (name, short_description, description, game_type, version, levels, platform, play_url, website_url,
 created, updated, player_count, scores_count, locked, status) [Required]
 */
bool Scoreoid::getGameField(const char* field)
{
    this->_currentApiCall = SO_GETGAMEFIELD;
    CCString* result = CCString::createWithFormat("field=%s",field);
    return this->HttpRequest("http://www.scoreoid.com/api/getGameField", result->getCString(),"getGameField",callfuncND_selector(Scoreoid::HttpRequestGameCallback));
}

/*
 method lets you pull all your game information.
 */
bool Scoreoid::getGame()
{
    if (this->_currentApiCall != SO_INIT)
    {
        this->_currentApiCall = SO_GETGAME;
    }
    return this->HttpRequest("http://www.scoreoid.com/api/getGame", "","getGame",callfuncND_selector(Scoreoid::HttpRequestGameCallback));
}

/*
 method lets you pull a specific field from your player info.
 username => String Value, the players username [Required]
 field => String Value, the selected field (username, password, unique_id, first_name, last_name, email, platform, created,
 updated, bonus, achievements, best_score, gold, money, kills, lives, time_played, unlocked_levels, unlocked_items, inventory,
 last_level, current_level, current_time, current_bonus, current_kills, current_achievements, current_gold, current_unlocked_levels,
 current_unlocked_items, current_lifes, xp, energy, boost, latitude, longitude, game_state, platform, rank) [Required]
 */
bool Scoreoid::getPlayerField(const char* username,const char* field)
{
    this->_currentApiCall = SO_GETPLAYERFIELD;
    CCString* result = CCString::createWithFormat("username=%s&field=%s",username,field);
    return this->HttpRequest("http://www.scoreoid.com/api/getPlayerField", result->getCString(),"getPlayerField",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method lets you pull all the scores for a player.
 username => String Value, the players username [Required]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]
 */
bool Scoreoid::getPlayerScores(const char* username, const char* start_date, const char* end_date, const char* difficulty)
{
    this->_currentApiCall = SO_GETPLAYERSCORES;
    CCString* result = CCString::createWithFormat("username=%s&start_date=%s&end_date=%s&difficulty=%s",username,start_date,end_date,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/getPlayerScores", result->getCString(),"getPlayerScores",callfuncND_selector(Scoreoid::HttpRequestScoresCallback));
}

/*
 method deletes a player and all the players scores.
 username => String Value, the players username [Required]
 */
bool Scoreoid::deletePlayer(const char* username)
{
    this->_currentApiCall = SO_DELETEPLAYER;
    CCString* result = CCString::createWithFormat("username=%s",username);
    return this->HttpRequest("http://www.scoreoid.com/api/deletePlayer", result->getCString(),"deletePlayer",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method Check if player exists and returns the player information. Post parameters work as query conditions. This method can be used for login by adding username and password parameters.
 id =>  The players ID [String] [Optional]
 username => String Value, the players username [Required]
 password => The players password [String] [Optional]
 email => The players email [String] [Optional]
 */
bool Scoreoid::getPlayer(const char* username, const char* id, const char* password, const char* email)
{
    this->_currentApiCall = SO_GETPLAYER;
    CCString* result = CCString::createWithFormat("username=%s&id=%s&password=%s&email=%s",username,id,password,email);
    return this->HttpRequest("http://www.scoreoid.com/api/getPlayer", result->getCString(),"getPlayer",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method lets you edit your player information.
 username => String Value, the players username [Required]
 fields = string with all fields in format password=[value] separated by &
 password, unique_id, first_name, last_name, email, created, updated, bonus, achievements, gold, money, kills, lives, time_played, unlocked_levels, unlocked_items, inventory, last_level, current_level, current_time, current_bonus, current_kills, current_achievements, current_gold, current_unlocked_levels, current_unlocked_items, current_lifes, xp, energy, boost, latitude, longitude, game_state, platform)
 For description of all the fields www.scoreoid.com
 */
bool Scoreoid::editPlayer(const char* username, const char* fields)
{
    this->_currentApiCall = SO_EDITPLAYER;
    CCString* result = CCString::createWithFormat("username=%s&%s",username,fields);
    return this->HttpRequest("http://www.scoreoid.com/api/editPlayer", result->getCString(),"editPlayer",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method lets you count all your players.
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 */
bool Scoreoid::countPlayers(const char* start_date, const char*end_date, const char* platform)
{
    this->_currentApiCall = SO_COUNTPLAYERS;
    CCString* result = CCString::createWithFormat("start_date=%s&end_date=%s&platform=%s",start_date,end_date,platform);
    return this->HttpRequest("http://www.scoreoid.com/api/countPlayers", result->getCString(),"countPlayers",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method lets you update your player field’s.
 username => String Value, the players username [Required]
 field => String Value, (password, unique_id, first_name, last_name, email, created, updated, bonus, achievements, gold, money, kills, lives, time_played, unlocked_levels, unlocked_items, inventory, last_level, current_level, current_time, current_bonus, current_kills, current_achievements, current_gold, current_unlocked_levels, current_unlocked_items, current_lifes, xp, energy, boost, latitude, longitude, game_state, platform) [Required]
 value => String Value, the field value [Required]
 */
bool Scoreoid::updatePlayerField(const char* username, const char* field, const char* value)
{
    this->_currentApiCall = SO_UPDATEPLAYERFIELD;
    CCString* result = CCString::createWithFormat("username=%s&field=%s&value=%s",username,field,value);
    return this->HttpRequest("http://www.scoreoid.com/api/updatePlayerField", result->getCString(),"updatePlayerField",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method lets you create a player with a number of optional values.
 username => The players username [String] [Required]
 fields = string with all fields in format password=[value] separated by &
 password, unique_id, first_name, last_name, email, created, updated, bonus, achievements, gold, money, kills, lives, time_played, unlocked_levels, unlocked_items, inventory, last_level, current_level, current_time, current_bonus, current_kills, current_achievements, current_gold, current_unlocked_levels, current_unlocked_items, current_lifes, xp, energy, boost, latitude, longitude, game_state, platform)
 */
bool Scoreoid::createPlayer(const char* username, const char* fields)
{
    this->_currentApiCall = SO_CREATEPLAYER;
    CCString* result = CCString::createWithFormat("username=%s&fields",username,fields);
    return this->HttpRequest("http://www.scoreoid.com/api/createPlayer", result->getCString(),"createPlayer",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
}

/*
 method lets you count all your game best scores.
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]
 */
bool Scoreoid::countBestScores(const char* start_date, const char*end_date, const char* platform, const char* difficulty)
{
    this->_currentApiCall =  SO_COUNTBESTSCORES;
    CCString* result = CCString::createWithFormat("start_date=%s&end_date=%s&platform=%s&difficulty=%s",start_date,end_date,platform,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/countBestScores", result->getCString(),"countBestScores",callfuncND_selector(Scoreoid::HttpRequestScoreCallback));
}

/*
 method lets you get all your game average scores.
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]
 */
bool Scoreoid::getAverageScore(const char* start_date, const char*end_date, const char* platform, const char* difficulty)
{
    this->_currentApiCall = SO_GETAVERAGESCORE;
    CCString* result = CCString::createWithFormat("start_date=%s&end_date=%s&platform=%s&difficulty=%s",start_date,end_date,platform,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/getAverageScore", result->getCString(),"getAverageScore",callfuncND_selector(Scoreoid::HttpRequestScoreCallback));
}

/*
 method lets you get all your games best scores.
 order_by => String Value, (date or score) [Optional]
 order => String Value, asc or desc [Optional]
 limit => Number Value, the limit, "20" retrieves rows 1 - 20 | "10,20" retrieves 20 scores starting from the 10th [Optional]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]     */
bool Scoreoid::getBestScores(const char* order_by, const char* order, const char* limit,const char* start_date, const char*end_date, const char* platform, const char* difficulty)
{
    this->_currentApiCall = SO_GETBESTSCORES;
    CCString* result = CCString::createWithFormat("order_by=%s&order=%s&limit=%s&start_date=%s&end_date=%s&platform=%s&difficulty=%s",order_by,order,limit,start_date,end_date,platform,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/getBestScores", result->getCString(),"getBestScores",callfuncND_selector(Scoreoid::HttpRequestScoresCallback));
}

/*
 method lets you count all your game scores.
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]
 */
bool Scoreoid::countScores(const char* start_date, const char*end_date, const char* platform, const char* difficulty)
{
    this->_currentApiCall = SO_COUNTSCORES;
    CCString* result = CCString::createWithFormat("start_date=%s&end_date=%s&platform=%s&difficulty=%s",start_date,end_date,platform,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/countScores", result->getCString(),"countScores",callfuncND_selector(Scoreoid::HttpRequestScoreCallback));
}

/*
 method lets you pull all your game scores.
 order_by => String Value, (date or score) [Optional]
 order => String Value, asc or desc [Optional]
 limit => Number Value, the limit, "20" retrieves rows 1 - 20 | "10,20" retrieves 20 scores starting from the 10th [Optional]
 start_date => String Value, YYY-MM-DD format [Optional]
 end_date => String Value, YYY-MM-DD format [Optional]
 platform => The players platform needs to match the string value that was used when creating the player  [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]
 */
bool Scoreoid::getScores(const char* order_by, const char* order, const char* limit,const char* start_date, const char*end_date, const char* platform, const char* difficulty)
{
    this->_currentApiCall = SO_GETSCORES;
    CCString* result = CCString::createWithFormat("order_by=%s&order=%s&limit=%s&start_date=%s&end_date=%s&platform=%s&difficulty=%s",order_by,order,limit,start_date,end_date,platform,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/getScores", result->getCString(),"getScores",callfuncND_selector(Scoreoid::HttpRequestScoresCallback));
}

/*
 method lets you create a user score.
 username => String Value, if user does not exist it well be created [Required]
 score => Integer Value, [Required]
 platform => String Value, [Optional]
 unique_id => Integer Value, [Optional]
 difficulty => Integer Value, between 1 to 10 (don't use 0 as it's the default value) [Optional]
 */
bool Scoreoid::createScore(const char* username, const char* score, const char* platform, const char* unique_id, const char* difficulty)
{
    this->_currentApiCall = SO_CREATESCORE;
    CCString* result = CCString::createWithFormat("username=%s&score=%s&platform=%s&unique_id=%s&difficulty=%s",username,score,platform,unique_id,difficulty);
    return this->HttpRequest("http://www.scoreoid.com/api/createScore", result->getCString(),"createScore",callfuncND_selector(Scoreoid::HttpRequestScoreCallback));
}

/*
 * Login the player or create a new one
 */
bool Scoreoid::login(const char* playerID,bool shouldCreate)
{
    this->_currentApiCall = SO_LOGIN;
    this->_createUser = shouldCreate;
    this->_localUserLoggedIn = false;
    this->_localUserId = playerID;
    CCString* result = CCString::createWithFormat("username=%s&id=%s&password=%s&email=%s",playerID,"","","");
    return this->HttpRequest("http://www.scoreoid.com/api/getPlayer", result->getCString(),"getPlayer",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));    
}

// LoginPlayerHandler()
void Scoreoid::loginPlayerHandler(SOPlayer* player,SOResult result)
{
    CCLOG("Player login :%d",result.result);
    if (result.value.compare("Player not found") == 0 && this->_createUser)
    {
        CCLOG("Player not found!");
        this->_currentApiCall = SO_LOGIN_CREATEUSER;
        CCString* result = CCString::createWithFormat("username=%s&fields",this->_localUserId.c_str(),"");
        this->HttpRequest("http://www.scoreoid.com/api/createPlayer", result->getCString(),"createPlayer",callfuncND_selector(Scoreoid::HttpRequestPlayerCallback));
        
    }
    else
    {
        if (result.result == SO_API_SUCCES)
        {
            this->_localUserLoggedIn = true;
        }
        this->_localUserLoggedIn = true;
        // Callback
        if (ScoreoidDelegate* delegate = Scoreoid::GetInstance()->getDelegate()) {
            return delegate->playerCallback(player, result);
        }

    }
    
}
void Scoreoid::loginCreatePlayerHandler(SOPlayer* player,SOResult result)
{
    this->login(this->_localUserId.c_str(), false);
}
