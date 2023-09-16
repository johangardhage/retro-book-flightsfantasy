#include  <stdio.h>
#include  <ctype.h>
#include  <alloc.h>
#include  "poly.h"
#include  "loadpoly.h"

FILE *f;
int num;

int loadpoly(world_type *world,char *filename)
{
  f=fopen(filename,"rt");
  (*world).number_of_objects=getnumber(f);
  (*world).obj=(object_type far *)farmalloc((*world).number_of_objects*sizeof(object_type));
  for (int objnum=0; objnum<(*world).number_of_objects; objnum++) {
    (*world).obj[objnum].number_of_vertices=getnumber(f);
    (*world).obj[objnum].vertex=(vertex_type far *)farmalloc((*world).obj[objnum].number_of_vertices*sizeof(vertex_type));
    for (int vertnum=0; vertnum<(*world).obj[objnum].number_of_vertices; vertnum++) {
      (*world).obj[objnum].vertex[vertnum].lx=getnumber(f);
      (*world).obj[objnum].vertex[vertnum].ly=getnumber(f);
      (*world).obj[objnum].vertex[vertnum].lz=getnumber(f);
      (*world).obj[objnum].vertex[vertnum].lt=1;
      (*world).obj[objnum].vertex[vertnum].wt=1;
    }
    (*world).obj[objnum].number_of_polygons=getnumber(f);
    (*world).obj[objnum].polygon=(polygon_type far *)farmalloc((*world).obj[objnum].number_of_polygons*sizeof(polygon_type));
    for (int polynum=0; polynum<(*world).obj[objnum].number_of_polygons; polynum++) {
      (*world).obj[objnum].polygon[polynum].number_of_vertices=getnumber(f);
      (*world).obj[objnum].polygon[polynum].vertex=
        (vertex_type far **)farmalloc((*world).obj[objnum].polygon[polynum].number_of_vertices
          *sizeof(vertex_type far *));
      for (int vertnum=0; vertnum<(*world).obj[objnum].polygon[polynum].number_of_vertices; vertnum++) {
        (*world).obj[objnum].polygon[polynum].vertex[vertnum]=
          &(*world).obj[objnum].vertex[getnumber(f)];
      }
      (*world).obj[objnum].polygon[polynum].color=getnumber(f);
    }
    (*world).obj[objnum].convex=getnumber(f);
  }
  fclose(f);
  return(0);
}

int getnumber(FILE *f)
{
  char ch;
  int sign=1;

  num=0;
  if ((ch=nextchar(f))=='-') {
     sign=-1;
     ch=nextchar(f);
  }
  while (isdigit(ch)) {
    num=num*10+ch-'0';
    ch=nextchar(f);
  }
  return(num*sign);
}

char nextchar(FILE *f)
{
  char ch;

  while(!feof(f)) {
    while(isspace(ch=fgetc(f)));
    if (ch=='*')
       while((ch=fgetc(f))!='\n');
         else return(ch);
  }
  return(0);
}
