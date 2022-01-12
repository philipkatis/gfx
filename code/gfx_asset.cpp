struct index_set
{
    u64 Position;
    u64 TextureCoordinate;
    u64 Normal;
};

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
        v2 *TextureCoordinates = (v2 *)OS_AllocateMemory(TextureCoordinateCount * sizeof(v2));
        v3 *Normals = (v3 *)OS_AllocateMemory(NormalCount * sizeof(v3));

        // TODO(philip): Change this into a hash table.
        index_set *UniqueIndexSets = (index_set *)OS_AllocateMemory(TriangleCount * 3 * sizeof(index_set));
        u64 UniqueIndexSetCount = 0;

        Asset->Indices = (u32 *)OS_AllocateMemory(TriangleCount * 3 * sizeof(u32));

        u64 PositionIndex = 0;
        u64 TextureCoordinateIndex = 0;
        u64 NormalIndex = 0;

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
                            // TODO(philip): Pull this out to a function.
                            // TODO(philip): Do this in a loop.

                            Pointer = SkipWhitespace(++Pointer);
                            char *XString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            Pointer = SkipWhitespace(++Pointer);
                            char *YString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            v2 *TextureCoordinate = TextureCoordinates +
                                TextureCoordinateIndex++;

                            TextureCoordinate->X = atof(XString);
                            TextureCoordinate->Y = atof(YString);
                        } break;

                        case 'n':
                        {
                            // TODO(philip): Pull this out to a function.
                            // TODO(philip): Do this in a loop.

                            Pointer = SkipWhitespace(++Pointer);
                            char *XString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            Pointer = SkipWhitespace(++Pointer);
                            char *YString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            Pointer = SkipWhitespace(++Pointer);
                            char *ZString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            v3 *Normal = Normals + NormalIndex++;

                            Normal->X = atof(XString);
                            Normal->Y = atof(YString);
                            Normal->Z = atof(ZString);
                        } break;

                        default:
                        {
                            // TODO(philip): Pull this out to a function.
                            // TODO(philip): Do this in a loop.

                            Pointer = SkipWhitespace(Pointer);
                            char *XString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            Pointer = SkipWhitespace(++Pointer);
                            char *YString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            Pointer = SkipWhitespace(++Pointer);
                            char *ZString = Pointer;

                            Pointer = SkipUntilWhitespace(Pointer);
                            *Pointer = 0;

                            v3 *Position = Positions + PositionIndex++;

                            Position->X = atof(XString);
                            Position->Y = atof(YString);
                            Position->Z = atof(ZString);
                        } break;
                    }
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

                        u64 Position = atoi(PositionString);
                        u64 TextureCoordinate = atoi(TextureCoordinateString);
                        u64 Normal = atoi(NormalString);

                        b32 Found = false;
                        u64 SetID = 0;

                        // TODO(philip): Replace this with a search into the hash table.
                        for (u64 Index = 0;
                             Index < UniqueIndexSetCount;
                             ++Index)
                        {
                            index_set *Set = UniqueIndexSets + Index;
                            if (Set->Position == Position &&
                                Set->TextureCoordinate == TextureCoordinate &&
                                Set->Normal == Normal)
                            {
                                Found = true;
                                SetID = Index;

                                break;
                            }
                        }

                        if (!Found)
                        {
                            SetID = UniqueIndexSetCount;

                            index_set *Set = UniqueIndexSets + UniqueIndexSetCount++;
                            Set->Position = Position;
                            Set->TextureCoordinate = TextureCoordinate;
                            Set->Normal = Normal;
                        }

                        Asset->Indices[Asset->IndexCount++] = SetID;
                    }
                } break;

                default:
                {
                    Pointer = SkipLine(Pointer);
                } break;
            }

            ++Pointer;
        }

        Asset->VertexCount = UniqueIndexSetCount;
        Asset->Vertices = (vertex *)OS_AllocateMemory(Asset->VertexCount * sizeof(vertex));

        for (u64 Index = 0;
             Index < UniqueIndexSetCount;
             ++Index)
        {
            index_set *Set = UniqueIndexSets + Index;
            vertex *Vertex = Asset->Vertices + Index;

            Vertex->Position = Positions[Set->Position - 1];
            Vertex->TextureCoordinate = TextureCoordinates[Set->TextureCoordinate - 1];
            Vertex->Normal = Normals[Set->Normal - 1];
        }

        OS_FreeMemory(UniqueIndexSets);
        OS_FreeMemory(Normals);
        OS_FreeMemory(TextureCoordinates);
        OS_FreeMemory(Positions);

        OS_FreeFileMemory(&FileData);
    }
}

function void
FreeMeshAsset(mesh_asset *MeshAsset)
{
    Assert(MeshAsset);

    OS_FreeMemory(MeshAsset->Vertices);
    OS_FreeMemory(MeshAsset->Indices);

    MeshAsset->VertexCount = 0;
    MeshAsset->Vertices = 0;
    MeshAsset->IndexCount = 0;
    MeshAsset->Indices = 0;
}
