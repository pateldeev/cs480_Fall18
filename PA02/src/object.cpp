#include "object.h"

Object::Object(float orbitalRadius, float rotationSpeed, float orbitSpeed) :
		orbitRadius(orbitalRadius), speedRotation(rotationSpeed), speedOrbit(orbitSpeed), pauseRotation(false), pauseOrbit(false) {
	/*
	 # Blender File for a Cube
	 o Cube
	 v 1.000000 -1.000000 -1.000000
	 v 1.000000 -1.000000 1.000000
	 v -1.000000 -1.000000 1.000000
	 v -1.000000 -1.000000 -1.000000
	 v 1.000000 1.000000 -0.999999
	 v 0.999999 1.000000 1.000001
	 v -1.000000 1.000000 1.000000
	 v -1.000000 1.000000 -1.000000
	 s off
	 f 2 3 4
	 f 8 7 6
	 f 1 5 6
	 f 2 6 7
	 f 7 8 4
	 f 1 4 8
	 f 1 2 4
	 f 5 8 6
	 f 2 1 6
	 f 3 2 7
	 f 3 7 4
	 f 5 1 8
	 */

	Vertices = {
		{	{	1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
		{	{	1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{	{	-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{	{	-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
		{	{	1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
		{	{	1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
		{	{	-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
		{	{	-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}
	};

	Indices = {
		2, 3, 4,
		8, 7, 6,
		1, 5, 6,
		2, 6, 7,
		7, 8, 4,
		1, 4, 8,
		1, 2, 4,
		5, 8, 6,
		2, 1, 6,
		3, 2, 7,
		3, 7, 4,
		5, 1, 8
	};

	// The index works at a 0th index
	for(unsigned int i = 0; i < Indices.size(); i++)
	{
		Indices[i] = Indices[i] - 1;
	}

	angleRotation = angleOrbit = 0.0f;
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
}

Object::~Object(void) {
	Vertices.clear();
	Indices.clear();
}

void Object::Update(unsigned int dt) {

	if (!pauseRotation)
		angleRotation += dt * M_PI * speedRotation;

	if (!pauseOrbit)
		angleOrbit += dt * M_PI * speedOrbit;

	model = glm::translate(glm::vec3(orbitRadius * std::cos(angleOrbit), orbitRadius * std::sin(angleOrbit), 0.0));
	model = glm::rotate(model, angleRotation, glm::vec3(0.0, 1.0, 0.0));
}

void Object::Render(void) {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

glm::mat4 Object::GetModel(void) {
	return model;
}

void Object::SetRotationSpeed(float rotationSpeed) {
	speedRotation = rotationSpeed;
}

float Object::GetRotationSpeed(void) const {
	return speedRotation;
}

void Object::SetOrbitSpeed(float orbitSpeed) {
	speedOrbit = orbitSpeed;
}

float Object::GetOrbitSpeed(void) const {
	return speedOrbit;
}

void Object::ToggleRotation(bool rotate) {
	pauseRotation = !rotate;
}

bool Object::IsRotating(void) const {
	return !pauseRotation;
}

void Object::ToggleOrbit(bool orbit) {
	pauseOrbit = !orbit;
}

bool Object::IsOrbiting(void) const {
	return !pauseOrbit;
}

