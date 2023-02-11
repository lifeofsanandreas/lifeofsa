#if defined _antitb_included
	#endinput
#endif
#define _antitb_included
#include <a_samp>

static
	bool:PlayerAimed[ MAX_PLAYERS char ];
	
public OnPlayerUpdate(playerid)
{
    if(PlayerAimed{ playerid } == false)
	{
	    static
			cameramode, Keys[ 3 ];
		cameramode = GetPlayerCameraMode(playerid);
		GetPlayerKeys(playerid, Keys[ 0 ], Keys[ 1 ], Keys[ 2 ]);
	    if((cameramode == 53 || cameramode == 7) || Keys[ 0 ] == KEY_FIRE)
	    {
	        PlayerAimed{ playerid } = true;
	    }
	}
	#if defined hook_OnPlayerUpdate
 		hook_OnPlayerUpdate(playerid);
	#endif
	return 1;
}
public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(PlayerAimed{ playerid } == false)
	{
	    static
			bool:validweapon;
		switch(weaponid)
		{
		    case WEAPON_COLT45,
			WEAPON_SAWEDOFF,
			WEAPON_UZI, WEAPON_MP5, WEAPON_TEC9,
			WEAPON_AK47, WEAPON_M4,
			WEAPON_MOLTOV, WEAPON_FLAMETHROWER: validweapon = false;
			default: validweapon = true;
		}
		if(validweapon == true)
		{
		    CallRemoteFunction("OnPlayerTriggerbot", "i", playerid);
		}
		return 0;
	}
	else PlayerAimed{ playerid } = false;
	#if defined hook_OnPlayerWeaponShot
    	hook_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#endif
	return 1;
}
//- - - -
#if defined hook_OnPlayerWeaponShot
    forward hook_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

#if defined _ALS_OnPlayerWeaponShot
    #undef OnPlayerWeaponShot
#else
    #define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot hook_OnPlayerWeaponShot
//- - - -
#if defined hook_OnPlayerUpdate
    forward hook_OnPlayerUpdate(playerid);
#endif

#if defined _ALS_OnPlayerUpdate
    #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate hook_OnPlayerUpdate
//- - - -
forward OnPlayerTriggerbot(playerid);