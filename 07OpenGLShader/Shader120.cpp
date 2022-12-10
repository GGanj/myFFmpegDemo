#include "Shader120.h"
void Shader120::initialize()
{
    const char* vs = "\
    #version 120\n\
    attribute vec2 _position;\
    attribute vec2 _uv;\
    varying vec2 _outColor;\
    void main() {\
        _outColor = _uv;\
        gl_Position = gl_ModelViewProjectionMatrix * vec4(_position, 0.0, 1.0);\
    }";
    const char* ps = "\
    #version 120\n\
    uniform sampler2D _texture;\
    varying vec2 _outColor;\
    void main() {\
        gl_FragColor = texture2D(_texture, _outColor);\
    }";
    createProgram(vs, ps);
    _position = glGetAttribLocation(_program, "_position");
    _uv = glGetAttribLocation(_program, "_uv");
    _texture = glGetUniformLocation(_program, "_texture");
}

void Shader120::begin()
{
    glUseProgram(_program);
}

void Shader120::end()
{
    glUseProgram(0);
}
