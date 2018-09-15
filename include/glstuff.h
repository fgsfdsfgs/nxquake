#ifndef __GLSTUFF_H
#define __GLSTUFF_H

#include "quakedef.h"

#define QGL_MAXVERTS 16384
#define QGL_MSTACKLEN 8

// internal shader shit

// fragment shaders
#define QGL_SHADER_MODULATE_COLOR 0
#define QGL_SHADER_MODULATE 1
#define QGL_SHADER_REPLACE 2
#define QGL_SHADER_RGBA_COLOR 3
#define QGL_SHADER_MONO_COLOR 4
#define QGL_SHADER_MODULATE_COLOR_A 5
#define QGL_SHADER_MODULATE_A 6
#define QGL_SHADER_RGBA_A 7
#define QGL_SHADER_REPLACE_A 8
// vertex shaders
#define QGL_SHADER_TEXTURE2D 0
#define QGL_SHADER_TEXTURE2D_WITH_COLOR 1
#define QGL_SHADER_COLOR 2
#define QGL_SHADER_VERTEX_ONLY 3
// shader programs
#define QGL_SHADER_TEX2D_REPL 0
#define QGL_SHADER_TEX2D_MODUL 1
#define QGL_SHADER_TEX2D_MODUL_CLR 2
// #define QGL_SHADER_RGBA_COLOR        3  // already defined above
#define QGL_SHADER_NO_COLOR 4
#define QGL_SHADER_TEX2D_REPL_A 5
#define QGL_SHADER_TEX2D_MODUL_A 6
#define QGL_SHADER_RGBA_CLR_A 7
#define QGL_SHADER_FULL_A 8

// gl1 imm shit

typedef double GLdouble;

#define GL_PERSPECTIVE_CORRECTION_HINT 0 // get that stupid shit out of the way

#define GL_TEXTURE0_ARB GL_TEXTURE0
#define GL_TEXTURE1_ARB GL_TEXTURE1
#define GL_POLYGON GL_TRIANGLE_FAN
#define GL_QUADS GL_TRIANGLE_FAN

#define GL_COLOR 0x1800
#define GL_SMOOTH 0x1D01
#define GL_FLAT 0x1D00
#define GL_MODULATE 0x2100
#define GL_DECAL 0x2101
#define GL_ALPHA_TEST 0x0BC0
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_INTENSITY 0x8049
#define GL_LUMINANCE 0x1909
#define GL_MATRIX_MODE 0x0BA0
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_MODELVIEW_MATRIX 0x0BA6
#define GL_MAX_TEXTURE_UNITS 0x84E2

qboolean QGL_Init(void);
void QGL_Deinit(void);
void QGL_EndFrame(void);

void qglBegin(GLenum prim);
void qglEnd(void);

void qglVertex3f(GLfloat x, GLfloat y, GLfloat z);
void qglVertex3fv(GLfloat* v);
void qglVertex2f(GLfloat x, GLfloat y);
void qglVertex2fv(GLfloat* v);

void qglTexCoord2f(GLfloat s, GLfloat t);
void qglTexCoord2fv(GLfloat* v);

void qglColor3f(GLfloat r, GLfloat g, GLfloat b);
void qglColor3fv(GLfloat* v);
void qglColor3ub(GLubyte r, GLubyte g, GLubyte b);
void qglColor3ubv(GLubyte* v);
void qglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void qglColor4fv(GLfloat* v);
void qglColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
void qglColor4ubv(GLubyte* v);

void qglShadeModel(GLenum type);
void qglTexEnvi(GLenum target, GLenum pname, GLint param);
GLboolean qglIsEnabled(GLenum thing);
void qglEnable(GLenum param);
void qglDisable(GLenum param);
void qglGetFloatv(GLenum param, GLfloat* v);

void qglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
    GLenum format, GLenum type, const GLvoid* pixels);

void qglMatrixMode(GLenum mode);
void qglLoadIdentity(void);
void qglPushMatrix(void);
void qglPopMatrix(void);
void qglLoadMatrixf(GLfloat* m);
void qglTranslatef(GLfloat x, GLfloat y, GLfloat z);
void qglRotatef(GLfloat a, GLfloat x, GLfloat y, GLfloat z);
void qglScalef(GLfloat x, GLfloat y, GLfloat z);
void qglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
void qglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);

void qglAlphaFunc(GLenum a, GLfloat b);
void qglPolygonMode(GLenum a, GLenum b);

#define glBegin qglBegin
#define glEnd qglEnd

#define glVertex3f qglVertex3f
#define glVertex3fv qglVertex3fv
#define glVertex2f qglVertex2f
#define glVertex2fv qglVertex2fv

#define glTexCoord2f qglTexCoord2f
#define glTexCoord2fv qglTexCoord2fv

#define glColor3f qglColor3f
#define glColor3fv qglColor3fv
#define glColor3ub qglColor3ub
#define glColor3ubv qglColor3ubv
#define glColor4f qglColor4f
#define glColor4fv qglColor4fv
#define glColor4ub qglColor4ub
#define glColor4ubv qglColor4ubv

#define glShadeModel qglShadeModel
#define glTexEnvf qglTexEnvi // this is only used with integer args
#define glTexEnvi qglTexEnvi

#define glMatrixMode qglMatrixMode
#define glLoadIdentity qglLoadIdentity
#define glLoadMatrixf qglLoadMatrixf
#define glPushMatrix qglPushMatrix
#define glPopMatrix qglPopMatrix
#define glTranslatef qglTranslatef
#define glRotatef qglRotatef
#define glScalef qglScalef
#define glFrustum qglFrustum
#define glOrtho qglOrtho

#define glAlphaFunc qglAlphaFunc

#define glDepthRange glDepthRangef

#endif
