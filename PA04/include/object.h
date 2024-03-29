#ifndef OBJECT_H
#define OBJECT_H

#include "objLoader.h"

class Object {
public:
	Object(const std::string & objFile, bool readColor);
	~Object(void);
	void Update(void);
	void Update(const glm::vec3 & translation, const glm::vec3 & scale, const glm::vec3 & rotationAngles);
	void Render(void);
	
	glm::mat4 GetModel(void);
	
	glm::vec3 GetTranslation(void) const;
	glm::vec3 GetScale(void) const;
	glm::vec3 GetRotationAngles(void) const;

private:
	glm::mat4 m_model;
	
	glm::vec3 m_translation;
	glm::vec3 m_scale;
	glm::vec3 m_rotationAngles;
	
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	GLuint VB;
	GLuint IB;
};

#endif /* OBJECT_H */
