// TcontrolSlide.cpp
#include "TControlSlide.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <Message.h>

#include "colors.h"

TControlSlide::TControlSlide (BRect frame, const char* name,
		const char* label, BMessage* message,
		int32 minValue, int32 maxValue,
		int32 factor, const char* format,
		thumb_style thumbType,
		uint32 resizingMode,
		uint32 flags)
    : BSlider ( frame, name, label, message, minValue, maxValue,
    			thumbType, resizingMode, flags)
{
	if ( factor == 0)
		mFactor = 1;
	else
		mFactor = factor;
                     	
	mFormat = new char [strlen(format)+1]; 
	strcpy(mFormat,format);

	SetHashMarks(B_HASH_MARKS_BOTTOM);
	if (maxValue - minValue < 9)
		SetHashMarkCount (maxValue - minValue + 1);
	else
		SetHashMarkCount (9);
	
	char	dummy[16];
	char	dummy2[16];
	sprintf (dummy, "%ld", minValue);
	sprintf (dummy2, "%ld", maxValue);
	SetLimitLabels(dummy, dummy2);

	// bar colors
	SetBarColor (blue);
	UseFillColor (true, &red);
}

TControlSlide::~TControlSlide()
{
	delete mFormat;
}
	
char* TControlSlide::UpdateText(void) const
{
	memset (m_theValueString, 0, sizeof (m_theValueString));
	sprintf(m_theValueString, mFormat, (float) Value() / mFactor);
	return m_theValueString;
}
