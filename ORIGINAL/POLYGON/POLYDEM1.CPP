#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "poly.h"
#include  "drawpoly.h"
#include  "loadpoly.h"
#include  "screen.h"

world_type world;

void main(int argc,char* argv[])
{
  float xangle=0,yangle=0,zangle=0;         // X,Y&Z angles to rotate shape
  float xrot=0.1,yrot=0.1,zrot=0.1;         // X,Y&Z rotation increments
  unsigned char *screen_buffer;             // Offscreen drawing buffer

	if (argc!=2) {                            // Read command-line arguments
		puts("Wrong number of arguments.\n");   // If wrong number, print message
		exit(0);                                // ...and abort
	}
  loadpoly(&world,argv[1]);                 // Load object description(s)
  screen_buffer=new unsigned char[64000];   // Create offscreen buffer
//  screen_buffer=(unsigned char *)MK_FP(0xa000,0);
	int oldmode=*(int *)MK_FP(0x40,0x49);			// Save previous video mode
	setmode(0x13);										        // Set mode 13h
  while (!kbhit()) {                        // Loop until key is pressed
    cls(screen_buffer);                     // Clear screen buffer
    inittrans();                            // Initialize translations
    scale(1);                               // Create scaling matrix
    rotate(xangle,yangle,zangle);           // Create rotation matrices
    xangle+=xrot;                           // Increment rotation angles
    yangle+=yrot;
    zangle+=zrot;
    translate(0,0,600);                      // Create translation matrix
    transform(&world.obj[0]);               // Transform OBJECT using MATRIX
    project(&world.obj[0],400);             // Perform perspective projection
    draw_object(world.obj[0],screen_buffer);  // Draw transformed object
    putwindow(0,0,320,200,screen_buffer);   // Move buffer to video RAM
  }
	setmode(oldmode);                       // Reset previous video mode & end
}
