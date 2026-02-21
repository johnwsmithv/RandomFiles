#version 460 core
out vec4 FragColor;
void main()
{
    // We're going to keep things simple by rendering the color as orange
    // Colors are represented as Red, Green, Blue and Alpa (Opacity)
    // The values are going to be between 0.0 and 1.0
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}