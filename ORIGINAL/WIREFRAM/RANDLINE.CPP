
// RANDLINE.CPP
//
// A program to draw random lines on the mode 13h
// display using the line drawing function in the
// file BRESNHAM.ASM.
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	<stdlib.h>
#include	<time.h>
#include	"bresnham.h"
#include	"screen.h"

const COLOR=15;								    // Set line color
                                  //  (15=white)

void main()
{
	randomize();										// Initialize random
                                  //  numbers
	char far *screen =
    (char far *)MK_FP(0xa000,0);	// Point to video RAM
	int oldmode =                   // Save previous video
    *(int *)MK_FP(0x40,0x49);			//  mode
	cls(screen);										// Clear mode 13h
                                  //  display;
	setmode(0x13);									// Set 320x200x256-color
                                  //  graphics
	int x1=random(320);							// Initialize line to
                                  //  random
	int y1=random(200);							// ...starting values
	while (!kbhit()) {							// Draw lines until
                                  //  keypress
		int x2=random(320);						// Continue line to
		int y2=random(200);						// ...random end point
		linedraw(x1,y1,x2,y2,COLOR,screen); // Draw line
		x1=x2;												// Start next line at
		y1=y2;												// ...end point of last
                                  //  line
	}
	setmode(oldmode);								// Reset previous video
                                  //  mode & end
}
