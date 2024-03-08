function b32
IsWhitespace(char Character)
{
    b32 Result = ((Character == ' ') || (Character == '\t') || (Character == '\n') || (Character == '\r') ||
                  (Character == '\v') || (Character == '\f'));
    return Result;
}

function b32
StringCompare(char *StringA, char *StringB)
{
    b32 Equal = true;

    while (*StringA && *StringB && (*StringA == *StringB))
    {
        ++StringA;
        ++StringB;
    }

    if (*StringA != *StringB)
    {
        Equal = false;
    }

    return Equal;
}

function char *
SkipWhitespace(char *String)
{
    while (*String && IsWhitespace(*String))
    {
        ++String;
    }

    return String;
}

function char *
SkipUntilWhitespace(char *String)
{
    while (*String && !IsWhitespace(*String))
    {
        ++String;
    }

    return String;
}

function char *
SkipLine(char *String)
{
    while (*String && (*String != '\n'))
    {
        ++String;
    }

    return String;
}

function char *
SkipUntil(char *String, char Delimiter)
{
    while (*String && (*String != Delimiter))
    {
        ++String;
    }

    return String;
}

function char *
ExtractDirectoryPath(char *FilePath)
{
    char *End = 0;

    for (char *Character = FilePath;
         *Character;
         ++Character)
    {
        if (*Character == '\\')
        {
            End = Character;
        }
    }

    u64 DirectoryPathLength = (End - FilePath);

    char *Path = (char *)OS_AllocateMemory((DirectoryPathLength + 1) * sizeof(char));
    memcpy(Path, FilePath, DirectoryPathLength * sizeof(char));

    return Path;
}

function char *
ConcatenatePaths(char *PathA, char *PathB)
{
    u64 PathALength = strlen(PathA);
    u64 PathBLength = strlen(PathB);
    u64 PathLength = (PathALength + PathBLength + 1);

    char *Path = (char *)OS_AllocateMemory((PathLength + 1) * sizeof(char));
    char *Pointer = Path;

    memcpy(Pointer, PathA, PathALength * sizeof(char));
    Pointer += (PathALength * sizeof(char));

    *Pointer = '\\';
    ++Pointer;

    memcpy(Pointer, PathB, PathBLength * sizeof(char));

    return Path;
}
