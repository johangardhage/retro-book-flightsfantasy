
// Constant definitions:

const PARALLEL=0;           // Orthographic projection flag
const PERSPECTIVE=1;        // Perspective projection flag

// Variable structures to hold shape data:

struct vertex_type {				// Structure for individual vertices
	int lx,ly,lz,lt;					// Local coordinates of vertex
  int wx,wy,wz,wt;          // World coordinates of vertex
	int sx,sy,st;		    			// Screen coordinates of vertex
};

struct	polygon_type {
	int	number_of_vertices;	// Number of vertices in polygon
	int	color;              // Color of polygon
	int	zmax,zmin;					// Maximum and minimum z coordinates of polygon
	int xmax,xmin;
	int ymax,ymin;
	vertex_type **vertex;		// List of vertices
	int	sortflag;						// For hidden surface sorts
};

struct	object_type{
	int		number_of_vertices;	// Number of vertices in object
	int		number_of_polygons;	// Number of polygons in object
	int		x,y,z;							// World coordinates of object's local origin
	polygon_type	*polygon;		// List of polygons in object
	vertex_type *vertex;			// Array of vertices in object
	int		convex;							// Is it a convex polyhedron?
};

struct	world_type {
	int		number_of_objects;
	object_type	*obj;
};

struct 	polygon_list_type {
	int		number_of_polygons;
	polygon_type *polygon;
};

// Function prototypes:

void inittrans();
void scale(int sf);
void rotate(int ax,int ay,int az);
void translate(int xt,int yt,int zt);
void transform(object_type *object);
void project(object_type *object,int distance,int xorigin,int yorigin);
void draw_object(object_type object,char far *screen);
int	backface(polygon_type polygon);
