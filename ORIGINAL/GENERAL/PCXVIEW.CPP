
// PCXVIEW.CPP - 1/30/93
//
// A program to display a PCX file specified at the command line.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include	<stdio.h>
#include	<stdlib.h>
#include	<conio.h>
#include	<dos.h>
#include	"screen.h"
#include	"pcx.h"

pcx_struct pcxbuf;  // Buffer for PCX data
Pcx pcxloader;      // PCX loader object

void main(int argc,char* argv[])
{
	if (argc!=2) { // Are there 2 arguments on command line?
		puts("Wrong number of arguments.\n"); // If not...
		exit(0);                              // Exit w/error
	}
	if (pcxloader.load(argv[1],&pcxbuf)) { // Get name of PCX
		puts("Cannot load PCX file.\n");     // Can't open it?
		exit(0);                             // Abort w/error
	}
  //
  // Display the PCX:
  //
	cls((char *)MK_FP(0xa000,0));         // Clear the screen
	int oldmode=*(int *)MK_FP(0x40,0x49); // Set VGA mode...
	setgmode(0x13);                       // to 320x200x256
	setpalette(pcxbuf.palette);     // Set PCX palette
  //
  // Create pointer to video display:
  //
	char far *screen=(char far *)MK_FP(0xa000,0);
  //
  // Loop through all 64,000 bytes, displaying each:
  //
	for(long i=0; i<64000; i++) screen[i]=pcxbuf.image[i];
  //
  // Hold screen until a key is pressed, then reset
  //  graphics mode and exit to DOS
  //
	while (!kbhit());  // Wait for key press
	setgmode(oldmode); // Reset old graphics mode
}
