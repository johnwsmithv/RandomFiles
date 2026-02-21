#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory> // std::unique_ptr
#include <iostream> // std::cout
#include <tuple> // std::tuple
#include <span> // std::span

// TODO: This is not ideal and we want to read the file source in
//       so we don't have to copy pasta it here.
// NOTE: If I upgrade to gcc-15, I'll have access to #embed which
//       will allow me to literally embed the contents of the
//       shaders into the binary at compile time.
const char* vertexShaderSource = 
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n";

const char* fragmentShaderSource =
    "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";

/**
 * @brief Callback function which is passed into the 
 * glfwSetFramebufferSizeCallback which ensures
 * that the Window is able to properly resize
 * when the user adjusts it.
 * 
 * @param window The Window which we are resizing
 * @param width The width of the window
 * @param height The height of the window
 */
void frameBufferCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    (void)window; // Doing this to ignore the unused variable warning from the compiler
}

struct vec2 {
    int x;
    int y;
};

struct vec3 {
    int x;
    int y;
    int z;
};

// The blackhole code is primarily from this YouTube video:
// https://www.youtube.com/watch?v=8-B6ryuBkCM&t=46s
struct Blackhole {
    vec2 position;
    double mass;
    // This is the event horizon of the blackhole. This is the distance at
    // which not even light can escape the blackhole.
    // The formula is 2GM / c^2
    double r_s;

    Blackhole(vec2 position, double mass)
        : position(position), mass(mass)
    {
        // The units are (Newton * (Meters ** 2)) / (kg ** 2)
        constexpr double gravitationalConstant = 6.67430e-11;
        // The units are meters per second
        constexpr double speedOfLight = 299792458;
        this->r_s = (2 * gravitationalConstant * this->mass) / (speedOfLight * speedOfLight);
    }

    void drawCircle() {
        // glad_glColor3
    }
};



uint32_t createVertexShader() {
    // 1. Create the shader object, referenced by an ID
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 2. Attach the shader source code and assign it to the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 3. Check if the compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    return vertexShader;
}

// TODO: It would be interesting to defined a Struct with shader return values
//       and to defined the destructor as calling glDeleteShader to ensure
//       resources are freed so we don't forget!
uint32_t createFragmentShader() {
    // 1. Create the fragment shader object, referenced by an ID
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 2. Attach the fragment shader source code and assign it to the shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // 3. Check if the compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    return fragmentShader;
}

uint32_t createShaderProgram() {
    uint32_t shaderProgram = glCreateProgram();

    // 1. Attach the vertex and fragment shaders!
    auto vertexShader = createVertexShader();
    auto fragmentShader = createFragmentShader();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // 2. Link the program!
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
            infoLog << std::endl;
    }

    // Now that we've attached and linked the shaders, we can delete them!
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// We want to send this vertex data as the input to the first process of a graphics pipeline
// i.e. the Vertex Shader. We are sending this memory as a Vertex Buffer Object (VBO).
// We can send a huge batch of data to the GPU all at once to save time. 

std::tuple<uint32_t, uint32_t> drawTriangle(const std::span<float> vertices) {
    // This object is going to have a unique ID corresponding to it
    uint32_t VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // We want to bind the Vertex Array Object first, then bind and set vertex buffer(s), 
    // and then configure vertex attribute(s).
    glBindVertexArray(VAO);

    // The buffer type of a vertex buffer object is "GL_ARRAY_BUFFER"
    // We need to bind the newly created buffer, VBO, to the GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Now that we have a VBO, we can copy the vertices into the buffers memory
    // "GL_STATIC_DRAW" specifies how we want the GPU to manage the given data
    std::cout << "The size of vertices is " << vertices.size_bytes() << "\n";
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

    // If we want to do some rendering, we need to set up a vertex and fragment shader.
    // We can use GLSL (OpenGL Shading Language)
    constexpr uint32_t vertexAttrLocation = 0; // This corresponds to the vertex shader position
    constexpr bool normalizeData = GL_FALSE;
    constexpr uint32_t stride = sizeof(vec3);
    constexpr uint32_t vertexSize = sizeof(vec3) / sizeof(int);
    glVertexAttribPointer(vertexAttrLocation, vertexSize, GL_FLOAT, normalizeData, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as 
    // the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, 
    // but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray 
    // anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    return {VBO, VAO};    
}

struct Vertex {
    uint32_t VBO;
    uint32_t VAO;
    uint32_t EBO;
};

Vertex drawRectangle(std::span<float> vertices, std::span<unsigned int> indices) {
    // In order to render a rectangle, we need to use something called an "Element Buffer Object" (EBO)
    Vertex vertex;

    glGenVertexArrays(1, &vertex.VAO);
    glGenBuffers(1, &vertex.VBO);
    glGenBuffers(1, &vertex.EBO);
    // We want to bind the Vertex Array Object first, then bind and set vertex buffer(s), 
    // and then configure vertex attribute(s).
    glBindVertexArray(vertex.VAO);

    // The buffer type of a vertex buffer object is "GL_ARRAY_BUFFER"
    // We need to bind the newly created buffer, VBO, to the GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vertex.VBO);

    // Now that we have a VBO, we can copy the vertices into the buffers memory
    // "GL_STATIC_DRAW" specifies how we want the GPU to manage the given data
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);

    // If we want to do some rendering, we need to set up a vertex and fragment shader.
    // We can use GLSL (OpenGL Shading Language)
    constexpr uint32_t vertexAttrLocation = 0; // This corresponds to the vertex shader position
    constexpr bool normalizeData = GL_FALSE;
    constexpr uint32_t stride = sizeof(vec3);
    constexpr uint32_t vertexSize = sizeof(vec3) / sizeof(int);
    glVertexAttribPointer(vertexAttrLocation, vertexSize, GL_FLOAT, normalizeData, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as 
    // the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, 
    // but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray 
    // anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    return vertex;
}

void drawCircle() {
    // GL_TRIANGLE_FAN
}

// Vertex Array Object (VAO) stores:
// 1. Calls to glEnableVertexAttribArray or glDisableVertexAttribArray
// 2. Vertex attribute configurations via glVertexAttribPointer
// 3. Vertex buffer objects associated with vertex attributes by call to glVertexAttribPointer

int main() {
    // I'm following an OpenGL tutorial: https://learnopengl.com/book/book_pdf.pdf
    // Remember that OpenGL is a 3D graphics library and thus we need to 
    // provide it with (X, Y, Z) coordinates. The coordinated need to be
    // between -1.0 and 1.0 on all three axis. This is within the so called
    // "normalized device coordinates".

    // Since the data is stored (X, Y, Z) and each value is a float, each vertex
    // is 12 bytes, and therefore the stride between vertices is 12 bytes.
    float triangleVertices[] = {
        // X, Y, Z
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    std::span<float> triangleVerticesSpan(triangleVertices, 9);

    // In order to remove overhead of needing to specify the same vertices more
    // than once, we can specify the order in which we want to draw the vertices in!
    float rectagleVertices[] = {
        // first triangle
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        // second triangle
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f // top left
    };
    std::span<float> rectangleVerticesSpan(rectagleVertices, 18);

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };
    std::span<unsigned int> rectangleIndicesSpan(indices, 6);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr int windowWidth = 800;
    constexpr int windowHeigth = 600;
    const std::string windowName = "Sample Window";
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeigth, windowName.c_str(), nullptr, nullptr);

    if(window == nullptr) {
        std::cout << "Failed to create a Window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // When the user resizes the window, we need to ensure we handle this
    // so we can register a callback function with the Window
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!\n";
        return -1;
    }

    // Now that everything is initialied, we can create our shader program!
    const auto shaderProgram = createShaderProgram();

    // We need to now specify how OpenGL should interpret the vertex data before rendering
    // auto [VBO, VAO] = drawTriangle(verticesSpan);
    auto vertex = drawRectangle(rectangleVerticesSpan, rectangleIndicesSpan);

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // The glfwWindowShouldClose checks at the beginning of each loop if the Window
    // has been instructed to close. If so the function will return true, thus
    // terminating the loop and the program as well.
    while(!glfwWindowShouldClose(window)) {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Now we can render the object with the shader program
        // To draw the triangle
        // glUseProgram(shaderProgram);
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // To draw the Rectangle
        glUseProgram(shaderProgram);
        glBindVertexArray(vertex.VAO);
        /// Takes the indices from the EBO bound to the GL_ELEMENT_ARRAY_BUFFER
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vertex.VAO);
    glDeleteBuffers(1, &vertex.VBO);
    glDeleteBuffers(1, &vertex.EBO);
    glDeleteProgram(shaderProgram);

    // We need to ensure we call this function so the system can reclaim its resources!
    glfwTerminate();
    return 0;
}