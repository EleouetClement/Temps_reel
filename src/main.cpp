#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }


    //Affihce la version opengl utilisée par le GPU
    std::cout << glGetString(GL_VERSION) << std::endl;

    //On a la même structure que sur les mesh Unity avec deux tableaux, un tableau de vertices et un d'index
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

    unsigned int buffer;

    //Declaration du buffer
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//Permet de specifier de que l'on veut voir affiché

    //Specification de la taille du buffer
    glBufferData(GL_ARRAY_BUFFER, 6 * 2* sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);//Permet de specifier de que l'on veut voir affiché
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);



    ShaderSources source = ParseShader("shaders/Basic.shader");

    /*std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;*/
    
    unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //DrawCall de la fonction
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);//On met nullptr car on a bind le tableau d'indices
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}