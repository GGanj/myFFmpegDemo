#include "Shader430.h"

Shader430::Shader430() {
    _vHandle    =   -1;
    _pHandle    =   -1;
    _program    =   -1;
    remove("error1.txt");
    remove("error2.txt");
}
Shader430::~Shader430() {

}

bool Shader430::createProgram(const char* vs, const char* ps) {
    bool    result = false;
    do
    {
        ///////////////////////////////////
        ///////////创建shader
        _vHandle = glCreateShader(GL_VERTEX_SHADER);
        _pHandle = glCreateShader(GL_FRAGMENT_SHADER);

        ///////////////////////////////////
        ///////////指定shader的源代码
        glShaderSource(_vHandle, 1, (const GLchar**)&vs, 0);
        glShaderSource(_pHandle, 1, (const GLchar**)&ps, 0);

        ///////////////////////////////////
        ///////////编译shader
        glCompileShader(_vHandle);
        GLint   status = 0;
        char    compileLog[1024] = { 0 };
        glGetShaderiv(_vHandle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            glGetShaderInfoLog(_vHandle, sizeof(compileLog), 0, compileLog);
            FILE* fp = fopen("error1.txt", "wb+");
            fprintf(fp, compileLog);
            fclose(fp);
            fp = NULL;
            assert(false && compileLog);
            break;
        }

        glCompileShader(_pHandle);

        glGetShaderiv(_pHandle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            glGetShaderInfoLog(_pHandle, sizeof(compileLog), 0, compileLog);
            FILE* fp = fopen("error2.txt", "wb+");
            fprintf(fp, compileLog);
            fclose(fp);
            fp = NULL;
            assert(false && compileLog);
            break;
        }
        /////////////////////////////////////////
        /////////创建Program
        _program = glCreateProgram();
        glAttachShader(_program, _vHandle);
        glAttachShader(_program, _pHandle);

        //////////////////////////////////////////
        /////////链接程序
        glLinkProgram(_program);

        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            glGetProgramInfoLog(_program, sizeof(compileLog), 0, compileLog);
            break;
        }
        result = true;

    } while (false);

    if (!result)
    {
        if (_vHandle != -1)
        {
            glDeleteShader(_vHandle);
        }
        if (_pHandle != -1)
        {
            glDeleteShader(_pHandle);
        }
        if (_program != -1)
        {
            glDeleteProgram(_program);
        }
        _vHandle = -1;
        _pHandle = -1;
        _program = -1;
    }
    return  result;
}

void Shader430::initialize()
{
    const char* vs = "#version 430\n\
                     struct VertexA\n\
                     {\n\
                        vec2	_position; \n\
                        vec2	_uv; \n\
                     }; \n\
                     in  VertexA   _va; \n\
                     layout(location = 0) uniform	mat4		_matrix;\n\
                     layout(location = 0) out		vec2		_OutColor;\n\
                     void main()\n\
                     {\n\
                        _OutColor   =  _va._uv;\n\
                        gl_Position = _matrix*vec4(_va._position,0.0,1.0);\n\
                     }";

    const char* ps = " \
                    #version 330\n\
                    in	vec2 _OutColor;\
                    uniform sampler2D _textureY;\
                    uniform sampler2D _textureU;\
                    uniform sampler2D _textureV;\
                    void main()\
                    {\
                      vec3 yuv;\
                      vec3 rgb;\
                      yuv.x = texture2D(_textureY, _OutColor).a;\
                      yuv.y = texture2D(_textureU, _OutColor).a - 0.5;\
                      yuv.z = texture2D(_textureV, _OutColor).a - 0.5;\
                      rgb = mat3( 1,              1,       1,\
                                  0,       -0.39465, 2.03210,\
                                  1.13983, -0.58060,       0) * yuv;\
                      gl_FragColor = vec4(rgb, 1);\
                     }";

    createProgram(vs, ps);
    _textureY = glGetUniformLocation(_program, "_textureY");
    _textureU = glGetUniformLocation(_program, "_textureU");
    _textureV = glGetUniformLocation(_program, "_textureV");
}

void Shader430::begin()
{
    glUseProgram(_program);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void Shader430::end()
{
    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
