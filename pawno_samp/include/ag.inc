 /*
 --*                                *
 --*   @Author:      Hanger         *
 --*   @Version:     0.2.7.2        *
 --*   @Released:    29/04/2012     *
 --*                                *
 */

#if defined _avs_included
	#endinput
#endif
#define _avs_included

forward DelayReset(forplayerid, vehicleid); /* Resets Vehicle Default location */
forward OnVehicleTeleportDetect(forplayerid, vehicleid); /* When vehicle spawn is detected due script */
forward OnStrangeEnterDetect(playerid, vehicleid); /* If player got into vehicle without serversided functions e.g. script - PutPlayerInVehicle, keys */
/* Returns playerid if detection is made whilst player is in vehicle otherwise it will return INVALID_PLAYER_ID */
forward OnUnknownVehicleDetect(vehicleid, playerid); /* When an unknown vehicle is detected and deleted */

forward avs_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
forward avs_OnPlayerStateChange(playerid, newstate, oldstate);
forward avs_OnVehicleStreamIn(vehicleid, forplayerid);
forward avs_OnVehicleStreamOut(vehicleid, forplayerid);
forward avs_OnVehicleSpawn(vehicleid);
forward avs_OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat);

new
	Float:VehiclePosition[MAX_VEHICLES][3],
	Float:PlayerPosition[MAX_PLAYERS][3],
	PlayerOldVehicleID[MAX_PLAYERS] = { -1, ... },
	bool:IsPlayerEntering[MAX_PLAYERS char] = { false, ... },
	bool:AllowReset[MAX_VEHICLES char] = { true, ... },
	bool:SpamCheck[MAX_VEHICLES char] = { false, ... },
	bool:SpamCheck2[MAX_PLAYERS char] = { false, ... },
	bool:VehicleByServer[MAX_VEHICLES char] = { false, ... };

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(SpamCheck{vehicleid})
	{
		DelayReset(playerid, vehicleid);
	}
	else { IsPlayerEntering{playerid} = true; }
	return CallLocalFunction("avs_OnPlayerEnterVehicle", "iii", playerid, vehicleid, ispassenger);
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(!IsPlayerNPC(playerid))
	{
		if(newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
		{
			PlayerOldVehicleID[playerid] = GetPlayerVehicleID(playerid);
			if(!IsPlayerEntering{playerid} || SpamCheck{PlayerOldVehicleID[playerid]} || !AllowReset{PlayerOldVehicleID[playerid]})
			{
				if(!IsPlayerEntering{playerid})
				{ CallLocalFunction("OnStrangeEnterDetect", "ii", playerid, PlayerOldVehicleID[playerid]); }
				DelayReset(playerid, PlayerOldVehicleID[playerid]);
			}
			if(!VehicleByServer{PlayerOldVehicleID[playerid]})
			{
				DestroyVehicle(PlayerOldVehicleID[playerid]);
				CallLocalFunction("OnUnknownVehicleDetect", "ii", PlayerOldVehicleID[playerid], playerid);
			}
		}
		else if(newstate == PLAYER_STATE_ONFOOT)
		{
			if(oldstate == PLAYER_STATE_DRIVER && AllowReset{PlayerOldVehicleID[playerid]})
			{
				GetVehiclePos(PlayerOldVehicleID[playerid], VehiclePosition[PlayerOldVehicleID[playerid]][0],VehiclePosition[PlayerOldVehicleID[playerid]][1],VehiclePosition[PlayerOldVehicleID[playerid]][2]);
			}
			PlayerOldVehicleID[playerid] = -1;
		}
		IsPlayerEntering{playerid} = false;
	}
	return CallLocalFunction("avs_OnPlayerStateChange", "iii", playerid, newstate, oldstate);
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	if(!IsPlayerNPC(forplayerid))
	{
		new Float:newDist = GetVehicleDistanceFromPoint(vehicleid, VehiclePosition[vehicleid][0], VehiclePosition[vehicleid][1],VehiclePosition[vehicleid][2]);
		if(newDist >= 15.0)
		{
			AllowReset{vehicleid} = false;
			SpamCheck{vehicleid} = true;
			SpamCheck2{forplayerid} = true;
			GetPlayerPos(forplayerid, PlayerPosition[forplayerid][0], PlayerPosition[forplayerid][1], PlayerPosition[forplayerid][2]);
			new Float:vPos[3];
			GetVehiclePos(vehicleid, vPos[0], vPos[1], vPos[2]);
			if(IsPlayerInRangeOfPoint(forplayerid, 7.0, vPos[0], vPos[1], vPos[2]))
			{
				CallLocalFunction("OnVehicleTeleportDetect", "ii", forplayerid, vehicleid);
			}
		}
		if(!VehicleByServer{vehicleid})
		{
			DestroyVehicle(vehicleid);
			CallLocalFunction("OnUnknownVehicleDetect", "ii", vehicleid, INVALID_PLAYER_ID);
		}
	}
	return CallLocalFunction("avs_OnVehicleStreamIn", "ii", vehicleid, forplayerid);
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	if(SpamCheck2{forplayerid})
	{
		SpamCheck{vehicleid} = false;
		SpamCheck2{forplayerid} = false;
	}
	return CallLocalFunction("avs_OnVehicleStreamOut", "ii", vehicleid, forplayerid);
}

public OnVehicleSpawn(vehicleid)
{
	if(!VehicleByServer{vehicleid})
	{
		DestroyVehicle(vehicleid);
		CallLocalFunction("OnUnknownVehicleDetect", "ii", vehicleid, INVALID_PLAYER_ID);
	}
	AllowReset{vehicleid} = true;
	SpamCheck{vehicleid} = false;
	GetVehiclePos(vehicleid, VehiclePosition[vehicleid][0], VehiclePosition[vehicleid][1],VehiclePosition[vehicleid][2]);
	return CallLocalFunction("avs_OnVehicleSpawn", "i", vehicleid);
}

public OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat)
{
	if(SpamCheck{vehicleid})
		DelayReset(playerid, vehicleid);
	if(AllowReset{vehicleid})
		GetVehiclePos(vehicleid, VehiclePosition[vehicleid][0], VehiclePosition[vehicleid][1],VehiclePosition[vehicleid][2]);
	return CallLocalFunction("avs_OnUnoccupiedVehicleUpdate", "iii", vehicleid, playerid, passenger_seat);
}

public DelayReset(forplayerid, vehicleid)
{
	IsPlayerEntering{forplayerid} = false;
	if(IsPlayerInAnyVehicle(forplayerid)) { RemovePlayerFromVehicle(forplayerid); }
	SetVehiclePos(vehicleid, VehiclePosition[vehicleid][0], VehiclePosition[vehicleid][1],VehiclePosition[vehicleid][2]);
	if(!IsVehicleStreamedIn(vehicleid, forplayerid))
	{
		SpamCheck{vehicleid} = false;
		AllowReset{vehicleid} = true;
	}
	return 0;
}

stock avs_AddStaticVehicle(modelID, Float: spawn_X, Float: spawn_Y, Float: spawn_Z, Float: z_Angle, color1, color2)
{
	new vehicleid = 0;
	for(new i = 1; i < MAX_VEHICLES; i++)
	{
		if(GetVehicleModel(i) >= 400) continue;
		else vehicleid = i; break;
	}
	VehiclePosition[vehicleid][0] = spawn_X;
	VehiclePosition[vehicleid][1] = spawn_Y;
	VehiclePosition[vehicleid][2] = spawn_Z;
	AllowReset{vehicleid} = true;
	SpamCheck{vehicleid} = false;
	VehicleByServer{vehicleid} = true;
	modelID = AddStaticVehicle(modelID, spawn_X, spawn_Y, spawn_Z, z_Angle, color1, color2);
	return modelID;
}

stock avs_AddStaticVehicleEx(modelID, Float: spawn_X, Float: spawn_Y, Float: spawn_Z, Float: z_Angle, color1, color2, respawn_Time)
{
	new vehicleid = 0;
	for(new i = 1; i < MAX_VEHICLES; i++)
	{
		if(GetVehicleModel(i) >= 400) continue;
		else vehicleid = i; break;
	}
	VehiclePosition[vehicleid][0] = spawn_X;
	VehiclePosition[vehicleid][1] = spawn_Y;
	VehiclePosition[vehicleid][2] = spawn_Z;
	AllowReset{vehicleid} = true;
	SpamCheck{vehicleid} = false;
	VehicleByServer{vehicleid} = true;
	modelID = AddStaticVehicleEx(modelID, spawn_X, spawn_Y, spawn_Z, z_Angle, color1, color2, respawn_Time);
	return modelID;
}

stock avs_CreateVehicle(modelID, Float: spawn_X, Float: spawn_Y, Float: spawn_Z, Float: z_Angle, color1, color2, respawn_Time)
{
	new vehicleid = 0;
	for(new i = 1; i < MAX_VEHICLES; i++)
	{
		if(GetVehicleModel(i) >= 400) continue;
		else vehicleid = i; break;
	}
	VehiclePosition[vehicleid][0] = spawn_X;
	VehiclePosition[vehicleid][1] = spawn_Y;
	VehiclePosition[vehicleid][2] = spawn_Z;
	AllowReset{vehicleid} = true;
	SpamCheck{vehicleid} = false;
	VehicleByServer{vehicleid} = true;
	modelID = CreateVehicle(modelID, spawn_X, spawn_Y, spawn_Z, z_Angle, color1, color2, respawn_Time);
	return modelID;
}

stock avs_SetVehiclePos(vehicleid, Float:X, Float:Y, Float:Z)
{
	VehiclePosition[vehicleid][0] = X;
	VehiclePosition[vehicleid][1] = Y;
	VehiclePosition[vehicleid][2] = Z;
	return SetVehiclePos(vehicleid, X, Y, Z);
}

stock avs_PutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if(!IsPlayerNPC(playerid))
	{ IsPlayerEntering{playerid} = true; }
	return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

stock avs_DestroyVehicle(vehicleid)
{
	VehicleByServer{vehicleid} = false;
	return DestroyVehicle(vehicleid);
}

#if defined _ALS_OnPlayerEnterVehicle
    #undef OnPlayerEnterVehicle
#else
    #define _ALS_OnPlayerEnterVehicle
#endif
#define OnPlayerEnterVehicle avs_OnPlayerEnterVehicle

#if defined _ALS_OnPlayerStateChange
    #undef OnPlayerStateChange
#else
    #define _ALS_OnPlayerStateChange
#endif
#define OnPlayerStateChange avs_OnPlayerStateChange

#if defined _ALS_OnVehicleStreamIn
    #undef OnVehicleStreamIn
#else
    #define _ALS_OnVehicleStreamIn
#endif
#define OnVehicleStreamIn avs_OnVehicleStreamIn

#if defined _ALS_OnVehicleStreamOut
    #undef OnVehicleStreamOut
#else
    #define _ALS_OnVehicleStreamOut
#endif
#define OnVehicleStreamOut avs_OnVehicleStreamOut

#if defined _ALS_OnVehicleSpawn
    #undef OnVehicleSpawn
#else
    #define _ALS_OnVehicleSpawn
#endif
#define OnVehicleSpawn avs_OnVehicleSpawn

#if defined _ALS_OnUnoccupiedVehicleUpdate
    #undef OnUnoccupiedVehicleUpdate
#else
    #define _ALS_OnUnoccupiedVehicleUpdate
#endif
#define OnUnoccupiedVehicleUpdate avs_OnUnoccupiedVehicleUpdate

#if defined _ALS_AddStaticVehicle
    #undef AddStaticVehicle
#else
    #define _ALS_AddStaticVehicle
#endif
#define AddStaticVehicle avs_AddStaticVehicle

#if defined _ALS_AddStaticVehicleEx
    #undef AddStaticVehicleEx
#else
    #define _ALS_AddStaticVehicleEx
#endif
#define AddStaticVehicleEx avs_AddStaticVehicleEx

#if defined _ALS_CreateVehicle
    #undef CreateVehicle
#else
    #define _ALS_CreateVehicle
#endif
#define CreateVehicle avs_CreateVehicle

#if defined _ALS_SetVehiclePos
    #undef SetVehiclePos
#else
    #define _ALS_SetVehiclePos
#endif
#define SetVehiclePos avs_SetVehiclePos

#if defined _ALS_PutPlayerInVehicle
    #undef PutPlayerInVehicle
#else
    #define _ALS_PutPlayerInVehicle
#endif
#define PutPlayerInVehicle avs_PutPlayerInVehicle

#if defined _ALS_DestroyVehicle
    #undef DestroyVehicle
#else
    #define _ALS_DestroyVehicle
#endif
#define DestroyVehicle avs_DestroyVehicle
