
// OPTDEMO.CPP
//  Demonstrate optimized polygon-fill graphics animation
//  code

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "poly.h"
#include  "drawpoly.h"
#include  "loadpoly.h"
#include  "screen.h"
#include	"pcx.h"

const XORIGIN=80;
const YORIGIN=80;
const WIND_WIDTH=7*21;
const WIND_HEIGHT=7*21;

world_type world;
pcx_struct background;
Pcx bgloader;

void main(int argc,char* argv[])
{
	int key;
	float xangle=0,yangle=0,zangle=0; // X,Y&Z angles of
                                    //  object
	float xrot=0,yrot=0,zrot=0;       // X,Y&Z rotations
	unsigned char *screen_buffer;     // Offscreen drawing
                                    //  buffer

  // Read arguments from command line. If wrong number,
  //  print message and abort:

	if (argc!=2) {
		puts("Wrong number of arguments.\n");
		exit(0);
	}

  // Load background image:

	if (bgloader.load("3dbg2.pcx",&background)) {
		puts("Cannot load PCX file.\n");
		exit(0);
	}
	loadpoly(&world,argv[1]);               // Load objects
	screen_buffer=new unsigned char[64000]; // Create buffer
	int oldmode=*(int *)MK_FP(0x40,0x49);	  // Save previous
                                          //  video mode
	setmode(0x13);										      // Set mode 13h
	setpalette(background.palette,0,256);

	for(long i=0; i<64000; i++)             // Put background
    screen_buffer[i]=background.image[i]; //  in buffer

	int curobj=0;                           // First object
  int scalefactor=1;
  int zdistance=600;
	while (key!=27) {
		clrwin(10,8,WIND_WIDTH,WIND_HEIGHT,screen_buffer);
		inittrans();          // Initialize transformations
		scale(scalefactor);   // Create scaling matrix
		rotate(xangle,yangle,zangle); // Create rotation matrix

    // Rotate object one increment:

		xangle+=xrot;
		yangle+=yrot;
		zangle+=zrot;

    // Check for 256 degree wrap around:

		if (xangle>255) xangle=0;
		if (xangle<0) xangle=255;
		if (yangle>255) yangle=0;
		if (yangle<0) yangle=255;
		if (zangle>255) zangle=0;
		if (zangle<0) zangle=255;

    // Translate object:

		translate(0,0,zdistance);

    // Transform object with matrix:

		transform(&world.obj[curobj]);

    // Do perspective projection:

		project(&world.obj[curobj],400,XORIGIN,YORIGIN);

    // Draw the object:

		draw_object(world.obj[curobj],screen_buffer);

    // Put it on the video display:

		putwindow(0,0,320,200,screen_buffer);

    // Watch for user input:

		if (kbhit()) {  // If input received....
			key=getch();  // Read the key code
			switch(key) {
				case 13:

          // ENTER: Go to next object

					curobj++;
					if (curobj>=world.number_of_objects)
						curobj=0;
					break;

				case 55:

          // "7": Speed up x rotation

					xrot++;
					break;

				case 52:

          // "4": Stop x rotation

					xrot=0;
					break;

				case 49:

          // "1": Slow down x rotation

					--xrot;
					break;

				case 56:

          // "8": Speed up y rotation

					yrot++;
					break;

				case 53:

          // "5": Stop y rotation

					yrot=0;
					break;

				case 50:

          // "2": Slow down y rotation

					--yrot;
					break;

				case 57:

          // "9": Speed up z rotation

					zrot++;
					break;

				case 54:

          // "6": Stop z rotation

					zrot=0;
					break;

				case 51:

          // "3": Slow down z rotation

					--zrot;
					break;

				case '+':

          // "+": Increase distance

					zdistance+=30;
					break;

				case '-':

          // "-": Decrease distance

					if (zdistance>530) zdistance-=30;
					break;
			}
		}
	}
	setmode(oldmode); // Reset video and exit
}
