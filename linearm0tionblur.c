/*
Version 0.2	may 2013

Copyright (C) 2013  Gaspar Fernández http://totaki.com/poesiabinaria

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

//compile: gcc -c -fPIC -Wall mblur2.c -o mblur2.o && gcc -shared -o mblur2.so mblur2.o `Magick-congfig --libs` `Magick-config --cflags`

//*******************************************************************

#include <stdio.h>  
#include <frei0r.h>
#include <stdlib.h>
#include <assert.h>
#include <magick/MagickCore.h>
#include "mblurconf.h"
#include "frei0r_utils.h"

typedef struct
{
//status
  videosize_t size;

  double angle;
  double radius;
  double deviation;

  double preview;			/* Applies motion blur to a resized frame */
} Tinstance;

param0r_def parameters[4];

int f0r_init()
{
  parameters[0] = f0ut_set_double_param("angle", "Angle", 45, 0, 360);
  parameters[1] = f0ut_set_double_param("radius", "Radius", 50, 0, 1000);
  parameters[2] = f0ut_set_double_param("deviation", "Standard deviation", 50, 0, 1000);
  parameters[3] = f0ut_set_double_param("preview", "Preview", 5, 0, 10);
  return 1;
}

//------------------------------------------------
void f0r_deinit()
{
}

//-----------------------------------------------
void f0r_get_plugin_info(f0r_plugin_info_t* info)
{
  f0ut_set_plugin_info(info, 
		       FREI0R_PLUGIN_NAME,
		       FREI0R_PLUGIN_AUTHOR,
		       FREI0R_PLUGIN_DESCRIPTION,
		       F0R_PLUGIN_TYPE_FILTER,
		       F0R_COLOR_MODEL_RGBA8888,
		       FREI0R_PLUGIN_VERSION_MA,
		       FREI0R_PLUGIN_VERSION_MI,
		       4);
}

//--------------------------------------------------
void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  f0ut_get_param_info (info, &parameters[param_index]);
}

//----------------------------------------------
f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  Tinstance *in;

  in=calloc(1,sizeof(Tinstance));
  f0ut_set_videosize(&in->size, width, height);

  //default values
  f0ut_set_default_value(&in->angle, &parameters[0], &in->size);
  f0ut_set_default_value(&in->radius, &parameters[1], &in->size);
  f0ut_set_default_value(&in->deviation, &parameters[2], &in->size);
  f0ut_set_default_value(&in->preview, &parameters[3], &in->size);

  return (f0r_instance_t)in;
}

//---------------------------------------------------
void f0r_destruct(f0r_instance_t instance)
{
  Tinstance *in;

  in=(Tinstance*)instance;

  free(in);
}

//-----------------------------------------------------
void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param, int param_index)
{
  Tinstance *in;

  in=(Tinstance*)instance;

  if (param_index==0)
    f0ut_set_param_value(&in->size, &parameters[0], param, (void*)&in->angle);
  else if (param_index==1)
    f0ut_set_param_value(&in->size, &parameters[1], param, (void*)&in->radius);
  else if (param_index==2)
    f0ut_set_param_value(&in->size, &parameters[2], param, (void*)&in->deviation);
  else if (param_index==3)
    f0ut_set_param_value(&in->size, &parameters[3], param, (void*)&in->preview);
}

//--------------------------------------------------
void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param, int param_index)
{
  Tinstance *in;

  in=(Tinstance*)instance;

  if (param_index==0)
    f0ut_get_param_value(&in->size, &parameters[0], param, (void*)&in->angle);
  else if (param_index==1)
    f0ut_get_param_value(&in->size, &parameters[1], param, (void*)&in->radius);
  else if (param_index==2)
    f0ut_get_param_value(&in->size, &parameters[2], param, (void*)&in->deviation);
  else if (param_index==3)
    f0ut_get_param_value(&in->size, &parameters[3], param, (void*)&in->preview);
}

void minify(Image **image, int steps, ExceptionInfo *exception)
{
  int i;

  Image *tmpimg=NULL;
  for (i=0; i<steps; i++)
    {
      tmpimg = MinifyImage(*image, exception);
      DestroyImage(*image);
      *image=tmpimg;
    }
  /* tmpimg = MinifyImage(image, exception); */
  /* image = tmpimg; */
}

void magnify(Image **image, int steps, ExceptionInfo *exception)
{
  int i;

  Image *tmpimg=NULL;
  for (i=0; i<steps; i++)
    {
      tmpimg = MagnifyImage(*image, exception);
      DestroyImage(*image);
      *image=tmpimg;
    }
  /* tmpimg = MagnifyImage(image, exception); */
  /* image = tmpimg; */
}

//==============================================================
void f0r_update(f0r_instance_t instance, double time, const uint32_t* inframe, uint32_t* outframe)
{
  Tinstance *in;
  in=(Tinstance*)instance;
  ExceptionInfo *exception;
  /* Generamos la excepción */
  exception = AcquireExceptionInfo();
  /* Transformamos el mapa de imagen de entrada para imagemagick */
  Image *im = ConstituteImage(in->size.width, in->size.height, "RGBA", CharPixel, inframe, exception);

  if (in->preview) 
    {
      minify(&im, (int)in->preview, exception);
    }
  /* Generamos la imagen de salida */
  Image *out = MotionBlurImage(im, in->radius, in->deviation, in->angle, exception);
  if (in->preview) 
    {
      magnify(&out, (int)in->preview, exception);
    }
  /* Volvemos a coger el contenido de la imagen para frei0r */
  ExportImagePixels(out, 0, 0, in->size.width, in->size.height, "RGBA", CharPixel,  outframe, exception);

  /* Liberamos memoria */
  exception = DestroyExceptionInfo(exception);
  im = DestroyImage(im);
  out = DestroyImage(out);
}
