(This README file was last updated on June 23, 1993.)

The program files used in Flights of Fantasy evolved as the book was
being written. For that reason, some of the files change from chapter 
and in several cases these changes are not documented in the book itself.
This file, README.TXT, documents those changes. It will be updated as
necessary, both on the distribution disk provided with the book and in
the archive files, available on the Compuserve Gamers Forum (GO GAMERS),
that contain the current versions of the flight simulator code.

01/20/93 - The WALKMAN2 program, described in Chapter Five of the book,
uses a somewhat different version of the sprite functions developed in
Chapter Four. These rewritten functions are available in the versions of
the SPRITE.CPP and SPRITE.H files found in the directory WALKMAN2 on the
distribution disk. The primary difference between these functions and
the ones in Chapter Four is that it is no longer necessary to pass the
address of a sprite buffer to the LOAD function. Instead, a buffer is
created automatically by the construction for the Sprite class. This
improves the encapsulation of the Sprite objects, isolating low level
memory functions from the calling function. Prototypes for these revised
functions can be found in the SPRITE.H file in the WALKMAN2 directory.

01/21/93 - The size of the world database in the current version of the
Flights of Fantasy Flight Simulator is quite small. Because of the
limited precision of the fixed point routines currently being used in
the program, wraparound occurs fairly rapidly in the scenery. Thus, you
may find yourself flying by the same scenery twice in fairly rapid
succession. There are two ways to expand the world, both of which are
left as exercises for the reader. (Check The Gamers Forum on Compuserve
for newer versions of the program implementing larger databases.) The
first is to use 386-based machine language fixed point routines, as
described in the chapter on optimization. The second is to use floating
point arithmetic. As noted in the book, this chapter is becoming more
feasible as more and more machines have math coprocessors installed.

6/23/93 - Several changes were made to the files on the FLIGHTS OF FANTASY
disk to correct bugs reported by users. In addition, the assembled OBJ
files for all ASM files are now included, for the benefit of users who
own Turbo C++ but not Turbo Asssembler. To use these files, change the
PRJ files so that all references to ASM files refer to the similarly
named OBJ files.

Other changes made on this date are:

FSIM/AIRCRAFT.CPP - Line 544 in this module of the FOF simulator was
changed from:

  loopTime /= 1000;

to:

  if (!(loopTime /= 1000)) loopTime = 1;

This avoids a lock-up problem that occurred during program initialization
on fast 486 and occasional 386 machines (usually those that lack a joystick
or SoundBlaster to slow down the code). The lock-up occurred when the
number of microseconds between the initialization of the timer and the
first frame of the animation was less than 1000. When that value was
divided by 1000 to produce the number of milliseconds (stored in loopTime),
it became zero -- and triggered a divide-by-zero error a few instructions
hence.

3DWIRE/WAITE.CPP - Some users complained that this program locked up
their machines. Although I was unable to duplicate this problem on my
own machine, it may have been the result of the wireframe graphic
routines overwriting the bounds of the video buffer. (No clipping had been
introduced into the code at this point.) The program has been slightly
altered to move the wireframe W ten units further from the viewer,
lessening the chance of memory being trashed.

OPTIMIZE/OPTDEMO.CPP - The scaling function in this program caused the
screen to be trashed when the "-" key was pressed. The scaling function
has been replaced with a distance changing function that moves the polygon
object closer to and further from the viewer (with a distant cap on forward
motion to prevent overflow of the viewport). Once again, clipping routines
had not been introduced at this point, so the window can be easily over-
written by a scaled polygon.

