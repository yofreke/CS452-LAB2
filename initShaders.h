#ifndef INITSHADERS_H_
#define INITSHADERS_H_

//#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/gl.h"
#include "GL/glu.h"

#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;

/* These are the function prototypes that initShaders.h will be using.  Defining the prototypes at the top of the class allow us to define the bodies of the functions later on in the file. */
GLuint createShader(GLenum type, const GLchar* shadeSource);
const GLchar* inputShader(const char* filename);
GLuint createProgram(const vector<GLuint> shadeList);

/* This defines the ShaderInfo struct. It contains an enum for what kind of shader the struct is, as well as a filename for the shader. */
typedef struct{
  GLenum type;// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
  const char* filename;//name of file to input
} ShaderInfo;

/* This function is called once at the beginning of the program. It is responsible for taking the shader info, and adding them to a list of GLuint, and then passing that list to the function responsible for creating the shader program that will be used by our OpenGL application. */
void initShaders(ShaderInfo* shaders){

  ShaderInfo* shade=shaders;
  // An expandable vector which will contain GLuints pointing to all of the shader instances.
  vector<GLuint> Shadelist;
  
  // Iterate through all of the shaders in the list passed to the function
  while(shade->type != GL_NONE){
    // Create a shader with the type from the list, and with the file from the list. Push it to the back of the Shadelist
    Shadelist.push_back(createShader(shade->type,inputShader(shade->filename)));
    // Increment which ShaderInfo we are looking at
    ++shade;
  }
  
  // This GLuint points to an OpenGL program instance, which is using the shaders we have created, and put into ShadeList.
  GLuint program=createProgram(Shadelist);
  
  // Sets OpenGL to use the program that we have created. This tells opengl that it should use the vertex processor and fragment processor linked to the program, if they exists. They should, if shaders contains a ShadeInfo with GL_VERTEX_SHADER and GL_FRAGMENT_SHADER
  glUseProgram(program);
}

//NOTE: if the file is unable to open or is empty this function will segmentation fault your program
/* This function loads the specified file into a GLchar buffer. */
const GLchar* inputShader(const char* filename){
  // Opens the file with file mode read binary ("rb").
  FILE* fshade = fopen(filename, "rb");
  
  // Check to see if there is a read lock on the file, if there is return null.
  if(!fshade){
    fprintf(stderr,"unable to open file '%s'\n",filename);
    return NULL;
  }
  
  // Get the size of the file so that we know how big the buffer for the file should be.
  fseek(fshade, 0, SEEK_END);
  long filesize=ftell(fshade);
  fseek(fshade, 0, SEEK_SET);
  
  // Allocate a buffer for the file and read the file into that buffer
  GLchar* shadingSource= new GLchar[filesize+1];
  fread(shadingSource, 1, filesize, fshade);
  
  // If the file is empty, return null.
  if(ftell(fshade) == 0){
    fprintf(stderr, "File '%s' is empty.\n",filename);
    return NULL;
  }

  // Close the file
  fclose(fshade);
  
  // Set an end string character ('\0') at the end of the file buffer
  shadingSource[filesize] = 0;
  
  // Returns the buffer containing the contents of the file
  return const_cast<const GLchar*>(shadingSource);//overloads the const so the value with change per file  
}

/* This function is responsible for creating the shader and return the GLuint where the shader is at. */
GLuint createShader(GLenum type, const GLchar* shadeSource){
  
  // glCreateShader creates a shader for either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
  GLuint shader = glCreateShader(type);
  // Puts the source code that we loaded from file into the shader that we just created.
  glShaderSource(shader, 1, &shadeSource, NULL);
  
  // Compiles the shader, after the source has been loaded into the shader.
  glCompileShader(shader);
  
  // These two lines check the compile status of the shader that we are creating.
  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);//returns the comiple status into the variable
  
  // If the compile status is bad, then give us some good output.
  if(!compileStatus){
    // Make a variable which will be the size of the debug info.
    GLint logSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);//returns the size of the the source file into the variable
    
    // Allocate the memory needed for the debug log.
    GLchar* infoLog = new GLchar[logSize+1];
    glGetShaderInfoLog(shader,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
    
    // define the normal char array for the shade info
    const char *shadeInfo= NULL;
    switch(type){//way to get what shader has the error
      case GL_VERTEX_SHADER: shadeInfo = "vertex"; break;
      case GL_GEOMETRY_SHADER_EXT: shadeInfo = "geometric"; break;
      case GL_FRAGMENT_SHADER: shadeInfo = "fragment"; break;
    }
    fprintf(stderr,"\nCompile failure in %u shader: %s\n Error message:\n%s\n",type,shadeInfo,infoLog);//prints information need to debug shaders
    delete[] infoLog;//memory management
  }
  return shader;//self explanatory
}

/* Create a shader program from the actual shader instances.  The shaders in the shadeList will be linked to the program. */
GLuint createProgram(const vector<GLuint> shadeList){
  // Tell OpenGL to make the new program and give us the GLuint identifier.
  GLuint program = glCreateProgram();
  
  // Goes through each of the shaders in the shadeList and attaches the shader to the program.
  for(GLuint i=0;i<shadeList.size();i++){glAttachShader(program,shadeList[i]);}
  
  // Binds the attribute in_position to the program at position 0
  glBindAttribLocation(program, 0, "in_position");
  // Binds the attribute in_color to the program at position 1
  glBindAttribLocation(program, 1, "in_color");
  // Links OpenGL to the program that we have been creating.
  glLinkProgram(program);
  
  // Checks the link status of the program.  Must first have a variable for OpenGL to put the status into.
  GLint linkStatus;
  // Puts the status of the program into the status variable.
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  
  // If the link status is bad...
  if(!linkStatus){
    // Debug info, same routine as earlier with the compile status.
    GLint logSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);//returns the linking status into the variable
    
    GLchar *infoLog = new GLchar[logSize+1];//allocating memory for the debug info
    glGetProgramInfoLog(program,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
    
    fprintf(stderr,"\nShader linking failed: %s\n",infoLog);//prints your linking failed
    delete[] infoLog;//memory management
    
    for(GLuint i=0;i<shadeList.size();i++){glDeleteShader(shadeList[i]);}//memory management
  }
  return program;//self explanatory
}
#endif
