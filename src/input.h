
// control key states - set to true when pressed, set to false when read by application
static bool aileron_left = false;     // true if left arrow pressed
static bool aileron_right = false;    // true if right arrow pressed
static bool elevator_up = false;      // true if down arrow pressed
static bool elevator_down = false;    // true if up arrow pressed
static bool rudder_right = false;     // true if '>' pressed
static bool rudder_left = false;      // true if '<' pressed
static bool throttle_up = false;      // true if plus key pressed
static bool throttle_down = false;    // true if minus key pressed
static bool ignition_change = false;  // true if 'I' (i) pressed
static bool view_forward = true;      // true if forward view key pressed
static bool view_right = false;       // true if right view key pressed
static bool view_left = false;        // true if left view key pressed
static bool view_rear = false;        // true if rear view key pressed
static bool brake_tog = false;        // true if brake key pressed

// control sensitivity in amount of motion per 1/20 second
static unsigned char stick_sens = 15;   // arbitrary value for control sensitivity
static unsigned char rudder_sens = 15;  // arbitrary value for control sensitivity

static int rudderPos;                     // current position of rudder
static int stickX;                       // current stick x and y position
static int stickY;

// this function provides a dump of the aircraft state vector values (see the
// struct definition in AIRCRAFT.H). If debug is true it is output
// with the proper header and footer for a realtime dump. If debug is false
// it is formatted to be output at the end of the program.
void VectorDump(state_vect *tSV)
{
	printf("State vector realtime dump:\n");
	ACDump();
	printf("right aileron: %i       \n", -tSV->aileron_pos);
	printf("left aileron:  %i       \n", tSV->aileron_pos);
	printf("elevator:      %i       \n", tSV->elevator_pos);
	printf("rudder:        %i       \n", tSV->rudder_pos);
	printf("throttle:      %i       \n", tSV->throttle_pos);
	printf("ignition:      %u       \n", tSV->ignition_on);
	printf("engine on:     %i       \n", tSV->engine_on);
	printf("prop rpm:      %i       \n", tSV->rpm);
	printf("fuel level:    %i       \n", tSV->fuel);
	printf("x coordinate:  %i       \n", tSV->x_pos);
	printf("y coordinate:  %i       \n", tSV->y_pos);
	printf("z coordinate:  %i       \n", tSV->z_pos);
	printf("pitch:         %f       \n", tSV->pitch);
	printf("effect. pitch: %f       \n", tSV->efAOF);
	printf("roll:          %f       \n", tSV->roll);
	printf("yaw:           %f       \n", tSV->yaw);
	printf("speed:         %f       \n", tSV->h_speed);
	printf("rate of climb: %f       \n", tSV->climbRate);
	printf("altitude:      %i       \n", tSV->altitude);
	printf("\n\n");
}

void CheckKeys(state_vect *tSV)
{
	if (RETRO_KeyPressed(SDL_SCANCODE_D)) {
		VectorDump(tSV);
	} else if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
		aileron_left = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		aileron_right = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_UP)) {
		elevator_up = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
		elevator_down = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_X)) {
		rudder_right = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_Z)) {
		rudder_left = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_PAGEUP)) {
		throttle_up = true;
	} else if (RETRO_KeyState(SDL_SCANCODE_PAGEDOWN)) {
		throttle_down = true;
	} else if (RETRO_KeyPressed(SDL_SCANCODE_I)) {
		ignition_change = true;
	} else if (RETRO_KeyPressed(SDL_SCANCODE_F1)) {
		view_forward = true;
	} else if (RETRO_KeyPressed(SDL_SCANCODE_F2)) {
		view_right = true;
	} else if (RETRO_KeyPressed(SDL_SCANCODE_F3)) {
		view_rear = true;
	} else if (RETRO_KeyPressed(SDL_SCANCODE_F4)) {
		view_left = true;
	} else if (RETRO_KeyPressed(SDL_SCANCODE_B)) {
		brake_tog = true;
	}
}

// This function is called from GetControls(). It checks the state of all
// the flight control keys, and adjusts the values which would normally be
// mapped to the joystick if it was in use. GetControls() only calls this
// function if the joystick is not being used.
void CalcKeyControls()
{
	if (aileron_left) {
		stickX += stick_sens;
		if (stickX > 127) {
			stickX = 127;
		}
		aileron_left = false;
	} else if (aileron_right) {
		stickX -= stick_sens;
		if (stickX < -128) {
			stickX = -128;
		}
		aileron_right = false;
	} else {
		stickX = 0;
	}

	if (elevator_up) {
		stickY += stick_sens;
		if (stickY > 127) {
			stickY = 127;
		}
		elevator_up = false;
	} else if (elevator_down) {
		stickY -= stick_sens;
		if (stickY < -128) {
			stickY = -128;
		}
		elevator_down = false;
	} else {
		stickY = 0;
	}
}

// this function is called from GetControls(). It calculates the state of
// the standard controls which are never mapped to the joystick. This
// function is called on every call to GetControls()
void CalcStndControls(state_vect *tSV)
{
	if (rudder_right) {
		rudderPos += rudder_sens;
		if (rudderPos > 127)
			rudderPos = 127;
		rudder_right = false;
	} else if (rudder_left) {
		rudderPos -= rudder_sens;
		if (rudderPos < -128) {
			rudderPos = -128;
		}
		rudder_left = false;
	} else {
		rudderPos = 0;
	}

	if ((throttle_up) && (tSV->throttle_pos < 15)) {
		tSV->throttle_pos++;
		throttle_up = false;
	} else if ((throttle_down) && (tSV->throttle_pos > 0)) {
		tSV->throttle_pos--;
		throttle_down = false;
	}

	if (ignition_change) {
		if (tSV->ignition_on) {
			tSV->ignition_on = false;
		} else {
			tSV->ignition_on = true;
		}
		ignition_change = false;
	}

	if (brake_tog) {
		if (tSV->brake) {
			tSV->brake = false;
		} else {
			tSV->brake = true;
		}
		brake_tog = false;
	}
}

// this function is called from GetControls(). It checks the state of the
// sound and view controls, and updates the state vector if required.
// This function is called on every call to GetControls().
void CheckViewControls(state_vect *tSV)
{
	if (view_forward) {
		view_forward = false;
		tSV->view_state = 0;
	} else if (view_right) {
		view_right = false;
		tSV->view_state = 1;
	} else if (view_rear) {
		view_rear = false;
		tSV->view_state = 2;
	} else if (view_left) {
		view_left = false;
		tSV->view_state = 3;
	}
}

// this function is called from GetControls(). It remaps the controls surface
// deflection indexes to a -15 to +15 range.
void ReduceIndices(state_vect *tSV)
{
	tSV->aileron_pos /= 7;                   // convert all to +/- 16
	if (tSV->aileron_pos > 15) {
		tSV->aileron_pos = 15;
	} else if (tSV->aileron_pos < -15) {
		tSV->aileron_pos = -15;
	}

	tSV->elevator_pos /= 7;
	if (tSV->elevator_pos > 15) {
		tSV->elevator_pos = 15;
	} else if (tSV->elevator_pos < -15) {
		tSV->elevator_pos = -15;
	}

	tSV->rudder_pos /= 7;
	if (tSV->rudder_pos > 15) {
		tSV->rudder_pos = 15;
	} else if (tSV->rudder_pos < -15) {
		tSV->rudder_pos = -15;
	}
}

// This function is called from the main program, and is passed a pointer
// to the aircraft state vector. It updates this vector based on the current
// state of the control interface.
void GetControls(state_vect *tSV)
{
	CheckKeys(tSV);
	CalcKeyControls();                   // no, get the keyboard controls
	CalcStndControls(tSV);                 // go get the standard controls
	CheckViewControls(tSV);

	tSV->aileron_pos = stickX;               // update the state vector
	tSV->elevator_pos = stickY;              // with values calculated in
	tSV->rudder_pos = rudderPos;             // the other functions
	ReduceIndices(tSV);                    // remap the deflections
}
