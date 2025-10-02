#ifndef D3D_RASTERIZE_H
#define D3D_RASTERIZE_H

#include "hwospc\d3d_texturemgr.h"
#include "refptr.h"

enum 
{ 
  TEXTURE_COORD_DIFFUSE,   // Main texture
  TEXTURE_COORD_DECAL,     // decal or light map

//  TEXTURE_PASS_DYNAMIC,   // detail and environment
  MAX_TEXTURE_COORDS 
};

enum lock_type_t
{
  LOCK_CLEAR,
  LOCK_NOOVERWRITE,
  LOCK_OVERWRITE,
};

class hw_rasta_vert_lit
{
public:
  hw_rasta_vert_lit() {}


  ////////////////////////////////////////////////////////////////////////////////

  // untransformed constructors
  hw_rasta_vert_lit(const vector3d& _p, color32 _diffuse, float u, float v) 
    : xyz(_p), diffuse(_diffuse)
  {
    tc[ 0 ] = vector2d( u, v );
  }


  // compatibility version with different param order.
  hw_rasta_vert_lit(const vector3d& _p, const texture_coord& _tc, color32 _diffuse ) 
    : xyz(_p), diffuse(_diffuse)
  {
    tc[ 0 ] = _tc;
  }

  // clear out stuff that isn't normally overwritten but needs cleared
  void reset() {}

  vector3d xyz; // vertex coordinate

  
  float reserved;

        vector3d& get_unxform()       { return xyz; }
  const vector3d& get_unxform() const { return xyz; }


  inline void set_xyz(const vector3d& p)
  {
    xyz=p; 
  }

  color32 diffuse; // color
  color32 specular; // compat with D3D_LVERTEX

  // this is the simplest way I thought of to make sure verts match.
  // which is required.
  unsigned char bone_ids[MAX_SKIN_BONES]; // used to be kept in specular.c.a
  float bone_weights[MAX_SKIN_BONES];

  vector2d tc[ MAX_TEXTURE_COORDS ]; // texture coordinate
};

// @TODO
class vertex_context
{
};

class hw_rasta_vert
{
public:
  hw_rasta_vert() {}

  ////////////////////////////////////////////////////////////////////////////////
  // untransformed constructors
  hw_rasta_vert(const vector3d& _p, color32 _diffuse, float u, float v)
    : xyz(_p), diffuse(_diffuse)
  {
    bone_ids[0] = 0;
    tc[0].x = u;
    tc[0].y = v;

  }

  // compatibility version with different param order.
  hw_rasta_vert(const vector3d& _p, const texture_coord& _tc, color32 _diffuse )
    : xyz(_p), diffuse(_diffuse)
  {
    bone_ids[0] = 0;
    tc[0] = _tc;
  }

  hw_rasta_vert(const vector3d& _p, color32 _diffuse, uint8 bone_idx, float _u, float _v)
    : xyz(_p), diffuse(_diffuse)
  {
    bone_ids[0] = bone_idx;
    tc[0] = vector2d(_u,_v);
  }


  ////////////////////////////////////////////////////////////////////////////////

  // there are no transformed constructors yet

  // clear out stuff that isn't normally overwritten but needs cleared
  void reset() { /*bone_id = 0;*/ }

  vector3d xyz; // vertex coordinate

        vector3d& get_unxform()       { return xyz; }
  const vector3d& get_unxform() const { return xyz; }

  inline void set_xyz(const vector3d& p)
  {
    xyz=p; 
  }

  unsigned char clip_flags;
  signed char normalx,normaly,normalz;

  void set_normal(const vector3d& v)
  {
    normalx = (signed char)(v.x*127.0F);
    normaly = (signed char)(v.y*127.0F);
    normalz = (signed char)(v.z*127.0F);
  }
  vector3d get_normal() const
  {
    return vector3d(normalx*(1.0F/127.0F),
                    normaly*(1.0F/127.0F),
                    normalz*(1.0F/127.0F));
  }

  color32 diffuse; // color

  int num_bones;
  unsigned char bone_ids[MAX_SKIN_BONES]; // used to be kept in specular.c.a
  float bone_weights[MAX_SKIN_BONES];

        unsigned char& boneid()       { return bone_ids[0]; }
  const unsigned char& boneid() const { return bone_ids[0]; }

  vector2d tc[ MAX_TEXTURE_COORDS ]; // texture coordinate

};

class vert_buf : public ref
{
  vert_buf( const vert_buf& vb );
public:
  vert_buf(); // to construct a global vertex workspace type buffer
  vert_buf(int _max_size); // to construct a buffer of a particular size
	~vert_buf();

  void deconstruct();
  
  void lock(int _max_size, lock_type_t type=LOCK_CLEAR); // pass in -1 to prevent resizing (which clears verts)
  void unlock();

  void optimize();

  int get_max_size() const { return max_size; }


  hw_rasta_vert * begin() const { assert(locked); return verts; }

  hw_rasta_vert * end()   const { assert(locked); return verts + max_size; }

private:
  hw_rasta_vert * verts;
  int max_size;
  bool locked;
  bool optimized;
  friend class hw_rasta;
};

class hw_rasta_vert_xformed
{
public:
  hw_rasta_vert_xformed() {}


  hw_rasta_vert_xformed(const vector3d& _p, rational_t _rhw, texture_coord _tc, color32 _diffuse/*,
                color32 _specular = color32(0,0,0,0)*/ ) 
     : xyz(_p)
     , rhw(_rhw) 
     , diffuse(_diffuse) /*specular(_specular), */
  {
    tc[ 0 ] = _tc;
  }

  // clear out stuff that isn't normally overwritten but needs cleared
  void reset() {}

  vector3d xyz; // vertex coordinate
  
  float rhw;    // inverse Z

  const vector3d& get_unxform() const { return xyz; }
  inline void set_xyz_rhw(const vector3d& p, rational_t _rhw)
  {
    xyz=p; rhw=_rhw;
  }
  inline void set_xyz_rhw(const vector4d& p)

  {
    xyz.x=p.x; xyz.y=p.y; xyz.z=p.z; rhw=p.w;
  }
  inline void homogenize_xyzw(const vector4d& p)
  {
    if (!p.w)
      xyz.x=xyz.y=xyz.z=rhw=0.0f;
    else
    {
      rhw = 1.0f / p.w;
      xyz.x=p.x*rhw; xyz.y=p.y*rhw; xyz.z=p.z*rhw;  
    }
  }

  color32 diffuse; // color
  color32 specular; // compat with D3D_TLVERTEX
  // this is the simplest way I thought of to make sure verts match.
  // which is required.
  unsigned char bone_ids[MAX_SKIN_BONES]; // used to be kept in specular.c.a
  float bone_weights[MAX_SKIN_BONES];


  vector2d tc[ MAX_TEXTURE_COORDS ]; // texture coordinate
};

class vert_buf_xformed : public ref
{
  vert_buf_xformed( const vert_buf_xformed& vb );
public:
  vert_buf_xformed();
  vert_buf_xformed(int _max_size); // to construct a buffer of a particular size
	~vert_buf_xformed();

  void deconstruct();

  void lock(int _max_size, lock_type_t type=LOCK_CLEAR); // pass in -1 to prevent clearing of existing verts
  void unlock();

  int get_max_size() const { return max_size; }

  hw_rasta_vert_xformed * begin() const { assert(locked); return verts; }

  hw_rasta_vert_xformed * end()   const { assert(locked); return verts + max_size; }

private:
  hw_rasta_vert_xformed* verts;
  int max_size;
  bool locked;
  friend class hw_rasta;

};




#endif
