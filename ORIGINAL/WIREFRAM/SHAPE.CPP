
// SHAPE.CPP
//  A short program to draw shapes on the mode 13h
// display based on data contained in a structure
// of type SHAPE_TYPE
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"bresnham.h"
#include	"screen.h"

int const DISTANCE=10;

// Variable structures for shape data:

struct vertex_type {	    // Structure for vertices
  int x,y;	              // X & Y coordinates for vertex
};

struct shape_type {		    // Structure to hold shape data
  int color;					    // Color of shape
  int	number_of_vertices;	// Number of vertices in shape
  vertex_type *vertex;		// Array of vertex descriptor
};

// Data for triangle:

vertex_type triangle_array[]={
    160,70,		            // First vertex
    190,130,	            // Second vertex
    130,130		            // Third vertex
};
shape_type shape={

    // Triangle:

    15,	                  // Color (white)
    3,	                  // Number of vertices
    triangle_array	      // Pointer to vertex array
};

// Function prototypes:

void draw_shape(shape_type shape,char far *screen);

void main()
{
	char far *screen =
    (char far *)MK_FP(0xa000,0);	  // Create pointer to
                                    //  video RAM
	cls(screen);											// Clear screen
	int oldmode=*(int *)MK_FP(0x40,0x49); // Save previous
                                        //  video mode
	setmode(0x13);										// Set mode 13h
	draw_shape(shape,screen);					// Draw shape on
                                    //  display

	while (!kbhit());									// Wait for key, then
                                    //  terminate
	setmode(oldmode);									// Reset previous video
                                    //  mode & end
}

void draw_shape(shape_type shape,char far *screen)

// Draws the shape contained in the structure SHAPE

{

	// Loop through vertices in shape:

	for (int i=0; i<shape.number_of_vertices; i++) {

    // Calculate offset of next vertex:

		int p2=i+1;

    // Wrap to 0:

		if (p2>=shape.number_of_vertices) p2=0;

		// Draw line from this vertex to next vertex:

		linedraw(shape.vertex[i].x,shape.vertex[i].y,
              shape.vertex[p2].x,shape.vertex[p2].y,
              shape.color,screen);
	}
}
