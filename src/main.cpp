#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include<vector>
#include<cstddef>
#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"


struct ShaderSources
{
    std::string VertexSource;
    std::string FragmentSource;
};
struct Vertex
{
    glm::vec3 position;
    glm::vec3 couleur;
    glm::vec2 uvs;
    glm::vec3 normals;
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


    //Affihce la version opengl utilisée par le GPU
    std::cout << glGetString(GL_VERSION) << std::endl;
    {//Ce scope permet de detruire les variable avant le glterminate pour qu'il qu'openGL ne detecte pas d'erreur
        //On a la même structure que sur les mesh Unity avec deux tableaux, un tableau de vertices et un d'index
        std::vector<Vertex> vertices =
        {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}, {}, {}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}, {}, {}},
            {{0.5f, -0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {}, {}}
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

        unsigned int vertexBuff;
        //Declaration du buffer
        glGenBuffers(1, &vertexBuff);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);//Permet de specifier de que l'on veut voir affiché

        //Specification de la taille du buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


       /* GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));*/

        //IndexBuffer ib(indices, 6);

        ShaderSources source = ParseShader("shaders/Basic.shader");

        /*std::cout << "VERTEX" << std::endl;
        std::cout << source.VertexSource << std::endl;
        std::cout << "FRAGMENT" << std::endl;
        std::cout << source.FragmentSource << std::endl;*/

        unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        const auto index = glGetAttribLocation(shader, "position");
        const auto indexCouleur = glGetAttribLocation(shader, "color_in");
        GLCall(glEnableVertexAttribArray(index));
        GLCall(glEnableVertexAttribArray(indexCouleur));
        GLCall(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(struct Vertex, Vertex::position)));
        glVertexAttribPointer(indexCouleur, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(struct Vertex, Vertex::couleur));
        glVertexAttribPointer(indexCouleur, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(struct Vertex, Vertex::uvs));
        glVertexAttribPointer(indexCouleur, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(struct Vertex, Vertex::normals));



        //Desynchronisation des elements
        /*GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));*/
        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));
            glClearColor(1.0f, 0.2f, 0.2f, 1.0f);

            //GLCall(glUseProgram(shader));
            //GLCall(glBindVertexArray(vao));
            //ib.Bind();

            //DrawCall de la fonction
            //GLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));//On met nullptr car on a bind le tableau d'indices
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));//On met nullptr car on a bind le tableau d'indices
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
    }
    glfwTerminate();
    return 0;
}