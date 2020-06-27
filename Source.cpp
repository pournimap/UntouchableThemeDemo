#include <windows.h>
#include <windowsx.h> //For GET_X_LPARAM and GET_Y_LPARAM

#include <stdio.h> // for FILE I/O

#include <gl\glew.h>//use it before other GL.h

#include <gl/GL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//here using glm library instead of vmath library

#include "vmath.h"
#include "camera.h" 

#include "Header.h"
#include "Sphere.h"
#include "Obj_Loader.h"
#include "MyCylinder.h"
#include "MyStack.h"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")
#pragma comment (lib, "Winmm.lib")


const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLuint gVertexShaderObjectShader;
GLuint gFragmentShaderObjectShader;
GLuint gShaderProgramObjectShader;

GLuint gVertexShaderObjectSimpleDepthShader;
GLuint gFragmentShaderObjectSimpleDepthShader;
GLuint gShaderProgramObjectSimpleDepthShader;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

//Shadow Mapping Uniform
GLuint gdiffuseTextureUniform;
GLuint gshadowMapUniform;
GLuint glightPosUniform;
GLuint gviewPosUniform;
GLuint gprojectionUniform;
GLuint gviewUniform;
GLuint gmodelUniform;
GLuint glightSpaceMatrixUniform;

//SimpleDepthShader Uniform
GLuint glightSpaceMatrixSimpleDepthShaderUniform;
GLuint gmodelSimpleDepthShaderUniform;

GLuint planeVAO;
GLuint planeVBOposition;
GLuint planeVBOnormal;
GLuint planeVBOtexture;

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_texture;
GLuint gVbo_sphere_element;

GLuint gNumElements;
GLuint gNumVertices;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint cubeVAO;
GLuint cubeVBOposition;
GLuint cubeVBOnormal;
GLuint cubeVBOtexture;

GLuint depthMapFBO;
GLuint godraysFBO;
GLuint depthMap;
GLuint depth_debug_tex;
GLuint woodTexture;
GLuint whiteTexture;

glm::mat4 gPerspectiveProjectionMatrix;

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f, -8.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

bool FirstMouse = true;

GLfloat lastX = (float)SCR_WIDTH / 2.0;
GLfloat lastY = (float)SCR_HEIGHT / 2.0;

float xLightPosMove = 0.0f;
float yLightPosMove = 0.0f;
float zLightPosMove = 0.0f;

float yPositionMove = 0.0f;
float xPositionMove = 0.0f;
float zPositionMove = -3.0f;

std::vector<float> gv_vertices_1, gv_textures_1, gv_normals_1;

GLuint gVao_AstroMedicomp;
GLuint gVbo_Position, gVbo_Normal, gVbo_Texture;
int count_of_vertices_Astromedicomp;

float gAngle = 1.0f;
float gWidth, gHeight;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint postPassProgram;
GLuint colorProgram;

//GLuint gTex_depthUniform_ShowLightDepth;
GLuint gDecayUniform_postPass, gExposureUniform_postPass, gNumSamplesUniform_postPass, gOcclusionTextureUniform_postPass,
gScreenSpaceSunPosUniform_postPass, gDensityUniform_postPass, gWeightUniform_postPass;

GLuint gColorUniform_Color, gModelMatrixUniform_Color, gViewMatrixUniform_Color, gProjMatrixUniform_Color;
void InitPostProcessProgramShaders();
void InitColorProgramShaders();

void InitQuad();
void displayQuad();
GLuint gVao_quad, gVbo_quad_normal, gVbo_quad_position;

struct PointLight
{
	vec3 u_La;
	vec3 u_Ld;
	vec3 u_Ls;
	float DiffuseIntensity;
	float u_constant;
	float u_linear;
	float u_quadratic;
	vec3 position;
	PointLight()
	{
		position = vec3(0.0, 0.0, 0.0);
		u_constant = 0.0f;
		u_linear = 0.0f;
		u_quadratic = 0.0f;
		u_La = vec3(0.0, 0.0, 0.0);
		u_Ld = vec3(0.0, 0.0, 0.0);
		u_Ls = vec3(0.0, 0.0, 0.0);
	}
};

struct PointLightUniform
{
	GLuint u_La;
	GLuint u_Ld;
	GLuint u_Ls;
	GLuint u_constant;
	GLuint u_linear;
	GLuint u_quadratic;
	GLuint position;
	GLuint DiffuseIntensity;
};
PointLightUniform m_pointLightsLocation[3];
GLuint gNumPointLightsUniform;

struct SpotLight
{
	vec3 u_La;
	vec3 u_Ld;
	vec3 u_Ls;
	float u_constant;
	float u_linear;
	float u_quadratic;
	vec3 position;
	float diffuseIntensity;
	vec3 u_light_direction;
	float u_cutoff;
	float u_outerCutOff;
	float u_exponent;

	SpotLight()
	{
		position = vec3(0.0, 0.0, 0.0);
		u_constant = 0.0f;
		u_linear = 0.0f;
		u_quadratic = 0.0f;
		u_La = vec3(0.0, 0.0, 0.0);
		u_Ld = vec3(0.0, 0.0, 0.0);
		u_Ls = vec3(0.0, 0.0, 0.0);
		diffuseIntensity = 0.0;
		u_light_direction = vec3(0.0, 0.0, 0.0);
		u_cutoff = 0.0f;
		u_outerCutOff = 0.0f;
	}
};

struct SpotLightUniform
{
	GLuint u_La;
	GLuint u_Ld;
	GLuint u_Ls;
	GLuint u_constant;
	GLuint u_linear;
	GLuint u_quadratic;
	GLuint position;
	GLuint diffuseIntensity;
	GLuint u_light_direction;
	GLuint u_cutoff;
	GLuint u_outerCutOff;
	GLuint u_exponent;
};

int lightSequence = 1;
SpotLightUniform m_spotLightsLocation[3];
GLuint gNumSpotLightsUniform;
GLuint gIsPointLightUniform, gIsSpotLightUniform;

bool isAnimationStart = false;
GLuint gWhichSceneUniform;
GLuint gWhichColorUniform;
int gWhichColor = 0;
bool gAll = false;
GLuint gAllColorUniform;
bool leftTransition = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void Update(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGLPP");
	bool bDone = false;

	//code
	fopen_s(&gpFile, gszLogFileName, "w");
	fprintf_s(gpFile, "Log File Is Successfully Opened.\n");
	fclose(gpFile);

	//initializing members of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	//Registering Class
	RegisterClassEx(&wndclass);

	//Create Window
	//Parallel to glutInitWindowSize(), glutInitWindowPosition() and glutCreateWindow() all three together
	hwnd = CreateWindow(szClassName,
		TEXT("OpenGL Programmable Pipeline Window"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		SCR_WIDTH,
		SCR_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	initialize();

	//Message Loop
	while (bDone == false) //Parallel to glutMainLoop();
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// rendring function
			display();
			Update();
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true) //Continuation to glutLeaveMainLoop();
					bDone = true;
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	void MouseMovement(double xpos, double ypos);

	static bool bIsAKeyPressed = false;
	static bool bIsLKeyPressed = false;

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) //if 0, the window is active
			gbActiveWindow = true;
		else //if non-zero, the window is not active
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE: //Parallel to glutReshapeFunc();
		resize(LOWORD(lParam), HIWORD(lParam)); //Parallel to glutReshapeFunc(resize);
		break;
	case WM_MOUSEMOVE:
		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);
		break;
	case WM_KEYDOWN: //Parallel to glutKeyboardFunc();
		switch (wParam)
		{
		case VK_ESCAPE: //case 27
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true; //Parallel to glutLeaveMainLoop();
			break;
		case WM_MOUSEWHEEL:
			break;
		case 0x41://A
			isAnimationStart = !isAnimationStart;
			break;
		case 0x43://C
			xLightPosMove += 0.1f;
			break;
		case 0x44://D
			xLightPosMove -= 0.1f;
			break;
		case 0x46: //for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		case 0x5A: //z
			//print
			fopen_s(&gpFile, "log.txt", "a+");
			fprintf(gpFile, "xPosMove = %f, yPosMove = %f, zPosMove = %f\n", xLightPosMove, yLightPosMove, zLightPosMove);
			fprintf(gpFile, "Eye = %f, %f, %f\n center = %f, %f, %f\n, Up = %f, %f, %f\n", camera.Position[0], camera.Position[1], camera.Position[2], camera.Target[0], camera.Target[1], camera.Target[2], camera.Up[0], camera.Up[1], camera.Up[2]);
			fclose(gpFile);
			break;
		case 0x48://H
			PlaySound(MAKEINTRESOURCE(THEMESONG), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
			leftTransition = true;
			break;
		case 0x49://I
			zLightPosMove += 0.1f;
			break;
		case 0x4A: //J
			zLightPosMove -= 0.1f;
			break;
		case 0x4B: //k
			yLightPosMove -= 0.1f;
			break;
		case 0x4C: //L
			yLightPosMove += 0.1f;
			break;
		case 0x4D: //M
			yPositionMove -= 1.0f;
			break;
		case 0x4E: //N
			yPositionMove += 1.0f;
			break;
		case 0x50: //p
			zPositionMove -= 1.0f;
			break;
		case 0x51: //q
			if (gAll == false)
			{
				gAll = true;
				//gAllColorUniform = true;
			}
			else
			{
				gAll = false;
				//gAllColorUniform = false;
			}
			break;
		case 0x52://R for Red
			gWhichColor = 0;
			break;
		case 0x47://G for Green
		//gAngle += 0.1f;
			gWhichColor = 1;
			break;
		case 0x42://B for Blue
			gWhichColor = 2;
			//zPositionMove -= 1.0f;
			break;

		case 0x54: //T
			//gAngle -= 0.1f;
			yLightPosMove += 0.1f;
			break;
		case 0x55:

			camera.MoveCamera(CameraMoveOnY, 0.1f, 45.0f);
			break;
		case 0x56:
			camera.MoveCamera(CameraMoveOnY, -0.1f, 45.0f);
			break;

		case 0x25: //%
			camera.MoveCamera(CameraMoveOnX, 0.1f, 45.0f);
			break;
		case 0x26:
			camera.MoveCamera(CameraMoveOnZ, 0.1f, 45.0f);
			break;
		case 0x27:
			camera.MoveCamera(CameraMoveOnX, -0.1f, 45.0f);
			break;
		case 0x28:
			camera.MoveCamera(CameraMoveOnZ, -0.1f, 45.0f);
			break;

		case 0x31://1
			lightSequence = 1;
			camera = Camera(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 0x32: // 2
			lightSequence = 2;
			camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 0x33: //3
			lightSequence = 3;
			camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:  //Parallel to glutMouseFunc();
		break;
	case WM_CLOSE: //Parallel to glutCloseFunc();
		uninitialize(); //Parallel to glutCloseFunc(uninitialize);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
void MouseMovement(double xpos, double ypos)
{

	if (FirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		FirstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void ToggleFullscreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

//FUNCTION DEFINITIONS
void initialize(void)
{
	void ShadowMappingShaderBuildNCompileShader();
	void SimpleDepthShaderBuildNCompileShader();

	void uninitialize(void);
	void resize(int, int);

	int LoadGLTextures(GLuint*, TCHAR[]);
	void GenerateDepthMap(void);

	void InitPlane();
	void InitSphere();
	void InitAstromedicompModel();
	void InitCube();
	void ToggleFullscreen();
	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization of structure 'PIXELFORMATDESCRIPTOR'
	//Parallel to glutInitDisplayMode()
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	//choose a pixel format which best matches with that of 'pfd'
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//set the pixel format chosen above
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//make the rendering context created above as current n the current hdc
	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf_s(gpFile, "%s\n", glGetString(GL_VERSION));
	fprintf_s(gpFile, "Shading Lang Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fclose(gpFile);

	glEnable(GL_DEPTH_TEST);

	ShadowMappingShaderBuildNCompileShader();
	SimpleDepthShaderBuildNCompileShader();

	InitPostProcessProgramShaders();
	InitColorProgramShaders();

	InitPlane();
	InitSphere();
	InitCube();
	InitQuad();
	count_of_vertices_Astromedicomp = LoadMeshData("resources/AstroHeightobj.obj", gv_vertices_1, gv_textures_1, gv_normals_1);
	InitAstromedicompModel();

	// load textures
	// -------------


	LoadGLTextures(&whiteTexture, MAKEINTRESOURCE(IDBITMAP_WHITE));
	LoadGLTextures(&woodTexture, MAKEINTRESOURCE(IDBITMAP_WOOD));


	// configure depth map FBO
	// -----------------------
	GenerateDepthMap();

	glUseProgram(gShaderProgramObjectShader);
	glUniform1i(gdiffuseTextureUniform, 0);
	glUniform1i(gshadowMapUniform, 1);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	gWidth = SCR_WIDTH;
	gHeight = SCR_HEIGHT;


	resize(SCR_WIDTH, SCR_HEIGHT);
	ToggleFullscreen();

}

void GenerateDepthMap(void)
{
	//Create a framebuffer object for rendering depth map
	glGenFramebuffers(1, &depthMapFBO);

	//Create 2D texture that we'll use as framebuffer's depth buffer

	//Create depth texture
	glGenTextures(1, &depthMap);
	//glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf_s(gpFile, "depth_fbo frameBuffer  successful\n");
	fclose(gpFile);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &godraysFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, godraysFBO);
	glGenTextures(1, &depth_debug_tex);
	glBindTexture(GL_TEXTURE_2D, depth_debug_tex);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depth_debug_tex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "godraysFBO frameBuffer  Un successful.\n");
		fclose(gpFile);
	}
	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf_s(gpFile, "godraysFBO frameBuffer  successful\n");
	fclose(gpFile);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void InitPlane()
{

	float planeVertices[] = {
		// positions            // normals         // texcoords
		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	//position
	glGenBuffers(1, &planeVBOposition);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBOposition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);


	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &planeVBOnormal);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBOnormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &planeVBOtexture);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBOtexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_TEXTURE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
void InitAstromedicompModel()
{
	/*****************VAO For Cube*****************/
	glGenVertexArrays(1, &gVao_AstroMedicomp);
	glBindVertexArray(gVao_AstroMedicomp);

	/*****************Cube Position****************/
	glGenBuffers(1, &gVbo_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
	glBufferData(GL_ARRAY_BUFFER, gv_vertices_1.size() * sizeof(float), &gv_vertices_1[0], GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*****************Cube Normal****************/
	glGenBuffers(1, &gVbo_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal);
	glBufferData(GL_ARRAY_BUFFER, gv_normals_1.size() * sizeof(float), &gv_normals_1[0], GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenBuffers(1, &gVbo_Texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture);
	glBufferData(GL_ARRAY_BUFFER, gv_textures_1.size() * sizeof(float), &gv_textures_1[0], GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_TEXTURE);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);
}

void InitSphere()
{
	//Sphere sphere;
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();



	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	//position
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//normal
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_sphere_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_textures), sphere_textures, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_TEXTURE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
}

void InitCube(void)
{
	float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
															  // front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
	   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	   -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 // top face
		 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &cubeVBOposition);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOposition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &cubeVBOnormal);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOnormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &cubeVBOtexture);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOtexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_TEXTURE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//glm::vec3 sunDir(2.0f, 0.0f, -10.0f);
//vec3 sunDir(2.0f, 5.0f, -10.0f);
vec3 sunDir(32.2f, 4.5f, -16.0f);

static bool leftCompleted = false;
static bool rightCompleted = false;
static bool bDoneLeftRightAnimation = false;
void renderSun()
{
	void renderSphere();
	glUseProgram(colorProgram);

	mat4 modelMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	//mat4 viewMatrix = mat4::identity();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 13.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 10.0f, 13.0f), glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	
	modelMatrix = vmath::translate(vec3(sunDir[0] + xLightPosMove, sunDir[1] + yLightPosMove, sunDir[2] + zLightPosMove));
	if (leftTransition == true)
	{
		xLightPosMove -= 0.02f;
		if (xLightPosMove <= -57.0f)
		{
			leftTransition = false;
			leftCompleted = true;
		}

	}
	else if (leftCompleted == true)
	{
		xLightPosMove += 0.02f;
		if (xLightPosMove >= -0.6f)
		{
			leftCompleted = false;
			rightCompleted = true;
		}
	}
	else if (rightCompleted == true)
	{
		xLightPosMove -= 0.02f;
		if (xLightPosMove <= -32.0f)
		{
			rightCompleted = false;
			bDoneLeftRightAnimation = true;
		}
	}

	glUniformMatrix4fv(gModelMatrixUniform_Color, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform_Color, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(gProjMatrixUniform_Color, 1, GL_FALSE, &gPerspectiveProjectionMatrix[0][0]);
	glUniform3fv(gColorUniform_Color, 1, vec3(0.98, 0.83, 0.25));


	glDisable(GL_DEPTH_TEST);

	//renderSphere();
	glBindVertexArray(gVao_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);
}

glm::vec4 vec3Transform(glm::vec3& vSrcVector, glm::mat4& matSrcMatrix)
{
	glm::vec4 vDstVector;
	vDstVector[0] = matSrcMatrix[0][0] * vSrcVector[0] +
		matSrcMatrix[1][0] * vSrcVector[1] +
		matSrcMatrix[2][0] * vSrcVector[2] +
		matSrcMatrix[3][0];
	vDstVector[1] = matSrcMatrix[0][1] * vSrcVector[0] +
		matSrcMatrix[1][1] * vSrcVector[1] +
		matSrcMatrix[2][1] * vSrcVector[2] +
		matSrcMatrix[3][1];
	vDstVector[2] = matSrcMatrix[0][2] * vSrcVector[0] +
		matSrcMatrix[1][2] * vSrcVector[1] +
		matSrcMatrix[2][2] * vSrcVector[2] +
		matSrcMatrix[3][2];
	vDstVector[3] = matSrcMatrix[0][3] * vSrcVector[0] +
		matSrcMatrix[1][3] * vSrcVector[1] +
		matSrcMatrix[2][3] * vSrcVector[2] +
		matSrcMatrix[3][3];
	return vDstVector;
}

glm::vec4 vec4Transform(glm::vec4& vSrcVector, glm::mat4& matSrcMatrix)
{
	glm::vec4 vDstVector;
	vDstVector[0] = matSrcMatrix[0][0] * vSrcVector[0] +
		matSrcMatrix[1][0] * vSrcVector[1] +
		matSrcMatrix[2][0] * vSrcVector[2] +
		matSrcMatrix[3][0] * vSrcVector[3];
	vDstVector[1] = matSrcMatrix[0][1] * vSrcVector[0] +
		matSrcMatrix[1][1] * vSrcVector[1] +
		matSrcMatrix[2][1] * vSrcVector[2] +
		matSrcMatrix[3][1] * vSrcVector[3];
	vDstVector[2] = matSrcMatrix[0][2] * vSrcVector[0] +
		matSrcMatrix[1][2] * vSrcVector[1] +
		matSrcMatrix[2][2] * vSrcVector[2] +
		matSrcMatrix[3][2] * vSrcVector[3];
	vDstVector[3] = matSrcMatrix[0][3] * vSrcVector[0] +
		matSrcMatrix[1][3] * vSrcVector[1] +
		matSrcMatrix[2][3] * vSrcVector[2] +
		matSrcMatrix[3][3] * vSrcVector[3];
	return vDstVector;
}
glm::vec2 getScreenSpaceSunPos()
{
	
	glm::vec4 sunDir = glm::vec4(32.2f + xLightPosMove, 4.5f + yLightPosMove, -16.0f + zLightPosMove, 1.0f);

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	//mat4 translateM = translate(3.0f, 0.0f, -10.0f);
	//glm::mat4 viewMatrix = glm::mat4();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 10.0f, 13.0f), glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = gPerspectiveProjectionMatrix;

	glm::vec4 v = vec4Transform(sunDir, model);
	v = vec4Transform(v, viewMatrix);

	v = vec4Transform(v, projection);

	v = glm::vec4(v[0] / v[3], v[1] / v[3], v[2] / v[3], v[3] / v[3]);


	v = v + glm::vec4(1.0, 1.0, 0.0, 0.0);

	v = glm::vec4(v[0] * 0.5, v[1] * 0.5, v[2] * 0.5, v[3] * 0.5);

	return (glm::vec2(v[0], v[1]));

}
void display(void)
{
	void renderPlane();
	void renderScene(const GLuint & shader, GLuint & NewModel);
	//glm::vec3 lightPos(xLightPosMove, yLightPosMove, zLightPosMove);
	glm::vec3 lightPos(sunDir[0] + xLightPosMove, sunDir[1] + yLightPosMove, sunDir[2] + zLightPosMove);
	//sunDir

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	//float near_plane = 1.0f, far_plane = 7.5f;

	float near_plane = 0.1f, far_plane = 100.0f;
	//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); 
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

	lightSpaceMatrix = lightProjection * lightView;

	// render scene from light's point of view
	glUseProgram(gShaderProgramObjectSimpleDepthShader);
	glUniformMatrix4fv(glightSpaceMatrixSimpleDepthShaderUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
	renderScene(gShaderProgramObjectSimpleDepthShader, gmodelSimpleDepthShaderUniform);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, godraysFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glm::mat4 view1 = camera.getViewMatrix();
	//glm::mat4 view1 = glm::lookAt(glm::vec3(0.0f, 10.0f, 18.0f), glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	renderSun();
	glUseProgram(colorProgram);
	glUniform3fv(gColorUniform_Color, 1, vec3(0.0, 0.0, 0.0));
	glUniformMatrix4fv(gViewMatrixUniform_Color, 1, GL_FALSE, &view1[0][0]);
	glUniformMatrix4fv(gProjMatrixUniform_Color, 1, GL_FALSE, &gPerspectiveProjectionMatrix[0][0]);


	renderScene(colorProgram, gModelMatrixUniform_Color);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset viewport
	glViewport(0, 0, gWidth, gHeight);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map  
	// --------------------------------------------------------------

	glViewport(0, 0, gWidth, gHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderSun();

	glUseProgram(gShaderProgramObjectShader);

	glUniform1i(gNumPointLightsUniform, 3);
	glUniform1i(gNumSpotLightsUniform, 3);

	glUniform1i(gWhichSceneUniform, lightSequence);

	float PI = 3.14;

	PointLight p1[3];
	p1[0].u_La = vec3(0.00f, 0.00f, 0.00f);
	p1[0].u_Ld = vec3(1.0f, 0.0f, 0.0f);
	p1[0].u_Ls = vec3(1.0f, 0.0f, 0.0f);
	p1[0].u_linear = 0.1;
	p1[0].u_constant = 0.0;
	p1[0].u_quadratic = 0.0;
	p1[0].DiffuseIntensity = 0.5f;
	if (isAnimationStart == true)
	{
		p1[0].position = vec3(0.0, 5.0f * cos(2 * PI * gAngle), 5.0f * sin(2 * PI * gAngle));
	}
	else
	{
		p1[0].position = vec3(2.0f, 0.0f, 1.0f);
	}
	//p1[0].position = vec3(0.0001 * cos(gAngle), -0.0f, 0.0001 * sin(gAngle));

	p1[1].u_La = vec3(0.00f, 0.0f, 0.0f);
	p1[1].u_Ld = vec3(0.0f, 1.0f, 0.0f);
	p1[1].u_Ls = vec3(0.0f, 1.0f, 0.0f);
	p1[1].u_linear = 0.1;
	p1[1].DiffuseIntensity = 0.5f;
	if (isAnimationStart == true)
	{
		p1[1].position = vec3(5.0f * cos(2 * PI * gAngle), 5.0f * sin(2 * PI * gAngle), 0.0);
	}
	else
	{
		p1[1].position = vec3(0.0f, 0.0f, -1.0);
	}

	p1[2].u_La = vec3(0.0f, 0.0f, 0.0f);
	p1[2].u_Ld = vec3(0.0f, 0.0f, 1.0f);
	p1[2].u_Ls = vec3(0.0f, 0.0f, 1.0f);
	p1[2].u_linear = 0.1;
	p1[2].DiffuseIntensity = 0.5f;
	if (isAnimationStart == true)
	{
		p1[2].position = vec3(5.0f * cos(2 * PI * gAngle), 0.0f, 5.0f * sin(2 * PI * gAngle));
	}
	else
	{
		p1[2].position = vec3(-2.0f, 0.0f, 1.0);
	}
	for (int i = 0; i < 3; i++)
	{
		glUniform3fv(m_pointLightsLocation[i].u_La, 1, p1[i].u_La);
		glUniform3fv(m_pointLightsLocation[i].u_Ls, 1, p1[i].u_Ls);
		glUniform3fv(m_pointLightsLocation[i].u_Ld, 1, p1[i].u_Ld);
		glUniform1f(m_pointLightsLocation[i].DiffuseIntensity, p1[i].DiffuseIntensity);

		glUniform3fv(m_pointLightsLocation[i].position, 1, p1[i].position);
		glUniform1f(m_pointLightsLocation[i].u_constant, p1[i].u_constant);
		glUniform1f(m_pointLightsLocation[i].u_linear, p1[i].u_linear);
		glUniform1f(m_pointLightsLocation[i].u_quadratic, p1[i].u_quadratic);
	}


	//float gAngle1 = 30.0f;
	//spotLight
	SpotLight spot[3];
	//memset(spot, 0, sizeof(spot));
	spot[0].u_La = vec3(0.0, 0.0, 0.0);
	spot[0].u_Ld = vec3(1.0f, 0.0f, 0.0f);
	spot[0].u_Ls = vec3(1.0f, 0.0f, 0.0f);
	spot[0].u_linear = 0.1;
	spot[0].u_constant = 0.0;
	spot[0].u_quadratic = 0.0;
	spot[0].diffuseIntensity = 1.0f;
	//spot[0].position = vec3(cos(2 * PI * gAngle), sin(2 * PI * gAngle), 0.0);
	spot[0].position = vec3(0.0, 5.0f * cos(2 * PI * gAngle), 5.0f * sin(2 * PI * gAngle));
	spot[0].u_light_direction = vec3(0.0, 0.0f, 8.0f);
	spot[0].u_cutoff = 10.5f;
	spot[0].u_outerCutOff = 17.5f;
	spot[0].u_exponent = 1.5;

	spot[1].u_La = vec3(0.0, 0.00, 0.00);
	spot[1].u_Ld = vec3(0.0f, 1.0f, 0.0f);
	spot[1].u_Ls = vec3(0.0f, 1.0f, 0.0f);
	spot[1].u_linear = 0.1;
	spot[1].u_constant = 0.0;
	spot[1].u_quadratic = 0.0;
	spot[1].diffuseIntensity = 1.0f;
	//spot[1].position = vec3(cos(4 * PI * gAngle), sin(4 * PI * gAngle), 0.0);
	spot[1].position = vec3(5.0f * cos(2 * PI * gAngle), 5.0f * sin(2 * PI * gAngle), 0.0);
	spot[1].u_light_direction = vec3(-20.0, 0.0f, 0.0f);
	spot[1].u_cutoff = 10.5f;
	spot[1].u_outerCutOff = 17.5f;
	spot[0].u_exponent = 5.5;

	spot[2].u_La = vec3(0.0, 0.00, 0.00);
	spot[2].u_Ld = vec3(0.0f, 0.0f, 1.0f);
	spot[2].u_Ls = vec3(0.0f, 0.0f, 1.0f);
	spot[2].u_linear = 0.1;
	spot[2].u_constant = 0.0;
	spot[2].u_quadratic = 0.0;
	spot[2].diffuseIntensity = 1.0f;
	spot[2].position = vec3(5.0f * cos(2 * PI * gAngle), 0.0f, 5.0f * sin(2 * PI * gAngle));
	spot[2].u_light_direction = vec3(-20.0, 0.0f, 0.0f);
	spot[2].u_cutoff = 10.5f;
	spot[2].u_outerCutOff = 17.5f;

	for (int i = 0; i < 3; i++)
	{
		//glUniform1i(gWhichColorUniform, 2);
		glUniform3fv(m_spotLightsLocation[i].u_La, 1, spot[i].u_La);
		glUniform3fv(m_spotLightsLocation[i].u_Ls, 1, spot[i].u_Ls);
		glUniform3fv(m_spotLightsLocation[i].u_Ld, 1, spot[i].u_Ld);
		glUniform1f(m_spotLightsLocation[i].diffuseIntensity, spot[i].diffuseIntensity);

		glUniform3fv(m_spotLightsLocation[i].position, 1, spot[i].position);
		glUniform1f(m_spotLightsLocation[i].u_constant, spot[i].u_constant);
		glUniform1f(m_spotLightsLocation[i].u_linear, spot[i].u_linear);
		glUniform1f(m_spotLightsLocation[i].u_quadratic, spot[i].u_quadratic);

		glUniform1f(m_spotLightsLocation[i].u_cutoff, spot[i].u_cutoff);
		glUniform1f(m_spotLightsLocation[i].u_outerCutOff, spot[i].u_outerCutOff);
		glUniform3fv(m_spotLightsLocation[i].u_light_direction, 1, spot[i].u_light_direction);
	}
	

	//gPerspectiveProjectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	gPerspectiveProjectionMatrix = glm::perspective(45.0f, (float)gWidth / (float)gHeight, 0.1f, 100.0f);

	//glm::lookAt(Position, Position + Front, Up);
	//glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 3.0f) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	static float gDownAngle = -8.0f;
	static float gLeftMove = 0.0f;
	if(lightSequence == 1)
	camera = Camera(glm::vec3(gLeftMove, 0.0f, 12.0f), glm::vec3(gLeftMove, gDownAngle, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	else
		camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	static bool bStartLeftMoveAnimation = false;
	static bool bStartRightMoveAnimation = false;
	static bool bIsSunLeftAnimationStart = false;
	static bool bIsSunRightAnimationStart = false;
	if (bDoneLeftRightAnimation == true)
	{
		gDownAngle += 0.0075;
		if (gDownAngle >= 0.0f)
		{
			bDoneLeftRightAnimation = false;
			bStartLeftMoveAnimation = true;
		}
	}
	
	if (bStartLeftMoveAnimation == true)
	{
		xLightPosMove -= 0.01f;
		if (xLightPosMove <= -5.0f)
		{
			bStartLeftMoveAnimation = false;
			bIsSunRightAnimationStart = true;
		}
	}
	if (bIsSunRightAnimationStart == true)
	{
		xLightPosMove += 0.01f;
		if (xLightPosMove >= 0.0f)
		{
			bIsSunRightAnimationStart = false;
		}
	}

	glm::mat4 view = camera.getViewMatrix();
	
	glUniformMatrix4fv(gprojectionUniform, 1, GL_FALSE, &gPerspectiveProjectionMatrix[0][0]);

	glUniformMatrix4fv(gviewUniform, 1, GL_FALSE, &view[0][0]);

	// set light uniforms
	//glUniform3fv(gviewPosUniform, 1, &camera.Position[0]);
	glm::vec3 viewPos = glm::vec3(0.0f, 10.0f, 18.0f);
	glUniform3fv(gviewPosUniform, 1, &viewPos[0]);
	glUniform3fv(glightPosUniform, 1, &lightPos[0]);
	glUniformMatrix4fv(glightSpaceMatrixUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	if (lightSequence == 1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		mat4 model = mat4::identity();
		glUniform1i(gdiffuseTextureUniform, 0);
		glUniformMatrix4fv(gmodelUniform, 1, GL_FALSE, &model[0][0]);
		renderPlane();
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderScene(gShaderProgramObjectShader, gmodelUniform);

	glUseProgram(0);



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glUseProgram(postPassProgram);

	glm::vec2 screenPos = getScreenSpaceSunPos();
	glUniform2fv(gScreenSpaceSunPosUniform_postPass, 1, &screenPos[0]);
	glUniform1f(gDecayUniform_postPass, 1.0f);
	glUniform1f(gWeightUniform_postPass, 0.01f);
	glUniform1f(gDensityUniform_postPass, 2.0f);
	glUniform1f(gExposureUniform_postPass, 1.0);
	glUniform1i(gNumSamplesUniform_postPass, 100);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depth_debug_tex);
	glUniform1i(gOcclusionTextureUniform_postPass, 2);

	//renderPlane();

	displayQuad();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glDisable(GL_BLEND);
	
	SwapBuffers(ghdc);


}

void renderScene(const GLuint& shader, GLuint& NewModel)
{
	void renderCube();
	void renderSphere();
	void renderPlane();
	void renderAstromedicomp();


	glm::mat4 model;

	if (lightSequence == 1)
	{
		
		glUniform1i(gdiffuseTextureUniform, 0);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(xPositionMove, -13.5f + yPositionMove, -8.0f + zPositionMove));
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(0.0f, 1.0, 0.0f)));

		glUniformMatrix4fv(NewModel, 1, GL_FALSE, &model[0][0]);
		renderAstromedicomp();
	}
	else
	{
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(10.0f, 7.0f, 10.0f));
		glUniformMatrix4fv(NewModel, 1, GL_FALSE, &model[0][0]);
		renderCube();
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0));
		
		model = glm::scale(model, glm::vec3(2.0f));
		glUniformMatrix4fv(NewModel, 1, GL_FALSE, &model[0][0]);
		
		renderSphere();
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(3.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.0f));
		glUniformMatrix4fv(NewModel, 1, GL_FALSE, &model[0][0]);
		//renderCube();
		renderSphere();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
		
		model = glm::scale(model, glm::vec3(2.0f));
		glUniformMatrix4fv(NewModel, 1, GL_FALSE, &model[0][0]);
		//renderCube();
		renderSphere();
	}
	
}

void renderAstromedicomp()
{
	glBindVertexArray(gVao_AstroMedicomp);

	glDrawArrays(GL_TRIANGLES, 0, count_of_vertices_Astromedicomp);

	glBindVertexArray(0);
}

void renderPlane()
{
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void renderSphere()
{
	glBindVertexArray(gVao_sphere);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

void renderCube()
{
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Update()
{
	if (isAnimationStart == true)
	{
		gAngle += 0.01f;
		if (gAngle > 10.0f)
			gAngle = 0.0f;
	}
}

int LoadGLTextures(GLuint* texture, TCHAR imageResourceId[])
{
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;
	glGenTextures(1, texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf(gpFile, "Image Successfully Loaded.\n");
		fclose(gpFile);

		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf(gpFile, "Image loading Unsuccessfull.\n");
		fclose(gpFile);

	}
	return (iStatus);
}

//shadow_mapping
void ShadowMappingShaderBuildNCompileShader()
{
	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf(gpFile, "In ShadowMappingShaderBuildNCompileShader.\n");
	fclose(gpFile);

	void uninitialize();
	//VERTEX SHADE
	gVertexShaderObjectShader = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertextShaderSourceCodeShader =
		"#version 460" \
		"\n" \
		"layout (location = 0) in vec3 aPos;" \
		"layout (location = 2) in vec3 aNormal;" \
		"layout (location = 3) in vec2 aTexCoords;" \
		"out vec2 TexCoords;" \
		"out VS_OUT{" \
		"vec3 FragPos;" \
		"vec3 Normal;" \
		"vec2 TexCoords;" \
		"vec4 FragPosLightSpace;" \
		"} vs_out;" \
		"uniform mat4 projection;" \
		"uniform mat4 view;" \
		"uniform mat4 model;" \
		"uniform mat4 lightSpaceMatrix;" \
		"void main()" \
		"{" \
		"vs_out.FragPos = vec3(model * vec4(aPos, 1.0));" \
		"vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;" \
		"vs_out.TexCoords = aTexCoords;" \
		"vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);" \
		"gl_Position = projection * view * model * vec4(aPos, 1.0);" \
		"}";

	glShaderSource(gVertexShaderObjectShader, 1, (const GLchar**)&vertextShaderSourceCodeShader, NULL);

	//compile shader
	glCompileShader(gVertexShaderObjectShader);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectShader, iInfoLogLength, &written, szInfoLog);
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf(gpFile, "Vertex Shader Shadow Sampling Compilation Log : %s\n", szInfoLog);
				fclose(gpFile);

				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//FRAGMENT SHADER
	gFragmentShaderObjectShader = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCodeShader =
		"#version 460" \
		"\n" \
		"out vec4 FragColor;" \
		"in VS_OUT{" \
		"vec3 FragPos;" \
		"vec3 Normal;" \
		"vec2 TexCoords;" \
		"vec4 FragPosLightSpace;" \
		"} fs_in;" \
		"uniform sampler2D diffuseTexture;" \
		"uniform sampler2D shadowMap;" \
		"uniform vec3 lightPos;" \
		"uniform vec3 viewPos;" \
		"uniform int gWhichColor;" \
		"uniform bool isAllColor;" \
		//shadow
		"float ShadowCalculation(vec4 fragPosLightSpace)" \
		"{" \
		"vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;" \
		"projCoords = projCoords * 0.5 + 0.5; " \
		"float closestDepth = texture(shadowMap, projCoords.xy).r; " \
		"float currentDepth = projCoords.z;" \
		"vec3 normal = normalize(fs_in.Normal);" \
		"vec3 lightDir = normalize(lightPos - fs_in.FragPos);" \
		"float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);" \
		"float shadow = 0.0;" \
		"vec2 texelSize = 1.0 / textureSize(shadowMap, 0);" \
		"for (int x = -1; x <= 1; ++x)" \
		"{" \
		"for (int y = -1; y <= 1; ++y)" \
		"{" \
		"float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; " \
		"shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;" \
		"}" \
		"}" \
		"shadow /= 9.0;" \
		"if (projCoords.z > 1.0)" \
		"shadow = 0.0; " \
		"return shadow;" \
		"}" \

		"const int MAX_POINT_LIGHTS = 3;" \

		"struct PointLight" \
		"{" \
		"vec3 u_La;" \
		"vec3 u_Ld;" \
		"vec3 u_Ls;" \
		"float u_constant;" \
		"float u_linear;" \
		"float u_quadratic;" \
		"vec3 position;" \
		"float diffuseIntensity;"\
		"};" \

		"uniform int gNumPointLights;" \
		"uniform PointLight pointLight[3];" \
		"uniform int isPointLight;" \
		"uniform int isSpotLight;" \
		"uniform int isDirectionalLight;" \

		"vec3 calculatePointLight(int index)" \
		"{" \
		"vec3 pointLightColor;"

		"vec3 ambient = pointLight[index].u_La;" \
		"vec3 normalized_transformed_normals = normalize(fs_in.Normal);" \
		"vec3 normalized_light_direction = normalize(pointLight[index].position);"\
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction), 0.0);" \
		"vec3 diffuse = pointLight[index].u_Ld * pointLight[index].diffuseIntensity * tn_dot_ld;" \
		"vec3 view_direction = normalize(viewPos - fs_in.FragPos);" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 specular = pointLight[index].u_Ls * pow(max(dot(reflection_vector, view_direction), 0.0), 64.0f);" \
		"float distance = length(pointLight[index].position - fs_in.FragPos);" \
		"float attenuation = 1.0 / (pointLight[index].u_constant + pointLight[index].u_linear * distance + pointLight[index].u_quadratic * (distance * distance));" \
		"diffuse = diffuse * attenuation;" \
		"specular = specular * attenuation;" \
		"pointLightColor = ambient + diffuse + specular;" \
		"return pointLightColor;" \
		"}" \

		//SpotLight
		"struct SpotLight" \
		"{" \
		"vec3 u_La;" \
		"vec3 u_Ld;" \
		"vec3 u_Ls;" \
		"float u_constant;" \
		"float u_linear;" \
		"float u_quadratic;" \
		"vec3 position;" \
		"float diffuseIntensity;"\
		"vec3 u_light_direction;" \
		"float u_cutoff;" \
		"float u_outerCutOff;" \
		"float exponent;" \
		"};" \

		"uniform int gNumSpotLights;" \
		"uniform SpotLight spotLight[3];" \

		"vec3 calculateSpotLight(int index)" \
		"{" \
		"vec3 spotLightColor;"

		"vec3 ambient = spotLight[index].u_La;" \
		"vec3 normalized_transformed_normals = normalize(fs_in.Normal);" \
		"vec3 normalized_light_direction = normalize(spotLight[index].position - fs_in.FragPos);"\
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction), 0.0);" \
		"vec3 diffuse = spotLight[index].u_Ld * spotLight[index].diffuseIntensity * tn_dot_ld;" \

		"vec3 view_direction = normalize(viewPos - fs_in.FragPos);" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 specular = spotLight[index].u_Ls * pow(max(dot(reflection_vector, view_direction), 0.0), 64.0f);" \

		"float spotAttenuation;" \
		"float theta = dot(normalized_light_direction, normalize(-spotLight[index].u_light_direction));" \
		
		"float epsilon = (spotLight[index].u_cutoff - spotLight[index].u_outerCutOff);" \
		"float intensity = clamp((theta - spotLight[index].u_outerCutOff) / epsilon, 0.0, 1.0);" \
		"diffuse = diffuse * intensity;" \
		"specular = specular * intensity;" \

		"float distance = length(spotLight[index].position - fs_in.FragPos);" \
		"float attenuation = 1.0 / (spotLight[index].u_constant + spotLight[index].u_linear * distance + spotLight[index].u_quadratic * (distance * distance));" \

		/*"attenuation *= spotAttenuation;" \*/

		"diffuse = diffuse * attenuation;" \
		"specular = specular * attenuation;" \

		// calculate shadow
		"float shadow = ShadowCalculation(fs_in.FragPosLightSpace);"\
		"spotLightColor = ambient + (diffuse + specular);" \
		"return spotLightColor;" \
		"}" \




		"void main()" \
		"{" \
		"vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;" \
		"vec3 normal = normalize(fs_in.Normal);" \
		"vec3 lightColor = vec3(0.3);" \
		// ambient
		"vec3 ambient = 0.3 * color;" \
		// diffuse
		"vec3 lightDir = normalize(lightPos - fs_in.FragPos);" \
		"float diff = max(dot(lightDir, normal), 0.0);" \
		"vec3 diffuse = diff * lightColor;"\
		// specular
		"vec3 viewDir = normalize(viewPos - fs_in.FragPos);" \
		"vec3 reflectDir = reflect(-lightDir, normal); " \
		"float spec = 0.0;" \
		"vec3 halfwayDir = normalize(lightDir + viewDir);" \
		"spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);"\
		"vec3 specular = spec * lightColor;"\
		// calculate shadow
		"float shadow = ShadowCalculation(fs_in.FragPosLightSpace);"\

		"vec3 directionalLighting = vec3(0.0, 0.0, 0.0);" \
		"if(isDirectionalLight == 1)" \
		"{" \
		"directionalLighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;"\
		"}" \

		"vec3 pointLighting= vec3(0.0, 0.0, 0.0);" \
		"if(isDirectionalLight == 2)" \
		"{" \
		"for(int i = 0; i < gNumPointLights; ++i)" \
		"{" \
		/*"if(i == gWhichColor  || (isAllColor == true))" \
		"{" \*/
		"pointLighting += calculatePointLight(i);" \
		/*"}" \*/
		"}" \
		"}" \

		"vec3 spotLighting = vec3(0.0, 0.0, 0.0);" \
		"if(isDirectionalLight == 3)" \
		"{" \
		"for(int i = 0; i < gNumSpotLights; ++i)" \
		"{" \
		/*"if(i == gWhichColor  || (isAllColor == true))" \
		"{" \*/
		"spotLighting += calculateSpotLight(i);" \
		/*"}" \*/
		"}" \
		"}" \

		"FragColor = vec4((directionalLighting + pointLighting + spotLighting) * color, 1.0);"\
		"}";

	glShaderSource(gFragmentShaderObjectShader, 1, (const GLchar**)&fragmentShaderSourceCodeShader, NULL);

	glCompileShader(gFragmentShaderObjectShader);

	glGetShaderiv(gFragmentShaderObjectShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectShader, iInfoLogLength, &written, szInfoLog);
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf(gpFile, "Fragment Shader Shadow Sampling Compilation Log : %s\n", szInfoLog);
				fclose(gpFile);

				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Shader Program

	gShaderProgramObjectShader = glCreateProgram();

	glAttachShader(gShaderProgramObjectShader, gVertexShaderObjectShader);

	glAttachShader(gShaderProgramObjectShader, gFragmentShaderObjectShader);

	glBindAttribLocation(gShaderProgramObjectShader, MALATI_ATTRIBUTE_VERTEX, "aPos");
	glBindAttribLocation(gShaderProgramObjectShader, MALATI_ATTRIBUTE_NORMAL, "aNormal");
	glBindAttribLocation(gShaderProgramObjectShader, MALATI_ATTRIBUTE_TEXTURE, "aTexCoords");

	glLinkProgram(gShaderProgramObjectShader);

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectShader, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectShader, iInfoLogLength, &written, szInfoLog);
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf(gpFile, "Shader Program Link Shadow Sampling Log : %s\n", szInfoLog);
				fclose(gpFile);

				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gdiffuseTextureUniform = glGetUniformLocation(gShaderProgramObjectShader, "diffuseTexture");
	gshadowMapUniform = glGetUniformLocation(gShaderProgramObjectShader, "shadowMap");

	glightPosUniform = glGetUniformLocation(gShaderProgramObjectShader, "lightPos");
	gviewPosUniform = glGetUniformLocation(gShaderProgramObjectShader, "viewPos");

	gprojectionUniform = glGetUniformLocation(gShaderProgramObjectShader, "projection");
	gviewUniform = glGetUniformLocation(gShaderProgramObjectShader, "view");
	gmodelUniform = glGetUniformLocation(gShaderProgramObjectShader, "model");
	glightSpaceMatrixUniform = glGetUniformLocation(gShaderProgramObjectShader, "lightSpaceMatrix");
	gNumPointLightsUniform = glGetUniformLocation(gShaderProgramObjectShader, "gNumPointLights");
	gNumSpotLightsUniform = glGetUniformLocation(gShaderProgramObjectShader, "gNumSpotLights");

	gWhichSceneUniform = glGetUniformLocation(gShaderProgramObjectShader, "isDirectionalLight");
	gIsPointLightUniform = glGetUniformLocation(gShaderProgramObjectShader, "isPointLight");
	gIsSpotLightUniform = glGetUniformLocation(gShaderProgramObjectShader, "isSpotLight");
	gWhichColorUniform = glGetUniformLocation(gShaderProgramObjectShader, "gWhichColor");
	gAllColorUniform = glGetUniformLocation(gShaderProgramObjectShader, "isAllColor");
	char Name[128];
	for (int i = 0; i < 3; i++)
	{
		//char Name[128];
		memset(Name, 0, sizeof(Name));

		snprintf(Name, sizeof(Name), "pointLight[%d].u_La", i);
		m_pointLightsLocation[i].u_La = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].u_Ld", i);
		m_pointLightsLocation[i].u_Ld = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].u_Ls", i);
		m_pointLightsLocation[i].u_Ls = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].u_constant", i);
		m_pointLightsLocation[i].u_constant = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].u_linear", i);
		m_pointLightsLocation[i].u_linear = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].u_quadratic", i);
		m_pointLightsLocation[i].u_quadratic = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].position", i);
		m_pointLightsLocation[i].position = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "pointLight[%d].diffuseIntensity", i);
		m_pointLightsLocation[i].DiffuseIntensity = glGetUniformLocation(gShaderProgramObjectShader, Name);
	}


	for (int i = 0; i < 3; i++)
	{
		//char Name[128];
		memset(Name, 0, sizeof(Name));

		snprintf(Name, sizeof(Name), "spotLight[%d].u_La", i);
		m_spotLightsLocation[i].u_La = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_Ld", i);
		m_spotLightsLocation[i].u_Ld = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_Ls", i);
		m_spotLightsLocation[i].u_Ls = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_constant", i);
		m_spotLightsLocation[i].u_constant = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_linear", i);
		m_spotLightsLocation[i].u_linear = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_quadratic", i);
		m_spotLightsLocation[i].u_quadratic = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].position", i);
		m_spotLightsLocation[i].position = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].diffuseIntensity", i);
		m_spotLightsLocation[i].diffuseIntensity = glGetUniformLocation(gShaderProgramObjectShader, Name);
		/*"vec3 u_light_direction;" \
		"float u_cutoff;" \
		"float u_outerCutOff;" \*/
		snprintf(Name, sizeof(Name), "spotLight[%d].u_light_direction", i);
		m_spotLightsLocation[i].u_light_direction = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_cutoff", i);
		m_spotLightsLocation[i].u_cutoff = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].u_outerCutOff", i);
		m_spotLightsLocation[i].u_outerCutOff = glGetUniformLocation(gShaderProgramObjectShader, Name);

		snprintf(Name, sizeof(Name), "spotLight[%d].exponent", i);
		m_spotLightsLocation[i].u_exponent = glGetUniformLocation(gShaderProgramObjectShader, Name);

	}

}

void SimpleDepthShaderBuildNCompileShader()
{
	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf(gpFile, "In SimpleDepthShaderBuildNCompileShader \n ");
	fclose(gpFile);

	void uninitialize();
	//VERTEX SHADE
	gVertexShaderObjectSimpleDepthShader = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertextShaderSourceCodeSimpleDepthShader =
		"#version 460" \
		"\n" \
		"layout (location = 0) in vec3 aPos;" \
		"uniform mat4 lightSpaceMatrix;" \
		"uniform mat4 model;" \
		"void main()" \
		"{" \
		"gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);" \
		"}";

	glShaderSource(gVertexShaderObjectSimpleDepthShader, 1, (const GLchar**)&vertextShaderSourceCodeSimpleDepthShader, NULL);

	//compile shader
	glCompileShader(gVertexShaderObjectSimpleDepthShader);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectSimpleDepthShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectSimpleDepthShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectSimpleDepthShader, iInfoLogLength, &written, szInfoLog);
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf(gpFile, "Vertex Shader SimpleDepthShader Compilation Log : %s\n", szInfoLog);
				fclose(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//FRAGMENT SHADER
	gFragmentShaderObjectSimpleDepthShader = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCodeSimpleDepthShader =
		"#version 460" \
		"\n" \
		"void main(void)" \
		"{" \
		/*"gl_FragDepth = gl_FragCoord.z;" \*/
		"}";

	glShaderSource(gFragmentShaderObjectSimpleDepthShader, 1, (const GLchar**)&fragmentShaderSourceCodeSimpleDepthShader, NULL);

	glCompileShader(gFragmentShaderObjectSimpleDepthShader);

	glGetShaderiv(gFragmentShaderObjectSimpleDepthShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectSimpleDepthShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectSimpleDepthShader, iInfoLogLength, &written, szInfoLog);
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf(gpFile, "Fragment Shader SimpleDepthShader Compilation Log : %s\n", szInfoLog);
				fclose(gpFile);

				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Shader Program
	gShaderProgramObjectSimpleDepthShader = glCreateProgram();

	glAttachShader(gShaderProgramObjectSimpleDepthShader, gVertexShaderObjectSimpleDepthShader);
	glAttachShader(gShaderProgramObjectSimpleDepthShader, gFragmentShaderObjectSimpleDepthShader);

	glBindAttribLocation(gShaderProgramObjectSimpleDepthShader, MALATI_ATTRIBUTE_VERTEX, "aPos");

	glLinkProgram(gShaderProgramObjectSimpleDepthShader);

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectSimpleDepthShader, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectSimpleDepthShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectSimpleDepthShader, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link SimpleDepthShader Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	glightSpaceMatrixSimpleDepthShaderUniform = glGetUniformLocation(gShaderProgramObjectSimpleDepthShader, "lightSpaceMatrix");
	gmodelSimpleDepthShaderUniform = glGetUniformLocation(gShaderProgramObjectSimpleDepthShader, "model");
}

void resize(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gPerspectiveProjectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gWidth = width;
	gHeight = height;
	
}
void UninitializeAstromedicomp(void)
{
	if (gVao_AstroMedicomp)
	{
		glDeleteVertexArrays(1, &gVao_AstroMedicomp);
		gVao_AstroMedicomp = 0;
	}
	if (gVbo_Position)
	{
		glDeleteBuffers(1, &gVbo_Position);
		gVbo_Position = 0;
	}
	if (gVbo_Normal)
	{
		glDeleteBuffers(1, &gVbo_Normal);
		gVbo_Normal = 0;
	}
	if (gVbo_Texture)
	{
		glDeleteBuffers(1, &gVbo_Texture);
		gVbo_Texture = 0;
	}
}
void UninitializePlane(void)
{
	if (planeVAO)
	{
		glDeleteVertexArrays(1, &planeVAO);
		planeVAO = 0;
	}
	if (planeVBOposition)
	{
		glDeleteBuffers(1, &planeVBOposition);
		planeVBOposition = 0;
	}
	if (planeVBOnormal)
	{
		glDeleteBuffers(1, &planeVBOnormal);
		planeVBOnormal = 0;
	}
	if (planeVBOtexture)
	{
		glDeleteBuffers(1, &planeVBOtexture);
		planeVBOtexture = 0;
	}
}
void UninitializeSphere(void)
{
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}
	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}
	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}
	if (gVbo_sphere_texture)
	{
		glDeleteBuffers(1, &gVbo_sphere_texture);
		gVbo_sphere_texture = 0;
	}
	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}
}
void UninitializeCube(void)
{
	if (cubeVAO)
	{
		glDeleteVertexArrays(1, &cubeVAO);
		cubeVAO = 0;
	}
	if (cubeVBOposition)
	{
		glDeleteBuffers(1, &cubeVBOposition);
		cubeVBOposition = 0;
	}
	if (cubeVBOnormal)
	{
		glDeleteBuffers(1, &cubeVBOnormal);
		cubeVBOnormal = 0;
	}
	if (cubeVBOtexture)
	{
		glDeleteBuffers(1, &cubeVBOtexture);
		cubeVBOtexture = 0;
	}
}

void uninitialize(void)
{
	void UninitializeCube(void);
	void UninitializeSphere(void);
	void UninitializePlane(void);
	void UninitializeAstromedicomp(void);

	//UNINITIALIZATION CODE
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	UninitializePlane();
	UninitializeCube();

	UninitializeAstromedicomp();

	glDetachShader(gShaderProgramObjectShader, gVertexShaderObjectShader);
	glDetachShader(gShaderProgramObjectShader, gFragmentShaderObjectShader);

	glDeleteShader(gVertexShaderObjectShader);
	gVertexShaderObjectShader = 0;
	glDeleteShader(gFragmentShaderObjectShader);
	gFragmentShaderObjectShader = 0;

	glDeleteProgram(gShaderProgramObjectShader);
	gShaderProgramObjectShader = 0;

	glDetachShader(gShaderProgramObjectSimpleDepthShader, gVertexShaderObjectSimpleDepthShader);
	glDetachShader(gShaderProgramObjectSimpleDepthShader, gFragmentShaderObjectSimpleDepthShader);

	glDeleteShader(gVertexShaderObjectSimpleDepthShader);
	gVertexShaderObjectSimpleDepthShader = 0;
	glDeleteShader(gFragmentShaderObjectSimpleDepthShader);
	gFragmentShaderObjectSimpleDepthShader = 0;

	glDeleteProgram(gShaderProgramObjectSimpleDepthShader);
	gShaderProgramObjectSimpleDepthShader = 0;

	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(ghrc);
	ghrc = NULL;

	//Delete the device context
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf_s(gpFile, "Log File Is Successfully Closed.\n");
	fclose(gpFile);
}



//It is parallel to Simple DepthShader
void InitColorProgramShaders()
{
	void uninitialize(void);
	//VERTEX SHADER
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertextShaderSourceCode =
		"#version 460" \
		"\n" \
		"in vec3 vPosition;" \
		"in vec3 vNormal;" \

		/*"out vec3 outNormal;" \*/

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"void main(void)" \
		"{" \
		/*"outNormal = vNormal;" \*/
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(vPosition, 1.0f);" \
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertextShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Color Vertex Shader Compilation Log : %s\n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//FRAGMENT SHADER

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode =
		"#version 460" \
		"\n" \
		"uniform vec3 uColor;" \

		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(uColor, 1.0);" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(gFragmentShaderObject);

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Color Fragment Shader Compilation Log : %s\n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Shader Program

	colorProgram = glCreateProgram();

	glAttachShader(colorProgram, gVertexShaderObject);

	glAttachShader(colorProgram, gFragmentShaderObject);

	glBindAttribLocation(colorProgram, MALATI_ATTRIBUTE_VERTEX, "vPosition");


	glLinkProgram(colorProgram);

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(colorProgram, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(colorProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(colorProgram, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "colorProgram Shader Program Link Log : %s\n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelMatrixUniform_Color = glGetUniformLocation(colorProgram, "u_model_matrix");
	gViewMatrixUniform_Color = glGetUniformLocation(colorProgram, "u_view_matrix");
	gProjMatrixUniform_Color = glGetUniformLocation(colorProgram, "u_projection_matrix");
	gColorUniform_Color = glGetUniformLocation(colorProgram, "uColor");

}
void InitPostProcessProgramShaders()
{
	void uninitialize(void);
	//VERTEX SHADER
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertextShaderSourceCode =
		"#version 460" \
		"\n" \
		"in vec2 vPosition;" \
		"out vec2 vUv;" \

		"void main(void)" \
		"{" \

		"vUv = 0.5 * (vPosition + 1.0);" \

		/*"const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0)," \
		"								vec4(1.0, -1.0, 0.5, 1.0)," \
		"								vec4(-1.0, 1.0, 0.5, 1.0)," \
		"								vec4(1.0, 1.0, 0.5, 1.0));" \*/
		"gl_Position = vec4(vPosition.xy, 0.0, 1.0);" \
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertextShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PostProcess Program Vertex Shader Compilation Log : %s\n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//FRAGMENT SHADER

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode =
		"#version 460" \
		"\n" \
		"uniform sampler2D uOcclusionTexture;" \
		"in vec2 vUv;" \

		"uniform vec2 uScreenSpaceSunPos;" \

		"uniform float uDensity;" \
		"uniform float uWeight;" \
		"uniform float uDecay;" \
		"uniform float uExposure;" \
		"uniform int uNumSamples;" \

		"out vec4 FragColor;" \

		"vec3 godRays(float density," \
		"float weight," \
		"float decay," \
		"float exposure," \
		"int numSamples," \
		"sampler2D occlusionTexture," \
		"vec2 screenSpaceLightPos," \
		"vec2 uv)" \
		"{" \

		"vec3 fragColor = vec3(1.0, 0.0, 0.0);" \

		"vec2 deltaTexCoord = vec2(uv - screenSpaceLightPos.xy);" \

		"vec2 texCoo = uv.xy;" \
		"deltaTexCoord *= (1.0 / float(numSamples)) * density;" \
		"float illuminationDecay = 1.0;" \

		"for(int i = 0; i < numSamples; i++)" \
		"{" \
		"texCoo -= deltaTexCoord;" \
		"vec3 samp = texture2D(occlusionTexture, texCoo).xyz;" \
		"samp *= illuminationDecay * weight;" \
		"fragColor += samp;" \
		"illuminationDecay *= decay;" \
		"}" \

		"fragColor *= exposure;" \

		"return fragColor;" \
		"}" \



		"void main(void)" \
		"{" \
		/*"vec3 fragColor = godRays(uDensity," \
		"uWeight, uDecay, uExposure, uNumSamples," \
		"uOcclusionTexture, uScreenSpaceSunPos, vUv);" \*/

		"vec3 fragColor = vec3(0.0, 0.0, 0.0);" \

		"vec2 deltaTexCoord = vec2(vUv - uScreenSpaceSunPos.xy);" \

		"vec2 texCoo = vUv.xy;" \
		"deltaTexCoord *= (1.0 / float(uNumSamples)) * uDensity;" \
		"float illuminationDecay = 1.0;" \

		/*"fragColor = texture2D(uOcclusionTexture, texCoo).xyz;" \*/
		"for(int i = 0; i < uNumSamples; i++)" \
		"{" \
		"texCoo -= deltaTexCoord;" \
		"vec3 samp = texture2D(uOcclusionTexture, texCoo).xyz;" \
		"samp *= illuminationDecay * uWeight;" \
		"fragColor += samp;" \
		"illuminationDecay *= uDecay;" \
		"}" \

		"fragColor *= uExposure;" \



		"FragColor = vec4(fragColor, 1.0) * vec4(0.98, 0.83f, 0.25, 1.0f);" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(gFragmentShaderObject);

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "ShowLightDepth Program Fragment Shader Compilation Log : %s\n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Shader Program

	postPassProgram = glCreateProgram();

	glAttachShader(postPassProgram, gVertexShaderObject);

	glAttachShader(postPassProgram, gFragmentShaderObject);

	glBindAttribLocation(postPassProgram, MALATI_ATTRIBUTE_VERTEX, "vPosition");
	//glBindAttribLocation(show_light_depth_program, MALATI_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(postPassProgram);

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(postPassProgram, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(postPassProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(postPassProgram, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "postPassProgram Shader Program Link Log : %s\n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gScreenSpaceSunPosUniform_postPass = glGetUniformLocation(postPassProgram, "uScreenSpaceSunPos");
	gDensityUniform_postPass = glGetUniformLocation(postPassProgram, "uDensity");
	gWeightUniform_postPass = glGetUniformLocation(postPassProgram, "uWeight");
	gDecayUniform_postPass = glGetUniformLocation(postPassProgram, "uDecay");
	gExposureUniform_postPass = glGetUniformLocation(postPassProgram, "uExposure");
	gNumSamplesUniform_postPass = glGetUniformLocation(postPassProgram, "uNumSamples");
	gOcclusionTextureUniform_postPass = glGetUniformLocation(postPassProgram, "uOcclusionTexture");
}

void InitQuad()
{
	//Square
	GLfloat quadVertices[] =
	{

		//front
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

	};

	const GLfloat quadNormals[] =
	{

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

	};

	glGenVertexArrays(1, &gVao_quad);
	glBindVertexArray(gVao_quad);

	//position
	glGenBuffers(1, &gVbo_quad_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_quad_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//normal
	glGenBuffers(1, &gVbo_quad_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_quad_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadNormals), quadNormals, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void displayQuad()
{
	glBindVertexArray(gVao_quad);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
}