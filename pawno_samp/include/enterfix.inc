/*

    About: Fix for missing animation when enter locked vehicle as driver
    Author: ziggi

*/

#if !defined _samp_included
    #error Include a_samp.inc first
#endif


enum e_Player_Data {
	bool:e_IsOpening,
	bool:e_IsTrying,
	e_VehicleID,
	e_TickCount,
}

static
	player_data[MAX_PLAYERS][e_Player_Data],
	bool:is_locked[MAX_VEHICLES];



/* 
 * 
 * ENTER_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
 * 
 */

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if (is_locked[vehicleid]) {
		new engine, lights, alarm, doors, bonnet, boot, objective;
		GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
		SetVehicleParamsEx(vehicleid, engine, lights, alarm, VEHICLE_PARAMS_ON, bonnet, boot, objective);

		player_data[playerid][e_IsOpening] = true;
		player_data[playerid][e_VehicleID] = vehicleid;
		player_data[playerid][e_TickCount] = GetTickCount();
	}

#if defined ENTER_OnPlayerEnterVehicle
	return ENTER_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
#else
	return 1;
#endif
}

#if defined _ALS_OnPlayerEnterVehicle
	#error _ALS_OnPlayerEnterVehicle defined
#endif
#define _ALS_OnPlayerEnterVehicle

#define OnPlayerEnterVehicle ENTER_OnPlayerEnterVehicle

#if defined ENTER_OnPlayerEnterVehicle
	forward ENTER_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
#endif



/* 
 * 
 * ENTER_OnPlayerUpdate(playerid)
 * 
 */

public OnPlayerUpdate(playerid)
{
	if (player_data[playerid][e_IsOpening]) {
		if (GetTickCount() >= player_data[playerid][e_TickCount] + 4000) {
			ENTER_Stop(playerid);
		} else {
			if (1020 <= GetPlayerAnimationIndex(playerid) <= 1021) {
				player_data[playerid][e_IsTrying] = true;
			} else if (player_data[playerid][e_IsTrying]) {
				ENTER_Stop(playerid);
			}
		}
	}

#if defined ENTER_OnPlayerUpdate
	return ENTER_OnPlayerUpdate(playerid);
#else
	return 1;
#endif
}

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate ENTER_OnPlayerUpdate

#if defined ENTER_OnPlayerUpdate
	forward ENTER_OnPlayerUpdate(playerid);
#endif



/* 
 * 
 * ENTER_Stop(playerid)
 * 
 */

stock ENTER_Stop(playerid)
{
	player_data[playerid][e_IsTrying] = false;
	player_data[playerid][e_IsOpening] = false;

	new vehicleid = player_data[playerid][e_VehicleID],
		engine, lights, alarm, doors, bonnet, boot, objective;

	GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vehicleid, engine, lights, alarm, VEHICLE_PARAMS_OFF, bonnet, boot, objective);

	ClearAnimations(playerid);
}



/* 
 * 
 * ENTER_SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective)
 * 
 */

stock ENTER_SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective)
{
	is_locked[vehicleid] = doors == VEHICLE_PARAMS_ON ? true : false;
	SetVehicleParamsEx(vehicleid, engine, lights, alarm, VEHICLE_PARAMS_OFF, bonnet, boot, objective);
}

#if defined _ALS_SetVehicleParamsEx
	#undef SetVehicleParamsEx
#else
	#define _ALS_SetVehicleParamsEx
#endif

#define SetVehicleParamsEx ENTER_SetVehicleParamsEx



/* 
 * 
 * ENTER_GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective)
 * 
 */

stock ENTER_GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective)
{
	GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
	doors = is_locked[vehicleid] ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF;
}

#if defined _ALS_GetVehicleParamsEx
	#undef GetVehicleParamsEx
#else
	#define _ALS_GetVehicleParamsEx
#endif

#define GetVehicleParamsEx ENTER_GetVehicleParamsEx
