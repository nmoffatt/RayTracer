#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_intensity[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

//added conditionals
extern int shadow_on;
extern int reflection_on;
extern int refraction_on;
extern int chessboard_on;
extern int stochastic_on;
extern int supersampling_on;
extern int step_max;

// added chess board vars and functions
extern Vector board_normal;  
extern bool intersect_board(Point o, Vector u,Point *hit);   
extern RGB_float board_color(Point o);

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Vector V, Vector surf_norm, Spheres *sph) {
//
// do your thing here
//

	RGB_float color = {0,0,0};

	Vector l_m = get_vec(q, light1);
	double delta = vec_len(l_m);
	normalize(&l_m);

	//calc rm
	float cosTheta = vec_dot(surf_norm, vec_scale(l_m,-1));
	Vector r_m = vec_plus(l_m, vec_scale(surf_norm, 2*cosTheta));
	normalize(&r_m);
	
	//calc attenuation c
	float denom = 1/(decay_a + decay_b*delta + decay_c*pow(delta,2));
	
	
	//ambient
	color.r += global_ambient[0]*sph->mat_ambient[0];
	color.g += global_ambient[1]*sph->mat_ambient[1];
	color.b += global_ambient[2]*sph->mat_ambient[2];


	float I_m[3];

	for(int i=0; i<3; i++)	
		I_m[i] = light1_intensity[i];
		
	int S_m=1;

	//shadow
	if(inShadow(q, l_m, scene,sph) && shadow_on)
	{
		S_m = 0;
	}

	//diffuse
	color.r += S_m*I_m[0]*denom*sph->mat_diffuse[0]*vec_dot(surf_norm, l_m);
	color.g += S_m*I_m[1]*denom*sph->mat_diffuse[1]*vec_dot(surf_norm, l_m); 
	color.b += S_m*I_m[2]*denom*sph->mat_diffuse[2]*vec_dot(surf_norm, l_m);

	float N = sph-> mat_shineness;

	//specular 
	color.r += S_m*I_m[0]*denom*(sph->mat_specular[0]*(pow(vec_dot(r_m, V), N)));
	color.g += S_m*I_m[1]*denom*(sph->mat_specular[1]*(pow(vec_dot(r_m, V), N)));
	color.b += S_m*I_m[2]*denom*(sph->mat_specular[2]*(pow(vec_dot(r_m, V), N)));


	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
//helper function
Vector get_refracted_ray(Vector& normal, Vector& v, Spheres* sph, Point hit)
{
	float n=1.0/1.5;

	float cos_L = vec_dot(normal, v);
	float cos_t = sqrtf(1-n*n*(1-cos_L*cos_L));

	Vector refracted_ray = vec_plus((vec_scale(v,n)),vec_scale(normal,(n*(cos_L-cos_t))));

	//calculate where the reflective ray will hit on other side of sphere
	
	double a = vec_dot(refracted_ray, refracted_ray);
	double b = 2 * vec_dot(v, get_vec(sph->center, hit));
	double c = vec_dot(get_vec(sph->center, hit), get_vec(sph->center, hit));
	c -= sph->radius * sph->radius;

	double dt = b * b - 4 * a * c;

	Point hit2;

	double t0 = (-b - sqrt(dt)) / (a * 2);

	hit2.x = hit.x + t0 * v.x;
	hit2.y = hit.y + t0 * v.y;
	hit2.z = hit.z + t0 * v.z;

	Vector next_normal=sphere_normal(hit2, sph);
	
	float cos_L2 = vec_dot(next_normal, refracted_ray);
	float cos_t2 = sqrtf(1-n*n*(1-cos_L2*cos_L2));

	Vector next_refracted_ray = vec_plus((vec_scale(refracted_ray,n)),vec_scale(next_normal,(n*cos_L2-cos_t2)));

	
	return next_refracted_ray;
}

RGB_float recursive_ray_trace(Point p, Vector ray, int step) {
//
// do your thing here
//
	RGB_float color = background_clr;
	
	Spheres *closest_sph;
	Point* hit = new Point;
	closest_sph = intersect_scene(p, ray, scene, hit, 0);

	//Point* plane_hit;
	color = background_clr;

 	Point board_hit;
    	if(chessboard_on && intersect_board(p, ray, &board_hit))
    	{
        	Vector eye_vec = get_vec(board_hit, eye_pos);
        	normalize(&eye_vec);
        	color = board_color(board_hit);
        	Vector shadow_v = get_vec(board_hit, light1);

        	if ((shadow_on) && (inShadow(board_hit, shadow_v, scene, NULL))) 
		{
            		color = clr_scale(color, 0.5);
        	}

       		if ((reflection_on) && (step <= step_max)) 
		{
            		normalize(&ray);
            		Vector reflected_ray = vec_plus(vec_scale(board_normal, -2 * vec_dot(board_normal, ray)), ray);

            		RGB_float reflected_color = recursive_ray_trace(board_hit, reflected_ray, step++ );

            		color = clr_add(color, clr_scale(reflected_color, 0.3));
            		color = clr_scale(color, 1.0 / 1.3);
        	}
    	}

	if(closest_sph != NULL)
	{
		Vector eye_vec = get_vec(*hit, eye_pos);
		Vector surf_norm = sphere_normal(*hit, closest_sph);
		Vector light_vec = get_vec(*hit, p);
		normalize(&light_vec);
		normalize(&surf_norm);
		normalize(&eye_vec);

		color = phong(*hit, eye_vec, surf_norm, closest_sph);

		//Reflections

		RGB_float R_color = {0,0,0};

		if(step < step_max && reflection_on)
		{
			//R=2(n.L)n-L
			Vector R = vec_minus(vec_scale(surf_norm, vec_dot(surf_norm, light_vec)*2), light_vec);
			step ++;
			normalize(&R);

			float k_r=closest_sph->reflectance;
			R_color = recursive_ray_trace(*hit, R, step);
			R_color = clr_scale(R_color, k_r);

			RGB_float random_R_color = {0,0,0};

			if (stochastic_on) 
			{

               			for (int i = 0; i < 5; ++i) 
				{
					//generate random rays
                    			double rand_x = rand() / RAND_MAX / 2;
                    			double rand_y = rand() / RAND_MAX / 2;
                    			double rand_z = rand() / RAND_MAX / 2;

                    			Vector random_R = R;
                    			random_R.x = rand_x + random_R.x;
                    			random_R.y = rand_y + random_R.y;
                    			random_R.z = rand_z + random_R.z;

                    			normalize(&random_R);

                    			random_R_color = recursive_ray_trace(*hit, random_R, step++);

					//take avg of rays
                    			R_color = clr_add(R_color, random_R_color);
	
					R_color = clr_scale(R_color,1/5); 
                		}

				//times by kd
				R_color.r = color.r*closest_sph->mat_diffuse[1];
				R_color.g += color.g*closest_sph->mat_diffuse[2];
				R_color.b += color.b*closest_sph->mat_diffuse[3];

			}

			//add to Iphong
			color = clr_add(color, R_color);

		}

		//Refractions

		RGB_float refracted_color = {0,0,0};

		if((step < step_max) && (refraction_on) && (closest_sph->refractiveness))
		{
			Vector refract_vec= get_refracted_ray(surf_norm, light_vec, closest_sph, *hit);
			normalize(&refract_vec);

			step++;
			
			refract_vec.y=-1*refract_vec.y;
			refract_vec.x=-1*refract_vec.x;

			refracted_color = recursive_ray_trace(*hit, refract_vec, step);
			color = clr_add(color, clr_scale(refracted_color, 0.5));

		}
	}

	delete hit;

	return color;
}


/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //

      ret_color = recursive_ray_trace(eye_pos, ray, 1);

     // ret_color = background_clr; // just background for now

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

// Checkboard for testing
//RGB_float clr = {float(i/32), 0, float(j/32)};
//ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
