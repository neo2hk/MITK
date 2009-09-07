/*****************************************************************************

 Copyright (c) 1993-2000,  Div. Medical and Biological Informatics, 
 Deutsches Krebsforschungszentrum, Heidelberg, Germany
 All rights reserved.

 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

 - All advertising materials mentioning features or use of this software must 
   display the following acknowledgement: 
          
     "This product includes software developed by the Div. Medical and 
      Biological Informatics, Deutsches Krebsforschungszentrum, Heidelberg, 
      Germany."

 - Neither the name of the Deutsches Krebsforschungszentrum nor the names of 
   its contributors may be used to endorse or promote products derived from 
   this software without specific prior written permission. 

   THIS SOFTWARE IS PROVIDED BY THE DIVISION MEDICAL AND BIOLOGICAL
   INFORMATICS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE DIVISION MEDICAL AND BIOLOGICAL INFORMATICS,
   THE DEUTSCHES KREBSFORSCHUNGSZENTRUM OR CONTRIBUTORS BE LIABLE FOR 
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN 
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

 Send comments and/or bug reports to:
   mbi-software@dkfz-heidelberg.de

*****************************************************************************/

/** @file
 *  this function adds a constant to an image
 */

/** @brief adds a constant to an image
 *  @param pic_old pointer to the first image
 *  @param value constant which is added to image
 *  @param keep tells whether the image type could be changed when
 *				necessary 
 *  @arg @c mitkIpFuncNoKeep : image data type could be changed
 *  @arg @c mitkIpFuncKeep :    image data type of original pictures 
 *                                        is  kept (if there will be an 
 *                                        over-/underflow the max. or min. 
 *                                        possible greyvalue is taken)
 *  @param pic_return  - memory used to store return image ( if pic_return == NULL 
 *                new memory is allocated ) 
 *
 *  @return pointer to the new image
 *
 *  @author Antje Schroeder
 *  @date 01.08.95
 * COPYRIGHT (c) 1995 by DKFZ (Dept. MBI) HEIDELBERG, FRG
 */

/* include-Files                                                        */

#include "mitkIpFuncP.h"

mitkIpPicDescriptor *mitkIpFuncAddC ( mitkIpPicDescriptor *pic_old,
                              ipFloat8_t      value, 
                              mitkIpFuncFlagI_t   keep,
                              mitkIpPicDescriptor *pic_return );

#ifndef DOXYGEN_IGNORE

/* definition of macros                                                 */

#define ADDC( type_1, pic_1, pic_new, value )                            \
{                                                                        \
  mitkIpPicFORALL_3 ( ADDC2, pic_new, pic_1, type_1, value );                \
}                                                                        \
 

#define ADDC2( type_n, pic_new, pic_1, type_1, value )                   \
{                                                                        \
  ipUInt4_t  i, no_elem;                                                 \
                                                                         \
  no_elem = _mitkIpPicElements ( pic_1 );                                    \
  for ( i = 0; i < no_elem; i++ )                                        \
    {                                                                    \
       (( type_n * ) pic_new->data ) [i] = ( type_n )                    \
             ( value + (( type_1 * ) pic_1->data ) [i] );                \
    }                                                                    \
}

#define ADDC3( type_n, pic_1, pic_new, value )                           \
{                                                                        \
  ipUInt4_t  i, no_elem;                                                 \
  type_n     help;                                                       \
                                                                         \
  no_elem = _mitkIpPicElements ( pic_1 );                                    \
  for ( i = 0; i < no_elem; i++ )                                        \
    {                                                                    \
       help  = (( type_n * ) pic_1->data ) [i];                          \
       (( type_n * ) pic_new->data ) [i] =                               \
          ( max_gv > ( ipFloat8_t ) help + value ) ?                     \
             (( min_gv < ( ipFloat8_t ) help + value ) ?                 \
                 ( (type_n)help + (type_n)value ) : ( type_n ) min_gv ) :\
             ( type_n ) max_gv;                                          \
    }                                                                    \
}



/* -------------------------------------------------------------------  */
/*
*/
/* -------------------------------------------------------------------  */

mitkIpPicDescriptor *mitkIpFuncAddC ( mitkIpPicDescriptor *pic_old,
                              ipFloat8_t      value, 
                              mitkIpFuncFlagI_t   keep,
                              mitkIpPicDescriptor *pic_return )
{

  mitkIpPicDescriptor *pic_new;         /* pointer to new image             */
  ipFloat8_t      max_gv;           /* max. possible greyvalue          */
  ipFloat8_t      min_gv;           /* min. possible greyvalue          */
  ipFloat8_t      min1, max1;       /* extreme greyvalues of 1. image   */ 
  ipFloat8_t      smin, smax;       /* product of extreme greyvalues    */


  /* check image data                                                   */

  if ( _mitkIpFuncError ( pic_old ) != mitkIpFuncOK ) return ( mitkIpFuncERROR );

  /* check value                                                        */

  if ( value == 0. ) return ( pic_old );

  /* calculate max. and min. possible greyvalues for data type of images*/

  if ( _mitkIpFuncExtT ( pic_old->type, pic_old->bpe, &min_gv, &max_gv ) != mitkIpFuncOK )
    return ( mitkIpFuncERROR );

  /* find out data type of new iamge                                    */

  if ( keep == mitkIpFuncKeep )
    {
       pic_new = _mitkIpFuncMalloc ( pic_old, pic_return, mitkIpOVERWRITE );
       if ( pic_new == NULL ) return ( mitkIpFuncERROR );
    }
  else if ( keep == mitkIpFuncNoKeep )
    {
       /* calculate max. and min. greyvalues of both images                  */

       if ( mitkIpFuncExtr ( pic_old, &min1, &max1 ) != mitkIpFuncOK ) return ( mitkIpFuncERROR );

       smax      = max1 + value;
       smin      = min1 + value;

       /* change image type of images of type mitkIpPicInt                  */

       if ( pic_old->type == mitkIpPicInt )
         {
           if ( smax < max_gv && smin > min_gv ) 
             {
                pic_new = mitkIpPicCopyHeader ( pic_old, NULL );
             }
           else
             {
                pic_new       = mitkIpPicCopyHeader ( pic_old, NULL );
                pic_new->type = mitkIpPicFloat;
                pic_new->bpe  = 64;
                _mitkIpFuncExtT ( pic_new->type, pic_new->bpe, &min_gv, &max_gv );
             }
         }

       /* change image type of images of type mitkIpPicUInt                 */

       else if ( pic_old->type == mitkIpPicUInt )
         {
           if ( smax < max_gv && smin > min_gv )
             {
                pic_new = mitkIpPicCopyHeader ( pic_old, NULL );
             }
           else
             {
                pic_new = mitkIpPicCopyHeader ( pic_old, NULL );
                pic_new->type = mitkIpPicInt;
                pic_new->bpe  = 16;
                _mitkIpFuncExtT ( pic_new->type, pic_new->bpe, &min_gv, &max_gv );
                if ( smax > max_gv || smin < min_gv )
                  {
                     pic_new->type = mitkIpPicFloat;
                     pic_new->bpe  = 64;
                     _mitkIpFuncExtT ( pic_new->type, pic_new->bpe, &min_gv, &max_gv );
                  }
             }
         } 

       /* change image type of images of type mitkIpPicUInt                 */
 
       else if ( pic_old->type == mitkIpPicFloat )
         {
            pic_new = mitkIpPicCopyHeader ( pic_old, NULL );
         }
       else 
         {
            _mitkIpFuncSetErrno ( mitkIpFuncTYPE_ERROR );
            return ( mitkIpFuncERROR );
         }
       
    }
  else
    {
       _mitkIpFuncSetErrno ( mitkIpFuncFLAG_ERROR );
       return ( mitkIpFuncERROR );
    }

  if ( pic_new == NULL ) 
    {
       _mitkIpFuncSetErrno ( mitkIpFuncPICNEW_ERROR );
       return ( mitkIpFuncERROR );
    }
 
  if ( keep == mitkIpFuncNoKeep )
     pic_new->data = malloc ( _mitkIpPicSize  ( pic_new ) );
  if ( pic_new->data == NULL )
    {
       mitkIpPicFree ( pic_new );
       _mitkIpFuncSetErrno ( mitkIpFuncMALLOC_ERROR );
       return ( mitkIpFuncERROR );
    }

  /* macro to invert the picture (for all data types)                   */

  if ( keep == mitkIpFuncNoKeep )
    mitkIpPicFORALL_2 ( ADDC, pic_old, pic_new, value )
  else if ( keep == mitkIpFuncKeep )
    mitkIpPicFORALL_2 ( ADDC3, pic_old, pic_new, value )

  /* Copy Tags */

  mitkIpFuncCopyTags(pic_new, pic_old);
  
    

  return pic_new;
}
#endif
