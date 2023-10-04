
const int DEBUG = 0;              // operating mode constants
const int FLIGHT = 1;
const int WALK = 2;

// This is the aircraft model state vector type. It holds the current state
// of the aircraft, controls, attitude, and velocities, as well as the
// current view status. This struct is modified by the functions in
// aircraft.cpp, input.cpp, and fsmain.cpp. However, the only declaration
// of this type (in the current version) is in fsmain.cpp. The other modules
// get it by reference during function calls
struct state_vect
{
	int opMode;
	int aileron_pos;           // aileron position -15 to 15
	int elevator_pos;          // elevator position -15 to 15
	int throttle_pos;          // throttle position 0 to 16
	int rudder_pos;            // rudder position -15 to 15
	bool ignition_on;       // ignition state on/off (true/false)
	bool engine_on;         // engine running if true
	int rpm;                   // rpm of engine
	unsigned char fuel;                 // gallons of fuel
	unsigned char fuelConsump;          // fuel consumption in gallons/hr.
	int x_pos;                 // current location on world-x
	int y_pos;                 // current location on world-y
	int z_pos;                 // current location on world-z
	double pitch;              // rotation about x 0 to 255
	double yaw;                // rotation about y 0 to 255
	double roll;               // rotation about z 0 to 255
	float h_speed;             // horizontal speed (airspeed, true)
	float v_speed;             // vertical speed during last time slice
	float delta_z;             // z distance travelled in last pass
	float efAOF;               // effective angle of flight
	float climbRate;           // rate of climb in feet per minute
	int altitude;              // altitude in feet
	bool airborne;          // true if the plane has taken off
	bool stall;             // true if stall condition
	bool brake;             // true if brake on
	unsigned char view_state;           // which way is the view pointing
	unsigned char sound_chng;           // boolean true if sound on/off state chngd
};

// struct delta_vect is used by the aircraft modeling functions in
// aircraft.cpp as a container for the current delta values for the
// aircraft rotations.
struct delta_vect
{
	double dPitch;             // delta change in pitch (deg.) per ms
	double dYaw;               // delta change in yaw (deg.) per ms
	double dRoll;              // delta change in roll (deg.) per ms
};

static const int LOOP = 40;        // if the timer is off this value will be used for the loop timing

// this block declares various data used internally in this module

static unsigned long loopTime;               // elapsed time since last pass
static int frmIndex;                 // index into frame rate save buffer
static bool frmWrap;              // true when NUM_FRMS frame times saved
static delta_vect deltaVect;         // copy of rotational deltas

// miscellaneous constants

static const double PI = 3.1415;     // value of pi
static const int NUM_FRMS = 500;         // number of frame times to keep
static const int TANK_SIZE = 8;          // size of fuel tank (in tens of gal.)

// flight model constants

static const float GRAV_C = -16.0;   // gravitational constant in ft./sec.
static const int SEA_LVL_Y = -20;        // y value of ground level
static const int START_Y = SEA_LVL_Y;    // start-up conditions
static const int START_Z = -7000;
static const int START_X = 0;
static const int START_YAW = 0;
static const int START_ROL = 0;
static const int START_PIT = 0;

static const long WALK_RATE = 100000; // world walk speed in feet/min

static unsigned int frameTimes[NUM_FRMS];     // buffer for 500 elapsed frame times

// this function is called from main() (FSMAIN.CPP) when running in debugging dump mode.
void ACDump()
{
	float framesPerSec;

	if (loopTime) {
		framesPerSec = 1000;
		framesPerSec /= loopTime;
	} else {
		framesPerSec = 0;
	}
	printf("Physical parameters dump:\n");
	printf("delta pitch (deg./ms): %f       \n", deltaVect.dPitch);
	printf("delta yaw (deg./ms):   %f       \n", deltaVect.dYaw);
	printf("delta roll (deg./ms):  %f       \n", deltaVect.dRoll);
	printf("last frame (ms):       %lu      \n", loopTime);
	printf("frames per sec.:       %f       \n", framesPerSec);
}

// This function is called at program termination to calculate and print
// the average time between frames in millseconds
void ReportFrameRate()
{
	int i;
	int frames;

	loopTime = 0;
	if (frmWrap) {
		frames = NUM_FRMS;
	} else {
		frames = frmIndex;
	}

	printf("Average time per frame (ms):");

	if (frames) {
		for (i = 0; i < frames; i++) {
			loopTime += frameTimes[i];
		}
		loopTime /= frames;
		printf(" %lu\r\n", loopTime);
	} else {
		printf(" timer disabled\r\n");
	}
}

// this function is called from CalcNewPos() to place the last time between
// frames in milliseconds in the frameTimes[] array. The array index wraps
// at 499, so the buffer is continually overwritten. When the program
// terminates ReportFrameRate() reports the average of the last 500 elapsed
// times
void AddFrameTime()
{
	frameTimes[frmIndex] = loopTime;
	frmIndex++;
	if (frmIndex == NUM_FRMS) {
		frmIndex = 0;
		frmWrap = true;
	}
}

// sets up aircraft conditions at start
void ResetACState(state_vect *tSV)
{
	memset(tSV, 0, sizeof(*tSV));     // mem.h: clear the state vector

	// setup some initial values in the aircraft state vector
	tSV->opMode = FLIGHT;        // starting x position
	tSV->x_pos = START_X;        // starting x position
	tSV->y_pos = START_Y;        // sea level = world y -40
	tSV->z_pos = START_Z;        // starting z position
	tSV->yaw = START_YAW;        // starting direction
	tSV->pitch = START_PIT;
	tSV->roll = START_ROL;
	tSV->fuel = TANK_SIZE;       // starting fuel level/ units of 10
	tSV->fuelConsump = 8;        // consumption in gallons/hr
	tSV->airborne = false;       // "on the ground" flag
	tSV->brake = true;
	deltaVect.dPitch = 0;
	deltaVect.dRoll = 0;
	deltaVect.dYaw = 0;
}

// set aircraft conditions after landing
void LandAC(state_vect *tSV)
{
	tSV->airborne = false;
	tSV->pitch = 0;
	tSV->roll = 0;
	tSV->y_pos = SEA_LVL_Y;
	deltaVect.dPitch = 0;
	deltaVect.dRoll = 0;
	deltaVect.dYaw = 0;
}

// initializes aircraft statevector and flight model, starts internal timer
void InitAircraft(state_vect *tSV)
{
	loopTime = 0;
	ResetACState(tSV);        // set the starting aircraft state
	frmWrap = false;            // flag used by frame rate accumulator
}

// converts degrees to radians - expects degrees in -179 to +180 format
double Rads(double degrees)
{
	double result;

	if (degrees < 0) {
		degrees += 360;
	}
	result = degrees * (PI / 180);
	return(result);
}

// converts radians to degrees
double Degs(double radians)
{
	double result;
	result = radians * (180 / PI);
	return(result);
}

// FLIGHT MODEL STEP 1
// This function adjusts the engine rpm for the current iteration of the
// flight model. It also toggles the engine on/off in response to changes
// in the state_vect.ignition_on parameter
void CalcPowerDyn(state_vect *tSV)
{
	if (tSV->ignition_on)                        // is the ignition on?
	{
		if (!tSV->engine_on) {                       // yes, engine running?
			tSV->engine_on = true;                   // no, turn it on
		}

		// increment or decrement the rpm if it is less than or greater than
		// nominal for the throttle setting
		if (tSV->rpm < (375 + (tSV->throttle_pos * 117))) {
			tSV->rpm += loopTime * .5;
		}
		if (tSV->rpm > (375 + (tSV->throttle_pos * 117))) {
			tSV->rpm -= loopTime * .5;
		}
	} else {                                         // no, ignition is off
		if (tSV->engine_on) {                        // is the engine running?
			tSV->engine_on = false;                  // yes, shut it off
		}
		if (tSV->rpm) {                               // rpm > 0 ?
			tSV->rpm -= loopTime / 2;                // yes, decrement it
		}
	}
	if (tSV->rpm < 0) {                             // make sure it doesn't
		tSV->rpm = 0;                               // end up negative
	}
}

// FLIGHT MODEL STEP 2
// This function calculates the flight dynamics for the current pass
// through the flight model. The function does not attempt to model actual
// aerodynamic parameters. Rather, it is constructed of equations developed
// to produce a reasonable range of values for parameters like lift, speed,
// horizontal acceleration, vertical acceleration, etc.
void CalcFlightDyn(state_vect *tSV)
{
	float iSpeed;                       // speed ideally produced by x rpm
	float lSpeed;                       // modified speed for lift calc.
	float hAccel;                       // horizontal acceleration (thrust)
	float lVeloc;                       // vertical velocity from lift
	float gVeloc;                       // vertical velocity from gravity
	float AOA;                          // angle of attack

	iSpeed = tSV->rpm / 17.5;            // calc speed from rpm
	iSpeed += (tSV->pitch * 1.5);        // modify speed by pitch

	hAccel = ((tSV->rpm * (iSpeed - tSV->h_speed)) / 10000);
	hAccel /= 1000;
	hAccel *= loopTime;

	if ((tSV->brake) && (!tSV->airborne)) {
		if (tSV->h_speed > 0) {            // brake above 0 m.p.h.
			tSV->h_speed -= 1;
		} else {
			tSV->h_speed = 0;              // settle speed at 0 m.p.h.
		}
	} else {
		tSV->h_speed += hAccel;           // accelerate normally
	}

	lSpeed = (tSV->h_speed / 65) - 1;      // force speed to range -1..1
	if (lSpeed > 1) {
		lSpeed = 1;           // truncate it at +1
	}

	lVeloc = Degs(atan(lSpeed));          // lift curve: L = arctan(V)
	lVeloc += 45;                         // force lift to range 0..90
	lVeloc /= 5.29;                       // shift to range 0..~17
	lVeloc *= (-(tSV->pitch * .157) + 1);  // multiply by pitch modifier
	lVeloc /= 1000;                       // time slice
	lVeloc *= loopTime;

	gVeloc = loopTime * (GRAV_C / 1000);      // grav. constant this loop
	tSV->v_speed = gVeloc + lVeloc;            // sum up the vertical velocity
	if ((!tSV->airborne) && (tSV->v_speed < 0)) {    // v_speed = 0 at ground level
		tSV->v_speed = 0;
	}
	tSV->climbRate = tSV->v_speed / loopTime;     // save the value in feet/min.
	tSV->climbRate *= 60000;

	tSV->delta_z = tSV->h_speed * 5280;         // expand speed to feet/hr
	tSV->delta_z /= 3600000;                   // get feet/millisecond
	tSV->delta_z *= loopTime;                  // z distance travelled

	if (tSV->delta_z) {                         // find effective angle of flight
		tSV->efAOF = -(atan(tSV->v_speed / tSV->delta_z));
	} else {
		tSV->efAOF = -(atan(tSV->v_speed));      // atan() returns radians
	}

	AOA = Degs(tSV->efAOF);                    // convert to degrees

	// handle a stalling condition
	if (((tSV->pitch < AOA) && (AOA < 0)) && (tSV->h_speed < 40)) {
		if ((tSV->pitch - AOA) < -20) {
			tSV->stall = true;
		}
	}
	if (tSV->stall) {
		if (tSV->pitch > 30) {
			tSV->stall = false;
		} else {
			tSV->pitch++;
		}
	}
}

// FLIGHT MODEL STEP 3
// This function attempts to simulate inertial damping of the angular rates
// of change. It needs a lot of work, but you can see it's effects now in
// the "momentum" effect when the aircraft is rolled
void InertialDamp()
{
	// simulates inertial damping of angular velocities
	if (deltaVect.dPitch) {
		deltaVect.dPitch -= deltaVect.dPitch / 10;
		if (((deltaVect.dPitch > 0) && (deltaVect.dPitch < .01)) || ((deltaVect.dPitch < 0) && (deltaVect.dPitch > -.01))) {
			deltaVect.dPitch = 0;
		}
	}
	if (deltaVect.dYaw) {
		deltaVect.dYaw -= deltaVect.dYaw / 10;
		if (((deltaVect.dYaw > 0) && (deltaVect.dYaw < .01)) || ((deltaVect.dYaw < 0) && (deltaVect.dYaw > -.01))) {
			deltaVect.dYaw = 0;
		}
	}
	if (deltaVect.dRoll) {
		deltaVect.dRoll -= deltaVect.dRoll / 10;
		if (((deltaVect.dRoll > 0) && (deltaVect.dRoll < .01)) || ((deltaVect.dRoll < 0) && (deltaVect.dRoll > -.01))) {
			deltaVect.dRoll = 0;
		}
	}
}

// FLIGHT MODEL STEP 4
// this function is called from CalcROC() to calculate the current turn
// rate based on roll
float CalcTurnRate(state_vect *tSV)
{
	float torque = 0.0;

	if ((tSV->roll > 0) && (tSV->roll <= 90)) {
		torque = (tSV->roll * .00050);                   // (.00026)
	} else if ((tSV->roll < 0) && (tSV->roll >= -90)) {
		torque = (tSV->roll * .00050);
	}
	return(torque);
}

// FLIGHT MODEL STEP 5
// This function finds the current rates of change for aircraft motion in
// the three axes, based on control surface deflection, airspeed, and
// elapsed time. It uses the values in the ROC lookup table at the top
// of this file
void CalcROC(state_vect *tSV)
{
	float torque;

	// load deltaVect struct with delta change values for roll, pitch, and
	// yaw based on control position and airspeed
	if (tSV->airborne) {
		if (tSV->aileron_pos != 0) {
			torque = ((tSV->h_speed * tSV->aileron_pos) / 10000);
			if (deltaVect.dRoll != (torque * loopTime)) {
				deltaVect.dRoll += torque * 6; // *8
			}
		}
	}
	if (tSV->elevator_pos != 0) {
		torque = ((tSV->h_speed * tSV->elevator_pos) / 10000);
		if ((!tSV->airborne) && (torque > 0)) {
			torque = 0;
		}
		if (deltaVect.dPitch != (torque * loopTime)) {
			deltaVect.dPitch += torque * 1.5;    //* 4
		}
	}
	if (tSV->h_speed) {
		torque = 0.0;
		if (tSV->rudder_pos != 0) {
			torque = -((tSV->h_speed * tSV->rudder_pos) / 10000);
		}
		torque += CalcTurnRate(tSV);
		if (deltaVect.dYaw != (torque * loopTime)) {
			deltaVect.dYaw += torque * 1.5;   // *8
		}
	}
}

// FLIGHT MODEL STEP 6
// This function applies the current angular rates of change to the
// current aircraft rotations, and checks for special case conditions
// such as pitch exceeding +/-i90 degrees
void ApplyRots(state_vect *tSV)
{
	// transform pitch into components of yaw and pitch based on roll
	tSV->roll += deltaVect.dRoll;
	tSV->yaw += deltaVect.dYaw;
	tSV->pitch += (deltaVect.dPitch * cos(Rads(tSV->roll)));
	tSV->yaw += -(deltaVect.dPitch * sin(Rads(tSV->roll)));

	// handle bounds checking on roll and yaw at 180 or -180
	if (tSV->roll > 180) {
		tSV->roll = -180 + (tSV->roll - 180);
	} else if (tSV->roll < -180) {
		tSV->roll = 180 + (tSV->roll - -180);
	}
	if (tSV->yaw > 180) {
		tSV->yaw = -180 + (tSV->yaw - 180);
	} else if (tSV->yaw < -180) {
		tSV->yaw = 180 + (tSV->yaw - -180);
	}

	// handle special case when aircraft pitch passes the vertical
	if ((tSV->pitch > 90) || (tSV->pitch < -90)) {
		if (tSV->roll >= 0) {
			tSV->roll -= 180;
		} else if (tSV->roll < 0) {
			tSV->roll += 180;
		}
		if (tSV->yaw >= 0) {
			tSV->yaw -= 180;
		} else if (tSV->yaw < 0) {
			tSV->yaw += 180;
		}
		if (tSV->pitch > 0) {
			tSV->pitch = (180 - tSV->pitch);
		} else if (tSV->pitch < 0) {
			tSV->pitch = (-180 - tSV->pitch);
		}
	}

	// dampen everything out to 0 if they get close enough
	if ((tSV->pitch > -.5) && (tSV->pitch < .5)) {
		tSV->pitch = 0;
	}
	if ((tSV->roll > -.5) && (tSV->roll < .5)) {
		tSV->roll = 0;
	}
	if ((tSV->yaw > -.5) && (tSV->yaw < .5)) {
		tSV->yaw = 0;
	}
}

// WORLD WALK MODE ONLY
// this function controls the movement of the view center when the
// program is running in world walk mode. In this mode the flight
// controls move you around the world. This is useful for inspection
// when designing scenery.
void DoWalk(state_vect *tSV)
{
	tSV->delta_z = tSV->throttle_pos * (WALK_RATE / 15);
	tSV->delta_z /= 60000;
	tSV->delta_z *= loopTime;
	tSV->efAOF = Rads(tSV->pitch);
	tSV->h_speed = 140;
}

// ENTRY POINT FOR FLIGHT MODEL LOOP
// This function takes as parameters references to a state_vect structure
// containing the control input from the current pass, as well as the
// values for all other aircraft data from the previous pass.
//
// *A special thanks to Peter Rushworth, who called me from England at 4:00
// *in the morning (his time) to help me work on the pitch and roll component
// *calculations.
//
// DEBUGGING NOTE: If __TIMEROFF__ is defined (see top of this module) then
// this function sets the loopTime variable to LOOP ms, else it uses the timer1
// object to time the running of the flight model. The loopTime variable
// is set on entry to this module with the number of elapsed ms since the
// last call, and then used throughout the module for calculations of rate
// of change parameters. Defining __TIMEROFF__ effectively sets the
// performance of the system to match my 486. This lets you step through the
// flight model and get a nice, smooth change in the variables you're
// watching. Otherwise the timer continues to run while you're staring at the
// debugger screen.
void RunFModel(state_vect *tSV)
{
	float tmpX, tmpY, tmpZ;         // these are used later to preserve
	float newX, newY, newZ;         // position values during conversion
	static float collectX;          // accumulators for delta changes in
	static float collectY;          // x, y, and z world coords; adjusts
	static float collectZ;          // for rounding errors

	loopTime = SDL_GetTicks64();
	if (!(loopTime /= 1000)) {
		loopTime = 1;
	}
	AddFrameTime();

	// these seven near calls update all current aircraft parameters
	// based on the input from the last pass through the control loop
	// The order in which they are called is critical
	if (tSV->opMode == WALK) {
		DoWalk(tSV);               // traverse the world
	} else {
		CalcPowerDyn(tSV);         // calculate the power dynamics
		CalcFlightDyn(tSV);        // calculate the flight dynamics
	}
	InertialDamp();                // apply simulated inertial dampening
	CalcROC(tSV);                  // find the current rates of change
	ApplyRots(tSV);                // apply them to current rotations

	// The rest of this function calculates the new aircraft position

	// start the position calculation assuming a point at x = 0, y = 0,
	// z = distance travelled in the last time increment, assuming that
	// each coordinate in 3-space is equivalent to 1 foot

	tmpX = 0;                       // using temps because we need the
	tmpY = 0;                       // original data for the next loop
	tmpZ = tSV->delta_z;

	// note that the order of these rotations is significant

	// rotate the point in Z
	newX = (tmpX * cos(Rads(tSV->roll))) - (tmpY * sin(Rads(tSV->roll)));
	newY = (tmpX * sin(Rads(tSV->roll))) + (tmpY * cos(Rads(tSV->roll)));
	tmpX = newX;
	tmpY = newY;

	// rotate the point in x
	newY = (tmpY * cos(tSV->efAOF)) - (tmpZ * sin(tSV->efAOF));
	newZ = (tmpY * sin(tSV->efAOF)) + (tmpZ * cos(tSV->efAOF));
	tmpY = newY;
	tmpZ = newZ;

	tSV->efAOF = Degs(tSV->efAOF);

	// rotate the point in y
	newX = (tmpZ * sin(Rads(tSV->yaw))) + (tmpX * cos(Rads(tSV->yaw)));
	newZ = (tmpZ * cos(Rads(tSV->yaw))) - (tmpX * sin(Rads(tSV->yaw)));
	tmpX = newX;
	tmpZ = newZ;

	// translate the rotated point back to where it should be relative to
	// the last position (remember, the starting point for the rotations
	// is an imaginary point at world center)

	collectX += newX;
	if ((collectX > 1) || (collectX < -1)) {
		tSV->x_pos -= collectX;
		collectX = 0;
	}

	collectY += newY;
	if ((collectY > 1) || (collectY < -1)) {
		tSV->y_pos -= collectY;
		collectY = 0;
	}

	collectZ += newZ;
	if ((collectZ > 1) || (collectZ < -1)) {
		tSV->z_pos += collectZ;
		collectZ = 0;
	}

	tSV->altitude = -(tSV->y_pos - SEA_LVL_Y);

	// set the airborne flag when we first take off
	if ((!tSV->airborne) && (tSV->altitude)) {
		tSV->airborne = true;
	}
}
