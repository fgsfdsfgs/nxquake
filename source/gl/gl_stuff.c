/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "glquake.h"
#include "glshaders.h"
#include "quakedef.h"
#include "sys.h"
#include <cglm/cglm.h>
#include <stdint.h>
#include <stdlib.h>

//
// shaders
//

static GLuint fs[9];
static GLuint vs[4];
static GLuint programs[9];
static GLuint cur_program = (GLuint)-1;

static qboolean just_color; // HACK

static int state_mask = 0x00;
static int texenv_mask = 0;
static int texcoord_state = 0;
static int alpha_state = 0;
static int color_state = 0;
static GLfloat cur_color[4];
static GLfloat cur_texcoord[2];
static GLint u_mvp[9];
static GLint u_monocolor;
static GLint u_modcolor[2];

static qboolean shaders_loaded = false;
static qboolean shaders_reload = false;

static qboolean program_changed = true;

static void *CompileShader(const char *src, GLuint idx, GLboolean frag) {
    static GLchar msg[2048];
    GLint status;

    GLuint s = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
    if (!s)
        Sys_Error("Could not create %s shader #%d!", frag ? "frag" : "vert", idx);

    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(s, sizeof(msg), NULL, msg);
        glDeleteShader(s);
        Sys_Error("Could not compile %s shader #%d:\n%s", frag ? "frag" : "vert", idx, msg);
    }

    if (frag)
        fs[idx] = s;
    else
        vs[idx] = s;
}

static inline void LinkShader(GLuint pidx, GLuint fidx, GLuint vidx, GLboolean texcoord, GLboolean color) {
    static GLchar msg[2048];
    programs[pidx] = glCreateProgram();
    glAttachShader(programs[pidx], fs[fidx]);
    glAttachShader(programs[pidx], vs[vidx]);
    // vglBindAttribLocation( programs[pidx], 0, "aPosition", 3, GL_FLOAT );
    // if( texcoord ) vglBindAttribLocation( programs[pidx], 1, "aTexCoord", 2,
    // GL_FLOAT ); if( color ) vglBindAttribLocation( programs[pidx], 2,
    // "aColor", 4, GL_FLOAT );
    glLinkProgram(programs[pidx]);
    GLint success;
    glGetProgramiv(programs[pidx], GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(programs[pidx], sizeof(msg), NULL, msg);
        glDeleteProgram(programs[pidx]);
        Sys_Error("Could not link shader program #%d:\n%s", pidx, msg);
    }
}

void QGL_FreeShaders(void) {
    if (shaders_loaded) {
        for (int i = 0; i < 9; i++)
            glDeleteProgram(programs[i]);
        for (int i = 0; i < 9; i++)
            glDeleteShader(fs[i]);
        for (int i = 0; i < 4; i++)
            glDeleteShader(vs[i]);
        shaders_loaded = false;
    }
}

void QGL_ReloadShaders(void) {
    // glFinish( );
    QGL_FreeShaders();

    CompileShader(QGL_SHADER_MODULATE_SRC, QGL_SHADER_MODULATE, GL_TRUE);
    CompileShader(QGL_SHADER_MODULATE_COLOR_SRC, QGL_SHADER_MODULATE_COLOR, GL_TRUE);
    CompileShader(QGL_SHADER_REPLACE_SRC, QGL_SHADER_REPLACE, GL_TRUE);
    CompileShader(QGL_SHADER_MODULATE_A_SRC, QGL_SHADER_MODULATE_A, GL_TRUE);
    CompileShader(QGL_SHADER_MODULATE_COLOR_A_SRC, QGL_SHADER_MODULATE_COLOR_A, GL_TRUE);
    CompileShader(QGL_SHADER_REPLACE_A_SRC, QGL_SHADER_REPLACE_A, GL_TRUE);
    CompileShader(QGL_SHADER_TEXTURE2D_SRC, QGL_SHADER_TEXTURE2D, GL_FALSE);
    CompileShader(QGL_SHADER_TEXTURE2D_WITH_COLOR_SRC, QGL_SHADER_TEXTURE2D_WITH_COLOR, GL_FALSE);
    CompileShader(QGL_SHADER_RGBA_COLOR_SRC, QGL_SHADER_RGBA_COLOR, GL_TRUE);
    CompileShader(QGL_SHADER_MONO_COLOR_SRC, QGL_SHADER_MONO_COLOR, GL_TRUE);
    CompileShader(QGL_SHADER_RGBA_A_SRC, QGL_SHADER_RGBA_A, GL_TRUE);
    CompileShader(QGL_SHADER_COLOR_SRC, QGL_SHADER_COLOR, GL_FALSE);
    CompileShader(QGL_SHADER_VERTEX_ONLY_SRC, QGL_SHADER_VERTEX_ONLY, GL_FALSE);

    LinkShader(QGL_SHADER_TEX2D_REPL, QGL_SHADER_REPLACE, QGL_SHADER_TEXTURE2D, GL_TRUE, GL_FALSE);
    LinkShader(QGL_SHADER_TEX2D_MODUL, QGL_SHADER_MODULATE, QGL_SHADER_TEXTURE2D, GL_TRUE, GL_FALSE);
    LinkShader(QGL_SHADER_TEX2D_MODUL_CLR, QGL_SHADER_MODULATE_COLOR, QGL_SHADER_TEXTURE2D_WITH_COLOR, GL_TRUE, GL_TRUE);
    LinkShader(QGL_SHADER_RGBA_COLOR, QGL_SHADER_RGBA_COLOR, QGL_SHADER_COLOR, GL_FALSE, GL_TRUE);
    LinkShader(QGL_SHADER_NO_COLOR, QGL_SHADER_MONO_COLOR, QGL_SHADER_VERTEX_ONLY, GL_FALSE, GL_FALSE);
    LinkShader(QGL_SHADER_TEX2D_REPL_A, QGL_SHADER_REPLACE_A, QGL_SHADER_TEXTURE2D, GL_TRUE, GL_FALSE);
    LinkShader(QGL_SHADER_TEX2D_MODUL_A, QGL_SHADER_MODULATE_A, QGL_SHADER_TEXTURE2D, GL_TRUE, GL_FALSE);
    LinkShader(QGL_SHADER_FULL_A, QGL_SHADER_MODULATE_COLOR_A, QGL_SHADER_TEXTURE2D_WITH_COLOR, GL_TRUE, GL_TRUE);
    LinkShader(QGL_SHADER_RGBA_CLR_A, QGL_SHADER_RGBA_A, QGL_SHADER_COLOR, GL_FALSE, GL_TRUE);

    for (GLuint p = 0; p < 9; ++p)
        u_mvp[p] = glGetUniformLocation(programs[p], "uMVP");
    u_modcolor[0] = glGetUniformLocation(programs[QGL_SHADER_TEX2D_MODUL], "uColor");
    u_modcolor[1] = glGetUniformLocation(programs[QGL_SHADER_TEX2D_MODUL_A], "uColor");
    u_monocolor = glGetUniformLocation(programs[QGL_SHADER_NO_COLOR], "uColor");
    // uTexture should be 0 by default, so it should be fine

    shaders_loaded = true;
}

void QGL_SetShader(void) {
    qboolean changed = true;
    just_color = false;

    switch (state_mask + texenv_mask) {
    case 0x00: // Everything off
    case 0x04: // Modulate
    case 0x08: // Alpha Test
    case 0x0C: // Alpha Test + Modulate
        just_color = true;
        glUseProgram(programs[QGL_SHADER_NO_COLOR]);
        cur_program = QGL_SHADER_NO_COLOR;
        break;
    case 0x01: // Texcoord
    case 0x03: // Texcoord + Color
        glUseProgram(programs[QGL_SHADER_TEX2D_REPL]);
        cur_program = QGL_SHADER_TEX2D_REPL;
        break;
    case 0x02: // Color
    case 0x06: // Color + Modulate
        glUseProgram(programs[QGL_SHADER_RGBA_COLOR]);
        cur_program = QGL_SHADER_RGBA_COLOR;
        break;
    case 0x05: // Modulate + Texcoord
        glUseProgram(programs[QGL_SHADER_TEX2D_MODUL]);
        cur_program = QGL_SHADER_TEX2D_MODUL;
        break;
    case 0x07: // Modulate + Texcoord + Color
        glUseProgram(programs[QGL_SHADER_TEX2D_MODUL_CLR]);
        cur_program = QGL_SHADER_TEX2D_MODUL_CLR;
        break;
    case 0x09: // Alpha Test + Texcoord
    case 0x0B: // Alpha Test + Color + Texcoord
        glUseProgram(programs[QGL_SHADER_TEX2D_REPL_A]);
        cur_program = QGL_SHADER_TEX2D_REPL_A;
        break;
    case 0x0A: // Alpha Test + Color
    case 0x0E: // Alpha Test + Modulate + Color
        glUseProgram(programs[QGL_SHADER_RGBA_CLR_A]);
        cur_program = QGL_SHADER_RGBA_CLR_A;
        break;
    case 0x0D: // Alpha Test + Modulate + Texcoord
        glUseProgram(programs[QGL_SHADER_TEX2D_MODUL_A]);
        cur_program = QGL_SHADER_TEX2D_MODUL_A;
        break;
    case 0x0F: // Alpha Test + Modulate + Texcoord + Color
        glUseProgram(programs[QGL_SHADER_FULL_A]);
        cur_program = QGL_SHADER_FULL_A;
        break;
    default:
        changed = false;
        break;
    }
    if (changed) {
        program_changed = true;
        // printf("PROGRAM CHANGE %u -> %u\n", old_program, cur_program);
    }
}

void QGL_EnableGLState(int state) {
    switch (state) {
    case GL_TEXTURE_2D:
        if (!texcoord_state) {
            state_mask += 0x01 + 0x02; // also enable color
            color_state = 1;
            texcoord_state = 1;
        }
        break;
    case GL_MODULATE:
        texenv_mask = 0x04;
        break;
    case GL_REPLACE:
        texenv_mask = 0;
        break;
    case GL_ALPHA_TEST:
        if (!alpha_state) {
            state_mask += 0x08;
            alpha_state = 1;
        }
        break;
    default:
        return;
    }
    QGL_SetShader();
}

void QGL_DisableGLState(int state) {
    switch (state) {
    case GL_TEXTURE_2D:
        if (texcoord_state) {
            state_mask -= 0x01 + 0x02; // also disable color
            color_state = 0;
            texcoord_state = 0;
        }
        break;
    case GL_ALPHA_TEST:
        if (alpha_state) {
            state_mask -= 0x08;
            alpha_state = 0;
        }
        break;
    default:
        return;
    }
    QGL_SetShader();
}

void QGL_DrawGLPoly(GLenum prim, int num) {
    // draw shit ?
}

// HACKHACKHACK: GL function wrappers to go with this shit and a whole GL1 IMM
// emulator because fuck you mesa

static mat4 m_modelview = GLM_MAT4_IDENTITY_INIT;
static mat4 m_projection = GLM_MAT4_IDENTITY_INIT;
static mat4 m_mvp = GLM_MAT4_IDENTITY_INIT;
static mat4 *m_selected = &m_projection;
static mat4 m_stack_mv[QGL_MSTACKLEN] = {GLM_MAT4_IDENTITY_INIT};
static mat4 m_stack_p[QGL_MSTACKLEN] = {GLM_MAT4_IDENTITY_INIT};
static int m_stack_mvn = 1;
static int m_stack_pn = 1;
static qboolean mvp_modified = true;

#pragma pack(push, 1)
typedef struct {
    GLfloat pos[3];
    GLfloat uv[2];
    GLfloat color[4];
} bufvert_t;
#pragma pack(pop)

static int imm_mode = -1;
static int imm_numverts = 0;
static bufvert_t *imm_vertbuf = NULL;
static bufvert_t *imm_vertp = NULL;

qboolean QGL_Init(void) {
    imm_vertbuf = calloc(QGL_MAXVERTS, sizeof(bufvert_t));
    if (!imm_vertbuf)
        return false;
    imm_vertp = imm_vertbuf;

    imm_mode = -1;
    imm_numverts = 0;

    QGL_ReloadShaders();

    glActiveTexture(GL_TEXTURE0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return true;
}

void QGL_Deinit(void) {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    QGL_FreeShaders();
    free(imm_vertbuf);
    imm_vertp = imm_vertbuf = NULL;
}

void qglBegin(GLenum prim) {
    if (!imm_vertbuf)
        Sys_Error("what the hell");

    imm_vertp = imm_vertbuf;
    imm_mode = prim;
    imm_numverts = 0;
}

void qglEnd(void) {
    if (mvp_modified)
        glm_mat4_mul(m_projection, m_modelview, m_mvp);

    if (program_changed || mvp_modified)
        glUniformMatrix4fv(u_mvp[cur_program], 1, GL_FALSE, (GLfloat *)m_mvp);

    program_changed = false;
    mvp_modified = false;

    if (cur_program == QGL_SHADER_TEX2D_MODUL)
        glUniform4fv(u_modcolor[0], 1, cur_color);
    else if (cur_program == QGL_SHADER_TEX2D_MODUL_A)
        glUniform4fv(u_modcolor[1], 1, cur_color);
    else if (just_color)
        glUniform4fv(u_monocolor, 1, cur_color);

    if (imm_mode > -1 && imm_numverts) {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(bufvert_t), &(imm_vertbuf[0].pos[0]));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(bufvert_t), &(imm_vertbuf[0].uv[0]));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(bufvert_t), &(imm_vertbuf[0].color[0]));
        glDrawArrays(imm_mode, 0, imm_numverts);
    }

    imm_vertp = imm_vertbuf;
    imm_numverts = 0;
    imm_mode = -1;
}

void qglVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    imm_vertp->pos[0] = x;
    imm_vertp->pos[1] = y;
    imm_vertp->pos[2] = z;
    imm_vertp->uv[0] = cur_texcoord[0];
    imm_vertp->uv[1] = cur_texcoord[1];
    imm_vertp->color[0] = cur_color[0];
    imm_vertp->color[1] = cur_color[1];
    imm_vertp->color[2] = cur_color[2];
    imm_vertp->color[3] = cur_color[3];
    imm_vertp++;
    imm_numverts++;
}

void qglVertex3fv(GLfloat *v) { qglVertex3f(v[0], v[1], v[2]); }

void qglVertex2f(GLfloat x, GLfloat y) { qglVertex3f(x, y, 0.f); }

void qglVertex2fv(GLfloat *v) { qglVertex3f(v[0], v[1], 0.f); }

void qglTexCoord2f(GLfloat s, GLfloat t) {
    cur_texcoord[0] = s;
    cur_texcoord[1] = t;
}

void qglTexCoord2fv(GLfloat *v) {
    cur_texcoord[0] = v[0];
    cur_texcoord[1] = v[1];
}

void qglColor3f(GLfloat r, GLfloat g, GLfloat b) {
    cur_color[0] = r;
    cur_color[1] = g;
    cur_color[2] = b;
    cur_color[3] = 1.f;
}

void qglColor3fv(GLfloat *v) {
    cur_color[0] = v[0];
    cur_color[1] = v[1];
    cur_color[2] = v[2];
    cur_color[3] = 1.f;
}

void qglColor3ub(GLubyte r, GLubyte g, GLubyte b) {
    cur_color[0] = r / 255.f;
    cur_color[1] = g / 255.f;
    cur_color[2] = b / 255.f;
    cur_color[3] = 1.f;
}

void qglColor3ubv(GLubyte *v) {
    cur_color[0] = v[0] / 255.f;
    cur_color[1] = v[1] / 255.f;
    cur_color[2] = v[2] / 255.f;
    cur_color[3] = 1.f;
}

void qglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    cur_color[0] = r;
    cur_color[1] = g;
    cur_color[2] = b;
    cur_color[3] = a;
}

void qglColor4fv(GLfloat *v) {
    cur_color[0] = v[0];
    cur_color[1] = v[1];
    cur_color[2] = v[2];
    cur_color[3] = v[3];
}

void qglColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
    cur_color[0] = r / 255.f;
    cur_color[1] = g / 255.f;
    cur_color[2] = b / 255.f;
    cur_color[3] = a / 255.f;
}

void qglColor4ubv(GLubyte *v) {
    cur_color[0] = v[0] / 255.f;
    cur_color[1] = v[1] / 255.f;
    cur_color[2] = v[2] / 255.f;
    cur_color[3] = v[3] / 255.f;
}

void qglShadeModel(GLenum type) {
    // boob
}

void qglGetFloatv(GLenum param, GLfloat *v) {
    if (param == GL_MODELVIEW_MATRIX)
        memcpy(v, m_modelview, sizeof(GLfloat) * 16);
    else
        glGetFloatv(param, v);
}

GLboolean qglIsEnabled(GLenum thing) {
    if (thing == GL_ALPHA_TEST)
        return alpha_state;
    return glIsEnabled(thing);
}

void qglTexEnvi(GLenum target, GLenum pname, GLint param) {
    if (param == GL_MODULATE || param == GL_REPLACE)
        QGL_EnableGLState(param);
}

void qglEnable(GLenum param) {
    if (param == GL_ALPHA_TEST || param == GL_TEXTURE_2D)
        QGL_EnableGLState(param);
    else
        glEnable(param);
}

void qglDisable(GLenum param) {
    if (param == GL_ALPHA_TEST || param == GL_TEXTURE_2D)
        QGL_DisableGLState(param);
    else
        glDisable(param);
}

void qglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
                   GLenum format, GLenum type, const GLvoid *pixels) {
    uint8_t *in = (uint8_t *)pixels;

    if (internalformat == GL_RGB && format == GL_RGBA) // strip alpha from texture
    {
        int i = 0, size = width * height * 4;
        for (i = 0; i < size; i += 4, in += 4)
            in[3] = 255;
    }

    internalformat = format;
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

// matrix shit

void qglMatrixMode(GLenum mode) {
    if (mode == GL_PROJECTION)
        m_selected = &m_projection;
    else if (mode == GL_MODELVIEW)
        m_selected = &m_modelview;
}

void qglLoadIdentity(void) {
    glm_mat4_identity(*m_selected);
    mvp_modified = true;
}

void qglPushMatrix(void) {
    if (m_selected == &m_modelview) {
        if (m_stack_mvn == QGL_MSTACKLEN)
            Sys_Error("qglPushMatrix(): out of stack space!");
        glm_mat4_copy(*m_selected, m_stack_mv[m_stack_mvn]);
        m_stack_mvn++;
    } else if (m_selected == &m_projection) {
        if (m_stack_pn == QGL_MSTACKLEN)
            Sys_Error("qglPushMatrix(): out of stack space!");
        glm_mat4_copy(*m_selected, m_stack_p[m_stack_pn]);
        m_stack_pn++;
    }
}

void qglPopMatrix(void) {
    if (m_selected == &m_modelview) {
        if (m_stack_mvn == 0)
            Sys_Error("qglPopMatrix(): pop from empty stack!");
        glm_mat4_copy(m_stack_mv[m_stack_mvn], *m_selected);
        m_stack_mvn--;
    } else if (m_selected == &m_projection) {
        if (m_stack_pn == QGL_MSTACKLEN)
            Sys_Error("qglPushMatrix(): out of stack space!");
        glm_mat4_copy(m_stack_p[m_stack_pn], *m_selected);
        m_stack_pn--;
    }
    mvp_modified = true;
}

void qglLoadMatrixf(GLfloat *m) {
    memcpy(*m_selected, m, sizeof(GLfloat) * 16);
    mvp_modified = true;
}

void qglTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    glm_translate(*m_selected, (vec3){x, y, z});
    mvp_modified = true;
}

void qglRotatef(GLfloat a, GLfloat x, GLfloat y, GLfloat z) {
    glm_make_rad(&a);
    glm_rotate(*m_selected, a, (vec3){x, y, z});
    mvp_modified = true;
}

void qglScalef(GLfloat x, GLfloat y, GLfloat z) {
    glm_scale(*m_selected, (vec3){x, y, z});
    mvp_modified = true;
}

void qglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far) {
    glm_frustum(left, right, bottom, top, near, far, *m_selected);
    mvp_modified = true;
}

void qglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far) {
    glm_ortho(left, right, bottom, top, near, far, *m_selected);
    mvp_modified = true;
}

// stubs

void qglAlphaFunc(GLenum a, GLfloat b) {}
void qglPolygonMode(GLenum a, GLenum b) {}
