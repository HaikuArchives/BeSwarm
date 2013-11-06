// ============================================================
//  SaverModule.h	©1996 Hiroshi Lockheimer
// ============================================================

#include "TControlSlide.h"

#pragma once


// Export the following six functions
// You don't need to (and therefore should not) export anything else


#pragma export on


extern "C" {

// Uncomment the following line and the corresponding line in
// "SaverModule.cpp" to prevent 'Random' from calling your module

//extern char inhibit_random;


// The following two functions are optional
// saver will load your module even if they are not present

extern void module_initialize(void *inSettings, long inSettingsSize);
extern void module_cleanup(void **outSettings, long *outSettingsSize);


// The following four funtions are not optional
// saver will not load your module if not all of the following 
// four functions are present, so be sure to export them all
// even if you have no use for some of them 

extern void	module_start_saving(BView *inView);
extern void	module_stop_saving();
extern void module_start_config(BView *inView);
extern void module_stop_config();

}


#pragma export reset


// Put prototypes local to your module here
void SetSettings();
void GetSettings();
filter_result filter_msg (BMessage* message, BHandler **target, 
						   BMessageFilter *messageFilter);

TControlSlide	*gSlideMaxBees;
TControlSlide	*gSlideBeeVel;
TControlSlide	*gSlideBeeAcc;
TControlSlide	*gSlideWaspVel;
TControlSlide	*gSlideWaspAcc;
TControlSlide	*gSlideDelay;
TControlSlide	*gSlideCycles;
TControlSlide	*gSlideBorder;
TControlSlide	*gSlideMaxWasps;
TControlSlide	*gSlideHyper;
BCheckBox		*gCycleColors;
BCheckBox		*gHyperspace;
BCheckBox		*gHeatMap;
BButton			*gDefaults;
BView			*gDemoView;