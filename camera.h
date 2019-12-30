#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include "frustum.h"


struct Camera
{
private:
	glm::vec3 cameraPos {};
	glm::vec3 cameraFront { 0.0f, 0.0f, 1.0f };
	glm::vec3 cameraUp { 0.0f, 1.0f, 0.0f };
	glm::vec3 viewDirection{};
	glm::mat4 rotationMatrix {};
	glm::mat4 viewMatrix{};
	glm::mat4 projectionMatrix {};
	Frustum frustum;

	float FoV {};
	float aspectRatio {};
	float nearClip {};
	float farClip {};
	
public:
	Camera() = default;

	Camera(float FoV, float aspectRatio, float nearClip, float farClip) : projectionMatrix{ glm::perspective(glm::radians(FoV), aspectRatio, nearClip, farClip) } {}

	const glm::mat4& getProjectionMatrix() const
	{
		return projectionMatrix;
	}

	Camera& setPos(const float x, const float y, const float z)
	{
		cameraPos = glm::vec3(x, y, z);
		calculateViewMatrix();
		return *this;
	}

	void setRotation(const float x, const float y, const float z, const float w)
	{
		const auto rotQuaternion = glm::quat(w, x, y, z);
		rotationMatrix = glm::mat4(rotQuaternion);
	}

	const glm::mat4& getRotationMatrix() const
	{
		return rotationMatrix;
	}

	const glm::vec3& getViewDir() const
	{
		return viewDirection;
	}

	void calculateViewDirection()
	{
		viewDirection = cameraPos + cameraFront;
	}

	glm::vec3 getViewDirectionNormalized() const
	{
		return glm::normalize(getViewDir());
	}

	const glm::mat4 getViewMatrix() const
	{
		return viewMatrix;
	}

	void calculateViewMatrix()
	{
		viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	void generateFrustum()
	{
		frustum = Frustum(projectionMatrix * viewMatrix);
	}

	bool isMeshInsideFrustum(const glm::vec3& boundsMin, const glm::vec3& boundsMax) const
	{
		return frustum.IsBoxVisible(boundsMin, boundsMax);
	}

};
