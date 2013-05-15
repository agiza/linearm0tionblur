/* @(#)frei0r_utils.h
 */
/*
  Version 0.2	May 2013

  Copyright (C) 2013  Gaspar Fernández http://totaki.com/poesiabinaria
  					http://gaspar.totaki.com/

    This file is part of frei0r_utils

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
    ********************************************************************
*/

#ifndef _FREI0R_UTILS_H
#define _FREI0R_UTILS_H 1

#include <frei0r.h>

typedef struct
{
  int width;
  int height;
} videosize_t;

typedef struct
{
  char *name;
  char *explanation;
  int type;
  /* Ranges */
  double range_min;		/* Double values are 0..1 but we would need another range */
  double range_max;
  /* Default values */
  double val1, val2, val3;	/* Useful, por double types (val1), positions(val1,val2), colors(val1,val2,val3), bool (val1=0.0 or val1!=0.0) */
  char *strdefault;
} param0r_def;

#define GET_PIXEL(image, width, x, y, pixdata) {	 \
    pixdata[0] = image[(y*width + x)*3];		 \
    pixdata[1] = image[(y*width + x)*3+1];		 \
    pixdata[2] = image[(y*width + x)*3+2];		 \
    pixdata[3] = image[(y*width + x)*3+3];		 \
  }

#define SET_COLOR_VAR(var, rv, gv, bv) { \
    var.r=rv;				 \
    var.g=gv;				 \
    var.b=bv;				 \
  }

#define SET_POSITION_VAR(var, xv, yv) { \
    var.x=xv;				\
    var.y=yv;				\
  }

#define MAX2(a, b) ((a)>(b)?(a):(b))
#define MIN2(a, b) ((a)<(b)?(a):(b))

void f0ut_set_plugin_info(f0r_plugin_info_t* info, char* name, char* author, char* description,
			  int plugin_type, int color_model, int major_version,
			  int minor_version, int num_params);

void f0ut_set_videosize(videosize_t* vsz, int width, int height);

// from Marko Cebokli    http://lea.hamradio.si/~s57uuu
//stretch [0...1] to parameter range [min...max] linear
float map_value_forward(double v, float min, float max);
// from Marko Cebokli    http://lea.hamradio.si/~s57uuu
//collapse from parameter range [min...max] to [0...1] linear
double map_value_backward(float v, float min, float max);

void f0ut_set_param_value(const videosize_t *videosize, const param0r_def *pdef, f0r_param_t paramval, void *instanceVar);
void f0ut_get_param_value(const videosize_t *videosize, const param0r_def *pdef, f0r_param_t paramVal, void *instanceVar);

void f0ut_param_set_basis(param0r_def *param, char *name, char *explanation, int type);
param0r_def f0ut_set_bool_param(char *name, char *explanation, int default_value);
param0r_def f0ut_set_double_param(char *name, char *explanation, double def, double min, double max);
param0r_def f0ut_set_color_param(char *name, char *explanation, int r, int g, int b);
param0r_def f0ut_set_position_param(char *name, char *explanation, int x, int y);
param0r_def f0ut_set_string_param(char *name, char *explanation, char *default_value);

void f0ut_param0r_free(param0r_def* parameters, int num);
void f0ut_get_param_info(f0r_param_info_t *pInfo, param0r_def *pdef);
void f0ut_set_default_value(void *instanceVar, const param0r_def *pdef, const videosize_t *vsz);

#endif /* _FREI0R_UTILS_H */

