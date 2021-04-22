//Global Textdraws:

new PlayerText:entlassung;
new PlayerText:entzeit;


entlassung = CreatePlayerTextDraw(playerid,270.000030, 389.262207, "Entlassung:");
PlayerTextDrawLetterSize(entlassung, 0.449999, 1.600000);
PlayerTextDrawAlignment(entlassung, 1);
PlayerTextDrawColor(entlassung, -2147483393);
PlayerTextDrawSetShadow(entlassung, 0);
PlayerTextDrawSetOutline(entlassung, 1);
PlayerTextDrawBackgroundColor(entlassung, 51);
PlayerTextDrawFont(entlassung, 3);
PlayerTextDrawSetProportional(entlassung, 1);

Textdraw1 = TextDrawCreate(369.200042, 389.760009, "30 Sek");
TextDrawLetterSize(Textdraw1, 0.449999, 1.600000);
TextDrawAlignment(Textdraw1, 1);
TextDrawColor(Textdraw1, -1);
TextDrawSetShadow(Textdraw1, 0);
TextDrawSetOutline(Textdraw1, 1);
TextDrawBackgroundColor(Textdraw1, 51);
TextDrawFont(Textdraw1, 0);
TextDrawSetProportional(Textdraw1, 1);

