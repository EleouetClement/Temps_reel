#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h";

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


        //Declaration du buffer
        VertexArray va;

        VertexBufferLayout layout;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);


        IndexBuffer ib(indices, 6);

        
        
        Shader shader("shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        //Desynchronisation des elements
        va.UnBind();
        shader.UnBind();
        vb.unBind();
        ib.unBind();
        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));


            shader.Bind();
            shader.SetUniform4f("u_Color" , r, 0.3f, 0.8f, 1.0f);

            va.Bind();
            ib.Bind();

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
    }
    glfwTerminate();
    return 0;
}