////////////////////////////////////////////////////////////////////////////////

// pmesh.cpp
// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.

// possibly the slowest renderer ever built, brought to you courtesy of
// James Fristrom

////////////////////////////////////////////////////////////////////////////////


#include "global.h"

#include "algebra.h"
#include "po.h"
#include "app.h"
#include "bound.h"
#include "clipflags.h"

#include "color.h"

#include "game.h"
#include "geomgr.h"
#include "hwmath.h"
#include "hwrasterize.h"
#include "iri.h"
#include "light.h"
#include "lightmgr.h"

#include "maxiri.h"
#include "osalloc.h"
#include "osdevopts.h"
#include "oserrmsg.h"
#include "pmesh.h"
#include "profiler.h"
#include "project.h"
#include "renderflav.h"
#include "terrain.h"
#include "vertnorm.h"
#include "vertwork.h"
#include "vsplit.h"
#include "wds.h"
#include "widget.h"
#include <algorithm>

extern int global_frame_counter;

// evil
static int how_many_verts;

unsigned short* c_skin_tri_setup_nonprog( face* face_start, int face_count, hw_rasta_vert_lit* vert_begin, unsigned short* index_pool )

{
  face* my_face;
  face* face_end = face_start + face_count;
  unsigned short* index_ptr = index_pool;
  for( my_face = face_start; my_face != face_end; ++my_face )
  {
    int wedge0, wedge1, wedge2;
    wedge0 = my_face->wedge_refs[0];
    wedge1 = my_face->wedge_refs[1];
    wedge2 = my_face->wedge_refs[2];

    *index_ptr++ = wedge0;
    *index_ptr++ = wedge1;
    *index_ptr++ = wedge2;
  }
  // tally these anyway to not fool people in to thinking there's fewer tri's out there
  // than there are.
  return index_ptr;
}

unsigned short* c_skin_tri_setup_prog( vr_pmesh* me, int start, int end, hw_rasta_vert_lit* vert_begin, unsigned short* index_ptr, int level_of_detail )
{
  face* my_face;
  face* face_start = me->faces+start;
  face* face_end = me->faces+end;
  wedge* wedges = me->wedges;

  for ( my_face = face_start;
        my_face != face_end;
        ++my_face )
  {

    if( my_face->level_of_detail <= level_of_detail )
    {
      wedge_ref wr0, wr1, wr2;
      wedge* my_wedge;

      wr0 = my_face->wedge_refs[0];
      my_wedge = wedges + wr0;
      while( my_wedge->level_of_detail > level_of_detail )
      {
        wr0 = my_wedge->lower_detail;
        my_wedge = wedges + wr0;
      }
      assert( wr0 < how_many_verts );
//      hw_rasta_vert_lit* v0 = vert_begin+wr0; // unused -- remove me?

    #if defined(TARGET_MKS)
      if( v0->xyz.z < 0.01f )
        goto front_clipped;
    #endif
      wr1 = my_face->wedge_refs[1];
      my_wedge = wedges + wr1;
      while( my_wedge->level_of_detail > level_of_detail )
      {
        wr1 = my_wedge->lower_detail;
        my_wedge = wedges + wr1;
      }
      assert( wr1 < how_many_verts );
    #if defined(TARGET_MKS)
      if( v1->xyz.z < 0.01f )

        goto front_clipped;
    #endif
      wr2 = my_face->wedge_refs[2];
      my_wedge = wedges + wr2;
      while( my_wedge->level_of_detail > level_of_detail )
      {
        wr2 = my_wedge->lower_detail;
        my_wedge = wedges + wr2;
      }
      assert( wr2 < how_many_verts );

      *index_ptr++ = wr0;
      *index_ptr++ = wr1;
      *index_ptr++ = wr2;
    #if defined(TARGET_MKS)
      }
    #endif
    }
    else
      break;    // we've sorted faces by detail so we know we're done:  but it didn't help
#if defined(TARGET_MKS)
front_clipped:;
#endif

  }

  return index_ptr;
}


////////////////////////////////////////////////////////////////////////////////
// Assembly function prototypes for pmeshasm.src

#if defined(TARGET_MKS)
extern "C" {

void asm_xvertcopy(hw_rasta_vert* dest,hw_rasta_vert* src,int count);
void asm_xform_uni_alpha(hw_rasta_vert* vert_list, int count, matrix4x4* xform, hw_rasta_vert* vert_out, unsigned char universal_alpha);
void asm_xform_offset_into_diffuse(hw_rasta_vert* vert_list, int count, matrix4x4* xform, hw_rasta_vert* vert_out, unsigned char universal_alpha);

void asm_xform_vertex_alpha(hw_rasta_vert* vert_list, int count, matrix4x4* xform, hw_rasta_vert* vert_out );
void asm_xform_skin(hw_rasta_vert* vert_list, int count, matrix4x4 xforms[], hw_rasta_vert* vert_out );

bool asm_clip_verts( hw_rasta_vert* tvlit,
                     int vertex_count,
                     float near_plane_d,
                     float far_plane_d,
                     vector3d* xformed_facing_camera,

                     plane* plane_table,
                     vector3d* camera_origin,
                     int* all_clipped );             // returns whether some_front_clipped

unsigned short* asm_skin_tri_setup_prog( vr_pmesh* me, int start,int end, hw_rasta_vert* vert_begin, unsigned short* index_pool, int level_of_detail );

void flush_cache();
};
#endif

////////////////////////////////////////////////////////////////////////////////
//  optimization tool, because algebra.h ain't inlining
////////////////////////////////////////////////////////////////////////////////
// well this isn't exactly optimal either, especially on the Dreamcast, because
// it takes absolutely no advantage of the SH4 ftrv instruction! --Sean
////////////////////////////////////////////////////////////////////////////////
// the Dreamcast does its transforming in assembly, using ftrv.  This is an
// optimization tool for certain exceptions.  We do what we can.  -Jamie

#define RHW_XFORM( sv, mat, dv, rhw )  \
  rational_t w = mat[0][3]*sv.x+mat[1][3]*sv.y+mat[2][3]*sv.z+mat[3][3];  \
  rhw = 1/w; \
  dv.x  = mat[0][0]*sv.x+mat[1][0]*sv.y+mat[2][0]*sv.z+mat[3][0];  \
  dv.y  = mat[0][1]*sv.x+mat[1][1]*sv.y+mat[2][1]*sv.z+mat[3][1];  \
  dv.z  = mat[0][2]*sv.x+mat[1][2]*sv.y+mat[2][2]*sv.z+mat[3][2];

#define XFORM( sv, mat, dv ) \
  dv.x  = mat[0][0]*sv.x+mat[1][0]*sv.y+mat[2][0]*sv.z+mat[3][0];  \
  dv.y  = mat[0][1]*sv.x+mat[1][1]*sv.y+mat[2][1]*sv.z+mat[3][1];  \
  dv.z  = mat[0][2]*sv.x+mat[1][2]*sv.y+mat[2][2]*sv.z+mat[3][2];

#define XFORM_VERT( sv, mat, dv ) \
  dv.x  = mat[0][0]*sv.xyz.x+mat[1][0]*sv.xyz.y+mat[2][0]*sv.xyz.z+mat[3][0];  \
  dv.y  = mat[0][1]*sv.xyz.x+mat[1][1]*sv.xyz.y+mat[2][1]*sv.xyz.z+mat[3][1];  \
  dv.z  = mat[0][2]*sv.xyz.x+mat[1][2]*sv.xyz.y+mat[2][2]*sv.xyz.z+mat[3][2];

#define SO3_XFORM( sv, mat, dv ) \
  dv.x  = mat[0][0]*sv.x+mat[1][0]*sv.y+mat[2][0]*sv.z;  \
  dv.y  = mat[0][1]*sv.x+mat[1][1]*sv.y+mat[2][1]*sv.z;  \
  dv.z  = mat[0][2]*sv.x+mat[1][2]*sv.y+mat[2][2]*sv.z;



////////////////////////////////////////////////////////////////////////////////
// globals

////////////////////////////////////////////////////////////////////////////////
instance_bank<vr_pmesh> vr_pmesh_bank;


////////////////////////////////////////////////////////////////////////////////
// statics

////////////////////////////////////////////////////////////////////////////////
geometry_manager* vr_pmesh::l_geometry_pipe = NULL;

#if defined(TARGET_XBOX)

vert_lit_buf vert_workspace;
vert_lit_buf vert_workspace_small;
vert_lit_buf vert_workspace_quad;
vert_buf_xformed vert_workspace_xformed;
vert_buf_xformed vert_workspace_xformed_small;
vert_buf_xformed vert_workspace_xformed_quad;


#else

vert_buf vert_workspace;
#if defined (TARGET_PC)
vert_buf vert_workspace_small;
vert_buf vert_workspace_quad;
vert_buf_xformed vert_workspace_xformed;
vert_buf_xformed vert_workspace_xformed_small;
vert_buf_xformed vert_workspace_xformed_quad;
#elif defined (TARGET_PS2)
vert_buf vert_workspace_quad;
vert_buf_xformed vert_workspace_xformed;
vert_buf_xformed vert_workspace_xformed_quad;

#endif

#endif /* XBOX_USE_PMESH_STUFF JIV DEBUG */

unsigned short vert_utility_buffer[MAX_VERTS_PER_PRIMITIVE];


pmesh_normal* normal_pool;  // dynamic so we can get 32-byte alignment



////////////////////////////////////////////////////////////////////////////////


void xform_and_light_skin( hw_rasta_vert* vert_list_begin,
                           hw_rasta_vert* vert_list_end,  // must be a pointer into vert_workspace
                           bool light,
                           matrix4x4* bones_proj,   // xforms for projection
                           const matrix4x4* bones_world,
                           short* wedge_lod_starts,
                           int level_of_detail,

                           use_light_context *lites
                            ); // xforms for lights


#ifndef TARGET_PC
// on PC we're letting D3D do the xforms

static matrix4x4 total_xform; // ugly nasty global
#endif



////////////////////////////////////////////////////////////////////////////////
//  static lighting info
////////////////////////////////////////////////////////////////////////////////


vector3d bone_lights[MAX_BONES][ABSOLUTE_MAX_LIGHTS];

// permanent
matrix4x4* bones_proj;

////////////////////////////////////////////////////////////////////////////////
void vr_pmesh::init_pmesh_system( void )
{
}

////////////////////////////////////////////////////////////////////////////////
void vr_pmesh::kill_pmesh_system( void )
{
#if defined (TARGET_PC) || XBOX_USE_PMESH_STUFF
  vert_workspace.deconstruct();
  vert_workspace_small.deconstruct();
  vert_workspace_quad.deconstruct();
  vert_workspace_xformed.deconstruct();
  vert_workspace_xformed_small.deconstruct();
  vert_workspace_xformed_quad.deconstruct();
  vert_workspace_quad.deconstruct();

  vert_workspace_xformed.deconstruct();
  vert_workspace_xformed_quad.deconstruct();
#endif
}

////////////////////////////////////////////////////////////////////////////////
render_flavor_t vr_pmesh::render_passes_needed() const
{
  render_flavor_t passes=0;
  if (has_translucent_verts)
    return RENDER_TRANSLUCENT_PORTION; // do it all in translucent pass
    //passes |= RENDER_TRANSLUCENT_PORTION;
  for (int i=get_num_materials(); --i>=0; )
  {
    const material* mptr = get_material_ptr(i);
    if ( mptr->is_translucent() )
      passes |= RENDER_TRANSLUCENT_PORTION;
    else
      passes |= RENDER_OPAQUE_PORTION;
    if(mptr->has_environment_map())
      passes |= RENDER_TRANSLUCENT_PORTION;

    for(int x=1; x<MAPS_PER_MATERIAL; ++x)
    {
      if(mptr->has_diffuse_map(x))
        passes |= RENDER_TRANSLUCENT_PORTION;
    }
  }
  return passes;
}



////////////////////////////////////////////////////////////////////////////////
// vr_pmesh render function
////////////////////////////////////////////////////////////////////////////////



bool g_i_am_an_actor = false;
bool g_environment_maps_enabled = true;
bool g_decal_maps_enabled = true;
bool g_detail_maps_enabled = true;

material* g_material_debug_mask = NULL;


int g_start_render = 1;
int g_end_render   = INT_MAX;

int g_num_rendered = 0;
int skip_pass = 0;

void vr_pmesh::render_instance( render_flavor_t render_flavor, instance_render_info* iri, short *ifl_lookup )
{
	PANIC;
}



int vr_pmesh::calculate_integral_detail_level(instance_render_info* iri)
{
  // calculate detail level
  assert( iri->get_target_face_count() <= get_max_faces());
  int temp = max( (int)iri->get_target_face_count(),get_min_faces());
  float target_face_count = temp;
  int level_of_detail = (int)iri->get_target_face_count();
  if(get_max_faces()!=get_min_faces())
  {
    float target_detail = (target_face_count - get_min_faces())/
                          (get_max_faces() - get_min_faces());
    target_detail *= get_max_detail();
    level_of_detail = (int)target_detail;
  }
  else
    level_of_detail = 0;
  return level_of_detail;
}

class face_info
{
public:
  rational_t min_z2;
  rational_t max_z2;
  const face* my_face;

  face_info() {}
  face_info( rational_t _min_z2, rational_t _max_z2, const face* _my_face )
    : min_z2(_min_z2),
      max_z2(_max_z2),
      my_face(_my_face)

  {}

  bool operator<(const face_info& fi) const
  {
    return min_z2 < fi.min_z2;
  }
};

//-----------------------------------------------------------------------------------------

geometry_manager* geometry_manager_inst;

void vr_pmesh::render_material_clipped_full_detail(
      instance_render_info* iri,
      material_ref material_idx,
      material_map::iterator mi,
      color32 color_scale32,
      bool render_diffuse_map,
      bool render_extra_map[MAPS_PER_MATERIAL],
      bool render_environment_map )
{
#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF
	std::vector<material*>* r_materials;

  if( iri->alt_materials ) r_materials = iri->alt_materials;
  else r_materials = &materials;

  material* mat = (*r_materials)[material_idx];

  XDEBUG_MSG("vr_pmesh::render_material");

  unsigned short* indices_it = vert_utility_buffer;

  START_PROF_TIMER( proftimer_rgn_trisetup );
  int i;

  int end_face;
  material_map::iterator next = mi;
  ++next;
  if ( next == material_changes.end() )
    end_face = get_max_faces();

  else
    end_face = (*next).second;
  int num_indices;
  wedge_ref* start_index;
  // iterate through faces until reaching end of current material
  if( wedge_index_list == NULL )
  {
    for ( i=(*mi).second; i<end_face; ++i )

    {
      const face* my_face = &faces[i];


      // a backface cull here might be nice, but you can only do it for
      // tris who don't cross the front clip plane
      //clockwise backface exclusion
      int xv0,xv1,xv2;
      xv0 = my_face->wedge_refs[0];

      xv1 = my_face->wedge_refs[1];
      xv2 = my_face->wedge_refs[2];
      *indices_it++ = xv0;
      *indices_it++ = xv1;
      *indices_it++ = xv2;
    }

    // Avoiding MKS l_divs...
    assert (sizeof(*indices_it)==2);
    num_indices = ( ((char *) indices_it) - ((char *) vert_utility_buffer) ) >> 1;
    start_index = vert_utility_buffer;
  }
  else
  {
    int start_face = (*mi).second;
    start_index = wedge_index_list + start_face*3;
    num_indices = (end_face - start_face)*3;
  }
  STOP_PROF_TIMER( proftimer_rgn_trisetup );

  START_PROF_TIMER( proftimer_rgn_draw );
  if(num_indices)
  {

    int frame = 0;

    unsigned override_color = (color_scale32.to_ulong() != 0xffffffff)?FORCE_COLOR_PARAMETER:0;
    if(render_diffuse_map)
    {
      frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_DIFFUSE));
      mat->send_context( frame, MAP_DIFFUSE, iri->get_force_flags()|override_color, color_scale32 );


      hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
          start_index, num_indices, hw_rasta::SEND_VERT_FRONT_CLIP );

    }

    if( render_extra_map[MAP_DETAIL] )
    {
      frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_DETAIL));
      prepare_for_extra_diffuse_pass(mat, MAP_DETAIL);  // this prepares in vert_workspace


      START_PROF_TIMER( proftimer_instance_sendctx );
      mat->send_context( frame, MAP_DETAIL, iri->get_force_flags()|override_color, color_scale32 );

      STOP_PROF_TIMER( proftimer_instance_sendctx );

      START_PROF_TIMER( proftimer_instance_draw );
      hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
          start_index, num_indices, hw_rasta::SEND_VERT_FRONT_CLIP );
      STOP_PROF_TIMER( proftimer_instance_draw );
    }

    if(render_environment_map)
    {
      frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_ENVIRONMENT));
      prepare_for_environment_pass(mat);
      mat->send_context( frame, MAP_ENVIRONMENT, iri->get_force_flags()|override_color, color_scale32 );

      hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
          start_index, num_indices, hw_rasta::SEND_VERT_FRONT_CLIP );

    }

    if( render_extra_map[MAP_DIFFUSE2] )
    {
      frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_DIFFUSE2));
      prepare_for_extra_diffuse_pass(mat, MAP_DIFFUSE2);  // this prepares in vert_workspace


      START_PROF_TIMER( proftimer_instance_sendctx );
      mat->send_context( frame, MAP_DIFFUSE2, iri->get_force_flags()|override_color, color_scale32 );
      STOP_PROF_TIMER( proftimer_instance_sendctx );


      START_PROF_TIMER( proftimer_instance_draw );
      hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
          start_index, num_indices, hw_rasta::SEND_VERT_FRONT_CLIP );
      STOP_PROF_TIMER( proftimer_instance_draw );
    }

    if( render_environment_map || render_extra_map[MAP_DIFFUSE2] )
      undo_uv_damage();  // possible optimization:  you only need to do this if you're drawing
                         // more materials



    XDEBUG_MSG("vr_pmesh::render_material return");
  }
  STOP_PROF_TIMER( proftimer_rgn_draw );

#endif // TARGET_PC

}


void vr_pmesh::prepare_for_environment_pass(material *mat)
{
	PANIC;
}

void vr_pmesh::undo_uv_damage()
{
	PANIC;
}


void vr_pmesh::undo_skin_uv_damage()
{
	PANIC;
}

void vr_pmesh::prepare_for_extra_diffuse_pass(material *mat, int _map)
{
	PANIC;
}

//#pragma fixme( "how come I can't un-const bones_world without breaking the renderer?" )
po bones_world[MAX_BONES];


void vr_pmesh::render_skin( render_flavor_t render_flavor,
                         const instance_render_info* iri,
                         const po* _bones_world,
                         int num_bones )
{
#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF
  memcpy( bones_world, _bones_world, num_bones*sizeof(po) );
  // exit early if we're forcing everything translucent and this isn't the translucent pass

  bool pass_xluc = (render_flavor & RENDER_TRANSLUCENT_PORTION)!=0;
  if (iri->force_translucent() && !pass_xluc)

  {
    assert(false);
    return;
  }

  START_PROF_TIMER( proftimer_render_skin );

  std::vector<material*>   *r_materials;

  if( iri->alt_materials ) r_materials = iri->alt_materials;

  else r_materials = &materials;


  // transform the directional lights into view space.

  XDEBUG_MSG("pmesh::render");


#ifdef PROFILING_ON
  float detail_diff = get_max_faces() - iri->get_target_face_count();

  if(!(render_flavor & RENDER_TRANSLUCENT_PORTION))

  {
    ADD_PROF_COUNT( profcounter_lod_tri_estimate, detail_diff );
  }
#endif

  // we are letting D3D do the xforms now, but skins are going to be in world space
  // after we are done with them.  If we could get them into camera space it'd save
  // D3D from doing some work.  Just set this to exactly the inverse of the world_to_view
  // matrix and mix the world_to_view matrix in with the bone matrices.
  geometry_manager::inst()->set_local_to_world( identity_matrix );

  // calculate detail level

  assert( iri->get_target_face_count() <= get_max_faces());
  assert( get_min_faces() <= get_max_faces());
  int temp = max( (int)iri->get_target_face_count(),get_min_faces());
  float target_face_count = temp;
  int level_of_detail = get_max_detail();
  float detail_fraction = 0;


  if(get_max_faces()!=get_min_faces())
  {
    float target_detail = (target_face_count - get_min_faces())/
                          (get_max_faces() - get_min_faces());
    target_detail *= get_max_detail();
    level_of_detail = (int)target_detail;
    detail_fraction = target_detail - level_of_detail;
  }
  // else it's a simple mesh, highest detail = 0


  // prepare bones:  a bone starts out going from bone-local to waist-local
  // (in case of the waist, it's identity).
  // but the xform we need is waist-local (unanimated) to waist-local (animated)
  // so we need to start with waist-local to bone-local
  // which we can find using the inverse of the bone's pivot
  int i;
  int bwsize = num_bones;
  for(i=0;i<bwsize;++i)
  {
    po pivot_po = get_bone_pivot(i);

    matrix4x4 pivot_xform = pivot_po.get_matrix();
    matrix4x4 pivot_xform_invert = pivot_xform.inverse();
    bones_proj[i] = pivot_xform_invert * ((matrix4x4 *)bones_world)[i];
    ((po*)&bones_world[i])->invert(); // for lights.  FIXME:  doesn't take into account NEW skin paradigm
    *((po*)&bones_world[i]) = bones_world[i].get_matrix() * pivot_xform;
  }

  bool light=(render_flavor & RENDER_NO_LIGHTS)==0 &&
             (iri->get_force_flags()&FORCE_NO_LIGHT)==0;

  // we need to light vertices for each material independently instead!
  if (light)
    for (int i=get_num_materials(); --i>=0; )
    {
      if ( get_material_ptr( i )->is_full_self_illum() )
      {
        light=false;
        break;
      }
    }
  if (iri->get_force_flags()&FORCE_LIGHT)
    light=true;

  // transform lights to bone space
  const vector3d& center_pos=iri->get_local_to_world().get_position(); // center of object
  light_manager* light_set = iri->get_light_set();
  if (!light || !light_set)
  {
    light_set = light_manager::get_static_light_set();
  }
  // This object is only updated by light_manager::prepare_for_rendering
  use_light_context lites;
  light_set->prepare_for_rendering(&lites);

  lites.transform_lights_to_bone_space(center_pos, (matrix4x4 *)bones_world, num_bones);


  how_many_verts = xverts_for_lod[level_of_detail];
#ifdef _DEBUG
	if( filename == "kravenkraven" ) {
		//DebugBreak( );
	}
#endif

  hw_rasta_vert* workspace_end = xverts + how_many_verts;

  // prepare xverts to see if they're in the model for this level of detail
  // we don't need to mask and or if no wedge_lod_starts,
  // because CLIP_NOTINLOD will never get set.

  vert_workspace.lock(how_many_verts);

  xform_and_light_skin( xverts,
                        workspace_end,
                        light, // do lighting?
                        bones_proj,
                        (matrix4x4 *)bones_world,
                        wedge_lod_starts,
                        level_of_detail,
                        &lites );


  vert_workspace.unlock();

  material_map::iterator mi;

  color32 color_scale32 = iri->get_color_scale();


  for ( mi=material_changes.begin(); mi!=material_changes.end(); ++mi)
  {
    int material_idx = (*mi).first;
    assert(material_idx>=0);

    // in case the material set doesn't match the mesh (can happen with bad variant data)
    if ( material_idx >= (int)r_materials->size() )
      material_idx = 0;

    if (material_idx != INVISIBLE_MATERIAL_REF)

    {
      const material* curmat=(*r_materials)[material_idx];
      if (!curmat)
        continue;


      bool render_diffuse_map;
      bool render_environment_map;
      bool render_decal_map = false;
      bool render_detail_map = false;
      bool is_xluc = color_scale32.c.a<255 || iri->force_translucent() ||
                     curmat->is_translucent() || has_translucent_verts;
      render_diffuse_map = is_xluc == pass_xluc;
      render_environment_map = /*pass_xluc &&*/ g_environment_maps_enabled;

      render_decal_map = /*pass_xluc &&*/ g_decal_maps_enabled && curmat->has_diffuse_map(MAP_DIFFUSE2);
      render_detail_map = /*pass_xluc &&*/  g_detail_maps_enabled && curmat->has_diffuse_map(MAP_DETAIL);
      if(!curmat->has_environment_map())
        render_environment_map = false;


      START_PROF_TIMER( proftimer_skin_trisetup );

      XDEBUG_MSG("vr_pmesh::render_material");

      // iterate through faces until reaching end of current material
      int end;
      material_map::iterator next = mi;
      ++next;
      if ( next == material_changes.end() )
        end = get_max_faces();
      else
        end = (*next).second;
      unsigned short* index_ptr = vert_utility_buffer;

      vert_workspace.lock(-1); // don't clear! (OW! I want to get rid of this lock, it's very painful)
      hw_rasta_vert_lit* vert_workspace_begin = (hw_rasta_vert_lit*)vert_workspace.begin();

      // Special case the non-progressive case
      wedge_ref* index_start = vert_utility_buffer;
      int num_indices;
      if( wedge_index_list )
      {
        index_start = wedge_index_list + (*mi).second*3;
        num_indices = (end - (*mi).second)*3;
      }
      else
      {
	      if (level_of_detail>=get_max_detail() )
	      {
	        index_ptr = c_skin_tri_setup_nonprog( faces+(*mi).second,
	                                    end - (*mi).second,
	                                    vert_workspace_begin,
	                                    index_ptr );
	      }
	      else
	      {
	      #if defined(TARGET_MKS)
	        index_ptr = asm_skin_tri_setup_prog( this,
	                                  (*mi).second,
	                                  end,
	                                  vert_workspace_begin,
	                                  index_ptr,
	                                  level_of_detail );

	      #else

          index_ptr = c_skin_tri_setup_prog( this,
	                                (*mi).second,
	                                end,
	                                vert_workspace_begin,
	                                index_ptr,
	                                level_of_detail );
	      #endif
	      }
	      // Avoiding MKS l_divs...
	      // num_indices = index_ptr - index_start;
	      assert (sizeof(*index_ptr)==2);
	      num_indices = ( ((char *) index_ptr) - ((char *) index_start) ) >> 1;
	    }

      STOP_PROF_TIMER( proftimer_skin_trisetup );


      vert_workspace.unlock();

      START_PROF_TIMER( proftimer_skin_draw );

      if(num_indices)
      {

        int frame = 0;
        if(render_diffuse_map)
        {
          material* mat = (*r_materials)[material_idx];
          unsigned override_color = (color_scale32.to_ulong() != 0xffffffff)?FORCE_COLOR_PARAMETER:0;

          frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_DIFFUSE));
          mat->send_context( frame, MAP_DIFFUSE, iri->get_force_flags()|override_color, color_scale32 );
//          g_game_ptr->get_blank_material()->send_context( frame, MAP_DIFFUSE, iri->get_force_flags()|override_color, color_scale32 );
          hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
              index_start, num_indices,
              iri->get_skip_clip()?0:hw_rasta::SEND_VERT_FRONT_CLIP );

          if( !pass_xluc )
            hw_rasta::inst()->send_start( hw_rasta::PT_TRANS_POLYS );
          if( render_detail_map || render_environment_map || render_decal_map )
          {
            if( render_detail_map )
            {
              frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_DETAIL));
              prepare_for_extra_diffuse_pass(mat, MAP_DETAIL);  // this prepares in vert_workspace

              START_PROF_TIMER( proftimer_instance_sendctx );

              mat->send_context( frame, MAP_DETAIL, iri->get_force_flags()|override_color, color_scale32 );
              STOP_PROF_TIMER( proftimer_instance_sendctx );

              START_PROF_TIMER( proftimer_instance_draw );
              hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
                  index_start, num_indices,
                  iri->get_skip_clip()?0:hw_rasta::SEND_VERT_FRONT_CLIP );
              STOP_PROF_TIMER( proftimer_instance_draw );
            }

            if( render_environment_map )
            {
              frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_ENVIRONMENT));
              prepare_for_environment_pass(mat);  // this prepares in vert_workspace

              START_PROF_TIMER( proftimer_instance_sendctx );
              mat->send_context( frame, MAP_ENVIRONMENT, iri->get_force_flags()|override_color, color_scale32 );
              STOP_PROF_TIMER( proftimer_instance_sendctx );


              START_PROF_TIMER( proftimer_instance_draw );
              hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
                  index_start, num_indices,
                  iri->get_skip_clip()?0:hw_rasta::SEND_VERT_FRONT_CLIP );
              STOP_PROF_TIMER( proftimer_instance_draw );
            }

            if( render_decal_map )
            {
              frame = iri->time_to_frame_locked(mat->get_anim_length(MAP_DIFFUSE2));
              prepare_for_extra_diffuse_pass(mat, MAP_DIFFUSE2);  // this prepares in vert_workspace

              START_PROF_TIMER( proftimer_instance_sendctx );
              mat->send_context( frame, MAP_DIFFUSE2, iri->get_force_flags()|override_color, color_scale32 );
              STOP_PROF_TIMER( proftimer_instance_sendctx );

              START_PROF_TIMER( proftimer_instance_draw );

              hw_rasta::inst()->send_indexed_vertex_list(vert_workspace, get_num_wedges(),
                  index_start, num_indices,
                  iri->get_skip_clip()?0:hw_rasta::SEND_VERT_FRONT_CLIP );

              STOP_PROF_TIMER( proftimer_instance_draw );
            }

          }
          if( !pass_xluc )
            hw_rasta::inst()->send_start( hw_rasta::PT_OPAQUE_POLYS );
        }
      }

      STOP_PROF_TIMER( proftimer_skin_draw );
      XDEBUG_MSG("vr_pmesh::render_material return");
    }
  }

  XDEBUG_MSG("vr_pmesh::render return");
  if( os_developer_options::inst()->is_flagged( os_developer_options::FLAG_SHOW_NORMALS ) )
    render_skin_normals( iri, bones_world );
  STOP_PROF_TIMER( proftimer_render_skin );
#endif // TARGET_PC
}

void vr_pmesh::clip_xform_and_light( hw_rasta_vert* vert_list_begin,
                      hw_rasta_vert* vert_list_end,
                      unsigned cxl_flags,
                      int num_dir_lights,

                      int num_point_lights,

                      int alpha,
                      const po& clip_world2view,
                      const po& clip_local2view,
                      bool icon_render,

                      use_light_context *lites
                      )
{
	PANIC;
}


void xform_and_light_skin( hw_rasta_vert* vert_list_begin,
                           hw_rasta_vert* vert_list_end,  // must be a pointer into vert_workspace
                           bool light,
                           matrix4x4* bones_proj,   // xforms for projection
                           const matrix4x4* bone_spaces,  // xforms for lights
                           short* wedge_lod_starts,

                           int level_of_detail,
                           use_light_context *lites)
{
	PANIC;
}


void vr_pmesh::render_normals( const instance_render_info* iri )
{
}

void vr_pmesh::render_skin_normals( const instance_render_info* iri,
                              const po* bones_world )
{
	PANIC;
}


//#pragma fixme( "rewrite anim length system to understand diff't per-layer anim lengths. -mkv 4/6/01" )
// Compute the "animation length" of the materials on a pmesh
// by simply taking the maximum of all animations across all layers
// of all materials
int vr_pmesh::get_anim_length() const

{
	PANIC;
}


void vr_pmesh::clear_lighting()
{
#ifdef TARGET_MKS
  // MKS does its lighting in-place in the original mesh data
  // and so it needs cleared out to draw a non-lit version, otherwise it gets
  // overbrightened by the additional specular.  Can't we turn off the specular
  // render state in Kamui?
  hw_rasta_vert* wp = xverts;
  for(int i=num_wedges; --i>=0; ++wp)

  {
    wp->specular.i &= 0xFF000000;    // black
  }
#endif
}

void vr_pmesh::anim_uvs( time_value_t t )

{
	PANIC;
}


bool vr_pmesh::is_uv_animated() const
{
	std::vector<material*>::const_iterator mi;

  for ( mi=materials.begin(); mi!=materials.end(); ++mi )
  {
    if ( (*mi)->u_anim || (*mi)->v_anim )

      return true;
  }

  return false;
}


// we should change this so it uses a 256-entry lookup table instead of calling pow() 3 times per vertex!

void set_global_brightness( rational_t brightness )
{
  const rational_t  k = 1.0f / 255.0f;


  assert( brightness >= -1.0f && brightness <= 1.0f );
  if( brightness >= -.009f && brightness <= .009f ) return;
  if( brightness > .0f ) brightness = 1 - brightness;
  else brightness *= -1000.0f;
  for( instance_bank<vr_pmesh>::pref_set::iterator si = vr_pmesh_bank.refs_by_ptr.begin(), sie = vr_pmesh_bank.refs_by_ptr.end(); si != sie; ++si )
  {
    if( *si && (*si)->ptr )
    {
      hw_rasta_vert *xv = (*si)->ptr->xverts;
      for( int i = 0, j = (*si)->ptr->get_num_wedges(); i < j; ++i, ++xv )
      {
        if( brightness <= 1.0f )

        {
          xv->diffuse.set_red  ( (unsigned char)(255.0f * (rational_t)pow(k * (rational_t)xv->diffuse.get_red  (), brightness)) );

          xv->diffuse.set_green( (unsigned char)(255.0f * (rational_t)pow(k * (rational_t)xv->diffuse.get_green(), brightness)) );
          xv->diffuse.set_blue ( (unsigned char)(255.0f * (rational_t)pow(k * (rational_t)xv->diffuse.get_blue (), brightness)) );
        }
        else
        {
          xv->diffuse.set_red  ( (unsigned char)(255.0f / pow(brightness, 1.0f - k * (rational_t)xv->diffuse.get_red  ())) );
          xv->diffuse.set_green( (unsigned char)(255.0f / pow(brightness, 1.0f - k * (rational_t)xv->diffuse.get_green())) );
          xv->diffuse.set_blue ( (unsigned char)(255.0f / pow(brightness, 1.0f - k * (rational_t)xv->diffuse.get_blue ())) );
        }
      }
    }
  }
}


void undo_global_brightness_set( rational_t previous_brightness_value )
{
  const rational_t k = 1.0f / 255.0f;

  instance_bank<vr_pmesh>::pref_set::iterator si, sie;
  int i, j;


  assert( previous_brightness_value >= -1.0f && previous_brightness_value <= 1.0f );
  if( previous_brightness_value >= -.009f && previous_brightness_value <= .009f ) return;


  if( previous_brightness_value > .0f )
  {
    previous_brightness_value = 1.0f / ( 1.0f - previous_brightness_value );
    for( si = vr_pmesh_bank.refs_by_ptr.begin(), sie = vr_pmesh_bank.refs_by_ptr.end(); si != sie; ++si )

    {
      if( *si && (*si)->ptr )
      {
        hw_rasta_vert *xv = (*si)->ptr->xverts;
        for( i = 0, j = (*si)->ptr->get_num_wedges(); i < j; ++i, ++xv )
        {
          xv->diffuse.set_red  ( (unsigned char)(255.0f * (rational_t)pow(k * (rational_t)xv->diffuse.get_red  (), previous_brightness_value)) );
          xv->diffuse.set_green( (unsigned char)(255.0f * (rational_t)pow(k * (rational_t)xv->diffuse.get_green(), previous_brightness_value)) );
          xv->diffuse.set_blue ( (unsigned char)(255.0f * (rational_t)pow(k * (rational_t)xv->diffuse.get_blue (), previous_brightness_value)) );
        }

      }
    }
  }
  else
  {
    previous_brightness_value = -1000 * previous_brightness_value;
    for( si = vr_pmesh_bank.refs_by_ptr.begin(), sie = vr_pmesh_bank.refs_by_ptr.end(); si != sie; ++si )
    {
      if( *si && (*si)->ptr )
      {
        hw_rasta_vert *xv = (*si)->ptr->xverts;
        for( i = 0, j = (*si)->ptr->get_num_wedges(); i < j; ++i, ++xv )
        {
          xv->diffuse.set_red  ( (unsigned char)(255.0f * (1.0f - log(k * (rational_t)xv->diffuse.get_red  ()) / log(previous_brightness_value))) );
          xv->diffuse.set_green( (unsigned char)(255.0f * (1.0f - log(k * (rational_t)xv->diffuse.get_green()) / log(previous_brightness_value))) );
          xv->diffuse.set_blue ( (unsigned char)(255.0f * (1.0f - log(k * (rational_t)xv->diffuse.get_blue ()) / log(previous_brightness_value))) );
        }
      }
    }
  }
}


inline unsigned char multiply_clip_color( unsigned char col, rational_t factor )
{
  rational_t frv = (rational_t)col * factor;
  if( frv > 255 ) frv = 255;
  return (unsigned char)frv;

}



void set_global_linear_brightness( rational_t brightness )
{
#define BRIGHTNESS_SCALE_FACTOR 2.0f

  assert( brightness >= -1.0f && brightness <= 1.0f );
  if( brightness < .0f ) brightness += 1.0f;
  else if( brightness > .0f ) brightness = BRIGHTNESS_SCALE_FACTOR * ( brightness + 1.0f );
  else return;
  for( instance_bank<vr_pmesh>::pref_set::iterator si = vr_pmesh_bank.refs_by_ptr.begin(), sie = vr_pmesh_bank.refs_by_ptr.end(); si != sie; ++si )
  {
    if( *si && (*si)->ptr )
    {
      hw_rasta_vert *xv = (*si)->ptr->xverts;
      for( int i = 0, j = (*si)->ptr->get_num_wedges(); i < j; ++i, ++xv )
      {
        xv->diffuse.set_red  ( multiply_clip_color(xv->diffuse.get_red  (), brightness) );
        xv->diffuse.set_green( multiply_clip_color(xv->diffuse.get_green(), brightness) );
        xv->diffuse.set_blue ( multiply_clip_color(xv->diffuse.get_blue (), brightness) );
      }
    }
  }
}

#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF
unsigned vr_pmesh::optdisallow = 0;
#endif

////////////////////////////////////////////////////////////////////////////////
// statics
////////////////////////////////////////////////////////////////////////////////
extern pmesh_normal *normal_pool;

#if defined (TARGET_PC) 
//extern vert_buf non_indexed_workspace;
#endif

//extern instance_render_info* viri;
extern matrix4x4* bones_proj;


const int MAX_N_TANGLE_FACES  = 100;
const int MAX_N_TANGLE_VERTS  = MAX_N_TANGLE_FACES*3;
const int MAX_N_TANGLE_WEDGE_REFS = MAX_N_TANGLE_FACES*3;
int * global_n_tangle_xverts_for_lod;
wedge * global_n_tangle_wedges;
hw_rasta_vert * global_n_tangle_rasta_verts;
vert_ref * global_n_tangle_vert_refs;
face * global_n_tangle_faces;

wedge_ref * global_n_tangle_wedge_refs;

reduced_face * global_n_tangle_reduced_faces;

// Big ol' hack to avoid parameter passing for the moment.
bool making_n_tangle = false;

enum { MAX_ATTENUATION=16384 };

////////////////////////////////////////////////////////////////////////////////
// vr_pmesh
////////////////////////////////////////////////////////////////////////////////

void initialize_mesh_stuff()
{
//  int sub_before_1=0, sub_after_1=0, sub_before_2=0, sub_after_2=0;

#if defined (TARGET_PC) || XBOX_USE_PMESH_STUFF
  normal_pool = (pmesh_normal*)os_malloc( MAX_VERTS_PER_PRIMITIVE * sizeof(pmesh_normal) );
#else
  normal_pool = NULL;

#endif

  bones_proj = (matrix4x4*)os_malloc( MAX_BONES * sizeof(matrix4x4) );


//  int temp1 = MAX_VERTS_PER_PRIMITIVE;
//  int temp2 = VIRTUAL_MAX_VERTS_PER_PRIMITIVE;
  // account for vertex and
//  int offset =  temp1 * ( sizeof( hw_rasta_vert ) + sizeof(vert_normal) )
//            -   temp2 * ( sizeof( hw_rasta_vert ) + sizeof(vert_normal) );
//P  memtrack::set_total_alloced_offset( offset  + sub_after_1-sub_before_1 + sub_after_2-sub_before_2);

  // the "Default" context distinguishes game allocations from overhead;
  // in this, it serves the same purpose as the total_alloced_offset value
//P  g_memory_context.push_context( "Default" );


  global_n_tangle_wedges = (wedge*)os_malloc( MAX_N_TANGLE_VERTS*sizeof(wedge) );
  global_n_tangle_rasta_verts = (hw_rasta_vert*)os_malloc( MAX_N_TANGLE_VERTS*sizeof(hw_rasta_vert) );
  global_n_tangle_vert_refs = (vert_ref*)os_malloc( MAX_N_TANGLE_VERTS*sizeof(vert_ref) );
  global_n_tangle_faces = (face*)os_malloc( MAX_N_TANGLE_FACES*sizeof(face) );
  global_n_tangle_wedge_refs = (wedge_ref*)os_malloc( MAX_N_TANGLE_WEDGE_REFS*sizeof(wedge_ref) );
  global_n_tangle_reduced_faces = (reduced_face*)os_malloc( MAX_N_TANGLE_FACES*sizeof(reduced_face) );

  global_n_tangle_xverts_for_lod = (int*)os_malloc( (MAX_N_TANGLE_VERTS+1)*sizeof(int) );
}

////////////////////////////////////////////////////////////////////////////////
// vr_pmesh
////////////////////////////////////////////////////////////////////////////////
vr_pmesh::vr_pmesh( unsigned _mesh_flags )
         : visual_rep(VISREP_PMESH)
         , min_faces(0)
         , max_detail(0)
{
  has_translucent_verts = false;
  mesh_flags = _mesh_flags | LIT_LAST_FRAME;
  xverts = NULL;
  uvanim_update_frame = 0;
  wedges = NULL;
  vert_refs_for_wedge_ref = NULL;
  wedge_lod_starts = NULL;
  original_face_for_face_slot = NULL;
  faces = NULL;
  reduced_faces = NULL;
  wedge_index_list = NULL;
  num_faces = 0;
  num_wedges = 0;
  pivot = ZEROVEC;
  pivot_valid = false;

  xverts_for_lod = NULL;

  materials.resize(0);
  verts = NEW std::vector<vert>;
#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF
  optimized_verts = NULL;
#endif

}


vr_pmesh::vr_pmesh(const char * pmesh_filename, unsigned _mesh_flags ) : visual_rep( VISREP_PMESH )
{
#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF
  optimized_verts = NULL;
#endif
  _construct(stringx(pmesh_filename), _mesh_flags);
}


vr_pmesh::~vr_pmesh()
{
	PANIC;
}

///////////////////////////////////////////////////////////////////////////////
// NEWENT File I/O
///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
int g_debug_thingy;
#endif

vr_pmesh::vr_pmesh( chunk_file& fs, unsigned _mesh_flags )
:   visual_rep( VISREP_PMESH )
{
  has_translucent_verts = false;
  progressive = false;
  mesh_flags = _mesh_flags | LIT_LAST_FRAME;
  verts = NULL;
  xverts = NULL;
  uvanim_update_frame = 0;
  wedges = NULL;
  faces = NULL;
  wedge_index_list = NULL;
  reduced_faces = NULL;
  vert_refs_for_wedge_ref = NULL;
  wedge_lod_starts = NULL;
  original_face_for_face_slot = NULL;
#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF

  optimized_verts = NULL;
#endif
  filename = fs.get_filename();
#ifdef _DEBUG
  if( filename == stringx("spidermanspidey") )
  {
    g_debug_thingy++;
  }

#endif
  load( fs );
}


void vr_pmesh::load( chunk_file& fs )
{
  pivot = ZEROVEC;
  pivot_valid = false;
  internal_serial_in( fs );
	debug_print( "loaded mesh '%s' has v/w/f/m %d/%d/%d/%d", filename.c_str( ), verts->size( ), num_wedges, num_faces, materials.size( ) );
}

void serial_in( chunk_file& fs, vr_pmesh* mesh )
{
  mesh->internal_serial_in( fs );
}

void vr_pmesh::internal_serial_in( chunk_file& fs )
{
	PANIC;
}




bool vr_pmesh::read_stuff1(chunk_file& fs, chunk_flavor flavor_flav, bool & no_warnings, int & vert_count, int & svert_count, int & morph_wedge_count)
{
	PANIC;
	return true;
}


class lod_info
{
public:
  float ranges[4];
  stringx names[4];

  lod_info( void )
  {

    for( int i = 0; i < 3; i++ ) {
      ranges[i] = 0.0f;
    }

  }
};

static const chunk_flavor CHUNK_LOD( "lod" );


void serial_in( chunk_file& fs, lod_info* lod )
{
  chunk_flavor cf;
  int level;
  float range;
  stringx name;

  for( serial_in( fs, &cf ); cf != CHUNK_END; serial_in( fs, &cf ) ) {

    if( cf == chunk_flavor( "level" ) ) {
      serial_in( fs, &level );
    } else if( cf == chunk_flavor( "range" ) ) {
      serial_in( fs, &range );
    } else if( cf == chunk_flavor( "name" ) ) {
      serial_in( fs, &name );
    } else {
      error( "unknown chunk parsing lod tag" );
    }

  }


  lod->ranges[level] = range;
  lod->names[level] = name;
}

bool vr_pmesh::read_stuff2(chunk_file& fs, chunk_flavor flavor_flav, int & last_wedges_lod, int & wedge_count, int & face_count, const stringx& texture_dir, int & vsplit_count)
{
	PANIC;
	return false;
}



void vr_pmesh::_construct(const stringx& pmesh_filename, unsigned _mesh_flags )
{
  has_translucent_verts = false;
  mesh_flags = _mesh_flags | LIT_LAST_FRAME;

  // <<<< most of the asserts in this file should give warnings about
  //      where file is corrupt to user, that is if this remains a text file
  //      that people touch
  verts = NULL;

  xverts = NULL;
  uvanim_update_frame = 0;
  wedges = NULL;
  vert_refs_for_wedge_ref = NULL;
  wedge_lod_starts = NULL;
  original_face_for_face_slot = NULL;
  faces = NULL;
  wedge_index_list = NULL;
  reduced_faces = NULL;
  pivot = ZEROVEC;
  pivot_valid = false;
  xverts_for_lod = NULL;

  chunk_file fs;

  // use ".txtmesh" extension no matter what
  stringx adjusted_pmesh_filename;
  stringx::size_type dot_pos = pmesh_filename.find( 0, '.' );
  if( dot_pos != stringx::npos )
    adjusted_pmesh_filename = pmesh_filename.substr( 0, dot_pos );

  else
    adjusted_pmesh_filename = pmesh_filename;
  adjusted_pmesh_filename += ".txtmesh";

  fs.open(adjusted_pmesh_filename);
  internal_serial_in( fs );
  filename = pmesh_filename;
}


// construct a pmesh consisting of a simple one-sided rectangle
void vr_pmesh::make_rectangle()

{
	PANIC;
}

// construct a pmesh consisting of a simple two-sided triangle
void vr_pmesh::make_double_sided_triangle()
{
	PANIC;
}



// construct a pmesh consisting of a simple one-sided rectangle


void vr_pmesh::make_n_tangle(int n)

{
	PANIC;
}


void vr_pmesh::compute_info()
{
  int i;
  bounding_box box;

  //float radius2 = 0.0f;

  for(i = get_num_wedges(); --i>=0; )
  {
    const vector3d& p = xverts[i].xyz;

    assert(p.is_valid());
    box.accumulate(p);
    //radius2 = max(p.length2(), radius2);
  }
  //radius = __fsqrt(radius2);
  center = box.center();
  assert(center.is_valid());
  float radius_rel_center2 = 0.0f;
  for(i = get_num_wedges(); --i>=0; )

  {
    vector3d delta = xverts[i].xyz - center;
    radius_rel_center2 = max( delta.length2(), radius_rel_center2 );
  }
  radius = __fsqrt(radius_rel_center2);
}


// compute visual center and radius for skin
void vr_pmesh::compute_info( po* bones, int num_bones )

{
  int i;


  // compute visual center

  bounding_box box;
  for ( i=get_num_wedges(); --i>=0; )
  {
    const hw_rasta_vert& v = xverts[i];
    assert( v.xyz.is_valid() && v.boneid()<num_bones );
    vector3d p = bones[v.boneid()].fast_8byte_xform( v.xyz );
    box.accumulate( p );
  }
  center = box.center();
  assert( center.is_valid() );

  float radius_rel_center2 = 0.0f;
  for ( i=get_num_wedges(); --i>=0; )
  {
    const hw_rasta_vert& v = xverts[i];
    vector3d p = bones[v.boneid()].fast_8byte_xform( v.xyz );
    p -= center;
    radius_rel_center2 = max( p.length2(), radius_rel_center2 );
  }
  radius = __fsqrt( radius_rel_center2 );
}


rational_t vr_pmesh::compute_xz_radius_rel_center( const po& xform )
{

#if defined(TARGET_PC)  || XBOX_USE_PMESH_STUFF
  if (optimized_verts)

    return radius * xform.get_scale();
#endif

  rational_t rad2 = 0.0f;
  int i;
  vector3d ctr = xform.non_affine_slow_xform( center );
  for ( i=get_num_wedges(); --i>=0; )

  {
    vector3d v = xform.non_affine_slow_xform( xverts[i].xyz );
    v -= ctr;
    rad2 = max( v.xz_length2(), rad2 );
  }
  return __fsqrt(rad2);
}


#ifdef DEBUG
static vr_pmesh* fmc_this;
#endif

//int face_material_compare(const void *v1, const void *v2);
static int face_material_compare(const void *v1, const void *v2)
{
  face *f1 = (face *)v1, *f2 = (face *)v2;
  if( f1->get_material_ref()==f2->get_material_ref())
  {
    return f1->level_of_detail - f2->level_of_detail;
  }
  return f1->get_material_ref() - f2->get_material_ref();
}


// move this step into exporters to decrease load time...or implement Wade's evil heap load
void vr_pmesh::optimize()
{
	PANIC;
}


void vr_pmesh::mark_self_lit_verts()
{
  if(!materials.empty())
  {
    int i;
    for( i=get_num_wedges(); --i>=0; )
      xverts[i].clip_flags &= NORMAL_MASK;
  }
}

#if defined(TARGET_PC) || XBOX_USE_PMESH_STUFF
void vr_pmesh::optimize_static_mesh_for_d3d()
{
	PANIC;
}
#endif // TARGET_PC

void vr_pmesh::rescale_verts(rational_t s)
{
  int i;
  if( verts )
  {

    for (i=verts->size(); --i>=0; )
    {
      (*verts)[i] = vert(s*((*verts)[i].get_point()));
    }
    for (i=get_num_wedges(); --i>=0; )
    {
      xverts[i].xyz = (*verts)[ vert_refs_for_wedge_ref[i] ].point;
    }
  }
}



const stringx& vr_pmesh::get_bone_name( int x )

{
  return bones[x].name;
}

#ifdef DEBUG
//static int debug_hack;

#endif

const po& vr_pmesh::get_bone_pivot( int x )
{

  return bones[x].pivot;
}



void vr_pmesh::set_light_method( light_method_t lmt )
{
  if(lmt==LIGHT_METHOD_DIFFUSE)
  {
    mesh_flags = (mesh_flags & ~1) | FORCE_LIGHT; // isn't there a named constant we can use instead of ~1?
  }
  else
  {
    mesh_flags = (mesh_flags & ~1) | NORMAL;

  }
  // this code relies on textures that aren't lit not
  // being shared with meshes whose textures are lit:

  // this should be the case, because skin, entity,
  // and level textures are all kept in their own
  // subdirectories

  for( int i=0 ; i<(int)materials.size() ; ++i )
  {
    if(lmt==LIGHT_METHOD_DIFFUSE)
	    materials[i]->set_flags( materials[i]->get_flags() & (0xffff^MAT_ALLOW_ADDITIVE_LIGHT) );
	  else
	    materials[i]->set_flags( materials[i]->get_flags()| MAT_ALLOW_ADDITIVE_LIGHT );

	  materials[i]->process_vertex_contexts();
  }

}

wedge_ref vr_pmesh::get_wedge_ref( face_ref fr, int corner ) const
{
  // if this stuff is too slow we could make progressive and non-progressive meshes two different
  // subclasses of vr_pmesh...
  if(faces)
    return faces[fr].get_wedge_ref(corner);
  return wedge_index_list[ fr*3+corner ];
}

bool vr_pmesh::is_water( face_ref fr ) const
{
  if(faces)
    return faces[fr].is_water();

  return reduced_faces[fr].is_water();
}

bool vr_pmesh::is_walkable( face_ref fr ) const
{
  if(faces)
    return faces[fr].is_walkable();
  return reduced_faces[fr].is_walkable();
}

bool vr_pmesh::is_notwalkable( face_ref fr ) const

{
  if(faces)
    return faces[fr].is_notwalkable();
  return reduced_faces[fr].is_walkable();
}

unsigned char vr_pmesh::get_surface_type( face_ref faceid ) const
{
  if(faces)
    return faces[faceid].get_surface_type();
  return reduced_faces[faceid].get_surface_type();
}

bool vr_pmesh::is_cosmetic( face_ref fr ) const

{
  if(faces)
    return faces[fr].is_cosmetic();
  return reduced_faces[fr].is_cosmetic();
}

material_ref vr_pmesh::get_material_ref( face_ref fr ) const
{
  if(faces)
    return faces[fr].get_material_ref();
  return reduced_faces[fr].get_material_ref();

}

unsigned short vr_pmesh::get_face_flags( face_ref fr ) const
{
  if(faces)
    return faces[fr].get_flags();
  return reduced_faces[fr].get_flags();
}




// @TODO
void vr_pmesh::shrink_memory_footprint()
{
	typedef void (__fastcall *ptr)(vr_pmesh*);
	ptr func = (ptr)0x005AE3D0;

	func(this);
}

#include "my_assertions.h"

void validate_vr_pmesh(void)
{
	VALIDATE_VAL(VISREP_PMESH, 0);
}

#include "my_patch.h"

void patch_vr_pmesh(void)
{
}
