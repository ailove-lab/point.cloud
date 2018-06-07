#include <stdlib.h>
#include <stdio.h>

#include "Shader.h"

static void check_shader(GLuint sid);
static void load_file( char*  filename,  char** buf);

static void
Init(Shader* outShader, char* name) {
    
    char vert_name[256];
    char frag_name[256];
    char* vert_text;
    char* frag_text;
    
    sprintf(vert_name, "shaders/%s.vert", name);
    sprintf(frag_name, "shaders/%s.frag", name);
    
    load_file(vert_name, &vert_text);
    load_file(frag_name, &frag_text);

    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, (const char**)&vert_text, NULL);
    glCompileShader(vert_id);
    check_shader(vert_id);
    
    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, (const char**)&frag_text, NULL);
    glCompileShader(frag_id);
    check_shader(frag_id);

    outShader->prog = glCreateProgram();
    glAttachShader(outShader->prog, vert_id);
    glAttachShader(outShader->prog, frag_id);
    glLinkProgram (outShader->prog);

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    
    outShader->mvp = glGetUniformLocation(outShader->prog, "mvp");
    // printf("mvp %d\n", outShader->mvp);
    // shader->col = glGetUniformLocation(shader->prog, "col");
    // shader->pos = glGetAttribLocation (shader->prog, "pos");

    // cleanup
    free(frag_text);
    free(vert_text);
}

static Shader* 
Create(char* name) {
    Shader* shader = malloc(sizeof(Shader));
	Init(shader, name);
    return shader;
}

static void
Release(Shader* shader) {
    glDeleteProgram(shader->prog);
    free(shader);
}

static void
Start(Shader* shader) {
    // SET SHADER
    glUseProgram(shader->prog);
}

static void
Stop(Shader* shader) {
	glUseProgram(0);
}

struct AShader AShader[1] = {{
	Create,
	Init,
	Release,
	Start,
	Stop,
}};

// -----------------------------------------------------------------

static void 
load_file(
    char*  filename, 
    char** buf) {

    long length = 0;
    FILE* fp = fopen (filename, "rb");
    
    if(fp) {
        fseek (fp, 0, SEEK_END);
        length = ftell(fp);
        fseek (fp, 0, SEEK_SET);
        *buf = malloc(length+1);
        if (*buf) {
            fread(*buf, 1, length, fp);
        }
        fclose (fp);
        (*buf)[length]=0;
    }
}

static void 
check_shader(GLuint sid) {
    GLint isCompiled = 0;
    glGetShaderiv(sid, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        GLchar error[maxLength];
        glGetShaderInfoLog(sid, maxLength, &maxLength, error);
        fprintf(stderr, "ERR: %s\n", error);

        // Provide the infolog in whatever manor you deem best.
        glDeleteShader(sid); // Don't leak the shader.
        return;
    }
}

// draw loop
/*
void 
wire_draw(
    vbo_t* vbo, 
    vec3 col, 
    float ratio, 
    float x, 
    float y, 
    float scale,
    bool loop) {

    mat4x4 m, p, mvp;

    // SET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);        

    // SET ATTRIBUTES
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat)*2, (void*) 0);

    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    //              sizeof(float) * 5, (void*) (sizeof(float) * 2));

    // SET VIEW
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, x, y, 0.0);
    mat4x4_scale_aniso(m, m, scale, scale, scale);

    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    // SET SHADER
    glUseProgram(program);
    // send view matrix
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glUniform3fv(col_location,  1, (const GLfloat*) col);

    // DRAW
    uint32_t start = 0;
    for(uint32_t i=0; i<vbo->shapes; i++) {
        GLuint l = vbo->parts[i];
        glDrawArrays(loop?GL_LINE_LOOP:GL_LINE_STRIP, start, l);
        start+= l;
    }
    // whole at once
    // glDrawArrays(GL_LINE_LOOP, 0, vbo->vertices);

    // UNSET SHADER
    glUseProgram(0);

    // UNSET ATTRIBUTES
    glDisableVertexAttribArray(vpos_location);

    // UNSET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
*/

/////////////////////////////////////////////////////////////////

