/*
	
	SwarmConfWorld.h
	
	Copyright 1997 Be Incorporated, All Rights Reserved.

*/

#ifndef SwarmConf_WORLD_H
#define SwarmConf_WORLD_H

#ifndef _APPLICATION_H
#include <Application.h>
#endif

typedef struct {
	long		magic;		// swarm settings magic
	long		version;	// swarm settings version
	int			bees;		// number of bees
	int			wasps;		// number of wasps
	float		wasp_vel;	// maximum wasp speed
	float		bee_vel;	// maximum bee speed
	float		wasp_acc;	// maximum wasp acceleration
	float		bee_acc;	// maximum bee acceleration
	float		delay;		// delay between updates, in microseconds
	float		border;		// border limiting wasp travel
	bool		bCycleCols;	// cycle colors
	int			colCycleChange;		// animation cycles after which color 
									// is changed
	bool		bHyperSpace;	// Hyperspace wasps
	int			hypertime;		// hyperspace after max. X seconds
	bool		bHeatMap;	// heat mapping on
} SwarmSettings;

class SwarmConfApplication : public BApplication 
{
public:
	SwarmConfApplication();
};

#endif //SwarmConf_WORLD_H
