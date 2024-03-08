#ifndef GFX_GL_H
#define GFX_GL_H

//
// NOTE(philip): OpenGL Header
//

// NOTE(philip): Since Windows only ever shipped with OpenGL 1.1, any modern OpenGL functions need to be loaded
// explicitly from the OpenGL driver that is provided by the GPU manufacturer. Below are all the types, constants and
// functions from OpenGL 1.2 and onward that were used in this application, as well as reference.

// NOTE(philip): Reference: https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h

// NOTE(philip): OpenGL 1.2

#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1

// NOTE(philip): OpenGL 1.5

typedef u64 GLsizeiptr;

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4

typedef void gl_bind_buffer(GLenum target, GLuint buffer);
typedef void gl_delete_buffers(GLsizei n, const GLuint *buffers);
typedef void gl_gen_buffers(GLsizei n, GLuint *buffers);
typedef void gl_buffer_data(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

global gl_bind_buffer *glBindBuffer = 0;
global gl_delete_buffers *glDeleteBuffers = 0;
global gl_gen_buffers *glGenBuffers = 0;
global gl_buffer_data *glBufferData = 0;

// NOTE(philip): OpenGL 2.0

typedef char GLchar;

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83

typedef void gl_attach_shader(GLuint program, GLuint shader);
typedef void gl_compile_shader(GLuint shader);
typedef GLuint gl_create_program(void);
typedef GLuint gl_create_shader(GLenum type);
typedef void gl_delete_program(GLuint program);
typedef void gl_delete_shader(GLuint shader);
typedef void gl_detach_shader(GLuint program, GLuint shader);
typedef void gl_enable_vertex_attrib_arrray(GLuint index);
typedef void gl_get_program_iv(GLuint program, GLenum pname, GLint *params);
typedef void gl_get_program_info_log(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void gl_get_shader_iv(GLuint shader, GLenum pname, GLint *params);
typedef void gl_get_shader_info_log(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLint gl_get_uniform_location(GLuint program, const GLchar *name);
typedef void gl_link_program(GLuint program);
typedef void gl_shader_source(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void gl_use_program(GLuint program);
typedef void gl_uniform_1fv(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_uniform_3fv(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_uniform_4fv(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_uniform_matrix_4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void gl_validate_program(GLuint program);
typedef void gl_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                      const void *pointer);

global gl_attach_shader *glAttachShader = 0;
global gl_compile_shader *glCompileShader = 0;
global gl_create_program *glCreateProgram = 0;
global gl_create_shader *glCreateShader = 0;
global gl_delete_program *glDeleteProgram = 0;
global gl_delete_shader *glDeleteShader = 0;
global gl_detach_shader *glDetachShader = 0;
global gl_enable_vertex_attrib_arrray *glEnableVertexAttribArray = 0;
global gl_get_program_iv *glGetProgramiv = 0;
global gl_get_program_info_log *glGetProgramInfoLog = 0;
global gl_get_shader_iv *glGetShaderiv = 0;
global gl_get_shader_info_log *glGetShaderInfoLog = 0;
global gl_get_uniform_location *glGetUniformLocation = 0;
global gl_link_program *glLinkProgram = 0;
global gl_shader_source *glShaderSource = 0;
global gl_use_program *glUseProgram = 0;
global gl_uniform_1fv *glUniform1fv = 0;
global gl_uniform_3fv *glUniform3fv = 0;
global gl_uniform_4fv *glUniform4fv = 0;
global gl_uniform_matrix_4fv *glUniformMatrix4fv = 0;
global gl_validate_program *glValidateProgram = 0;
global gl_vertex_attrib_pointer *glVertexAttribPointer = 0;

// NOTE(philip): OpenGL 3.0

typedef void gl_bind_vertex_array(GLuint array);
typedef void gl_delete_vertex_arrays(GLsizei n, const GLuint *arrays);
typedef void gl_gen_vertex_arrays(GLsizei n, GLuint *arrays);

global gl_bind_vertex_array *glBindVertexArray = 0;
global gl_delete_vertex_arrays *glDeleteVertexArrays = 0;
global gl_gen_vertex_arrays *glGenVertexArrays = 0;

//
// NOTE(philip): GPU Structures
//

struct shader
{
    GLuint Program;

    // NOTE(philip): System Uniforms
    GLint ViewProjectionUniform;
    GLint CameraPositionUniform;

    // NOTE(philip): Scene Uniforms
    GLint AmbientLightColorUniform;
    GLint AmbientLightIntensityUniform;

    GLint LightPositionUniform;
    GLint LightColorUniform;
    GLint LightIntensityUniform;

    // NOTE(philip): Object Uniforms
    GLint TransformUniform;

    // NOTE(philip): Material Uniforms
    GLint MaterialUseDiffuseMapUniform;
    GLint MaterialBaseColorUniform;
    GLint MaterialIsLitUniform;
    GLint MaterialAmbientUniform;
    GLint MaterialDiffuseUniform;
    GLint MaterialSpecularUniform;
};

struct material
{
    material_properties Properties;

    b32 UseDiffuseMap;
    GLuint DiffuseMap;

    v4 BaseColor;

    f32 Ambient;
    f32 Diffuse;
    f32 Specular;
};

struct mesh
{
    GLuint VertexArray;
    GLuint VertexBuffer;
    GLuint IndexBuffer;

    u64 SubmeshCount;
    submesh *Submeshes;

    u64 MaterialCount;
    material *Materials;
};

#endif
