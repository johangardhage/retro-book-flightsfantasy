
// Constant definitions:

const XORIGIN=160;    // Origin of coordinate system X axis
const YORIGIN=100;    // Origin of coordinate system Y axis

// Variable structures to hold shape data:

struct vertex_type {  // Structure for individual vertices
	int lx,ly,lz,lt;		// Local coordinates of vertex
  int wx,wy,wz,wt;    // World coordinates of vertex
	int sx,sy,sz,st;		// Screen coordinates of vertex
};

struct line_type  {   // Structure for wireframe lines
  int start,end;      // Pointers to vertices for start
                      //  and end
};

struct shape_type {		     // Structure for complete shape
	int color;					     // Color of shape
	int	number_of_vertices;  // Number of vertices in shape
  int number_of_lines;     // Number of lines in shape
	vertex_type *vertex;		 // Array of vertex descriptors
  line_type *line;         // Array of line descriptors
};

// Function prototypes:

void inittrans();
void scale(float sf);
void rotate(float ax,float ay,float az);
void translate(int xt,int yt,int zt);
void transform(shape_type *shape);
void project(shape_type *shape,int distance);
void draw_shape(shape_type shape,char far *screen);
