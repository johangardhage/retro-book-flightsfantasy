
const SCREEN_WIDTH=320,SCREEN_HEIGHT=200;

extern "C" {
	void cls(char far *screen_adr);
	void setmode(int mode);
	void setpalette(char far *color_regs,int firstreg,int numregs);
}
