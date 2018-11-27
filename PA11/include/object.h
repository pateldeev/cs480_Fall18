#ifndef OBJECT_H
#define OBJECT_H

#include "graphics_headers.h"

#include <vector>

enum ObjType {
	DEAD = 0, P1_ALIVE = 1, P2_ALIVE = 2, P1_DYING = 3, P2_DYING = 4, P1_MARKED = 5, P2_MARKED = 6
};

class Object {
public:
	Object(void) = delete;
	Object(const std::string & objFile, const glm::uvec2 & size, const glm::vec3 & traslation = glm::vec3(0, 0, 0), const glm::vec3 & rotation =
			glm::vec3(0, 0, 0), const glm::vec3 & scale = glm::vec3(1, 1, 1));
	~Object(void);

//Object is not meant to be copied or moved
	Object(const Object &) = delete;
	Object(Object &&) = delete;
	Object& operator=(const Object &) = delete;
	Object& operator=(Object&&) = delete;

	void Update(void);
	void Render(void);

	glm::mat4 GetModel(void);

	glm::vec3 GetTranslation(void) const;
	void SetTranslation(const glm::vec3 & translation);

	glm::vec3 GetRotation(void) const;
	void SetRotation(const glm::vec3 & rotation);

	glm::vec3 GetScale(void) const;
	void SetScale(const glm::vec3 & scale);

	ObjType GetType(unsigned int r, unsigned int c) const;
	std::vector<ObjType> & GetTypesList(void);
	void SetType(const ObjType type, unsigned int r, unsigned int c);

	float GetDistanceFromPoint(glm::vec3 point) const;

	void LoadTexture(const std::string & textureFile, ObjType type = ObjType::DEAD);

private:
	void LoadObjAssimp(const std::string & objFile);

	glm::mat4 m_model;

	glm::vec3 m_translation;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	glm::uvec2 m_numInstances;
	std::vector<ObjType> m_types;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	GLuint VB;
	GLuint IB;

	GLuint m_textures[7]; //vector of texture locations on GPU {follow enumaration layout}
};

#endif /* OBJECT_H */
