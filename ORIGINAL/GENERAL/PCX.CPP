
// PCX.CPP - 1/10/93
//
// A set of functions to create a PCX object for loading and
// compressing PCX files.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include  <io.h>
#include	<fcntl.h>
#include	<mem.h>

#include "screen.h"
#include "pcx.h"

const long IMAGE_SIZE=64000; // Size of PCX bitmap

int Pcx::load(char far *filename,pcx_struct *pcx)
{

// Function to load PCX data from file FILENAME into
//  structure PCX.

	// Open file; return nonzero value on error

	if ((infile=open(filename,O_BINARY))==NULL)
			return(-1);

  // Move file pointer to start of header:

	lseek(infile,0L,SEEK_SET);

  // Reader header from PCX file:

	int readlen=read(infile,&(pcx->header),
										sizeof(pcx_header));

  // Decompress bitmap and place in buffer,
  //  returning non-zero value if error occurs:

	if (!load_image(infile,pcx)) return(-1);

  // Decompress palette and store in array:

	load_palette(infile,pcx);

	close(infile); // Close PCX file
	return(0);     // Return non-error condition
}

int Pcx::load_image(int pcxfile,pcx_struct *pcx)

// Decompress bitmap and store in buffer

{
  // Symbolic constants for encoding modes, with
  //  BYTEMODE representing uncompressed byte mode
  //  and RUNMODE representing run-length encoding mode:

  const int BYTEMODE=0, RUNMODE=1;

  // Buffer for data read from disk:

  const int BUFLEN=5*1024;
  int mode=BYTEMODE;  // Current encoding mode being used,
                              //  initially set to BYTEMODE
  int readlen;  // Number of characters read from file
  static unsigned char outbyte; // Next byte for buffer
  static unsigned char bytecount; // Counter for runs
  static unsigned char buffer[BUFLEN]; // Disk read buffer

  // Allocate memory for bitmap buffer and return -1 if
  //  an error occurs:

	if ((pcx->image=new unsigned char[IMAGE_SIZE])==NULL)
		return(-1);
	int bufptr=0; // Point to start of buffer
	readlen=0;    // Zero characters read so far

  // Create pointer to start of image:

	unsigned char *image_ptr=pcx->image;

  // Loop for entire length of bitmap buffer:

	for (long i=0; i<IMAGE_SIZE; i++) {
		if (mode==BYTEMODE) { // If we're in individual byte
                          //  mode....
			if (bufptr>=readlen) { // If past end of buffer...
				bufptr=0;            // Point to start

        // Read more bytes from file into buffer;
        //  if no more bytes left, break out of loop

				if ((readlen=read(pcxfile,buffer,BUFLEN))==0)
					break;
			}
			outbyte=buffer[bufptr++]; // Next byte of bitmap
			if (outbyte>0xbf) {       // If run-length flag...

        // Calculate number of bytes in run:

				bytecount = (int)((int)outbyte & 0x3f);
				if (bufptr>=readlen) {  // If past end of buffer
					bufptr=0;             // Point to start

          // Read more bytes from file into buffer;
          //  if no more bytes left, break out of loop

					if ((readlen=read(pcxfile,buffer,BUFLEN))==0)
						break;
				}
				outbyte=buffer[bufptr++]; // Next byte of bitmap

        // Switch to run-length mode:

				if (--bytecount > 0) mode = RUNMODE;
			}
		}

    // Switch to individual byte mode:

		else if (--bytecount == 0) mode=BYTEMODE;

    // Add next byte to bitmap buffer:

		*image_ptr++=outbyte;
	}
}

void Pcx::load_palette(int pcxfile,pcx_struct *pcx)

// Load color register values from file into palette array

{

  // Seek to start of palette, which is always 768 bytes
  //  from end of file:

	lseek(pcxfile,-768L,SEEK_END);

  // Read all 768 bytes of palette into array:

	read(pcxfile,pcx->palette,3*256);

  // Adjust for required bit shift:

	for (int i=0; i<256; i++)
		for (int j=0; j<3; j++)
			pcx->palette[i*3+j]=pcx->palette[i*3+j]>>2;
}

int Pcx::compress(pcx_struct *pcx)

// Function to compress bitmap stored in PCX->IMAGE into
//  compressed image buffer PCX->CIMAGE

{
	int value,runlength,runptr;
  unsigned char *cbuffer; // Buffer for compression

	long ptr=0;  // Pointer to position in PCX->IMAGE
	long cptr=0; // Pointer to position in PCX->CIMAGE

  // Allocate 64K buffer in which to perform compression,
  //  return error if memory not available:

	if ((cbuffer=new unsigned char[65530])==NULL) return(-1);

  // Begin compression:

	while (ptr<IMAGE_SIZE) {
		if (pcx->image[ptr]==0) { // If next byte transparent
			runlength=0;  // Set length of run to zero

      // Loop while next byte is zero:

			while (pcx->image[ptr]==0) {
				ptr++;       // Point to next byte of IMAGE
				runlength++; // Count off this byte
				if (runlength>=127) break; // Can't count more than
                                   //  127
				if (ptr >= IMAGE_SIZE) break; // If end of IMAGE,
                                      //  break out of loop
			}

      // Set next byte of CIMAGE to run length:

			cbuffer[cptr] = (unsigned char) (runlength + 128);
			cptr++; // Point to next byte of CIMAGE

      // Set next byte of CIMAGE to zero:

			cbuffer[cptr] = 0;
			cptr++; // Point to next byte of CIMAGE
		}
		else {

     // If not a run of zeroes, get next byte from IMAGE

			runlength=0;  // Set run length to zero
			runptr=cptr++; // Remember this address, then point
                     //  to next byte of CIMAGE

      // Loop while next byte is not zero:

			while (pcx->image[ptr]!=0) {

        // Get next byte of IMAGE into CIMAGE:

				cbuffer[cptr]=pcx->image[ptr];
				cptr++; // Point to next byte of CIMAGE
				ptr++;  // Point to next byte of IMAGE
				runlength++; // Count off this byte
				if (runlength>=127) break; // Can't count more than
                                   //  127
				if (ptr >= IMAGE_SIZE) break; // If end of IMAGE,
                                      //  break out of loop
			}
			cbuffer[runptr]=runlength; // Put run length in buffer
		}
	}

  // Allocate memory for CIMAGE, return error if memory
  //  not available:

	if ((pcx->cimage=new unsigned char[cptr])==NULL)
      return(-1);

  // Copy compressed image into CIMAGE buffer:

	memcpy(pcx->cimage,cbuffer,cptr);
	delete cbuffer; // Kill of temporary workspace
	pcx->clength = cptr; // Set length of compressed image
	return(0); // Return no error
}
