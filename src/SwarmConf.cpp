/*
	
	SwarmConfWorld.cpp
	
	Copyright 1997 Be Incorporated, All Rights Reserved.
	
*/

#ifndef SwarmConf_WINDOW_H
#include "ConfWindow.h"
#endif
#ifndef SwarmConf_WORLD_H
#include "SwarmConf.h"
#endif

main()
{	
	SwarmConfApplication	myApplication;

	myApplication.Run();

	return(0);
}

SwarmConfApplication::SwarmConfApplication()
		  		  : BApplication("application/x-vnd.Steins.SwarmConfigTest")
{
	SwarmConfWindow		*aWindow;
	BRect			aRect;

	// set up a rectangle and instantiate a new window
	aRect.Set(100, 80, 100+241, 80+244);
	aWindow = new SwarmConfWindow(aRect);
			
	// make window visible
	aWindow->Show();
}
