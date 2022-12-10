#pragma once
#include "shader.h"

class Shader330 : public Program
{
public:
    attribute	  _position;
    attribute     _uv;
    uniform       _texture;
    uniform       _matrix;
public:
    virtual void initialize();
    virtual void begin();
    virtual void end();
};

