#include "global.h"

#include "algebra.h"
#include "lensflare.h"
#include "profiler.h"
#include "game.h"
#include "geomgr.h"
#include "vertwork.h"
#include "matfac.h"
//#include "material.h"
#include "debug_render.h"
#include "osdevopts.h"
#include "collide.h"
#include "random.h"
#include "blendmodes.h"
#include "forceflags.h"
//#include "beachdata.h"
#if defined(TARGET_XBOX)
#include "wds.h"
#include "conglom.h"
#endif /* TARGET_XBOX JIV DEBUG */

/*
#include "kellyslater_controller.h"
#include "wave.h"
#include "ksngl.h"	// For KSNGL_CreateScratchMesh
					*/

void lensflare::init()
{
  amount = 0.0f;
  target = 0.0f;


  nflares = 0;

  // start off lensflares randomly offset from time 0.0, this should
  // evenly spread their line of sight checks.
  los_freq = 0.2f;
  los_time = random( los_freq );

  entity::set_flag(EFLAG_GRAPHICS, true);
}


// TODO: Check which constructors are actually required.
lensflare::lensflare( const entity_id& _id, unsigned int _flags )

: entity( _id, ENTITY_LENSFLARE, _flags )
{
  init();
}

lensflare::lensflare( const lensflare& b )
: entity( b )
{
  init();
  copy_instance_data( b );
}

lensflare::lensflare(const entity_id& _id, entity_flavor_t _flavor, unsigned int _flags,
                     float _speed, float _losfreq, float _farrange,
                     int _nflares, stringx *textures, color *theColors, float *offsets, float *sizes)
: entity(_id, _flavor, _flags)
{
	PANIC;
}


lensflare::lensflare( chunk_file& fs, const entity_id& _id, entity_flavor_t _flavor, unsigned int _flags )
  : entity(_id, _flavor, _flags)
{
	PANIC;
}


lensflare::~lensflare()
{
  for ( int i = 0; i < nflares; i++ )
    if ( flares[i].texture != NULL )
    {
      delete flares[i].texture;
      flares[i].texture = NULL;
    }
}

entity* lensflare::make_instance( const entity_id& _id, unsigned int _flags ) const
{
  lensflare* newit = NEW lensflare( _id, _flags );
  newit->copy_instance_data( *((lensflare *)this) );

  return (entity*)newit;
}

void lensflare::copy_instance_data( const lensflare& b )

{
  speed = b.speed;
  amount = b.amount;
  target = b.target;

  farrange = b.farrange;

  nflares = b.nflares;
  for ( int i = 0; i < nflares; i++ )
  {
    flares[i].texture = NEW mat_fac( *b.flares[i].texture );
    flares[i].offset = b.flares[i].offset;
    flares[i].size = b.flares[i].size;
    flares[i].col = b.flares[i].col;
  }


  los_freq = b.los_freq;
  los_time = random( los_freq );

  entity::copy_instance_data(b);

}


#define INBETWEEN(a,x,y) (x > y?((a > y) && (a < x)):((a < y) && (a > x)))
bool blocked_by_wave()
{
	PANIC;
	return false;
}
void lensflare::frame_advance( time_value_t t )
{
	PANIC;
}

// basic billboard structure
struct billboard
{
  vector3d pos;     // world space position
  float angle;      // 2d rotation angle
  color32 c;        // color

  float w, h;       // horizontal and vertical size (world units)

//  float u0, v0;     // top left tex coords
//  float u1, v1;     // bottom right tex coords
};

// Creates a view space scratch mesh containing the billboards.
void render_billboards( int num, billboard* bb, mat_fac* mat )
{
	PANIC;
}

billboard lensflare_bbs[8];


void lensflare::render( camera* camera_link, rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct )
{
//  print_3d_text( get_abs_position(), color32_white, "flare: %f - %f", amount, target );
  if (( amount <= 0.0f)
//	  || WAVE_CameraUnderwater() // should no longer be necessary. (dc 07/06/02)
	  )
    return;


  vector3d eyep = geometry_manager::inst()->get_camera_pos();
  vector3d eyen = geometry_manager::inst()->get_camera_dir();
  vector3d fpos = get_abs_position();
  vector3d f2eye = fpos - eyep;


  float angledot=dot(f2eye, eyen);
  angledot/=eyen.length();
  angledot/=(fpos-eyep).length();
  float myscale = 0;
  if (angledot > .5f)

    myscale = (powf(11.0f, (angledot-.5f)/.5f) - 1)/5;

  float d = dot(eyen, f2eye);
  if (d <= PROJ_NEAR_PLANE_D )
    return;
  // could instead use the viewer pos to compute influence, then multiply by 2.0!
  //float atten = 1.0f-d/min(3.0F*brightness,PROJ_FAR_PLANE_D+1.0f);

  float atten = 1.0f-sqr(d)/sqr(farrange+1.0f);
  if (atten <= 0)
    return;
  atten *= amount;//*0.3f;

//  float ang=PI*0.5F*(eyen.x+eyen.y);


  float sclz=(PROJ_NEAR_PLANE_D+1e-3f)/d;


  for ( int i = 0; i < nflares; i++ )
  {
    billboard* bb = &lensflare_bbs[i];


/*	This doesn't seem to be doing what's intended.  (dc 10/17/01)
    color fade = flares[i].col*atten*farrange;
    fade.rescale();
    bb->c = fade.to_color32();
*/

    vector3d flarepos=lerp(fpos,eyep+eyen*d,flares[i].offset);
    flarepos=lerp(eyep,flarepos,sclz); // move to front plane
    bb->pos = flarepos;

    //float scale=min(atten*brightness*brightness*0.009F,3.0F*nearz);
    bb->w = bb->h = flares[i].size * sclz * myscale*(flares[i].offset + 1);

    bb->angle = cosf(angledot);

	// The regular attentuation didn't work right.  The flares would not become 

	// translucent even when they became very small.  Replaced with simpler 
	// system.  (dc 07/11/02)
	static float lensflare_attenfactor = 4;

	atten = 1 - lensflare_attenfactor * (1 - angledot);
	if (atten < 0) atten = 0;
    color fade = flares[i].col * atten;
    bb->c = fade.to_color32();

    render_billboards( 1, bb, flares[i].texture );
  }
}
