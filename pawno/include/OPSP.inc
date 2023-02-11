/*******************************************
* OnPlayerShootPlayer! V:1.0               *
* Credits: wups                            *
********************************************/

// include
#include <a_samp>
#tryinclude <foreach>



#if defined OPSP
	#endinput
#endif
#define OPSP
// variables

new phealth[MAX_PLAYERS];

// forwards
forward OnPlayerShootPlayer(shooter,target,damage);

public OnPlayerUpdate(playerid)
{
	static  Float:HP,
			Float:Armour;
	GetPlayerHealth(playerid,HP);
	GetPlayerArmour(playerid,Armour);
	if(HP+Armour < phealth[playerid])
	{
		#if defined _foreach_included
		foreach(Player, i)
		#else
	    for(new i;i<MAX_PLAYERS;i++)
	    #endif
	    {
	        if(i==playerid) continue;
	        if(IsPlayerAimingAtPlayer(i,playerid))
	        {
/*				new msg[40];
				format(msg,sizeof(msg),"Droped by: %iHP",floatround(phealth[playerid]-HP-Armour));
				SendClientMessage(i,0x33AA33AA,msg); */ // only testing
				CallLocalFunction("OnPlayerShootPlayer","iii",i,playerid,floatround(phealth[playerid]-HP-Armour));
				break;
	        }
	    }
	
	}
	phealth[playerid]=floatround(HP+Armour);
	return CallLocalFunction("RL_OnPlayerUpdate","i",playerid);
}


Float:DistanceCameraTargetToLocation(Float:CamX, Float:CamY, Float:CamZ,  Float:ObjX, Float:ObjY, Float:ObjZ,  Float:FrX, Float:FrY, Float:FrZ) {

	new Float:TGTDistance;

	TGTDistance = floatsqroot((CamX - ObjX) * (CamX - ObjX) + (CamY - ObjY) * (CamY - ObjY) + (CamZ - ObjZ) * (CamZ - ObjZ));

	new Float:tmpX, Float:tmpY, Float:tmpZ;

	tmpX = FrX * TGTDistance + CamX;
	tmpY = FrY * TGTDistance + CamY;
	tmpZ = FrZ * TGTDistance + CamZ;

	return floatsqroot((tmpX - ObjX) * (tmpX - ObjX) + (tmpY - ObjY) * (tmpY - ObjY) + (tmpZ - ObjZ) * (tmpZ - ObjZ));
}

stock IsPlayerAimingAt(playerid, Float:x, Float:y, Float:z, Float:radiusz)
{
	new Float:cx,Float:cy,Float:cz,Float:fx,Float:fy,Float:fz;
	GetPlayerCameraPos(playerid, cx, cy, cz);
	GetPlayerCameraFrontVector(playerid, fx, fy, fz);
	return (radiusz >= DistanceCameraTargetToLocation(cx, cy, cz, x, y, z, fx, fy, fz));
}

stock IsPlayerAimingAtPlayer(playerid, target) {
 new Float:x, Float:y, Float:z;
 GetPlayerPos(target, x, y, z);
 if (IsPlayerAimingAt(playerid, x, y, z-0.75, 1)) return true;
 if (IsPlayerAimingAt(playerid, x, y, z+0.25, 1)) return true;
 return false;
}

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate RL_OnPlayerUpdate
forward RL_OnPlayerUpdate(playerid);
