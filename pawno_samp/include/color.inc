/*
	@Release Name: GetVehicleColor
	@Release Type: Include
	@Author: RyDeR`
	@Version: 1.1
	@Bugs: /
	@Last Update: 13/09/2010 - 20:15
	@Credtis: Y_Less - For the method to not replace the function names.
	@Auhor Notes: /
	@Functions:		
			@Function: GetVehicleColor(vehicleid, &color1, &color2)
			@Info: Will store the vehicle colors in variable color1 and color2 you create.
			@Params:
				- vehicleid = The ID of the vehicle
				- &color1 = The variable you create to store color1 in.
				- &color2 = Same here as above but for color2.
*/

new
	vehicleColor[MAX_VEHICLES][2]
;

native r_AddStaticVehicle(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2) = AddStaticVehicle;
native r_AddStaticVehicleEx(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2, respawn_delay) = AddStaticVehicleEx;
native r_CreateVehicle(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2, respawn_delay) = CreateVehicle;
native r_DestroyVehicle(vehicleid) = DestroyVehicle;
native r_ChangeVehicleColor(vehicleid, color1, color2) = ChangeVehicleColor;

#define AddStaticVehicle \
	addStaticVehicle

#define AddStaticVehicleEx \
	addStaticVehicleEx

#define CreateVehicle \
	createVehicle

#define DestroyVehicle \
	destroyVehicle

#define ChangeVehicleColor \
	changeVehicleColor

stock addStaticVehicle(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2)
{
	new
		vehicleid = r_AddStaticVehicle(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2)
	;
	return (vehicleColor[vehicleid][0] = color1), (vehicleColor[vehicleid][1] = color2);
}


stock addStaticVehicleEx(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2, respawn_delay)
{
	new
		vehicleid = r_AddStaticVehicleEx(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2, respawn_delay)
	;
	return (vehicleColor[vehicleid][0] = color1), (vehicleColor[vehicleid][1] = color2);
}

stock createVehicle(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2, respawn_delay)
{
	new
		vehicleid = r_CreateVehicle(modelid, Float: x, Float: y, Float: z, Float: angle, color1, color2, respawn_delay)
	;
	return (vehicleColor[vehicleid][0] = color1), (vehicleColor[vehicleid][1] = color2);
}

stock destroyVehicle(vehicleid)
{
	r_DestroyVehicle(vehicleid);
	return (vehicleColor[vehicleid][0] = 0), (vehicleColor[vehicleid][1] = 0);
}

stock changeVehicleColor(vehicleid, color1, color2)
{
	r_ChangeVehicleColor(vehicleid, color1, color2);
	return (vehicleColor[vehicleid][0] = color1), (vehicleColor[vehicleid][1] = color2);
}

stock r_OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	#pragma unused playerid
	return (vehicleColor[vehicleid][0] = color1), (vehicleColor[vehicleid][1] = color2);
}

stock GetVehicleColor(vehicleid, &color1, &color2)
{
	return (color1 = vehicleColor[vehicleid][0]), (color2 = vehicleColor[vehicleid][1]);
}