#include "../llibreries/llibreries.hpp"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "bcm_host.h"
#include "../llibreries/graphics.h"
#include <opencv2/opencv.hpp> 
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <errno.h>
#include <sys/time.h>

class GfxTexture;
class Display;

#define check() assert(glGetError() == 0)

uint32_t GScreenWidth;
uint32_t GScreenHeight;
EGLDisplay GDisplay;
EGLSurface GSurface;
EGLContext GContext;

GfxShader GSimpleVS;
GfxShader GTrapecioVS;
GfxShader GSimpleFS;
GfxShader GYUVFS;
GfxShader GBlurFS;
GfxShader GSobelFS;
GfxShader GMedianFS;
GfxShader GMultFS;
GfxShader GThreshFS;
GfxShader GDilateFS;
GfxShader GErodeFS;
GfxShader GPatroFS;
GfxShader GCompFondoFS;
GfxShader GComp2FS;
GfxShader GCompGrisosFS;
GfxShader GCompPDFS;
GfxShader GComp3FS;
GfxShader GPuntFS;
GfxShader GMijaFS;
GfxShader GMija5x5FS;
GfxShader GCirculFS;
GfxShader GMatTransTabFS;

GfxProgram GMatTransTabProg;
GfxProgram GCirculProg;
GfxProgram GSimpleProg;
GfxProgram GTrapecioProg;
GfxProgram GYUVProg;
GfxProgram GBlurProg;
GfxProgram GSobelProg;
GfxProgram GMedianProg;
GfxProgram GMultProg;
GfxProgram GThreshProg;
GfxProgram GDilateProg;
GfxProgram GErodeProg;
GfxProgram GPatroProg;
GfxProgram GCompFondoProg;
GfxProgram GMatTransProg;
GfxProgram GComp2Prog;
GfxProgram GCompGrisosProg;
GfxProgram GCompPDProg;
GfxProgram GComp3Prog;
GfxProgram GPuntProg;
GfxProgram GMijaProg;
GfxProgram GMija5x5Prog;

GLuint GQuadVertexBuffer;
GLuint GTrapVertexBuffer;

void InitGraphics()
{
	bcm_host_init();
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	static const EGLint context_attributes[] = 
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLConfig config;

	// get an EGL display connection
	GDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(GDisplay!=EGL_NO_DISPLAY);
	check();

	// initialize the EGL display connection
	result = eglInitialize(GDisplay, NULL, NULL);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(GDisplay, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);
	check();

	// create an EGL rendering context
	GContext = eglCreateContext(GDisplay, config, EGL_NO_CONTEXT, context_attributes);
	assert(GContext!=EGL_NO_CONTEXT);
	check();

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &GScreenWidth, &GScreenHeight);
	assert( success >= 0 );

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = GScreenWidth;
	dst_rect.height = GScreenHeight;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = GScreenWidth << 16;
	src_rect.height = GScreenHeight << 16;        

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );

	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
		0/*layer*/, &dst_rect, 0/*src*/,
		&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

	nativewindow.element = dispman_element;
	nativewindow.width = GScreenWidth;
	nativewindow.height = GScreenHeight;
	vc_dispmanx_update_submit_sync( dispman_update );

	check();

	GSurface = eglCreateWindowSurface( GDisplay, config, &nativewindow, NULL );
	assert(GSurface != EGL_NO_SURFACE);
	check();

	// connect the context to the surface
	result = eglMakeCurrent(GDisplay, GSurface, GSurface, GContext);
	assert(EGL_FALSE != result);
	check();

	// Set background color and clear buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//(0.15f, 0.25f, 0.35f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT );

	//load the test shaders
	GSimpleVS.LoadVertexShader			("shaders/simplevertshader.glsl");
	GTrapecioVS.LoadVertexShader			("shaders/trapeciovertshader.glsl");
	GSimpleFS.LoadFragmentShader			("shaders/simplefragshader.glsl");

	GYUVFS.LoadFragmentShader       		("shaders/yuvfragshader.glsl");
	GBlurFS.LoadFragmentShader      		("shaders/blurfragshader.glsl");
	GSobelFS.LoadFragmentShader     		("shaders/sobelfragshader.glsl");
	GMedianFS.LoadFragmentShader    		("shaders/medianfragshader.glsl");
	GMultFS.LoadFragmentShader      		("shaders/multfragshader.glsl");
	GThreshFS.LoadFragmentShader    		("shaders/threshfragshader.glsl");
	GDilateFS.LoadFragmentShader    		("shaders/dilatefragshader.glsl");
	GErodeFS.LoadFragmentShader     		("shaders/erodefragshader.glsl");
	GPatroFS.LoadFragmentShader     		("shaders/patrofragshader.glsl"); 
	GCompFondoFS.LoadFragmentShader 		("shaders/compensarfondofragshader.glsl");
	GComp2FS.LoadFragmentShader     		("shaders/compensar2fragshader.glsl");
	GCompGrisosFS.LoadFragmentShader		("shaders/compensargrisosfragshader.glsl");
	GComp3FS.LoadFragmentShader     		("shaders/compensar3fragshader.glsl");
	GPuntFS.LoadFragmentShader      		("shaders/puntfragshader.glsl");
	GMijaFS.LoadFragmentShader      		("shaders/filtremija.glsl");
	GMija5x5FS.LoadFragmentShader   		("shaders/filtremija5x5.glsl");
	GCirculFS.LoadFragmentShader    		("shaders/circulfragshader.glsl");
	GCompPDFS.LoadFragmentShader   			("shaders/compensarPDfragshader.glsl");
	GMatTransTabFS.LoadFragmentShader 		("shaders/MatTransTabuladafragshader.glsl"); 

	GTrapecioProg.Create(&GTrapecioVS,&GSimpleFS);
	GMatTransTabProg.Create( &GSimpleVS, &GMatTransTabFS );
	GCirculProg.Create(&GSimpleVS,&GCirculFS);
	GMija5x5Prog.Create(&GSimpleVS,&GMija5x5FS);
	GMijaProg.Create(&GSimpleVS,&GMijaFS);
	GPuntProg.Create(&GSimpleVS,&GPuntFS);
	GSimpleProg.Create(&GSimpleVS,&GSimpleFS);
	GYUVProg.Create(&GSimpleVS,&GYUVFS);
	GBlurProg.Create(&GSimpleVS,&GBlurFS);
	GSobelProg.Create(&GSimpleVS,&GSobelFS);
	GMedianProg.Create(&GSimpleVS,&GMedianFS);
	GMultProg.Create(&GSimpleVS,&GMultFS);
	GThreshProg.Create(&GSimpleVS,&GThreshFS);
	GDilateProg.Create(&GSimpleVS,&GDilateFS);
	GErodeProg.Create(&GSimpleVS,&GErodeFS);
	GPatroProg.Create(&GSimpleVS,&GPatroFS);
	GCompFondoProg.Create(&GTrapecioVS,&GCompFondoFS);  //GTrapecioVS
	GMatTransProg.Create(&GTrapecioVS,&GSimpleFS);
	GComp2Prog.Create(&GSimpleVS,&GComp2FS);
	GCompGrisosProg.Create(&GSimpleVS,&GCompGrisosFS);
	GCompPDProg.Create(&GSimpleVS,&GCompPDFS);
	GComp3Prog.Create(&GTrapecioVS,&GComp3FS);
	check();

	//create an ickle vertex buffer
	static const GLfloat quad_vertex_positions[] = {
		0.0f, 0.0f,	1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	glGenBuffers(1, &GQuadVertexBuffer);
	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_positions), quad_vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check();

	static const GLfloat trap_vertex_positions[] = {
		0.0f, 0.0f, 1.0f, 1.0f, 
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	glGenBuffers(1, &GTrapVertexBuffer);
	check();
	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trap_vertex_positions), trap_vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check();

}

void BeginFrame()
{
	// Prepare viewport
	glViewport ( 0, 0, GScreenWidth, GScreenHeight );
    check();

	// Clear the background
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//(0.15f, 0.25f, 0.35f, 1.0f);
    //glClear( GL_COLOR_BUFFER_BIT );
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    check();
}

void EndFrame()
{
	eglSwapBuffers(GDisplay,GSurface);
	check();
}

void ReleaseGraphics()
{

}

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog : " << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}
}

bool GfxShader::LoadVertexShader(const char* filename)
{
	//cheeky bit of code to read the whole file into memory
	assert(!Src);
	FILE* f = fopen(filename, "rb");
	assert(f);
	fseek(f,0,SEEK_END);
	int sz = ftell(f);
	fseek(f,0,SEEK_SET);
	Src = new GLchar[sz+1];
	fread(Src,1,sz,f);
	Src[sz] = 0; //null terminate it!
	fclose(f);

	//now create and compile the shader
	GlShaderType = GL_VERTEX_SHADER;
	Id = glCreateShader(GlShaderType);
	glShaderSource(Id, 1, (const GLchar**)&Src, 0);
	glCompileShader(Id);
	check();

	//compilation check
	GLint compiled;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &compiled);
	if(compiled==0)
	{
		printf("Failed to compile vertex shader %s:\n%s\n", filename, Src);
		printShaderInfoLog(Id);
		glDeleteShader(Id);
		return false;
	}
	else
	{
		printf("Compiled vertex shader %s:\n%s\n", filename, Src);
	}

	return true;
}

bool GfxShader::LoadFragmentShader(const char* filename)
{
	//cheeky bit of code to read the whole file into memory
	assert(!Src);
	FILE* f = fopen(filename, "rb");
	assert(f);
	fseek(f,0,SEEK_END);
	int sz = ftell(f);
	fseek(f,0,SEEK_SET);
	Src = new GLchar[sz+1];
	fread(Src,1,sz,f);
	Src[sz] = 0; //null terminate it!
	fclose(f);

	//now create and compile the shader
	GlShaderType = GL_FRAGMENT_SHADER;
	Id = glCreateShader(GlShaderType);
	glShaderSource(Id, 1, (const GLchar**)&Src, 0);
	glCompileShader(Id);
	check();

	//compilation check
	GLint compiled;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &compiled);
	if(compiled==0)
	{
		printf("Failed to compile fragment shader %s:\n%s\n", filename, Src);
		printShaderInfoLog(Id);
		glDeleteShader(Id);
		return false;
	}
	else
	{
		printf("Compiled fragment shader %s:\n%s\n", filename, Src);
	}

	return true;
}

bool GfxProgram::Create(GfxShader* vertex_shader, GfxShader* fragment_shader)
{
	VertexShader = vertex_shader;
	FragmentShader = fragment_shader;
	Id = glCreateProgram();
	glAttachShader(Id, VertexShader->GetId());
	glAttachShader(Id, FragmentShader->GetId());
	glLinkProgram(Id);
	check();
	printf("Created program id %d from vs %d and fs %d\n", GetId(), VertexShader->GetId(), FragmentShader->GetId());

	// Prints the information log for a program object
	char log[1024];
	glGetProgramInfoLog(Id,sizeof log,NULL,log);
	printf("%d:program:\n%s\n", Id, log);

	return true;	
}

void DrawTextureRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GSimpleProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GSimpleProg.GetId(),"tex"), 0);
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawMultRect(GfxTexture* texture, float x0, float y0, float x1, float y1, float r, float g, float b, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GMultProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GMultProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GMultProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform4f(glGetUniformLocation(GMultProg.GetId(),"col"),r,g,b,1);
	glUniform1i(glGetUniformLocation(GMultProg.GetId(),"tex"), 0);
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawThreshRect(GfxTexture* texture, float x0, float y0, float x1, float y1, float r, float g, float b, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GThreshProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GThreshProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GThreshProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform4f(glGetUniformLocation(GThreshProg.GetId(),"col"),r,g,b,0);
	glUniform1i(glGetUniformLocation(GThreshProg.GetId(),"tex"), 0);
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawBlurredRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GBlurProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GBlurProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GBlurProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GBlurProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GBlurProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawDilateRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GDilateProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GDilateProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GDilateProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GDilateProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GDilateProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawPatroRect(GfxTexture* texture, float R, float G, float B, float x0, float y0, float x1, float y1, float inc, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GPatroProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GPatroProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GPatroProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GPatroProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GPatroProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	glUniform2f(glGetUniformLocation(GPatroProg.GetId(),"incr"),inc,inc);
	glUniform4f(glGetUniformLocation(GPatroProg.GetId(),"rgb"), R, G, B, 1.0);

	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawPuntRect(GfxTexture* texture, int x, int y, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GPuntProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GPuntProg.GetId(),"offset"), -1.0, -1.0);
	glUniform2f(glGetUniformLocation(GPuntProg.GetId(),"scale"),   2.0,  2.0);
	glUniform1i(glGetUniformLocation(GPuntProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GPuntProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	glUniform2f(glGetUniformLocation(GPuntProg.GetId(),"pos"),x,y);

	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}


void DrawCirculRect(GfxTexture* texture, int x, int y, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GCirculProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GCirculProg.GetId(),"offset"), -1.0, -1.0);
	glUniform2f(glGetUniformLocation(GCirculProg.GetId(),"scale"),   2.0,  2.0);
	glUniform1i(glGetUniformLocation(GCirculProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GCirculProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	glUniform2f(glGetUniformLocation(GCirculProg.GetId(),"pos"),x,y);

	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawMijaRect(GfxTexture* texture, int AxA, int x, int y, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GMijaProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GMijaProg.GetId(),"offset"), -1.0, -1.0);
	glUniform2f(glGetUniformLocation(GMijaProg.GetId(),"scale"),   2.0,  2.0);
	glUniform1i(glGetUniformLocation(GMijaProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GMijaProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	glUniform2f(glGetUniformLocation(GMijaProg.GetId(),"pos"),x,y);
	glUniform1i(glGetUniformLocation(GMijaProg.GetId(),"AxA"), AxA);

	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}
//filtremija5x5.glsl
void DrawMija5x5Rect(GfxTexture* texture, int AxA, int x, int y, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GMija5x5Prog.GetId());	check();

	glUniform2f(glGetUniformLocation(GMija5x5Prog.GetId(),"offset"), -1.0, -1.0);
	glUniform2f(glGetUniformLocation(GMija5x5Prog.GetId(),"scale"),   2.0,  2.0);
	glUniform1i(glGetUniformLocation(GMija5x5Prog.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GMija5x5Prog.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	glUniform2f(glGetUniformLocation(GMija5x5Prog.GetId(),"pos"),x,y);
	glUniform1i(glGetUniformLocation(GMija5x5Prog.GetId(),"AxA"), AxA);

	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void Matriu_Transformacio (GfxTexture* texture_im, float corregir_x, float corregir_y,  GfxTexture* render_target)
{
	if(render_target)
	{

		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight());
		check();
	}


	float x = corregir_x/texture_im->GetWidth();
	float y = corregir_y/texture_im->GetHeight();

	glUseProgram(GMatTransProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GMatTransProg.GetId(),"offset"), -1.0-x, -1.0-y); //-1.0,  1.0); //x0,y0);  //GCompFondoProg
	glUniform2f(glGetUniformLocation(GMatTransProg.GetId(),"scale"),   2.0,  2.0); //2.0, -2.0); //x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GMatTransProg.GetId(),"tex_im"), 0);
	glUniform2f(glGetUniformLocation(GMatTransProg.GetId(),"texelsize_im"), 1.f/texture_im->GetWidth(), 1.f/texture_im->GetHeight());
	glUniform2f(glGetUniformLocation(GMatTransProg.GetId(),"texelsize_llum"), 1.f/render_target->GetWidth(), 1.f/render_target->GetHeight());

	check();

	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();


	GLuint loc = glGetAttribLocation(GTrapecioProg.GetId(),"vertex");   //GTrapecioProg  //GSimpleProg
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
		//glScissor(1.0, 1.0, 1500.0, 1500.0);
		//glEnable(GL_SCISSOR_TEST);
		//glHistogram();
		//glScale(1.0, 1.0, 1.0);
	}
}

void DrawCompensarFondo(GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture_pant->GetId());	check();
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(GCompFondoProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GCompFondoProg.GetId(),"offset"), -1.0, -1.0); //-1.0,  1.0); //x0,y0);  //GCompFondoProg
	glUniform2f(glGetUniformLocation(GCompFondoProg.GetId(),"scale"),   2.0,  2.0); //2.0, -2.0); //x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GCompFondoProg.GetId(),"tex_im"), 0);
	glUniform2f(glGetUniformLocation(GCompFondoProg.GetId(),"texelsize_im"), 1.f/texture_im->GetWidth(), 1.f/texture_im->GetHeight());
	glUniform1i(glGetUniformLocation(GCompFondoProg.GetId(),"tex_pant"), texture_pant->GetId()); //tex_render->GetId());
	glUniform2f(glGetUniformLocation(GCompFondoProg.GetId(),"texelsize_llum"), 1.f/render_target->GetWidth(), 1.f/render_target->GetHeight());
	glUniform1f(glGetUniformLocation(GCompFondoProg.GetId(),"fondo_ref"), ((float)maxvalor/255.0)*1.0);
	glUniform4f(glGetUniformLocation(GCompFondoProg.GetId(),"color_base"), Rc/255.0, Gc/255.0, Bc/255.0, 1.0);

	check();

	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();

	//glActiveTexture(GL_TEXTURE0);

/*
	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();
*/
	GLuint loc = glGetAttribLocation(GTrapecioProg.GetId(),"vertex");   //GTrapecioProg  //GSimpleProg
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
		//glScissor(1.0, 1.0, 1500.0, 1500.0);
		//glEnable(GL_SCISSOR_TEST);
		//glHistogram();
		//glScale(1.0, 1.0, 1.0);
	}
}

void DrawCompensarGrisos(GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GCompGrisosProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GCompGrisosProg.GetId(),"offset"), -1.0, -1.0); //-1.0,  1.0); //x0,y0);  //GCompFondoProg
	glUniform2f(glGetUniformLocation(GCompGrisosProg.GetId(),"scale"),   2.0,  2.0); //2.0, -2.0); //x1-x0,y1-y0);
	glUniform2f(glGetUniformLocation(GCompGrisosProg.GetId(),"texelsize_im"), 1.f/texture_im->GetWidth(), 1.f/texture_im->GetHeight());
	glUniform1i(glGetUniformLocation(GCompGrisosProg.GetId(),"tex_im")  , 0);
	glUniform1i(glGetUniformLocation(GCompGrisosProg.GetId(),"tex_pant"), 1); //tex_render->GetId());
	glUniform2f(glGetUniformLocation(GCompGrisosProg.GetId(),"texelsize_llum"), 1.f/render_target->GetWidth(), 1.f/render_target->GetHeight());
	glUniform1f(glGetUniformLocation(GCompGrisosProg.GetId(),"fondo_ref"), ((float)maxvalor/255.0)*1.0);
	glUniform4f(glGetUniformLocation(GCompGrisosProg.GetId(),"color_base"), Rc/255.0, Gc/255.0, Bc/255.0, 1.0);

	check();

	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture_pant->GetId());	check();
	glActiveTexture(GL_TEXTURE0);

	GLuint loc = glGetAttribLocation(GTrapecioProg.GetId(),"vertex");   //GTrapecioProg  //GSimpleProg
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawCompensarPD(GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GCompPDProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GCompPDProg.GetId(),"offset"), -1.0, -1.0); //-1.0,  1.0); //x0,y0);  //GCompFondoProg
	glUniform2f(glGetUniformLocation(GCompPDProg.GetId(),"scale"),   2.0,  2.0); //2.0, -2.0); //x1-x0,y1-y0);
	glUniform2f(glGetUniformLocation(GCompPDProg.GetId(),"texelsize_im"), 1.f/texture_im->GetWidth(), 1.f/texture_im->GetHeight());
	glUniform1i(glGetUniformLocation(GCompPDProg.GetId(),"tex_im")  , 0);
	glUniform1i(glGetUniformLocation(GCompPDProg.GetId(),"tex_pant"), 1); //tex_render->GetId());
	glUniform2f(glGetUniformLocation(GCompPDProg.GetId(),"texelsize_llum"), 1.f/render_target->GetWidth(), 1.f/render_target->GetHeight());
	glUniform1f(glGetUniformLocation(GCompPDProg.GetId(),"fondo_ref"), ((float)maxvalor/255.0)*1.0);
	glUniform4f(glGetUniformLocation(GCompPDProg.GetId(),"color_base"), Rc/255.0, Gc/255.0, Bc/255.0, 1.0);

	check();

	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture_pant->GetId());	check();
	glActiveTexture(GL_TEXTURE0);

	GLuint loc = glGetAttribLocation(GTrapecioProg.GetId(),"vertex");   //GTrapecioProg  //GSimpleProg
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawCompensar2(GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GComp2Prog.GetId());	check();

	glUniform2f(glGetUniformLocation(GComp2Prog.GetId(),"offset"), -1.0, -1.0); //-1.0,  1.0); //x0,y0);  //GCompFondoProg
	glUniform2f(glGetUniformLocation(GComp2Prog.GetId(),"scale"),   2.0,  2.0); //2.0, -2.0); //x1-x0,y1-y0);
	glUniform2f(glGetUniformLocation(GComp2Prog.GetId(),"texelsize_im"), 1.f/texture_im->GetWidth(), 1.f/texture_im->GetHeight());
	glUniform1i(glGetUniformLocation(GComp2Prog.GetId(),"tex_im")  , 0);
	glUniform1i(glGetUniformLocation(GComp2Prog.GetId(),"tex_pant"), 1); //tex_render->GetId());
	glUniform2f(glGetUniformLocation(GComp2Prog.GetId(),"texelsize_llum"), 1.f/render_target->GetWidth(), 1.f/render_target->GetHeight());
	glUniform1f(glGetUniformLocation(GComp2Prog.GetId(),"fondo_ref"), ((float)maxvalor/255.0)*1.0);
	glUniform4f(glGetUniformLocation(GComp2Prog.GetId(),"color_base"), Rc/255.0, Gc/255.0, Bc/255.0, 1.0);

	check();

	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture_pant->GetId());	check();
	glActiveTexture(GL_TEXTURE0);

	GLuint loc = glGetAttribLocation(GTrapecioProg.GetId(),"vertex");   //GTrapecioProg  //GSimpleProg
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawCompensar3(GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture_pant->GetId());	check();
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(GComp3Prog.GetId());	check();

	glUniform2f(glGetUniformLocation(GComp3Prog.GetId(),"offset"), -1.0, -1.0); //-1.0,  1.0); //x0,y0);  //GCompFondoProg
	glUniform2f(glGetUniformLocation(GComp3Prog.GetId(),"scale"),   2.0,  2.0); //2.0, -2.0); //x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GComp3Prog.GetId(),"tex_im"), 0);
	glUniform2f(glGetUniformLocation(GComp3Prog.GetId(),"texelsize_im"), 1.f/texture_im->GetWidth(), 1.f/texture_im->GetHeight());
	glUniform1i(glGetUniformLocation(GComp3Prog.GetId(),"tex_pant"), texture_pant->GetId()); //tex_render->GetId());
	glUniform2f(glGetUniformLocation(GComp3Prog.GetId(),"texelsize_llum"), 1.f/render_target->GetWidth(), 1.f/render_target->GetHeight());
	glUniform1f(glGetUniformLocation(GComp3Prog.GetId(),"fondo_ref"), ((float)maxvalor/255.0)*1.0);
	glUniform4f(glGetUniformLocation(GComp3Prog.GetId(),"color_base"), Rc/255.0, Gc/255.0, Bc/255.0, 1.0);

	check();

	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();

	//glActiveTexture(GL_TEXTURE0);

/*
	glBindBuffer(GL_ARRAY_BUFFER, GTrapVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture_im->GetId());	check();
*/
	GLuint loc = glGetAttribLocation(GTrapecioProg.GetId(),"vertex");   //GTrapecioProg  //GSimpleProg
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
		//glScissor(1.0, 1.0, 1500.0, 1500.0);
		//glEnable(GL_SCISSOR_TEST);
		//glHistogram();
		//glScale(1.0, 1.0, 1.0);
	}
}

void DrawErodeRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GErodeProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GErodeProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GErodeProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GErodeProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GErodeProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawSobelRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GSobelProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GSobelProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GSobelProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GSobelProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GSobelProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}


void DrawMedianRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GMedianProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GMedianProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GMedianProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GMedianProg.GetId(),"tex"), 0);
	glUniform2f(glGetUniformLocation(GMedianProg.GetId(),"texelsize"),1.f/texture->GetWidth(),1.f/texture->GetHeight());
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glBindTexture(GL_TEXTURE_2D,texture->GetId());	check();

	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}

void DrawYUVTextureRect(GfxTexture* ytexture, GfxTexture* utexture, GfxTexture* vtexture, float x0, float y0, float x1, float y1, GfxTexture* render_target)
{
	if(render_target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,render_target->GetFramebufferId());
		glViewport ( 0, 0, render_target->GetWidth(), render_target->GetHeight() );
		check();
	}

	glUseProgram(GYUVProg.GetId());	check();

	glUniform2f(glGetUniformLocation(GYUVProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GYUVProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GYUVProg.GetId(),"tex0"), 0);
	glUniform1i(glGetUniformLocation(GYUVProg.GetId(),"tex1"), 1);
	glUniform1i(glGetUniformLocation(GYUVProg.GetId(),"tex2"), 2);
	check();

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);	check();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,ytexture->GetId());	check();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,utexture->GetId());	check();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,vtexture->GetId());	check();
	glActiveTexture(GL_TEXTURE0);

	GLuint loc = glGetAttribLocation(GYUVProg.GetId(),"vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);	check();
	glEnableVertexAttribArray(loc);	check();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 ); check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(render_target)
	{
		//glFinish();	check();
		//glFlush(); check();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	}
}


bool GfxTexture::CreateRGBA(int width, int height, const void* data)
{
	Width = width;
	Height = height;
	glGenTextures(1, &Id);
	check();
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	check();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
	IsRGBA = true;
	return true;
}

bool GfxTexture::CreateGreyScale(int width, int height, const void* data)
{
	Width = width;
	Height = height;
	glGenTextures(1, &Id);
	check();
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, Width, Height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	check();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
	IsRGBA = false;
	return true;
}

bool GfxTexture::GenerateFrameBuffer()
{
	//Create a frame buffer that points to this texture
	glGenFramebuffers(1,&FramebufferId);
	check();
	glBindFramebuffer(GL_FRAMEBUFFER,FramebufferId);
	check();
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,Id,0);
	check();
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	check();
	return true;
}

void GfxTexture::SetPixels(const void* data)
{
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, IsRGBA ? GL_RGBA : GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
	check();
}

void SaveFrameBuffer(const char* fname)
{
	void* image = malloc(GScreenWidth*GScreenHeight*4);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	check();
	glReadPixels(0,0,GScreenWidth,GScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);

	unsigned error = lodepng::encode(fname, (const unsigned char*)image, GScreenWidth, GScreenHeight, LCT_RGBA);
	if(error) 
		printf("error: %d\n",error);

	free(image);

}

void GfxTexture::Save(const char* fname)
{
	void* image = malloc(Width*Height*4);
	glBindFramebuffer(GL_FRAMEBUFFER,FramebufferId);
	check();
	glReadPixels(0,0,Width,Height,IsRGBA ? GL_RGBA : GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
	check();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	unsigned error = lodepng::encode(fname, (const unsigned char*)image, Width, Height, IsRGBA ? LCT_RGBA : LCT_GREY);
	if(error) 
		printf("error: %d\n",error);

	free(image);
}

void GfxTexture::SaveBMP(const char* fname)
{
	void* image = malloc(Width*Height*4);
	glBindFramebuffer(GL_FRAMEBUFFER,FramebufferId);
	check();
	glReadPixels(0,0,Width,Height,IsRGBA ? GL_RGBA : GL_LUMINANCE, GL_UNSIGNED_BYTE, image);

	Mat imagen(Size(Width, Height), CV_8UC4, image);
	imwrite((const char*)fname, imagen);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	free(image);
}

void GfxTexture::GetPixels( void* image_desti )
{
	glBindFramebuffer(GL_FRAMEBUFFER,FramebufferId);
	check();
	glReadPixels(0,0,Width,Height,IsRGBA ? GL_RGBA : GL_LUMINANCE, GL_UNSIGNED_BYTE, image_desti);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}


int ILLUMINACIO (GfxTexture* texture, int R, int G, int B, int fondo)
{
	char rut[35]="";
	if(fondo == 0)
	{

		glBindTexture(GL_TEXTURE_2D, texture->GetId());  check();
		unsigned char* datos = new unsigned char [texture->GetWidth()*texture->GetHeight()*4];
		Mat illuminacio (Size(texture->GetWidth(),texture->GetHeight()), CV_8UC4, datos);
		//Mat illuminacio (Size(texture->GetWidth(),texture->GetHeight()), CV_8UC3, Scalar(R,G,B));  //texture->GetWidth(),texture->GetHeight()
		illuminacio = Scalar(R,G,B,255);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); check();  //GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); check();
				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  check();   //GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  check();   //GL_LINEAR		
		
		glTexImage2D(GL_TEXTURE_2D,			// Type of texture
				0,			// Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGBA,			// Internal colour format to convert to
				illuminacio.cols,	// Image width  i.e. 640 for Kinect in standard mode   image.cols
				illuminacio.rows,	// Image height i.e. 480 for Kinect in standard mode   image.rows
				0,			// Border width in pixels (can either be 1 or 0)
				GL_RGBA,			// Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,	// Image data type   // GL_UNSIGNED_BYTE
				illuminacio.ptr());	// The actual image data itself
		check(); 
				
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		illuminacio.release();
		delete [] datos;
		

		return 1;
	}
	else if(fondo == 1) strcat(rut, "/home/pi/Proces/FONDO/llum_1.bmp");
	else if(fondo == 2) strcat(rut, "/home/pi/Proces/FONDO/llum_2.bmp");
	else if(fondo == 3) strcat(rut, "/home/pi/Proces/FONDO/llum_3.bmp");
	else if(fondo == 4) strcat(rut, "/home/pi/Proces/FONDO/llum_4.bmp");
	else if(fondo == 5) strcat(rut, "/home/pi/Proces/FONDO/llum_5.bmp");
	else if(fondo == 6) strcat(rut, "/home/pi/Proces/FONDO/llum_6.bmp");
	else if(fondo == 8) strcat(rut, "/home/pi/Proces/FONDO/llum_8.bmp");
	else if(fondo == 9) strcat(rut, "/home/pi/Proces/FONDO/llum_9.bmp");
	else if(fondo == 10) strcat(rut, "/home/pi/Proces/FONDO/llum_10.bmp");
	else if(fondo == 11) strcat(rut, "/home/pi/Proces/FONDO/llum_11.bmp");
	else if(fondo == 12) strcat(rut, "/home/pi/Proces/FONDO/llum_12.bmp");
	else if(fondo == 13) strcat(rut, "/home/pi/Proces/FONDO/llum_13.bmp");
	else if(fondo == 14) strcat(rut, "/home/pi/Proces/FONDO/llum_14.bmp");
	else if(fondo == 15) strcat(rut, "/home/pi/Proces/FONDO/llum_15.bmp");
	else if(fondo == 16) strcat(rut, "/home/pi/Proces/FONDO/llum_16.bmp");
	else if(fondo == 17) strcat(rut, "/home/pi/Proces/FONDO/llum_17.bmp");
	else if(fondo == 18) strcat(rut, "/home/pi/Proces/FONDO/llum_18.bmp");
	else if(fondo == 19) strcat(rut, "/home/pi/Proces/FONDO/llum_19.bmp");
	else if(fondo == 20) strcat(rut, "/home/pi/Proces/FONDO/llum_20.bmp");
	else if(fondo == 21) strcat(rut, "/home/pi/Proces/FONDO/llum_21.bmp");
	else if(fondo == 22) strcat(rut, "/home/pi/Proces/FONDO/llum_22.bmp");
	else if(fondo == 23) strcat(rut, "/home/pi/Proces/FONDO/llum_23.bmp");
	else if(fondo == 24) strcat(rut, "/home/pi/Proces/FONDO/llum_24.bmp");
	else if(fondo == 25) strcat(rut, "/home/pi/Proces/FONDO/llum_25.bmp");
	else if(fondo == 26) strcat(rut, "/home/pi/Proces/FONDO/llum_26.bmp");
	else if(fondo == 27) strcat(rut, "/home/pi/Proces/FONDO/llum_27.bmp");
	else if(fondo == 28) strcat(rut, "/home/pi/Proces/FONDO/llum_28.bmp");
	else if(fondo == 29) strcat(rut, "/home/pi/Proces/FONDO/llum_29.bmp");
	else if(fondo == 30) strcat(rut, "/home/pi/Proces/FONDO/llum_30.bmp");
	else if(fondo == 31) strcat(rut, "/home/pi/Proces/FONDO/llum_31.bmp");
	else if(fondo == 32) strcat(rut, "/home/pi/Proces/FONDO/llum_32.bmp");
	else if(fondo == 33) strcat(rut, "/home/pi/Proces/FONDO/llum_33.bmp");
	else if(fondo == 34) strcat(rut, "/home/pi/Proces/FONDO/llum_34.bmp");
	else if(fondo == 35) strcat(rut, "/home/pi/Proces/FONDO/llum_35.bmp");
	else if(fondo == 36) strcat(rut, "/home/pi/Proces/FONDO/llum_36.bmp");
	else if(fondo == 37) strcat(rut, "/home/pi/Proces/FONDO/llum_37.bmp");
	else if(fondo == 38) strcat(rut, "/home/pi/Proces/FONDO/llum_38.bmp");
	else if(fondo == 39) strcat(rut, "/home/pi/Proces/FONDO/llum_39.bmp");
	else if(fondo == 40) strcat(rut, "/home/pi/Proces/FONDO/llum_40.bmp");
	else if(fondo == 41) strcat(rut, "/home/pi/Proces/FONDO/llum_A1.bmp");
	else if(fondo == 42) strcat(rut, "/home/pi/Proces/FONDO/llum_A2.bmp");
	else if(fondo == 43) strcat(rut, "/home/pi/Proces/FONDO/llum_patro.bmp");
	else if(fondo == 44) strcat(rut, "/home/pi/Proces/FONDO/patro_circuls.bmp");
	else if(fondo == 45) strcat(rut, "/home/pi/Proces/FONDO/patro_circuls2.bmp"); 
	else if(fondo == 51) strcat(rut, "/home/pi/Proces/FONDO/llum_B1.bmp");
	else if(fondo == 53) strcat(rut, "/home/pi/Proces/FONDO/llum_B3.bmp");
	else if(fondo == 54) strcat(rut, "/home/pi/Proces/FONDO/llum_B4.bmp");
	else if(fondo == 55) strcat(rut, "/home/pi/Proces/FONDO/llum_B5.bmp");
	else return 0;

	Mat illuminacio = imread(rut);

	glBindTexture(GL_TEXTURE_2D, texture->GetId());  check();

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); check();  //GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); check();
				
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  check();   //GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  check();   //GL_LINEAR
			
		
	glTexImage2D(GL_TEXTURE_2D,			// Type of texture
				0,			// Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,			// Internal colour format to convert to
				illuminacio.cols,	// Image width  i.e. 640 for Kinect in standard mode   image.cols
				illuminacio.rows,	// Image height i.e. 480 for Kinect in standard mode   image.rows
				0,			// Border width in pixels (can either be 1 or 0)
				GL_RGB,			// Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,	// Image data type   // GL_UNSIGNED_BYTE
				illuminacio.ptr());	// The actual image data itself
	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, ample, alt, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, illum);
	check();
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
	illuminacio.release();

}

void mat2textUC( const Mat& mat, GfxTexture* texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->GetId());  check();
    //float color[]={0.0f, 0.0f, 0.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color); check();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); check();  // CLAMP_TO_EDGE    GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); check();
				
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  check();   //GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  check();   //GL_LINEAR		
		
	glTexImage2D(GL_TEXTURE_2D,			// Type of texture
				0,			// Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGBA,			// Internal colour format to convert to
				mat.cols,	// Image width  i.e. 640 for Kinect in standard mode   image.cols
				mat.rows,	// Image height i.e. 480 for Kinect in standard mode   image.rows
				0,			// Border width in pixels (can either be 1 or 0)
				GL_RGBA,			// Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,	// Image data type   // GL_UNSIGNED_BYTE
				mat.ptr());	// The actual image data itself
	check(); 
				
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void mat2textF( const Mat& mat, GfxTexture* texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->GetId());  check();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); check();  //GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); check();
				
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  check();   //GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  check();   //GL_LINEAR		
		
	glTexImage2D(GL_TEXTURE_2D,			// Type of texture
				0,			// Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGBA,			// Internal colour format to convert to
				mat.cols,	// Image width  i.e. 640 for Kinect in standard mode   image.cols
				mat.rows,	// Image height i.e. 480 for Kinect in standard mode   image.rows
				0,			// Border width in pixels (can either be 1 or 0)
				GL_RGBA,			// Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_FLOAT,	// Image data type   // GL_UNSIGNED_BYTE
				mat.ptr());	// The actual image data itself
	check(); 
				
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}





