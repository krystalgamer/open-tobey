////////////////////////////////////////////////////////////////////////////////.


// geomgr.cpp

// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.

// geometry manager
// This is the geometry pipe.  This is where you put information about what
// kinds of transformations you want to be doing to the geometry, and if you're
// sending transformed vertices to the buffer, you use this class to do the
// transforming.


////////////////////////////////////////////////////////////////////////////////
#include "global.h"

#include "algebra.h"
#include "geomgr.h"
#include "hwrasterize.h"
#include "osdevopts.h"
#include "plane.h"
#include "rect.h"
#include "sphere.h"
#include "camera.h"

#include "profiler.h"
#include "osalloc.h"


//!class character;
#if defined(TARGET_XBOX)

#ifndef NGL
#error foo

#endif
#endif /* TARGET_XBOX JIV DEBUG */

DEFINE_SINGLETON(geometry_manager)


rational_t PROJ_ZOOM = 0.8f;
rational_t PROJ_RECIP_ZOOM = 1.0f;
rational_t PROJ_FAR_PLANE_D = 300.0f;
rational_t PROJ_FAR_PLANE_RHW = 1.0f / PROJ_FAR_PLANE_D;


////////////////////////////////////////////////////////////////////////////////
// geometry_manager
////////////////////////////////////////////////////////////////////////////////
geometry_manager::geometry_manager()
{
  xforms = (matrix4x4*)os_malloc32(NUM_XFORMS*sizeof(matrix4x4));
  for (int i=NUM_XFORMS; --i>=0; )
    xforms[i]=identity_matrix;


  scene_analyzer_enabled=false;
  scene_analyzer=identity_matrix;

  camera_pos = vector3d(0,0,0);
  camera_up  = vector3d(0,1,0);
  camera_dir = vector3d(0,0,1);
}

geometry_manager::~geometry_manager()
{
  os_free32(xforms);
  xforms = NULL;
}

void geometry_manager::restore()
{
  rebuild_view_frame();
}

void geometry_manager::set_clip_distance(rational_t clip_dist)
{
  PROJ_FAR_PLANE_D = clip_dist;
  PROJ_FAR_PLANE_RHW = 1.0F / ( clip_dist * PROJ_SIN_FOV );
#ifdef NGL

  ksnglSetPerspectiveMatrix( proj_field_of_view_in_degrees(),
    nglGetScreenWidth()/2,
    nglGetScreenHeight()/2,

    PROJ_NEAR_PLANE_D,
    PROJ_FAR_PLANE_D );
#endif
  rebuild_view_frame();
}

void geometry_manager::rebuild_view_frame()
{
	PANIC;
}


/*-----------------------------------------------------------------------------
 Name: set_look_at
 Desc: Given an eye point, a lookat point, and an up vector, this
       function builds a 4x4 view matrix.
-----------------------------------------------------------------------------*/
void geometry_manager::set_look_at( matrix4x4* dest,
                                    const vector3d& from, 
                                    const vector3d& look_at,
                                    const vector3d& up )
{
  matrix4x4 viewmat;
  // Get the z basis vector, which points straight ahead. This is the
  // difference from the eyepoint to the lookat point.
  vector3d vView = look_at - from;

  rational_t fLength = vView.length();
  
  // If we're jammed up, just point down the Z axis.  This really shouldn't be happening though.

  if ( fLength <= 1e-6f )

  {
  	vView = ZVEC;
  	fLength = 1;
  }

  // Normalize the z basis vector
  vView /= fLength;

  // Get the dot product, and calculate the projection of the z basis
  // vector onto the up vector. The projection is the y basis vector.
  rational_t fDotProduct = dot( up, vView );

  vector3d vUp = up - fDotProduct * vView;


  // If this vector has near-zero length because the input specified a
  // bogus up vector, let's try a default up vector
  if (1e-6f > (fLength = vUp.length()))
  {
    vUp = vector3d( 0.0f, 1.0f, 0.0f ) - vView.y * vView;

    // If we still have near-zero length, resort to a different axis.
    if (1e-6f > (fLength = vUp.length()))

    {
      vUp = vector3d( 0.0f, 0.0f, 1.0f ) - vView.z * vView;
      fLength = vUp.length();

      assert( 1e-6f <= fLength );
    }
  }

  // Normalize the y basis vector
  vUp /= fLength;

  // The x basis vector is found simply with the cross product of the y

  // and z basis vectors
  vector3d vRight = cross( vUp, vView );

  // Start building the matrix. The first three rows contains the basis
  // vectors used to rotate the view to point at the lookat point
  viewmat = identity_matrix;
  viewmat[0][0] = vRight.x;    viewmat[0][1] = vUp.x;    viewmat[0][2] = vView.x;
  viewmat[1][0] = vRight.y;    viewmat[1][1] = vUp.y;    viewmat[1][2] = vView.y;
  viewmat[2][0] = vRight.z;    viewmat[2][1] = vUp.z;    viewmat[2][2] = vView.z;

  // Do the translation values (rotations are still about the eyepoint)
  viewmat[3][0] = - dot( from, vRight );
  viewmat[3][1] = - dot( from, vUp );
  viewmat[3][2] = - dot( from, vView );

  *dest = viewmat;
}

void geometry_manager::set_view( const vector3d& from, 
                                 const vector3d& look_at,
                                 const vector3d& up )
{
  camera_pos = from;
  camera_up = up;

  // Get the z basis vector, which points straight ahead. This is the
  // difference from the eyepoint to the lookat point.
  vector3d vView = look_at - from;
  rational_t fLength = vView.length();
  
  // If we're jammed up, just point down the Z axis.  This really shouldn't be happening though.
  if ( fLength <= 1e-6f )
  {
  	vView = ZVEC;
  	fLength = 1;
  }

  // Normalize the z basis vector
  vView /= fLength;
  camera_dir = vView;


  matrix4x4 viewmat;
  set_look_at( &viewmat, from, look_at, up );
  set_xform( XFORM_WORLD_TO_VIEW, viewmat );
}

void geometry_manager::set_scene_analyzer( const vector3d& from, 
                         const vector3d& look_at,
                         const vector3d& up )
{
  set_look_at( &scene_analyzer, from, look_at, up );

}


void geometry_manager::set_xform( xform_t xformtype, const matrix4x4& matrix )
{
	PANIC;
}

bool geometry_manager::within_view( const sphere& s ) const
{
  // use view space to clip
  vector3d view_origin = xform3d_1_homog(xforms[XFORM_WORLD_TO_VIEW], s.get_origin());
  sphere view_sphere( view_origin, s.get_r() );
  // the clipping frustum is 6 planes:  
  //   z-front, z-back, 
  return ( view_frustum.includes( view_sphere ) );
}


// this returns radius in a funky coordinate system that I don't totally grok.
rational_t geometry_manager::radius_world_to_screen( const sphere& s ) const
{
  rational_t screen_radius = 1.0e32f;  // arbitrarily large number
  // check to make sure view z!=0 so we don't crash trying to do world-to-screen
  vector3d view_origin = xform3d_1_homog(xforms[XFORM_WORLD_TO_VIEW], s.get_origin());
  if(view_origin.z>=0.1f)
  {
    screen_radius = s.get_r() / view_origin.z * PROJ_NEAR_PLANE_D;
#ifndef NDEBUG
    // because screen space behaves in funky ways that I don't understand, I'm using
    // up vector to transform the center of the sphere and a point on its circumference.
//    vector3d screen_origin =  // unused -- remove me?
    xform3d_1_homog(get_world_to_screen(), s.get_origin());

    assert(( camera_up.length()>=0.999f )&&( camera_up.length()<=1.001f ));
    vector3d sphere_edge = s.get_origin() + camera_up * s.get_r();
    //vector3d screen_edge =  // unused -- remove me?
    xform3d_1_homog(get_world_to_screen(), sphere_edge);

    // don't use z to find distance between edge and center
//    rational_t alt_screen_radius = ( screen_edge - screen_origin ).xy_length(); // unused -- remove me?
    // this line is a fake:  there's no way screen_radius & alt_screen_radius
    // are ever going to be exactly equal, but we can examine them in the debugger
    // to make sure they're proportional
//    assert( alt_screen_radius != screen_radius );
#endif
  }

  return screen_radius;
}


void geometry_manager::set_cop(float x, float y, float min_z, float max_z)
{
	PANIC;
}
