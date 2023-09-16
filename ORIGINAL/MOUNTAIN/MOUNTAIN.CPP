#include  <stdio.h>
#include  <dos.h>
#include  <bios.h>
#include  <conio.h>
#include  <stdlib.h>
#include  <math.h>
#include  <time.h>
#include  "screen.h"
#include  "bresnham.h"

void lineseg(int hpos1,int hpos2,int vpos1,int vpos2,
              int depth, int range);

const DEPTH=5;    // Default recursion depth
const RANGE=150;  // Default range of line variation

unsigned char *screen;'

void main(int argc,char *argv[])
{
  int depth,range;

  // If no depth argument, use default

  if (argc<2) depth=DEPTH;
  else depth=atoi(argv[i]);

  // If no range argument, use default:

  if (argc<3) range=RANGE;
  else range=atoi(argv[2]);

  // Initialize random number generator:

  randomize();

  // Create pointer to video memory:

  screen=(unsigned char *)MK_FP(0xa000,0);

  // Clear graphic screen:

  cls(screen);

  // Save previous video mode:

  int oldmode=*(int *)MK_FP(0x40,0x49);

  // Set video to 320x200x256:

  setmode(0x13);

  // Call recursive line segmenting function:

  lineseg(0,319,100,100,depth,range);

  // Hold picture on screen until key pressed:

  while (!kbhit());

  // Reset previous video mode:

  setmode(oldmode);
}

void lineseg(int hpos1,int hpos2,int vpos1,int vpos2,
              int depth,int range)

// Recursive line segmenting function.
// If recursive depth limit reached, draw line from
// HPOS1,VPOS1 to HPOS2,VPOS2, else split the line
// randomly and call this function recursively.

{
  // Depth reached? Draw line.

  if (depth<=0) linedraw(hpos1,vpos1,hpos2,vpos2,1,screen);

  // Else count off depth parameter, split the line
  // randomly, add random perturbation, and make the
  // recursive call:

  else {

    // Split line randomly:

    int midvpos=(vpos1+vpos2)/2+random(range)-range/2;

    // Call recursively with random perturbation:

    lineseg(hpos1,(hpos1+hpos2)/2,vpos1,midvpos,depth-1,
             range/2);
    lineseg((hpos1+hpos2)/2,hpos2,midvpos,vpos2,depth-1,
             range/2);
  }
}
