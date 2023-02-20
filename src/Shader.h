#ifndef SHADER_H
#define SHADER_H

#include <GLAD/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using uint = unsigned int;

class Shader {
 public:
  uint ID;

  Shader(const char *vertexPath, const char *fragmentPath);

  void use();

  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec3(const std::string &name, float value[3]) const;

 private:
  bool verifyShaderCompile(uint shader);
  bool verifyProgramCompile(uint program);
};

#endif