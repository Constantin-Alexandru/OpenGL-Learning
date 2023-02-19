#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

/* TYPEDEFS */

using uint = unsigned int;

/* STRUCTURES & ENUMS */
enum class Render { TRIANGLE = 1 << 0, RECTANGLE = 1 << 1 };

struct WindowProps {
  int width;
  int height;

  Render render = Render::RECTANGLE;

  WindowProps() : width(800), height(600){};
  WindowProps(int _width, int _height) : width(_width), height(_height){};
};

/* GLOBAL VARIABLES */
WindowProps props;

uint VBO;  //? VBO - Vertex Buffer Object
uint VAO;  //? VAO - Vertex Array Object
uint EBO;  //? EBO - Element Buffer Object

uint shaderProgram;

/* CONSTANTS */

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";

const float triangle_vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                                   0.0f,  0.0f,  0.5f, 0.0f};

const float rectangle_vertices[] = {0.5f,  0.5f,  0.0f, 0.5f,  -0.5f, 0.0f,
                                    -0.5f, -0.5f, 0.0f, -0.5f, 0.5f,  0.0f};

const uint rectangle_indices[] = {0, 1, 3, 1, 2, 3};

/* FUNCTION DECLARATIONS */

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
bool verifyShaderCompile(uint shader);
bool verifyProgramCompile(uint program);

int main() {
  props = WindowProps();

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(props.width, props.height,
                                        "OpenGL-Learning", NULL, NULL);

  if (!window) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to intialise GLAD\n";
    return -1;
  }

  glViewport(0, 0, props.width, props.height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  uint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  if (!verifyShaderCompile(vertexShader)) {
    glDeleteShader(vertexShader);
    return -1;
  }

  uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  if (!verifyShaderCompile(fragmentShader)) {
    glDeleteShader(fragmentShader);
    return -1;
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  if (!verifyProgramCompile(shaderProgram)) return -1;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  if (props.render == Render::TRIANGLE)
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices,
                 GL_STATIC_DRAW);

  if (props.render == Render::RECTANGLE)
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices),
                 rectangle_vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangle_indices),
               rectangle_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    if (props.render == Render::TRIANGLE) glDrawArrays(GL_TRIANGLES, 0, 3);

    if (props.render == Render::RECTANGLE) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

/* FUNCTIONS IMPLEMENTATIONS */

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  props.width = width;
  props.height = height;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

bool verifyShaderCompile(uint shader) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR: Shader Failed To Compile: " << infoLog << '\n';

    return false;
  }

  return true;
}

bool verifyProgramCompile(uint program) {
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "ERROR: Program Failed To Compile: " << infoLog << '\n';

    return false;
  }

  return true;
}