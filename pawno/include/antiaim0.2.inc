/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BUST AIM
//
//BustAim is an feature rich include which tries to detect players with aimbots.It by itself cannot be for sure if the player
//is using aimbot.It is designed to trigger warnings and provide administrators with vital information about the suspect.
//BustAim tries its level best to detect players who are using aimbots.This include while detecting players for aimbots
//sometimes also detects some miscellaneous hacks such AFK Ghost(Randomly Aimed Shots) or modified weapon.dat
//
//License:Public Domain
//
//Credits:
//Yashas
//RedShirt & niCe & JernejL for camera/aiming functions
//ipsLeon & Kyance for their aimbot detectors
//Pottus for constructive criticism
//Slice for the information regarding Maximum Weapon Firing Range
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Callbacks:
//	OnPlayerSuspectedForAimbot(playerid,hitid,weaponid,warnings)
//
//Stocks:
//	native BustAim::GetPlayerWeaponProfile(playerid,weaponid,&allshots,&hitshots,&max_cont_shots,&out_of_range_warns,&random_aim_warns,&proaim_tele_warns,&backward_shot_warns);
//	native BustAim::ResetPlayerWeaponProfile(playerid,weaponid);
//  native BustAim::GetPlayerProfile(playerid,&shotsfired,&shotshit,&max_cont_shots,&out_of_range_warns,&random_aim_warns,&proaim_tele_warns,&backward_shot_warns);
//  native BustAim::ResetPlayerProfile(playerid);
//  native BustAim::SetPlayerFlags(playerid,flags);
//  native BustAim::GetPlayerFlags(playerid,&flags);
//  native BustAim::ResetPlayerFlags(playerid);
//  native BustAim::GetTeleportStats(playerid,Float:arr[],sz = sizeof(arr));
//  native BustAim::GetAimStats(playerid,Float:arr[],sz = sizeof(arr))
//  native BustAim::GetRangeStats(playerid,Float:arr[],sz = sizeof(arr))
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined _INCLUDE_BUSTAIM_
	#endinput
#endif
#define _INCLUDE_BUSTAIM_
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BustAim:: BS_

#if !defined BUSTAIM_MAX_PL_PERCENTAGE
	#define BUSTAIM_MAX_PL_PERCENTAGE 2
#endif
#if !defined BUSTAIM_MAX_PING
	#define BUSTAIM_MAX_PING 350
#endif
#if !defined BUSTAIM_SKIP_WEAPON_IDS
	#define BUSTAIM_SKIP_WEAPON_IDS 38
#endif
#if !defined BUSTAIM_MAX_CONTINOUS_SHOTS
	#define BUSTAIM_MAX_CONTINOUS_SHOTS 10
#endif
#if !defined BUSTAIM_OUT_OF_RANGE_PROBES
	#define BUSTAIM_OUT_OF_RANGE_PROBES 4
#endif
#if !defined BUSTAIM_PROAIM_TELEPORT_PROBES
	#define BUSTAIM_PROAIM_TELEPORT_PROBES 3
#endif
#if !defined BUSTAIM_BACKWARD_SHOTS_PROBES
	#define BUSTAIM_BACKWARD_SHOTS_PROBES 2
#endif
#if !defined BUSTAIM_RANDOM_AIM_PROBES
	#define BUSTAIM_RANDOM_AIM_PROBES 5
#endif
#if !defined MIN_DIST_FOR_TELEPORT_CHECKS
	#define MIN_DIST_FOR_TELEPORT_CHECKS 5
#endif
#if !defined MIN_DIST_FOR_AIM_CHECKS
	#define MIN_DIST_FOR_AIM_CHECKS 5
#endif
#if !defined MAX_B2V_DEVIATION
	#define MAX_B2V_DEVIATION 5
#endif
#if !defined BUSTAIM_PLAYER_SPHERE_RADIUS
	#define BUSTAIM_PLAYER_SPHERE_RADIUS 3
#endif
#if !defined BUSTAIM_DEFAULT_PLAYER_FLAGS
	#define BUSTAIM_DEFAULT_PLAYER_FLAGS (CHECK_FOR_OUT_OF_RANGE_SHOTS	|	CHECK_FOR_PROAIM_TELEPORT   |	CHECK_FOR_RANDOM_AIM_SHOTS	|	CHECK_FOR_BACKWARD_SHOTS | 	CHECK_FOR_CONTINOUS_SHOTS)
#endif
#if !defined BUSTAIM_WSTATS_SHOTS
	#define BUSTAIM_WSTATS_SHOTS 3
#endif

//DO NOT CHANGE THIS
#define BS_TOTAL_SHOOTING_WEAPONS 17 //includes RPG,HS,FT
#if BS_TOTAL_SHOOTING_WEAPONS != 17
	#error BS_TOTAL_SHOOTING_WEAPONS should always be set to 17
#endif

#define BS_GetNormalWeaponRange(weaponid) BustAim_g_WeaponRange[weaponid-22]
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
forward OnPlayerSuspectedForAimbot(playerid,hitid,weaponid,warnings);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum (<<=1)
{
	WARNING_OUT_OF_RANGE_SHOT=1,
	WARNING_PROAIM_TELEPORT,
	WARNING_RANDOM_AIM,
	WARNING_BACKWARD_SHOT,
	WARNING_CONTINOUS_SHOTS
}
enum (<<=1)
{
	CHECK_FOR_OUT_OF_RANGE_SHOTS=1,
	CHECK_FOR_PROAIM_TELEPORT,
	CHECK_FOR_RANDOM_AIM_SHOTS,
	CHECK_FOR_BACKWARD_SHOTS,
	CHECK_FOR_CONTINOUS_SHOTS,
	IGNORE_PLAYER
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Modified(increased M4,AK Ranges from 70,90 to 100,120) Slice's Max Weapon Ranges from Weapon Config Include
new const Float:BustAim_g_WeaponRange[] =
{
	35.0, // 22 - Colt 45
	35.0, // 23 - Silenced
	35.0, // 24 - Deagle
	40.0, // 25 - Shotgun
	35.0, // 26 - Sawed-off
	40.0, // 27 - Spas
	35.0, // 28 - UZI
	45.0, // 29 - MP5
	100.0, // 30 - AK47
	120.0, // 31 - M4
	35.0, // 32 - Tec9
	100.0, // 33 - Cuntgun
	320.0, // 34 - Sniper
	0.0, // 35 - Rocket launcher
	0.0, // 36 - Heatseeker
	0.0, // 37 - Flamethrower
	75.0  // 38 - Minigun
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static BustAim_g_PlayerSettings[MAX_PLAYERS char];
static BustAim_g_PlayerPrevWeapon[MAX_PLAYERS char];

static BustAim_g_ContinousShots[MAX_PLAYERS char];
static BustAim_g_RandomAimShots[MAX_PLAYERS char];
static BustAim_g_TeleportShots[MAX_PLAYERS char];
static BustAim_g_OutOfRangeShots[MAX_PLAYERS char];
static BustAim_g_BackwardShot[MAX_PLAYERS char];

#if !defined BUSTAIM_DISABLE_PROFILING
static BustAim_g_TotalRandomAimWarns[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
static BustAim_g_TotalTeleportWarns[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
static BustAim_g_TotalBackwardWarns[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
static BustAim_g_TotalOutOfRangeWarns[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
static BustAim_g_MaxContinousShots[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
static BustAim_g_ShotsFired[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
static BustAim_g_ShotsHit[MAX_PLAYERS][BS_TOTAL_SHOOTING_WEAPONS];
#endif

#if !defined BUSTAIM_DISABLE_WSTATS
static Float:BustAim_g_TeleportWarningStats[MAX_PLAYERS][BUSTAIM_WSTATS_SHOTS];
static Float:BustAim_g_AimWarningStats[MAX_PLAYERS][BUSTAIM_WSTATS_SHOTS];
static Float:BustAim_g_RangeWarningStats[MAX_PLAYERS][BUSTAIM_WSTATS_SHOTS];
static Float:BustAim_g_DCTTL_temp;
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Float:internal_BS_DCTTL(Float:CamX, Float:CamY, Float:CamZ, Float:ObjX, Float:ObjY, Float:ObjZ, Float:FrX, Float:FrY, Float:FrZ)
{
   	static Float:TGTDistance,Float:tmpX, Float:tmpY, Float:tmpZ;
    TGTDistance = floatsqroot((CamX - ObjX) * (CamX - ObjX) + (CamY - ObjY) * (CamY - ObjY) + (CamZ - ObjZ) * (CamZ - ObjZ));

    tmpX = FrX * TGTDistance + CamX;
    tmpY = FrY * TGTDistance + CamY;
    tmpZ = FrZ * TGTDistance + CamZ;

    return floatsqroot((tmpX - ObjX) * (tmpX - ObjX) + (tmpY - ObjY) * (tmpY - ObjY) + (tmpZ - ObjZ) * (tmpZ - ObjZ));
}
static Float:internal_BS_GPATP(Float:x2, Float:y2, Float:X, Float:Y)
{
  static Float:DX, Float:DY,Float:angle;

  DX = floatabs(floatsub(x2,X));
  DY = floatabs(floatsub(y2,Y));

  if (DY == 0.0 || DX == 0.0) {
    if(DY == 0 && DX > 0) angle = 0.0;
    else if(DY == 0 && DX < 0) angle = 180.0;
    else if(DY > 0 && DX == 0) angle = 90.0;
    else if(DY < 0 && DX == 0) angle = 270.0;
    else if(DY == 0 && DX == 0) angle = 0.0;
  }
  else {
    angle = atan(DX/DY);

    if(X > x2 && Y <= y2) angle += 90.0;
    else if(X <= x2 && Y < y2) angle = floatsub(90.0, angle);
    else if(X < x2 && Y >= y2) angle -= 90.0;
    else if(X >= x2 && Y > y2) angle = floatsub(270.0, angle);
  }

  return floatadd(angle, 90.0);
}
static internal_BS_GXYIFOP(&Float:x, &Float:y, Float:angle, Float:distance)
{
        x += (distance * floatsin(-angle, degrees));
        y += (distance * floatcos(-angle, degrees));
}
static internal_BS_IsCameraAimingAt(weaponid,Float:x, Float:y, Float:z,Float:vector_x,Float:vector_y,Float:vector_z,Float:camera_x,Float:camera_y,Float:camera_z,Float:radius)
{
   	static Float:vertical, Float:horizontal;
    switch (weaponid)
    {
		case 34,35,36:
		{
			if (internal_BS_DCTTL(camera_x, camera_y, camera_z, x, y, z, vector_x, vector_y, vector_z) < radius) return true;
			return false;
        }
        case 30,31: {vertical = 4.0; horizontal = -1.6;}
        case 33: {vertical = 2.7; horizontal = -1.0;}
        default: {vertical = 6.0; horizontal = -2.2;}
	}
	new Float:angle = internal_BS_GPATP(0, 0, floatsqroot(vector_x*vector_x+vector_y*vector_y), vector_z) - 270.0;
	new Float:resize_x, Float:resize_y, Float:resize_z = floatsin(angle+vertical, degrees);
     
	internal_BS_GXYIFOP(resize_x, resize_y, internal_BS_GPATP(0, 0, vector_x, vector_y)+horizontal, floatcos(angle+vertical, degrees));
    #if !defined BUSTAIM_DISABLE_WSTATS
    	if ((BustAim_g_DCTTL_temp = internal_BS_DCTTL(camera_x, camera_y, camera_z, x, y, z, resize_x, resize_y, resize_z)) < radius) return true;
    #else
		if (internal_BS_DCTTL(camera_x, camera_y, camera_z, x, y, z, resize_x, resize_y, resize_z) < radius) return true;
	#endif
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
    static Float:pX,Float:pY,Float:pZ;
	static Float:hX,Float:hY,Float:hZ;
	static Float:cX,Float:cY,Float:cZ;
	
	#if !defined BUSTAIM_DISABLE_PROFILING
		BustAim_g_ShotsFired[playerid][weaponid-22]++;
	#endif
	
    if(hittype == BULLET_HIT_TYPE_PLAYER)
    {
    	#if !defined BUSTAIM_DISABLE_PROFILING
        	BustAim_g_ShotsHit[playerid][weaponid-22]++;
		#endif
        if(BustAim_g_PlayerSettings{playerid} & IGNORE_PLAYER)
            #if defined BustAim_OnPlayerWeaponShot
	        	return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
			#else
				return 1;
			#endif
        
		if(BustAim_g_PlayerPrevWeapon{playerid} != weaponid)
		{
		    BustAim_g_ContinousShots{playerid} =
			BustAim_g_RandomAimShots{playerid} =
			BustAim_g_TeleportShots{playerid} =
			BustAim_g_OutOfRangeShots{playerid} =
			BustAim_g_BackwardShot{playerid} = 0;
			
		    BustAim_g_PlayerPrevWeapon{playerid} = weaponid;
		}
        
        //if(IsPlayerNPC(playerid) || IsPlayerNPC(hitid))
	     //   if(BustAim_g_OPWS_pointer != -1)
	      //  	return _CALL_OPWS(playerid,weaponid,hittype,hitid,fX,fY,fZ);
	      // 	else return 1;

        if(NetStats_PacketLossPercent(playerid) > BUSTAIM_MAX_PL_PERCENTAGE ||  NetStats_PacketLossPercent(hitid) > BUSTAIM_MAX_PL_PERCENTAGE)
			#if defined BustAim_OnPlayerWeaponShot
	           		return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
			#else
				return 1;
			#endif

        if(GetPlayerPing(playerid) > BUSTAIM_MAX_PING ||  GetPlayerPing(playerid) > BUSTAIM_MAX_PING)
			#if defined BustAim_OnPlayerWeaponShot
	           		return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
			#else
				return 1;
			#endif
			
   		if(IsPlayerInAnyVehicle(playerid) || IsPlayerInAnyVehicle(hitid))
        	#if defined BustAim_OnPlayerWeaponShot
	       		return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
			#else
				return 1;
			#endif
			
		if(GetPlayerSurfingVehicleID(playerid) != INVALID_VEHICLE_ID || GetPlayerSurfingVehicleID(hitid) != INVALID_VEHICLE_ID)
      		#if defined BustAim_OnPlayerWeaponShot
	           		return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
				#else
					return 1;
				#endif

		switch(weaponid)
		{
		    case BUSTAIM_SKIP_WEAPON_IDS:
		    {
		        #if defined BustAim_OnPlayerWeaponShot
	           		return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
				#else
					return 1;
				#endif
		    }
		}
	    
	    GetPlayerVelocity(hitid,pX,pY,pZ);
	    
        new warning = 0;
	    if(BustAim_g_PlayerSettings{playerid} & CHECK_FOR_CONTINOUS_SHOTS)
		    if((pX*pX + pY*pY + pZ*pZ) > 0.01)
		        if(!(++BustAim_g_ContinousShots{playerid} % BUSTAIM_MAX_CONTINOUS_SHOTS))
					warning |= WARNING_CONTINOUS_SHOTS;

		GetPlayerPos(hitid,hX,hY,hZ);
		GetPlayerLastShotVectors(playerid,pX,pY,pZ,cX,cY,cZ);

		new Float:S2V_dist = VectorSize(pX-hX,pY-hY,pZ-hZ); //Shooter to Victim Distance
		if(BustAim_g_PlayerSettings{playerid} & CHECK_FOR_OUT_OF_RANGE_SHOTS)
			if(S2V_dist > BustAim_g_WeaponRange[weaponid-22])
			{
			    #if !defined BUSTAIM_DISABLE_WSTATS
                    BustAim_g_RangeWarningStats[playerid][BustAim_g_OutOfRangeShots{playerid}%BUSTAIM_WSTATS_SHOTS] = S2V_dist;
				#endif
		        if(++BustAim_g_OutOfRangeShots{playerid} > BUSTAIM_OUT_OF_RANGE_PROBES)
				{
				    BustAim_g_OutOfRangeShots{playerid} = 0;
				    #if !defined BUSTAIM_DISABLE_PROFILING
				    	BustAim_g_TotalOutOfRangeWarns[playerid][weaponid-22]++;
					#endif
					warning |= WARNING_OUT_OF_RANGE_SHOT;
				}
			}
        if(BustAim_g_PlayerSettings{playerid} & CHECK_FOR_PROAIM_TELEPORT)
			if(S2V_dist > MIN_DIST_FOR_TELEPORT_CHECKS)
			    if(VectorSize(cX-hX,cY-hY,cZ-hZ) > MAX_B2V_DEVIATION)
			    {
			        #if !defined BUSTAIM_DISABLE_WSTATS
                    	BustAim_g_TeleportWarningStats[playerid][BustAim_g_TeleportShots{playerid}%BUSTAIM_WSTATS_SHOTS] = VectorSize(cX-hX,cY-hY,cZ-hZ);
					#endif
			        if(++BustAim_g_TeleportShots{playerid} > BUSTAIM_PROAIM_TELEPORT_PROBES)
					{
					    BustAim_g_TeleportShots{playerid} = 0;
					    #if !defined BUSTAIM_DISABLE_PROFILING
					    	BustAim_g_TotalTeleportWarns[playerid][weaponid-22]++;
						#endif
						warning |= WARNING_PROAIM_TELEPORT;
					}
				}
		GetPlayerCameraFrontVector(playerid,pX,pY,pZ);
		GetPlayerCameraPos(playerid,cX,cY,cZ);

		if(BustAim_g_PlayerSettings{playerid} & CHECK_FOR_BACKWARD_SHOTS)
			if(VectorSize(pX + cX  - hX,pY + cY - hY,pZ + cZ - hZ) > VectorSize(cX-hX,cY-hY,cZ-hZ))
			    if(++BustAim_g_BackwardShot{playerid} > BUSTAIM_BACKWARD_SHOTS_PROBES)
				{
				    BustAim_g_BackwardShot{playerid} = 0;
				    #if !defined BUSTAIM_DISABLE_PROFILING
				    	BustAim_g_TotalBackwardWarns[playerid][weaponid-22]++;
					#endif
					warning |= WARNING_BACKWARD_SHOT;
				}

        if(BustAim_g_PlayerSettings{playerid} & CHECK_FOR_RANDOM_AIM_SHOTS)
			if(S2V_dist > MIN_DIST_FOR_AIM_CHECKS)
				if(!internal_BS_IsCameraAimingAt(weaponid,hX,hY,hZ,pX,pY,pZ,cX,cY,cZ,BUSTAIM_PLAYER_SPHERE_RADIUS))
				{
				    #if !defined BUSTAIM_DISABLE_WSTATS
                    	BustAim_g_AimWarningStats[playerid][BustAim_g_RandomAimShots{playerid}%BUSTAIM_WSTATS_SHOTS] = BustAim_g_DCTTL_temp;
					#endif
	                if(++BustAim_g_RandomAimShots{playerid} > BUSTAIM_RANDOM_AIM_PROBES)
	                {
	                    BustAim_g_RandomAimShots{playerid} = 0;
	                    #if !defined BUSTAIM_DISABLE_PROFILING
					    	BustAim_g_TotalRandomAimWarns[playerid][weaponid-22]++;
						#endif
						warning |= WARNING_RANDOM_AIM;
	                }
				}
		if(warning)
		{
	        #if defined OnPlayerSuspectedForAimbot
	           if(OnPlayerSuspectedForAimbot(playerid,hitid,weaponid,warning)) BustAim_g_PlayerSettings{playerid} |= IGNORE_PLAYER;
	        #endif
		}
	}
	else
	{
	    #if !defined BUSTAIM_DISABLE_PROFILING
	    	if(BustAim_g_ContinousShots{playerid} > BustAim_g_MaxContinousShots[playerid][weaponid-22])
            	BustAim_g_MaxContinousShots[playerid][weaponid-22] = BustAim_g_ContinousShots{playerid};
	 	#endif
	    BustAim_g_ContinousShots{playerid} =
		BustAim_g_RandomAimShots{playerid} =
		BustAim_g_TeleportShots{playerid} =
		BustAim_g_OutOfRangeShots{playerid} =
		BustAim_g_BackwardShot{playerid} = 0;
	}
    #if defined BustAim_OnPlayerWeaponShot
		return BustAim_OnPlayerWeaponShot(playerid,weaponid,hittype,hitid,fX,fY,fZ);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnPlayerWeaponShot
#undef OnPlayerWeaponShot
#else
#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot BustAim_OnPlayerWeaponShot

#if defined BustAim_OnPlayerWeaponShot
	forward BustAim_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined BUSTAIM_DISABLE_WSTATS
stock BustAim::GetTeleportStats(playerid,Float:arr[],sz = sizeof(arr))
{
	#pragma unused playerid
	#pragma unused arr
	#pragma unused sz
    print("[BUST-AIM] GetTeleportStats must not be used when WSTATS is disabled.");
}
stock BustAim::GetRandomAimStats(playerid,Float:arr[],sz = sizeof(arr))
{
    #pragma unused playerid
	#pragma unused arr
	#pragma unused sz
    print("[BUST-AIM] GetAimStats must not be used when WSTATS is disabled.");
}
stock BustAim::GetRangeStats(playerid,Float:arr[],sz = sizeof(arr))
{
    #pragma unused playerid
	#pragma unused arr
	#pragma unused sz
    print("[BUST-AIM] GetRangeStats must not be used when WSTATS is disabled.");
}
#else
stock BustAim::GetTeleportStats(playerid,Float:arr[],sz = sizeof(arr))
{
	memcpy(_:arr,_:BustAim_g_TeleportWarningStats[playerid],0,sz*4,sz);
}
stock BustAim::GetRandomAimStats(playerid,Float:arr[],sz = sizeof(arr))
{
    memcpy(_:arr,_:BustAim_g_AimWarningStats[playerid],0,sz*4,sz);
}
stock BustAim::GetRangeStats(playerid,Float:arr[],sz = sizeof(arr))
{
    memcpy(_:arr,_:BustAim_g_RangeWarningStats[playerid],0,sz*4,sz);
}
#endif

#if defined BUSTAIM_DISABLE_PROFILING
stock BustAim::GetPlayerWeaponProfile(playerid,weaponid,&allshots,&hitshots,&max_cont_shots,&out_of_range_warns,&random_aim_warns,&proaim_tele_warns,&backward_shot_warns)
{
    #pragma unused playerid
	#pragma unused weaponid
	print("[BUST-AIM] GetPlayerWeaponProfile must not be used when profiling is disabled.");
}
stock BustAim::ResetPlayerWeaponProfile(playerid,weaponid)
{
    #pragma unused playerid
	#pragma unused weaponid
    print("[BUST-AIM] ResetWeaponProfile must not be used when profiling is disabled");
}
stock BustAim::GetPlayerProfile(playerid,&shotsfired,&shotshit,&max_cont_shots,&out_of_range_warns,&random_aim_warns,&proaim_tele_warns,&backward_shot_warns)
{
    #pragma unused playerid
    print("[BUST-AIM] GetPlayerProfile must not be used when profiling is disabled.");
}
stock BustAim::ResetPlayerProfile(playerid)
{
    #pragma unused playerid
    print("[BUST-AIM] ResetPlayerProfile must not be used when profiling is disabled.");
}
#else
stock BustAim::GetPlayerWeaponProfile(playerid,weaponid,&allshots,&hitshots,&max_cont_shots,&out_of_range_warns,&random_aim_warns,&proaim_tele_warns,&backward_shot_warns)
{
	if(22 < weaponid < 39)
	{
		weaponid -= 22;
		allshots = BustAim_g_ShotsFired[playerid][weaponid];
		hitshots = BustAim_g_ShotsHit[playerid][weaponid];
		max_cont_shots = BustAim_g_MaxContinousShots[playerid][weaponid];
		out_of_range_warns = BustAim_g_TotalOutOfRangeWarns[playerid][weaponid];
	    random_aim_warns = BustAim_g_TotalRandomAimWarns[playerid][weaponid];
		proaim_tele_warns = BustAim_g_TotalTeleportWarns[playerid][weaponid];
		backward_shot_warns = BustAim_g_TotalBackwardWarns[playerid][weaponid];
		return 0;
	}
	return 1;
}
stock BustAim::ResetPlayerWeaponProfile(playerid,weaponid)
{
    if(22 < weaponid < 39)
	{
		weaponid -= 22;
		BustAim_g_ShotsFired[playerid][weaponid] =
		BustAim_g_ShotsHit[playerid][weaponid] =
		BustAim_g_MaxContinousShots[playerid][weaponid] =
		BustAim_g_TotalOutOfRangeWarns[playerid][weaponid] =
	    BustAim_g_TotalRandomAimWarns[playerid][weaponid] =
		BustAim_g_TotalTeleportWarns[playerid][weaponid] =
		BustAim_g_TotalBackwardWarns[playerid][weaponid] = 0;
		return 0;
	}
	return 1;
}
stock BustAim::GetPlayerProfile(playerid,&shotsfired,&shotshit,&max_cont_shots,&out_of_range_warns,&random_aim_warns,&proaim_tele_warns,&backward_shot_warns)
{
	for(new i = 0;i < BS_TOTAL_SHOOTING_WEAPONS;i++)
	{
	    shotsfired += BustAim_g_ShotsFired[playerid][i];
	    shotshit += BustAim_g_ShotsHit[playerid][i];
	    out_of_range_warns += BustAim_g_TotalOutOfRangeWarns[playerid][i];
	    random_aim_warns += BustAim_g_TotalRandomAimWarns[playerid][i];
	    proaim_tele_warns+= BustAim_g_TotalTeleportWarns[playerid][i];
	    backward_shot_warns += BustAim_g_TotalBackwardWarns[playerid][i];
	    if(BustAim_g_MaxContinousShots[playerid][i] > max_cont_shots)
	    	max_cont_shots = BustAim_g_MaxContinousShots[playerid][i];
	}
}
stock BustAim::ResetPlayerProfile(playerid)
{
    Fill68(BustAim_g_TotalRandomAimWarns[playerid]);
	Fill68(BustAim_g_TotalTeleportWarns[playerid]);
	Fill68(BustAim_g_TotalBackwardWarns[playerid]);
	Fill68(BustAim_g_TotalOutOfRangeWarns[playerid]);
	Fill68(BustAim_g_ShotsFired[playerid]);
	Fill68(BustAim_g_ShotsHit[playerid]);
	Fill68(BustAim_g_MaxContinousShots[playerid]);
}
#endif
stock BustAim::SetPlayerFlags(playerid,flags)
{
    BustAim_g_PlayerSettings{playerid} = flags;
}
stock BustAim::GetPlayerFlags(playerid,&flags)
{
    flags = BustAim_g_PlayerSettings{playerid};
}
stock BustAim::ResetPlayerFlags(playerid)
{
    BustAim_g_PlayerSettings{playerid} = BUSTAIM_DEFAULT_PLAYER_FLAGS;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static stock Fill68(loc[])
{
	new val = 0;
  	#emit LOAD.S.alt loc
	#emit LOAD.S.pri val
	#emit FILL 68
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public OnPlayerConnect(playerid)
{
    BustAim_g_PlayerSettings{playerid} = BUSTAIM_DEFAULT_PLAYER_FLAGS;
	
    BustAim_g_ContinousShots{playerid} =
	BustAim_g_RandomAimShots{playerid} =
	BustAim_g_TeleportShots{playerid} =
	BustAim_g_OutOfRangeShots{playerid} =
	BustAim_g_BackwardShot{playerid} = 0;
	#if !defined BUSTAIM_DISABLE_PROFILING
		Fill68(BustAim_g_TotalRandomAimWarns[playerid]);
		Fill68(BustAim_g_TotalTeleportWarns[playerid]);
		Fill68(BustAim_g_TotalBackwardWarns[playerid]);
		Fill68(BustAim_g_TotalOutOfRangeWarns[playerid]);
		Fill68(BustAim_g_ShotsFired[playerid]);
		Fill68(BustAim_g_ShotsHit[playerid]);
		Fill68(BustAim_g_MaxContinousShots[playerid]);
	#endif

	#if defined BustAim_OnPlayerConnect
		return BustAim_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
#undef OnPlayerConnect
#else
#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect BustAim_OnPlayerConnect

#if defined BustAim_OnPlayerConnect
	forward BustAim_OnPlayerConnect(playerid);
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////