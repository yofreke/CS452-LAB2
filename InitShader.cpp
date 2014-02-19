
#include "Angel.h"

// This file is very similar to initShaders.h, so I will be more brief in the documentation of this file.
namespace Angel {

// Create a NULL-terminated string by reading the provided file
static char*
readShaderSource(const char* shaderFile)
{
	// Open the file with the path provided in mode read ("r")
    FILE* fp = fopen(shaderFile, "r");

    // Check to see if the file exists
    if ( fp == NULL ) { return NULL; }

    // Get the file size
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    // Read the file into a char buffer
    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);

    // Terminate the buffer.
    buf[size] = '\0';
    fclose(fp);

    return buf;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const char* vShaderFile, const char* fShaderFile)
{
	// This is a struct used to contain shader info, the type of shader, and the filename of the shader.  Angel keeps the source in his Shader struct as well.
    struct Shader {
		const char*  filename;
		GLenum       type;
		GLchar*      source;
	};
	shaders[2] = {
		{ vShaderFile, GL_VERTEX_SHADER, NULL },
		{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };

    // Create an OpenGL program that we will use.
    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
    	// Read each shader's source into the structs source field.
		Shader& s = shaders[i];
		s.source = readShaderSource( s.filename );
		if ( shaders[i].source == NULL ) {
		    std::cerr << "Failed to read " << s.filename << std::endl;
		    exit( EXIT_FAILURE );
		}

		// Compile the shader
		GLuint shader = glCreateShader( s.type );
		glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
		glCompileShader( shader );

		// Check for compile errors
		GLint  compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) {
		    std::cerr << s.filename << " failed to compile:" << std::endl;
		    GLint  logSize;
		    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
		    char* logMsg = new char[logSize];
		    glGetShaderInfoLog( shader, logSize, NULL, logMsg );
		    std::cerr << logMsg << std::endl;
		    delete [] logMsg;

		    exit( EXIT_FAILURE );
		}

		// Memory management
		delete [] s.source;

		// Attach the compiled shader to the program we are creating
		glAttachShader( program, shader );
    }

    // Link the program
    glLinkProgram(program);

    // Check for a successful linkage
    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog( program, logSize, NULL, logMsg );
		std::cerr << logMsg << std::endl;
		delete [] logMsg;

		exit( EXIT_FAILURE );
    }

    /* use program object */
    glUseProgram(program);

    // Return our new shader program!
    return program;
}

}  // Close namespace Angel block
