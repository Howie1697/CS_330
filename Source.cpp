// Howard Gilmore
//FINAL | CS-330 | SNHU
// 08/13/2022

/* Controls:
    W:              Forward
    A:              Backward
    S:              Left
    D:              Right
    Q:              Down
    E:              Up
    P:			    Hold for orthographic viewing, default is perspective
    [:              Texture Scaling -
    ]:              Texture Scaling +
    Scroll Wheel:	Zooms the camera in and out (changes FOV)
*/

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h" // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/camera.h" // Camera class


using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "7-1: FINAL"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint cubeVao;         // Handle for the vertex array object
        GLuint pyramidVao;
        GLuint bowlVao;
        GLuint cubeVbo;         // Handle for the vertex buffer object
        GLuint pyramidVbo;
        GLuint bowlVbo;
        GLuint nCubeVertices;    // Number of indices of the mesh
        GLuint nPyramidVertices;
        GLuint nBowlVertices;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture
    GLuint gTextureFloor;
    GLuint gTextureRoof;
    GLuint gTextureWall;
    GLuint gTextureStove;
    GLuint gTextureWindow;
    GLuint gTextureDoor;
    GLuint gTextureBowl;
    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_REPEAT;

    // Shader programs
    GLuint programId;
    GLuint gStoveProgramId;
    GLuint gStoveBackProgramId;
    GLuint gStoveLightProgramId;
    GLuint gFloorProgramId;
    GLuint gWallProgramId;
    GLuint gWallLeftProgramId;
    GLuint gWallRightProgramId;
    GLuint gDoorwayProgramId;
    GLuint gWindowProgramId;
    GLuint gRoofProgramId;
    GLuint gBowlProgramId;

    // camera
    Camera gCamera(glm::vec3(-1.5f, 2.0f, 8.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // globals
    bool keys[1024];
    bool ortho = false;			// Sets orthographic projection

    // Stove position and scale
    glm::vec3 gStovePosition(1.3f, 0.0f, -0.6f);
    glm::vec3 gStoveScale(2.0f);

    // Stove back position and scale
    glm::vec3 gStoveBackPosition(1.3f, 1.3f, -1.475f);
    glm::vec3 gStoveBackScale(2.0f, 0.6f, 0.25f);

    // Stove Light position and scale
    glm::vec3 gStoveLightPosition(0.55f, 1.4f, -1.35f);
    glm::vec3 gStoveLightScale(0.03f);
    glm::vec3 gStoveLightColor(1.0f, 0.5f, 0.5f);
    glm::vec3 gStoveLightObjectColor(1.0f, 1.0f, 1.0f);

    // Floor position and scale
    glm::vec3 gFloorPosition(0.0f, -1.05f, 4.0f);
    glm::vec3 gFloorScale(8.0f, 0.1f, 12.0f);

    // Roof position and scale
    glm::vec3 gRoofPosition(0.0f, 4.05f, 4.0f);
    glm::vec3 gRoofScale(8.0f, 0.1f, 12.0f);

    // Wall back position and scale
    glm::vec3 gWallBackPosition(0.0f, 1.5f, -1.95f);
    glm::vec3 gWallBackScale(8.0f, 5.0f, 0.1f);

    // Wall right position and scale
    glm::vec3 gWallRightPosition(3.95f, 1.5f, 4.0f);
    glm::vec3 gWallRightScale(0.1f, 5.0f, 12.0f);

    // Wall left position and scale
    glm::vec3 gWallLeftPosition(-3.95f, 1.5f, 4.0f);
    glm::vec3 gWallLeftScale(0.1f, 5.0f, 12.0f);

    // Doorway position and scale
    glm::vec3 gDoorwayPosition(-2.0f, 1.16f, -1.949f);
    glm::vec3 gDoorwayScale(2.5f, 4.5f, 0.105f);

    // Window position, scale, & color
    glm::vec3 gWindowPosition(3.95f, 1.9f, 5.0f);
    glm::vec3 gWindowScale(0.105f, 2.0f, 2.0f);
    glm::vec3 gWindowColor(1.0f, 1.0f, 0.94f);
    glm::vec3 gWindowObjectColor(1.0f, 1.0f, 0.94f);

    // Bowl position and scale
    glm::vec3 gBowlPosition(1.0f, 1.0f, 0.0f);
    glm::vec3 gBowlScale(0.5f);

    // Lamp animation
    bool gIsLampOrbiting = false;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMeshCube(GLMesh& mesh);
void UCreateMeshBowl(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Stove Vertex Shader Source Code*/
const GLchar* stoveVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Stove Fragment Shader Source Code*/
const GLchar* stoveFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phone lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.15f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Stove back Vertex Shader Source Code*/
const GLchar* stoveBackVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Stove back Fragment Shader Source Code*/
const GLchar* stoveBackFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.15f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Floor Vertex Shader Source Code*/
const GLchar* floorVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Floor Fragment Shader Source Code*/
const GLchar* floorFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing floor color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.1f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 4.0f; // Set specular light strength
    float highlightSize = 8.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Roof Vertex Shader Source Code*/
const GLchar* roofVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Roof Fragment Shader Source Code*/
const GLchar* roofFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing floor color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = -0.12f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.6f; // Set specular light strength
    float highlightSize = 2.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 0.0); // Send lighting results to GPU
}
);

/* Wall Vertex Shader Source Code*/
const GLchar* wallVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Wall Fragment Shader Source Code*/
const GLchar* wallFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = -0.6f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.8f; // Set specular light strength
    float highlightSize = 2.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Wall Left Vertex Shader Source Code*/
const GLchar* wallLeftVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Wall Left Fragment Shader Source Code*/
const GLchar* wallLeftFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = -0.6f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.8f; // Set specular light strength
    float highlightSize = 2.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Wall Right Vertex Shader Source Code*/
const GLchar* wallRightVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Wall Right Fragment Shader Source Code*/
const GLchar* wallRightFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.2f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.7f; // Set specular light strength
    float highlightSize = 3.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Doorway Vertex Shader Source Code*/
const GLchar* doorwayVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Doorway Fragment Shader Source Code*/
const GLchar* doorwayFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = -0.6f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.4f; // Set specular light strength
    float highlightSize = 2.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


/* Window Vertex Shader Source Code*/
const GLchar* windowVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Window Fragment Shader Source Code*/
const GLchar* windowFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 1.8f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 8.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Bowl Vertex Shader Source Code*/
const GLchar* bowlVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Bowl Fragment Shader Source Code*/
const GLchar* bowlFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.15f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 8.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Stove Light Vertex Shader Source Code*/
const GLchar* stoveLightVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Bowl Fragment Shader Source Code*/
const GLchar* stoveLightFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.15f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.0f; // Set specular light strength
    float highlightSize = 0.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMeshCube(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader programs
    if (!UCreateShaderProgram(stoveVertexShaderSource, stoveFragmentShaderSource, gStoveProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(floorVertexShaderSource, floorFragmentShaderSource, gFloorProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(doorwayVertexShaderSource, doorwayFragmentShaderSource, gDoorwayProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(wallVertexShaderSource, wallFragmentShaderSource, gWallProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(wallLeftVertexShaderSource, wallLeftFragmentShaderSource, gWallLeftProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(wallRightVertexShaderSource, wallRightFragmentShaderSource, gWallRightProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(windowVertexShaderSource, windowFragmentShaderSource, gWindowProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(roofVertexShaderSource, roofFragmentShaderSource, gRoofProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(stoveBackVertexShaderSource, stoveBackFragmentShaderSource, gStoveBackProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(stoveLightVertexShaderSource, stoveLightFragmentShaderSource, gStoveLightProgramId))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMeshBowl(gMesh); // Calls the function to create the Vertex Buffer Object

    if (!UCreateShaderProgram(bowlVertexShaderSource, bowlFragmentShaderSource, gBowlProgramId))
        return EXIT_FAILURE;

    // Load textures

    //All textures used under Creative Commons CC0 public domain.
    const char* texFloor = "resources/textures/floor.jpg";
    if (!UCreateTexture(texFloor, gTextureFloor))
    {
        cout << "Failed to load texture " << texFloor << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gFloorProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gFloorProgramId, "uTexture"), 0);

    const char* texRoof = "resources/textures/roof.jpg";
    if (!UCreateTexture(texRoof, gTextureRoof))
    {
        cout << "Failed to load texture " << texRoof << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gFloorProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gFloorProgramId, "uTexture"), 0);

    const char* texWall = "resources/textures/wall.jpg";
    if (!UCreateTexture(texWall, gTextureWall))
    {
        cout << "Failed to load texture " << texWall << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gWallProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gWallProgramId, "uTexture"), 0);

    const char* texStove = "resources/textures/stove.jpg";
    if (!UCreateTexture(texStove, gTextureStove))
    {
        cout << "Failed to load texture " << texStove << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gStoveProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gStoveProgramId, "uTexture"), 0);

    const char* texWindow = "resources/textures/window.jpg";
    if (!UCreateTexture(texWindow, gTextureWindow))
    {
        cout << "Failed to load texture " << texWindow << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gWindowProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gWindowProgramId, "uTexture"), 0);

    const char* texDoor = "resources/textures/door.png";
    if (!UCreateTexture(texDoor, gTextureDoor))
    {
        cout << "Failed to load texture " << texDoor << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gDoorwayProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gDoorwayProgramId, "uTexture"), 0);

    const char* texBowl = "resources/textures/bowl.jpg";
    if (!UCreateTexture(texBowl, gTextureBowl))
    {
        cout << "Failed to load texture " << texBowl << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gBowlProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gBowlProgramId, "uTexture"), 0);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);	

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTextureFloor);
    UDestroyTexture(gTextureWall);
    UDestroyTexture(gTextureStove);
    UDestroyTexture(gTextureWindow);
    UDestroyTexture(gTextureDoor);
    UDestroyTexture(gTextureBowl);

    // Release shader programs
    UDestroyShaderProgram(gStoveProgramId);
    UDestroyShaderProgram(gStoveBackProgramId);
    UDestroyShaderProgram(gWindowProgramId);
    UDestroyShaderProgram(gFloorProgramId);
    UDestroyShaderProgram(gWallProgramId);
    UDestroyShaderProgram(gWallLeftProgramId);
    UDestroyShaderProgram(gWallRightProgramId);
    UDestroyShaderProgram(gDoorwayProgramId);
    UDestroyShaderProgram(gBowlProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully

    // Flashight

}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{

    static const float cameraSpeed = 2.0f;

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
       gCamera.WorldUp = glm::vec3(0.0f, -1.0f, 0.0f);
       ortho = true;
    }
    else
    {
       gCamera.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
       ortho = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && gTexWrapMode != GL_REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_REPEAT;

        cout << "Current Texture Wrapping Mode: REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && gTexWrapMode != GL_MIRRORED_REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_MIRRORED_REPEAT;

        cout << "Current Texture Wrapping Mode: MIRRORED REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_EDGE)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_EDGE;

        cout << "Current Texture Wrapping Mode: CLAMP TO EDGE" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_BORDER)
    {
        float color[] = { 1.0f, 0.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_BORDER;

        cout << "Current Texture Wrapping Mode: CLAMP TO BORDER" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
    {
        gUVScale += 0.05f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
    {
        gUVScale -= 0.05f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the cube VAO
    glBindVertexArray(gMesh.cubeVao);

    // STOVE: draw stove
       //----------------
       // Set the shader to be used
    glUseProgram(gStoveProgramId);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gStovePosition) * glm::scale(gStoveScale);

    glm::mat4 projection;
    glm::mat4 view;

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gStoveProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gStoveProgramId, "view");
    GLint projLoc = glGetUniformLocation(gStoveProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Stove Shader program for the cub color, light color, light position, and camera position
    GLint windowObjectColorLoc = glGetUniformLocation(gStoveProgramId, "objectColor");
    GLint windowLightColorLoc = glGetUniformLocation(gStoveProgramId, "lightColor");
    GLint windowLightPositionLoc = glGetUniformLocation(gStoveProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gStoveProgramId, "viewPosition");

    // Pass color, light, and camera data to the Stove Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gStoveProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureStove);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // STOVE BACK: draw stove back
    //----------------
    // Set the shader to be used
    glUseProgram(gStoveBackProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gStoveBackPosition) * glm::scale(gStoveBackScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gStoveBackProgramId, "model");
    viewLoc = glGetUniformLocation(gStoveBackProgramId, "view");
    projLoc = glGetUniformLocation(gStoveBackProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Stove Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gStoveBackProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gStoveBackProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gStoveBackProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gStoveBackProgramId, "viewPosition");

    // Pass color, light, and camera data to the Stove Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gStoveBackProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureStove);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Floor: draw floor
    //----------------
    // Set the shader to be used
    glUseProgram(gFloorProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gFloorPosition) * glm::scale(gFloorScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gFloorProgramId, "model");
    viewLoc = glGetUniformLocation(gFloorProgramId, "view");
    projLoc = glGetUniformLocation(gFloorProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Floor Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gFloorProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gFloorProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gFloorProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gFloorProgramId, "viewPosition");

    // Pass color, light, and camera data to the Floor Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gFloorProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureFloor);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Roof: draw roof
    //----------------
    // Set the shader to be used
    glUseProgram(gRoofProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gRoofPosition) * glm::scale(gRoofScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gRoofProgramId, "model");
    viewLoc = glGetUniformLocation(gRoofProgramId, "view");
    projLoc = glGetUniformLocation(gRoofProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gRoofProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gRoofProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gRoofProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gRoofProgramId, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gRoofProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureRoof);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Wall: draw back wall
    //----------------
    // Set the shader to be used
    glUseProgram(gWallProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gWallBackPosition) * glm::scale(gWallBackScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gWallProgramId, "model");
    viewLoc = glGetUniformLocation(gWallProgramId, "view");
    projLoc = glGetUniformLocation(gWallProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gWallProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gWallProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gWallProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gWallProgramId, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gWallProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureWall);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Wall: draw right wall
    //----------------
    // Set the shader to be used
    glUseProgram(gWallRightProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gWallRightPosition) * glm::scale(gWallRightScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gWallRightProgramId, "model");
    viewLoc = glGetUniformLocation(gWallRightProgramId, "view");
    projLoc = glGetUniformLocation(gWallRightProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gWallRightProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gWallRightProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gWallRightProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gWallRightProgramId, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gWallRightProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureWall);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Wall: draw left wall
    //----------------
    // Set the shader to be used
    glUseProgram(gWallLeftProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gWallLeftPosition) * glm::scale(gWallLeftScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gWallLeftProgramId, "model");
    viewLoc = glGetUniformLocation(gWallLeftProgramId, "view");
    projLoc = glGetUniformLocation(gWallLeftProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gWallLeftProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gWallLeftProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gWallLeftProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gWallLeftProgramId, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gWallLeftProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureWall);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Doorway: draw doorway
    //----------------
    // Set the shader to be used
    glUseProgram(gDoorwayProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gDoorwayPosition) * glm::scale(gDoorwayScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gDoorwayProgramId, "model");
    viewLoc = glGetUniformLocation(gDoorwayProgramId, "view");
    projLoc = glGetUniformLocation(gDoorwayProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gDoorwayProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gDoorwayProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gDoorwayProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gDoorwayProgramId, "viewPosition");

    // Pass color, light, and camera data to the Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gDoorwayProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureDoor);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // Window: draw window
    //----------------
    // Set the shader to be used
    glUseProgram(gWindowProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gWindowPosition) * glm::scale(gWindowScale);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gWindowProgramId, "model");
    viewLoc = glGetUniformLocation(gWindowProgramId, "view");
    projLoc = glGetUniformLocation(gWindowProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Wall Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gWindowProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gWindowProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gWindowProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gWindowProgramId, "viewPosition");

    // Pass color, light, and camera data to the Wall Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gWindowProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureWindow);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);


    // STOVE LIGHT: draw stove light
    //----------------
    glUseProgram(gStoveLightProgramId);

    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gStoveLightPosition) * glm::scale(gStoveLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gStoveLightProgramId, "model");
    viewLoc = glGetUniformLocation(gStoveLightProgramId, "view");
    projLoc = glGetUniformLocation(gStoveLightProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nCubeVertices);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);


    // Activate the bowl VAO
    glBindVertexArray(gMesh.bowlVao);


    // BOWL: draw bowl
    //----------------
    // Set the shader to be used
    glUseProgram(gBowlProgramId);

    // Model matrix: transformations are applied right-to-left order
    model = glm::translate(gBowlPosition) * glm::scale(gBowlScale) * glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.8f, 0.0f));

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gBowlProgramId, "model");
    viewLoc = glGetUniformLocation(gBowlProgramId, "view");
    projLoc = glGetUniformLocation(gBowlProgramId, "projection");


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Stove Shader program for the cub color, light color, light position, and camera position
    windowObjectColorLoc = glGetUniformLocation(gBowlProgramId, "objectColor");
    windowLightColorLoc = glGetUniformLocation(gBowlProgramId, "lightColor");
    windowLightPositionLoc = glGetUniformLocation(gBowlProgramId, "lightPos");
    viewPositionLoc = glGetUniformLocation(gBowlProgramId, "viewPosition");

    // Pass color, light, and camera data to the Stove Shader program's corresponding uniforms
    glUniform3f(windowObjectColorLoc, gWindowObjectColor.r, gWindowObjectColor.g, gWindowObjectColor.b);
    glUniform3f(windowLightColorLoc, gWindowColor.r, gWindowColor.g, gWindowColor.b);
    glUniform3f(windowLightPositionLoc, gWindowPosition.x, gWindowPosition.y, gWindowPosition.z);
    cameraPosition == gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gBowlProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureBowl);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nBowlVertices);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMeshCube(GLMesh& mesh)
{
    // Cube Position and Color data
    GLfloat cube[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        //Front Face         //Positive Z Normal
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        //Left Face          //Negative X Normal
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Right Face         //Positive X Normal
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Bottom Face        //Negative Y Normal
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        //Top Face           //Positive Y Normal
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nCubeVertices = sizeof(cube) / (sizeof(cube[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.cubeVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.cubeVao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.cubeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cubeVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

        // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
};

void UCreateMeshBowl(GLMesh& mesh)
{
    // Bowl Position and Color data
    GLfloat bowl[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
       -0.3f, -0.0f, -0.3f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.3f, -0.0f, -0.3f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       -0.3f, -0.0f, -0.3f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        //Front Face         //Positive Z Normal
       -0.3f, -0.0f,  0.3f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.3f, -0.0f,  0.3f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       -0.3f, -0.0f,  0.3f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        //Left Face          //Negative X Normal
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
       -0.3f, -0.0f, -0.3f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -0.3f, -0.0f, -0.3f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -0.3f, -0.0f,  0.3f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Right Face         //Positive X Normal
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.3f, -0.0f, -0.3f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.3f, -0.0f, -0.3f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.3f, -0.0f,  0.3f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Bottom Face        //Negative Y Normal
       -0.3f, -0.0f, -0.3f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.3f, -0.0f, -0.3f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.3f, -0.0f,  0.3f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.3f, -0.0f,  0.3f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       -0.3f, -0.0f,  0.3f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
       -0.3f, -0.0f, -0.3f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        //Top Face           //Positive Y Normal
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nBowlVertices = sizeof(bowl) / (sizeof(bowl[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.bowlVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.bowlVao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.bowlVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.bowlVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(bowl), bowl, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

        // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
};


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.cubeVao);
    glDeleteBuffers(1, &mesh.cubeVbo);
    glDeleteVertexArrays(1, &mesh.pyramidVao);
    glDeleteBuffers(1, &mesh.pyramidVbo);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}