/* Starting code for assignment 3.
   Calculates a ray that passes through each pixel of the viewing window.
   Draws randomly coloured pixel in the viewing window. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Linux OpenGL Headers
/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
*/

// MacOS OpenGL Headers

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

	// window size
#define SIZE 512


typedef struct Sphere {
   float x, y, z;
   float r;
   float ar, ag, ab;
   float dr, dg, db;
   float sr, sg, sb;
} Sphere;

typedef struct Light {
   float x;
   float y;
   float z;
} Light;

Sphere sphere;
Light light;


	/* calculate the length of a vector */
float length(float *x, float *y, float *z) {
   return( sqrtf( (*x * *x) + (*y * *y) + (*z * *z)) );
}

	/* calculate the distance between two points */
float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
   return( sqrtf( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2))) );
}


	/* creates a unit vector */
	/* divide a vector by its own length */
void normalize(float *xd, float *yd, float *zd) {
float len;
   len = length(xd, yd, zd);
   *xd = *xd / len;
   *yd = *yd / len;
   *zd = *zd / len;
}

 
	/* OoenGL calls this to draw the screen */
void display() {
int i, j;
float x, y;
float r, g, b;

float x0, y0, z0;   // viewpoint origin
float xd, yd, zd;   // view ray direction


   glClearColor(0.0, 0.0, 0.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT);

   glPointSize(1.0);

	/* ray tracing code starts here */
	/* set viewer position (x0, y0, z0) */
   x0 = 0.0;
   y0 = 0.0;
   z0 = 0.0;

	/* draw individual pixels */
   glBegin(GL_POINTS);

	/* nested loops move over the viewing window */
	/* i and j move through the x and y coordinates respectively */
   for(i=0; i<SIZE; i++) {
      for(j=0; j<SIZE; j++) {
		/* screen location to draw pixel */
		/* used with glVertexf() but not in calculations for vectors */
         x = (float) (i - (SIZE/2)) / (SIZE/2);
         y = (float) (j - (SIZE/2)) / (SIZE/2);

		/* calculate the ray direction (xd, yd, zd) */
		/* ray starts at the origin (x0, y0, z0) and ends at
		   a point on the viewing window (xd, yd, zd) */
        	/* screen corner coordinates are (-1,-1) to (1,1) */
         xd = ((float) (i - (SIZE/2)) / (SIZE/2));
         yd = ((float) (j - (SIZE/2)) / (SIZE/2));
         zd = -1.0;
         normalize(&xd, &yd, &zd);

			/* ray tracing code */
			/* your code starts here */

		/* calculate B, C.  A == 1.0 */
      float A = 1.0;
      float B = 2.0 * (xd * (x0 - sphere.x) + yd * (y0 - sphere.y) + zd * (z0 - sphere.z));
      float C = (x0 - sphere.x)*(x0 - sphere.x)  + (y0 - sphere.y)*(y0 - sphere.y)   + (z0 - sphere.z)*(z0 - sphere.z)  - sphere.r*sphere.r;

		/* calculate the discriminant */

      float dis = B*B - 4.0*A*C;

      float t0;
      float t1;

      float rix, riy, riz;

		/* if there is one intersection point (discriminant == 0)
		   then calculate intersection of ray and sphere at (xi, yi, zi) */
      if (dis == 0)
      {
         t0 = ((-B) - sqrtf(dis)) / (2*A);

         rix = x0 + xd*t0;
         riy = y0 + yd*t0;
         riz = z0 + zd*t0;
      }

		/* if there are two two intersection points (discriminant > 0) 
		   then determine which point is closer to the viewpoint (x0, y0, z0)
		   and calculate the intersection of point (xi, yi, zi) */
      else if (dis > 0)
      {
         t0 = ((-B) - sqrtf(dis)) / (2*A);
         t1 = ((-B) + sqrtf(dis)) / (2*A);

         float ri0x, ri0y, ri0z;
         float ri1x, ri1y, ri1z;

         ri0x = x0 + xd*t0;
         ri0y = y0 + yd*t0;
         ri0z = z0 + zd*t0;
         ri1x = x0 + xd*t1;
         ri1y = y0 + yd*t1;
         ri1z = z0 + zd*t1;

         float dist0 = distance(x0, y0, z0, ri0x, ri0y, ri0z);
         float dist1 = distance(x0, y0, z0, ri1x, ri1y, ri1z);

         if (dist0 <= dist1)
         {
            rix = ri0x;
            riy = ri0y;
            riz = ri0z;
         }
         else
         {
            rix = ri1x;
            riy = ri1y;
            riz = ri1z;
         }
      }
      else
      {
         continue;
      }
      

		/* calculate normal vector (nx, ny, nz) to the intersection point */
      float Nx, Ny, Nz;

      Nx = (rix - sphere.x)/sphere.r;
      Ny = (riy - sphere.y)/sphere.r;
      Nz = (riz - sphere.z)/sphere.r;

      normalize(&Nx, &Ny, &Nz);

		/* calculate viewing vector (vx, vy, vz) */
      float Vx, Vy, Vz;

      Vx = x0 - rix;
      Vy = y0 - riy;
      Vz = z0 - riz;

      normalize(&Vx, &Vy, &Vz);

		/* calculate the light vector (lx, ly, lz)  */
      float Lx, Ly, Lz;

      Lx = light.x - rix;
      Ly = light.y - riy;
      Lz = light.z - riz;

      normalize(&Lx, &Ly, &Lz);

		/* calculate the dot product N.L, using the normal vector
		   and the light vector */
      float NdotL = Nx*Lx + Ny*Ly + Nz*Lz;

		/* calculate the reflection vector (rx, ry, rz) */
      float Rx, Ry, Rz;

      Rx = 2.0 * NdotL * Nx - Lx;
      Ry = 2.0 * NdotL * Ny - Ly;
      Rz = 2.0 * NdotL * Nz - Lz;

      normalize(&Rx, &Ry, &Rz);

		/* calculate the dot product R.V, using the reflection vector
		   and the viewing vector */
      float RdotV = Rx*Vx + Ry*Vy + Rz*Vz;

		/* calculate illumination using the parameters read from the
		   file and N.L and R.V  */
      float Ir, Ig, Ib;

      Ir = sphere.ar + sphere.dr*NdotL + sphere.sr*RdotV;
      Ig = sphere.ag + sphere.dg*NdotL + sphere.sg*RdotV;
      Ib = sphere.ab + sphere.db*NdotL + sphere.sb*RdotV;

		/* set the colour of the point - calculate the r,g,b
		   values using ambient, diffuse, and specular light parameters
		   and the dot products N.L and R.V  */

			/* end of ray tracing code */

		/* replace the following three lines with the illumination calculations */
         
         glColor3f(Ir, Ig, Ib);
		/* draw the point on the display window at point (x, y) */
         glVertex2f(x, y);


      }  // end loop j
   }     // end loop i

   glEnd();
 
   glFlush();
}

	/* read the keyboard, when q pressed exit the program */
void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
   }
}
 
int main(int argc, char** argv) {

	/* read input file */
   if (argc != 2)
   {
      printf("No scene file given.\n");
      return 1;
   }

   FILE* fp = fopen(argv[1], "r");

   if (fp == NULL)
   {
      printf("Failed to open file: %s\n", argv[1]);
      return 1;
   }

   char subject [16];
   int hasLight = 0;
   int hasSphere = 0;

   while (hasLight == 0 || hasSphere == 0)
   {

      if (fscanf(fp, "%s", subject) != 1)
      {
         printf("Failed to read file.\n");
         fclose(fp);
         return 1;
      }

      //read in light source
      if (strcmp("light", subject) == 0)
      {
         if (fscanf(fp, "%f %f %f", &light.x, &light.y, &light.z) != 3)
         {
            printf("Failed to initialize light from file.\n");
            fclose(fp);
            return 1;
         }
         hasLight = 1;
      }
      //read in sphere
      else if (strcmp("sphere", subject) == 0)
      {
         if (fscanf(fp, "%f %f %f %f %f %f %f %f %f %f %f %f %f", &sphere.x, &sphere.y, &sphere.z, &sphere.r, &sphere.ar, &sphere.ag, &sphere.ab, &sphere.dr, &sphere.dg, &sphere.db, &sphere.sr, &sphere.sg, &sphere.sb) != 13)
         {
            printf("Failed to initialize sphere from file.\n");
            fclose(fp);
            return 1;
         }
         hasSphere = 1;
      }
      else
      {
         printf("Unknown object: %s\n", subject);
         fclose(fp);
         return 1;
      }
   }

   fclose (fp);

   /* Initialize OpenGL and GLUT */
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(SIZE,SIZE);
   glutCreateWindow("2D Test");
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);

   glutMainLoop();
   return 0;
}
