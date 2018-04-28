#pragma once
#include "Tool.h"
class Camera {

public:
	static float fov;
	static  glm::vec3 cameraPos;
	static  glm::vec3 cameraFront;
	glm::mat4 getViewMat() { return viewMat; }
	static void processKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float CameraSpeed = 2.5f * deltaTime;
		if (action == GLFW_REPEAT) {
			if (key == GLFW_KEY_W) {
				cameraPos += CameraSpeed * cameraFront;
			}
			if (key == GLFW_KEY_S) {
				cameraPos -= CameraSpeed * cameraFront;
			}

			if (key == GLFW_KEY_A) {
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * CameraSpeed;
			}

			if (key == GLFW_KEY_D) {
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * CameraSpeed;
			}
		}
		refreshMat();
	}
	static void processMousePosition(GLFWwindow* window, double xPos, double yPos) {

		if (firstMouse) {
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		float xOffset = xPos - lastX;
		float yOffset = lastY - yPos;
		lastX = xPos;
		lastY = yPos;

		float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch >89.0f) {
			pitch = 89.0f;
		}
		if (pitch < -89.0f) {
			pitch = -89.0f;
		}

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
		refreshMat();
	}

	static void processScroll(GLFWwindow* window, double xOffset, double yOffset) {
		if (fov >= 1.0f && fov <= 45.0f) {
			fov -= yOffset;
		}
		if (fov <= 1.0f) {
			fov = 1.0f;
		}
		if (fov >= 45.0f) {
			fov = 45.0f;
		}
	}

private:
	static void refreshMat(void) { viewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); }
	static  glm::vec3 cameraUp;
	static  glm::mat4 viewMat;
	static  float     deltaTime;
	static  float     lastFrame;
	static  float     yaw;
	static  float     pitch;
	static  float     lastX;
	static  float     lastY;
	static  bool      firstMouse;
};