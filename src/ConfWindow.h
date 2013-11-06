/*
	
	SwarmConfWindow.h
	
	Copyright 1997 Be Incorporated, All Rights Reserved.
	
*/

#ifndef SwarmConf_WINDOW_H
#define SwarmConf_WINDOW_H

#ifndef _WINDOW_H
#include <Window.h>
#endif

class SwarmConfWindow : public BWindow 
{
public:
				SwarmConfWindow(BRect frame); 
virtual	bool	QuitRequested();
};

#endif //SwarmConf_WINDOW_H
