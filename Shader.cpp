#include "Shader.h"

Shader::Shader(const char* vertPath, const char* fragPath, const char* geomPath)
{
    std::string vertCode, fragCode, geomCode;
    std::ifstream vertSource, fragSource, geomSource;
    vertSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    geomSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertSource.open(vertPath);
        fragSource.open(fragPath);

        std::stringstream vertStream, fragStream;
        vertStream << vertSource.rdbuf();
        fragStream << fragSource.rdbuf();
        vertSource.close();
        fragSource.close();

        vertCode = vertStream.str();
        fragCode = fragStream.str();

        if (geomPath != nullptr) {
            geomSource.open(geomPath);
            std::stringstream geomStream;
            geomStream << geomSource.rdbuf();
            geomSource.close();
            geomCode = geomStream.str();
        }
    }
    catch (std::ifstream::failure& exception)
    {
        std::cerr << "ERROR: shader file reading failed" << std::endl;
    }

    const char* vertString = vertCode.c_str();
    const char* fragString = fragCode.c_str();
    unsigned int vert, frag;

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertString, NULL);
    glCompileShader(vert);
    checkCompilation(vert, "vertex shader");

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragString, NULL);
    glCompileShader(frag);
    checkCompilation(frag, "fragment shader");

    unsigned int geom;
    if (geomPath != nullptr) {
        const char* geomString = geomCode.c_str();
        geom = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom, 1, &geomString, NULL);
        glCompileShader(geom);
        checkCompilation(geom, "geometry shader");
    }

    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    if (geomPath != nullptr)
        glAttachShader(ID, geom);
    glLinkProgram(ID);
    checkCompilation(ID, "program");

    glDeleteShader(vert);
    glDeleteShader(frag);
    if (geomPath != nullptr)
        glDeleteShader(geom);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::checkCompilation(GLuint shaderID, std::string type)
{
    GLint success;
    GLchar info[1024];

    if (type == "program") {
        glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderID, 1024, NULL, info);
            std::cerr << "ERROR: failed to link program\n" << info << "\n" << std::endl;
        }
    }
    else {
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderID, 1024, NULL, info);
            std::cerr << "ERROR: failed to compile shader of type: " << type << "\n" << info << "\n" << std::endl;
        }
    }
}