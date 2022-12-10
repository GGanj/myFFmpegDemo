#include "Shader430.h"
void Shader430::initialize()
{
    const char* vs = "\
    #version 430\n\
    layout(location = 0) in vec2 _position;\
    layout(location = 1) in vec2 _uv;\
    layout(location = 0) uniform mat4 _matrix;\
    layout(location = 0) out vec2 _outColor;\
    void main() {\
        _outColor = _uv;\
        gl_Position = _matrix * vec4(_position, 0.0, 1.0);\
    }";
    const char* ps = "\
    #version 430\n\
    layout(location = 1) uniform sampler2D _texture;\
    layout(location = 0) in vec2 _outColor;\
    void main() {\
        gl_FragColor = texture2D(_texture, _outColor);\
    }";
    createProgram(vs, ps);
}

void Shader430::begin()
{
    glUseProgram(_program);
}

void Shader430::end()
{
    glUseProgram(0);
}