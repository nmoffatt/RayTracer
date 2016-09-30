//
// this provide functions to set up the scene
//
#include "sphere.h"
#include <stdio.h>

extern Point light1;
extern float light1_intensity[3];

extern float global_ambient[3];
extern Spheres *scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // set up global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // set up light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // sphere 1
  Point sphere1_ctr = {1.5, -0.2, -3.2};
  float sphere1_rad = 1.23;
  float sphere1_ambient[] = {0.7, 0.7, 0.7};
  float sphere1_diffuse[] = {0.1, 0.5, 0.8};
  float sphere1_specular[] = {1.0, 1.0, 1.0};
  float sphere1_shineness = 10;
  float sphere1_reflectance = 0.4;
  scene = add_sphere(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
             sphere1_diffuse, sphere1_specular, sphere1_shineness,
		     sphere1_reflectance, 1);

  // sphere 2
  Point sphere2_ctr = {-1.5, 0.0, -3.5};
  float sphere2_rad = 1.5;
  float sphere2_ambient[] = {0.6, 0.6, 0.6};
  float sphere2_diffuse[] = {1.0, 0.0, 0.25};
  float sphere2_specular[] = {1.0, 1.0, 1.0};
  float sphere2_shineness = 6;
  float sphere2_reflectance = 0.3;
  scene = add_sphere(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
             sphere2_diffuse, sphere2_specular, sphere2_shineness,
		     sphere2_reflectance, 2);

  // sphere 3
  Point sphere3_ctr = {-0.35, 1.75, -2.25};
  float sphere3_rad = 0.5;
  float sphere3_ambient[] = {0.2, 0.2, 0.2};
  float sphere3_diffuse[] = {0.0, 1.0, 0.25};
  float sphere3_specular[] = {0.0, 1.0, 0.0};
  float sphere3_shineness = 30;
  float sphere3_reflectance = 0.3;
  scene = add_sphere(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
             sphere3_diffuse, sphere3_specular, sphere3_shineness,
		     sphere3_reflectance, 3);
  
}

/***************************************
 * You can create your own scene here
 ***************************************/

  // chessboard defined //

  Point board_p = {0, -2, -15};
  float board_ambient[3]  = {0.4, 0.4, 0.4};
  float board_specular[3] = {1.0, 1.0, 1.0};
  float board_shineness = 10;
  float board_reflectance = 1.0;

  Vector board_normal = {0, -1, 0};


  bool intersect_board(Point o, Vector u,Point *hit)
  {
    normalize(&board_normal);

    Vector v;
    v.x = o.x - board_p.x;
    v.y = o.y - board_p.y;
    v.z = o.z - board_p.z;

    if ((vec_dot(board_normal, u)) == (0 && vec_dot(board_normal, v))) {
        return false;
    }

    double t = vec_dot(board_normal, v) / vec_dot(board_normal, u) * -1;

    if (t > 0) 
    {
        hit->x = o.x + t * u.x;
        hit->y = o.y + t * u.y;
        hit->z = o.z + t * u.z;

        return true;
    }

    return false;
  }

  RGB_float board_color(Point o)
  {
    RGB_float color = {0.0, 0.0, 0.0};

    int i = int(o.x + 100) - 100;
    int j = int(o.z + 100) - 100;

    if (i >= 4 || i < -4 || j >= 2 || j < - 9) { //change 4 back to 2
        return background_clr;
    }

    if (((i % 2 == 0) && (j % 2 == 0)) || ((i % 2 != 0) && (j % 2 != 0))) 
    	color = {1, 1, 1};

    

    return color;
  }

void set_up_user_scene() {

//same default scene with transparent spheres
// set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // set up global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // set up light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  //refractiveness of water is 1.333
  // sphere 1
  Point sphere1_ctr = {1.5, -0.2, -3.2};
  float sphere1_rad = 1.23;
  float sphere1_ambient[] = {0.7, 0.7, 0.7};
  float sphere1_diffuse[] = {0.1, 0.5, 0.8};
  float sphere1_specular[] = {1.0, 1.0, 1.0};
  float sphere1_shineness = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_refractiveness = 1.333;
  scene = add_sphere2(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
             sphere1_diffuse, sphere1_specular, sphere1_shineness,
		     sphere1_reflectance, sphere1_refractiveness, 1);

  // sphere 2
  Point sphere2_ctr = {-1.5, 0.0, -3.5};
  float sphere2_rad = 1.5;
  float sphere2_ambient[] = {0.6, 0.6, 0.6};
  float sphere2_diffuse[] = {1.0, 0.0, 0.25};
  float sphere2_specular[] = {1.0, 1.0, 1.0};
  float sphere2_shineness = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_refractiveness = 1.333;
  scene = add_sphere2(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
             sphere2_diffuse, sphere2_specular, sphere2_shineness,
		     sphere2_reflectance, sphere2_refractiveness, 2);

  // sphere 3
  Point sphere3_ctr = {-0.35, 1.75, -2.25};
  float sphere3_rad = 0.5;
  float sphere3_ambient[] = {0.2, 0.2, 0.2};
  float sphere3_diffuse[] = {0.0, 1.0, 0.25};
  float sphere3_specular[] = {0.0, 1.0, 0.0};
  float sphere3_shineness = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_refractiveness = 1.333;
  scene = add_sphere2(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
             sphere3_diffuse, sphere3_specular, sphere3_shineness,
		     sphere3_reflectance, sphere3_refractiveness, 3);
  
}
