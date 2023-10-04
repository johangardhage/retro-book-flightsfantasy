//
// fof.c
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrogfx.h"
#include "fix.h"
#include "screen.h"
#include "poly.h"
#include "aircraft.h"
#include "viewcntl.h"
#include "view.h"
#include "input.h"
#include "loadpoly.h"
#include "drawpoly.h"
#include "gauges.h"

state_vect tSV;               // the control-state vector

// handles a ground approach by determining from pitch and roll whether
// the airplane has landed safely or crashed
void GroundApproach(state_vect *tSV)
{
	// handle approaching the ground
	if (tSV->opMode == FLIGHT) {
		if ((tSV->airborne) && (tSV->altitude <= 0)) {
			if (((tSV->pitch > 10) || (tSV->pitch < -10)) || ((tSV->roll > 10) || (tSV->roll < -10))) {
				ShowCrash();
				ResetACState(tSV);
			} else {
				LandAC(tSV);
			}
		}
	}
}

void DEMO_Render(double deltatime)
{
	GetControls(&tSV);
	RunFModel(&tSV);
	UpdateView(&tSV);
	GroundApproach(&tSV);
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/title.pcx");
	RETRO_LoadImage("assets/ckpit01.pcx");  // front view
	RETRO_LoadImage("assets/sideview.pcx");  // right view
	FlipFrame(0, 0, 319, 199, RETRO_ImageData(PCX_RIGHT));
	RETRO_LoadImage("assets/sideview.pcx");  // left view
	RETRO_LoadImage("assets/tail.pcx");  // rear view
	RETRO_LoadImage("assets/doodads.pcx");  // buttons

	RETRO_Blit(RETRO_ImageData(PCX_TITLE));

	for (int i = 0; i < 50; i++) {
		RETRO_FadeIn(50, i, RETRO_ImagePalette());
		RETRO_Flip();
		SDL_Delay(50);
	}

	SDL_Delay(100);

	for (int i = 0; i < 50; i++) {
		RETRO_FadeOut(50, i, RETRO_ImagePalette());
		RETRO_Flip();
		SDL_Delay(50);
	}

	RETRO_Blit(RETRO_ImageData(PCX_FRONT));
	RETRO_SetPalette(RETRO_ImagePalette(PCX_FRONT));

	InitView();
	SetUpACDisplay(RETRO_ImageData(PCX_DOODADS));
	InitAircraft(&tSV);
}

void DEMO_Startup(void)
{
	printf("        The Waite Group's 'Flights of Fantasy' (c) 1992\r\n");
	printf("----------------------------------------------------------------\r\n");
	printf("* view control keys: F1         - look forward\r\n");
	printf("                     F2         - look right\r\n");
	printf("                     F3         - look behind\r\n");
	printf("                     F4         - look left\r\n");
	printf("\r\n");
	printf("* engine control:    i          - toggle ignition/engine on/off\r\n");
	printf("                     pageup     - increase throttle setting\r\n");
	printf("                     pagedown   - decrease throttle setting\r\n");
	printf("\r\n");
	printf("* aircraft control:  pitch up   - down arrow\r\n");
	printf("                     pitch down - up arrow\r\n");
	printf("                     left roll  - left arrow\r\n");
	printf("                     right roll - right arrow\r\n");
	printf("                     rudder     - 'z' or 'x' keys\r\n");
	printf("                     brake      - 'b'\r\n");
}
