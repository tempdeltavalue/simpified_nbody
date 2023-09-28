
#include <iostream>
#include <glm/gtc/random.hpp>

#include "WindowInputManager.h"
#include "Particle.h"
#include "VertexFragmentShader.h"
#include "Camera.h"

#include "vector"
#include <random>
#include <ctime>

#include "Shader.h"
#include "ComputeShader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION // This must be defined before including stb_image_write.h
#include "stb_image.h"
#include "stb_image_write.h"

#include <string>
#include <cstdarg>

// read files 
#include <fstream>
#include <sstream>

// Function to generate a random float in the range -1 to 1
float getRandomFloat() {
    static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(rng);
}

inline std::string format(const char* fmt, ...) {
    int size = 512;
    char* buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if (size <= nsize) { //fail delete buffer and try again
        delete[] buffer;
        buffer = 0;
        buffer = new char[nsize + 1]; //+1 for /0
        nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    return ret;
}


// Function to calculate the variance of an array of glm::vec4 vectors
glm::vec4 calculateVariance(const glm::vec4* vecArray, int arraySize, const glm::vec4& mean) {
    glm::vec4 sumOfSquares(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < arraySize; i++) {
        glm::vec4 deviation = vecArray[i] - mean;
        sumOfSquares += deviation * deviation;
    }

    return sumOfSquares / static_cast<float>(arraySize);
}


glm::vec4 calculateMean(const glm::vec4* vecArray, int arraySize) {
    glm::vec4 sum(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < arraySize; i++) {
        sum += vecArray[i];
    }

    // Divide the sum by the number of vectors to get the mean
    glm::vec4 mean = sum / static_cast<float>(arraySize);

    return mean;
}

std::vector<Particle> getParticles(glm::vec3 worldDimensions, int n) {
    Particle* particles = new Particle[n];

    float length = glm::length(worldDimensions) / 20.f;


    // Determine radius of sphere based on world dimensions
    float radius = glm::length(worldDimensions) / 3.0f;


    for (int i = 0; i < n; ++i) {
        glm::vec3 position = glm::sphericalRand(radius);
        // Shift particle position by half of the world dimensions
        position += 0.5f * worldDimensions;
        glm::vec3 initialVel = glm::vec3(0); // (worldDimensions.z == 0) ? glm::linearRand(glm::vec3(-length, -length, 0.f), glm::vec3(length, length, 0.f)) : glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
        particles[i] = Particle(position, initialVel, glm::linearRand(0.001, 0.01));  // 0.02
    }

    std::vector<Particle> particleVector(particles, particles + n);

    delete[] particles; // free the allocated memory

    return particleVector;
}



int main(int argc, char *argv[])
{


    glm::vec3 worldDimensions(5.f, 5.f, 5.f);

    int numParticles = 40000;

    std::vector<Particle> particles = getParticles(worldDimensions, numParticles);


    glm::vec4* velocities = new glm::vec4[numParticles]();
    glm::vec4* accelerations = new glm::vec4[numParticles]();
    glm::vec4* positions = new glm::vec4[numParticles]();
    glm::vec4* masses = new glm::vec4[numParticles]();
    glm::vec4* forces = new glm::vec4[numParticles]();


    for (int i = 0; i < numParticles; i++) {
        velocities[i] = particles[i].velocity;
        accelerations[i] = particles[i].acceleration;
        positions[i] = particles[i].position;
        masses[i] = glm::vec4(particles[i].mass, 0, 0, 0);
        forces[i] = glm::vec4(0.f);
    }


    glm::vec2 windowDim(800, 600);
    Window window(windowDim, "temp");

    std::string positionsCalculatorPath = "../Shaders/glsl/updateParticles.glsl";
    std::string forceCalculatorPath = "../Shaders/glsl/forceCalculation.glsl";


    /// Compute shaders
    int blockSize = 64;
    float timeStep = 0.001;
    float squareSoft = 0.1;

    ComputeShader *positionCalculator = new ComputeShader(positionsCalculatorPath);
    positionCalculator->use();
    positionCalculator->setFloat("deltaTime", timeStep);

    ComputeShader *forceCalculator = new ComputeShader(forceCalculatorPath);
    forceCalculator->use();
    forceCalculator->setFloat("squaredSoftening", squareSoft);
    /////



    /// Create data buffers in OpenGL

    GLuint VAO;
    GLuint VBO;
    GLuint postitions_SSBO, velocities_SSBO, accelerations_SSBO, masses_SSBO, forces_SSBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &postitions_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, postitions_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * numParticles, positions, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &velocities_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocities_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * numParticles, velocities, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &accelerations_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, accelerations_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * numParticles, accelerations, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &masses_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, masses_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * numParticles, masses, GL_STATIC_DRAW);

    glGenBuffers(1, &forces_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, forces_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * numParticles, forces, GL_DYNAMIC_DRAW);

    ///

       // Create a std::vector of floats and initialize it with random values
    const int numElements = 3;

    std::vector<float> weights(numElements);
    for (int i = 0; i < numElements; ++i) {
        weights[i] = getRandomFloat();
    }


    // Generate and bind the OpenGL shader storage buffer object (SSBO)
    GLuint weights_SSBO;
    glGenBuffers(1, &weights_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, weights_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * numParticles, weights.data(), GL_DYNAMIC_DRAW);

    // Now, weights.data() contains the data of the std::vector in a format suitable for OpenGL SSBO
    // You can pass weights_SSBO to your shader for further processing.

    // Don't forget to clean up when you're done
    //glDeleteBuffers(1, &weights_SSBO);


    /// draw shaders


    Camera *camera = new Camera(windowDim, worldDimensions);

    VertexFragmentShader *renderShader = new VertexFragmentShader("../Shaders/glsl/vertexShader.glsl", "../Shaders/glsl/fragmentShader.glsl");
    renderShader->use();
    renderShader->setFloat("worldSize", glm::length(worldDimensions));


    WindowInputManager windowInputManager(&window, camera);
    int width = 800;
    int height = 600;
    int c = 0;

    std::ofstream outFile("standard_deviation.txt");


    while (!glfwWindowShouldClose(window.getWindow()))
    {

        //Draw
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderShader->use();
        renderShader->setMat4("modelViewProjection", camera->getModelViewProjection());
        renderShader->setVec3("cameraPos", camera->getPosition());
        renderShader->setBool("pointSize", false);
        glDrawArrays(GL_POINTS, 0, numParticles);




        /// update particles positions (compute)
        forceCalculator->use();
        forceCalculator->setInt("numParticles", numParticles);
        glDispatchCompute(ceil(numParticles / blockSize), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        positionCalculator->use();
        positionCalculator->setInt("numParticles", numParticles);
        glDispatchCompute(ceil(numParticles / blockSize), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


        //unsigned char* pixels = new unsigned char[4 * width * height];

        //// Read pixels from the default frame buffer
        //glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        //// Flip the image vertically (if necessary, stb_image_write will write it upside down)
        //for (int i = 0; i < height / 2; i++) {
        //    for (int j = 0; j < 4 * width; j++) {
        //        std::swap(pixels[i * 4 * width + j], pixels[(height - 1 - i) * 4 * width + j]);
        //    }
        //}

        //// Save the image (e.g., as a PNG)
  
        //stbi_write_png(format("C:/Users/tempdelta/Desktop/nbody/images/output_%d.png", c).c_str(), width, height, 4, pixels, 4 * width);
        //c += 1;

        //// Cleanup
        //delete[] pixels;




        /// Stuff
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, forces_SSBO);
        // Map the position SSBO memory to CPU-accessible memory
        glm::vec4* positions = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


        glm::vec4 mean = calculateMean(positions, numParticles);
        glm::vec4 variance = calculateVariance(positions, numParticles, mean);
        glm::vec4 standardDeviation = glm::sqrt(variance);
        float standardD = (mean.x + mean.y + mean.z) / 3;
        //std::cout << "Standard Deviation: " << mean.x << mean.y << mean.z << std::endl;
        std::stringstream ss;
        ss << standardD;
        std::string standardDeviationString = ss.str();
        outFile << standardDeviationString << std::endl;

        // Close the file (inside the loop)
        //outFile.close();


        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    //delete everything !;
}

