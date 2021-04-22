//3DMenu. Author: SDraw
//Original posts are on forum.sa-mp.com, pawno.ru
/* Fake natives. Thanks to TheArcher.
native Create3DMenu(Float:x,Float:y,Float:z,Float:rotation,boxes,playerid);
native SetBoxText(MenuID,box,text[],materialsize,fontface[],fontsize,bold,fontcolor,selectcolor,unselectcolor,textalignment);
native Select3DMenu(playerid,MenuID);
native CancelSelect3DMenu(playerid);
native Destroy3DMenu(MenuID);
*/

#define MAX_3DMENUS (MAX_PLAYERS)
#define MAX_BOXES (16)

new SelectedMenu[MAX_PLAYERS];
new SelectedBox[MAX_PLAYERS];
enum MenuParams { Float:Rotation,Boxes,bool:IsExist,
	Objects[MAX_BOXES],Float:OrigPosX[MAX_BOXES],Float:OrigPosY[MAX_BOXES],Float:OrigPosZ[MAX_BOXES],
	Float:AddingX,Float:AddingY,SelectColor[MAX_BOXES],UnselectColor[MAX_BOXES],Player };
new MenuInfo[MAX_3DMENUS][MenuParams];

//Callbacks
forward OnPlayerSelect3DMenuBox(playerid,MenuID,boxid);
forward OnPlayerChange3DMenuBox(playerid,MenuID,boxid);

stock Create3DMenu(Float:x,Float:y,Float:z,Float:rotation,boxes,playerid)
{
	if(boxes > MAX_BOXES || boxes <= 0 || playerid < 0 || !IsPlayerConnected(playerid)) return -1;
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
	    if(MenuInfo[i][IsExist]) continue;
     	new Float:NextLineX,Float:NextLineY;
      	new lineindx,binc;
       	MenuInfo[i][Rotation] = rotation;
		MenuInfo[i][Boxes] = boxes;
		MenuInfo[i][AddingX] = 0.25*floatsin(rotation,degrees);
		MenuInfo[i][AddingY] = -floatcos(rotation,degrees)*0.25;
		NextLineX = floatcos(rotation,degrees)+0.05*floatcos(rotation,degrees);
		NextLineY = floatsin(rotation,degrees)+0.05*floatsin(rotation,degrees);
		for(new b = 0; b < boxes; b++)
		{
  			if(b%4 == 0 && b != 0) lineindx++,binc+=4;
   			MenuInfo[i][Objects][b] = CreateDynamicObject(2661,x+NextLineX*lineindx,y+NextLineY*lineindx,z+1.65-0.55*(b-binc),0,0,rotation,-1,-1,playerid,100.0);
      		GetDynamicObjectPos(MenuInfo[i][Objects][b],MenuInfo[i][OrigPosX][b],MenuInfo[i][OrigPosY][b],MenuInfo[i][OrigPosZ][b]);
		}
		MenuInfo[i][IsExist] = true;
		MenuInfo[i][Player] = playerid;
		Streamer_Update(playerid);
		return i;
	}
	return -1;
}

stock SetBoxText(MenuID,box,text[],materialsize,fontface[],fontsize,bold,fontcolor,selectcolor,unselectcolor,textalignment)
{
	if(!MenuInfo[MenuID][IsExist]) return -1;
	if(box == MenuInfo[MenuID][Boxes] || box < 0) return -1;
	if(MenuInfo[MenuID][Objects][box] == INVALID_OBJECT_ID) return -1;
	MenuInfo[MenuID][SelectColor][box] = selectcolor;
	MenuInfo[MenuID][UnselectColor][box] = unselectcolor;
	if(SelectedBox[MenuInfo[MenuID][Player]] == box) SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][box],0,text,materialsize,fontface,fontsize,bold,fontcolor,selectcolor,textalignment);
	else SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][box],0,text,materialsize,fontface,fontsize,bold,fontcolor,unselectcolor,textalignment);
	return 1;
}

stock Select3DMenu(playerid,MenuID)
{
	if(!IsPlayerConnected(playerid)) return -1;
	if(!MenuInfo[MenuID][IsExist]) return -1;
	if(MenuInfo[MenuID][Player] != playerid) return -1;
	if(SelectedMenu[playerid] != -1) CancelSelect3DMenu(playerid);
	SelectedBox[playerid] = 0;
	new text[128],materialsize,fontface[128],fontsize,bold,fontcolor,backcolor,textalignment;
	GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][0],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
	SelectedMenu[playerid] = MenuID;
 	MoveDynamicObject(MenuInfo[MenuID][Objects][0],MenuInfo[MenuID][OrigPosX][0]+MenuInfo[MenuID][AddingX],MenuInfo[MenuID][OrigPosY][0]+MenuInfo[MenuID][AddingY],MenuInfo[MenuID][OrigPosZ][0],1.0);
 	SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][0],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][SelectColor][0],textalignment);
	return 1;
}

#if defined FILTERSCRIPT
public OnFilterScriptInit()
{
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
 		SelectedMenu[i] = -1;
		SelectedBox[i] = -1;
	}
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
	    	for(new b = 0; b < MAX_BOXES; b++) MenuInfo[i][Objects][b] = INVALID_OBJECT_ID;
     		MenuInfo[i][Boxes] = 0;
	 	MenuInfo[i][IsExist] = false;
	 	MenuInfo[i][AddingX] = 0.0;
 		MenuInfo[i][AddingY] = 0.0;
 		MenuInfo[i][Player] = -1;
	}
 	if(funcidx("menu3d_OnFilterScriptInit") != -1) return CallLocalFunction("menu3d_OnFilterScriptInit", "");
	return 1;
}
#if defined _ALS_OnFilterScriptInit
    #undef OnFilterScriptInit
#else
    #define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit menu3d_OnFilterScriptInit
forward menu3d_OnFilterScriptInit();

public OnFilterScriptExit()
{
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
		if(MenuInfo[i][IsExist]) Destroy3DMenu(i);
	}
	if(funcidx("menu3d_OnFilterScriptExit") != -1) return CallLocalFunction("menu3d_OnFilterScriptExit", "");
	return 1;
}
#if defined _ALS_OnFilterScriptExit
    #undef OnFilterScriptExit
#else
    #define _ALS_OnFilterScriptExit
#endif
#define OnFilterScriptExit menu3d_OnFilterScriptExit
forward menu3d_OnFilterScriptExit();

#else
public OnGameModeInit()
{
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
	    for(new b = 0; b < MAX_BOXES; b++) MenuInfo[i][Objects][b] = INVALID_OBJECT_ID;
	    MenuInfo[i][Boxes] = 0;
	    MenuInfo[i][IsExist] = false;
	    MenuInfo[i][AddingX] = 0.0;
 	    MenuInfo[i][AddingY] = 0.0;
 	    MenuInfo[i][Player] = -1;
	}
	if(funcidx("menu3d_OnGameModeInit") != -1) return CallLocalFunction("menu3d_OnGameModeInit", "");
	return 1;
}
#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit menu3d_OnGameModeInit
forward menu3d_OnGameModeInit();

public OnGameModeExit()
{
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
		if(MenuInfo[i][IsExist]) Destroy3DMenu(i);
	}
	if(funcidx("menu3d_OnGameModeExit") != -1) return CallLocalFunction("menu3d_OnGameModeExit", "");
	return 1;
}
#if defined _ALS_OnGameModeExit
    #undef OnGameModeExit
#else
    #define _ALS_OnGameModeExit
#endif
#define OnGameModeExit menu3d_OnGameModeExit
forward menu3d_OnGameModeExit();

#endif
public OnPlayerConnect(playerid)
{
    SelectedMenu[playerid] = -1;
	SelectedBox[playerid] = -1;
	if(funcidx("menu3d_OnPlayerConnect") != -1) return CallLocalFunction("menu3d_OnPlayerConnect", "d",playerid);
	return 1;
}
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect menu3d_OnPlayerConnect
forward menu3d_OnPlayerConnect(playerid);

public OnPlayerDisconnect(playerid,reason)
{
    if(SelectedMenu[playerid] != -1) CancelSelect3DMenu(playerid);
    if(funcidx("menu3d_OnPlayerDisconnect") != -1) return CallLocalFunction("menu3d_OnPlayerDisconnect", "dd",playerid,reason);
    return 1;
}
#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect menu3d_OnPlayerDisconnect
forward menu3d_OnPlayerDisconnect(playerid, reason);

public OnPlayerKeyStateChange(playerid,newkeys,oldkeys)
{
	if(SelectedMenu[playerid] != -1)
	{
		new MenuID = SelectedMenu[playerid];
	    if(newkeys == KEY_NO)
	    {
	        new text[128],materialsize,fontface[32],fontsize,bold,fontcolor,backcolor,textalignment;
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][UnselectColor][SelectedBox[playerid]],textalignment);
			MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			SelectedBox[playerid]++;
			if(SelectedBox[playerid] == MenuInfo[MenuID][Boxes]) SelectedBox[playerid] = 0;
			MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]]+MenuInfo[MenuID][AddingX],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]]+MenuInfo[MenuID][AddingY],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][SelectColor][SelectedBox[playerid]],textalignment);
		}
		if(newkeys == KEY_YES)
	    {
	        new text[128],materialsize,fontface[32],fontsize,bold,fontcolor,backcolor,textalignment;
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][UnselectColor][SelectedBox[playerid]],textalignment);
	        MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			SelectedBox[playerid]--;
			if(SelectedBox[playerid] < 0) SelectedBox[playerid] = MenuInfo[MenuID][Boxes]-1;
			MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]]+MenuInfo[MenuID][AddingX],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]]+MenuInfo[MenuID][AddingY],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][SelectColor][SelectedBox[playerid]],textalignment);
		}
		if(newkeys == KEY_NO || newkeys == KEY_YES) if(funcidx("OnPlayerChange3DMenuBox") != -1) CallLocalFunction("OnPlayerChange3DMenuBox","ddd",playerid,MenuID,SelectedBox[playerid]);
		if(newkeys == KEY_SPRINT) if(funcidx("OnPlayerSelect3DMenuBox") != -1) CallLocalFunction("OnPlayerSelect3DMenuBox","ddd",playerid,MenuID,SelectedBox[playerid]);
	}
	if(funcidx("menu3d_OnPlayerKeyStateChange") != -1) return CallLocalFunction("menu3d_OnPlayerKeyStateChange", "ddd",playerid,newkeys,oldkeys);
	return 1;
}
#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange menu3d_OnPlayerKeyStateChange
forward menu3d_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);

stock CancelSelect3DMenu(playerid)
{
	if(!IsPlayerConnected(playerid)) return -1;
	if(SelectedMenu[playerid] == -1) return -1;
	new MenuID = SelectedMenu[playerid];
	new text[128],materialsize,fontface[32],fontsize,bold,fontcolor,backcolor,textalignment;
	GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
	SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][UnselectColor][SelectedBox[playerid]],textalignment);
	MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
	SelectedMenu[playerid] = -1;
	SelectedBox[playerid] = -1;
	return 1;
}

stock Destroy3DMenu(MenuID)
{
    if(!MenuInfo[MenuID][IsExist]) return -1;
    if(SelectedMenu[MenuInfo[MenuID][Player]] == MenuID) CancelSelect3DMenu(MenuInfo[MenuID][Player]);
    for(new i = 0; i < MenuInfo[MenuID][Boxes]; i++)
    {
		DestroyDynamicObject(MenuInfo[MenuID][Objects][i]);
		MenuInfo[MenuID][Objects][i] = INVALID_OBJECT_ID;
	}
 	MenuInfo[MenuID][Boxes] = 0;
 	MenuInfo[MenuID][IsExist] = false;
 	MenuInfo[MenuID][AddingX] = 0.0;
 	MenuInfo[MenuID][AddingY] = 0.0;
 	MenuInfo[MenuID][Player] = -1;
	return 1;
}
