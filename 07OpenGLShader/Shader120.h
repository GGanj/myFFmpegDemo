#pragma once
#include "shader.h"

class Shader120 : public Program
{
public:
    attribute	  _position;
    attribute     _uv;
    uniform       _texture;
public:
    virtual void    initialize();
    virtual void    begin();
    virtual void    end();
};

