//
//  Notification.h
//  ScoreoidX
//
//  Created by EAA van Poeijer on 20-04-13.
//
//

#ifndef __ScoreoidX__ScoreoidObjects__
#define __ScoreoidX__ScoreoidObjects__

#include "cocos2d.h"

using namespace cocos2d;

struct SOResult
{
    int result;
    std::string apiCall;
    int apiCallCode;
    std::string field;
    std::string value;
    int getIntValue() { return std::atoi(value.c_str());};
    int getDoubleValue() { return std::atof(value.c_str());};
    bool getBoolValue() { return std::atoi(value.c_str());};
};

class  SONotification : public CCObject
{
private:
    std::string _title;
    std::string _content;
    std::string _start_date;
    std::string _end_date;
    bool _status;
    bool _debug;
public:
    SONotification() : _title(""), _content(""),_start_date(""),_end_date(""),_status(false),_debug(false){}
    std::string getTile() { return _title; };
    std::string getContent() { return _content; };
    std::string getStart_Date() { return _start_date; };
    std::string getEnd_Date() { return _end_date; };
    bool getStatus() { return _status; };
    bool getDebug() { return _debug; };
    
    void setTile(const char* value) { this->_title = value; };
    void setContent(const char* value) { this->_content = value; };
    void setStart_Date(const char* value) { this->_start_date = value; };
    void setEnd_Date(const char* value) { this->_end_date = value; };
    void setStatus(bool value) { this->_status = value; };
    void setDebug(bool value) { this->_debug = value; };
    
    
    
};

class SOScore : public CCObject
{
private:
    std::string _userName;
    std::string _email;
    std::string _first_Name;
    std::string _last_Name;
    std::string _platform;
    std::string _scoreId;
    std::string _playerId;
    std::string _data;
    std::string _game_Id;
    int _score;
    int _difficulty;
    std::string _leaderBoard;
    std::string _created;
    
public:
    SOScore() : _userName(""),_email(""),_first_Name(""),_last_Name(""),_platform(""),_scoreId(""),_playerId(""),_data(""),_game_Id(""),_score(-1),_difficulty(-1),_leaderBoard(""),_created(""){}
    std::string getuserName() { return this->_userName;};
    std::string getemail() { return this->_email;};
    std::string getfirst_Name() { return this->_first_Name;};
    std::string getlast_Name() { return this->_last_Name;};
    std::string getplatform() { return this->_platform;};
    std::string getscoreId() { return this->_scoreId;};
    std::string getplayerId() { return this->_playerId;};
    std::string getdata() { return this->_data;};
    std::string getgame_Id() { return this->_game_Id;};
    int getscore() { return this->_score;};
    int getdifficulty() { return this->_difficulty;};
    std::string getleaderBoard() { return this->_leaderBoard;};
    std::string getcreated() { return this->_created;};
    
    void setuserName (const char* value) { this->_userName= value;};
    void setemail (const char* value) { this->_email= value;};
    void setfirst_Name (const char* value) { this->_first_Name= value;};
    void setlast_Name (const char* value) { this->_last_Name= value;};
    void setplatform (const char* value) { this->_platform= value;};
    void setscoreId (const char* value) { this->_scoreId= value;};
    void setplayerId (const char* value) { this->_playerId= value;};
    void setdata (const char* value) { this->_data= value;};
    void setgame_Id (const char* value) { this->_game_Id= value;};
    void setscore (int value) { this->_score= value;};
    void setdifficulty (int value) { this->_difficulty= value;};
    void setleaderBoard (const char* value) { this->_leaderBoard= value;};
    void setcreated (const char* value) { this->_created= value;};
};

class SOPlayer : public CCObject
{
private:
    std::string _username;
    std::string _unique_id;
    std::string _first_name;
    std::string _last_name;
    std::string _email;
    int _bonus;
    std::string _achievements;
    int _gold;
    int _money;
    int _kills;
    int _lives;
    int _time_played;
    std::string _unlocked_levels;
    std::string _unlocked_items;
    std::string _inventory;
    std::string _last_level;
    std::string _current_level;
    int _current_time;
    int _current_bonus;
    int _current_kills;
    std::string _current_achievements;
    int _current_gold;
    int _current_unlocked_levels;
    std::string _current_unlocked_items;
    int _current_lives;
    std::string _xp;
    std::string _energy;
    std::string _boost;
    std::string _latitude;
    std::string _longtitude;
    std::string _game_state;
    std::string _platform;
    int _rank;
    int _best_score;
    std::string _created;
    std::string _updated;

public:
    SOPlayer() :_username(""),_unique_id(""),_first_name(""),_last_name(""),_email(""),_bonus(-1),_achievements(""),_gold(-1),_money(-1),_kills(-1),_lives(-1),_time_played(-1),_unlocked_levels(""),_unlocked_items(""),_inventory(""),_last_level(""),_current_level(""),_current_time(-1),_current_bonus(-1),_current_kills(-1),_current_achievements(""),_current_gold(-1),_current_unlocked_levels(-1),_current_unlocked_items(""),_current_lives(-1),_xp(""),_energy(""),_boost(""),_latitude(""),_longtitude(""),_game_state(""),_platform(""),_rank(-1),_best_score(-1),_created(""),_updated(""){}
    std::string getusername() { return this->_username;};
    std::string getunique_id() { return this->_unique_id;};
    std::string getfirst_name() { return this->_first_name;};
    std::string getlast_name() { return this->_last_name;};
    std::string getemail() { return this->_email;};
    int getbonus() { return this->_bonus;};
    std::string getachievements() { return this->_achievements;};
    int getgold() { return this->_gold;};
    int getmoney() { return this->_money;};
    int getkills() { return this->_kills;};
    int getlives() { return this->_lives;};
    int gettime_played() { return this->_time_played;};
    std::string getunlocked_levels() { return this->_unlocked_levels;};
    std::string getunlocked_items() { return this->_unlocked_items;};
    std::string getinventory() { return this->_inventory;};
    std::string getlast_level() { return this->_last_level;};
    std::string getcurrent_level() { return this->_current_level;};
    int getcurrent_time() { return this->_current_time;};
    int getcurrent_bonus() { return this->_current_bonus;};
    int getcurrent_kills() { return this->_current_kills;};
    std::string getcurrent_achievements() { return this->_current_achievements;};
    int getcurrent_gold() { return this->_current_gold;};
    int getcurrent_unlocked_levels() { return this->_current_unlocked_levels;};
    std::string getcurrent_unlocked_items() { return this->_current_unlocked_items;};
    int getcurrent_lives() { return this->_current_lives;};
    std::string getxp() { return this->_xp;};
    std::string getenergy() { return this->_energy;};
    std::string getboost() { return this->_boost;};
    std::string getlatitude() { return this->_latitude;};
    std::string getlongtitude() { return this->_longtitude;};
    std::string getgame_state() { return this->_game_state;};
    std::string getplatform() { return this->_platform;};
    int getrank() { return this->_rank;};
    int getbest_score() { return this->_best_score;};
    std::string getcreated() { return this->_created;};
    std::string getupdated() { return this->_updated;};
    
    void setusername (const char* value) { this->_username= value;};
    void setunique_id (const char* value) { this->_unique_id= value;};
    void setfirst_name (const char* value) { this->_first_name= value;};
    void setlast_name (const char* value) { this->_last_name= value;};
    void setemail (const char* value) { this->_email= value;};
    void setbonus (int value) { this->_bonus= value;};
    void setachievements (const char* value) { this->_achievements= value;};
    void setgold (int value) { this->_gold= value;};
    void setmoney (int value) { this->_money= value;};
    void setkills (int value) { this->_kills= value;};
    void setlives (int value) { this->_lives= value;};
    void settime_played (int value) { this->_time_played= value;};
    void setunlocked_levels (const char* value) { this->_unlocked_levels= value;};
    void setunlocked_items (const char* value) { this->_unlocked_items= value;};
    void setinventory (const char* value) { this->_inventory= value;};
    void setlast_level (const char* value) { this->_last_level= value;};
    void setcurrent_level (const char* value) { this->_current_level= value;};
    void setcurrent_time (int value) { this->_current_time= value;};
    void setcurrent_bonus (int value) { this->_current_bonus= value;};
    void setcurrent_kills (int value) { this->_current_kills= value;};
    void setcurrent_achievements (const char* value) { this->_current_achievements= value;};
    void setcurrent_gold (int value) { this->_current_gold= value;};
    void setcurrent_unlocked_levels (int value) { this->_current_unlocked_levels= value;};
    void setcurrent_unlocked_items (const char* value) { this->_current_unlocked_items= value;};
    void setcurrent_lives (int value) { this->_current_lives= value;};
    void setxp (const char* value) { this->_xp= value;};
    void setenergy (const char* value) { this->_energy= value;};
    void setboost (const char* value) { this->_boost= value;};
    void setlatitude (const char* value) { this->_latitude= value;};
    void setlongtitude (const char* value) { this->_longtitude= value;};
    void setgame_state (const char* value) { this->_game_state= value;};
    void setplatform (const char* value) { this->_platform= value;};
    void setrank (int value) { this->_rank= value;};
    void setbest_score (int value) { this->_best_score= value;};
    void setcreated (const char* value) { this->_created= value;};
    void setupdated (const char* value) { this->_updated= value;};
};

class SOGame : public CCObject
{
private:
    std::string _user_id;
    std::string _name;
    std::string _short_description;
    std::string _description;
    std::string _game_type;
    std::string _version;
    int _levels;
    std::string _platform;
    std::string _play_url;
    std::string _website_url;
    int _players_count;
    int _scores_count;
    int _status;
    std::string _created;
    std::string _updated;


public:
SOGame() :_user_id(""),_name(""),_short_description(""),_description(""),_game_type(""),_version(""),_levels(-1),_platform(""),_play_url(""),_website_url(""),_players_count(-1),_scores_count(-1),_status(-1),_created(""),_updated("") {}
    
    std::string getuser_id() { return this->_user_id;};
    std::string getname() { return this->_name;};
    std::string getshort_description() { return this->_short_description;};
    std::string getdescription() { return this->_description;};
    std::string getgame_type() { return this->_game_type;};
    std::string getversion() { return this->_version;};
    int getlevels() { return this->_levels;};
    std::string getplatform() { return this->_platform;};
    std::string getplay_url() { return this->_play_url;};
    std::string getwebsite_url() { return this->_website_url;};
    int getplayers_count() { return this->_players_count;};
    int getscores_count() { return this->_scores_count;};
    int getstatus() { return this->_status;};
    std::string getcreated() { return this->_created;};
    std::string getupdated() { return this->_updated;};
    
    void setuser_id (const char* value) { this->_user_id= value;};
    void setname (const char* value) { this->_name= value;};
    void setshort_description (const char* value) { this->_short_description= value;};
    void setdescription (const char* value) { this->_description= value;};
    void setgame_type (const char* value) { this->_game_type= value;};
    void setversion (const char* value) { this->_version= value;};
    void setlevels (int value) { this->_levels= value;};
    void setplatform (const char* value) { this->_platform= value;};
    void setplay_url (const char* value) { this->_play_url= value;};
    void setwebsite_url (const char* value) { this->_website_url= value;};
    void setplayers_count (int value) { this->_players_count= value;};
    void setscores_count (int value) { this->_scores_count= value;};
    void setstatus (int value) { this->_status= value;};
    void setcreated (const char* value) { this->_created= value;};
    void setupdated (const char* value) { this->_updated= value;};
    
};
#endif /* defined(__ScoreoidX__ScoreoidObjects__) */
