#ifndef NGL_PC_H
#define NGL_PC_H

/*---------------------------------------------------------------------------------------------------------

  Defines and types for projects that conditionally compile NGL.
---------------------------------------------------------------------------------------------------------*/
#define NGL       1


struct nglMesh
{
	// @TODO
};

// Mesh material structure with a fixed set of passes.
#pragma pack(push,1)


struct nglMaterial
{
	int a;
	// @TODO
};

#pragma pack(pop)

// Texture resource structure.  These are stored in the texture instance bank.
struct nglTexture
{
	uint32 Type;           // Texture type, see NGLTEX_xxx enum.
	uint32 Static;         // The texture is static and must NEVER be released
	// Furthermore never release the associated D3D texture and surface.

	uint32 Width, Height;  // Texture dimensions.
	uint32 MipmapLevels;   // Number of mipmap levels.
	uint32 *Data;          // Pixel data address in RAM (only valid when locked!).
	uint32 DataPitch;      // Pitch of the data (only valid when locked!).

	// Textures.
	// @Patch
	/*
	union
	{
		IDirect3DTexture8 *Simple;    // D3D "simple" texture.
		IDirect3DCubeTexture8 *Cube;  // D3D cube texture for cubic mapping (6 surfaces).
	}
	DXTexture;

	// File Pointer (When loading an XPR file, the file pointer is not released until the texture is destroyed
	nglFileBuf *File;

	// Surface.
	IDirect3DSurface8 *DXSurface;  // D3D surface (aka texture of level 0).
	*/


	// IFL format data:
	uint32 NFrames;           // If this member is greater than 1, this is an animated texture.
	nglTexture **Frames;      // Array of texture pointers for the individual frames
	// (which are also stored in the instance bank).
	uint32 Format;            // See XBox NGLTF_xxx flags.


	// @Patch
	//nglFixedString FileName;  // Original filename.
};

// Mesh flags.
enum
{
	// Clipping stuff.
	NGLMESH_PERFECT_TRICLIP = 0x00000001,
	NGLMESH_REJECT_TRICLIP  = 0x00000002,
	NGLMESH_REJECT_SPHERE   = 0x00000004,

	// Transform stuff.
	NGLMESH_SKINNED         = 0x00000020,

	NGLMESH_LOD             = 0x00000100,

	NGLMESH_TEMP            = 0x00001000,  // Created in temporary memory, lost after the next call to nglListSend.
	NGLMESH_DOUBLE_BUFFER   = 0x00002000,  // NGL will double buffer the scratch mesh (to prevent modifying it while it's rendering).

	// Set by code, tool should never set.
	NGLMESH_PROCESSED       = 0x00100000,
	NGLMESH_BUFFER_OWNER    = 0x00200000,
	NGLMESH_SCRATCH_MESH    = 0x00400000,

	// Lighting categories.
	NGLMESH_LIGHTCAT_1      = 0x01000000,
	NGLMESH_LIGHTCAT_2      = 0x02000000,
	NGLMESH_LIGHTCAT_3      = 0x04000000,

	NGLMESH_LIGHTCAT_4      = 0x08000000,
	NGLMESH_LIGHTCAT_5      = 0x10000000,
	NGLMESH_LIGHTCAT_6      = 0x20000000,
	NGLMESH_LIGHTCAT_7      = 0x40000000,

	NGLMESH_LIGHTCAT_8      = 0x80000000,
};

EXPORT void nglPrintf(const char *Format, ...);

#endif
