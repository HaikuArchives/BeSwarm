// BeSwarm class header file
#include <interface/Point.h>


/* Constants */
#define BEES	100	/* number of bees */
#define WASPS	2	/* number of wasps */ 
#define TIMES	3	/* number of time positions recorded */
#define BEEACC	3	/* acceleration of bees */
#define WASPACC 5	/* maximum acceleration of wasp */
#define BEEVEL	11	/* maximum bee velocity */
#define WASPVEL 12	/* maximum wasp velocity */
#define DELAY	20000	/* microsecond delay between updates */
#define WINWIDTH 512	/* default window width */
#define WINHEIGHT 512	/* default window height */
#define BORDER	50	/* wasp won't go closer than this to the edges */
#define CYCLECOLS	true	/* color cycling on */
#define HYPERSPACE	false	/* hyperspace wasp after random frames */
#define HEATMAP		false	/* heat map: change color if near to target */
#define COLORFRAMES	10		/* color cycling every 10th frame */
#define BORDER	50	/* wasp won't go closer than this to the edges */
#define HYPERTIME	30	/* range of seconds in which a hyperspace jump occurs */


#define MAX_COLS	240	/* colors in color list. */ 

/* Macros */
#define X(t,b)	(x[t*bees + b])		/* addressing into dynamic array */
#define Y(t,b)	(y[t*bees + b])		/* addressing into dynamic array */
//#define RAND(v)	((random() % v) - (v/2)) /* random number around 0 */
#define RAND(v)	((v%2!=0)?(random() % v) - (v/2):\
				          (random() % v) + 0.5 - (v/2)) /* random number around 0 */
				
//#define RAND(v)	((v%2!=0)?(float) ((rand()) * v / RAND_MAX) - (v/2):\
//				          (float) ((rand()) * v / RAND_MAX) - (v/2)+1) /* random number around 0 */

#define XY(t,b) (xy[t*bees+b])		/* addressing into dynamic BPoint array */
#define WXY(t,b) (wxy[t*wasps+b])	/* addressing into dynamic BPoint array */

/* Magics */
#define BESWARM_MAGIC		'xbSW'
#define BESWARM_VERSION		0x0130
#define BESWARM_TEXT_VER	"1.3"

// Messages
#define BESWARM_DEFAULTS	'xbDf'



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

class XSwarm 
{
public:
	XSwarm (BView* inView, SwarmSettings* theSettings);
	~XSwarm (void);
	long Go (void);
	long Stop (void);

private:

	static long entry_func (void *arg);
	long 		Animate (void);
	thread_id 	my_thread;
	sem_id 		termsem;
	int 		random();
	
	BView*		drawView;	// the view for drawing

// settings
	int			bees;		// number of bees
	int			wasps;		// number of wasps
	float		wasp_vel;	// maximum wasp speed
	float		bee_vel;	// maximum bee speed
	float		wasp_acc;	// maximum wasp acceleration
	float		bee_acc;	// maximum bee acceleration
	float		delay;		// delay between updates, in microseconds
	float		border;		// border limiting wasp travel
	bool		bCycleCols;	// cycle colors
	bool		bHyperSpace;	// hyperspace enabled
	int			hypertime;		// hyperspace after max. X seconds
	bool		bHeatMap;		// heat mapping is on

// control variables	
	rgb_color	wasp_color;	// color of wasp
	rgb_color	bee_color;	// color of bees
	rgb_color	bg_color;	// background color (normally black)
	
	rgb_color*	color_list;	// array of cycle colors
	int			col_idx;	// color index
	int			colCycleChange;		// animation cycles after which color is cycled
	int*		hyper_frames;	// hyperspace after x frames (per wasp)
	

// variables for the swarm

    BPoint		*xy;		/* bee positions xy[time][bee#] */
    BPoint		*xyv;		/* bee velocities */
    BPoint		*wxy;		/* wasp positions wxy [time][wasp]*/
	BPoint		*wxyv;		/* wasp velocity */
	int			*distxy;	/* distance for every bee */

    bool		init;		/* initial drawing has taken place */
    
    double 		frames;		/* frames drawed */
    double		time;		/* time used for calculating and drawing */
    double		drawTime;	/* time used only for drawing only */
};

