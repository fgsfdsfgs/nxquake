/*
 * MIT License
 *
 * CGLM Copyright (c) 2016 Sebastien Serre <ssbx@sysmo.io>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @mainpage
 * All CGLM documentation is included ini the main cglm.h file.
 */

/**
 * @file cglm.h
 * @brief Cglm
 */
#ifndef CGLM_H
#define CGLM_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    /**
     * @brief represent a 4x4 matrix.
     *
     *       a0   a1  a2   a3 <br>
     *
     *       b0   b1  b2   b3 <br>
     *
     *       c0   c1  c2   c3 <br>
     *
     *       d0   d1  d2   d3 <br>
     *
     */
    typedef struct {
        GLfloat a0, a1, a2, a3,
        b0, b1, b2, b3,
        c0, c1, c2, c3,
        d0, d1, d2, d3;
    } CGLMmat4;

    /**
     * @brief represent a 3 dimention vertex.
     */
    typedef struct {
        GLfloat x, y, z;
    } CGLMvec3;

    static const CGLMmat4
    empty_matrix_4 = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    /**
     * @brief create a 4x4 matrix
     * @return CLGMmat4
     */
    static CGLMmat4
    cglmMat4(GLfloat num) {
        CGLMmat4 result = empty_matrix_4;
        result.a0 = num;
        result.b1 = num;
        result.c2 = num;
        result.d3 = num;
        return result;
    }

    /**
     * @brief Normalize a vector.
     * @return normalized GLMvec3
     */
    static CGLMvec3
    cglmNormalize(CGLMvec3 vector) {
        GLfloat x = vector.x * vector.x;
        GLfloat y = vector.y * vector.y;
        GLfloat z = vector.z * vector.z;
        GLfloat all = (GLfloat) sqrt(x + y + z);

        CGLMvec3 result;
        result.x = vector.x / all;
        result.y = vector.y / all;
        result.z = vector.z / all;

        return result;
    }

    /**
     * @brief subsctract vector b from vector a.
     * @return the resulting vector
     */
    static CGLMvec3
    cglmSubsVec3(
            CGLMvec3 a,
            CGLMvec3 b) {
        CGLMvec3 result;
        result.x = a.x + (-b.x);
        result.y = a.y + (-b.y);
        result.z = a.z + (-b.z);

        return result;
    }

    /**
     * @brief Cross two vectors
     * @return the resulting vector
     */
    static CGLMvec3
    cglmCross(
            CGLMvec3 x,
            CGLMvec3 y) {
        CGLMvec3 result;
        result.x = x.y * y.z - y.y * x.z;
        result.y = x.z * y.x - y.z * x.x;
        result.z = x.x * y.y - y.x * x.y;

        return result;
    }

    /**
     * @brief compute the dot value of two vectors.
     * @return the dot value
     */
    static GLfloat
    cglmDot(CGLMvec3 a, CGLMvec3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    /**
     * @brief Creates a matrix for a symetric perspective-view frustum based on
     *        the default handedness.
     * @param fovy Specifies the field of view angle in the y direction.
     *             Expressed in radians.
     * @param aspect Specifies the aspect ratio that determines the field of view
     *               in the x direction. The aspect ratio is the ratio of
     *               x (width) to y (height).
     * @param near Specifies the distance from the viewer to the near clipping
     *             plane (always positive).
     * @param far Specifies the distance from the viewer to the far clipping
     *            plane (always positive).
     */
    static CGLMmat4
    cglmPerspective(
            GLfloat fovy,
            GLfloat aspect,
            GLfloat zNear,
            GLfloat zFar) {
        GLfloat tanHalfFovy = (GLfloat) tan(fovy / (GLfloat) 2);

        CGLMmat4 result = cglmMat4(0.0f);
        result.a0 = (GLfloat) 1 / (aspect * tanHalfFovy);
        result.b1 = (GLfloat) 1 / (tanHalfFovy);
        result.c2 = -(zFar + zNear) / (zFar - zNear);
        result.c3 = -(GLfloat) 1;
        result.d2 = -((GLfloat) 2 * zFar * zNear) / (zFar - zNear);

        return result;
    }

    /**
     * @brief Creates a matrix for an orthographic parallel viewing volume.
     * @param left
     * @param right
     * @param bottom
     * @param top
     * @param zNear
     * @param zFar
     */
    static CGLMmat4
    cglmOrtho(
            GLfloat left,
            GLfloat right,
            GLfloat bottom,
            GLfloat top,
            GLfloat zNear,
            GLfloat zFar) {
        CGLMmat4 result = cglmMat4(1.0f);
        result.a0 = (GLfloat) 2 / (right - left);
        result.b1 = (GLfloat) 2 / (top - bottom);
        result.c2 = -(GLfloat) 2 / (zFar - zNear);
        result.d0 = -(right + left) / (right - left);
        result.d1 = -(top + bottom) / (top - bottom);
        result.d2 = -(zFar + zNear) / (zFar - zNear);

        return result;
    }

    /**
     * @brief Creates a frustum matrix.
     * @param left
     * @param right
     * @param bottom
     * @param top
     * @param near
     * @param far
     */
    static CGLMmat4
    cglmFrustum(
            GLfloat left,
            GLfloat right,
            GLfloat bottom,
            GLfloat top,
            GLfloat zNear,
            GLfloat zFar) {
        CGLMmat4 result = cglmMat4(0.0f);
        result.a0 = ((GLfloat) 2 * zNear) / (right - left);
        result.b1 = ((GLfloat) 2 * zNear) / (top - bottom);
        result.c0 = (right + left) / (right - left);
        result.c1 = (top + bottom) / (top - bottom);
        result.c2 = -(zFar + zNear) / (zFar - zNear);
        result.c3 = -(GLfloat) 1;
        result.d2 = -((GLfloat) 2 * zFar * zNear) / (zFar - zNear);

        return result;
    }

    /**
     * @brief Build a look at view matrix based on the default handedness.
     * @param eye Position of the camera
     * @param center Position where the camera is looking at
     * @param up Normalized up vector, how the camera is oriented.
     *           Typically (0, 0, 1)
     */
    static CGLMmat4
    cglmLookAt(
            CGLMvec3 eye,
            CGLMvec3 center,
            CGLMvec3 up) {
        const CGLMvec3 f = cglmNormalize(cglmSubsVec3(center, eye));
        const CGLMvec3 s = cglmNormalize(cglmCross(f, up));
        const CGLMvec3 u = cglmCross(s, f);

        CGLMmat4 result = cglmMat4(1.0f);
        result.a0 = s.x;
        result.b0 = s.y;
        result.c0 = s.z;
        result.a1 = u.x;
        result.b1 = u.y;
        result.c1 = u.z;
        result.a2 = -f.x;
        result.b2 = -f.y;
        result.c2 = -f.z;
        result.d0 = -cglmDot(s, eye);
        result.d1 = -cglmDot(u, eye);
        result.d2 = cglmDot(f, eye);

        return result;
    }

    /**
     * @brief multiply vector by a scalar
     * @return a vector
     */
    static CGLMvec3
    cglmScalarMultVec3(
            CGLMvec3 v,
            float s) {
        CGLMvec3 result;
        result.x = v.x * s;
        result.y = v.y * s;
        result.z = v.z * s;

        return result;
    }

    /**
     * @brief multiply matrice by a scalar
     * @return a vector
     */
    static CGLMmat4
    cglmScalarMultMat4(
            CGLMmat4 m,
            float s) {
        CGLMmat4 result = {
            m.a0 * s, m.a1 * s, m.a2 * s, m.a3 * s,
            m.b0 * s, m.b1 * s, m.b2 * s, m.b3 * s,
            m.c0 * s, m.c1 * s, m.c2 * s, m.c3 * s,
            m.d0 * s, m.d1 * s, m.d2 * s, m.d3 * s
        };

        return result;
    }

    /**
     * @brief add vectors
     * @return a vector
     */
    static CGLMvec3
    cglmAddVec3(
            CGLMvec3 a,
            CGLMvec3 b) {
        CGLMvec3 result;
        result.x = a.x + b.x;
        result.y = a.y + b.y;
        result.z = a.z + b.z;

        return result;
    }

    /**
     * @brief multiply matrix m1 by matrix M2
     * @return a matrix
     */
    static CGLMmat4
    cglmMultMat4(
            CGLMmat4 m1,
            CGLMmat4 m2) {
        CGLMmat4 result = {
            m2.a0 * m1.a0 + m2.a1 * m1.b0 + m2.a2 * m1.c0 + m2.a3 * m1.d0, // = a0
            m2.a0 * m1.a1 + m2.a1 * m1.b1 + m2.a2 * m1.c1 + m2.a3 * m1.d1, // = a1
            m2.a0 * m1.a2 + m2.a1 * m1.b2 + m2.a2 * m1.c2 + m2.a3 * m1.d2, // = a2
            m2.a0 * m1.a3 + m2.a1 * m1.b3 + m2.a2 * m1.c3 + m2.a3 * m1.d3, // = a3

            m2.b0 * m1.a0 + m2.b1 * m1.b0 + m2.b2 * m1.c0 + m2.b3 * m1.d0, // = b0
            m2.b0 * m1.a1 + m2.b1 * m1.b1 + m2.b2 * m1.c1 + m2.b3 * m1.d1, // = b1
            m2.b0 * m1.a2 + m2.b1 * m1.b2 + m2.b2 * m1.c2 + m2.b3 * m1.d2, // = b2
            m2.b0 * m1.a3 + m2.b1 * m1.b3 + m2.b2 * m1.c3 + m2.b3 * m1.d3, // = b3

            m2.c0 * m1.a0 + m2.c1 * m1.b0 + m2.c2 * m1.c0 + m2.c3 * m1.d0, // = c0
            m2.c0 * m1.a1 + m2.c1 * m1.b1 + m2.c2 * m1.c1 + m2.c3 * m1.d1, // = c1
            m2.c0 * m1.a2 + m2.c1 * m1.b2 + m2.c2 * m1.c2 + m2.c3 * m1.d2, // = c2
            m2.c0 * m1.a3 + m2.c1 * m1.b3 + m2.c2 * m1.c3 + m2.c3 * m1.d3, // = c3

            m2.d0 * m1.a0 + m2.d1 * m1.b0 + m2.d2 * m1.c0 + m2.d3 * m1.d0, // = d0
            m2.d0 * m1.a1 + m2.d1 * m1.b1 + m2.d2 * m1.c1 + m2.d3 * m1.d1, // = d1
            m2.d0 * m1.a2 + m2.d1 * m1.b2 + m2.d2 * m1.c2 + m2.d3 * m1.d2, // = d2
            m2.d0 * m1.a3 + m2.d1 * m1.b3 + m2.d2 * m1.c3 + m2.d3 * m1.d3, // = d3
        };

        return result;
    }

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CGLM_H

