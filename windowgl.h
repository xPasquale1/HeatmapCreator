#pragma once

#include <windows.h>
#include <windowsx.h>
#include "glcorearb.h"
#include <fstream>

#include "util.h"
#include "math.h"
#include "font.h"

//TODO namespace endlich

void* loadGlFunction(const char* name){
	void* p = (void*)wglGetProcAddress(name);
	if(p == 0 || p == (void*)1 || p == (void*)2 || p == (void*)3 || p == (void*)-1){
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void*)GetProcAddress(module, name);
	}
	return p;
}

PFNGLGETERRORPROC glGetError;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVIEWPORTPROC glViewport;
PFNGLCLEARPROC glClear;
PFNGLENABLEPROC glEnable;
PFNGLDISABLEPROC glDisable;
PFNGLBLENDFUNCPROC glBlendFunc;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLGENTEXTURESPROC glGenTextures;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLBINDTEXTURESPROC glBindTextures;
PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLTEXPARAMETERIPROC glTexParameteri;
PFNGLTEXPARAMETERFVPROC glTexParameterfv;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;

//TODO sollte einen OpenGL Kontext erstellen bevor das aufgerufen wird
ErrCode initDrawLinesProgram();
ErrCode initDrawCirclesProgram();
ErrCode initDrawRectanglesProgram();
static volatile GLuint drawLinesProgram;
static volatile GLuint drawCirclesProgram;
static volatile GLuint drawRectanglesProgram;
ErrCode init(){
	glGetError = (PFNGLGETERRORPROC)loadGlFunction("glGetError");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)loadGlFunction("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)loadGlFunction("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)loadGlFunction("glUseProgram");
	glViewport = (PFNGLVIEWPORTPROC)loadGlFunction("glViewport");
	glClear = (PFNGLCLEARPROC)loadGlFunction("glClear");
	glEnable = (PFNGLENABLEPROC)loadGlFunction("glEnable");
	glDisable = (PFNGLDISABLEPROC)loadGlFunction("glDisable");
	glBlendFunc = (PFNGLBLENDFUNCPROC)loadGlFunction("glBlendFunc");
	glClearColor = (PFNGLCLEARCOLORPROC)loadGlFunction("glClearColor");
	glDrawArrays = (PFNGLDRAWARRAYSPROC)loadGlFunction("glDrawArrays");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)loadGlFunction("glVertexAttribPointer");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)loadGlFunction("glVertexAttribIPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)loadGlFunction("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)loadGlFunction("glDisableVertexAttribArray");
	glCreateShader = (PFNGLCREATESHADERPROC)loadGlFunction("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)loadGlFunction("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)loadGlFunction("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)loadGlFunction("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)loadGlFunction("glGetShaderiv");
	glAttachShader = (PFNGLATTACHSHADERPROC)loadGlFunction("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)loadGlFunction("glDetachShader");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)loadGlFunction("glGetShaderInfoLog");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)loadGlFunction("glLinkProgram");
	glGenBuffers = (PFNGLGENBUFFERSPROC)loadGlFunction("glGenBuffers");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)loadGlFunction("glDeleteBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)loadGlFunction("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)loadGlFunction("glBufferData");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)loadGlFunction("glGenVertexArrays");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)loadGlFunction("glDeleteVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)loadGlFunction("glBindVertexArray");
	glGenTextures = (PFNGLGENTEXTURESPROC)loadGlFunction("glGenTextures");
	glDeleteTextures = (PFNGLDELETETEXTURESPROC)loadGlFunction("glDeleteTextures");
	glBindTexture = (PFNGLBINDTEXTUREPROC)loadGlFunction("glBindTexture");
	glBindTextures = (PFNGLBINDTEXTURESPROC)loadGlFunction("glBindTextures");
	glTexImage2D = (PFNGLTEXIMAGE2DPROC)loadGlFunction("glTexImage2D");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)loadGlFunction("glGenerateMipmap");
	glTexParameteri = (PFNGLTEXPARAMETERIPROC)loadGlFunction("glTexParameteri");
	glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)loadGlFunction("glTexParameterfv");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)loadGlFunction("glActiveTexture");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)loadGlFunction("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)loadGlFunction("glUniform1i");
	glUniform4i = (PFNGLUNIFORM4IPROC)loadGlFunction("glUniform4i");
	glUniform4f = (PFNGLUNIFORM4FPROC)loadGlFunction("glUniform4f");
	glUniform1f = (PFNGLUNIFORM1FPROC)loadGlFunction("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC)loadGlFunction("glUniform2f");
	if(ErrCheck(initDrawLinesProgram(), "Draw Lines initialisieren") != SUCCESS) return GENERIC_ERROR;
	if(ErrCheck(initDrawCirclesProgram(), "Draw Circles initialisieren") != SUCCESS) return GENERIC_ERROR;
	if(ErrCheck(initDrawRectanglesProgram(), "Draw Rectangles initialisieren") != SUCCESS) return GENERIC_ERROR;
	return SUCCESS;
}

#define INVALIDHANDLEERRORS

#define WINDOWFLAGSTYPE BYTE
enum WINDOWFLAG : WINDOWFLAGSTYPE{
	WINDOW_NONE = 0,
	WINDOW_CLOSE = 1,
	WINDOW_RESIZE = 2
};
//Hat viele Attribute die man auch über die win api abrufen könnte, aber diese extra zu speichern macht alles übersichtlicher
struct Window{
	HWND handle;									//Fensterhandle
	WORD windowWidth = 800;							//Fensterbreite
	WORD windowHeight = 800;						//Fensterhöhe
	WINDOWFLAG flags = WINDOW_NONE;					//Fensterflags
	std::string windowClassName;					//Ja, jedes Fenster hat seine eigene Klasse... GROSSES TODO
	HGLRC glContext;								//OpenGL Context des Fensters
};

typedef LRESULT (*window_callback_function)(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK default_window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
ErrCode createWindow(Window& window, HINSTANCE hInstance, LONG windowWidth, LONG windowHeight, LONG x, LONG y, WORD pixelSize, const char* name = "Window", window_callback_function callback = default_window_callback, HWND parentWindow = NULL)noexcept{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		8,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	WNDCLASS window_class = {};
	window_class.hInstance = hInstance;
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	std::string className = "Window-Class" + std::to_string((unsigned long long)&window);	//TODO meh...
	window_class.lpszClassName = className.c_str();
	window_class.lpfnWndProc = callback;

	window.windowClassName = className;
	//Registriere Fenster Klasse
	if(!RegisterClass(&window_class)){
		std::cerr << "Register-Class: " << GetLastError() << std::endl;
		return CREATE_WINDOW;
	}

	RECT rect;
    rect.top = 0;
    rect.bottom = windowHeight;
    rect.left = 0;
    rect.right = windowWidth;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	DWORD w = rect.right - rect.left;
	DWORD h = rect.bottom - rect.top;

	//Erstelle das Fenster
	window.handle = CreateWindow(window_class.lpszClassName, name, WS_VISIBLE | WS_OVERLAPPEDWINDOW, x, y, w, h, parentWindow, NULL, hInstance, NULL);
	if(window.handle == NULL){
		std::cerr << "Create-Window: "<< GetLastError() << std::endl;
		return CREATE_WINDOW;
	}

	HDC context = GetDC(window.handle);
	int iPixelFormat = ChoosePixelFormat(context, &pfd);
	if(SetPixelFormat(context, iPixelFormat, &pfd) == FALSE){
		std::cerr << "Create-Window: "<< GetLastError() << std::endl;
		return CREATE_WINDOW;
	}
	HGLRC glContext = wglCreateContext(context);	//Handle GL Rendering Context
	if(wglMakeCurrent(context, glContext) == FALSE){
		std::cerr << "Create-Window: "<< GetLastError() << std::endl;
		return CREATE_WINDOW;
	}

	window.glContext = glContext;

	//TODO idk ob das so ok ist, win32 doku sagt nicht viel darüber aus... aber angeblich ist USERDATA
	//genau für sowas gedacht und es sollte auch nie überschrieben werden...
	SetWindowLongPtr(window.handle, GWLP_USERDATA, (LONG_PTR)&window);

	window.windowWidth = windowWidth;
	window.windowHeight = windowHeight;
	return SUCCESS;
}

//Zerstört das Fenster und alle allokierten Ressourcen mit diesem
//TODO Kontext noch löschen und so
ErrCode destroyWindow(Window& window)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == INVALID_HANDLE_VALUE) return WINDOW_NOT_FOUND;
	#endif
	if(DestroyWindow(window.handle) == 0){
		std::cerr << "DestroyWindow " << GetLastError() << std::endl;
		return GENERIC_ERROR;
	}
	if(wglDeleteContext(window.glContext) == FALSE){
		std::cerr << "wglDeleteContext " << GetLastError() << std::endl;
		return GENERIC_ERROR;
	}
	if(!UnregisterClassA(window.windowClassName.c_str(), NULL)){
		std::cerr << "UnregisterClassA " << GetLastError() << std::endl;
		return GENERIC_ERROR;
	}
	return SUCCESS;
}

ErrCode setWindowFlag(Window& window, WINDOWFLAG state)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	window.flags = (WINDOWFLAG)(window.flags | state);
	return SUCCESS;
}
ErrCode resetWindowFlag(Window& window, WINDOWFLAG state)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	window.flags = (WINDOWFLAG)(window.flags & ~state);
	return SUCCESS;
}
bool getWindowFlag(Window& window, WINDOWFLAG state)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return false;
	#endif
	return (window.flags & state);
}

//TODO Sollte ERRCODE zurückgeben und WINDOWFLAG als Referenzparameter übergeben bekommen
//Gibt den nächsten Zustand des Fensters zurück und löscht diesen anschließend, Anwendung z.B. while(state = getNextWindowState() != WINDOW_NONE)...
WINDOWFLAG getNextWindowState(Window& window)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NONE;
	#endif
	WINDOWFLAG flag = (WINDOWFLAG)(window.flags & -window.flags);
	window.flags = (WINDOWFLAG)(window.flags & ~flag);
	return flag;
}

ErrCode resizeWindow(Window& window, WORD width, WORD height, WORD pixel_size)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	wglMakeCurrent(GetDC(window.handle), window.glContext);
	window.windowWidth = width;
	window.windowHeight = height;
	glViewport(0, 0, width, height);
	return SUCCESS;
}

//TODO anstatt solch eine komplexe Funktion in createWindow rein zu geben, könnte man seine eigene schreiben mit Window* und uMsg,... als Parameter
//und diese default funktion ruft diese dann optional nur auf
LRESULT CALLBACK default_window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if(window == nullptr || window->handle == NULL) return DefWindowProc(hwnd, uMsg, wParam, lParam);	//TODO das ist ein Fehler, wie melden aber?
	switch(uMsg){
		case WM_CLOSE:
		case WM_DESTROY:{
			ErrCheck(setWindowFlag(*window, WINDOW_CLOSE), "setze close Fensterstatus");
			return 0;
		}
		case WM_SIZE:{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			if(!width || !height) break;
			ErrCheck(setWindowFlag(*window, WINDOW_RESIZE), "setzte resize Fensterstatus");
			ErrCheck(resizeWindow(*window, width, height, 1), "Fenster skalieren");
			break;
		}
		case WM_LBUTTONDOWN:{
			if(!getButton(mouse, MOUSE_LMB)){

			};
			setButton(mouse, MOUSE_LMB);
			break;
		}
		case WM_LBUTTONUP:{
			resetButton(mouse, MOUSE_LMB);
			break;
		}
		case WM_RBUTTONDOWN:{
			if(!getButton(mouse, MOUSE_RMB)){

			};
			setButton(mouse, MOUSE_RMB);
			break;
		}
		case WM_RBUTTONUP:{
			resetButton(mouse, MOUSE_RMB);
			break;
		}
		case WM_MOUSEMOVE:{
			mouse.x = GET_X_LPARAM(lParam);
			mouse.y = GET_Y_LPARAM(lParam);
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void getMessages(Window& window)noexcept{
	MSG msg;
	while(PeekMessage(&msg, window.handle, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

constexpr DWORD RGBA(BYTE r, BYTE g, BYTE b, BYTE a=255)noexcept{return DWORD(b|g<<8|r<<16|a<<24);}
constexpr BYTE A(DWORD color)noexcept{return BYTE(color>>24);}
constexpr BYTE R(DWORD color)noexcept{return BYTE(color>>16);}
constexpr BYTE G(DWORD color)noexcept{return BYTE(color>>8);}
constexpr BYTE B(DWORD color)noexcept{return BYTE(color);}

ErrCode clearWindow(Window& window)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	wglMakeCurrent(GetDC(window.handle), window.glContext);
	glClear(GL_COLOR_BUFFER_BIT);
	return SUCCESS;
}

ErrCode enableBlending(Window& window)noexcept{
	wglMakeCurrent(GetDC(window.handle), window.glContext);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return SUCCESS;
}

ErrCode disableBlending(Window& window)noexcept{
	wglMakeCurrent(GetDC(window.handle), window.glContext);
	glDisable(GL_BLEND);
	return SUCCESS;
}

//TODO in util.h packen
ErrCode loadShader(GLuint& shader, GLenum shaderType, const GLchar* code, GLint length)noexcept{
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &code, &length);
	glCompileShader(shader);
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		GLchar logBuffer[200];
		GLsizei charsWritten = 0;
		glGetShaderInfoLog(shader, sizeof(logBuffer), &charsWritten, logBuffer);
		std::cerr << logBuffer << std::endl;
		glDeleteShader(shader);
		return GENERIC_ERROR;	//TODO Fehlercode
	}
	return SUCCESS;
}

struct Image{
	DWORD* data = nullptr;
	WORD width = 0;		//x-Dimension
	WORD height = 0;	//y-Dimension
};

ErrCode loadImage(const char* name, Image& image)noexcept{
	std::fstream file;
	file.open(name, std::ios::in | std::ios::binary);
	if(!file.is_open()) return FILE_NOT_FOUND;
	file.read((char*)&image.width, 2);
	file.read((char*)&image.height, 2);
	image.data = new(std::nothrow) DWORD[image.width*image.height];
	if(!image.data) return BAD_ALLOC;
	char val[4];
	for(DWORD i=0; i < image.width*image.height; ++i){
		file.read(&val[0], 1);
		file.read(&val[1], 1);
		file.read(&val[2], 1);
		file.read(&val[3], 1);
		image.data[i] = RGBA(val[0], val[1], val[2], val[3]);
	}
	file.close();
	return SUCCESS;
}

void destroyImage(Image& image)noexcept{
	delete[] image.data;
	image.data = nullptr;
}

//-------------------------------Zeichen Operationen-------------------------------

ErrCode drawImage(Window& window, Image& image, WORD x1, WORD y1, WORD x2, WORD y2)noexcept{
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec2 pos;"
	"layout(location=1) in vec2 tex;"
	"out vec2 texCoord;"
	"void main(){"
	"   texCoord = tex;"
	"   gl_Position = vec4(pos, 1.0, 1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"in vec2 texCoord;"
	"uniform sampler2D texture0;"
	"void main(){"
	"   vec4 texColor = texture2D(texture0, texCoord);"
	"   fragColor = vec4(texColor);"
	"}";

	wglMakeCurrent(GetDC(window.handle), window.glContext);
    GLuint program = glCreateProgram();
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    if(ErrCheck(loadShader(vertexShader, GL_VERTEX_SHADER, vertexShaderCode, sizeof(vertexShaderCode)), "Vertex Shader laden") != SUCCESS) return GENERIC_ERROR;
    if(ErrCheck(loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderCode, sizeof(fragmentShaderCode)), "Fragment Shader laden") != SUCCESS) GENERIC_ERROR;
    glAttachShader(program, fragmentShader);
    glAttachShader(program, vertexShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	float startX = (float)x1/window.windowWidth*2-1;
	float startY = (float)(window.windowHeight-y1)/window.windowHeight*2-1;
	float endX = (float)x2/window.windowWidth*2-1;
	float endY = (float)(window.windowHeight-y2)/window.windowHeight*2-1;

    float vertices[] = {
        startX, endY,
        endX, endY,
        endX, startY,
        endX, startY,
        startX, startY,
        startX, endY
    };
    float texCoords[] = {
        0.f, 1.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 0.f,
        0.f, 0.f,
        0.f, 1.f
    };
    GLuint VBOPos, VBOTex, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOPos);
    glGenBuffers(1, &VBOTex);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOTex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(program);

    glUniform1i(glGetUniformLocation(program, "texture0"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOTex);
	glDeleteTextures(1, &tex);
	glDeleteProgram(program);
	return SUCCESS;
}

struct RectangleData{
	WORD x;
	WORD y;
	WORD dx;
	WORD dy;
	DWORD color;
};

ErrCode initDrawRectanglesProgram(){
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec4 pos;"
	"layout(location=1) in uint color;"
	"out vec4 vPos;"
	"out vec4 vColor;"
	"void main(){"
	"	vPos = pos;"
	"	float r = float((color>>16) & 0xFFu)/255.0;"
	"	float g = float((color>>8) & 0xFFu)/255.0;"
	"	float b = float((color) & 0xFFu)/255.0;"
	"	float a = float((color>>24) & 0xFFu)/255.0;"
	"	vColor = vec4(r, g, b, a);"
	"   gl_Position = vec4(1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"in vec4 color;"
	"void main(){"
	"   fragColor = vec4(color);"
	"}";
	const GLchar geometryShaderCode[] = 
	"#version 330\n"
	"layout(points) in;"
	"layout(triangle_strip, max_vertices = 4) out;"
	"in vec4 vPos[];"
	"in vec4 vColor[];"
	"out vec4 color;"
	"uniform vec2 wDimensions;"
	"void main(){"
	"	color = vColor[0];"
	"	float xMin = vPos[0].x;"
	"	float xMax = vPos[0].x+vPos[0].z;"
	"	float yMin = vPos[0].y;"
	"	float yMax = vPos[0].y+vPos[0].w;"
	"	float xMinPos = ((xMin*2.0)/wDimensions.x)-1.0;"
	"	float yMinPos = (((wDimensions.y-yMin)*2.0)/wDimensions.y)-1.0;"
	"	float xMaxPos = ((xMax*2.0)/wDimensions.x)-1.0;"
	"	float yMaxPos = (((wDimensions.y-yMax)*2.0)/wDimensions.y)-1.0;"
	"	gl_Position = vec4(xMinPos, yMinPos, 1.0, 1.0);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMinPos, yMaxPos, 1.0, 1.0);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMaxPos, yMinPos, 1.0, 1.0);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMaxPos, yMaxPos, 1.0, 1.0);"
	"	EmitVertex();"
	"	EndPrimitive();"
	"}";

    drawRectanglesProgram = glCreateProgram();
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
	GLuint geometryShader = 0;
    if(ErrCheck(loadShader(vertexShader, GL_VERTEX_SHADER, vertexShaderCode, sizeof(vertexShaderCode)), "Vertex Shader laden") != SUCCESS) return GENERIC_ERROR;
    if(ErrCheck(loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderCode, sizeof(fragmentShaderCode)), "Fragment Shader laden") != SUCCESS) return GENERIC_ERROR;
	if(ErrCheck(loadShader(geometryShader, GL_GEOMETRY_SHADER, geometryShaderCode, sizeof(geometryShaderCode)), "Geometry Shader laden") != SUCCESS) return GENERIC_ERROR;
    glAttachShader(drawRectanglesProgram, vertexShader);
	glAttachShader(drawRectanglesProgram, fragmentShader);
	glAttachShader(drawRectanglesProgram, geometryShader);
    glLinkProgram(drawRectanglesProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
	return SUCCESS;
}
ErrCode renderRectangles(Window& window, RectangleData* rectangles, DWORD count){
	wglMakeCurrent(GetDC(window.handle), window.glContext);

    GLuint VBOPos, VBOColor, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOPos);
	glGenBuffers(1, &VBOColor);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(RectangleData), rectangles, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(RectangleData), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOColor);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(RectangleData), rectangles, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RectangleData), (void*)8);

    glUseProgram(drawRectanglesProgram);

	glUniform2f(glGetUniformLocation(drawRectanglesProgram, "wDimensions"), window.windowWidth, window.windowHeight);

    glDrawArrays(GL_POINTS, 0, count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOColor);
	return SUCCESS;
}

struct CircleData{
	WORD x;
	WORD y;
	float radius;
	DWORD color;
};

ErrCode initDrawCirclesProgram(){
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec2 pos;"
	"layout(location=1) in float radius;"
	"layout(location=2) in uint color;"
	"out vec2 vPos;"
	"out float vRadius;"
	"out vec4 vColor;"
	"void main(){"
	"	vPos = pos;"
	"	vRadius = radius;"
	"	float r = float((color>>16) & 0xFFu)/255.0;"
	"	float g = float((color>>8) & 0xFFu)/255.0;"
	"	float b = float((color) & 0xFFu)/255.0;"
	"	float a = float((color>>24) & 0xFFu)/255.0;"
	"	vColor = vec4(r, g, b, a);"
	"   gl_Position = vec4(1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"in vec2 texCoord;"
	"in vec2 pos;"
	"in float radius;"
	"in vec4 color;"
	"void main(){"
	"	if(distance(pos, texCoord) > radius) discard;"
	"   fragColor = vec4(color);"
	"}";
	const GLchar geometryShaderCode[] = 
	"#version 330\n"
	"layout(points) in;"
	"layout(triangle_strip, max_vertices = 4) out;"
	"in vec2 vPos[];"
	"in float vRadius[];"
	"in vec4 vColor[];"
	"out vec2 texCoord;"
	"out vec2 pos;"
	"out float radius;"
	"out vec4 color;"
	"uniform vec2 wDimensions;"
	"void main(){"
	"	radius = vRadius[0];"
	"	color = vColor[0];"
	"	float xMin = vPos[0].x-vRadius[0];"
	"	float xMax = vPos[0].x+vRadius[0];"
	"	float yMin = vPos[0].y-vRadius[0];"
	"	float yMax = vPos[0].y+vRadius[0];"
	"	float xMinPos = ((xMin*2.0)/wDimensions.x)-1.0;"
	"	float yMinPos = (((wDimensions.y-yMin)*2.0)/wDimensions.y)-1.0;"
	"	float xMaxPos = ((xMax*2.0)/wDimensions.x)-1.0;"
	"	float yMaxPos = (((wDimensions.y-yMax)*2.0)/wDimensions.y)-1.0;"
	"	pos = vPos[0];"
	"	gl_Position = vec4(xMinPos, yMinPos, 1.0, 1.0);"
	"	texCoord = vec2(xMin, yMin);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMinPos, yMaxPos, 1.0, 1.0);"
	"	texCoord = vec2(xMin, yMax);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMaxPos, yMinPos, 1.0, 1.0);"
	"	texCoord = vec2(xMax, yMin);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMaxPos, yMaxPos, 1.0, 1.0);"
	"	texCoord = vec2(xMax, yMax);"
	"	EmitVertex();"
	"	EndPrimitive();"
	"}";

    drawCirclesProgram = glCreateProgram();
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
	GLuint geometryShader = 0;
    if(ErrCheck(loadShader(vertexShader, GL_VERTEX_SHADER, vertexShaderCode, sizeof(vertexShaderCode)), "Vertex Shader laden") != SUCCESS) return GENERIC_ERROR;
    if(ErrCheck(loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderCode, sizeof(fragmentShaderCode)), "Fragment Shader laden") != SUCCESS) return GENERIC_ERROR;
	if(ErrCheck(loadShader(geometryShader, GL_GEOMETRY_SHADER, geometryShaderCode, sizeof(geometryShaderCode)), "Geometry Shader laden") != SUCCESS) return GENERIC_ERROR;
    glAttachShader(drawCirclesProgram, vertexShader);
	glAttachShader(drawCirclesProgram, fragmentShader);
	glAttachShader(drawCirclesProgram, geometryShader);
    glLinkProgram(drawCirclesProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
	return SUCCESS;
}
ErrCode renderCircles(Window& window, CircleData* circles, DWORD count){
	wglMakeCurrent(GetDC(window.handle), window.glContext);

    GLuint VBOPos, VBOWidth, VBOColor, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOPos);
	glGenBuffers(1, &VBOWidth);
	glGenBuffers(1, &VBOColor);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(CircleData), circles, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(CircleData), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOWidth);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(CircleData), circles, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(CircleData), (void*)4);

	glBindBuffer(GL_ARRAY_BUFFER, VBOColor);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(CircleData), circles, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(CircleData), (void*)8);

    glUseProgram(drawCirclesProgram);

	glUniform2f(glGetUniformLocation(drawCirclesProgram, "wDimensions"), window.windowWidth, window.windowHeight);

    glDrawArrays(GL_POINTS, 0, count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOWidth);
	glDeleteBuffers(1, &VBOColor);
	return SUCCESS;
}

struct LineData{
	WORD x1;
	WORD y1;
	WORD x2;
	WORD y2;
	float width;
	DWORD color;
};

ErrCode initDrawLinesProgram(){
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec4 pos;"
	"layout(location=1) in float width;"
	"layout(location=2) in uint color;"
	"out vec4 vPos;"
	"out float vLineWidth;"
	"out vec4 vColor;"
	"void main(){"
	"	vPos = pos;"
	"	vLineWidth = width;"
	"	float r = float((color>>16) & 0xFFu)/255.0;"
	"	float g = float((color>>8) & 0xFFu)/255.0;"
	"	float b = float((color) & 0xFFu)/255.0;"
	"	float a = float((color>>24) & 0xFFu)/255.0;"
	"	vColor = vec4(r, g, b, a);"
	"   gl_Position = vec4(1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"in vec2 texCoord;"
	"in vec2 pos1;"
	"in vec2 pos2;"
	"in float lineWidth;"
	"in vec4 color;"
	"void main(){"
	"	vec2 dir = pos2-pos1;"
	"	float proj = dir.x*(texCoord.x-pos1.x)+dir.y*(texCoord.y-pos1.y);"
	"	float len = dir.x*dir.x+dir.y*dir.y;"
	"	float d = proj/len;"
	"	d = clamp(d, 0.f, 1.f);"
	"	vec2 projPt = pos1+dir*d;"
	"	vec2 diff = texCoord-projPt;"
	"	if(length(diff) > lineWidth) discard;"
	"   fragColor = vec4(color);"
	"}";
	const GLchar geometryShaderCode[] = 
	"#version 330\n"
	"layout(points) in;"
	"layout(triangle_strip, max_vertices = 4) out;"
	"in vec4 vPos[];"
	"in float vLineWidth[];"
	"in vec4 vColor[];"
	"out vec2 texCoord;"
	"out vec2 pos1;"
	"out vec2 pos2;"
	"out float lineWidth;"
	"out vec4 color;"
	"uniform vec2 wDimensions;"
	"void main(){"
	"	lineWidth = vLineWidth[0];"
	"	color = vColor[0];"
	"	float xMin = min(vPos[0].x, vPos[0].z)-vLineWidth[0];"
	"	float xMax = max(vPos[0].x, vPos[0].z)+vLineWidth[0];"
	"	float yMin = min(vPos[0].y, vPos[0].w)-vLineWidth[0];"
	"	float yMax = max(vPos[0].y, vPos[0].w)+vLineWidth[0];"
	"	float xMinPos = ((xMin*2.0)/wDimensions.x)-1.0;"
	"	float yMinPos = (((wDimensions.y-yMin)*2.0)/wDimensions.y)-1.0;"
	"	float xMaxPos = ((xMax*2.0)/wDimensions.x)-1.0;"
	"	float yMaxPos = (((wDimensions.y-yMax)*2.0)/wDimensions.y)-1.0;"
	"	pos1 = vPos[0].xy;"
	"	pos2 = vPos[0].zw;"
	"	gl_Position = vec4(xMinPos, yMinPos, 1.0, 1.0);"
	"	texCoord = vec2(xMin, yMin);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMinPos, yMaxPos, 1.0, 1.0);"
	"	texCoord = vec2(xMin, yMax);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMaxPos, yMinPos, 1.0, 1.0);"
	"	texCoord = vec2(xMax, yMin);"
	"	EmitVertex();"
	"	gl_Position = vec4(xMaxPos, yMaxPos, 1.0, 1.0);"
	"	texCoord = vec2(xMax, yMax);"
	"	EmitVertex();"
	"	EndPrimitive();"
	"}";

    drawLinesProgram = glCreateProgram();
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
	GLuint geometryShader = 0;
    if(ErrCheck(loadShader(vertexShader, GL_VERTEX_SHADER, vertexShaderCode, sizeof(vertexShaderCode)), "Vertex Shader laden") != SUCCESS) return GENERIC_ERROR;
    if(ErrCheck(loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderCode, sizeof(fragmentShaderCode)), "Fragment Shader laden") != SUCCESS) return GENERIC_ERROR;
	if(ErrCheck(loadShader(geometryShader, GL_GEOMETRY_SHADER, geometryShaderCode, sizeof(geometryShaderCode)), "Geometry Shader laden") != SUCCESS) return GENERIC_ERROR;
    glAttachShader(drawLinesProgram, vertexShader);
	glAttachShader(drawLinesProgram, fragmentShader);
	glAttachShader(drawLinesProgram, geometryShader);
    glLinkProgram(drawLinesProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
	return SUCCESS;
}
ErrCode renderLines(Window& window, LineData* lines, DWORD count){
	wglMakeCurrent(GetDC(window.handle), window.glContext);

    GLuint VBOPos, VBOWidth, VBOColor, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOPos);
	glGenBuffers(1, &VBOWidth);
	glGenBuffers(1, &VBOColor);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(LineData), lines, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(LineData), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOWidth);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(LineData), lines, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(LineData), (void*)8);

	glBindBuffer(GL_ARRAY_BUFFER, VBOColor);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(LineData), lines, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(LineData), (void*)12);

    glUseProgram(drawLinesProgram);

	glUniform2f(glGetUniformLocation(drawLinesProgram, "wDimensions"), window.windowWidth, window.windowHeight);

    glDrawArrays(GL_POINTS, 0, count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOWidth);
	glDeleteBuffers(1, &VBOColor);
	return SUCCESS;
}

ErrCode drawWindow(Window& window)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	wglMakeCurrent(GetDC(window.handle), window.glContext);
	if(SwapBuffers(GetDC(window.handle)) == FALSE){
		std::cerr << "SwapBuffers " << GetLastError() << std::endl;
		return GENERIC_ERROR;
	}
	return SUCCESS;
}

DWORD drawFontChar(Font& font, std::vector<LineData>& lines, BYTE character, WORD x, WORD y){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[character]];
    WORD startIdx = 0;
    for(SWORD i=0; i < glyph.numContours; ++i){
        WORD endIdx = glyph.endOfContours[i];
        for(WORD j=startIdx; j < endIdx; ++j){
            WORD end = j+1;
            lines.push_back({(WORD)(glyph.xCoords[j]+x), (WORD)(glyph.yCoords[j]+y), (WORD)(glyph.xCoords[end]+x), (WORD)(glyph.yCoords[end]+y), 1, RGBA(255, 255, 255)});
        }
        lines.push_back({(WORD)(glyph.xCoords[endIdx]+x), (WORD)(glyph.yCoords[endIdx]+y), (WORD)(glyph.xCoords[startIdx]+x), (WORD)(glyph.yCoords[startIdx]+y), 1, RGBA(255, 255, 255)});
        startIdx = endIdx+1;
    }
	return font.horMetricsCount > 1 ? font.horMetrics[font.asciiToGlyphMapping[character]].advanceWidth : font.horMetrics[0].advanceWidth;
}

DWORD drawFontString(Font& font, std::vector<LineData>& lines, const char* string, WORD x, WORD y){
	DWORD offset = 0;
	for(size_t i=0; i < strlen(string); ++i){
		offset += drawFontChar(font, lines, string[i], x+offset, y);
	}
	return offset;
}
