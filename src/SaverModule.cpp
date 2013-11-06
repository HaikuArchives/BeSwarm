// ============================================================
//  SaverModule.cpp	©1996 Hiroshi Lockheimer
// ============================================================

#ifndef _VIEW_H
#include <View.h>
#endif

#include "SaverModule.h"
#include "TControlSlide.h"
#include "TTabView.h"

#include "XSwarm.h"
#include "colors.h"


// Uncomment the following line to prevent 'Random' from calling
// your module

//char inhibit_random = 1;
XSwarm*			m_saver = NULL;		// Pointer to swarm class
XSwarm*			m_demo = NULL;		// Pointer to demo swarm
SwarmSettings 	theSettings;		// swarm settings
static 			gConfiguring = 0;	// currently configuring

// ------------------------------------------------------------
// 		¥ module_initialize  [optional]
// ------------------------------------------------------------
// This function gets called once, immediately after saver has
// loaded your module
//
// It is not known at this point whether you are being loaded
// to save the screen or to configure yourself (or both)
//
// inSettings is a pointer to data inSettingsSize long that saver 
// has stored for you, it is your responsibility to free it before 
// your module is unloaded
//
// saver may pass you NULL and 0 if it cannot find any data that
// belongs to you

void
module_initialize(
	void	*inSettings,
	long	inSettingsSize)
{
	bool valid = false;
	
	if ((inSettings != NULL) && (inSettingsSize > 0))
	{
		// read settings into own data
		SwarmSettings*	inSet = (SwarmSettings*) inSettings;
		if ((inSettingsSize == sizeof (SwarmSettings))
		 && (inSet->magic == BESWARM_MAGIC) && (inSet->version == BESWARM_VERSION))
		{
			// copy settings
			theSettings = *inSet;
			valid = true;
		}
		// free settings memory
		free (inSettings);
	}
	if (false == valid)
	{
		// set settings to default values
		theSettings.magic = BESWARM_MAGIC;
		theSettings.version = BESWARM_VERSION;
		theSettings.bees = BEES;
		theSettings.wasps = WASPS;
		theSettings.wasp_vel = WASPVEL;
		theSettings.bee_vel = BEEVEL;
		theSettings.wasp_acc = WASPACC;
		theSettings.bee_acc = BEEACC;
		theSettings.delay = DELAY;
		theSettings.border = BORDER;
		theSettings.bCycleCols = CYCLECOLS;
		theSettings.colCycleChange = COLORFRAMES;
		theSettings.bHyperSpace = HYPERSPACE;
		theSettings.hypertime = HYPERTIME;
	}
}


// ------------------------------------------------------------
// 		¥ module_cleanup  [optional]
// ------------------------------------------------------------
// This function gets called once, immediately before saver will
// unload your module
//
// This is the last chance to clean up after yourself
//
// Set outSettings to a pointer that is outSettingsSize long, 
// and saver will store it for you until the next time your module 
// is loaded
//
// Pass back NULL and 0 if you do not need to store anything
//
// Do not attempt to free what you pass back to saver

void
module_cleanup(
	void	**outSettings,
	long	*outSettingsSize)
{
	SwarmSettings* outSet;
	
	outSet = (SwarmSettings*) malloc (sizeof (theSettings));
	*outSet = theSettings;
	*outSettings = outSet;
	*outSettingsSize = sizeof (theSettings);
}


// ------------------------------------------------------------
// 		 module_start_saving  [required]
// ------------------------------------------------------------
// This function gets called when it is time to "save" the screen
//
// You are given a BView in which you are to do your work
//
// The window in which the BView resides is not shown at this time,
// it is your responsibility to inView->Window()->Show() it
//
// Spawn your thread(s) here and return from this function ASAP
// Do all your drawing in inView, or in its children (if you 
// decide to add any)
//
// inView's View and Low color are set to black, High color is white
//
// saver never locks inView's window for you, so be sure to
// inView->Window()->Lock() before you try to do anything with
// inView, otherwise you win a free trip to the debugger

void
module_start_saving(
	BView	*inView)
{
	// create animation thread
	if (m_saver != NULL)
		delete m_saver;

	if ( gConfiguring == 1)
		GetSettings();

	m_saver = new XSwarm (inView, &theSettings); 
	if (m_saver != NULL)
	{
		m_saver->Go();
	}
}


// ------------------------------------------------------------
// 		¥ module_stop_saving  [required]
// ------------------------------------------------------------
// This function gets called right before the BWindow that
// you have been drawing into gets closed
//
// Exit from all your thread(s), free any memory that you no 
// longer need, unlock anything that you locked, be good and 
// clean up after yourself

void
module_stop_saving()
{
	if (m_saver != NULL)
	{
		m_saver->Stop();
		delete m_saver;
		m_saver = NULL;
	}
}


// ------------------------------------------------------------
// 		 module_start_config  [required]
// ------------------------------------------------------------
// This function gets called when the user has selected your 
// module in the settings panel
//
// You are free to add any children to inView or modify it
// in any way (but don't even think about deleting it)
//
// inView is 241 units wide and 244 units high

void
module_start_config(
	BView *inView)
{
	if (!inView->Window()->Lock())
		return;
		
	BRect frame;
	BStringView	*view;
	
	long	theControlTop = inView->Bounds().top + 20;
	
	frame = inView->Bounds();
	frame.InsetBy(5,5);
	
	TTabView *theTabView = new TTabView(frame, "TabView", B_FOLLOW_ALL,B_WILL_DRAW);
	theTabView->SetViewColor(inView->ViewColor());
	inView->AddChild(theTabView);
	
	frame = theTabView->Bounds();
	BView	*theCredits = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theCredits->SetViewColor(inView->ViewColor());
	theTabView->AddChild(theCredits,"Credits");
	
	frame = theTabView->Bounds();
	BView	*theSettingsView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theSettingsView->SetViewColor(inView->ViewColor());
	theTabView->AddChild(theSettingsView,"Settings");
	
	frame = theTabView->Bounds();
	BView	*theBeeView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theBeeView->SetViewColor(inView->ViewColor());
	theTabView->AddChild(theBeeView,"Bees");

	frame = theTabView->Bounds();
	BView	*theWaspView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theWaspView->SetViewColor(inView->ViewColor());
	theTabView->AddChild(theWaspView,"Wasps");

	frame = theTabView->Bounds();
	frame.InsetBy (30,30);
	frame.top += 10;
	gDemoView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	gDemoView->SetViewColor(black);
	theTabView->AddChild(gDemoView,"Preview");
	
	if (m_demo != NULL)
		delete m_demo;
	
	m_demo = new XSwarm (gDemoView, &theSettings); 
	if (m_demo != NULL)
	{
		m_demo->Go();
	}


// Setup Credit Text
	frame = theCredits->Bounds();
	frame.InsetBy(5,5);
	
	theControlTop=10;
	frame.top += theControlTop;
	frame.left += 10;
	frame.bottom = frame.top + 15.0;

	view = new BStringView(frame, B_EMPTY_STRING, "BeSwarm " BESWARM_TEXT_VER);

	theCredits->AddChild(view);
	view->SetViewColor(theCredits->ViewColor());
	view->SetFont(be_bold_font);

	char *theRealText = "   by Dirk Steins (dsteins@ibm.net)\n\n"
						"Based on xswarm from Jeff Butterworth (butterwo@cs.unc.edu).\n\n"
						"This module is using some controls from"
						" Electron Storm by Trevor Walker (twalker@carroll1.cc.edu).\n\n"  
						"This module is freeware. ";

	theControlTop=10;
	frame = theCredits->Bounds();
	frame.InsetBy (5,5);
	frame.top += theControlTop;

	BTextView *theText = new BTextView(frame, "", frame,B_FOLLOW_ALL,B_WILL_DRAW);
	
	theCredits->AddChild(theText);
	theText->SetViewColor(theCredits->ViewColor());
	theText->SetLowColor(theCredits->ViewColor());
	theText->SetText(theRealText);
	theText->SetWordWrap(TRUE);
	theText->SetAutoindent(FALSE);
	theText->MakeSelectable(FALSE);

// Settings View Setup
// Color cycling
	theControlTop=5;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.right -= 100.0;
	frame.bottom = frame.top + 12.0;

	gCycleColors = new  BCheckBox(frame, "", "Cycle colors",NULL, 
								  B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW );
	theSettingsView->AddChild(gCycleColors);
	gCycleColors->SetValue(theSettings.bCycleCols);

// Heat Mapping 
	frame.left = frame.right + 10;
	frame.right = theSettingsView->Bounds().right - 10;
	gHeatMap = new  BCheckBox(frame, "", "Heat Map",NULL, 
								  B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW );
	theSettingsView->AddChild(gHeatMap);
	gHeatMap->SetValue(theSettings.bHeatMap);

	theControlTop+=20;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Cycle color every n'th frame");
	theSettingsView->AddChild(view);
	view->SetViewColor(theSettingsView->ViewColor());

	theControlTop +=20;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	BTextControl *caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theSettingsView->AddChild(caption);
	caption->SetViewColor(theSettingsView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont(be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theSettingsView->Bounds().left + 40;
	gSlideCycles = new TControlSlide(frame, NULL, caption, 1, 100, B_HORIZONTAL, 1, "%.1f");
	theSettingsView->AddChild(gSlideCycles);
	gSlideCycles->SetValue(theSettings.colCycleChange);

// Frames per second (delay)
	theControlTop+=20;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Frames/sec.");
	theSettingsView->AddChild(view);
	view->SetViewColor(theSettingsView->ViewColor());

	theControlTop +=20;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theSettingsView->AddChild(caption);
	caption->SetViewColor(theSettingsView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theSettingsView->Bounds().left + 40;
	gSlideDelay = new TControlSlide(frame, NULL, caption, 5, 200, B_HORIZONTAL, 1, "%.1f");
	theSettingsView->AddChild(gSlideDelay);
	gSlideDelay->SetValue(1000000.0 / theSettings.delay);

// Hyperspace enabled	
	theControlTop +=30;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	gHyperspace = new  BCheckBox(frame, "", "Hyperspace",NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW );
	theSettingsView->AddChild(gHyperspace);
	gHyperspace->SetValue(theSettings.bHyperSpace);

// Hyperspace time
	theControlTop+=20;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Max. time for hyperspace (seconds)");
	theSettingsView->AddChild(view);
	view->SetViewColor(theSettingsView->ViewColor());

	theControlTop +=20;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theSettingsView->AddChild(caption);
	caption->SetViewColor(theSettingsView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theSettingsView->Bounds().left + 40;
	gSlideHyper = new TControlSlide(frame, NULL, caption, 5, 300, B_HORIZONTAL, 1, "%.1f");
	theSettingsView->AddChild(gSlideHyper);
	gSlideHyper->SetValue(theSettings.hypertime);


// Back to default 	
	theControlTop +=30;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 40.0;
	frame.right -= 40.0;	
	frame.bottom = frame.top + 12.0;
	
	gDefaults = new  BButton (frame, "", "Defaults", new BMessage (BESWARM_DEFAULTS), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW );
	gDefaults->SetTarget (theSettingsView);
	theSettingsView->AddChild(gDefaults);
	
// add message filter for default message
	BMessageFilter *mFilter = new BMessageFilter (BESWARM_DEFAULTS,
												   &filter_msg);
	
	theSettingsView->AddFilter (mFilter);	


// Bees View Setup
// MaxBees
	theControlTop=5;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Number of Bees");
	theBeeView->AddChild(view);
	view->SetViewColor(theBeeView->ViewColor());

	theControlTop +=20;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theBeeView->AddChild(caption);
	caption->SetViewColor(theBeeView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theBeeView->Bounds().left + 40;
	gSlideMaxBees = new TControlSlide(frame, NULL, caption,10, 400, B_HORIZONTAL, 1, "%.1f");
	theBeeView->AddChild(gSlideMaxBees);
	gSlideMaxBees->SetValue(theSettings.bees);

// MaxAcceleration
	theControlTop+=20;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Bee acceleration");
	theBeeView->AddChild(view);
	view->SetViewColor(theBeeView->ViewColor());

	theControlTop +=20;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theBeeView->AddChild(caption);
	caption->SetViewColor(theBeeView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theWaspView->Bounds().left + 40;
	gSlideBeeAcc = new TControlSlide(frame, NULL, caption, 1, 9, B_HORIZONTAL, 1, "%.1f");
	theBeeView->AddChild(gSlideBeeAcc);
	gSlideBeeAcc->SetValue(theSettings.bee_acc);

// MaxSpeed
	theControlTop+=20;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Bee max speed");
	theBeeView->AddChild(view);
	view->SetViewColor(theBeeView->ViewColor());

	theControlTop +=20;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theBeeView->AddChild(caption);
	caption->SetViewColor(theBeeView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theBeeView->Bounds().left + 40;
	gSlideBeeVel = new TControlSlide(frame, NULL, caption, 3, 19, B_HORIZONTAL, 1, "%.1f");
	theBeeView->AddChild(gSlideBeeVel);
	gSlideBeeVel->SetValue(theSettings.bee_vel);

// Wasp View Setup
// MaxWasp
	theControlTop=5;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Number of Wasps");
	theWaspView->AddChild(view);
	view->SetViewColor(theWaspView->ViewColor());

	theControlTop +=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theWaspView->AddChild(caption);
	caption->SetViewColor(theWaspView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theWaspView->Bounds().left + 40;
	gSlideMaxWasps = new TControlSlide(frame, NULL, caption, 1, 8, B_HORIZONTAL, 1, "%.1f");
	theWaspView->AddChild(gSlideMaxWasps);
	gSlideMaxWasps->SetValue(theSettings.wasps);

// MaxAcceleration
	theControlTop+=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Wasp acceleration");
	theWaspView->AddChild(view);
	view->SetViewColor(theWaspView->ViewColor());

	theControlTop +=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theWaspView->AddChild(caption);
	caption->SetViewColor(theWaspView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theWaspView->Bounds().left + 40;
	gSlideWaspAcc = new TControlSlide(frame, NULL, caption, 1, 9, B_HORIZONTAL, 1, "%.1f");
	theWaspView->AddChild(gSlideWaspAcc);
	gSlideWaspAcc->SetValue(theSettings.wasp_acc);

// MaxSpeed
	theControlTop+=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Wasp max speed");
	theWaspView->AddChild(view);
	view->SetViewColor(theWaspView->ViewColor());

	theControlTop +=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theWaspView->AddChild(caption);
	caption->SetViewColor(theWaspView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theWaspView->Bounds().left + 40;
	gSlideWaspVel = new TControlSlide(frame, NULL, caption, 3, 19, B_HORIZONTAL, 1, "%.1f");
	theWaspView->AddChild(gSlideWaspVel);
	gSlideWaspVel->SetValue(theSettings.wasp_vel);

// Border
	theControlTop+=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	view = new BStringView(frame, B_EMPTY_STRING, "Border for wasp");
	theWaspView->AddChild(view);
	view->SetViewColor(theWaspView->ViewColor());

	theControlTop +=20;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.bottom = frame.top + B_H_SCROLL_BAR_HEIGHT;
	frame.right -= 20;
	frame.left = frame.right - 70;
	
	caption = new BTextControl(frame, NULL,  NULL, "", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	theWaspView->AddChild(caption);
	caption->SetViewColor(theWaspView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont (be_plain_font);

	frame.right = frame.left - 10;
	frame.left = theWaspView->Bounds().left + 40;
	gSlideBorder = new TControlSlide(frame, NULL, caption, 10, 100, B_HORIZONTAL, 1, "%.1f");
	theWaspView->AddChild(gSlideBorder);
	gSlideBorder->SetValue(theSettings.border);

	inView->Window()->UpdateIfNeeded();
	
	inView->Window()->Unlock();

	gConfiguring = 1;
}


// ------------------------------------------------------------
// 		 module_stop_config  [required]
// ------------------------------------------------------------
// This function gets called right before the BView that was passed
// to you in module_start_config() gets deleted
//
// Free any memory that you may have allocated, you know the game...
//
// Since your view will be deleted, it is not necessary to delete 
// the children that you added to it

void
module_stop_config()
{
	GetSettings();
	gConfiguring = 0;

	if (m_demo != NULL)
	{
		m_demo->Stop();
		delete m_demo;
		m_demo = NULL;
	}
}

void
SetSettings()
{
	gSlideMaxBees->SetValue(theSettings.bees);
	gSlideMaxWasps->SetValue(theSettings.wasps);
	gSlideBeeAcc->SetValue(theSettings.bee_acc);
	gSlideBeeVel->SetValue(theSettings.bee_vel);
	gSlideWaspAcc->SetValue(theSettings.wasp_acc);
	gSlideWaspVel->SetValue(theSettings.wasp_vel);
	gSlideDelay->SetValue(1000000.0 / theSettings.delay);
	gSlideBorder->SetValue(theSettings.border);
	gSlideCycles->SetValue(theSettings.colCycleChange);
	gCycleColors->SetValue(theSettings.bCycleCols);
	gHyperspace->SetValue(theSettings.bHyperSpace);
	gSlideHyper->SetValue(theSettings.hypertime);
	gHeatMap->SetValue(theSettings.bHeatMap);
}

void
GetSettings()
{
	theSettings.bees = gSlideMaxBees->Value();
	theSettings.wasps = gSlideMaxWasps->Value();
	theSettings.bee_acc = gSlideBeeAcc->Value();
	theSettings.bee_vel = gSlideBeeVel->Value();
	theSettings.wasp_acc = gSlideWaspAcc->Value();
	theSettings.wasp_vel = gSlideWaspVel->Value();
	theSettings.delay = 1000000.0 / gSlideDelay->Value();
	theSettings.border = gSlideBorder->Value();
	theSettings.colCycleChange = gSlideCycles->Value();
	theSettings.bCycleCols = gCycleColors->Value();
	theSettings.bHyperSpace = gHyperspace->Value();
	theSettings.hypertime = gSlideHyper->Value();
	theSettings.bHeatMap = gHeatMap->Value();
}

filter_result filter_msg (BMessage* message, BHandler **target, 
						   BMessageFilter *messageFilter)
{
	switch (message->what)
	{
		case BESWARM_DEFAULTS:
			// Set settings back to default
			theSettings.bees = BEES;
			theSettings.wasps = WASPS;
			theSettings.wasp_vel = WASPVEL;
			theSettings.bee_vel = BEEVEL;
			theSettings.wasp_acc = WASPACC;
			theSettings.bee_acc = BEEACC;
			theSettings.delay = DELAY;
			theSettings.border = BORDER;
			theSettings.bCycleCols = CYCLECOLS;
			theSettings.colCycleChange = COLORFRAMES;
			theSettings.bHyperSpace = HYPERSPACE;
			theSettings.hypertime = HYPERTIME;
			theSettings.bHeatMap = HEATMAP;
			SetSettings();
			break;
		default: break;
	}
	
	return B_DISPATCH_MESSAGE;
}