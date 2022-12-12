#pragma once
#include <fstream>
#include <assert.h>

#include <stdlib.h>
#include "GL/glew.h"

class Shader430
{
public:
    Shader430();
    ~Shader430();
    virtual void initialize();
    virtual void begin();
    virtual void end();
protected:
    bool createProgram(const char* vs, const char* ps);
public:
    typedef GLint   attribute;
    typedef GLint   uniform;
public:
    GLint           _vHandle;
    GLint           _pHandle;
    GLint           _program;

    attribute	  _position;
    attribute     _uv;
    uniform       _textureY;
    uniform       _textureU;
    uniform       _textureV;
    uniform       _matrix;


};

