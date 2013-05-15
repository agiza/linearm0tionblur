/*
  Version 0.2	May 2013 frei0r_utils Tools to make easier 
			 frei0r plugin creation

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

#include "frei0r_utils.h"
#include <string.h>
#include <stdlib.h>

void f0ut_set_plugin_info(f0r_plugin_info_t* info, char* name, char* author, char* description,
			  int plugin_type, int color_model, int major_version,
			  int minor_version, int num_params)
{
  info->name=name;
  info->author=author;
  info->plugin_type=plugin_type;
  info->color_model=color_model;
  info->frei0r_version=FREI0R_MAJOR_VERSION;
  info->major_version=major_version;
  info->minor_version=minor_version;
  info->num_params=num_params;
  info->explanation=description;
}

void f0ut_set_videosize(videosize_t* vsz, int width, int height)
{
  vsz->width=width;
  vsz->height=height;
}

float map_value_forward(double v, float min, float max)
{
  return min+(max-min)*v;
}

double map_value_backward(float v, float min, float max)
{
  return (v-min)/(max-min);
}

void f0ut_param_set_basis(param0r_def *param, char *name, char *explanation, int type)
{
  param->name=malloc(sizeof(char)*(strlen(name)+1));
  param->explanation=malloc(sizeof(char)*(strlen(explanation)+1));

  strcpy(param->name, name);
  strcpy(param->explanation, explanation);

  param->type = type;
}

void f0ut_get_param_value(const videosize_t *videosize, const param0r_def *pdef, f0r_param_t paramVal, void *instanceVar)
{
  switch (pdef->type)
    {
    case F0R_PARAM_BOOL:
      *(int*)paramVal = *(int*)instanceVar;
      break;
    case F0R_PARAM_DOUBLE:
      *(double*)paramVal = map_value_backward(*(double*)instanceVar, pdef->range_min, pdef->range_max);
      break;
    case F0R_PARAM_COLOR:
      ((f0r_param_color_t*)paramVal)->r = map_value_backward(((f0r_param_color_t*)instanceVar)->r, 0, 255);
      ((f0r_param_color_t*)paramVal)->g = map_value_backward(((f0r_param_color_t*)instanceVar)->g, 0, 255);
      ((f0r_param_color_t*)paramVal)->b = map_value_backward(((f0r_param_color_t*)instanceVar)->b, 0, 255);
      break;
    case F0R_PARAM_POSITION:
      ((f0r_param_position_t*)paramVal)->x = map_value_backward(((f0r_param_position_t*)instanceVar)->x, 0, videosize->width);
      ((f0r_param_position_t*)paramVal)->x = map_value_backward(((f0r_param_position_t*)instanceVar)->y, 0, videosize->height);
      break;
    case F0R_PARAM_STRING:
      free (*(char**)paramVal);
      *(char**)paramVal= malloc(sizeof(char)*(strlen(*(char**)instanceVar))+1);
      strcpy(*(char**)paramVal, *(char**)instanceVar);
      break;
    }
}

void f0ut_set_param_value(const videosize_t *videosize, const param0r_def *pdef, f0r_param_t paramVal, void *instanceVar)
{
  switch (pdef->type)
    {
    case F0R_PARAM_BOOL:
      *(int*)instanceVar = (*(int*)paramVal);
      break;
    case F0R_PARAM_DOUBLE:
      *(double*)instanceVar = map_value_forward((*(double*)paramVal), pdef->range_min, pdef->range_max);
      break;
    case F0R_PARAM_COLOR:
      ((f0r_param_color_t*)instanceVar)->r = map_value_forward(((f0r_param_color_t*)paramVal)->r, 0, 255);
      ((f0r_param_color_t*)instanceVar)->g = map_value_forward(((f0r_param_color_t*)paramVal)->g, 0, 255);
      ((f0r_param_color_t*)instanceVar)->b = map_value_forward(((f0r_param_color_t*)paramVal)->b, 0, 255);
      break;
    case F0R_PARAM_POSITION:
      ((f0r_param_position_t*)instanceVar)->x = map_value_forward(((f0r_param_position_t*)paramVal)->x, 0, videosize->width);
      ((f0r_param_position_t*)instanceVar)->x = map_value_forward(((f0r_param_position_t*)paramVal)->y, 0, videosize->height);
      break;
    case F0R_PARAM_STRING:
      free (*(char**)instanceVar);
      *(char**)instanceVar= malloc(sizeof(char)*(strlen(*(char**)paramVal))+1);
      strcpy(*(char**)instanceVar, *(char**)paramVal);
      break;
    }
}

/* Mover a otra función el set_default_value */
void f0ut_set_default_value(void *instanceVar, const param0r_def *pdef, const videosize_t *vsz)
{
  f0r_param_color_t tmp_color;
  f0r_param_position_t tmp_pos;

  switch (pdef->type) 
    {
    case F0R_PARAM_BOOL:
      *(int*)instanceVar = (int)pdef->val1;
    case F0R_PARAM_DOUBLE:
      *(double*)instanceVar = (double)pdef->val1;
      //      f0ut_set_param_value(NULL, pdef, (f0r_param_t*)&pdef->val1, instanceVar); 
      break;
    case F0R_PARAM_COLOR:
      SET_COLOR_VAR(tmp_color, pdef->val1, pdef->val2, pdef->val3);
      //      f0ut_set_param_value(NULL, pdef, (f0r_param_t*)&tmp_color, instanceVar);
      *(f0r_param_color_t*)instanceVar = tmp_color;
      break;
    case F0R_PARAM_POSITION:
      SET_POSITION_VAR(tmp_pos, pdef->val1, pdef->val2);
      *(f0r_param_position_t*)instanceVar = tmp_pos;
      //      f0ut_set_param_value(vsz, pdef, (f0r_param_t*)&tmp_pos, instanceVar);
      break;
    case F0R_PARAM_STRING:
      f0ut_set_param_value(NULL, pdef, (f0r_param_t*)&pdef->strdefault, instanceVar);
      break;
    }
}

param0r_def f0ut_set_bool_param(char *name, char *explanation, int default_value)
{
  param0r_def pdef;
  f0ut_param_set_basis(&pdef, name, explanation, F0R_PARAM_BOOL);

  pdef.val1=(default_value)?1.0:0.0;

  return pdef;
}

param0r_def f0ut_set_double_param(char *name, char *explanation, double def, double min, double max)
{
  param0r_def pdef;
  f0ut_param_set_basis(&pdef, name, explanation, F0R_PARAM_DOUBLE);

  pdef.val1=def;
  pdef.range_min=min;
  pdef.range_max=max;

  return pdef;
}

param0r_def f0ut_set_color_param(char *name, char *explanation, int r, int g, int b)
{
  param0r_def pdef;

  f0ut_param_set_basis(&pdef, name, explanation, F0R_PARAM_COLOR);

  pdef.val1=(double)r;
  pdef.val2=(double)g;
  pdef.val3=(double)b;

  return pdef;

}

param0r_def f0ut_set_position_param(char *name, char *explanation, int x, int y)
{
  param0r_def pdef;

  f0ut_param_set_basis(&pdef, name, explanation, F0R_PARAM_POSITION);

  pdef.val1=(double)x;
  pdef.val2=(double)y;

  return pdef;
}

param0r_def f0ut_set_string_param(char *name, char *explanation, char *default_value)
{
  param0r_def pdef;
  f0ut_param_set_basis(&pdef, name, explanation, F0R_PARAM_STRING);

  pdef.strdefault=malloc(sizeof(char)*(strlen(default_value)+1));
  strcpy(pdef.strdefault, default_value);

  return pdef;
}

void f0ut_param0r_free(param0r_def* parameters, int num)
{
  int i;

  for (i=0; i<num; i++)
    {
      free(parameters[i].name);
      free(parameters[i].explanation);

      if (parameters[i].type == F0R_PARAM_STRING)
	free(parameters[i].strdefault);
    }
}

void f0ut_get_param_info(f0r_param_info_t *pInfo, param0r_def *pdef)
{
  pInfo->name = pdef-> name;
  pInfo->type = pdef-> type;
  pInfo->explanation=pdef -> explanation;
}
