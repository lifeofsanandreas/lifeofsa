						/////////////////////////////
						///Vehicle Object Streamer///
						//////   Created by:    /////
						//////    °Nanory °   ///////
						//////  Version 0.35  ///////
						/////////////////////////////

#include <a_samp>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_OBJECTS_PER_VEHICLE 10 /// With this Setting you decide how many Object a Vehicle can have!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Variablen//
new VehicleObjectModel[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];
new Float:VehicleObjectRotX[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];
new Float:VehicleObjectRotY[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];
new Float:VehicleObjectRotZ[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];
new Float:VehicleObjectPotX[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];
new Float:VehicleObjectPotY[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];
new Float:VehicleObjectPotZ[MAX_VEHICLES][MAX_OBJECTS_PER_VEHICLE];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
forward OnGameModeInit();
public OnGameModeInit()
{
	new x;
	new i;
	printf("[VOS] Vehicle Object Streamer ready to work.");
	for(x=0; x< MAX_VEHICLES; x++)
	{
     	for(i=0; i<MAX_OBJECTS_PER_VEHICLE; i++)
	    {
	    	VehicleObjectModel[x][i] = -1;
		}
	}
	return CallLocalFunction("VOS_OnGameModeInit","i",1);
}
forward VOS_OnGameModeInit();
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit VOS_OnGameModeInit

public OnVehicleStreamIn(vehicleid,forplayerid)
{
	new i;
	new str[128];
	for(i=0; i<MAX_OBJECTS_PER_VEHICLE; i++)
	{
		if(IsAVehicleObject(vehicleid,i))
		{
			format(str,sizeof(str),"PlayerVehObj_%d_%d",vehicleid,i);
			SetPVarInt(forplayerid,str,CreatePlayerObject(forplayerid,VehicleObjectModel[vehicleid][i],0,0,0,0,0,0));
			AttachPlayerObjectToVehicle(forplayerid,GetPVarInt(forplayerid,str),vehicleid,VehicleObjectPotX[vehicleid][i],VehicleObjectPotY[vehicleid][i],VehicleObjectPotZ[vehicleid][i],VehicleObjectRotX[vehicleid][i],VehicleObjectRotY[vehicleid][i],VehicleObjectRotZ[vehicleid][i]);
		}
	}
	return CallLocalFunction("VOS_OnVehicleStreamIn","ii",vehicleid,forplayerid);
}
forward VOS_OnVehicleStreamIn(vehicleid,forplayerid);
#if defined _ALS_OnVehicleStreamIn
	#undef OnVehicleStreamIn
#else
	#define _ALS_OnVehicleStreamIn
#endif
#define OnVehicleStreamIn VOS_OnVehicleStreamIn


public OnVehicleStreamOut(vehicleid,forplayerid)
{
	new x;
	new str[128];
	for(x=0; x<MAX_OBJECTS_PER_VEHICLE; x++)
	{
		format(str,sizeof(str),"PlayerVehObj_%d_%d",vehicleid,x);
		if(GetPVarType(forplayerid, str))
		{
			DestroyPlayerObject(forplayerid,GetPVarInt(forplayerid,str));
			DeletePVar(forplayerid,str);
		}
	}
	return CallLocalFunction("VOS_OnVehicleStreamOut","ii",vehicleid,forplayerid);
}
forward VOS_OnVehicleStreamOut(vehicleid,forplayerid);
#if defined _ALS_OnVehicleStreamOut
	#undef OnVehicleStreamOut
#else
	#define _ALS_OnVehicleStreamOut
#endif
#define OnVehicleStreamOut VOS_OnVehicleStreamOut*/

public OnVehicleDeath(vehicleid, killerid)
{
	RemoveVehicleObjects(vehicleid);
	return CallLocalFunction("VOS_OnVehicleDeath", "ii", vehicleid, killerid);
}
forward VOS_OnVehicleDeath(vehicleid, killerid);
#if defined _ALS_OnVehicleDeath
	#undef OnVehicleDeath
#else
	#define _ALS_OnVehicleDeath
#endif
#define OnVehicleDeath VOS_OnVehicleDeath

forward CreateVehicleObject(vehicleid,objectid,Float:ObjX,Float:ObjY,Float:ObjZ,Float:RotX,Float:RotY,Float:RotZ);
public CreateVehicleObject(vehicleid,objectid,Float:ObjX,Float:ObjY,Float:ObjZ,Float:RotX,Float:RotY,Float:RotZ)
{
	new str[128];
	new freeslot = GetVehicleFreeObjectSlot(vehicleid);
	if(freeslot >= 0)
	{
        VehicleObjectRotX[vehicleid][freeslot] = RotX;
		VehicleObjectRotY[vehicleid][freeslot] = RotY;
		VehicleObjectRotZ[vehicleid][freeslot] = RotZ;
		VehicleObjectPotX[vehicleid][freeslot] = ObjX;
		VehicleObjectPotY[vehicleid][freeslot] = ObjY;
		VehicleObjectPotZ[vehicleid][freeslot] = ObjZ;
		VehicleObjectModel[vehicleid][freeslot] = objectid;
		new i;
     	for(i=0; i<MAX_PLAYERS; i++)
	    {
			if(IsVehicleStreamedIn(vehicleid,i))
			{
				format(str,sizeof(str),"PlayerVehObj_%d_%d",vehicleid,freeslot);
				SetPVarInt(i,str,CreatePlayerObject(i,VehicleObjectModel[vehicleid][freeslot],0,0,0,0,0,0));
				AttachPlayerObjectToVehicle(i,GetPVarInt(i,str),vehicleid,VehicleObjectPotX[vehicleid][freeslot],VehicleObjectPotY[vehicleid][freeslot],VehicleObjectPotZ[vehicleid][freeslot],VehicleObjectRotX[vehicleid][freeslot],VehicleObjectRotY[vehicleid][freeslot],VehicleObjectRotZ[vehicleid][freeslot]);
			}
		}
		return freeslot;
	}
	return -1;
}

forward RemoveVehicleObject(vehicleid,slot);
public RemoveVehicleObject(vehicleid,slot)
{
	new i = slot;
	new str[128];
	if(IsAVehicleObject(vehicleid,i))
	{
		VehicleObjectRotX[vehicleid][i] = 0;
		VehicleObjectRotY[vehicleid][i] = 0;
		VehicleObjectRotZ[vehicleid][i] = 0;
		VehicleObjectPotX[vehicleid][i] = 0;
		VehicleObjectPotY[vehicleid][i] = 0;
		VehicleObjectPotZ[vehicleid][i] = 0;
		VehicleObjectModel[vehicleid][i] = -1;
		new x;
		for(x=0; x<MAX_PLAYERS; x++)
		{
			if(IsVehicleStreamedIn(vehicleid, x))
			{
				format(str,sizeof(str),"PlayerVehObj_%d_%d",vehicleid,i);
				if(GetPVarType(x, str))
				{
					DestroyPlayerObject(x,GetPVarInt(x,str));
					DeletePVar(x,str);
				}
			}
		}
	}
	return 1;
}

forward RemoveVehicleObjects(vehicleid);
public RemoveVehicleObjects(vehicleid)
{
	new i;
	new str[128];
    for(i=0; i<MAX_OBJECTS_PER_VEHICLE; i++)
	{
		if(IsAVehicleObject(vehicleid,i))
		{
		    VehicleObjectRotX[vehicleid][i] = 0;
			VehicleObjectRotY[vehicleid][i] = 0;
			VehicleObjectRotZ[vehicleid][i] = 0;
			VehicleObjectPotX[vehicleid][i] = 0;
			VehicleObjectPotY[vehicleid][i] = 0;
			VehicleObjectPotZ[vehicleid][i] = 0;
			VehicleObjectModel[vehicleid][i] = -1;
			new x;
			for(x=0; x<MAX_PLAYERS; x++)
			{
				if(IsVehicleStreamedIn(vehicleid, x))
				{
					format(str,sizeof(str),"PlayerVehObj_%d_%d",vehicleid,i);
					if(GetPVarType(x, str))
					{
						DestroyPlayerObject(x,GetPVarInt(x,str));
						DeletePVar(x,str);
					}
				}
			}
		}
	}
	return 1;
}

forward GetVehicleFreeObjectSlot(vehicleid);
public GetVehicleFreeObjectSlot(vehicleid)
{
	new i;
 	for(i=0; i<MAX_OBJECTS_PER_VEHICLE; i++){if(!IsAVehicleObject(vehicleid,i)){return i;}}
	return -1;
}

forward IsAVehicleObject(vehicleid,index);
public IsAVehicleObject(vehicleid,index)
{
 	if(VehicleObjectModel[vehicleid][index] != -1){return 1;}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
