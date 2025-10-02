#ifndef D3D_TEXTUREMGR_H
#define D3D_TEXTUREMGR_H

#include "refptr.h"

// Texel bit organization.
enum texture_format_t
{
  TEX_ARGB1555 = 0,
  TEX_RGB565,
  TEX_ARGB4444,
  TEX_PAL8 = 6,
  TEX_RGB888,
  TEX_ARGB8888,
  INVALID_TEXTURE_FORMAT,
  NUM_TEXTURE_FORMATS = INVALID_TEXTURE_FORMAT   
};



class hw_texture : public ref
{
public:
  hw_texture() 
    : users( 0 ), width( 0 ), height( 0 ), format( 0 ) 
  {
    data = NULL;
    data_size = 0;
#ifndef PRE_WADE_GFX
    // @Patch
	//xb_texture = NULL;
#endif
  }
  virtual ~hw_texture()
  {
    if ((data != NULL) && (data_size > 0))
      free(data);
  }

  inline bool is_valid()    const 
#ifdef PRE_WADE_GFX
  { return (data != NULL) && (data_size > 0); }
#else
  { 
	  // @Patch
	  return true;
	  //return (xb_texture); 
  }
#endif
  inline bool is_loaded()   const { return is_valid(); }
  inline const stringx& get_name() const { assert( is_loaded() ); return name; }
  inline int get_width()    const { assert( is_loaded() ); return width; }
  inline int get_height()   const { assert( is_loaded() ); return height; }

	inline int get_original_width()  const { assert(is_loaded()); return original_width; }   // original height before any compatibility scaling
	inline int get_original_height() const { assert(is_loaded()); return original_height; }  // original width before any compatibility scaling
	
  inline int get_format() const { assert( is_loaded() ); return format; }
	inline bool is_translucent() const { return (format==TEX_ARGB8888)||(format==TEX_ARGB1555)||(format==TEX_ARGB4444); }

  void unload();

protected:
  stringx name;
  uint32 users;         // track how many people are using this texture
  uint32 width, height;
  uint32 original_width, original_height;
	uint32 width_bit, height_bit;	// 2 ^ n = Texture dimensions.
  int format;         
  uint32 sce_format;    // sony's texture format type

  uint32 addr;			    // VRAM address.

	uint32  data_size;		// Size of texture data.
	uint32* data;		      // RAM address.
#ifndef PRE_WADE_GFX
  
	// @Patch
	//nglTexture *xb_texture;
#endif

	// Load this texture from a file.
  bool load( const stringx & _name );
  bool load_raw_tga(os_file &the_file);
  
  friend class hw_texture_mgr;
  friend class hw_rasta;
  friend class vertex_context;
  friend void save_entire_state();
  friend void load_entire_state();
};

#endif
