// BeSwarm implementatition

//#include <stdlib.h>
//#include <support/Debug.h>
#include "XSwarm.h"
#include "colors.h"

#define NO_TIME


unsigned long idum=0;

inline unsigned long lrand()
{
	// quick and dirty linear congruential random number generator
	idum=1664525L*idum+1013904223L;
	
	// this is a touch of my own, needed to prevent visible patterns in the static
	if(idum&0x00800200)
		idum=1664525L*idum+1013904223L;
	return idum;
}

XSwarm::XSwarm (BView* inView, SwarmSettings* theSettings)
{
	drawView = inView;
	
	// read settings
	bees = theSettings->bees;
	wasps = theSettings->wasps;
	wasp_vel = theSettings->wasp_vel;
	bee_vel = theSettings->bee_vel;
	wasp_acc = theSettings->wasp_acc;
	bee_acc = theSettings->bee_acc;
	delay = theSettings->delay;
	border = theSettings->border;
	bCycleCols = theSettings->bCycleCols;
	colCycleChange = theSettings->colCycleChange;
	bHyperSpace = theSettings->bHyperSpace;
	hypertime = theSettings->hypertime;
	bHeatMap = theSettings->bHeatMap;

	bg_color = black;	// black
	wasp_color = white;	// white
	bee_color =  red;	// red 

	// initialize
	init = true;
	
	// allocate arrays
	xy = new BPoint[3*bees];
	xyv = new BPoint[bees];
	wxy = new BPoint[3*wasps];
	wxyv = new BPoint[wasps];
	distxy = new int[bees];
	
	color_list = (rgb_color*) malloc (sizeof (rgb_color) * MAX_COLS);
	hyper_frames = (int*) malloc (sizeof (int) * wasps);
	
	frames = 0.0;
	time = 0.0;
	drawTime = 0.0;
	
	// initialize color cycling table
	int idx = 0;
	int rgbTab[3] = {255, 0, 0};
	int rgbIdx = 0;
	int rgbRunTab[6] = {1, 0, 2, 1, 0, 2};
	int step = 255 / 6;
	for (idx = 0; idx < MAX_COLS; idx++)
	{
		color_list[idx].red = (uchar) rgbTab[0];
		color_list[idx].green =  (uchar) rgbTab[1];
		color_list[idx].blue = (uchar) rgbTab[2];
		color_list[idx].alpha =  (uchar) 0;
		
		rgbTab[rgbRunTab[rgbIdx]] += step;
		if (rgbTab[rgbRunTab[rgbIdx]] > 255)
		{
			rgbTab[rgbRunTab[rgbIdx]] = 255;
			step = -step;
			rgbIdx++;
		}
		else if (rgbTab[rgbRunTab[rgbIdx]] < 0)
		{
			rgbTab[rgbRunTab[rgbIdx]] = 0;
			step = -step;
			rgbIdx++;
		}
		if (rgbIdx > 5) rgbIdx = 0;
	}
}

XSwarm::~XSwarm ()
{
	// free any allocated memory
	free (hyper_frames);
	free (color_list);
	delete[] xy;
	delete[] xyv;
	delete[] wxy;
	delete[] wxyv;
	delete[] distxy;

#ifndef NO_TIME
	
	if (time > 0.0)
	{
		printf ("Frames: %f\nTime: %f\nusecs per frame: %f\n\nFrames per second: %f\n",
			frames, time, time / frames, 1e6 / (time / frames));
		printf ("\nFps drawing: %f\n", 1e6 / (drawTime / frames));
	}
#endif
}

long XSwarm::entry_func(void *arg)
{
	XSwarm *obj = (XSwarm*) arg;
	return (obj->Animate());
}

long XSwarm::Animate(void)
{
#ifndef NO_TIME
	double 	stime, etime, dtime;
#endif
	float	winH, winW;
	register int		b;
	register int		w;
	int		cycles;
	int		hyper_cycles;
	
	/* Here the animation is done */ 
	drawView->Window()->Show();
	drawView->Window()->Lock();
	
	// Set Pen Size and color
	drawView->SetPenSize (1.0);
	drawView->SetHighColor (bg_color);
	drawView->FillRect (drawView->Bounds());

	drawView->SetLowColor (bg_color);
	drawView->SetHighColor (wasp_color);
	
	
	// get view width and heigth 
    winH = drawView->Bounds().bottom;
	winW = drawView->Bounds().right;
	
	// initialize color cycling
	col_idx = 0;
	cycles = 0;
	
	// initialize hyperspace
	hyper_cycles = 0;
	for (w = 0; w < wasps; w++)
	{
		// hyperspace after 0 to 20 seconds
		hyper_frames[w] = random() %  (hypertime * (int) (1000000.0 / delay));
		if (hyper_frames[w] == 0) 
			hyper_frames[w] = 1;
	}
	
	/* Initialize point positions, velocities, etc. */

    /* wasp */
    for (w = 0; w < wasps; w++)
    {
		WXY(0,w).Set ((float) ((int)border + random() % (int)(winW - 2.0*border)),
	    			(float) ((int)border + random() % (int)(winH - 2.0*border)));
	    WXY(1,w) = WXY(0,w);
	    
	    wxyv[w] = BPoint (0.0, 0.0);
	}
	
    /* bees */
    for (b = 0 ; b < bees ; b++)
    {
    	XY(0,b) = BPoint (random() % (int) winW, random () % (int) winH);
    	XY(1,b) = XY (0, b);
    	xyv[b] = BPoint (RAND(7), RAND(7));

//		X(0,b) = random() % winW;
//		X(1,b) = X(0,b);
//		Y(0,b) = random() % winH;
//		Y(1,b) = Y(0,b);
//		xv[b] = RAND(7);
//		yv[b] = RAND(7);
    }

	drawView->Window()->Unlock();

	/* Delay so we don't use all of the cpu time. */
    while (acquire_sem_etc(termsem, 1, B_TIMEOUT, delay) == B_TIMED_OUT)
	{
		drawView->Window()->Lock();

#ifndef NO_TIME
		frames += 1.0;
		
		stime = system_time();
#endif

		// increment cycle count
		cycles++;
		hyper_cycles++;
		
		/* <=- Wasp -=> */
		/* Age the position arrays. */
		// this could be much better done through a memcopy
		// but speed is not factor here
		for (w = 0; w < wasps; w++)
		{
			WXY(2,w) = WXY(1,w);
			WXY(1,w) = WXY(0,w);
	// 		wxy[2] = wxy[1];
	//		wxy[1] = wxy[0];
			
	//		wx[2] = wx[1];
	//		wx[1] = wx[0];
	//		wy[2] = wy[1];
	//		wy[1] = wy[0];
		
			/* Accelerate */
			wxyv[w] += BPoint ((float) RAND((int)wasp_acc), (float) RAND((int)wasp_acc));
//			wxyv += BPoint ((float) RAND((int)wasp_acc), (float) RAND((int)wasp_acc));
	//		wxv += RAND(wasp_acc);
	//		wyv += RAND(wasp_acc);
		
		    /* Speed Limit Checks */
		    if (wxyv[w].x > wasp_vel) wxyv[w].x = wasp_vel;
		    if (wxyv[w].x < -wasp_vel) wxyv[w].x = -wasp_vel;
		    if (wxyv[w].y > wasp_vel) wxyv[w].y = wasp_vel;
		    if (wxyv[w].y < -wasp_vel) wxyv[w].y = -wasp_vel;
	
	//	    if (wxv > wasp_vel) wxv = wasp_vel;
	//	    if (wxv < -wasp_vel) wxv = -wasp_vel;
	//	    if (wyv > wasp_vel) wyv = wasp_vel;
	//	    if (wyv < -wasp_vel) wyv = -wasp_vel;
	
		    /* Move */
		    // wxy[0] = wxy[1] + wxyv;
		    WXY(0,w) += wxyv[w];		// this is the same as the upper line
		    							// and should be faster
		    
	//	    wx[0] = wx[1] + wxv;
	//	    wy[0] = wy[1] + wyv;
	
	// since we donÕt have any other windows on screen we can 
	// ignore this
	//	    Bounce_Against_Windows(wx, wy, &wxv, &wyv, BehindWindow);
	
		    /* Bounce Checks */
		    if ((WXY(0,w).x < border) || (WXY(0,w).x > winW-border-1.0))
		    {
				wxyv[w].x = -wxyv[w].x;
				WXY(0,w).x += wxyv[w].x*2.0;
		    }
		    if ((WXY(0,w).y < border) || (WXY(0,w).y > winH-border-1.0))
		    {
				wxyv[w].y = -wxyv[w].y;
				WXY(0,w).y += wxyv[w].y*2.0;
		    }
		}
			
		/* Don't let things settle down. */
		if (bees > 0) /* Avoid division by 0. */
		{
			xyv[random() % bees] += BPoint (RAND(5), RAND(5));

//		    xv[random() % bees] += RAND(3);
//		    yv[random() % bees] += RAND(3);
		}
	
		/* <=- Bees -=> */
		for (b = 0 ; b < bees ; b++)
		{
		    /* Age the arrays. */
		    XY(2,b) = XY(1,b);
		    XY(1,b) = XY(0,b);
		    
//		    X(2,b) = X(1,b);
//		    X(1,b) = X(0,b);
//		    Y(2,b) = Y(1,b);
//		    Y(1,b) = Y(0,b);
	
		    /* Accelerate */
		    BPoint dxy;
		    int distance = 1000000;
		    
		    // find nearest wasp
		    for (w = 0; w < wasps; w++)
		    {
		    	BPoint dXY  = WXY(1,w) - XY(1,b);
		    	
		    	int dist = abs(dXY.x) + abs (dXY.y);  /* approximation */
		    	if (dist < distance)
		    	{
		    		distance = dist;
		    		dxy = dXY;
		    	}
		    }
		    if (distance == 0) distance = 1;
		    distxy[b] = distance;
		    xyv[b].x += (dxy.x*bee_acc)/distance;
		    xyv[b].y += (dxy.y*bee_acc)/distance;
		    
//		    dx = wx[1] - X(1,b);
//		    dy = wy[1] - Y(1,b);
//		    distance = abs(dx)+abs(dy); /* approximation */
//		    if (distance == 0) distance = 1;
//		    xv[b] += (dx*bee_acc)/distance;
//		    yv[b] += (dy*bee_acc)/distance;
	
		    /* Speed Limit Checks */
		    if (xyv[b].x > bee_vel) xyv[b].x = bee_vel;
		    if (xyv[b].x < -bee_vel) xyv[b].x = -bee_vel;
		    if (xyv[b].y > bee_vel) xyv[b].y = bee_vel;
		    if (xyv[b].y < -bee_vel) xyv[b].y = -bee_vel;
	
//		    if (xv[b] > bee_vel) xv[b] = bee_vel;
//		    if (xv[b] < -bee_vel) xv[b] = -bee_vel;
//		    if (yv[b] > bee_vel) yv[b] = bee_vel;
//		    if (yv[b] < -bee_vel) yv[b] = -bee_vel;
	
		    /* Move */
		    XY(0,b) = XY(1,b) + xyv[b];
		    
//		    X(0,b) = X(1,b) + xv[b];
//		    Y(0,b) = Y(1,b) + yv[b];
	
		    /* Fill the segment lists. */
//		    segs[b].x1 = X(0,b);
//		    segs[b].y1 = Y(0,b);
//		    segs[b].x2 = X(1,b);
//		    segs[b].y2 = Y(1,b);
//		    old_segs[b].x1 = X(1,b);
//		    old_segs[b].y1 = Y(1,b);
//		    old_segs[b].x2 = X(2,b);
//		    old_segs[b].y2 = Y(2,b);
		}
	
		/* Erase previous, draw current, sync for smoothness. */

#ifndef NO_TIME
		dtime = system_time();
#endif
			
		/* Wasp */
	    if (init == FALSE)
	    {
	    	drawView->BeginLineArray (wasps);
	    	for (w = 0; w < wasps; w++)
	    	{
		    	drawView->AddLine (WXY(1,w), WXY(2,w), bg_color);
		    }
	    	drawView->EndLineArray();
//			XDrawLine(display.dpy, display.win, display.erase_gc,
//			    wx[1], wy[1], wx[2], wy[2]);
		}
		
		// now check for hyperspace
		if (bHyperSpace)
		{
	    	for (w = 0; w < wasps; w++)
	    	{
	    		if (hyper_frames[w] == hyper_cycles)
	    		{
	    			// hyperspace this wasp
					WXY(0,w).Set ((float) ((int)border + random() % (int)(winW - 2.0*border)),
				    			(float) ((int)border + random() % (int)(winH - 2.0*border)));
				    WXY(1,w) = WXY(0,w);
				    WXY(2,w) = WXY(0,w);
				    
				    // adjust hyper_cycles for other wasps
				    for (int ww = 0; ww < wasps; ww++)
				    {
				    	hyper_frames[ww] -= hyper_cycles;
				    }
				    
				    hyper_frames[w] = random() %  (hypertime * (int) (1000000.0 / delay)) ;
				    
				    // reset hyper_cycles
				    hyper_cycles = 0;
	    		}
			}
		}
					
		drawView->BeginLineArray(wasps);
    	for (w = 0; w < wasps; w++)
    	{
			drawView->AddLine (WXY(0,w), WXY(1,w), wasp_color);
		}
		drawView->EndLineArray();
		
//	    XDrawLine(display.dpy, display.win, display.wasp_gc,
//			wx[0], wy[0], wx[1], wy[1]);
	
		/* Bees */
	    if (init == FALSE)
	    {
	    	drawView->BeginLineArray (bees);
	    	for (b=0; b < bees; b++)
	    	{
	    		drawView->AddLine (XY(1,b), XY(2,b), bg_color);
	    	}
	    	drawView->EndLineArray();
//			XDrawSegments(display.dpy, display.win, display.erase_gc,
//			    old_segs, bees);
		}
	    else
			init = FALSE;

		if (cycles >= colCycleChange)
		{
			if (bCycleCols)
			{
				bee_color = color_list[col_idx++];
				if (col_idx >= MAX_COLS) col_idx = 0;
			}
			cycles = 0;
		}

    	drawView->BeginLineArray (bees);
    	if (bHeatMap)
    	{
	    	for (b=0; b < bees; b++)
	    	{
		   		rgb_color dc;
		   		dc = color_list[(col_idx + (distxy[b]<50?distxy[b]/5:10)) % MAX_COLS];
	    		drawView->AddLine (XY(0,b), XY(1,b), dc);
	    	}
    	}
		else
		{
	    	for (b=0; b < bees; b++)
	    	{
	    		drawView->AddLine (XY(0,b), XY(1,b), bee_color);
	    	}
		}
    	drawView->EndLineArray();
    	
 //	    XDrawSegments(display.dpy, display.win, display.bee_gc, segs, bees);
	
		// do we have anything equal ??
		drawView->Sync();
#ifndef NO_TIME
		etime = system_time();
		
		time += (etime - stime);
		drawTime += (etime - dtime);
#endif
		
//		XSync(display.dpy, False);
		
//	#ifdef HP_UX
//		if (delay != 0) usleep(delay);
//	#else
//		if (delay != 0) nap(0,delay);
//	#endif
		drawView->Window()->Unlock();

	}

// drawView->Window()->Unlock();

	return 0;
}

long XSwarm::Go(void)
{
	my_thread = spawn_thread(entry_func, "BeSwarm animator", B_NORMAL_PRIORITY, this);
	termsem = create_sem(0, "termination");
	return (resume_thread(my_thread));
}

long XSwarm::Stop(void)
{
	int32	ret;
	release_sem(termsem);
	wait_for_thread(my_thread, &ret);
	delete_sem(termsem);
	return ret;
}

int XSwarm::random()
{
	return rand();
}