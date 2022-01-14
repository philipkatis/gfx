//
// NOTE(philip): OBJ
//

function char *
OBJ_ParseV2(char *Pointer, v2 *Vector)
{
    for (u32 ComponentIndex = 0;
         ComponentIndex < 2;
         ++ComponentIndex)
    {
        Pointer = SkipWhitespace(++Pointer);
        char *ComponentString = Pointer;

        Pointer = SkipUntilWhitespace(Pointer);
        *Pointer = 0;

        // TODO(philip): Replace atof.
        Vector->Data[ComponentIndex] = atof(ComponentString);
    }

    return Pointer;
}

function char *
OBJ_ParseV3(char *Pointer, v3 *Vector)
{
    for (u32 ComponentIndex = 0;
         ComponentIndex < 3;
         ++ComponentIndex)
    {
        Pointer = SkipWhitespace(++Pointer);
        char *ComponentString = Pointer;

        Pointer = SkipUntilWhitespace(Pointer);
        *Pointer = 0;

        // TODO(philip): Replace atof.
        Vector->Data[ComponentIndex] = atof(ComponentString);
    }

    return Pointer;
}

// TODO(philip): Documentation.
function u32
GetIndexSetID(index_set_table *Table, u64 Position, u64 TextureCoordinate, u64 Normal)
{
    u32 ID = 0;
    b32 Found = false;

    // TODO(philip): Better hash function.
    u64 Hash = Position + TextureCoordinate + Normal;
    u64 Slot = Hash % INDEX_SET_TABLE_SLOT_COUNT;

    for (index_set *Set = Table->Slots[Slot];
         Set;
         Set = Set->Next)
    {
        if ((Set->Position == Position) && (Set->TextureCoordinate == TextureCoordinate) &&
            (Set->Normal == Normal))
        {
            ID = Set->ID;
            Found = true;

            break;
        }
    }

    if (!Found)
    {
        // TODO(philip): Change to using a memory arena.
        index_set *Set = (index_set *)OS_AllocateMemory(sizeof(index_set));

        Set->ID = Table->Count++;
        Set->Position = Position;
        Set->TextureCoordinate = TextureCoordinate;
        Set->Normal = Normal;

        Set->Next = Table->Slots[Slot];
        Table->Slots[Slot] = Set;

        ID = Set->ID;
    }

    return ID;
}

// TODO(philip): Documentation.
// TODO(philip): Return success or failure.
function void
LoadOBJ(char *Path, mesh_asset *Asset)
{
    Assert(Asset);

    // TODO(philip): Redundant?
    *Asset = { };

    buffer FileData;
    if (OS_ReadEntireFile(Path, &FileData))
    {
        u64 PositionCount = 0;
        u64 TextureCoordinateCount = 0;
        u64 NormalCount = 0;
        u64 TriangleCount = 0;

        {
            char *Pointer = (char *)FileData.Data;
            char *End = (char *)FileData.Data + FileData.Size;

            while (Pointer != End)
            {
                switch (*Pointer)
                {
                    case 'v':
                    {
                        switch (*(++Pointer))
                        {
                            case 't':
                            {
                                ++TextureCoordinateCount;
                            } break;

                            case 'n':
                            {
                                ++NormalCount;
                            } break;

                            default:
                            {
                                ++PositionCount;
                            } break;
                        }
                    } break;

                    case 'g':
                    {
                        ++Asset->SubmeshCount;
                    } break;

                    case 'f':
                    {
                        ++TriangleCount;
                    } break;
                }

                Pointer = SkipLine(Pointer);
                ++Pointer;
            }
        }

        v3 *Positions = (v3 *)OS_AllocateMemory(PositionCount * sizeof(v3));
        u64 PositionIndex = 0;

        v2 *TextureCoordinates = (v2 *)OS_AllocateMemory(TextureCoordinateCount * sizeof(v2));
        u64 TextureCoordinateIndex = 0;

        v3 *Normals = (v3 *)OS_AllocateMemory(NormalCount * sizeof(v3));
        u64 NormalIndex = 0;

        index_set_table IndexSetTable = { };

        Asset->Indices = (u32 *)OS_AllocateMemory(TriangleCount * 3 * sizeof(u32));

        Asset->Submeshes = (submesh *)OS_AllocateMemory(Asset->SubmeshCount * sizeof(submesh));
        s64 SubmeshIndex = -1;

        char *Pointer = (char *)FileData.Data;
        char *End = (char *)FileData.Data + FileData.Size;

        while (Pointer != End)
        {
            switch (*Pointer)
            {
                case 'v':
                {
                    switch (*(++Pointer))
                    {
                        case 't':
                        {
                            v2 *TextureCoordinate = TextureCoordinates + TextureCoordinateIndex++;
                            Pointer = OBJ_ParseV2(Pointer, TextureCoordinate);
                        } break;

                        case 'n':
                        {
                            v3 *Normal = Normals + NormalIndex++;
                            Pointer = OBJ_ParseV3(Pointer, Normal);
                        } break;

                        default:
                        {
                            v3 *Position = Positions + PositionIndex++;
                            Pointer = OBJ_ParseV3(Pointer, Position);
                        } break;
                    }
                } break;

                case 'g':
                {
                    Pointer = SkipLine(Pointer);

                    submesh *Submesh = Asset->Submeshes + ++SubmeshIndex;
                    Submesh->IndexOffset = Asset->IndexCount;
                } break;

                case 'f':
                {
                    for (u32 SetIndex = 0;
                         SetIndex < 3;
                         ++SetIndex)
                    {
                        // TODO(philip): Maybe turn this into a loop.

                        Pointer = SkipWhitespace(++Pointer);
                        char *PositionString = Pointer;

                        Pointer = SkipUntil(Pointer, '/');
                        *Pointer = 0;

                        char *TextureCoordinateString = ++Pointer;

                        Pointer = SkipUntil(Pointer, '/');
                        *Pointer = 0;

                        char *NormalString = ++Pointer;

                        Pointer = SkipUntilWhitespace(Pointer);
                        *Pointer = 0;

                        u64 Position = atoi(PositionString) - 1;
                        u64 TextureCoordinate = atoi(TextureCoordinateString) - 1;
                        u64 Normal = atoi(NormalString) - 1;

                        u64 ID = GetIndexSetID(&IndexSetTable, Position, TextureCoordinate, Normal);
                        Asset->Indices[Asset->IndexCount++] = ID;
                    }

                    submesh *Submesh = Asset->Submeshes + SubmeshIndex;
                    Submesh->IndexCount += 3;
                } break;

                default:
                {
                    Pointer = SkipLine(Pointer);
                } break;
            }

            ++Pointer;
        }

        Asset->VertexCount = IndexSetTable.Count;
        Asset->Vertices = (vertex *)OS_AllocateMemory(Asset->VertexCount * sizeof(vertex));

        for (u64 Slot = 0;
             Slot < INDEX_SET_TABLE_SLOT_COUNT;
             ++Slot)
        {
            index_set *Set = IndexSetTable.Slots[Slot];
            while (Set)
            {
                index_set *Next = Set->Next;

                vertex *Vertex = Asset->Vertices + Set->ID;
                Vertex->Position = Positions[Set->Position];
                Vertex->TextureCoordinate = TextureCoordinates[Set->TextureCoordinate];
                Vertex->Normal = Normals[Set->Normal];

                // TODO(philip): Change to using a memory arena.
                OS_FreeMemory(Set);

                Set = Next;
            }
        }

        OS_FreeMemory(Normals);
        OS_FreeMemory(TextureCoordinates);
        OS_FreeMemory(Positions);

        OS_FreeFileMemory(&FileData);
    }
}

function void
FreeMeshAsset(mesh_asset *Asset)
{
    Assert(Asset);

    OS_FreeMemory(Asset->Vertices);
    OS_FreeMemory(Asset->Indices);
    OS_FreeMemory(Asset->Submeshes);

    Asset->VertexCount = 0;
    Asset->Vertices = 0;
    Asset->IndexCount = 0;
    Asset->Indices = 0;
    Asset->SubmeshCount = 0;
    Asset->Submeshes = 0;
}
