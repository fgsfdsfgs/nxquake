#ifndef __GLSHADERS_H
#define __GLSHADERS_H

//
// vert
//

static const char *QGL_SHADER_TEXTURE2D_SRC = R"(
    precision mediump float;

    attribute vec3 inPos;
    attribute vec2 inTexCoord;
    attribute vec4 inColor;
    uniform mat4 uMVP;

    varying vec2 vTexCoord;

    void main() {
      gl_Position = uMVP * vec4(inPos, 1.0);
      vTexCoord = inTexCoord; 
    }
)";

static const char *QGL_SHADER_TEXTURE2D_WITH_COLOR_SRC = R"(
    precision mediump float;

    attribute vec3 inPos;
    attribute vec2 inTexCoord;
    attribute vec4 inColor;
    uniform mat4 uMVP;

    varying vec2 vTexCoord;
    varying vec4 vColor;

    void main() {
      gl_Position = uMVP * vec4(inPos, 1.0);
      vTexCoord = inTexCoord; 
      vColor = inColor;
    }
)";

static const char *QGL_SHADER_COLOR_SRC = R"(
    precision mediump float;

    attribute vec3 inPos;
    attribute vec2 inTexCoord;
    attribute vec4 inColor;
    uniform mat4 uMVP;

    varying vec4 vColor;

    void main() {
      gl_Position = uMVP * vec4(inPos, 1.0);
      vColor = inColor;
    }
)";

static const char *QGL_SHADER_VERTEX_ONLY_SRC = R"(
    precision mediump float;

    attribute vec3 inPos;
    attribute vec2 inTexCoord;
    attribute vec4 inColor;
    uniform mat4 uMVP;

    void main() {
      gl_Position = uMVP * vec4(inPos, 1.0);
    }
)";


//
// frag
//

static const char *QGL_SHADER_MODULATE_SRC = R"(
    precision mediump float;

    varying vec2 vTexCoord;
    uniform vec4 uColor;
    uniform sampler2D uTexture;

    void main() {
      gl_FragColor = texture2D(uTexture, vTexCoord) * uColor;
    }
)";

static const char *QGL_SHADER_MODULATE_A_SRC = R"(
    precision mediump float;

    varying vec2 vTexCoord;
    uniform vec4 uColor;
    uniform sampler2D uTexture;

    void main() {
      vec4 c = texture2D(uTexture, vTexCoord) * uColor;
      if (c.a < 0.666) discard;
      else gl_FragColor = c;
    }
)";

static const char *QGL_SHADER_MODULATE_COLOR_SRC = R"(
    precision mediump float;

    varying vec2 vTexCoord;
    varying vec4 vColor;
    uniform sampler2D uTexture;

    void main() {
      gl_FragColor = texture2D(uTexture, vTexCoord) * vColor;
    }
)";

static const char *QGL_SHADER_MODULATE_COLOR_A_SRC = R"(
    precision mediump float;

    varying vec2 vTexCoord;
    varying vec4 vColor;
    uniform sampler2D uTexture;

    void main() {
      vec4 c = texture2D(uTexture, vTexCoord) * vColor;
      if (c.a < 0.666) discard;
      else gl_FragColor = c;
    }
)";

static const char *QGL_SHADER_REPLACE_SRC = R"(
    precision mediump float;

    varying vec2 vTexCoord;
    uniform sampler2D uTexture;

    void main() {
      gl_FragColor = texture2D(uTexture, vTexCoord);
    }
)";

static const char *QGL_SHADER_REPLACE_A_SRC = R"(
    precision mediump float;

    varying vec2 vTexCoord;
    uniform sampler2D uTexture;

    void main() {
      vec4 c = texture2D(uTexture, vTexCoord);
      if (c.a < 0.666) discard;
      else gl_FragColor = c;
    }
)";

static const char *QGL_SHADER_RGBA_COLOR_SRC = R"(
    precision mediump float;

    varying vec4 vColor;

    void main() {
      gl_FragColor = vColor;
    }
)";

static const char *QGL_SHADER_RGBA_A_SRC = R"(
    precision mediump float;

    varying vec4 vColor;

    void main() {
      if (vColor.a < 0.666) discard;
      else gl_FragColor = vColor;
    }
)";

static const char *QGL_SHADER_MONO_COLOR_SRC = R"(
    precision mediump float;

    uniform vec4 uColor;

    void main() {
      gl_FragColor = uColor;
    }
)";

#endif