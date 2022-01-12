//
// NOTE(philip): Character Functions
//

function b32
IsWhitespace(char Character)
{
    b32 Result = ((Character == ' ') || (Character == '\t') || (Character == '\n') || (Character == '\r') ||
                  (Character == '\v') || (Character == '\f'));
    return Result;
}

//
// NOTE(philip): String Functions
//

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
