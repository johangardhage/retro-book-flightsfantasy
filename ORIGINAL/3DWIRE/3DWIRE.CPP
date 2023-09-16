
// 3DWIRE.CPP
//
// A program to draw a stationary wireframe cube
// on the mode 13h display
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"screen.h"
#include  "wire.h"

// Data for shapes:

vertex_type cube_vertices[]={  // Vertices for cube
  -10,-10,10,1,					     // Vertex 0
  0,0,0,1,
  0,0,0,1,
  10,-10,10,1,					     // Vertex 1
  0,0,0,1,
  0,0,0,1,
  10,10,10,1,						     // Vertex 2
  0,0,0,1,
  0,0,0,1,
  -10,10,10,1,					     // Vertex 3
  0,0,0,1,
  0,0,0,1,
  -10,-10,-10,1,				     // Vertex 4
  0,0,0,1,
  0,0,0,1,
  10,-10,-10,1,					     // Vertex 5
  0,0,0,1,
  0,0,0,1,
  10,10,-10,1,					     // Vertex 6
  0,0,0,1,
  0,0,0,1,
  -10,10,-10,1,					     // Vertex 7
  0,0,0,1,
  0,0,0,1,
};

line_type cube_lines[]={
  0,1,                       // Line 0
  1,2,                       // Line 1
  2,3,                       // Line 2
  3,0,                       // Line 3
  4,5,                       // Line 4
  5,6,                       // Line 5
  6,7,                       // Line 6
  7,4,                       // Line 7
  0,4,                       // Line 8
  1,5,                       // Line 9
  2,6,                       // Line 10
  3,7                        // Line 11
};

shape_type shape={

  15,	            // Color (white)
  8, 	            // Number of vertices
  12,             // Number of lines
  cube_vertices,  // Pointer to vertex array
  cube_lines      // Pointer to line array
};

// Function prototypes:

void draw_shape(shape_type shape,char far *screen);

void main()
{
  float matrix[4][4];              // Master transformation
                                   //  matrix
	char far *screen =
    (char far *)MK_FP(0xa000,0);   // Point to video RAM
	cls(screen);										 // Clear screen
	int oldmode =                    // Save previous video
    *(int *)MK_FP(0x40,0x49);			 //  mode
	setmode(0x13);									 // Set mode 13h
  inittrans();                     // Initialize matrix
  scale(2);                        // Create scaling matrix
  rotate(1.1,0.7,0.3);             // Create rotation
                                   //  matrices
  translate(0,0,100);              // Create translation
                                   //  matrix
 	transform(&shape);               // Transform SHAPE
                                   //  using MATRIX
  project(&shape,150);             // Perform perspective
                                   //  projection
  draw_shape(shape,screen);				 // Draw transformed
                                   //  shape
	while (!kbhit());								 // Wait for key,
                                   //  then terminate
	setmode(oldmode);                // Reset previous video
                                   //  mode & end
}
