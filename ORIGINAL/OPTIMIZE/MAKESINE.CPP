
#include	<stdio.h>
#include	<math.h>
#include  <conio.h>

const NUMBER_OF_DEGREES = 256; // Degrees in a circle
const SHIFT = 9;               // Fixed point shift
const SHIFT_MULT = 1<<SHIFT;   // Fixed point shift as
                               //  a multiplication
void main()
{
	float radians=0;
	FILE  *fname;

  // Create file FIX.H for constants and macros:

	fname=fopen("fix.h","wt");
	fprintf(fname,"\n#define ABS(X) (X<0?-X:X)\n");
	fprintf(fname,"#define COS(X) cos_table[ABS(X)&255]\n");
	fprintf(fname,"#define SIN(X) sin_table[ABS(X)&255]\n");
	fprintf(fname,"\nconst NUMBER_OF_DEGREES = %d;\n",
           NUMBER_OF_DEGREES);
	fprintf(fname,"const SHIFT = %d;\n",SHIFT);
	fprintf(fname,"const SHIFT_MULT = 1<<SHIFT;\n\n");
	fprintf(fname,"extern long cos_table[%d];\n",
           NUMBER_OF_DEGREES);
	fprintf(fname,"extern long sin_table[%d];\n",
           NUMBER_OF_DEGREES);
	fclose(fname);

  // Create file FIX.CPP for sine and cosine tables:

	fname=fopen("fix.cpp","wt");
  fprintf(fname,"\n//FIX.CPP\n");
  fprintf(fname,"//  Fixed point math tables\n\n");

  // Create cosine table:

	fprintf(fname,"long cos_table[%d]={\n    ",
           NUMBER_OF_DEGREES);
	int count=0;
	for (int i=0; i<NUMBER_OF_DEGREES; i++) {
		fprintf(fname,"%d, ",(long)(cos(radians)*SHIFT_MULT));
		radians += 6.28/NUMBER_OF_DEGREES;
		count++;
		if (count>=8) {
			fprintf(fname,"\n    ");
			count=0;
		}
	}
	fprintf(fname,"};\n\n");

  // Create sine table:

	fprintf(fname,"long sin_table[%d]={\n    ",
           NUMBER_OF_DEGREES);
	count=0;
	for (i=0; i<NUMBER_OF_DEGREES; i++) {
		fprintf(fname,"%d, ",(long)(sin(radians)*SHIFT_MULT));
		radians += 6.28/NUMBER_OF_DEGREES;
		count++;
    if (count>=8) {
      fprintf(fname,"\n    ");
      count=0;
    }
	}
  fprintf(fname,"};\n");
  fclose(fname);
}
