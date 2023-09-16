
// POLY.CPP
//   A set of functions to create translation matrices for various
// polygon transformation functions, to transform a polygon-fill
// object using a matrix concatenated from those matrices, and to
// draw a polygon-fill object on the mode 13h display

#include  <stdio.h>
#include  <math.h>
#include  "poly.h"
#include  "drawpoly.h"

// Global transformation arrays:

float matrix[4][4];         // Master transformation matrix
float smat[4][4];				    // Scaling matrix
float zmat[4][4];				    // Z rotation matrix
float xmat[4][4];				    // X rotation matrix
float ymat[4][4];				    // Y rotation matrix
float tmat[4][4];           // Translation matrix

// Function prototype:

// Multiply two 4x4 matrices, result in RESULT:

void matmult(float result[4][4],float mat1[4][4],float mat2[4][4]);

// Copy one 4x4 matrix into another 4x4 matrix:

void matcopy(float dest[4][4],float source[4][4]);

// Transformation functions:

void inittrans()
{

// Initialize master transformation matrix to the identity matrix

  matrix[0][0]=1; matrix[0][1]=0; matrix[0][2]=0; matrix[0][3]=0;
  matrix[1][0]=0; matrix[1][1]=1; matrix[1][2]=0; matrix[1][3]=0;
  matrix[2][0]=0; matrix[2][1]=0; matrix[2][2]=1; matrix[2][3]=0;
  matrix[3][0]=0; matrix[3][1]=0; matrix[3][2]=0; matrix[3][3]=1;
}

void scale(float sf)
{
  float mat[4][4];

	// Initialize scaling matrix:

	smat[0][0] = sf; smat[0][1] = 0;  smat[0][2] = 0;  smat[0][3] = 0;
	smat[1][0] = 0;  smat[1][1] = sf; smat[1][2] = 0;  smat[1][3] = 0;
	smat[2][0] = 0;  smat[2][1] = 0;  smat[2][2] = sf; smat[2][3] = 0;
	smat[3][0] = 0;  smat[3][1] = 0;  smat[3][2] = 0;  smat[3][3] = 1;

  // Concatenate with master matrix:

  matmult(mat,smat,matrix);
  matcopy(matrix,mat);
}

void rotate(float ax,float ay,float az)
{

  // Create three rotation matrices that will rotate an object
  // AX radians on the X axis, AY radians on the Y axis and
  // AZ radians on the Z axis

  float mat1[4][4];
  float mat2[4][4];

	// Initialize X rotation matrix:

	xmat[0][0]=1;  xmat[0][1]=0;        xmat[0][2]=0;       xmat[0][3]=0;
  xmat[1][0]=0;  xmat[1][1]=cos(ax);  xmat[1][2]=sin(ax); xmat[1][3]=0;
	xmat[2][0]=0;  xmat[2][1]=-sin(ax); xmat[2][2]=cos(ax); xmat[2][3]=0;
	xmat[3][0]=0;  xmat[3][1]=0;        xmat[3][2]=0;       xmat[3][3]=1;

  // Concatenate this matrix with master matrix:

  matmult(mat1,xmat,matrix);

  // Initialize Y rotation matrix:

	ymat[0][0]=cos(ay); ymat[0][1]=0; ymat[0][2]=-sin(ay); ymat[0][3]=0;
  ymat[1][0]=0;       ymat[1][1]=1; ymat[1][2]=0;        ymat[1][3]=0;
	ymat[2][0]=sin(ay); ymat[2][1]=0; ymat[2][2]=cos(ay);  ymat[2][3]=0;
	ymat[3][0]=0;       ymat[3][1]=0; ymat[3][2]=0;        ymat[3][3]=1;

  // Concatenate this matrix with master matrix:

  matmult(mat2,ymat,mat1);

	// Initialize Z rotation matrix:

	zmat[0][0]=cos(az);  zmat[0][1]=sin(az);  zmat[0][2]=0;  zmat[0][3]=0;
	zmat[1][0]=-sin(az); zmat[1][1]=cos(az);  zmat[1][2]=0;  zmat[1][3]=0;
	zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=1;  zmat[2][3]=0;
	zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;  zmat[3][3]=1;

  // Concatenate this matrix with master matrix:

  matmult(matrix,zmat,mat2);
}

void translate(int xt,int yt,int zt)
{

// Create a translation matrix that will translate an object an
// X distance of XT, a Y distance of YT, and a Z distance of ZT
// from the screen origin

  float mat[4][4];

  tmat[0][0]=1;  tmat[0][1]=0;  tmat[0][2]=0;  tmat[0][3]=0;
  tmat[1][0]=0;  tmat[1][1]=1;  tmat[1][2]=0;  tmat[1][3]=0;
  tmat[2][0]=0;  tmat[2][1]=0;  tmat[2][2]=1;  tmat[2][3]=0;
  tmat[3][0]=xt; tmat[3][1]=yt; tmat[3][2]=zt; tmat[3][3]=1;

  // Concatenate with master matrix:

  matmult(mat,matrix,tmat);
  matcopy(matrix,mat);
}

void transform(object_type *object)
{
	// Multiply all vertices in OBJECT with master transformation matrix:

	for (int v=0; v<(*object).number_of_vertices; v++) {
    vertex_type *vptr=&(*object).vertex[v];
		vptr->wx=vptr->lx*matrix[0][0]+vptr->ly*matrix[1][0]
       +vptr->lz*matrix[2][0]+matrix[3][0];
		vptr->wy=vptr->lx*matrix[0][1]+vptr->ly*matrix[1][1]
       +vptr->lz*matrix[2][1]+matrix[3][1];
		vptr->wz=vptr->lx*matrix[0][2]+vptr->ly*matrix[1][2]
       +vptr->lz*matrix[2][2]+matrix[3][2];
	}
}

void project(object_type *object,int distance)
{

// Project object onto screen with perspective projection

  for (int v=0; v<(*object).number_of_vertices; v++) { // Loop through vertices
    vertex_type *vptr=&(*object).vertex[v];  // Point to current vertex
    vptr->sx=distance*vptr->wx/(distance-vptr->wz)+XORIGIN;  // ...divide world x&y coords
    vptr->sy=distance*vptr->wy/(distance-vptr->wz)+YORIGIN;  // ... by z coordinates
  }
}

void draw_object(object_type object,char far *screen)
// Draw object in structure OBJECT
{
	// Loop through all polygons in object:
	for (int p=0; p<object.number_of_polygons; p++) {
  	// Draw current polygon:
    if (object.convex) {
      if(!backface(object.polygon[p])) {
        drawpoly(object.polygon[p],screen);
      }
    }
    else drawpoly(object.polygon[p],screen);
	}
}

void matmult(float result[4][4],float mat1[4][4],float mat2[4][4])
{

// Multiply matrix MAT1 by matrix MAT2, returning the result in RESULT

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      result[i][j]=0;
      for (int k=0; k<4; k++)
        result[i][j]+=mat1[i][k] * mat2[k][j];
    }
}

void matcopy(float dest[4][4],float source[4][4])
{

// Copy matrix SOURCE to matrix DEST

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      dest[i][j]=source[i][j];
}

int	backface(polygon_type p)
{

// 	 Returns 0 if POLYGON is visible, -1 if not.
//   POLYGON must be part of a convex polyhedron

    vertex_type *v0,*v1,*v2;  // Pointers to three vertices
    // Point to vertices:
    v0=p.vertex[0];
    v1=p.vertex[1];
    v2=p.vertex[2];
    int z=(v1->sx-v0->sx)*(v2->sy-v0->sy)-(v1->sy-v0->sy)*(v2->sx-v0->sx);
    return(z>=0);
}
