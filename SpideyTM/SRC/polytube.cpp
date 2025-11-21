#include "global.h"

#include "polytube.h"
#include "profiler.h"
#include "game.h"
#include "geomgr.h"
#include "b_spline.h"
#include "matfac.h"

#include "material.h"
#include "vertwork.h"
//#include "ksngl.h"	// For KSNGL_CreateScratchMesh (dc 06/06/02)
#ifdef TARGET_GC
#include "ngl.h"
#endif

//#include "ngl_support.h"

#ifdef NGL

  bool g_render_polytubes = true;

  void render_polytube(const std::vector<vector3d> &pts, rational_t radius, int num_sides, const color32 &col, mat_fac *the_material, rational_t tiles_per_meter, const matrix4x4 &the_matrix, rational_t max_length)
  {
	  PANIC;
  }


#else

  void render_polytube(const std::vector<vector3d> &pts, rational_t radius, int num_sides, const color32 &col, mat_fac *the_material, rational_t tiles_per_meter, const matrix4x4 &the_matrix, rational_t max_length)
  {
	  PANIC;
  }

#endif

























polytube::polytube( const entity_id& _id, unsigned int _flags )
  : entity( _id, _flags )
{

  init();
  flavor = ENTITY_POLYTUBE;
}


polytube::polytube( chunk_file& fs,
        const entity_id& _id,
        entity_flavor_t _flavor,
        unsigned int _flags )
  : entity(fs, _id, _flavor, _flags)
{
  init();
}


polytube::~polytube()
{
  if ( my_material != NULL )
  {
    delete my_material;
    my_material = NULL;
  }
}

void polytube::init()
{
  my_material = NULL;

  tiles_per_meter = 1.0f;
  num_sides = 5;

  tube_radius = 0.025f;
  use_spline = true;
  entity::set_flag(EFLAG_GRAPHICS, true);
  max_length = -1.0f;
}



entity* polytube::make_instance( const entity_id& _id,
                             unsigned int _flags ) const
{
  polytube* newit = NEW polytube( _id, _flags );
  newit->copy_instance_data( *((polytube *)this) );

  return (entity*)newit;
}

void polytube::copy_instance_data( const polytube& b )
{
  set_material(b.get_material());


  spline = b.spline;
  tiles_per_meter = b.tiles_per_meter;
  num_sides = b.num_sides;
  tube_radius = b.radius;
  use_spline = b.use_spline;
  max_length = b.max_length;

  entity::copy_instance_data(b);
}


render_flavor_t polytube::render_passes_needed() const
{
  render_flavor_t passes = 0;

  if ( (my_material && my_material->is_translucent()) || render_color.get_alpha() < 0xFF)
    passes |= RENDER_TRANSLUCENT_PORTION;
  else
    passes |= RENDER_OPAQUE_PORTION;

  return passes;

}

void polytube::set_material( const stringx& file )
{

  if(my_material == NULL)
    my_material = NEW mat_fac();


  my_material->load_material(file);
}


void polytube::set_material(mat_fac *mat)
{
  if(mat != NULL)
  {
    if(my_material == NULL)
      my_material = NEW mat_fac(*mat);
    else
      *my_material = *mat;
  }
  else
  {
    if(my_material != NULL)
    {
      delete my_material;
      my_material = NULL;
    }
  }
}




void polytube::render( camera* camera_link, rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct )
{
#ifdef NGL

  my_material->get_ngl_material()->MapBlendMode = NGLBM_BLEND;
  my_material->get_ngl_material()->Flags |= NGLMAT_ALPHA;
  my_material->get_ngl_material()->Flags |= NGLMAT_ANTIALIAS;

#endif

  render_polytube(use_spline ? spline.get_curve_pts() : spline.get_control_pts(), tube_radius, num_sides, render_color, my_material, tiles_per_meter, get_abs_po().get_matrix(), max_length);
}
