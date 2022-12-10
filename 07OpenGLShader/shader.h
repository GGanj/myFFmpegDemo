#pragma once
#include <fstream>
#include <assert.h>

#include <stdlib.h>
#include "GL/glew.h"

#include "GL/glut.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class   Program
{
public:
    typedef GLint   attribute;
    typedef GLint   uniform;
public:
    GLint           _vHandle;
    GLint           _pHandle;
    GLint           _program;
public:
	Program()
    {
        _vHandle    =   -1;
        _pHandle    =   -1;
        _program    =   -1;
		remove("error1.txt");
		remove("error2.txt");
    }
    virtual ~Program()
    {
    }
    virtual void    initialize()
    {
        const char* vs = "";
        const char* ps = "";
        createProgram(vs, ps);
    }
	///////////////////////////////////
	///////////开始使用shader
    virtual void    begin()
    {
        glUseProgram(_program);
    }
	///////////////////////////////////
	///////////结束使用shader
    virtual void    end()
    {
        glUseProgram(0);
    }
protected:
   
     ///////////////////////////////////
	///////////源代码编译shader
    
    bool    createProgram(const char* vs, const char* ps)
    {
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
                FILE* fp = nullptr;
                fopen_s(&fp, "error1.txt", "wb+");
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
                FILE* fp = nullptr;
                fopen_s(&fp, "error2.txt", "wb+");
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
};


class XShader:public Program
{
public:
	attribute	  _position; 
	attribute     _uv; 
	uniform       _texture; 
	uniform       _matrix;
public:
	//gl_ModelViewProjectionMatrix
	// layout(location = 0)in			vec2		_position;\n\
   //layout(location = 1)in	 		vec2   _uv;
	/*struct Vertex\n\
	{\n\
		in	vec2	_position; \n\
		in	vec2	_uv; \n\
	}; \n\*/
	virtual void    initialize()
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
						
		const char* ps = "#version 430\n\
						 layout(location  = 0) in	vec2		_OutColor;\n\
						 layout(location = 1) uniform sampler2D  _texture;\n\
						 void main()\n\
						 {\n\
							gl_FragColor = texture2D(_texture,_OutColor);\n\
						 }";
						 
		createProgram(vs, ps);
		
	}
	///////////////////////////////////
	///////////开始使用shader
	virtual void    begin()
	{
		glUseProgram(_program);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	///////////////////////////////////
	///////////结束使用shader
	virtual void    end()
	{
		glUseProgram(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

};
