#include "Camera.h"
glm::vec3 Camera::cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Camera::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 Camera::viewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
float     Camera::deltaTime = 0.0f;
float     Camera::lastFrame = 0.0f;
float     Camera::yaw = -90.0f;
float     Camera::pitch = 0.0f;
float     Camera::lastX = 400;
float     Camera::lastY = 300;
float     Camera::fov = 45.0f;
bool      Camera::firstMouse = true;
