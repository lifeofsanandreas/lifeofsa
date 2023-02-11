/*
	Player Functions Include (playerfuncs.inc)
	* Some usefull player functions!

 	Author: (creator)
	* Gammix

	(c) Copyright 2015
  	* This file is provided as is (no warranties).
*/

/*
FUNCTIONS:
native GivePlayerScore(playerid, score);
native SetPlayerMoney(playerid, money);
native RemovePlayerWeapon(playerid, weaponid);
native SetPlayerFacingPoint(playerid, Float:x, Float:y);
native IsPlayerFacingPoint(playerid, Float:x, Float:y, Float:range = 10.0);
native SetPlayerFacingPlayer(playerid, targetid);
native IsPlayerFacingPlayer(playerid, targetid, Float:range =  10.0);
native IsPlayerBehindPlayer(playerid, targetid, Float:range = 10.0);
native GetPlayerWeaponAmmo(playerid, weaponid);
native SetPlayerWeaponAmmo(playerid, weaponid, ammo);
native SetPlayerWalkingStyle(playerid, style);
native GetPlayerWalkingStyle(playerid);
native GetPlayerSkillLevel(playerid, skill);

CALLBACKS:
public OnPlayerWalk(playerid, style)
*/

//walking styles
#define WALKING_STYLE_DEFAULT     0
#define WALKING_STYLE_CIVILIAN    1
#define WALKING_STYLE_GANGSTA     2
#define WALKING_STYLE_GANGSTA_2   3
#define WALKING_STYLE_OLD         4
#define WALKING_STYLE_FAT_OLD     5
#define WALKING_STYLE_FAT         6
#define WALKING_STYLE_LADY        7
#define WALKING_STYLE_LADY_2      8
#define WALKING_STYLE_WHORE       9
#define WALKING_STYLE_WHORE_2     10
#define WALKING_STYLE_DRUNK       11
#define WALKING_STYLE_BLIND       12
#define WALKING_STYLE_ARMED       13

#if ! defined HOLDING
	#define HOLDING(%0) \
		((newkeys & (%0)) == (%0))
#endif

#if ! defined RELEASED
	#define RELEASED(%0) \
		(((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))
#endif

static gWalkingStyle[MAX_PLAYERS];
static gPlayerSkills[MAX_PLAYERS][11];

stock GivePlayerScore(playerid, score) return SetPlayerScore(playerid, GetPlayerScore(playerid) + score);

stock SetPlayerMoney(playerid, money)
{
	ResetPlayerMoney(playerid);
	return GivePlayerMoney(playerid, money);
}

stock RemovePlayerWeapon(playerid, weaponid)
{
	new weapon, ammo;
	for(new i; i != 13; i++)
	{
		GetPlayerWeaponData(playerid, i, weapon, ammo);
		if(weapon == weaponid)
		{
			return GivePlayerWeapon(playerid, weaponid, -ammo);
		}
	}
	return false
}

stock SetPlayerFacingPoint(playerid, Float:x, Float:y)
{
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

	new Float:angle;

	if( y > pos[1] ) angle = (-acos((x - pos[0]) / floatsqroot((x - pos[0])*(x - pos[0]) + (y - pos[1])*(y - pos[1]))) - 90.0);
	else if( y < pos[1] && x < pos[0] ) angle = (acos((x - pos[0]) / floatsqroot((x - pos[0])*(x - pos[0]) + (y - pos[1])*(y - pos[1]))) - 450.0);
	else if( y < pos[1] ) angle = (acos((x - pos[0]) / floatsqroot((x - pos[0])*(x - pos[0]) + (y - pos[1])*(y - pos[1]))) - 90.0);

	if(x > pos[0]) angle = (floatabs(floatabs(angle) + 180.0));
	else angle = (floatabs(angle) - 180.0);

	return SetPlayerFacingAngle(playerid, angle);
}

stock static IsAngleInRangeOfAngle(Float:a1, Float:a2, Float:range = 10.0)
{
	a1 -= a2;
	if((a1 < range) && (a1 > -range)) return true;
	return false;
}

stock IsPlayerFacingPoint(playerid, Float:x, Float:y, Float:range = 10.0)
{
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

	new Float:facing;
	GetPlayerFacingAngle(playerid, facing);

	new Float:angle;

	if( pos[1] > y ) angle = (-acos((pos[0] - x) / floatsqroot((pos[0] - x)*(pos[0] - x) + (pos[1] - y)*(pos[1] - y))) - 90.0);
	else if( pos[1] < y && pos[0] < x ) angle = (acos((pos[0] - x) / floatsqroot((pos[0] - x)*(pos[0] - x) + (pos[1] - y)*(pos[1] - y))) - 450.0);
	else if( pos[1] < y ) angle = (acos((pos[0] - x) / floatsqroot((pos[0] - x)*(pos[0] - x) + (pos[1] - y)*(pos[1] - y))) - 90.0);

	return (IsAngleInRangeOfAngle(-angle, facing, range));
}

stock SetPlayerFacingPlayer(playerid, targetid)
{
	new Float:pos[3];
	GetPlayerPos(targetid, pos[0], pos[1], pos[2]);

	return SetPlayerFacingAngle(playerid, pos[0], pos[1]);
}

stock IsPlayerFacingPlayer(playerid, targetid, Float:range =  10.0)
{
	new Float:pos[3];
	GetPlayerPos(targetid, pos[0], pos[1], pos[2]);

	return IsPlayerFacingPoint(playerid, pos[0], pos[1], range);
}

stock IsPlayerBehindPlayer(playerid, targetid, Float:range = 10.0)
{
	new Float:angle[2];
	GetPlayerFacingAngle(playerid, angle[0]);
	GetPlayerFacingAngle(targetid, angle[1]);

	return (IsAngleInRangeOfAngle(angle[0], angle[1], range) && IsPlayerFacingPlayer(playerid, targetid, range));
}

stock GetPlayerWeaponAmmo(playerid, weaponid)
{
	new current_weapon = GetPlayerWeapon(playerid);

	SetPlayerArmedWeapon(playerid, weaponid);
	new ammo = GetPlayerAmmo(playerid);

	SetPlayerArmedWeapon(playerid, current_weapon);
	return ammo;
}

stock SetPlayerWeaponAmmo(playerid, weaponid, ammo) return SetPlayerAmmo(playerid, weaponid, ammo);

stock SetPlayerWalkingStyle(playerid, style)
{
	gWalkingStyle[playerid] = style;
	return true;
}

stock GetPlayerWalkingStyle(playerid) return gWalkingStyle[playerid];

stock FUNC_SetPlayerSkillLevel(playerid, skill, level)
{
    gPlayerSkills[playerid][skill] = level;
    return SetPlayerSkillLevel(playerid, skill, level);
}
#if defined _ALS_SetPlayerSkillLevel
	#undef SetPlayerSkillLevel
#else
	#define _ALS_SetPlayerSkillLevel
#endif
#define SetPlayerSkillLevel FUNC_SetPlayerSkillLevel

stock GetPlayerSkillLevel(playerid, skill) return gPlayerSkills[playerid][skill];

public OnPlayerConnect(playerid)
{
    gWalkingStyle[playerid] = WALKING_STYLE_DEFAULT;
    for(new i; i < 11; i++) gPlayerSkills[playerid][i] = 0;
    return CallLocalFunction("FUNC_OnPlayerConnect", "i", playerid);
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect FUNC_OnPlayerConnect
forward FUNC_OnPlayerConnect(playerid);

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	new keys, updown, leftright;
	GetPlayerKeys(playerid, keys, updown, leftright);

	if(	HOLDING(KEY_WALK) &&
		(	(updown & KEY_UP ||
			updown & KEY_DOWN)
		||
			(leftright & KEY_LEFT ||
			leftright & KEY_RIGHT))
		)
	{
	    if(GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
		{
		   	if(GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
			{
			   	if(gWalkingStyle[playerid] != WALKING_STYLE_DEFAULT)
				{
					new stylestr[28];
	    			switch(gWalkingStyle[playerid])
					{
						case WALKING_STYLE_CIVILIAN: 	stylestr = "WALK_civi";
						case WALKING_STYLE_GANGSTA: 	stylestr = "WALK_gang1";
						case WALKING_STYLE_GANGSTA_2: 	stylestr = "WALK_gang2";
						case WALKING_STYLE_OLD: 		stylestr = "WALK_old";
						case WALKING_STYLE_FAT_OLD: 	stylestr = "WALK_fatold";
						case WALKING_STYLE_FAT: 		stylestr = "WALK_fat";
						case WALKING_STYLE_LADY: 		stylestr = "WOMAN_walknorm";
						case WALKING_STYLE_LADY_2: 		stylestr = "WOMAN_walkbusy";
						case WALKING_STYLE_WHORE: 		stylestr = "WOMAN_walkpro";
						case WALKING_STYLE_WHORE_2:		stylestr = "WOMAN_walksexy";
						case WALKING_STYLE_DRUNK: 		stylestr = "WALK_drunk";
						case WALKING_STYLE_BLIND: 		stylestr = "Walk_Wuzi";
						case WALKING_STYLE_ARMED: 		stylestr = "WALK_armed";
					}

					if(CallLocalFunction("OnPlayerWalk", "ii", playerid, gWalkingStyle[playerid]))
					{
						return ApplyAnimation(playerid, "PED", stylestr, 4.1, 1, 1, 1, 1, 1);
					}
				}
			}
		}
	}
	if(RELEASED(KEY_WALK))
	{
	    if(GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
		{
		   	if(GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
			{
			   	if(gWalkingStyle[playerid] != WALKING_STYLE_DEFAULT)
				{
					new stylestr[28];
	    			switch(gWalkingStyle[playerid])
					{
						case WALKING_STYLE_CIVILIAN: 	stylestr = "WALK_civi";
						case WALKING_STYLE_GANGSTA: 	stylestr = "WALK_gang1";
						case WALKING_STYLE_GANGSTA_2: 	stylestr = "WALK_gang2";
						case WALKING_STYLE_OLD: 		stylestr = "WALK_old";
						case WALKING_STYLE_FAT_OLD: 	stylestr = "WALK_fatold";
						case WALKING_STYLE_FAT: 		stylestr = "WALK_fat";
						case WALKING_STYLE_LADY: 		stylestr = "WOMAN_walknorm";
						case WALKING_STYLE_LADY_2: 		stylestr = "WOMAN_walkbusy";
						case WALKING_STYLE_WHORE: 		stylestr = "WOMAN_walkpro";
						case WALKING_STYLE_WHORE_2:		stylestr = "WOMAN_walksexy";
						case WALKING_STYLE_DRUNK: 		stylestr = "WALK_drunk";
						case WALKING_STYLE_BLIND: 		stylestr = "Walk_Wuzi";
						case WALKING_STYLE_ARMED: 		stylestr = "WALK_armed";
					}

					if(CallLocalFunction("OnPlayerWalk", "ii", playerid, gWalkingStyle[playerid]))
					{
						return ApplyAnimation(playerid, "PED", stylestr, 4.1, 0, 0, 0, 0, 1);
					}
				}
			}
		}
	}
	return CallLocalFunction("FUNC_OnPlayerKeyStateChange", "iii", playerid, newkeys, oldkeys);
}
#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange FUNC_OnPlayerKeyStateChange
forward FUNC_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);

//script(not internal) callback
forward OnPlayerWalk(playerid, style);