// TControlSlide.h

#ifndef _TCONTROL_SLIDE_H
#define _TCONTROL_SLIDE_H

#include <Rect.h>
#include <Slider.h>

class TControlSlide : public BSlider
{
public:
	TControlSlide (BRect frame, const char* name,
		const char* label, BMessage* message,
		int32 minValue, int32 maxValue,
		int32 factor = 1, const char* format = "%.2f",
		thumb_style thumbType = B_BLOCK_THUMB,
		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
		uint32 flags = B_FRAME_EVENTS|B_WILL_DRAW|B_NAVIGABLE
		);

	~TControlSlide();
	
	virtual char *UpdateText(void) const;
private:
	float			mFactor;
	long 			mMin;
	long			mMax;
	char *			mFormat;
	char			m_theValueString[64];
};

#endif