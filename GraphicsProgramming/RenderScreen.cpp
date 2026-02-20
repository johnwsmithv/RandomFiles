#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory> // std::unique_ptr
#include <iostream> // std::cout

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

int main() {
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

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!\n";
        return -1;
    }
    glViewport(0, 0, windowWidth, windowHeigth);

    // When the user resizes the window, we need to ensure we handle this
    // so we can register a callback function with the Window
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);

    return 0;
}