/*
	
	SwarmConfWindow.cpp
	
	Copyright 1995 Be Incorporated, All Rights Reserved.
	
*/

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef SwarmConf_WINDOW_H
#include "ConfWindow.h"
#endif
#ifndef SwarmConf_VIEW_H
#include "ConfView.h"
#endif

SwarmConfWindow::SwarmConfWindow(BRect frame)
				: BWindow(frame, "SwarmConfigTest", B_TITLED_WINDOW, B_NOT_ZOOMABLE )
{
	SwarmConfView	*aView;
	
	/* set up a rectangle and instantiate a new view */
	BRect aRect( Bounds() );
	aView = new SwarmConfView(aRect, "SwarmConfigTest");
	
	/* add view to window */
	AddChild(aView);
}

bool SwarmConfWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}
