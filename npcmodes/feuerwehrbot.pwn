#include <a_npc>
#define RECORDING "Feuerwehrbot"

main()
{
}

public OnRecordingPlaybackEnd()
{
	StartRecordingPlayback(2, RECORDING);//2 = Aufnahmetyp - Zu fu?, Recording = Der oben definierte Aufnahmename
}

public OnNPCExitVehicle()
{
	StartRecordingPlayback(2, RECORDING);//2 = Aufnahmetyp - Zu fu?, Recording = Der oben definierte Aufnahmename
}

public OnNPCSpawn()
{
	StartRecordingPlayback(2, RECORDING);//2 = Aufnahmetyp - Zu fu?, Recording = Der oben definierte Aufnahmename
}
