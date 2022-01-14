#ifndef GFX_ASSET_H
#define GFX_ASSET_H

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

#endif
