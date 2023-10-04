
#ifndef _VIEWCNTL_H_
#define _VIEWCNTL_H_

#include "gauges.h"
#include "view.h"
#include "loadpoly.h"

// offsets for view system, forward, right, rear, and left views, offset
// in degrees in the -180 to 180 system

world_type world;                    // Structure for world descriptor

static int rt2lft_ofs[4] = { 0, -90, 180, 90 };

// some constants to refer to value of current_view

enum { PCX_TITLE, PCX_FRONT, PCX_RIGHT, PCX_LEFT, PCX_REAR, PCX_DOODADS };

enum { forward, right, rear, left };

static float degree_mul;             // used to convert between degree systems

static unsigned char *crshTxt;            // pointer to crash bitmap
static int view_ofs;                 // offset used for current view
static view_type curview;   	       // Structure for view descriptor
static double acPitch;               // current aircraft rotations
static double acYaw;
static double acRoll;

// This block of declarations specifies the cockpit instrument objects

static FuelGauge *theFuelGauge;
static Altimeter *theAltimeter;
static KphDial *theKphDial;
static RpmGauge *theRpmGauge;
static SlipGauge *theSlipGauge;
static Compass *theCompass;
static IgnitionSwitch *theIgnitionSwitch;

// constants for use with 3D viewing system

static const int XORIGIN = 160;           // Screen origin in X dimension
static const int YORIGIN = 90;            // Screen origin in Y dimension
static const int FWINC_X = 159;           // 159 center of front view
static const int FWINC_Y = 73;            // 73
static const int FWIN_X1 = 0;             // upper left corner of front view
static const int FWIN_Y1 = 0;
static const int FWIN_X2 = 319;           // lower right corner of front view
static const int FWIN_Y2 = 147;

static const int AWINC_X = 160;           // center of side and rear views
static const int AWINC_Y = 100;
static const int SWIN_X1 = 0;             // upper left corner of side view
static const int SWIN_Y1 = 0;
static const int SWIN_X2 = 319;           // lower right corner of side view
static const int SWIN_Y2 = 170;

static const int RWIN_X1 = 0;             // upper left corner of rear view
static const int RWIN_Y1 = 0;
static const int RWIN_X2 = 319;           // lower right corner of rear view
static const int RWIN_Y2 = 199;           // CHRIS: this is a bug! Should be 200

static const int SKY_CLR = 11;            // palette color for sky
static const int GRND_CLR = 105;          // palette color for ground
static const int FCL_LEN = 400;           // focal plane distance

// instrument constants

static const int FUE_X = 49;              // fuel gauge location left
static const int FUE_Y = 141;             // fuel gauge location top
static const int ALT_X = 108;             // altimeter location left
static const int ALT_Y = 131;             // altimeter location top
static const int KPH_X = 171;             // speed dial location left
static const int KPH_Y = 130;             // speed dial location top
static const int RPM_X = 245;             // tach location left
static const int RPM_Y = 140;             // tach location top
static const int SLP_X = 102;             // slip gauge location left
static const int SLP_Y = 182;             // slip gauge location top
static const int COM_X = 33;              // compass location left
static const int COM_Y = 184;             // compass location top
static const int NEEDLE_CLR = 4;          // palette color for gauge needles

static const int GFX_LINE = 320;          // length of a screen line
static const int CRSH_TXT_X = 118;        // where the crash sign will go
static const int CRSH_TXT_Y = 78;
static const int CRSH_TXT_DX = 85;        // dimensions of the crash sign
static const int CRSH_TXT_DY = 24;

// instance, initialize, and test status of all the instrument objects
void GaugesSetUp(unsigned char *image)
{
	theFuelGauge = new FuelGauge(FUE_X, FUE_Y, TANK_SIZE, 0, NEEDLE_CLR);
	theAltimeter = new Altimeter(ALT_X, ALT_Y, NEEDLE_CLR);
	theKphDial = new KphDial(KPH_X, KPH_Y, NEEDLE_CLR);
	theRpmGauge = new RpmGauge(RPM_X, RPM_Y, NEEDLE_CLR);
	theSlipGauge = new SlipGauge(SLP_X, SLP_Y, image);
	theCompass = new Compass(COM_X, COM_Y, image);
	theIgnitionSwitch = new IgnitionSwitch(image);
}

// This function uses the Pcx object "loader" to get the forward cockpit
// view PCX file into ram. The image field of the struct pointed to by
// bkGround contains a far pointer to the 64,000 byte buffer created by
// the loader to hold the pcx image. This buffer will be used as the
// offscreen view buffer for the duration of execution. After the cockpit
// pcx is loaded the GaugesSetUp() function is called. This initializes the
// cockpit instrument graphics. The function then creates buffers for the
// upper and lower parts of the cockpit and grabs them from the offscreen
// view buffer.
void SetUpACDisplay(unsigned char *image)
{
	unsigned int bufSize;

	GaugesSetUp(image);
	bufSize = BufSize(44, 22, 128, 45);
	if ((crshTxt = new unsigned char[bufSize]) != NULL) {
		GetImage(44, 22, 128, 45, crshTxt, image);
	}
}

// this function is called from main() to initialize the view system, the
// pcx loader object and pcx definition structure are passed in from main(),
// and are the same ones used to load the title screen at the beginning of
// the program. Since we never deal with more than one PCX at a time we
// create a single PVC object and buffer in main() and pass them to other
// parts of the program as required.

void InitView()
{
	int polycount = loadpoly(&world, "assets/fof2.wld");
	initworld(polycount);
	degree_mul = NUMBER_OF_DEGREES;
	degree_mul /= 360;
}

// this function flips the side view image so that it looks correct in either
// the right or left cockpit views. Could be done much faster in assembler,
// but, since it's called only when the right view is called for it's not
// really worth the effort at optimization
void FlipFrame(int x1, int y1, int x2, int y2, unsigned char *buf)
{
	int i, j, ofs, lines, pix;

	unsigned char gfxLnBuf[GFX_LINE];       // line buffer for various gfx tricks

	lines = (y2 - y1) + 1;
	pix = (x2 - x1) + 1;
	for (i = 0; i < lines; i++) {
		ofs = i * pix;
		memcpy(gfxLnBuf, (buf + ofs), pix);
		for (j = (pix - 1); j > -1; j--) {
			*(buf + ofs + ((pix - 1) - j)) = gfxLnBuf[j];
		}
	}
}

// This function updates the cockpit instrument display
void UpdateInstruments(state_vect *tSV)
{
	int direction;

	theKphDial->Set(tSV->h_speed);
	theRpmGauge->Set(tSV->rpm);
	theFuelGauge->Set(tSV->fuel);
	theAltimeter->Set(tSV->altitude);

	direction = floor(tSV->yaw);
	if (direction < 0) {
		direction += 360;
	}
	if (direction) {
		direction = 360 - direction;
	}

	theCompass->Set(direction);
	theSlipGauge->Set(-(tSV->aileron_pos / 2));  // slip gauge shows controls
	theIgnitionSwitch->Set(tSV->ignition_on);

	if ((tSV->brake) && (tSV->view_state == 0)) {
		Line(23, 161, 23, 157, 12);
	} else {
		Line(23, 161, 23, 157, 8);
	}
}

// This function performs a final rotation on the view angles to get the
// proper viewing direction from the cockpit. Note that the rotation values
// in the state vector are assigned to the file scope variables acPitch,
// acRoll, and acYaw. Final view calculation is done using these variables so
// that no changes have to be made to the state vector data.
void ViewShift(state_vect *tSV)
{
	acPitch = tSV->pitch;
	acYaw = tSV->yaw;
	acRoll = tSV->roll;

	acYaw += view_ofs;
	switch (tSV->view_state) {
	case 1: {
		int temp = acRoll;
		acRoll = acPitch;
		acPitch = -temp;
	} break;
	case 2: {
		acPitch = -(acPitch);
		acRoll = -(acRoll);
	} break;
	case 3: {
		int temp = acRoll;
		acRoll = -(acPitch);
		acPitch = temp;
	} break;
	}

	// handle bounds checking on roll and yaw at 180 or -180
	if (acRoll > 180) {
		acRoll = -180 + (acRoll - 180);
	} else if (acRoll < -180) {
		acRoll = 180 + (acRoll - -180);
	}
	if (acYaw > 180) {
		acYaw = -180 + (acYaw - 180);
	} else if (acYaw < -180) {
		acYaw = 180 + (acYaw - -180);
	}

	// handle special case when aircraft pitch passes the vertical
	if ((acPitch > 90) || (acPitch < -90)) {
		if (acRoll >= 0) {
			acRoll -= 180;
		} else if (acRoll < 0) {
			acRoll += 180;
		}
		if (acYaw >= 0) {
			acYaw -= 180;
		} else if (acYaw < 0) {
			acYaw += 180;
		}
		if (acPitch > 0) {
			acPitch = (180 - acPitch);
		} else if (acPitch < 0) {
			acPitch = (-180 - acPitch);
		}
	}
}

// This function maps the rotation system used in the flight model to the
// rotations used in the 3D viewing system.
void MapAngles()
{
	// map the -180 .. 180 rotation system being used in the flight model
	// to the rotation range in NUMBER_OF_DEGREES (fix.h) used in the
	// view system. The value of degree_mul is given by NUMBER_OF_DEGREES/360,
	// and is calculated once in InitAircraft()

	if (acPitch < 0) {          // requires conversion if negative
		acPitch += 360;
	}
	acPitch *= degree_mul;

	if (acRoll < 0) {
		acRoll += 360;
	}
	acRoll *= degree_mul;

	if (acYaw < 0) {
		acYaw += 360;
	}
	acYaw *= degree_mul;

	// stuff the rotations fields of the struct we'll be passing to the
	// 3D view generation system
	curview.xangle = floor(acPitch);
	curview.yangle = floor(acYaw);
	curview.zangle = floor(acRoll);
}

// this function displays the "CRASH!" icon
void ShowCrash()
{
	PutImage(CRSH_TXT_X, CRSH_TXT_Y, (CRSH_TXT_X + CRSH_TXT_DX) - 1, (CRSH_TXT_Y + CRSH_TXT_DY) - 1, crshTxt);
	RETRO_Flip();
	SDL_Delay(5000);
}

// this function is called from main() to update the offscreen image buffer
// with the current aircraft instrument display, and view overlay. It also
// checks for changes in sound state, and toggles sound on/off in response
void UpdateView(state_vect *tSV)
{
	ViewShift(tSV);
	MapAngles();

	curview.copx = tSV->x_pos;
	curview.copy = tSV->y_pos;
	curview.copz = tSV->z_pos;

	view_ofs = rt2lft_ofs[tSV->view_state];
	if (tSV->opMode == WALK) {
		setview(AWINC_X, AWINC_Y, RWIN_X1, RWIN_Y1, RWIN_X2, RWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, RETRO.framebuffer);
	} else if (tSV->view_state == 0) {
		setview(FWINC_X, FWINC_Y, FWIN_X1, FWIN_Y1, FWIN_X2, FWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, RETRO.framebuffer);
	} else if (tSV->view_state == 1) {
		setview(AWINC_X, AWINC_Y, SWIN_X1, SWIN_Y1, SWIN_X2, SWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, RETRO.framebuffer);
	} else if (tSV->view_state == 2) {
		setview(AWINC_X, AWINC_Y, RWIN_X1, RWIN_Y1, RWIN_X2, RWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, RETRO.framebuffer);
	} else if (tSV->view_state == 3) {
		setview(AWINC_X, AWINC_Y, SWIN_X1, SWIN_Y1, SWIN_X2, SWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, RETRO.framebuffer);
	}

	display(&world, curview, 1);

	if (tSV->view_state == 0) {
		RETRO_BitBlit(RETRO_ImageData(PCX_FRONT));
	} else if (tSV->view_state == 1) {
		RETRO_BitBlit(RETRO_ImageData(PCX_RIGHT));
	} else if (tSV->view_state == 2) {
		RETRO_BitBlit(RETRO_ImageData(PCX_REAR));
	} else if (tSV->view_state == 3) {
		RETRO_BitBlit(RETRO_ImageData(PCX_LEFT));
	}

	if (tSV->opMode != WALK) {
		if (tSV->view_state == 0) {
			UpdateInstruments(tSV);
		}
	}
}

#endif
