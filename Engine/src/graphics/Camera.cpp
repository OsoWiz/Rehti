#include <Camera.hpp>

#include <iostream>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

// Forward declarations
struct GLFWwindow;

// TODO figure out how camera should function with the player target.
struct Target
{
	glm::vec3 pos;
};
// standard cartesian coordinate system
constexpr glm::vec3 POSITIVE_X_AXIS(1.f, 0.f, 0.f);
constexpr glm::vec3 POSITIVE_Y_AXIS(0.f, 1.f, 0.f);
constexpr glm::vec3 POSITIVE_Z_AXIS(0.f, 0.f, 1.f);

constexpr float STANDARD_ZOOM(12.f); // zoom used in the constructor. 10 Units away from the target
constexpr float MIN_ZOOM(2.f);       // minimum zoom
constexpr float MAX_ZOOM(25.f);      // maximum zoom

/**
 * @ brief Camera class that enables orbiting around a target.
 *		This class allows querying for view and projection matrices or a combination of both.
 */



constexpr float heightOffset = 1.5f;
double Camera::mouseX = 0;
double Camera::mouseY = 0;
bool Camera::canMove = false;

Camera::Camera(glm::vec3 targetPos, float width, float height, float fovRad, float near, float far, float sensitivity)
	: target(targetPos.x, targetPos.y + heightOffset, targetPos.z), cameraMatrix(1.f) // identity
	,
	sensitivity(sensitivity), currentZoom(STANDARD_ZOOM), zoomSensitivity(50.f * sensitivity), width(width), height(height)
{

	projectionMatrix = glm::perspective(fovRad, width / height, near, far);
	projectionMatrix[1][1] *= -1; // flip y axis
	moveLocation(target - getForward() * currentZoom);
	orbitRotate(glm::vec2(0.f, 2.f));
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::mat4 view = glm::lookAt(getLocation(), target, POSITIVE_Y_AXIS);
	return view;
}

glm::mat4 Camera::getOrientation() const
{
	return cameraMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return projectionMatrix;
}

glm::mat4 Camera::getWorldToScreenMatrix() const
{
	return projectionMatrix * getViewMatrix();
}

glm::vec3 Camera::getCameraRay(double x, double y) const
{
	std::cout << "Asked screen coordinates: " << x << ", " << y << "\n"
		<< std::endl;
	// Create a ray in world space from the camera to the given screen coordinates.
	// The ray is normalized.
	float normx = (2.f * x) / width - 1.f;
	float normy = 1.f - (2.f * y) / height;
	glm::vec4 rayClip = glm::vec4(normx, normy, -1.f, 1.f);
	glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.f, 0.f);
	glm::vec3 ray = -glm::normalize(glm::vec3(cameraMatrix * rayEye));

	std::cout << "Generated ray: " << ray.x << ", " << ray.y << ", " << ray.z << "\n"
		<< std::endl;
	std::cout << "Ray origin: " << getLocation().x << ", " << getLocation().y << ", " << getLocation().z << "\n"
		<< std::endl;
	return ray;
}

uint32_t Camera::getUboSize()
{
	return sizeof(glm::mat4);
}

void Camera::orbitRotate(glm::vec2 rotationVec)
{
	// In radians
	float horizontalAngle = rotationVec.x * sensitivity;
	float verticalAngle = rotationVec.y * sensitivity;

	glm::mat4 rotatedHorizontal = glm::rotate(glm::mat4(1.f), horizontalAngle, POSITIVE_Y_AXIS);
	glm::vec3 rightVec = getRight();
	glm::mat4 rotatedVertical = glm::rotate(glm::mat4(1.f), verticalAngle, rightVec);
	glm::mat4 rotated = rotatedVertical * rotatedHorizontal * getCameraMatrixOrigon();
	cameraMatrix = glm::translate(glm::mat4(1.f), target) * rotated;
}

void Camera::zoom(float zoomAmount)
{
	float newZoom = currentZoom + zoomAmount * zoomSensitivity;
	// clamp zoom
	newZoom = glm::min(newZoom, MAX_ZOOM);
	currentZoom = glm::max(newZoom, MIN_ZOOM);
	setLocation(target - getForward() * currentZoom);
}

void Camera::setSensitivity(float newSensitivity, float newZoomSens)
{
	sensitivity = newSensitivity;
	zoomSensitivity = newZoomSens;
}

void Camera::moveLocation(glm::vec3 movement)
{
	cameraMatrix[3][0] += movement.x;
	cameraMatrix[3][1] += movement.y;
	cameraMatrix[3][2] += movement.z;
}

glm::mat4 Camera::getCameraMatrixOrigon() const
{
	glm::mat4 cameraMatrix = cameraMatrix;
	glm::vec3 targetToCamera = getLocation() - target;
	cameraMatrix[3][0] = targetToCamera.x;
	cameraMatrix[3][1] = targetToCamera.y;
	cameraMatrix[3][2] = targetToCamera.z;

	return cameraMatrix;
}

glm::vec3 Camera::getLocation() const
{
	return glm::vec3(glm::column(cameraMatrix, 3));
}

glm::vec3 Camera::getForward() const
{
	return glm::vec3(glm::column(cameraMatrix, 2));
}

glm::vec3 Camera::getRight() const
{
	return glm::vec3(glm::column(cameraMatrix, 0));
}

glm::vec3 Camera::getUp() const
{
	return glm::vec3(glm::column(cameraMatrix, 1));
}

glm::vec2 Camera::getSensitivities() const
{
	return glm::vec2(sensitivity, zoomSensitivity);
}

void Camera::move(glm::vec3 movement)
{
	moveLocation(movement);
	target += movement;
}

void Camera::setLocation(glm::vec3 location)
{
	cameraMatrix[3][0] = location.x;
	cameraMatrix[3][1] = location.y;
	cameraMatrix[3][2] = location.z;
}

void Camera::setTargetAndCamera(glm::vec3 location)
{
	glm::vec3 locationWithoffSet = location + glm::vec3(0.f, heightOffset, 0.f);
	glm::vec3 diff = getLocation() - target;
	target = locationWithoffSet;
	setLocation(locationWithoffSet + diff);
}

#pragma region CameraCallbacks

// todo movement callbacks

#pragma endregion