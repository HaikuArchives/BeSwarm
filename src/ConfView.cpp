/*
	
	SwarmConfView.cpp
	
	Copyright 1997 Be Incorporated, All Rights Reserved.
	
*/

#ifndef SwarmConf_VIEW_H
#include "ConfView.h"
#endif
#ifndef _ROSTER_H
#include <Roster.h>
#endif

#include <TabView.h>
#include <TextView.h>
#include <StringView.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <ScrollBar.h>
#include <Button.h>

#include <stdio.h>
#include <string.h>
#include <String.h>

#include "colors.h"

/* Magics */
#define BESWARM_MAGIC		'xbSW'
#define BESWARM_VERSION		0x0130
#define BESWARM_TEXT_VER	"1.3"

// Messages
#define BESWARM_DEFAULTS	'xbDf'

TControlSlide		*gSlideMaxBees;
TControlSlide		*gSlideBeeVel;
TControlSlide		*gSlideBeeAcc;
TControlSlide		*gSlideWaspVel;
TControlSlide		*gSlideWaspAcc;
TControlSlide		*gSlideDelay;
TControlSlide		*gSlideCycles;
TControlSlide		*gSlideBorder;
TControlSlide		*gSlideMaxWasps;
TControlSlide		*gSlideHyper;
BCheckBox		*gCycleColors;
BCheckBox		*gHyperspace;
BCheckBox		*gHeatMap;
BButton			*gDefaults;
BView			*gDemoView;

SwarmConfView::SwarmConfView(BRect rect, const char *name)
	: BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor (216,216,216,0);
}

void SwarmConfView::AttachedToWindow()
{
	SetViewColor (216,216,216,0);
	module_start_config (this);
}


void SwarmConfView::module_start_config(
	BView *inView)
{
	if (!inView->Window()->Lock())
		return;
		
	inView->Window()->DisableUpdates();
	BRect frame;
	BStringView	*view;
	BTab*	tab;
	
	float	theControlTop = inView->Bounds().top + 20.0;
	
	frame = inView->Bounds();
	frame.InsetBy(5,5);
	
    BTabView *theTabView = new BTabView(frame, "TabView");
	theTabView->SetViewColor(inView->ViewColor());
	inView->AddChild(theTabView);
	
	frame = theTabView->Bounds();
//	frame.InsetBy(5,5);
	frame.bottom -= theTabView->TabHeight();
	
	BView	*theCredits = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
    theCredits->SetViewColor(inView->ViewColor());
	
	BView	*theSettingsView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theSettingsView->SetViewColor(inView->ViewColor());
	
	BView	*theBeeView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theBeeView->SetViewColor(inView->ViewColor());

	BView	*theWaspView = new BView(frame, B_EMPTY_STRING, 0,B_WILL_DRAW);
	theWaspView->SetViewColor(inView->ViewColor());

// Setup Credit Text
	frame = theCredits->Bounds();
//	frame.InsetBy(5,5);
	
	theControlTop=10;
	frame.top += theControlTop;
	frame.left += 10;
	frame.bottom = frame.top + 15.0;

	view = new BStringView(frame, "nullname", "BeSwarm 2.0");

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

// create tabs
	tab = new BTab(); 
    theTabView->AddTab(theCredits, tab); 
    tab->SetLabel("Credits"); 

	tab = new BTab(); 
    theTabView->AddTab(theSettingsView, tab); 
    tab->SetLabel("Config"); 

	tab = new BTab(); 
    theTabView->AddTab(theBeeView, tab); 
    tab->SetLabel("Bees"); 

	tab = new BTab(); 
    theTabView->AddTab(theWaspView, tab); 
    tab->SetLabel("Wasps"); 

	theTabView->SetTabWidth(B_WIDTH_FROM_WIDEST);

// Settings View Setup
// Color cycling
	theControlTop=5;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.right -= 100.0;
	frame.bottom = frame.top + 12.0;

	gCycleColors = new  BCheckBox(frame, "", "Cycle colors",NULL, 
								  B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW|B_NAVIGABLE );
	theSettingsView->AddChild(gCycleColors);
	gCycleColors->SetValue(theSettings.bCycleCols);

// Heat Mapping 
	frame.left = frame.right + 10;
	frame.right = theSettingsView->Bounds().right - 10;
	gHeatMap = new  BCheckBox(frame, "", "Heat Map",NULL, 
								  B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW|B_NAVIGABLE);
	theSettingsView->AddChild(gHeatMap);
	gHeatMap->SetValue(theSettings.bHeatMap);

	theControlTop +=18;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left = theSettingsView->Bounds().left + 20;
	frame.right = theSettingsView->Bounds().right - 10;

	TControlSlide* gSlideCycle = new TControlSlide(frame, "", "Cycle color every n'th frame:", NULL, 1, 100, 1, "%.f", B_TRIANGLE_THUMB);
	theSettingsView->AddChild(gSlideCycle);
	gSlideCycle->SetValue(theSettings.colCycleChange);

// Frames per second (delay)
	theControlTop += 48;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left = theSettingsView->Bounds().left + 20;
	frame.right = theSettingsView->Bounds().right - 10;

	TControlSlide* gSlideDelay = new TControlSlide(frame, "", "Frames/sec. :", NULL, 5, 200, 1, "%.f", B_TRIANGLE_THUMB);
	theSettingsView->AddChild(gSlideDelay);
	gSlideDelay->SetValue(1000000.0 / theSettings.delay);

// Hyperspace enabled	
	theControlTop += 48;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 10.0;
	frame.bottom = frame.top + 12.0;
	gHyperspace = new  BCheckBox(frame, "", "Hyperspace", NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW|B_NAVIGABLE );
	theSettingsView->AddChild(gHyperspace);
	gHyperspace->SetValue(theSettings.bHyperSpace);

// Hyperspace time
	theControlTop += 18;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left = theSettingsView->Bounds().left + 20;
	frame.right = theSettingsView->Bounds().right - 10;

	TControlSlide* gSlideHyper = new TControlSlide(frame, "", "Max. time for hyperspace (secs):", NULL, 5, 300, 1, "%.f", B_TRIANGLE_THUMB);
	theSettingsView->AddChild(gSlideHyper);
	gSlideHyper->SetValue(theSettings.hypertime);

// Back to default 	
	theControlTop += 50;
	frame = theSettingsView->Bounds();
	frame.top += theControlTop;
	frame.left += 40.0;
	frame.right -= 40.0;	
	frame.bottom = frame.top + 10.0;
	
	gDefaults = new  BButton (frame, "", "Defaults", new BMessage (BESWARM_DEFAULTS), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW );
	gDefaults->SetTarget (theSettingsView);
	theSettingsView->AddChild(gDefaults);
	
//	add message filter for default message
//	BMessageFilter *mFilter = new BMessageFilter (BESWARM_DEFAULTS,
//												   &filter_msg);
	
//	theSettingsView->AddFilter (mFilter);	



// Bees View Setup
// MaxBees
	theControlTop=5;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideMaxBees = new TControlSlide(frame, "", "Number of Bees", NULL, 10, 400, 1, "%.f", B_TRIANGLE_THUMB);
	theBeeView->AddChild(gSlideMaxBees);
	gSlideMaxBees->SetValue(theSettings.bees);

// MaxAcceleration
	theControlTop+= 48;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideBeeAcc = new TControlSlide(frame, "", "Bee acceleration", NULL, 1, 9, 1, "%.f", B_TRIANGLE_THUMB);
	theBeeView->AddChild(gSlideBeeAcc);
	gSlideBeeAcc->SetValue(theSettings.bee_acc);

// MaxSpeed
	theControlTop+=48;
	frame = theBeeView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideBeeVel = new TControlSlide(frame, "", "Bee max speed", NULL, 3, 19, 1, "%.f", B_TRIANGLE_THUMB);
	theBeeView->AddChild(gSlideBeeVel);
	gSlideBeeVel->SetValue(theSettings.bee_vel);

// Wasp View Setup
// MaxWasp
	theControlTop=5;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideMaxWasps = new TControlSlide(frame, "", "Number of Wasps", NULL, 1, 8, 1, "%.f", B_TRIANGLE_THUMB);
	theWaspView->AddChild(gSlideMaxWasps);
	gSlideMaxWasps->SetValue(theSettings.wasps);

// MaxAcceleration
	theControlTop+=48;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideWaspAcc = new TControlSlide(frame, "", "Wasp acceleration", NULL, 1, 9, 1, "%.f", B_TRIANGLE_THUMB);
	theWaspView->AddChild(gSlideWaspAcc);
	gSlideWaspAcc->SetValue(theSettings.wasp_acc);

// MaxSpeed
	theControlTop+=48;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideWaspVel = new TControlSlide(frame, "", "Wasp max speed", NULL, 3, 19, 1, "%.f", B_TRIANGLE_THUMB);
	theWaspView->AddChild(gSlideWaspVel);
	gSlideWaspVel->SetValue(theSettings.wasp_vel);

// Border
	theControlTop+=48;
	frame = theWaspView->Bounds();
	frame.top += theControlTop;
	frame.left = frame.left + 20;
	frame.right = frame.right - 10;
	gSlideBorder = new TControlSlide(frame, "", "Border for wasp", NULL, 10, 100, 1, "%.f", B_TRIANGLE_THUMB);
	theWaspView->AddChild(gSlideBorder);
	gSlideBorder->SetValue(theSettings.border);

	theCredits->Invalidate();
	theTabView->Invalidate();

	inView->Window()->EnableUpdates();
	inView->Window()->UpdateIfNeeded();
	inView->Window()->Unlock();
	
	theTabView->Select(1);
	theTabView->Select(0);

	
//	gConfiguring = 1;
}
