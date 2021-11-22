#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGl Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderSources
{
    std::string VertexSource;
    std::string FragmentSource;
};


static ShaderSources ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);
    
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                //Set mod to vertex
                type = ShaderType::VERTEX;

            }
            else
            {
                if (line.find("fragment") != std::string::npos)
                {
                    //Set mod to fragment
                    type = ShaderType::FRAGMENT;
                }
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };

}
static unsigned int CompileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();// equ to &src[0]
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char *) alloca(length * sizeof(char));// permet d'allouer dynamiquement dans la pile 
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile  " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader) 
{
    //Compile the shader then link it to a new program that well be executed on the gpu
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);


    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //On precise que l'on veut utiliser opengl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }


    //Affihce la version opengl utilis�e par le GPU
    std::cout << glGetString(GL_VERSION) << std::endl;

    //On a la m�me structure que sur les mesh Unity avec deux tableaux, un tableau de vertices et un d'index
    float positions[12] =
    {
        -0.5f, -0.5f,
         0.5f,  -0.5f,
         0.5f, 0.5f,
         -0.5f, 0.5f,
    };

    //Eq au tableau de triangles sur Unity
    unsigned int indices[6] =
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));


    unsigned int buffer;

    //Declaration du buffer
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));//Permet de specifier de que l'on veut voir affich�

    //Specification de la taille du buffer
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2* sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//Permet de specifier de que l'on veut voir affich�
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));


    


    ShaderSources source = ParseShader("shaders/Basic.shader");

    /*std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;*/
    
    unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    //recuperation de l'uniform dans le shader
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));//retourne la position du Uniform voulu ou -1 si le Uniform n'a pas �t� trouv�
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));


    //Desynchronisation des elements
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        

        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        GLCall(glBindVertexArray(vao));

        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        
        //DrawCall de la fonction
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));//On met nullptr car on a bind le tableau d'indices
        if (r > 1.0f)
        {
            increment = -0.05f;
        }
        else
        {
            if (r < 0.0f)
            {
                increment = 0.05f;
            }
        }

        r += increment;
        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}