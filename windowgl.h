#pragma once

#include <windows.h>
#include <windowsx.h>
#include "glcorearb.h"
#include <fstream>

#include "util.h"
#include "math.h"

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

void init(){
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

ErrCode drawLine(Window& window, WORD x1, WORD y1, WORD x2, WORD y2, WORD width, DWORD color)noexcept{
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec2 pos;"
	"layout(location=1) in vec2 tex;"
	"out vec2 texCoord;"
	"void main(){"
	"	texCoord = tex;"
	"   gl_Position = vec4(pos, 1.0, 1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"in vec2 texCoord;"
	"uniform vec4 color;"
	"uniform vec2 pos1;"
	"uniform vec2 pos2;"
	"uniform int lineWidth;"
	"void main(){"
	"	vec2 dir = pos2-pos1;"
	"	float proj = dir.x*(texCoord.x-pos1.x)+dir.y*(texCoord.y-pos1.y);"
	"	float len = dir.x*dir.x+dir.y*dir.y;"
	"	float d = proj/len;"
	"	d = clamp(d, 0.f, 1.f);"
	"	vec2 projPt = pos1+dir*d;"
	"	vec2 diff = texCoord-projPt;"
	"	if(length(diff) > lineWidth) discard;"
	"   fragColor = vec4(color/255);"
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

	float xMin = x1;
	if(x2 < x1) xMin = x2;
	xMin -= width;
	float xMax = x1;
	if(x2 > x1) xMax = x2;
	xMax += width;
	float yMin = y1;
	if(y2 < y1) yMin = y2;
	yMin -= width;
	float yMax = y1;
	if(y2 > y1) yMax = y2;
	yMax += width;
	float startX = (float)xMin/window.windowWidth*2-1;
	float startY = (float)(window.windowHeight-yMin)/window.windowHeight*2-1;
	float endX = (float)xMax/window.windowWidth*2-1;
	float endY = (float)(window.windowHeight-yMax)/window.windowHeight*2-1;

    float vertices[] = {
        startX, endY,
        endX, endY,
        endX, startY,
        endX, startY,
        startX, startY,
        startX, endY
    };
    float texCoords[] = {
        (float)xMin, (float)yMax,
        (float)xMax, (float)yMax,
        (float)xMax, (float)yMin,
        (float)xMax, (float)yMin,
        (float)xMin, (float)yMin,
        (float)xMin, (float)yMax
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

    glUseProgram(program);

    glUniform4f(glGetUniformLocation(program, "color"), R(color), G(color), B(color), A(color));
	glUniform2f(glGetUniformLocation(program, "pos1"), x1, y1);
	glUniform2f(glGetUniformLocation(program, "pos2"), x2, y2);
	glUniform1i(glGetUniformLocation(program, "lineWidth"), width);
    glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOTex);
	glDeleteProgram(program);
	return SUCCESS;
}

ErrCode drawRectangle(Window& window, WORD x, WORD y, WORD dx, WORD dy, DWORD color)noexcept{
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec2 pos;"
	"void main(){"
	"   gl_Position = vec4(pos, 1.0, 1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"uniform vec4 color;"
	"void main(){"
	"   fragColor = vec4(color/255);"
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

	float startX = (float)x/window.windowWidth*2-1;
	float startY = (float)(window.windowHeight-y)/window.windowHeight*2-1;
	float endX = (float)(x+dx)/window.windowWidth*2-1;
	float endY = (float)(window.windowHeight-(y+dy))/window.windowHeight*2-1;

    float vertices[] = {
        startX, endY,
        endX, endY,
        endX, startY,
        endX, startY,
        startX, startY,
        startX, endY
    };
    GLuint VBOPos, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOPos);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(program);

    glUniform4f(glGetUniformLocation(program, "color"), R(color), G(color), B(color), A(color));
    glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteProgram(program);
	return SUCCESS;
}

ErrCode drawCircle(Window& window, WORD x, WORD y, WORD rad, DWORD color){
	const GLchar vertexShaderCode[] = 
	"#version 330\n"
	"layout(location=0) in vec2 pos;"
	"layout(location=1) in vec2 tex;"
	"out vec2 texCoord;"
	"void main(){"
	"	texCoord = tex;"
	"   gl_Position = vec4(pos, 1.0, 1.0);"
	"}";
	const GLchar fragmentShaderCode[] = 
	"#version 330\n"
	"out vec4 fragColor;"
	"in vec2 texCoord;"
	"uniform vec4 color;"
	"void main(){"
	"	if(length(texCoord) > 1) discard;"
	"   fragColor = vec4(color/255);"
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

	float startX = (float)(x-rad)/window.windowWidth*2-1;
	float startY = (float)(window.windowHeight-(y-rad))/window.windowHeight*2-1;
	float endX = (float)(x+rad)/window.windowWidth*2-1;
	float endY = (float)(window.windowHeight-(y+rad))/window.windowHeight*2-1;

    float vertices[] = {
        startX, endY,
        endX, endY,
        endX, startY,
        endX, startY,
        startX, startY,
        startX, endY
    };
    float texCoords[] = {
        -1.f, 1.f,
        1.f, 1.f,
        1.f, -1.f,
        1.f, -1.f,
        -1.f, -1.f,
        -1.f, 1.f
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

    glUseProgram(program);

    glUniform4f(glGetUniformLocation(program, "color"), R(color), G(color), B(color), A(color));
    glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBOPos);
	glDeleteBuffers(1, &VBOTex);
	glDeleteProgram(program);
	return SUCCESS;
}

ErrCode drawFontChar()noexcept{
	return SUCCESS;
}

ErrCode drawWindow(Window& window)noexcept{
	#ifdef INVALIDHANDLEERRORS
	if(window.handle == NULL) return WINDOW_NOT_FOUND;
	#endif
	if(SwapBuffers(GetDC(window.handle)) == FALSE){
		std::cerr << "SwapBuffers " << GetLastError() << std::endl;
		return GENERIC_ERROR;
	}
	return SUCCESS;
}
