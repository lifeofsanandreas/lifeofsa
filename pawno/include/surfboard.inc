 
// Definitions
#define SOUND_BAR1 1083 // Sound for bar +
#define SOUND_BAR2 1084 // Sound for bar -
 
 
 
// Variables
new Text:Skillb1;
new Text:Skillb2;
new Text:SkillbText1;
new Text:SkillbText2;
new PlayerText:SkillbBar1[MAX_PLAYERS];
new PlayerText:SkillbBar2[MAX_PLAYERS];
new PlayerText:SkillbText[MAX_PLAYERS];
new delaysk[MAX_PLAYERS];
 
 
 
// Publics
forward hidebar(playerid);
public hidebar(playerid)
{
        TextDrawHideForPlayer(playerid, Skillb1);
        TextDrawHideForPlayer(playerid, Skillb2);
        TextDrawHideForPlayer(playerid, SkillbText1);
        TextDrawHideForPlayer(playerid, SkillbText2);
 
        PlayerTextDrawDestroy(playerid, SkillbBar1[playerid]);
        PlayerTextDrawDestroy(playerid, SkillbBar2[playerid]);
        PlayerTextDrawDestroy(playerid, SkillbText[playerid]);
       
        delaysk[playerid] = 0;
}
 
 
 
// Stocks
stock SkillBarInit()
{
        Skillb1 = TextDrawCreate(184.000000, 117.000000, "     ");
        TextDrawBackgroundColor(Skillb1, 255);
        TextDrawFont(Skillb1, 1);
        TextDrawLetterSize(Skillb1, 0.139999, 0.599998);
        TextDrawColor(Skillb1, -1);
        TextDrawSetOutline(Skillb1, 0);
        TextDrawSetProportional(Skillb1, 1);
        TextDrawSetShadow(Skillb1, 1);
        TextDrawUseBox(Skillb1, 1);
        TextDrawBoxColor(Skillb1, 120);
        TextDrawTextSize(Skillb1, 17.000000, -6.000000);
        TextDrawSetSelectable(Skillb1, 0);
 
        Skillb2 = TextDrawCreate(154.000000, 122.000000, "  ");
        TextDrawBackgroundColor(Skillb2, -2105376001);
        TextDrawFont(Skillb2, 0);
        TextDrawLetterSize(Skillb2, 0.879998, 1.199999);
        TextDrawColor(Skillb2, 1583309567);
        TextDrawSetOutline(Skillb2, 0);
        TextDrawSetProportional(Skillb2, 1);
        TextDrawSetShadow(Skillb2, 0);
        TextDrawUseBox(Skillb2, 1);
        TextDrawBoxColor(Skillb2, 1684301055);
        TextDrawTextSize(Skillb2, 91.000000, 0.000000);
        TextDrawSetSelectable(Skillb2, 0);
       
        SkillbText1 = TextDrawCreate(28.000000, 118.000000, "                        +");
        TextDrawBackgroundColor(SkillbText1, 336860320);
        TextDrawFont(SkillbText1, 1);
        TextDrawLetterSize(SkillbText1, 0.359997, 1.899999);
        TextDrawColor(SkillbText1, -286331152);
        TextDrawSetOutline(SkillbText1, 0);
        TextDrawSetProportional(SkillbText1, 1);
        TextDrawSetShadow(SkillbText1, 1);
        TextDrawSetSelectable(SkillbText1, 0);
       
        SkillbText2 = TextDrawCreate(28.000000, 118.000000, "                        -");
        TextDrawBackgroundColor(SkillbText2, 336860320);
        TextDrawFont(SkillbText2, 1);
        TextDrawLetterSize(SkillbText2, 0.359997, 1.899999);
        TextDrawColor(SkillbText2, -286331152);
        TextDrawSetOutline(SkillbText2, 0);
        TextDrawSetProportional(SkillbText2, 1);
        TextDrawSetShadow(SkillbText2, 1);
        TextDrawSetSelectable(SkillbText2, 0);
}
 
stock SkillBarExit()
{
        TextDrawDestroy(Skillb1);
        TextDrawDestroy(Skillb2);
        TextDrawDestroy(SkillbText1);
        TextDrawDestroy(SkillbText2);
}
 
stock SetSkillForPlayer(playerid, text[], timeaa, newval, oldval)
{
        if(delaysk[playerid]) { return 0; } else if (strval(text) > 9) { return 0; }
       
        if (newval > oldval)
        { // + bar
                SkillbBar2[playerid] = CreatePlayerTextDraw(playerid, (0.59*newval)+95.0, 122.000000, "  ");
                PlayerTextDrawBackgroundColor(playerid, SkillbBar2[playerid], -2105376001);
                PlayerTextDrawFont(playerid, SkillbBar2[playerid], 0);
                PlayerTextDrawLetterSize(playerid, SkillbBar2[playerid], 0.879998, 1.199998);
                PlayerTextDrawColor(playerid, SkillbBar2[playerid], 1583309567);
                PlayerTextDrawSetOutline(playerid, SkillbBar2[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbBar2[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbBar2[playerid], 0);
                PlayerTextDrawUseBox(playerid, SkillbBar2[playerid], 1);
                PlayerTextDrawBoxColor(playerid, SkillbBar2[playerid], 1366837759);
                PlayerTextDrawTextSize(playerid, SkillbBar2[playerid], 91.000000, 0.000000);
                PlayerTextDrawSetSelectable(playerid, SkillbBar2[playerid], 0);
 
                SkillbBar1[playerid] = CreatePlayerTextDraw(playerid, (0.59*oldval)+95.0, 122.000000, "  ");
                PlayerTextDrawBackgroundColor(playerid, SkillbBar1[playerid], -2105376001);
                PlayerTextDrawFont(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawLetterSize(playerid, SkillbBar1[playerid], 0.879998, 1.199998);
                PlayerTextDrawColor(playerid, SkillbBar1[playerid], 1583309567);
                PlayerTextDrawSetOutline(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbBar1[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawUseBox(playerid, SkillbBar1[playerid], 1);
                PlayerTextDrawBoxColor(playerid, SkillbBar1[playerid], -1128481557);
                PlayerTextDrawTextSize(playerid, SkillbBar1[playerid], 91.000000, 0.000000);
                PlayerTextDrawSetSelectable(playerid, SkillbBar1[playerid], 0);
               
                SkillbText[playerid] = CreatePlayerTextDraw(playerid, 28.000000, 118.000000, text);
                PlayerTextDrawBackgroundColor(playerid, SkillbText[playerid], 336860320);
                PlayerTextDrawFont(playerid, SkillbText[playerid], 1);
                PlayerTextDrawLetterSize(playerid, SkillbText[playerid], 0.359997, 1.899999);
                PlayerTextDrawColor(playerid, SkillbText[playerid], -286331152);
                PlayerTextDrawSetOutline(playerid, SkillbText[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbText[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbText[playerid], 1);
                PlayerTextDrawSetSelectable(playerid, SkillbText[playerid], 0);
               
                TextDrawShowForPlayer(playerid, SkillbText1);
                PlayerPlaySound(playerid, SOUND_BAR1, 0.0, 0.0, 0.0);
        } else if (newval < oldval) { // - bar
                SkillbBar2[playerid] = CreatePlayerTextDraw(playerid, (0.59*oldval)+95.0, 122.000000, "  ");
                PlayerTextDrawBackgroundColor(playerid, SkillbBar2[playerid], -2105376001);
                PlayerTextDrawFont(playerid, SkillbBar2[playerid], 0);
                PlayerTextDrawLetterSize(playerid, SkillbBar2[playerid], 0.879998, 1.199998);
                PlayerTextDrawColor(playerid, SkillbBar2[playerid], 1583309567);
                PlayerTextDrawSetOutline(playerid, SkillbBar2[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbBar2[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbBar2[playerid], 0);
                PlayerTextDrawUseBox(playerid, SkillbBar2[playerid], 1);
                PlayerTextDrawBoxColor(playerid, SkillbBar2[playerid], -1271119873);
                PlayerTextDrawTextSize(playerid, SkillbBar2[playerid], 91.000000, 0.000000);
                PlayerTextDrawSetSelectable(playerid, SkillbBar2[playerid], 0);
 
                SkillbBar1[playerid] = CreatePlayerTextDraw(playerid, (0.59*newval)+95.0, 122.000000, "  ");
                PlayerTextDrawBackgroundColor(playerid, SkillbBar1[playerid], -2105376001);
                PlayerTextDrawFont(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawLetterSize(playerid, SkillbBar1[playerid], 0.879998, 1.199998);
                PlayerTextDrawColor(playerid, SkillbBar1[playerid], 1583309567);
                PlayerTextDrawSetOutline(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbBar1[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawUseBox(playerid, SkillbBar1[playerid], 1);
                PlayerTextDrawBoxColor(playerid, SkillbBar1[playerid], -1128481557);
                PlayerTextDrawTextSize(playerid, SkillbBar1[playerid], 91.000000, 0.000000);
                PlayerTextDrawSetSelectable(playerid, SkillbBar1[playerid], 0);
               
                SkillbText[playerid] = CreatePlayerTextDraw(playerid, 28.000000, 118.000000, text);
                PlayerTextDrawBackgroundColor(playerid, SkillbText[playerid], 336860320);
                PlayerTextDrawFont(playerid, SkillbText[playerid], 1);
                PlayerTextDrawLetterSize(playerid, SkillbText[playerid], 0.359997, 1.899999);
                PlayerTextDrawColor(playerid, SkillbText[playerid], -286331152);
                PlayerTextDrawSetOutline(playerid, SkillbText[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbText[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbText[playerid], 1);
                PlayerTextDrawSetSelectable(playerid, SkillbText[playerid], 0);
 
                TextDrawShowForPlayer(playerid, SkillbText2);
                PlayerPlaySound(playerid, SOUND_BAR2, 0.0, 0.0, 0.0);
        } else {
                SkillbBar1[playerid] = CreatePlayerTextDraw(playerid, (0.59*oldval)+95.0, 122.000000, "  ");
                PlayerTextDrawBackgroundColor(playerid, SkillbBar1[playerid], -2105376001);
                PlayerTextDrawFont(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawLetterSize(playerid, SkillbBar1[playerid], 0.879998, 1.199998);
                PlayerTextDrawColor(playerid, SkillbBar1[playerid], 1583309567);
                PlayerTextDrawSetOutline(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbBar1[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbBar1[playerid], 0);
                PlayerTextDrawUseBox(playerid, SkillbBar1[playerid], 1);
                PlayerTextDrawBoxColor(playerid, SkillbBar1[playerid], -1128481557);
                PlayerTextDrawTextSize(playerid, SkillbBar1[playerid], 91.000000, 0.000000);
                PlayerTextDrawSetSelectable(playerid, SkillbBar1[playerid], 0);
 
                SkillbText[playerid] = CreatePlayerTextDraw(playerid, 28.000000, 118.000000, text);
                PlayerTextDrawBackgroundColor(playerid, SkillbText[playerid], 336860320);
                PlayerTextDrawFont(playerid, SkillbText[playerid], 1);
                PlayerTextDrawLetterSize(playerid, SkillbText[playerid], 0.359997, 1.899999);
                PlayerTextDrawColor(playerid, SkillbText[playerid], -286331152);
                PlayerTextDrawSetOutline(playerid, SkillbText[playerid], 0);
                PlayerTextDrawSetProportional(playerid, SkillbText[playerid], 1);
                PlayerTextDrawSetShadow(playerid, SkillbText[playerid], 1);
                PlayerTextDrawSetSelectable(playerid, SkillbText[playerid], 0);
        }
       
        TextDrawShowForPlayer(playerid, Skillb1);
        TextDrawShowForPlayer(playerid, Skillb2);
 
        PlayerTextDrawShow(playerid, SkillbBar2[playerid]);
        PlayerTextDrawShow(playerid, SkillbBar1[playerid]);
        PlayerTextDrawShow(playerid, SkillbText[playerid]);
       
        SetTimerEx("hidebar", timeaa, false, "i", playerid);
        delaysk[playerid] = 1;
       
        return 1;
}
 
 
 
//Callbacks
public OnGameModeInit()
{
    #if defined SK_OnGameModeInit
        SK_OnGameModeInit();
    #endif
   
    SkillBarInit();
}
 
public OnGameModeExit()
{
    #if defined SK_OnGameModeExit
        SK_OnGameModeExit();
    #endif
 
    SkillBarExit();
}
 
 
 
// Hooks
#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit SK_OnGameModeInit
#if defined SK_OnGameModeInit
    forward SK_OnGameModeInit();
#endif
 
#if defined _ALS_OnGameModeExit
    #undef OnGameModeExit
#else
    #define _ALS_OnGameModeExit
#endif
#define OnGameModeExit SK_OnGameModeExit
#if defined SK_OnGameModeExit
    forward SK_OnGameModeExit();
#endif
