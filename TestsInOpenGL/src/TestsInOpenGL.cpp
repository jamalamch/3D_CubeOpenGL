#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model; // Model matrix uniform

    // Output variables for fragment shader
    out vec3 FragPos; // Fragment position
    out vec3 Normal;  // Normal vector

    void main()
    {
        Normal = mat3(transpose(inverse(model))) * normalize(aPos);
        
        vec4 worldPos = model * vec4(aPos, 1.0);

        FragPos = vec3(worldPos);

        gl_Position = worldPos;
    }
)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    // Input from vertex shader
    in vec3 FragPos;
    in vec3 Normal;

    // Uniforms
    vec3 lightDir = vec3(0, 1, -0.2);
    vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
    vec3 lightColor = vec3(1, 1, 1);

    void main()
    {
        // Calculate diffuse light
        float diff = max(dot(normalize(Normal), normalize(-lightDir)), 0.0);
        vec3 diffuse = diff * lightColor * objectColor;

        // Final color
        FragColor = vec4(diffuse, 1.0);
    }
)";

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

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "Error ! glewInit" << std::endl;
        glfwTerminate();
        return -1;
    }
    else 
    {
        std::cout << "Done ! glewInit" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    // Define the vertices of the cube
    float vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // Back face
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    // Define the indices (connectivity) of the cube
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        1, 5, 6,
        6, 2, 1,
        0, 4, 7,
        7, 3, 0,
        3, 2, 6,
        6, 7, 3,
        0, 1, 5,
        5, 4, 0
    };

    // Create Vertex Buffer Object (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create Vertex Array Object (VAO)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create Element Buffer Object (EBO)
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
    }

    glUseProgram(shaderProgram);


    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");

    float rotation = 90;

    glm::mat4 model = glm::mat4(1.0f); // Identity matrix
    glm::vec3 axis0(0.0f, 0.6f, 0.5f);
    model = glm::rotate(model, glm::radians(rotation), axis0);
    glm::vec3 axis1(0.0f, 1, 0);
    double currentFrame = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float newFrame = glfwGetTime();
        float deltaTime = newFrame - currentFrame;
        currentFrame = newFrame;

        //std::cout << currentFrame << std::endl;
        rotation = deltaTime*10;

        // Apply rotation transformation
        model = glm::rotate(model, glm::radians(rotation), axis1);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draw the cube
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(vertices), GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Delete allocated resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}