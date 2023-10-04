
#ifndef _SCREEN_H_
#define _SCREEN_H_

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;

// set the pixel at coordinate x,y to color
// This could be translated into assembler, but is probably fast enough
void WritePixel(int x, int y, char color)
{
	RETRO.framebuffer[(y * 320) + x] = color;
}

// GetImage() grabs the pixel values in the rectangle marked by (x1,y1) on
// the top left, and (x2,y2) on the bottom right. The data is placed in buffer.
//
// NOTE: This function probably needs translating into assembler
void GetImage(int x1, int y1, int x2, int y2, unsigned char *dest, unsigned char *src = RETRO.framebuffer)
{
	int i;
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;

	for (i = 0; i < ydim; i++) {
		memcpy((void *)&dest[i * xdim], (void *)&src[(y1 + i) * 320 + x1], xdim);
	}
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// PutImage() copies the data in *buff to a rectangular area of the screen
// marked by (x1,y1) on the top left, and (x2,y2) on the bottom right.
//
// NOTE: This function probably needs translating into assembler
//-------+---------+---------+---------+---------+---------+---------+---------+
void PutImage(int x1, int y1, int x2, int y2, unsigned char *buffer)
{
	int i;
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;

	for (i = 0; i < ydim; i++) {
		memcpy((void *)&RETRO.framebuffer[(y1 + i) * 320 + x1], (void *)&buffer[i * xdim], xdim);
	}
}

// this function calls the linedraw function in BRESNHAM.ASM. This function
// is required as a gateway because the modules calling Line() do not know
// where the graphics buffer is actually located. The location of the
// buffer is stored in the static locations graphSeg and graphOff in this
// module, and set via calls to SetGfxBuffer(). An alternative would be to
// make graphSeg and graphOff global to the program by removing the static
// storage specifier, but this should only be done if the overhead of the
// extra far call involved in using this gateway is proven to be significant.
void Line(int x1, int y1, int x2, int y2, char color)
{
	int y_unit, x_unit; // Variables for amount of change in x and y

	int offset = y1 * 320 + x1; // Calculate offset into video RAM

	int ydiff = y2 - y1;   // Calculate difference between y coordinates
	if (ydiff < 0) {     // If the line moves in the negative  direction
		ydiff = -ydiff;    // ...get absolute value of difference
		y_unit = -320;     // ...and set negative unit in y dimension
	} else {
		y_unit = 320;   // Else set positive unit in y dimension
	}

	int xdiff = x2 - x1;			// Calculate difference between x coordinates
	if (xdiff < 0) {				// If the line moves in the negative direction
		xdiff = -xdiff;				// ...get absolute value of difference
		x_unit = -1;					// ...and set negative unit in x dimension
	} else {
		x_unit = 1;				// Else set positive unit in y dimension
	}

	int error_term = 0;			// Initialize error term
	if (xdiff > ydiff) {		// If difference is bigger in x dimension
		int length = xdiff + 1;	// ...prepare to count off in x direction
		for (int i = 0; i < length; i++) {  // Loop through points in x direction
			RETRO.framebuffer[offset] = color;	// Set the next pixel in the line to COLOR
			offset += x_unit;				// Move offset to next pixel in x direction
			error_term += ydiff;		// Check to see if move required in y direction
			if (error_term > xdiff) {	// If so...
				error_term -= xdiff;		// ...reset error term
				offset += y_unit;				// ...and move offset to next pixel in y dir.
			}
		}
	} else {								// If difference is bigger in y dimension
		int length = ydiff + 1;	// ...prepare to count off in y direction
		for (int i = 0; i < length; i++) {	// Loop through points in y direction
			RETRO.framebuffer[offset] = color;	// Set the next pixel in the line to COLOR
			offset += y_unit;				// Move offset to next pixel in y direction
			error_term += xdiff;    // Check to see if move required in x direction
			if (error_term > 0) {		// If so...
				error_term -= ydiff;	// ...reset error term
				offset += x_unit;			// ...and move offset to next pixel in x dir.
			}
		}
	}
}

// reads the dac data in palette into the supplied
// palette structure.
void ReadPalette(int start, int number, char *palette)
{
	RETRO_Palette pal;
	int i, j;
	if ((start > 256) | (start < 0) | ((start + number) > 256))
		return;
	for (i = start; i < (start + number); i++) {
		j = i * 3;
		pal = RETRO_Get6bitColor(i);
		palette[j] = pal.r;
		palette[++j] = pal.g;
		palette[++j] = pal.b;
	}
}

// clears a range of palette registers to zero
void ClrPalette(int start, int number)
{
	int i;
	if ((start > 256) | (start < 0) | ((start + number) > 256))
		return;
	for (i = start;i < (start + number);i++) {
		RETRO_Set6bitColor(i, 0, 0, 0);
	}
}

// WaitVbi() waits twice for the vertical blanking interval. Once to make
// sure any current vbi is completed, and once for the start of the next vbi
void WaitVbi()
{
}

// clear the graphics screen to the color provided
void ClearScr(char color)
{
	RETRO_Clear(color);
}

// fill the rectangular area bounded by (tlx,tly)(brx,bry) with color.
// tlx = top left x, tly = top left y, brx = bottom right x, bry = bottom
// right y.
//
// This function needs to be recoded in assembler
void BarFill(int x1, int y1, int x2, int y2, char color, unsigned char *buffer = RETRO.framebuffer)
{
	int row;
	for (row = y1; row <= y2; row++) {
		memset((void *)&buffer[(row * 320) + x1], color, ((x2 - x1) + 1));
	}
}

// calculates and returns the buffer size necessary to hold the image bounded
// by x1, y1 at top left, and x2, y2 at lower right.
int BufSize(int x1, int y1, int x2, int y2)
{
	int size;
	size = ((x2 - x1) + 1) * ((y2 - y1) + 1);
	return(size);
}

#endif
