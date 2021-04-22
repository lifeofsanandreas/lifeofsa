#if defined PathFinder
  #endinput
#endif
#define PathFinder
native PathFinder_Init(mapAndreasAddress);
native PathFinder_FindWay(routeid, Float:startX, Float:startY, Float:endX, Float:endY, Float:zDifference = 0.9, stepSize = 1, stepLimit = -1, maxSteps = 2000);
native PathFinder_GetNearNodeId(Float:posX, Float:posY);
native PathFinder_GetNodePos(nodeid, &Float:posX, &Float:posY, &Float:posZ);
native PathFinder_SetTickRate(rate = 5);
forward OnPathCalculated(routeid, success, Float:nodesX[], Float:nodesY[], Float:nodesZ[], nodesSize);                                                                                  