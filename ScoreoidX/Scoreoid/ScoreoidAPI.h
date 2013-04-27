//
//  ScoreoidAPI.h
//  ScoreoidX
//
//  Created by EAA van Poeijer on 21-04-13.
//
//

#ifndef ScoreoidX_ScoreoidAPI_h
#define ScoreoidX_ScoreoidAPI_h

#include "cocos2d.h"

using namespace cocos2d;

/*
 * RESULT
 */
#define SO_API_FAIL   -1
#define SO_API_ERROR  0
#define SO_API_SUCCES 1

// Idle
#define SO_NOTHING -1

// Init
#define SO_INIT 0
// Notification
#define SO_GETNOTIFICATION 1

// Game
#define SO_GETGAME 2
#define SO_GETGAMEFIELD 3
#define SO_GETGAMEAVERAGE 4
#define SO_GETGAMETOP 5
#define SO_GETGAMELOWEST 6
#define SO_GETGAMETOTAL 7

// Score
#define SO_CREATESCORE 8
#define SO_COUNTSCORES 9
#define SO_COUNTBESTSCORES 10
#define SO_GETAVERAGESCORE 11

// Scores
#define SO_GETSCORES 12      
#define SO_GETBESTSCORES 13
#define SO_GETPLAYERSCORES 14

// Player
#define SO_CREATEPLAYER 15
#define SO_EDITPLAYER 16
#define SO_DELETEPLAYER 17
#define SO_GETPLAYER 18
#define SO_GETPLAYERFIELD 19
#define SO_UPDATEPLAYERFIELD 20
#define SO_COUNTPLAYERS 21

// Players
#define SO_GETPLAYERS 22

// Login
#define SO_LOGIN 23
#define SO_LOGIN_CREATEUSER 24



#endif
