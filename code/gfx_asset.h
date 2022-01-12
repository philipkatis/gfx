#ifndef GFX_ASSET_H
#define GFX_ASSET_H

// TODO(philip): Remove this.
struct vertex
{
    v3 Position;
    v2 TextureCoordinate;
    v3 Normal;
};

struct mesh_asset
{
    // TODO(philip): Bitfield for what vertex data is available.

    u64 VertexCount;

    // TODO(philip): Change this to void pointer.
    vertex *Vertices;

    u64 IndexCount;
    u32 *Indices;
};

#endif
