#include "robot_arm.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <unistd.h>
#include <limits.h>
#include <fstream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include "shader_program.h"  

void RobotArm::renderSimpleCube(const ShaderProgram& shader) {
    // Simple cube vertices with colors
    float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    };
    
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 4, 7, 7, 3, 0,
        1, 5, 6, 6, 2, 1,
        3, 2, 6, 6, 7, 3,
        0, 1, 5, 5, 4, 0
    };
    
    static GLuint VAO, VBO, EBO;
    static bool initialized = false;
    
    if (!initialized) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        initialized = true;
        
        std::cout << "Fallback cube initialized" << std::endl;
    }
    
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(2.0f)); // Make it bigger
    shader.setUniform("model", model_matrix);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

RobotArm::RobotArm() {
    joints[0] = RobotJoint(0, -240.0f, 240.0f, 225.0f);
    joints[1] = RobotJoint(1, -120.0f, 120.0f, 150.0f);
    joints[2] = RobotJoint(2, 0.0f, 160.0f, 275.0f);
    joints[3] = RobotJoint(3, -200.0f, 200.0f, 412.0f);
    joints[4] = RobotJoint(4, -120.0f, 120.0f, 450.0f);
    joints[5] = RobotJoint(5, -360.0f, 360.0f, 720.0f);
    
    dh_parameters[0] = {0.0f, 0.1f, 0.0f, glm::radians(-90.0f)};
    dh_parameters[1] = {0.0f, 0.0f, 0.2f, 0.0f};
    dh_parameters[2] = {0.0f, 0.0f, 0.2f, 0.0f};
    dh_parameters[3] = {0.0f, 0.1f, 0.0f, glm::radians(-90.0f)};
    dh_parameters[4] = {0.0f, 0.0f, 0.0f, glm::radians(90.0f)};
    dh_parameters[5] = {0.0f, 0.05f, 0.0f, 0.0f};
    
    for (int i = 0; i < 6; i++) {
        isAnimating[i] = false;
        targetAngles[i] = 0.0f;
        animationSpeeds[i] = 0.0f;
    }
}

void RobotArm::initialize() {
    std::cout << "=== ROBOT ARM INITIALIZATION DEBUG ===" << std::endl;
    
    // Get current working directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    }
    
    // List files in current directory
    std::cout << "Listing current directory contents:" << std::endl;
    system("ls -la");
    
    // Check if assets directory exists
    std::cout << "Checking assets directory:" << std::endl;
    system("ls -la assets/ 2>&1 || echo 'Assets directory not found'");
    
    // Try multiple possible asset paths
    std::vector<std::string> modelPaths = {
        "assets/Yuki.glb",
        "./assets/Yuki.glb",
        "../assets/Yuki.glb",
        "../../assets/Yuki.glb",
        "Yuki.glb",
        "./Yuki.glb"
    };
    
    // Also check build directory
    std::cout << "Checking build directory:" << std::endl;
    system("ls -la build/linux/x86_64/release/ 2>&1 || echo 'Build directory not found'");
    system("ls -la build/linux/x86_64/release/assets/ 2>&1 || echo 'Build assets directory not found'");
    
    // Add build directory paths
    modelPaths.push_back("build/linux/x86_64/release/assets/Yuki.glb");
    modelPaths.push_back("./build/linux/x86_64/release/assets/Yuki.glb");
    
    bool modelLoaded = false;
    for (const auto& path : modelPaths) {
        std::cout << "=== Trying to load model from: " << path << " ===" << std::endl;
        
        // Check if file exists with more details
        std::ifstream testFile(path);
        if (testFile.good()) {
            std::cout << "✓ File exists: YES" << std::endl;
            testFile.close();
            
            // Get file size
            std::ifstream file(path, std::ifstream::ate | std::ifstream::binary);
            std::cout << "✓ File size: " << file.tellg() << " bytes" << std::endl;
            file.close();
        } else {
            std::cout << "✗ File exists: NO" << std::endl;
            continue;
        }
        
        if (loadModel(path)) {
            modelLoaded = true;
            std::cout << "✓ SUCCESS: Loaded robot model from: " << path << std::endl;
            std::cout << "✓ Number of meshes loaded: " << model.getMeshes().size() << std::endl;
            break;
        } else {
            std::cout << "✗ FAILED: Could not load from: " << path << std::endl;
        }
    }
    
    if (!modelLoaded) {
        std::cout << "⚠ WARNING: No robot model found. Program will run without 3D visualization." << std::endl;
        std::cout << "✓ Creating fallback visualization..." << std::endl;
    } else {
        std::cout << "✓ Robot model loaded successfully!" << std::endl;
    }
    std::cout << "=== END ROBOT ARM DEBUG ===" << std::endl;
}

void RobotArm::update(float delta_time) {
    for (int i = 0; i < 6; i++) {
        if (isAnimating[i]) {
            float current = joints[i].getCurrentAngle();
            float target = targetAngles[i];
            float speed = animationSpeeds[i];
            
            float diff = target - current;
            float max_change = speed * delta_time;
            
            if (std::abs(diff) <= max_change) {
                joints[i].setAngle(target);
                isAnimating[i] = false;
            } else {
                joints[i].setAngle(current + std::copysign(max_change, diff));
            }
        }
    }
    
    updateDHParameters();
}

void RobotArm::render(const ShaderProgram& shader) {
    // Only render if we have a model loaded
    if (!model.getMeshes().empty()) {
        std::cout << "Rendering robot model with " << model.getMeshes().size() << " meshes" << std::endl;
        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1f));
        model.render(shader, model_matrix);
    } else {
        // Render simple cube as fallback
        std::cout << "Rendering fallback cube (no model loaded)" << std::endl;
        renderSimpleCube(shader);
    }
}

void RobotArm::setJointAngles(const std::array<float, 6>& angles, bool animate, float speed_factor) {
    for (int i = 0; i < 6; i++) {
        setJointAngle(i, angles[i], animate, speed_factor);
    }
}

void RobotArm::setJointAngle(int index, float angle, bool animate, float speed_factor) {
    if (index < 0 || index >= 6) return;
    
    if (animate) {
        targetAngles[index] = angle;
        animationSpeeds[index] = joints[index].getMaxSpeed() * speed_factor;
        isAnimating[index] = true;
    } else {
        joints[index].setAngle(angle);
        isAnimating[index] = false;
    }
}

void RobotArm::resetToHomePosition(bool animate) {
    std::array<float, 6> home_angles = {0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f};
    setJointAngles(home_angles, animate);
}

bool RobotArm::loadModel(const std::string& path) {
    std::cout << "=== LOAD MODEL DEBUG: " << path << " ===" << std::endl;
    
    // Try multiple file formats
    std::vector<std::string> extensions = {".glb", ".gltf", ".obj", ".fbx", ".dae"};
    
    for (const auto& ext : extensions) {
        std::string testPath = path;
        if (ext != ".glb") {
            size_t dotPos = path.find_last_of(".");
            if (dotPos != std::string::npos) {
                testPath = path.substr(0, dotPos) + ext;
            } else {
                testPath = path + ext;
            }
        }
        
        std::cout << "Trying to load: " << testPath << std::endl;
        if (model.loadGLTF(testPath)) {
            std::cout << "✓ Successfully loaded model: " << testPath << std::endl;
            return true;
        } else {
            std::cout << "✗ Failed to load model: " << testPath << std::endl;
        }
    }
    
    std::cout << "✗ All model loading attempts failed for: " << path << std::endl;
    return false;
}

glm::mat4 RobotArm::getEndEffectorTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);
    
    for (int i = 0; i < 6; i++) {
        const auto& dh = dh_parameters[i];
        transform = transform * calculateDHMatrix(dh.theta, dh.d, dh.a, dh.alpha);
    }
    
    return transform;
}

std::array<float, 6> RobotArm::getJointAngles() const {
    std::array<float, 6> angles;
    for (int i = 0; i < 6; i++) {
        angles[i] = joints[i].getCurrentAngle();
    }
    return angles;
}

void RobotArm::updateDHParameters() {
    for (int i = 0; i < 6; i++) {
        dh_parameters[i].theta = glm::radians(joints[i].getCurrentAngle());
    }
}

glm::mat4 RobotArm::calculateDHMatrix(float theta, float d, float a, float alpha) const {
    glm::mat4 matrix(1.0f);
    
    matrix[0][0] = cos(theta);
    matrix[0][1] = -sin(theta) * cos(alpha);
    matrix[0][2] = sin(theta) * sin(alpha);
    matrix[0][3] = a * cos(theta);
    
    matrix[1][0] = sin(theta);
    matrix[1][1] = cos(theta) * cos(alpha);
    matrix[1][2] = -cos(theta) * sin(alpha);
    matrix[1][3] = a * sin(theta);
    
    matrix[2][1] = sin(alpha);
    matrix[2][2] = cos(alpha);
    matrix[2][3] = d;
    
    return matrix;
}