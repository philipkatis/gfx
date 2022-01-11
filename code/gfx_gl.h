#ifndef GFX_GL_H
#define GFX_GL_H

/*

  NOTE(philip): This application is being developed for Windows, where only OpenGL 1.1 is natively supported. So
  things like modern function definitions, etc. need to be included by the application and loaded from the OpenGL
  driver provided by the GPU vendor. Reference: https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h

*/

// NOTE(philip): OpenGL 1.5

typedef signed long long int GLsizeiptr;

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

#define GL_STATIC_DRAW                    0x88E4

typedef void gl_gen_buffers(GLsizei n, GLuint *buffers);
typedef void gl_bind_buffer(GLenum target, GLuint buffer);
typedef void gl_buffer_data(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void gl_delete_buffers(GLsizei n, const GLuint *buffers);

static gl_gen_buffers *glGenBuffers = 0;
static gl_bind_buffer *glBindBuffer = 0;
static gl_buffer_data *glBufferData = 0;
static gl_delete_buffers *glDeleteBuffers = 0;

// NOTE(philip): OpenGL 2.0

typedef char GLchar;

#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30

#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84

typedef GLuint gl_create_shader(GLenum type);
typedef void gl_shader_source(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void gl_compile_shader(GLuint shader);
typedef void gl_get_shader_iv(GLuint shader, GLenum pname, GLint *params);
typedef void gl_get_shader_info_log(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void gl_delete_shader(GLuint shader);
typedef GLuint gl_create_program(void);
typedef void gl_attach_shader(GLuint program, GLuint shader);
typedef void gl_link_program(GLuint program);
typedef void gl_validate_program(GLuint program);
typedef void gl_get_program_iv(GLuint program, GLenum pname, GLint *params);
typedef void gl_get_program_info_log(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLint gl_get_uniform_location(GLuint program, const GLchar *name);
typedef void gl_use_program(GLuint program);
typedef void gl_uniform_matrix_4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void gl_delete_program(GLuint program);
typedef void gl_enable_vertex_attrib_array(GLuint index);
typedef void gl_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                      const void *pointer);

static gl_create_shader *glCreateShader = 0;
static gl_shader_source *glShaderSource = 0;
static gl_compile_shader *glCompileShader = 0;
static gl_get_shader_iv *glGetShaderiv = 0;
static gl_get_shader_info_log *glGetShaderInfoLog = 0;
static gl_delete_shader *glDeleteShader = 0;
static gl_create_program *glCreateProgram = 0;
static gl_attach_shader *glAttachShader = 0;
static gl_link_program *glLinkProgram = 0;
static gl_validate_program *glValidateProgram = 0;
static gl_get_program_iv *glGetProgramiv = 0;
static gl_get_program_info_log *glGetProgramInfoLog = 0;
static gl_get_uniform_location *glGetUniformLocation = 0;
static gl_use_program *glUseProgram = 0;
static gl_uniform_matrix_4fv *glUniformMatrix4fv = 0;
static gl_delete_program *glDeleteProgram = 0;
static gl_enable_vertex_attrib_array *glEnableVertexAttribArray = 0;
static gl_vertex_attrib_pointer *glVertexAttribPointer = 0;

// NOTE(philip): OpenGL 3.0

typedef void gl_gen_vertex_arrays(GLsizei n, GLuint *arrays);
typedef void gl_bind_vertex_array(GLuint array);
typedef void gl_delete_vertex_arrays(GLsizei n, const GLuint *arrays);

static gl_gen_vertex_arrays *glGenVertexArrays = 0;
static gl_bind_vertex_array *glBindVertexArray = 0;
static gl_delete_vertex_arrays *glDeleteVertexArrays = 0;

#endif
