#include "Shader330.h"
void Shader330::initialize()
{
    const char* vs = "\
    #version 330\n\
    in vec2 _position;\
    in vec2 _uv;\
    uniform mat4 _matrix;\
    out vec2 _outColor;\
    void main() {\
        _outColor = _uv;\
        gl_Position = _matrix * vec4(_position, 0.0, 1.0);\
    }";
    const char* ps = "\
    #version 330\n\
    uniform sampler2D _texture;\
    in vec2 _outColor;\
    void main() {\
        gl_FragColor = texture2D(_texture, _outColor);\
    }";
    createProgram(vs, ps);
    _position = glGetAttribLocation(_program, "_position");
    _uv = glGetAttribLocation(_program, "_uv");
    _texture = glGetUniformLocation(_program, "_texture");
    _matrix = glGetUniformLocation(_program, "_matrix");
}

void Shader330::begin()
{
    glUseProgram(_program);
}

void Shader330::end()
{
    glUseProgram(0);
}