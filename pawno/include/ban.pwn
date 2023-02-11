/*
	PEDs Include (peds.inc)
	* Since samp 0.3.7 have actors system(which is coolest upgrade ever!), i have planned on peds(tpe of npcs) creator using pawn (not C++)
	* This include let you make AI and cool effects and peds without writing thier AI.
	* Well, this can be useful for making shopkeepers, body guards, dead bodies and lot more...

 	Author: (creator)
	* Gammix

	Contributors:
	* Pottus - ideas and improvement
	* a_angles - took help for making the ped face points

	(c) Copyright 2015
  	* This file is provided as is (no warranties).
*/
/*
FUNCTIONS:
native PED_Connect(skin, Float:x, Float:y, Float:z, Float:rotation = 0.0, Float:health = 100.0, bool:invulnerable = false);
native PED_IsConnected(pedid);
native PED_Disconnect(pedid);
native PED_SetPos(pedid, Float:x, Float:y, Float:z);
native PED_GetPos(pedid, &Float:x, &Float:y, &Float:z);
native PED_SetFacingAngle(pedid, Float:angle);
native PED_GetFacingAngle(pedid, &Float:angle);
native PED_SetVirtualWorld(pedid, world);
native PED_GetVirtualWorld(pedid);
native PED_ToggleInvulnerable(pedid, bool:toggle);
native PED_IsInvulnerable(pedid);
native PED_SetFacingPoint(pedid, Float:x, Float:y);
native PED_IsFacingPoint(pedid, Float:x, Float:y, Float:range = 10.0);
native PED_SetFacingPlayer(pedid, playerid);
native PED_IsFacingPlayer(pedid, playerid, Float:range =  10.0);
native PED_IsBehindPlayer(pedid, playerid, Float:range = 10.0);
native PED_IsInRangeOfPoint(pedid, Float:range, Float:x, Float:y, Float:z);
native PED_SetHealth(pedid, Float:health);
native PED_GetHealth(pedid, &Float:health);
native PED_IsDead(pedid);
native PED_ApplyAnimation(pedid, animlib[], animname[], Float:fDelta = 4.1, loop = 0, lockx = 1, locky = 1, freeze = 1, time = 0);
native PED_Stop(pedid);
native PED_Spawn(pedid);
native PED_SetSpawnInfo(pedid, Float:x, Float:y, Float:z, Float:rotation, Float:health);
native PED_IsStreamedIn(pedid, forplayerid);
native PED_PlaySound(pedid, soundid);
native _
native GetPlayerCameraTargetPED(playerid);
native GetPlayerTargetPED(playerid);

CALLBACKS:
public PED_OnConnect(pedid)
public PED_OnDisconnect(pedid)
public PED_OnSpawn(pedid)
public PED_OnTakeDamage(pedid, issuerid, weaponid, Float:amount, bodypart)
public PED_OnDeath(pedid, killerid, weaponid)
public PED_OnStreamIn(pedid, forplayerid)
public PED_OnStreamOut(pedid, forplayerid)
public PED_OnUpdate(pedid)

public OnPlayerTargetPED(playerid, pedid, weaponid)
public OnPlayerGivePEDDamage(playerid, pedid, weaponid, Float:amount, bodypart)
*/

#if ! defined foreach_player
	#define foreach_player(%1) for(new %1; %1 <= GetPlayerPoolSize(); %1++) if(IsPlayerConnected(%1))
#endif

//just kiding !:D!
#define INVALID_PED_ID  INVALID_ACTOR_ID
#define MAX_PEDS  		MAX_ACTORS

enum PedEnum
{
    P_SKIN,
	Float:P_X,
	Float:P_Y,
	Float:P_Z,
	Float:P_ROT,
	Float:P_HEALTH,
	P_DEATHTIMER
}
static gPED[MAX_PEDS][PedEnum];

//player targeting a ped!
static gPlayerTarget[MAX_PLAYERS];

public OnPlayerConnect(playerid)
{
	gPlayerTarget[playerid] = INVALID_PED_ID;

	#if defined HoOK_OnPlayerConnect
		HoOK_OnPlayerConnect(playerid);
	#endif
	return 1;
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect HoOK_OnPlayerConnect
#if defined HoOK_OnPlayerConnect
	forward HoOK_OnPlayerConnect(playerid);
#endif

//internal functions
stock static Float:PED_GetDistance(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
	return floatsqroot( (( x1 - x2 ) * ( x1 - x2 )) + (( y1 - y2 ) * ( y1 - y2 )) + (( z1 - z2 ) * ( z1 - z2 )) );
}

stock static PED_AngleInRangeOfAngle(Float:a1, Float:a2, Float:range = 10.0)
{
	a1 -= a2;
	if((a1 < range) && (a1 > -range)) return true;
	return false;
}

//include funcions
stock PED_Connect(skin, Float:x, Float:y, Float:z, Float:rotation = 0.0, Float:health = 100.0, bool:invulnerable = false)
{
	if(skin < 0 || skin > 311) return INVALID_PED_ID;

	new _actor = CreateActor(skin, x, y, z, rotation);
	if(_actor == INVALID_PED_ID) return INVALID_PED_ID;

   	gPED[_actor][P_SKIN] = skin;
 	gPED[_actor][P_X] = x;
 	gPED[_actor][P_Y] = y;
  	gPED[_actor][P_Z] = z;
   	gPED[_actor][P_ROT] = rotation;
   	gPED[_actor][P_HEALTH] = health;
   	SetActorHealth(_actor, health);
   	gPED[_actor][P_DEATHTIMER] = -1;
   	SetActorInvulnerable(_actor, invulnerable);

	CallLocalFunction("PED_OnConnect", "i", _actor);
	CallLocalFunction("PED_OnSpawn", "i", _actor);
	return _actor;
}

#define PED_IsConnected IsValidActor//validity checker

stock PED_Disconnect(pedid)
{
	if(! PED_IsConnected(pedid)) return false;

	if(gPED[pedid][P_DEATHTIMER] != -1) KillTimer(gPED[pedid][P_DEATHTIMER]);
	gPED[pedid][P_DEATHTIMER] = -1;

	DestroyActor(pedid);
	return CallLocalFunction("PED_OnDisconnect", "i", pedid);
}

stock PED_SetPos(pedid, Float:x, Float:y, Float:z)
{
	if(! PED_IsConnected(pedid)) return false;

    if(CallLocalFunction("PED_OnUpdate", "i", pedid)) return SetActorPos(pedid, x, y, z);
    return false;
}

stock PED_GetPos(pedid, &Float:x, &Float:y, &Float:z)
{
	if(! PED_IsConnected(pedid)) return false;

	return GetActorPos(pedid, x, y, z);
}

stock PED_SetFacingAngle(pedid, Float:angle)
{
	if(! PED_IsConnected(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid)) return SetActorFacingAngle(pedid, angle);
    return false;
}

stock PED_GetFacingAngle(pedid, &Float:angle)
{
	if(! PED_IsConnected(pedid)) return false;

	return GetActorFacingAngle(pedid, angle);
}

stock PED_SetVirtualWorld(pedid, world)
{
	if(! PED_IsConnected(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid)) return SetActorVirtualWorld(pedid, world);
    return false;
}

stock PED_GetVirtualWorld(pedid)
{
	if(! PED_IsConnected(pedid)) return false;

	return GetActorVirtualWorld(pedid);
}

stock PED_ToggleInvulnerable(pedid, bool:toggle)
{
	if(! PED_IsConnected(pedid)) return false;

    if(CallLocalFunction("PED_OnUpdate", "i", pedid)) return SetActorInvulnerable(pedid, toggle);
    return false;
}

stock PED_IsInvulnerable(pedid)
{
	if(! PED_IsConnected(pedid)) return false;

	return IsActorInvulnerable(pedid);
}

stock PED_SetFacingPoint(pedid, Float:x, Float:y)
{
	if(! PED_IsConnected(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid))
    {
		new Float:pX, Float:pY, Float:pZ;
		PED_GetPos(pedid, pX, pY, pZ);

		new Float:angle;

		if( y > pY ) angle = (-acos((x - pX) / floatsqroot((x - pX)*(x - pX) + (y - pY)*(y - pY))) - 90.0);
		else if( y < pY && x < pX ) angle = (acos((x - pX) / floatsqroot((x - pX)*(x - pX) + (y - pY)*(y - pY))) - 450.0);
		else if( y < pY ) angle = (acos((x - pX) / floatsqroot((x - pX)*(x - pX) + (y - pY)*(y - pY))) - 90.0);

		if(x > pX) angle = (floatabs(floatabs(angle) + 180.0));
		else angle = (floatabs(angle) - 180.0);

	 	return PED_SetFacingAngle(pedid, angle);
	}
	return false;
}

stock PED_IsFacingPoint(pedid, Float:x, Float:y, Float:range = 10.0)
{
	if(! PED_IsConnected(pedid)) return false;

	new Float:X, Float:Y, Float:Z, Float:A;
	PED_GetPos(pedid, X, Y, Z);
	PED_GetFacingAngle(pedid, A);

	new Float:angle;

	if( Y > y ) angle = (-acos((X - x) / floatsqroot((X - x)*(X - x) + (Y - y)*(Y - y))) - 90.0);
	else if( Y < y && X < x ) angle = (acos((X - x) / floatsqroot((X - x)*(X - x) + (Y - y)*(Y - y))) - 450.0);
	else if( Y < y ) angle = (acos((X - x) / floatsqroot((X - x)*(X - x) + (Y - y)*(Y - y))) - 90.0);

	return (PED_AngleInRangeOfAngle(-angle, A, range));
}

stock PED_SetFacingPlayer(pedid, playerid)
{
	if(! PED_IsConnected(pedid)) return false;
	if(! IsPlayerConnected(playerid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid))
    {
		new Float:pX, Float:pY, Float:pZ;
		GetPlayerPos(playerid, pX, pY, pZ);

		return PED_SetFacingPoint(pedid, pX, pY);
	}
    return false;
}

stock PED_IsFacingPlayer(pedid, playerid, Float:range =  10.0)
{
	if(! PED_IsConnected(pedid)) return false;
	if(! IsPlayerConnected(playerid)) return false;

	new Float:pX, Float:pY, Float:pZ;
	GetPlayerPos(playerid, pX, pY, pZ);

	return PED_IsFacingPoint(pedid, pX, pY, range);
}

stock PED_IsBehindPlayer(pedid, playerid, Float:range = 10.0)
{
	if(! PED_IsConnected(pedid)) return false;
	if(! IsPlayerConnected(playerid)) return false;

	new Float:za, Float:pa;
	PED_GetFacingAngle(pedid, za);
	GetPlayerFacingAngle(playerid, pa);

	return (PED_AngleInRangeOfAngle(za, pa, range) && PED_IsFacingPlayer(playerid, playerid));
}

stock PED_IsInRangeOfPoint(pedid, Float:range, Float:x, Float:y, Float:z)
{
	if(! PED_IsConnected(pedid)) return false;

	new Float:pos[3];
	PED_GetPos(pedid, pos[0], pos[1], pos[2]);

	return (PED_GetDistance(pos[0], pos[1], pos[2], x, y, z) <= range);
}

stock PED_SetHealth(pedid, Float:health)
{
	if(! PED_IsConnected(pedid)) return false;
	if(PED_IsDead(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid))
	{
	    if(health <= 0.0)
	    {
	    	if(CallLocalFunction("PED_OnDeath", "iii", pedid, INVALID_PLAYER_ID, 0))
			{
				if(gPED[pedid][P_DEATHTIMER] != -1) KillTimer(gPED[pedid][P_DEATHTIMER]);
				gPED[pedid][P_DEATHTIMER] = SetTimerEx("PED_SpawnPlayerAfterDeath", 3 * 1000 + 5 * 100, false, "i", pedid);//spawn after 3.5 seconds

                SetActorHealth(pedid, health);
			}
		    else return false;
		}
		else return SetActorHealth(pedid, health);
	}
	return false;
}

stock PED_GetHealth(pedid, &Float:health)
{
	if(! PED_IsConnected(pedid)) return false;

	return GetActorHealth(pedid, health);
}

stock PED_IsDead(pedid)
{
	if(! PED_IsConnected(pedid)) return false;

	new Float:health;
	PED_GetHealth(pedid, health);
	if(health <= 0.0) return true;
	return false;
}

stock PED_ApplyAnimation(pedid, animlib[], animname[], Float:fDelta = 4.1, loop = 0, lockx = 1, locky = 1, freeze = 1, time = 0)
{
	if(! PED_IsConnected(pedid)) return false;
	if(PED_IsDead(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid)) return ApplyActorAnimation(pedid, animlib, animname, fDelta, loop, lockx, locky, freeze, time);
    return false;
}

stock PED_Stop(pedid)
{
	if(! PED_IsConnected(pedid)) return false;
	if(PED_IsDead(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid)) return ClearActorAnimations(pedid);
	return false;
}

stock PED_Spawn(pedid)
{
	if(! PED_IsConnected(pedid)) return false;

	if(CallLocalFunction("PED_OnUpdate", "i", pedid))
    {
        new bool:invulnerable = false;
		if(PED_IsInvulnerable(pedid)) invulnerable = true;

        DestroyActor(pedid);
        CreateActor(gPED[pedid][P_SKIN], gPED[pedid][P_X], gPED[pedid][P_Y], gPED[pedid][P_Z], gPED[pedid][P_ROT]);
        SetActorHealth(pedid, gPED[pedid][P_HEALTH]);
        SetActorInvulnerable(pedid, invulnerable);

		foreach_player(i)
	 	{
			if(gPlayerTarget[i] == pedid) gPlayerTarget[i] = INVALID_PED_ID;
		}

		CallLocalFunction("PED_OnSpawn", "i", pedid);
	}
	return false;
}

stock PED_SetSpawnInfo(pedid, Float:x, Float:y, Float:z, Float:rotation, Float:health)
{
	if(! PED_IsConnected(pedid)) return false;

    gPED[pedid][P_X] = x;
    gPED[pedid][P_Y] = y;
    gPED[pedid][P_Z] = z;
    gPED[pedid][P_ROT] = rotation;
    gPED[pedid][P_HEALTH] = health;
    return true;
}

stock PED_IsStreamedIn(pedid, forplayerid)
{
	if(! PED_IsConnected(pedid)) return false;
	if(! IsPlayerConnected(forplayerid)) return false;

	return IsActorStreamedIn(pedid, forplayerid);
}

stock PED_PlaySound(pedid, soundid)
{
	if(! PED_IsConnected(pedid)) return false;
	if(! IsPlayerConnected(playerid)) return false;

	new Float:ped_pos[3];
	PED_GetPos(pedid, ped_pos[0], ped_pos[1], ped_pos[2]);
	foreach_player(i) PlayerPlaySound(i, soundid, ped_pos[0], ped_pos[1], ped_pos[2]);
	return true;
}

//player functions
stock GetPlayerCameraTargetPED(playerid)
{
	if(! IsPlayerConnected(playerid)) return false;

	return GetPlayerCameraTargetActor(playerid);
}

stock GetPlayerTargetPED(playerid)
{
	if(! IsPlayerConnected(playerid)) return false;

	return GetPlayerTargetActor(playerid);
}

public OnPlayerUpdate(playerid)
{
	new target_ped = GetPlayerTargetPED(playerid);
	if(target_ped != INVALID_PED_ID)
	{
	    if(gPlayerTarget[playerid] != target_ped)
	    {
	        gPlayerTarget[playerid] = target_ped;
	        CallLocalFunction("OnPlayerTargetPED", "iii", playerid, target_ped, GetPlayerWeapon(playerid));
	    }
	}

	#if defined HoOK_OnPlayerUpdate
		HoOK_OnPlayerUpdate(playerid);
	#endif
	return 1;
}
#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate HoOK_OnPlayerUpdate
#if defined HoOK_OnPlayerUpdate
	forward HoOK_OnPlayerUpdate(playerid);
#endif

public OnActorStreamIn(actorid, forplayerid)
{
	if(PED_IsConnected(actorid)) CallLocalFunction("PED_OnStreamIn", "ii", actorid, forplayerid);

	#if defined HoOK_OnActorStreamIn
		HoOK_OnActorStreamIn(actorid, forplayerid);
	#endif
	return 1;
}
#if defined _ALS_OnActorStreamIn
	#undef OnActorStreamIn
#else
	#define _ALS_OnActorStreamIn
#endif
#define OnActorStreamIn HoOK_OnActorStreamIn
#if defined HoOK_OnActorStreamIn
	forward HoOK_OnActorStreamIn(actorid, forplayerid);
#endif

public OnActorStreamOut(actorid, forplayerid)
{
	if(PED_IsConnected(actorid)) CallLocalFunction("PED_OnStreamOut", "ii", actorid, forplayerid);

	#if defined HoOK_OnActorStreamOut
		HoOK_OnActorStreamOut(actorid, forplayerid);
	#endif
	return 1;
}
#if defined _ALS_OnActorStreamOut
	#undef OnActorStreamOut
#else
	#define _ALS_OnActorStreamOut
#endif
#define OnActorStreamOut HoOK_OnActorStreamOut
#if defined HoOK_OnActorStreamOut
	forward HoOK_OnActorStreamOut(actorid, forplayerid);
#endif

public OnPlayerGiveDamageActor(playerid, damaged_actorid, Float:amount, weaponid, bodypart)
{

	if(! PED_IsInvulnerable(damaged_actorid))
	{
	    if(! PED_IsDead(damaged_actorid))
		{
			if(! CallLocalFunction("PED_OnTakeDamage", "iiifi", damaged_actorid, playerid, weaponid, amount, bodypart))
			{
			    CallLocalFunction("OnPlayerGivePEDDamage", "iiifi", playerid, damaged_actorid, weaponid, amount, bodypart);
				return 0;
			}
			else
			{
			    new Float:ped_health;
				PED_GetHealth(damaged_actorid, ped_health);
				ped_health -= amount;

			    CallLocalFunction("OnPlayerGivePEDDamage", "iiifi", playerid, damaged_actorid, weaponid, amount, bodypart);
				if(ped_health <= 0.0)//if the ped is dead
				{
					if(CallLocalFunction("PED_OnDeath", "iii", damaged_actorid, playerid, weaponid))
					{
						SetActorHealth(damaged_actorid, 0.0);

						if(gPED[damaged_actorid][P_DEATHTIMER] != -1) KillTimer(gPED[damaged_actorid][P_DEATHTIMER]);
						gPED[damaged_actorid][P_DEATHTIMER] = SetTimerEx("PED_SpawnPlayerAfterDeath", 3 * 1000 + 5 * 100, false, "i", damaged_actorid);//spawn after 3.5 seconds
					}
				}
				else SetActorHealth(damaged_actorid, ped_health);
			}
		}
	}

	#if defined HoOK_OnPlayerGiveDamageActor
		HoOK_OnPlayerGiveDamageActor(playerid, damaged_actorid, Float:amount, weaponid, bodypart);
	#endif
	return 1;
}
#if defined _ALS_OnPlayerGiveDamageActor
	#undef OnPlayerGiveDamageActor
#else
	#define _ALS_OnPlayerGiveDamageActor
#endif
#define OnPlayerGiveDamageActor HoOK_OnPlayerGiveDamageActor
#if defined HoOK_OnPlayerGiveDamageActor
	forward HoOK_OnPlayerGiveDamageActor(playerid, damaged_actorid, Float:amount, weaponid, bodypart);
#endif

forward PED_SpawnPlayerAfterDeath(pedid);
public PED_SpawnPlayerAfterDeath(pedid)	return (gPED[pedid][P_DEATHTIMER] = -1, PED_Spawn(pedid));

forward PED_OnConnect(pedid);
forward PED_OnDisconnect(pedid);
forward PED_OnSpawn(pedid);
forward PED_OnTakeDamage(pedid, issuerid, weaponid, Float:amount, bodypart);
forward PED_OnDeath(pedid, killerid, weaponid);
forward PED_OnStreamIn(pedid, forplayerid);
forward PED_OnStreamOut(pedid, forplayerid);
forward PED_OnUpdate(pedid);

forward OnPlayerTargetPED(playerid, pedid, weaponid);
forward OnPlayerGivePEDDamage(playerid, pedid, weaponid, Float:amount, bodypart);
