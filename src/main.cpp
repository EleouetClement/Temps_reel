#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include<vector>
#include<cstddef>
#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "stb_image.h"



static void error_callback(int /*error*/, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::string severitystr;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severitystr = "NOTIF";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severitystr = "WARN";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severitystr = "ERROR";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        severitystr = "FATAL";
        break;
    default:
        severitystr = "UNKNOWN";
        break;
    }
    //Ok
    std::cout << "[OPENGL " << severitystr << "](" << type << ") : " << message << std::endl;

    if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cout << "Breaking execution because an error was raised above medium severity!" << std::endl;
        __debugbreak();
    }
}
void APIENTRY opengl_error_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::cout << message << std::endl;
}

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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    //Affihce la version opengl utilisée par le GPU
    std::cout << glGetString(GL_VERSION) << std::endl;
    {//Ce scope permet de detruire les variable avant le glterminate pour qu'il qu'openGL ne detecte pas d'erreur
        //On a la même structure que sur les mesh Unity avec deux tableaux, un tableau de vertices et un d'index
        std::vector<Vertex> vertices =
        {
            {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {}},
            {{0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {}},
            {{0.5f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {}},
            {{-0.5f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {}},
        };

        

        //Eq au tableau de triangles sur Unity
        std::vector<unsigned int> indices =
        {//USED
            0, 1, 2,
            3, 0, 2
        };

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        //Generation et liaison de l'index buffer
        unsigned int indexBuffer;
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        unsigned int vertexBuff;
        //Declaration du VertexBuffer
        glGenBuffers(1, &vertexBuff);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);//Permet de specifier de que l'on veut voir affiché

        //Specification de la taille du buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


        ShaderSources source = ParseShader("shaders/Basic.shader");


        unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        const auto index = glGetAttribLocation(shader, "position");
        //const auto indexCouleur = glGetAttribLocation(shader, "color_in");
        const auto uvs = glGetAttribLocation(shader, "uvs_in");
        //const auto normals = glGetAttribLocation(shader, "normals_in");
        

        GLCall(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(struct Vertex, Vertex::position)));
        //glVertexAttribPointer(indexCouleur, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(struct Vertex, Vertex::couleur));
        glVertexAttribPointer(uvs, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(struct Vertex, Vertex::uvs));
        //glVertexAttribPointer(normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(struct Vertex, Vertex::normals));
        GLCall(glEnableVertexAttribArray(index));
        //GLCall(glEnableVertexAttribArray(indexCouleur));
        GLCall(glEnableVertexAttribArray(uvs));
        //GLCall(glEnableVertexAttribArray(normals));

        int width, height, nbChannels;
        stbi_set_flip_vertically_on_load(1);
        const unsigned char* image = stbi_load("Textures/crusader.png", &width, &height, &nbChannels, 0);
        if (!image)
        {
            std::cout << "Image non chargée!" << std::endl;
            return -1;
        }

        unsigned int texture;
        //Permet d'eviter le décalage lors de l'application de la texture.
        

        //Generation de la texture
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGB8, width, height);
		/*glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);*/
        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glBindTextureUnit(0, texture);
        
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        unsigned int sampler = glGetUniformLocation(shader, "color_texture");
        glUniform1i(sampler, 0);


        //Desynchronisation des elements
        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        //Declaration des matrices de transformations
        //test
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        //Envoi des matrices vers le shader avec des uniforms
        int modelloc = glGetUniformLocation(shader, "model");
        int viewloc = glGetUniformLocation(shader, "view");
        int projectionloc = glGetUniformLocation(shader, "projection");

        

        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glViewport(0, 0, 800, 600);
            GLCall(glClear(GL_COLOR_BUFFER_BIT));
            glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
            
			glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projectionloc, 1, GL_FALSE, glm::value_ptr(projection));

            //GLCall(glUseProgram(shader));
            //GLCall(glBindVertexArray(vao));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

            //DrawCall de la fonction
            //GLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));//On met nullptr car on a bind le tableau d'indices
            GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));//On met nullptr car on a bind le tableau d'indices
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