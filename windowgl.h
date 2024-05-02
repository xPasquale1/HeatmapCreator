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
ErrCode initDrawImageProgram();
static volatile GLuint drawLinesProgram;
static volatile GLuint drawCirclesProgram;
static volatile GLuint drawRectanglesProgram;
static volatile GLuint drawImageProgram;
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
	if(ErrCheck(initDrawImageProgram(), "Draw Image initialisieren") != SUCCESS) return GENERIC_ERROR;
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

//TODO sollte auch als Geometry Shader umgesetzt werden
ErrCode initDrawImageProgram(){
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

    drawImageProgram = glCreateProgram();
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    if(ErrCheck(loadShader(vertexShader, GL_VERTEX_SHADER, vertexShaderCode, sizeof(vertexShaderCode)), "Vertex Shader laden") != SUCCESS) return GENERIC_ERROR;
    if(ErrCheck(loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderCode, sizeof(fragmentShaderCode)), "Fragment Shader laden") != SUCCESS) GENERIC_ERROR;
    glAttachShader(drawImageProgram, fragmentShader);
    glAttachShader(drawImageProgram, vertexShader);
    glLinkProgram(drawImageProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	return SUCCESS;
}
ErrCode drawImage(Window& window, Image& image, WORD x1, WORD y1, WORD x2, WORD y2)noexcept{
	wglMakeCurrent(GetDC(window.handle), window.glContext);

	float startX = (float)x1/window.windowWidth*2-1;
	float startY = (float)(window.windowHeight-y1)/window.windowHeight*2-1;
	float endX = (float)x2/window.windowWidth*2-1;
	float endY = (float)(window.windowHeight-y2)/window.windowHeight*2-1;

    float vertices[] = {
        startX, startY,
		startX, endY,
		endX, startY,
		endX, endY
    };
    float texCoords[] = {
        0.f, 0.f,
		0.f, 1.f,
		1.f, 0.f,
		1.f, 1.f
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

    glUseProgram(drawImageProgram);

    glUniform1i(glGetUniformLocation(drawImageProgram, "texture0"), 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOTex);
	glDeleteTextures(1, &tex);
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

DWORD getFontStringSize(Font& font, const char* string){
	DWORD size = 0;
	for(size_t i=0; i < strlen(string); ++i){
		size += font.horMetrics[font.asciiToGlyphMapping[string[i]]].advanceWidth;
	}
	return size;
}

//-------------------------------GUI-------------------------------

ErrCode _defaultEvent(void*)noexcept{return SUCCESS;}
enum BUTTONFLAGS{
	BUTTON_VISIBLE=1,
	BUTTON_CAN_HOVER=2,
	BUTTON_HOVER=4,
	BUTTON_PRESSED=8,
	BUTTON_TEXT_CENTER=16,
	BUTTON_DISABLED=32
};
struct Button{
	ErrCode (*event)(void*)noexcept = _defaultEvent;	//Funktionspointer zu einer Funktion die gecallt werden soll wenn der Button gedrückt wird
	std::string text;
	Image* image = nullptr;
	Image* disabled_image = nullptr;
	ivec2 pos = {0, 0};
	ivec2 repos = {0, 0};
	ivec2 size = {50, 10};
	ivec2 resize = {55, 11};
	BYTE flags = BUTTON_VISIBLE | BUTTON_CAN_HOVER;
	DWORD color = RGBA(120, 120, 120);
	DWORD hover_color = RGBA(120, 120, 255);
	DWORD textcolor = RGBA(180, 180, 180);
	DWORD disabled_color = RGBA(90, 90, 90);
	WORD textsize = 16;
	void* data = nullptr;
};

void destroyButton(Button& button)noexcept{
	destroyImage(*button.image);
}

inline constexpr void setButtonFlag(Button& button, BUTTONFLAGS flag)noexcept{button.flags |= flag;}
inline constexpr void resetButtonFlag(Button& button, BUTTONFLAGS flag)noexcept{button.flags &= ~flag;}
inline constexpr bool getButtonFlag(Button& button, BUTTONFLAGS flag)noexcept{return (button.flags & flag);}
//TODO kann bestimmt besser geschrieben werden... und ErrCheck aufs Event sollte mit einem BUTTONSTATE entschieden werden
inline void buttonsClicked(Button* buttons, WORD button_count)noexcept{
	for(WORD i=0; i < button_count; ++i){
		Button& b = buttons[i];
		if(!getButtonFlag(b, BUTTON_VISIBLE) || getButtonFlag(b, BUTTON_DISABLED)) continue;
		ivec2 delta = {mouse.x - b.pos.x, mouse.y - b.pos.y};
		if(delta.x >= 0 && delta.x <= b.size.x && delta.y >= 0 && delta.y <= b.size.y){
			if(getButtonFlag(b, BUTTON_CAN_HOVER)) b.flags |= BUTTON_HOVER;
			if(getButton(mouse, MOUSE_LMB) && !getButtonFlag(b, BUTTON_PRESSED)){
				ErrCheck(b.event(b.data));
				b.flags |= BUTTON_PRESSED;
			}
			else if(!getButton(mouse, MOUSE_LMB)) b.flags &= ~BUTTON_PRESSED;
		}else if(getButtonFlag(b, BUTTON_CAN_HOVER)){
			b.flags &= ~BUTTON_HOVER;
		}
	}
}

inline ErrCode drawButtons(Window& window, Font& font, std::vector<LineData>& lines, std::vector<RectangleData>& rectangles, Button* buttons, WORD button_count)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	for(WORD i=0; i < button_count; ++i){
		Button& b = buttons[i];
		if(!getButtonFlag(b, BUTTON_VISIBLE)) continue;
		if(getButtonFlag(b, BUTTON_DISABLED)){
			if(b.disabled_image == nullptr)
				rectangles.push_back({(WORD)b.pos.x, (WORD)b.pos.y, (WORD)b.size.x, (WORD)b.size.y, b.disabled_color});
			else
				drawImage(window, *b.disabled_image, b.pos.x, b.pos.y, b.pos.x+b.size.x, b.pos.y+b.size.y);
		}else if(b.image == nullptr){
			if(getButtonFlag(b, BUTTON_CAN_HOVER) && getButtonFlag(b, BUTTON_HOVER))
				rectangles.push_back({(WORD)b.pos.x, (WORD)b.pos.y, (WORD)b.size.x, (WORD)b.size.y, b.hover_color});
			else
				rectangles.push_back({(WORD)b.pos.x, (WORD)b.pos.y, (WORD)b.size.x, (WORD)b.size.y, b.color});
		}else{
			if(getButtonFlag(b, BUTTON_CAN_HOVER) && getButtonFlag(b, BUTTON_HOVER))
				drawImage(window, *b.image, b.repos.x, b.repos.y, b.repos.x+b.resize.x, b.repos.y+b.resize.y);
			else
				drawImage(window, *b.image, b.pos.x, b.pos.y, b.pos.x+b.size.x, b.pos.y+b.size.y);
		}
		if(getButtonFlag(b, BUTTON_TEXT_CENTER)){
			DWORD offset = 0;
			// WORD tmp_font_size = font.font_size;
			// font.font_size = b.textsize;
			DWORD str_size = getFontStringSize(font, b.text.c_str());
			for(size_t i=0; i < b.text.size(); ++i){
				offset += drawFontChar(font, lines, b.text[i], b.pos.x+offset+b.size.x/2-str_size/2, b.pos.y+b.size.y/2-b.textsize/2);
			}
			// font.font_size = tmp_font_size;
		}else{
			DWORD offset = 0;
			// WORD tmp_font_size = font.font_size;
			// font.font_size = b.textsize;
			for(size_t i=0; i < b.text.size(); ++i){
				offset += drawFontChar(font, lines, b.text[i], b.pos.x+offset, b.pos.y+b.size.y/2-b.textsize/2);
			}
			// font.font_size = tmp_font_size;
		}
	}
	return SUCCESS;
}

inline void updateButtons(Window& window, Font& font, std::vector<LineData>& lines, std::vector<RectangleData>& rectangles, Button* buttons, WORD button_count)noexcept{
	buttonsClicked(buttons, button_count);
	drawButtons(window, font, lines, rectangles, buttons, button_count);
}

struct Label{
	std::string text;
	ivec2 pos = {0, 0};
	DWORD textcolor = RGBA(180, 180, 180);
	WORD text_size = 2;
};

enum MENUFLAGS{
	MENU_OPEN=1,
	MENU_OPEN_TOGGLE=2
};
#define MAX_BUTTONS 10
#define MAX_STRINGS 20
#define MAX_IMAGES 5
struct Menu{
	Image* images[MAX_IMAGES];	//Sind für die Buttons
	BYTE image_count = 0;
	Button buttons[MAX_BUTTONS];
	BYTE button_count = 0;
	BYTE flags = MENU_OPEN;		//Bits: offen, toggle bit für offen, Rest ungenutzt
	ivec2 pos = {};				//TODO Position in Bildschirmpixelkoordinaten
	Label labels[MAX_STRINGS];
	BYTE label_count = 0;
};

void destroyMenu(Menu& menu)noexcept{
	for(WORD i=0; i < menu.image_count; ++i){
		destroyImage(*menu.images[i]);
	}
}

inline constexpr void setMenuFlag(Menu& menu, MENUFLAGS flag)noexcept{menu.flags |= flag;}
inline constexpr void resetMenuFlag(Menu& menu, MENUFLAGS flag)noexcept{menu.flags &= ~flag;}
inline constexpr bool getMenuFlag(Menu& menu, MENUFLAGS flag)noexcept{return (menu.flags&flag);}

inline void updateMenu(Window& window, Menu& menu, Font& font, std::vector<LineData>& lines, std::vector<RectangleData>& rectangles)noexcept{
	if(getMenuFlag(menu, MENU_OPEN)){
		updateButtons(window, font, lines, rectangles, menu.buttons, menu.button_count);
		for(WORD i=0; i < menu.label_count; ++i){
			Label& label = menu.labels[i];
			DWORD offset = 0;
			for(size_t j=0; j < label.text.size(); ++j){
				// WORD tmp = font.font_size;
				// font.font_size = label.text_size;
				offset += drawFontChar(font, lines, label.text[j], label.pos.x+offset, label.pos.y);
				// font.font_size = tmp;
			}
		}
	}
}
