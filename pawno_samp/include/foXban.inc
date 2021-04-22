	    ////-/////-/////-/////-/////-/////-/////-/////-/////-/////-/////-/////-/////-//
	   //						 	 	FB_TimeBan-System								//
	  //								    foXban  		 					   //
	 //					       COPYRIGHT BY FOXHOUND / PSPGAMER					  //
	////-/////-/////-/////-/////-/////-/////-/////-/////-/////-/////-/////-/////-//

//------< INCLUDEs >------//
	#include <a_samp>
//------------------------//

//-------------< DEFINEs >--------------//
	#define FB_foxBanFile "Bans/%s.tban"
	#define MAX_STRING 255
//--------------------------------------//

//---------------< FORWARDs >---------------//
	forward FB_OnPlayerConnect(playerid);
//------------------------------------------//

//-------< NEWs >-------//
	new FB_tbDir[64];
//----------------------//

//----------------------------------------------------------< OnPlayerConnect >-----------------------------------------------------------//
	public OnPlayerConnect(playerid)
	{
		new pName[MAX_PLAYER_NAME];
		GetPlayerName(playerid,pName,sizeof(pName));
		format(FB_tbDir,sizeof(FB_tbDir),FB_foxBanFile,pName);
		if(FB_dini_Exists(FB_tbDir))
		{
			if(FB_dini_Int(FB_tbDir,"fbFB_Time")+FB_dini_Int(FB_tbDir,"fbDuration") > FB_Time())
			{
				new string[96];
				new sekki=(FB_dini_Int(FB_tbDir,"fbFB_Time")+FB_dini_Int(FB_tbDir,"fbDuration"))-FB_Time(),minni=sekki/60;
				sekki=sekki-minni*60;
				format(string,sizeof(string),"You are timebanned from this server. Remaining time: %d minutes %d seconds",minni,sekki);
				SendClientMessage(playerid,0xADC7E7FF,string);
				Kick(playerid);
			} else { FB_dini_Remove(FB_tbDir); }
		}
		return CallLocalFunction("FB_OnPlayerConnect","i",playerid);
	}
//----------------------------------------------------------------------------------------------------------------------------------------//

//------------------------< TBAN >------------------------//
	forward TBan(pID,bFB_Time);
	public TBan(pID,bFB_Time)
	{
		new pName[MAX_PLAYER_NAME],string[64];
		GetPlayerName(pID,pName,sizeof(pName));
		format(string,sizeof(string),FB_foxBanFile,pName);
		FB_dini_Create(string);
		FB_dini_IntSet(string,"fbFB_Time",FB_Time());
		FB_dini_IntSet(string,"fbDuration",bFB_Time*60);
		Kick(pID);
	}
//--------------------------------------------------------//

//---------------------------< TUNBAN >---------------------------//
	forward TUnban(playername[]);
	public TUnban(playername[])
	{
		format(FB_tbDir,sizeof(FB_tbDir),FB_foxBanFile,playername);
		if(FB_dini_Exists(FB_tbDir)) {
			FB_dini_Remove(FB_tbDir); return true; }
		return false;
	}
//----------------------------------------------------------------//

//---------------------------------------------< TUNBAN >---------------------------------------------//
	stock GetPlayerBanFB_Time(playername[],&minu=0,&seco=0)
	{
		format(FB_tbDir,sizeof(FB_tbDir),FB_foxBanFile,playername);
		if(FB_dini_Exists(FB_tbDir))
		{
			seco=(FB_dini_Int(FB_tbDir,"fbFB_Time")+FB_dini_Int(FB_tbDir,"fbDuration"))-FB_Time();
			seco=seco-minu*60;
			minu=seco/60;
		}
	}
//----------------------------------------------------------------------------------------------------//

//------------------------< BanFileExist >------------------------//
	forward BanFileExist(playername[]);
	public BanFileExist(playername[])
	{
		format(FB_tbDir,sizeof(FB_tbDir),FB_foxBanFile,playername);
		if(FB_dini_Exists(FB_tbDir)) { return 1; } else { return 0; }
	}
//----------------------------------------------------------------//

//////////////////////////////////////////// INTEGRATED DINI.inc & DUTILS.inc (THX DRACOBLUE) ////////////////////////////////////////////////
stock FB_mktime(hour,minute,second,day,month,year) {
	new timestamp2; timestamp2 = second + (minute * 60) + (hour * 3600); new days_of_month[12]; if ( ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) ) { days_of_month = {31,29,31,30,31,30,31,31,30,31,30,31}; } else { days_of_month = {31,28,31,30,31,30,31,31,30,31,30,31}; }
	new days_this_year = 0; days_this_year = day; if(month > 1) {  for(new i=0; i<month-1;i++) { days_this_year += days_of_month[i]; }
	} timestamp2 += days_this_year * 86400; for(new j=1970;j<year;j++) { timestamp2 += 31536000; if ( ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) )  timestamp2 += 86400; }
	return timestamp2; }
stock FB_Time() { new fxban_int_hour,fxban_int_minute,fxban_int_second,fxban_int_year,fxban_int_month,fxban_int_day; gettime(fxban_int_hour, fxban_int_minute, fxban_int_second); getdate(fxban_int_year, fxban_int_month, fxban_int_day); return FB_mktime(fxban_int_hour,fxban_int_minute,fxban_int_second,fxban_int_day,fxban_int_month,fxban_int_year); }
stock FB_fcopytextfile(oldname[],newname[]) {
	new File:ohnd,File:nhnd; if (!fexist(oldname)) return false; ohnd=fopen(oldname,io_read); nhnd=fopen(newname,io_write); new tmpres[MAX_STRING]; while (fread(ohnd,tmpres)) {
		FB_StripNewLine(tmpres); format(tmpres,sizeof(tmpres),"%s\r\n",tmpres); fwrite(nhnd,tmpres); }
	fclose(ohnd); fclose(nhnd); return true; }
stock FB_equal(str1[],str2[],bool:ignorecase) { if (strlen(str1)!=strlen(str2)) return false; if (strcmp(str1,str2,ignorecase)==0) return true; return false; }
stock FB_ret_memcpy(source[],index=0,numbytes) {
	new tmp[MAX_STRING],i=0; tmp[0]=0; if (index>=strlen(source)) return tmp; if (numbytes+index>=strlen(source)) numbytes=strlen(source)-index; if (numbytes<=0) return tmp; for (i=index;i<numbytes+index;i++) {
		tmp[i-index]=source[i]; if (source[i]==0) return tmp; }
	tmp[numbytes]=0; return tmp; }
stock FB_strlower(txt[]) {
	new tmp[MAX_STRING]; tmp[0]=0; if (txt[0]==0) return tmp; new i=0; for (i=0;i<strlen(txt);i++) {
		tmp[i]=tolower(txt[i]); }
	tmp[strlen(txt)]=0; return tmp; }
stock FB_StripNewLine(string[]) { new len = strlen(string); if (string[0]==0) return ; if ((string[len - 1] == '\n') || (string[len - 1] == '\r')) { string[len - 1] = 0; if (string[0]==0) return ; if ((string[len - 2] == '\n') || (string[len - 2] == '\r')) string[len - 2] = 0; } }
stock FB_set(dest[],source[]) {
	new count = strlen(source),i=0; for (i=0;i<count;i++) {
		dest[i]=source[i]; }
	dest[count]=0; }
stock FB_dini_Exists(filename[]) { if (fexist(filename)) return true; return false; }
stock FB_dini_Remove(filename[]) { if (!fexist(filename)) return false; fremove(filename); return true; }
stock FB_dini_Create(filename[]) { new File:fhnd; if (fexist(filename)) return false; fhnd=fopen(filename,io_write); fclose(fhnd); return true; }
stock FB_dini_IntSet(filename[],key[],value) { new valuestring[MAX_STRING]; format(valuestring,sizeof(valuestring),"%d",value); return FB_dini_Set(filename,key,valuestring); }
stock FB_dini_Int(filename[],key[]) { return strval(FB_dini_Get(filename,key)); }
stock FB_dini_Set(filename[],key[],value[]) {
	new File:fohnd, File:fwhnd; new bool:wasset=false; new tmpres[MAX_STRING]; if (key[0]==0) return false; format(tmpres,sizeof(tmpres),"%s.part",filename); fohnd=fopen(filename,io_read); if (!fohnd) return false; fremove(tmpres); fwhnd=fopen(tmpres,io_write); while (fread(fohnd,tmpres)) {
		FB_StripNewLine(tmpres); if ((!wasset)&&(FB_equal(FB_dini_PRIVATE_ExtractKey(tmpres),key,true))) { format(tmpres,sizeof(tmpres),"%s=%s",key,value); wasset=true; }
		fwrite(fwhnd,tmpres); fwrite(fwhnd,"\r\n"); }
	if (!wasset) { format(tmpres,sizeof(tmpres),"%s=%s",key,value); fwrite(fwhnd,tmpres); fwrite(fwhnd,"\r\n"); }
	fclose(fohnd); fclose(fwhnd); format(tmpres,sizeof(tmpres),"%s.part",filename); if (FB_fcopytextfile(tmpres,filename)) { return fremove(tmpres); }
	return false; }
stock FB_dini_Get(filename[],key[]) {
	new File:fohnd,tmpres[MAX_STRING],tmpres2[MAX_STRING]; tmpres[0]=0; fohnd=fopen(filename,io_read); if (!fohnd) return tmpres; while (fread(fohnd,tmpres)) {
		FB_StripNewLine(tmpres); if(FB_equal(FB_dini_PRIVATE_ExtractKey(tmpres),key,true)) { tmpres2[0]=0; strcat(tmpres2,FB_dini_PRIVATE_ExtractValue(tmpres)); fclose(fohnd); return tmpres2; } }
	fclose(fohnd); return tmpres; }
stock FB_dini_PRIVATE_ExtractKey(line[]) { new tmp[MAX_STRING]; tmp[0]=0; if (strfind(line,"=",true)==-1) return tmp; FB_set(tmp,FB_strlower(FB_ret_memcpy(line,0,strfind(line,"=",true)))); return tmp; }
stock FB_dini_PRIVATE_ExtractValue(line[]) {
	new tmp[MAX_STRING]; tmp[0]=0; if (strfind(line,"=",true)==-1) { return tmp; }
	FB_set(tmp,FB_ret_memcpy(line,strfind(line,"=",true)+1,strlen(line))); return tmp; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
#define OnPlayerConnect FB_OnPlayerConnect
