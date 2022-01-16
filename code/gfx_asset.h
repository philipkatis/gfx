#ifndef GFX_ASSET_H
#define GFX_ASSET_H

//
// NOTE(philip): OBJ
//

struct index_set
{
    u64 ID;
    u64 Position;
    u64 TextureCoordinate;
    u64 Normal;

    index_set *Next;
};

// TODO(philip): Is this good enough?
#define INDEX_SET_TABLE_SLOT_COUNT 4096

struct index_set_table
{
    index_set *Slots[INDEX_SET_TABLE_SLOT_COUNT];
    u64 Count;
};

// TODO(philip): Remove this.
struct vertex
{
    v3 Position;
    v2 TextureCoordinate;
    v3 Normal;
};

struct submesh
{
    u64 IndexOffset;
    u64 IndexCount;
};

struct mesh_asset
{
    // TODO(philip): Bitfield for what vertex data is available.

    u64 VertexCount;

    // TODO(philip): Change this to void pointer.
    vertex *Vertices;

    u64 IndexCount;
    u32 *Indices;

    u64 SubmeshCount;
    submesh *Submeshes;
};

//
// NOTE(philip): TGA
//

#pragma pack(push, 1)

struct tga_header
{
    u8 IDLength;
    u8 ColorMapType;
    u8 ImageType;

    u16 ColorMapFirstEntryIndex;
    u16 ColorMapEntryCount;
    u8 BitsPerColorMapEntry;

    u16 OriginX;
    u16 OriginY;
    u16 Width;
    u16 Height;
    u8 BitsPerPixel;
    u8 ImageDescriptor;
};

// TODO(philip): Static assert to make sure the size of this is always correct.

#pragma pack(pop)

typedef u32 texture_format;
enum
{
    TextureFormat_None = 0,
    TextureFormat_BGR  = 1,
    TextureFormat_BGRA = 2
};

struct texture_asset
{
    u8 *Data;

    texture_format Format;
    u32 Width;
    u32 Height;
};

#endif
