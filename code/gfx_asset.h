#ifndef GFX_ASSET_H
#define GFX_ASSET_H

typedef u32 material_properties;
enum
{
    MaterialProperty_None          = 0,
    MaterialProperty_Unlit         = 1,
    MaterialProperty_Wireframe     = 2
};

struct material_asset
{
    char *Name;

    material_properties Properties;

    v4 BaseColor;
    char *DiffuseMap;

    f32 Ambient;
    f32 Diffuse;
    f32 Specular;
};

struct index_set
{
    u64 ID;
    u64 Position;
    u64 TextureCoordinate;
    u64 Normal;

    index_set *Next;
};

#define INDEX_SET_TABLE_SLOT_COUNT 4096

struct index_set_table
{
    index_set *Slots[INDEX_SET_TABLE_SLOT_COUNT];
    u64 Count;
};

typedef u32 vertex_attributes;
enum
{
    VertexAttribute_None                      = 0,
    VertexAttribute_HasPositions              = 1,
    VertexAttribute_HasTextureCoordinates     = 2,
    VertexAttribute_HasNormals                = 4
};

struct submesh
{
    u64 IndexCount;
    u64 IndexDataOffset;

    s64 MaterialIndex;
};

struct mesh_asset
{
    vertex_attributes VertexAttributes;

    u64 VertexDataSize;
    u8 *VertexData;

    u64 IndexCount;
    u32 *IndexData;

    u64 SubmeshCount;
    submesh *Submeshes;

    u64 MaterialAssetCount;
    material_asset *MaterialAssets;
};

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
    texture_format Format;
    u32 Width;
    u32 Height;

    u8 *Data;
};

#endif
