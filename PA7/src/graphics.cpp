#include "graphics.h"

Graphics::Graphics(void) :
		m_camera(nullptr), m_shader(nullptr), m_followingPlanet(-1) {

}

Graphics::~Graphics(void) {
	for (Object * obj : m_planets)
		delete obj;

	for (std::vector<Object *> & temp : m_moons)
		for (Object * moon : temp)
			delete moon;

	delete m_camera;
	delete m_shader;
}

bool Graphics::Initialize(int width, int height, const std::string & vertShaderSrc, const std::string & fragShaderSrc, const glm::vec3 & eyePos,
		const ::glm::vec3 & focusPos) {

// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
	// std::cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK) {
		printf("GLEW Error: %s \n", glewGetErrorString(status));
		return false;
	}
#endif

	// For OpenGL 3
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Init Camera
	m_camera = new Camera(eyePos, focusPos);
	if (!m_camera->Initialize(width, height)) {
		printf("Camera Failed to Initialize\n");
		return false;
	}

	//Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize()) {
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER, vertShaderSrc)) {
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER, fragShaderSrc)) {
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize()) {
		printf("Program to Finalize\n");
		return false;
	}

	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_projectionMatrix not found\n");
		return false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_viewMatrix not found\n");
		return false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_modelMatrix not found\n");
		return false;
	}

	//enable depth testing
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void Graphics::AddPlanet(const Planet & planet, const std::string & moonObjFile) {

	m_planets.push_back(new Object(planet.objFile, planet.radius.x, planet.radius.y, planet.orbitSpeed, planet.rotationSpeed));
	m_planetNames.push_back(planet.name);
	m_planets.back()->SetScale(planet.modelScale);

	std::vector<Object *> moons;
	m_moons.push_back(moons);

	const int offsetPerMoon = 1;
	const int baseDistance = 3 + m_planets.back()->GetScale().y;
	float orbitSpeed, rotationSpeed, scale;
	for (unsigned int i = 0; i < planet.numMoons; ++i) {
		orbitSpeed = (float) (rand() / INT_MAX / 10000) + 0.00025;
		rotationSpeed = (float) (rand() / INT_MAX / 100000) + 0.000005;
		m_moons.back().push_back(
				new Object(moonObjFile, baseDistance + offsetPerMoon * i, baseDistance + offsetPerMoon * i, orbitSpeed, rotationSpeed));
		scale = (((float) (rand() / INT_MAX) / 4) + .25);
		m_moons.back().back()->SetScale(glm::vec3(scale, scale, scale));
	}
}

void Graphics::Update(unsigned int dt) {
	for (int i = 0; i < m_planets.size(); ++i) {
		m_planets[i]->Update(dt);

		for (int m = 0; m < m_moons[i].size(); ++m) {
			m_moons[i][m]->SetOrbitCenter(m_planets[i]->GetCurrentLocation());
			m_moons[i][m]->Update(dt);
		}
	}
}

void Graphics::FollowPlanet(const std::string & planetName) {
	m_followingPlanet = -1;
	for (int i = 0; i < m_planetNames.size(); ++i) {
		if (m_planetNames[i] == planetName)
			m_followingPlanet = i;
	}
}

bool Graphics::UpdateCamera(const glm::vec3 & eyePos, const glm::vec3 & eyeFocus) {
	m_camera->UpdatePosition(eyePos, eyeFocus);
	return true;
}

void Graphics::Render(void) {

	if (m_followingPlanet > -1) {
		glm::vec3 eyeFocus = { 0, 0, 0 };
		float distSun = glm::length(m_planets[m_followingPlanet]->GetCurrentLocation());
		glm::vec3 eyePos = (distSun + 10 * m_planets[m_followingPlanet]->GetScale().y)
				* glm::normalize(m_planets[m_followingPlanet]->GetCurrentLocation());
		eyePos.y += 3 * m_planets[m_followingPlanet]->GetScale().y;
		UpdateCamera(eyePos, eyeFocus);
	}

	//Clear the screen
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Start the correct program
	m_shader->Enable();

	//Send in the projection and view to the shader
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	//Render each planet and its moons
	for (int i = 0; i < m_planets.size(); ++i) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_planets[i]->GetModel()));
		m_planets[i]->Render();
		for (int m = 0; m < m_moons[i].size(); ++m) {
			glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moons[i][m]->GetModel()));
			m_moons[i][m]->Render();
		}
	}

	//Get any errors from OpenGL
	const GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		printf("Error initializing OpenGL! %d: %s \n", error, val.c_str());
	}
}

std::string Graphics::ErrorString(const GLenum error) const {
	if (error == GL_INVALID_ENUM)
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	else if (error == GL_INVALID_VALUE)
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	else if (error == GL_INVALID_OPERATION)
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	else if (error == GL_OUT_OF_MEMORY)
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	else
		return "None";
}

glm::vec3 Graphics::GetEyePos(void) const {
	return m_camera->GetEyePos();
}

glm::vec3 Graphics::GetEyeLoc(void) const {
	return m_camera->GetFocusPos();
}
