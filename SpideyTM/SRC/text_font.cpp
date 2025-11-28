// text_font.cpp
// Copyright (C) 2000 Treyarch L.L.C.  ALL RIGHTS RESERVED

// With precompiled headers enabled, all text up to and including
// the following line are ignored by the compiler (dc 01/18/02)
#include "global.h"


#include "hwrasterize.h"
#include "text_font.h"
#include "vertwork.h"
#include "material.h"
#include "forceflags.h"
#include "projconst.h"
#include "chunkfile.h"
#include "geomgr.h"
#include "matfac.h"
//#include "coords.h"
//#include "globaltextenum.h"

extern stringx ksGlobalButtonArray[];

#if defined(TARGET_XBOX)
#include "osdevopts.h"
#endif /* TARGET_XBOX JIV DEBUG */

// @Patch
#include "osdevopts.h"

int g_debug_text_quads = 1;

//#pragma todo("Remove text_font and have the debugging output use the NEW one.")

text_font::text_font(const char* filename)
{
  chunk_file f;
  stringx fdname = stringx(filename)+".fd";
  f.open(fdname);
  if (f.at_eof())
    error("missing "+fdname);
  cwidth=0;
  cheight=0;
  cspacing=1;
  first=32;
  count=0;
  cperrow=0;

  while (!f.at_eof())
  {
    chunk_flavor flavor;
    serial_in(f,&flavor);
    static const chunk_flavor flavor_cwidth("cwidth");

    static const chunk_flavor flavor_cheight("cheight");
    static const chunk_flavor flavor_cspacing("cspacing");
    static const chunk_flavor flavor_first("first");
    static const chunk_flavor flavor_count("count");
    static const chunk_flavor flavor_cperrow("cperrow");
    if (flavor == flavor_cwidth)
    {
      serial_in(f,&cwidth);

    }
    else if (flavor == flavor_cheight)
    {
      serial_in(f,&cheight);
    }
    else if (flavor == flavor_cspacing)
    {

      serial_in(f,&cspacing);
    }
    else if (flavor == flavor_first)
    {

      serial_in(f,&first);
    }
    else if (flavor == flavor_count)
    {
      serial_in(f,&count);
    }
    else if (flavor == flavor_cperrow)
    {
      serial_in(f,&cperrow);
    }

    else if (flavor == CHUNK_END || flavor == CHUNK_EOF)
    {
      break;
    }
  }

  f.close();


  assert(cwidth>0 && cwidth<256);
  assert(cheight>0 && cheight<256);
  assert(cspacing>=0 && cspacing<16);
  assert(cperrow>0 && cperrow<1024);
  assert(first>=0 && first<=65536);
  assert(count>0 && count<65536);

  mat = NEW material(filename,
    MAT_FULL_SELF_ILLUM      |
    MAT_NO_FOG,

    //anim_texture::PUNCH_THROUGH
    TEX_CLAMP_U              |
    TEX_CLAMP_V);
  // should be filtered
#ifdef NGL
  filespec spec( filename );
  stringx path = spec.path;
  stringx name = spec.name;
  nglSetTexturePath( (char*)path.c_str() );
#if defined(TARGET_XBOX)
	nglTexture* Tex = nglLoadTextureA( (char*)name.c_str() );
#else
  nglTexture* Tex = nglLoadTexture( (char*)name.c_str() ); // Toby removed the lock part

#endif
  if ( Tex )
    itz=vector2d(1.0f/Tex->Width,1.0f/Tex->Height);
  else
    itz = vector2d( 1, 1 );
#else
  const hw_texture* tex = mat->get_texture(0);
  vector2di texsize(tex->get_original_width(),tex->get_original_height());
  itz=vector2d(1.0f/texsize.x,1.0f/texsize.y);
#endif
}

text_font::~text_font()
{
  delete mat;
}


void text_font::get_char_extent(char c, glyph_info& ginfo) const
{
  c -= first;
#ifdef TARGET_PC

  ldiv_t res = ldiv(c,cperrow); // do div and mod at same time
  uint32 x = res.rem;
  uint32 y = res.quot;
#else
  uint32 x = c % cperrow;
  uint32 y = c / cperrow;
#endif
  ginfo.texoffset = vector2di(x*(cwidth+cspacing), y*(cheight+cspacing));
  ginfo.cellsize = vector2di(cwidth,cheight);
}


vector2di text_font::render(const stringx& str, const vector2di& pos, color32 col, float depth, float size)
{
	PANIC;
	return vector2di(0,0);
}

vector2di text_font::get_size(const stringx& str) const
{

  vector2di curpos(0,0);
  for (int i=0; i<str.length(); ++i)
  {
    char c=str[i];
    glyph_info ginfo;

    get_char_extent(c,ginfo);
    curpos.x += ginfo.cellsize.x;
  }

  return curpos;
}


//--------------------------------------------------------------

Font::Font()
{
  clear_glyph_info();
  m_texwidth = 0;
  m_texheight = 0;
  m_refcount = 0;
  max_width = 0;
}

Font::~Font()
{
}

void Font::unload()
{
  clear_glyph_info();
}


bool Font::is_loaded() const
{
  if (m_texname == "")
    return false;


  return true;
}


void Font::clear_glyph_info()
{

  memset(m_chars, 0, FONT_MAX_CHARS * sizeof (glyph_info));
}

#if !defined(TARGET_XBOX)
//#include "kshooks.h"	// For KSWhatever calls
#endif /* TARGET_XBOX JIV DEBUG */

//void KSReleaseFile( nglFileBuf* File );
bool Font::load(const stringx& fn)
{
	PANIC;
	return true;
}



void Font::reload()
{
	PANIC;
}

void Font::getColor(const char c, color32 & col)
{
	// only return alpha=0 for failed or no color cases, because render()
	// will assume that the function failed and draw with the original color

	// NOTE: These color codes are really only needed when you want
	// a single TextString object to have multiple colors.
	// If the TextString is going to be all one color, just set the color parameter in its constructor.
	switch (c)
	{
	case 'r' : col = color32(255, 0, 0, col.c.a); break;		// Red
	case 'g' : col = color32(149, 251, 149, col.c.a); break;		// Green
	case 'u' : col = color32(0, 0, 255, col.c.a); break;		// blUe
	case 'a' : col = color32(0, 0, 0, col.c.a); break;			// blAck
	case 'w' : col = color32(255, 255, 255, col.c.a); break;	// White
	case 'y' : col = color32(255, 255, 0, col.c.a); break;		// Yellow
	case 'o' : col = color32(255, 127, 0, col.c.a); break;		// Orange

	case '0' : col = color32(255, 150, 0, col.c.a); break;		// Gap tricks

	//case '1' : col = color32(255, 255, 127, col.c.a); break;	// High danger tricks
	case '1' : col = color32(255, 255, 102, col.c.a); break;	// High danger tricks
	//case '2' : col = color32(255, 255, 255, col.c.a); break;	// Med danger tricks
	case '2' : col = color32(255, 255, 255, col.c.a); break;	// Med danger tricks
	//case '3' : col = color32(140, 140, 140, col.c.a); break;	// Low danger tricks
	case '3' : col = color32(153, 163, 171, col.c.a); break;	// Low danger tricks

	case '4' : col = color32(2, 162, 255, col.c.a); break;		// Sick meter low tricks
	case '5' : col = color32(0, 216, 120, col.c.a); break;		// Sick meter med tricks
	case '6' : col = color32(236, 206, 74, col.c.a); break;		// Sick meter high tricks
	case '7' : col = color32(231, 101, 26, col.c.a); break;		// Sick meter extreme tricks

	case '8' : col = color32(255, 255, 102, col.c.a); break;	// Statistics screen: info color
	case '9' : col = color32(149, 251, 149, col.c.a); break;	// Statistics screen: standard color

	case 'B' : col = color32(10, 71, 102, col.c.a); break;		// Bio color regular (blue)
	case 'R' : col = color32(150, 0, 24, col.c.a); break;		// Bio color highlight (red)
	case 'G' : col = color32(149, 251, 149, col.c.a); break;		// Description title color (green)
	case 'W' : col = color32(255, 255, 255, col.c.a); break;	// Description color (white)

	case 'n' : col = color32(0, 0, 0, col.c.a); break;			// No color


	case 1   : col = color32(col.c.r, col.c.g, col.c.b, 255); break;	// full alpha
	case 2   : col = color32(col.c.r, col.c.g, col.c.b, 127); break;	// half alpha
	case 3   : col = color32(col.c.r, col.c.g, col.c.b, 63); break;		// 1/4th alpha
	case 4   : col = color32(col.c.r, col.c.g, col.c.b, 33); break;		// 1/8th alpha

	default : col = color32(255, 255, 255, 255);
	}
}

float Font::getScale(const char c)
{
	float scale;
	switch(c)
	{
	case '1': scale = 0.80f; break;
	case '2': scale = 0.85f; break;
	case '3': scale = 0.90f; break;
	case '4': scale = 0.95f; break;
	case '5': scale = 1.00f; break;
	case '6': scale = 1.05f; break;
	case '7': scale = 1.10f; break;
	case '8': scale = 1.15f; break;
	case '9': scale = 1.20f; break;
	case 'c': scale = 1.20f; break;		// credits scale
	case 'n': scale = 0.0f; break;
	default: scale = 0.0f; break;
	}
	return scale;
}

void Font::render(const stringx& text, color32 specifiedColor, bool no_color, bool override_alpha,
				  rational_t x, rational_t y, rational_t depth,
				  const enum HORIZJUST horizJustification, const enum VERTJUST vertJustification,
				  bool even_number_spacing, rational_t scale, float art_button_scale, 

				  float * delta_x, float * delta_y, rational_t angle, bool random_string_fade)
{
	PANIC;
}

bool Font::CheckArtButtonCodes(const char* text, int& length, int& button_index)
{
	PANIC;
	return true;
}

float Font::FindArtButtonRotation(int index)
{
	PANIC;
	return 0.0f;
}

Font::glyph_info* Font::getGlyph(char c)
{
  int which = getIndex(c);
  if (which >=0)
	  return &m_chars[which];
  else
    return NULL;
}

int Font::getIndex(char c)
{
	unsigned char c1 = (unsigned char) c;
	for(int i=0; i<numGlyphs; i++)
		if(m_chars[i].ascii == c1)
			return i;
	return -1;
}


stringx Font::get_filename() const
{
  return m_texname;
}


float Font::text_width(const stringx &s) const
{
  float w = 0;
  int len = s.length();

  for (int i = 0; i < len; i++)
  {

    w += m_chars[s[i]].cell_width;

  }
  return w;
}

float Font::getWidth(stringx s, float sc, bool even_number_spacing, float art_button_sc)
{
	PANIC;
	return 0.0f;
}


float Font::text_height(const stringx &s) const
{
  float h = 0;

  int len = s.length();

  for (int i = 0; i < len; i++)
  {
    if (m_chars[s[i]].cell_height > h)
      h = m_chars[s[i]].cell_height;
  }
  return h;

}



void Font::xform_coord(rational_t &x, rational_t &y, rational_t cx, rational_t cy, rational_t angle)
{
  float s, c, nx, ny;

  s = sin(angle);	// fix trig (dc 08/16/01)
  c = cos(angle);

  x -= cx;
  y -= cy;
  nx = c * x + s * y;
  ny = -s * x + c * y;
  x = nx + cx;
  y = ny + cy;
}





DEFINE_SINGLETON(font_mgr)



font_mgr::font_mgr()
{


}


font_mgr::~font_mgr()
{
	std::list<Font*>::iterator it = m_fonts.begin();

  for (; it != m_fonts.end(); it++)
  {
    delete *it;

  }
}

Font *font_mgr::acquire_font(const stringx& name)
{
	std::list<Font*>::iterator it = m_fonts.begin();
  for (; it != m_fonts.end(); it++)

  {
    Font *f = (*it);
    if (f->get_filename() == name)
    {
      f->inc_ref();
      return f;
    }
  }
  debug_print("Loading NEW font %s", name.c_str());
  Font *n = NEW Font;
  n->load(name);
  if (!n->is_loaded())
    error("Unable to load font %s", name.c_str());

  n->inc_ref();
  m_fonts.push_back(n);
  return n;
}

void font_mgr::release_font(Font *f)
{
  f->dec_ref();
  debug_print("Releasing font %s; %i references remain", f->get_filename().c_str(), f->get_ref());
  if (f->get_ref() == 0)
  {
    debug_print("No more references to this font; it's toast.");
    m_fonts.remove(f);
    delete f;
  }
}


////////////////////////////////////////////////////////////////////


const font_char_info def_char_infos[TYPEFACE_INFOS_MAX] =
{
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },

  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },

  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 9 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },

  {   0,   0,   0,   0, 0 },
  { 167,  62, 183,  78, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },

  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   1,   1,  12,  28, 0 },
  {  15,   1,  19,  28, 0 },
  {  23,   1,  34,  28, 0 },
  {  36,   1,  47,  28, 0 },

  {  49,   1,  61,  28, 0 },
  {   1,  30,  12,  57, 0 },
  {  13,  30,  24,  57, 0 },
  {  25,  30,  37,  57, 0 },
  {  38,  30,  49,  57, 0 },
  {  50,  30,  62,  57, 0 },
  {  55,  86,  59, 100, 0 },
  {   0,   0,   0,   0, 0 },

  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,  14,  15, 0 },
  {  16,   0,  29,  15, 0 },
  {  30,   0,  45,  15, 0 },
  {  47,   0,  61,  15, 0 },
  {  63,   0,  76,  15, 0 },
  {  78,   0,  90,  15, 0 },
  {  90,   0, 105,  15, 0 },
  { 108,   0, 121,  15, 0 },
  { 124,   0, 127,  15, 0 },
  { 129,   0, 139,  15, 0 },
  { 142,   0, 155,  15, 0 },

  { 157,   0, 169,  15, 0 },
  { 170,   0, 186,  15, 0 },
  { 189,   0, 202,  15, 0 },
  { 204,   0, 220,  15, 0 },
  { 221,   0, 234,  15, 0 },
  { 235,   0, 250,  15, 0 },
  {   0,  21,  14,  36, 0 },
  {  15,  21,  27,  36, 0 },
  {  28,  21,  41,  36, 0 },
  {  42,  21,  55,  36, 0 },
  {  56,  21,  71,  36, 0 },
  {  71,  21,  93,  36, 0 },
  {  93,  21, 107,  36, 0 },
  { 107,  21, 121,  36, 0 },
  { 121,  21, 133,  36, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  { 134,  23, 147,  36, 0 },
  { 148,  23, 159,  36, 0 },
  { 160,  23, 172,  36, 0 },

  { 174,  23, 186,  36, 0 },
  { 187,  23, 198,  36, 0 },
  { 200,  23, 210,  36, 0 },
  { 211,  23, 223,  36, 0 },
  { 225,  23, 236,  36, 0 },
  { 239,  23, 242,  36, 0 },
  { 243,  23, 251,  36, 0 },
  {   0,  44,  12,  57, 0 },

  {  13,  44,  23,  57, 0 },
  {  24,  44,  38,  57, 0 },
  {  40,  44,  52,  57, 0 },
  {  53,  44,  66,  57, 0 },
  {  67,  44,  78,  57, 0 },
  {  79,  44,  92,  57, 0 },
  {  94,  44, 105,  57, 0 },
  { 106,  44, 117,  57, 0 },
  { 117,  44, 128,  57, 0 },
  { 129,  44, 140,  57, 0 },
  { 141,  44, 153,  57, 0 },
  { 153,  44, 171,  57, 0 },
  { 172,  44, 183,  57, 0 },
  { 184,  44, 195,  57, 0 },
  { 195,  44, 206,  57, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 },
  {   0,   0,   0,   0, 0 }
};
//-----------------------------------------------------------------
bool font_def::open(const char *name)
{
	PANIC;
	return false;
}

void font_def::unload()

{
	PANIC;
}

//------------------------------------------------------------------
const font_char_info *font_def::get_char_info(char c) const
{
#ifndef __GNUC__
  assert(c >= 0 && c < TYPEFACE_INFOS_MAX);
#endif
  return &char_infos[c];
}
//------------------------------------------------------------------

int font_def::text_width(const stringx& s) const
{
  int w = 0;
  const font_char_info *ci;
  for(int i=0; i<s.size(); ++i)

  {
    ci = get_char_info(s[i]);
    if(ci)
    {
      w += ci->x1 - ci->x0 + 1 + ci->adv;
    }
  }
  return w;
}


void font_def::render(const stringx& s, color32 clr, float& x, float y, float z, float rhw, float scale) const
{
	PANIC;
}


/////////////////////////////////////////////////////////////


typeface_def::~typeface_def()
{
  unload();
}

//--------------------------------------------------------------
void typeface_def::open( const stringx& name )
{

  m_name = name;
}

//--------------------------------------------------------------
void typeface_def::load()
{
  if( !usercount )
  {
    // SYNTHESIZE FILENAME FOR char_infos BASED ON m_name AND READ DATA FIRST
    stringx s( m_name );

    s += ".fon";
    text_file fp;
    fp.open( s, os_file::FILE_READ );
    if(! fp.is_open() )
    {
      error( stringx("Typeface has no FON file") );
    }
    else
    {
      int i;
      int number_of_bitmaps;
      fp.read( &number_of_bitmaps );
      assert(number_of_bitmaps == 1);
      for( i = 0; i < number_of_bitmaps; ++i )
      {
        stringx texname;
        fp.read( &texname );
        assert(!frame.mat);

        // load the texture
        frame_info aframe;
        aframe.mat = NEW material(texname,
          MAT_FULL_SELF_ILLUM      |

          MAT_NO_FOG,
          //anim_texture::PUNCH_THROUGH
          TEX_CLAMP_U              |
          TEX_CLAMP_V);
        // should be filtered
#ifdef TARGET_PS2
        filespec foospec( texname );
        stringx path = foospec.path.c_str();
        stringx name = foospec.name.c_str();

        nglSetTexturePath( (char*)( path.c_str() ) );
        nglTexture* Tex = nglLoadTexture( (char*)( name.c_str() ) );
//        nglTexture* Tex = nglLoadTextureLock( (char*)( name.c_str() ) );

        if ( Tex )

          aframe.size = vector2d( Tex->Width, Tex->Height );
        else
          aframe.size = vector2d( 0, 0 );
#else
        const hw_texture* texture = aframe.mat->get_texture(0);
        aframe.size = vector2d(texture->get_original_width(),texture->get_original_height());
#endif
        frame = aframe; //frame.push_back(aframe);
      }

      for( i = 0; i < TYPEFACE_INFOS_MAX; ++i )
      {
        fp.read( &char_infos[ i ].x0 );
        fp.read( &char_infos[ i ].y0 );
        fp.read( &char_infos[ i ].x1 );
        fp.read( &char_infos[ i ].y1 );
        fp.read( &char_infos[ i ].adv );
        // NEEDED FOR MULTI-BITMAP TYPEFACES AT A LATER DATE
        //fp.read( &char_infos[ i ].bitmap );

        //char_infos[ i ].bitmap = 0;
      }
      while( 1 )
      {
        char c1, c2;
        int i1;
        fp.read( &c1 );
        if( c1 != '[' )
        {
          fp.read( &c2 );
          fp.read( &i1 );
          interletter_kern_info.push_back( inter_kern( c1, c2, i1 ) );
          continue;
        }
        break;
      }

      fp.close();
    }
  }

  // MAINTAIN COUNT OF USERS SO I DON'T RELEASE RESOURCES TOO EARLY DURING AN unload

  ++usercount;
}

//--------------------------------------------------------------
void typeface_def::unload()
{
  if( usercount )
  {
    --usercount;

    if( !usercount )
    {
      // SINCE THE LAST KNOWN USER JUST TOLD US TO UNLOAD
      // DUMP THE TEXTURE NOW

      //for( int i = 0; i < frame.size(); ++i )
      {
        delete frame.mat;

        frame.mat = NULL;

      }
      //frame.resize(0);

      interletter_kern_info.resize(0);
    }
  }
}

//--------------------------------------------------------------

int typeface_def::text_width( const stringx &s ) const
{
  int w = 0;
  const char_info *ci;
  int ssize = s.size();

  for (int i=0 ; i<ssize; ++i)
  {
    if( s[i] == '\\' )
    {
      ++i;
      if (i>=ssize-1) break;

      if (s[i] == 'n') break;
      if (s[i] == 'c') { i += 9; } // skip 3x3 color
      // skip any other escapes with extra chars here
      // ignores all other escapes
      continue;
    }
    ci = get_char_info((unsigned char)(s[i]));
    if(ci)
    {
      w += ci->x1 - ci->x0 + 0 + ci->adv;
      if( i < ssize-1 )
      {
        w += interkern( s[i], s[i+1] );
      }
    }
  }
  return w;

}



//--------------------------------------------------------------
int typeface_def::text_height( const stringx &s ) const
{
  int largest_h = 0;
  const char_info *ci;
  int ssize = s.size();

  for (int i = 0; i<ssize; ++i)
  {
    if( s[i] == '\\' )
    {

      ++i;
      if (i>=ssize-1) break;
      if( s[i] == 'n' ) break;
      if( s[i] == 'c' ) { i += 9; } // skip 3x3 color
      // skip any other escapes with extra chars here
      // ignores all other escapes
      continue;

    }
    ci = get_char_info(s[i]);
    if(ci)
    {
      int h = ci->y1 - ci->y0 + 1;
      if ( h > largest_h )
      {
        largest_h = h;
      }
    }
  }
  return largest_h;
}

//--------------------------------------------------------------

int typeface_def::interkern( int l1, int l2 ) const
{

	std::vector<inter_kern>::const_iterator viki;
  for( viki = interletter_kern_info.begin(); viki != interletter_kern_info.end(); ++viki )
  {
    if( (*viki).letter_pair == std::pair<int,int>(l1,l2) )
    {
      return (*viki).kern;
    }

  }
  return 0;
}



void typeface_def::render(const stringx& s, color32 clr, rational_t& x, rational_t y, rational_t z, rational_t rhw, rational_t scale) const
{
	PANIC;
}



//--------------------------------------------------------------
static std::list<typeface_def*> typeface_list;

typeface_def *typeface_open( const stringx &fname )
{
  typeface_def *res = 0;


  // DETERMINE IF FACE SETUP ALREADY
  if( (res = typeface_already_exists( fname ) ) != 0 )
  {
    return( res );
  }

  res = NEW typeface_def;
  if( res )
  {
    // FIRST WE MUST READ/SETUP A CHAR_INFO ARRAY
    res->open( fname );
    typeface_list.push_back( res );
  }

  // OTHERWISE ALLOCATE A NEW FACE
  return( res );
}

//--------------------------------------------------------------
typeface_def *typeface_already_exists( const stringx &fname )
{

	std::list<typeface_def*>::const_iterator tdi;

  for( tdi = typeface_list.begin(); tdi != typeface_list.end(); ++tdi )
  {
    if( fname == (*tdi)->m_name )
    {
      return *tdi;
    }
  }


  return 0;
}

//--------------------------------------------------------------
void typeface_close( typeface_def *tdefptr )
{
  if( !tdefptr->usercount )
  {
    typeface_list.remove( tdefptr );
    delete tdefptr;
  }
}

#include "my_assertions.h"

void validate_text_font(void)
{
}

#include "my_patch.h"

void patch_text_font(void)
{
}
