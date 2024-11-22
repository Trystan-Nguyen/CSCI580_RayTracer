/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include    "MathUtil.h"

# include <string>
# include <map>
using namespace std;

struct xyRes {
    int x;
    int y;
};

map<string, GzColor*> textureMap_map = map<string, GzColor*>();
map<string, xyRes> textureRes_map = map<string, xyRes>();
string currentTexture = "";
GzColor	*image=NULL;
int xs, ys;
int reset = 1;
inline int ARRAY(int x, int y) { return (x + y * xs); }

/* Image texture function */
int tex_fun(float u, float y, GzColor color, string textureName)
{
    //float u = 1.0 - x;
    float v = 1.0 - y;

  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (currentTexture != textureName) {
      if (image != NULL && textureMap_map.find(currentTexture) == textureMap_map.end()) {
          textureMap_map[currentTexture] = image;
          textureRes_map[currentTexture] = { xs, ys };
          image = NULL;
      }
      currentTexture = textureName;
      if (textureMap_map.find(currentTexture) == textureMap_map.end()) {
          reset = 1;
      }
      else {
          image = textureMap_map[currentTexture];
          xs = textureRes_map[currentTexture].x;
          ys = textureRes_map[currentTexture].y;
      }
  }

  if (reset) {          /* open and load texture file */
    fd = fopen (textureName.c_str(), "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */

  int x_lower = floor(u * (xs - 1));
  int x_upper = ceil(u * (xs - 1));

  int y_lower = floor(v * (ys - 1));
  int y_upper = ceil(v * (ys - 1));

  Vector3 a(image[ARRAY(x_lower, y_lower)][0], image[ARRAY(x_lower, y_lower)][1], image[ARRAY(x_lower, y_lower)][2]);
  Vector3 b(image[ARRAY(x_upper, y_lower)][0], image[ARRAY(x_upper, y_lower)][1], image[ARRAY(x_upper, y_lower)][2]);
  Vector3 c(image[ARRAY(x_upper, y_upper)][0], image[ARRAY(x_upper, y_upper)][1], image[ARRAY(x_upper, y_upper)][2]);
  Vector3 d(image[ARRAY(x_lower, y_upper)][0], image[ARRAY(x_lower, y_upper)][1], image[ARRAY(x_lower, y_upper)][2]);

  Vector3 left = interpolateVector(a, d, fabs(y_upper - v * (ys - 1)));
  Vector3 right = interpolateVector(b, c, fabs(y_upper - v * (ys - 1)));
  Vector3 result = interpolateVector(left, right, fabs(x_upper - u * (xs - 1)));
  color[0] = result.base[0];
  color[1] = result.base[1];
  color[2] = result.base[2];

  
  return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
    int x = floor(u * 10);
    int y = floor(v * 10);

    float val = x % 2 != y % 2 ? 1.0 : 0.0;

    color[0] = val;
    color[1] = val;
    color[2] = val;
    /**
    if (val > 0.0)
    {
        float thetaX = u * 2 * 3.14159;
        float thetaY = v * 2 * 3.14159;

        float xP = 0.2 * cos(thetaX) + 0.5;
        if (fabs(xP - v) < 0.2) color[1] *= 0.5;

        float yP = pow(0.2 * sin(2*thetaY) + 0.5,2);
        if (fabs(yP - u) < 0.5) color[2] *= 0.33;
    }
    */

	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

