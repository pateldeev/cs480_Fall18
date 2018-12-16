#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "camera.h"
#include "board.h"

#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

class Graphics {
public:
	Graphics(void) = delete;
	Graphics(const glm::uvec2 & windowSize, const glm::vec3 & eyePos, const glm::vec3 & eyeFocus, const GameInfo & game);
	~Graphics(void);

	//Graphics is not meant to be copied or moved
	Graphics(const Graphics &) = delete;
	Graphics(Graphics &&) = delete;
	Graphics& operator=(const Graphics &) = delete;
	Graphics& operator=(Graphics&&) = delete;

	//to add and change shaders
	void AddShaderSet(const std::string & setName, const std::string & vertexShaderSrc, const std::string & fragmentShaderSrc);
	void UseShaderSet(const std::string & setName);

	void Update(unsigned int dt);
	void Render(void);

	//for movement for camera
	void ZoomIn(float moveAmount);       // Move the camera location closer to the focus location
	void ZoomOut(float moveAmount);      // Move the camera location away from the focus location
	void MoveForward(float moveAmount);  // Move both the camera and focus positions toward the direction the camera is facing
	void MoveBackward(float moveAmount); // Move both the camera and focus positions away from the direction the camera is facing
	void MoveRight(float moveAmount);    // Move both the camera and focus positions right of the direction the camera is facing
	void MoveLeft(float moveAmount);     // Move both the camera and focus positions left of the direction the camera is facing
	void MoveUp(float moveAmount);       // Increase both the camera and focus position's y coordinate
	void MoveDown(float moveAmount);     // Decrease both the camera and focus position's y coordinate
	void RotateCamera(float newX, float newY); // Changes the focus position to cause the camera to look 'left,' 'right,' 'up,' or 'down' according to mouse movement

	void UpdateCamera(const glm::vec3 & eyePos, const glm::vec3 & eyeFocus);
	glm::vec3 GetEyePos(void) const;
	glm::vec3 GetEyeFocus(void) const;

	//for changing lighting
	void ChangeAmbientLight(const glm::vec3 & change);
	void ChangeDiffuseLight(const glm::vec3 & change);
	void ChangeSpecularLight(const glm::vec3 & change);

	void LeftClick(const glm::vec2 & mousePosition); //call this from engine, and it should successfully click on object

	// Updates the board one generation, according to Conway's rules
	void MoveForwardGeneration(void);

private:
	std::string ErrorString(const GLenum error) const;

	void UpdateCameraBindings(void); //updates bindings for camera in shader - need to call for camera change to take effect

	glm::vec3 GetPositionUnder(const glm::vec2 & mousePosition); //raycast and find position mouse is pointing at

	Camera m_camera;

	float m_yaw;
	float m_pitch;

	Board * m_board;
	int m_generation; // What generation number the board is at

	glm::uvec2 m_screenSize; //required for calculating mouse position in 3d space
};

#endif /* GRAPHICS_H */
