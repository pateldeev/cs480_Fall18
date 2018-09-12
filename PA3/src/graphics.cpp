#include "graphics.h"

Graphics::Graphics(void) {

}

Graphics::~Graphics(void) {
	delete m_planet;
	delete m_moon;
	delete m_camera;
	delete m_shader;
}

bool Graphics::Initialize(int width, int height, const glm::vec3 & eyePos) {

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
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif

	// For OpenGL 3
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height, eyePos)) {
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Create the objects
	m_planet = new Object(4.5, .001, .0004);
	m_moon = new Object(3.0, .0005, .0002);
	m_moon->SetScale(glm::vec3(0.35, 0.35, 0.35));

	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize()) {
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER)) {
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER)) {
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

	return true;
}

void Graphics::Update(unsigned int dt) {
	// Update the object
	m_planet->Update(dt);
	m_moon->SetOrbitCenter(m_planet->GetOrbitLoc());
	//printf("C: %3f ; %3f - L: %3f ; %3f \n", m_moon->GetOrbitCenter().x, m_moon->GetOrbitCenter().y, m_moon->GetOrbitLoc().x, m_moon->GetOrbitLoc().y);
	m_moon->Update(dt);
}

void Graphics::Render(void) {
	//clear the screen
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	// Render the objects
	glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_planet->GetModel()));
	m_planet->Render();

	glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));
	m_moon->Render();

	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

bool Graphics::handleEvent(const SDL_Event & event) {

	if (event.type == SDL_KEYDOWN) {

		switch (event.key.keysym.sym) {

		case SDLK_s:
			m_planet->ToggleRotation(false);
			m_planet->ToggleOrbit(false);
			return true;

		case SDLK_c:
			m_planet->ToggleRotation(true);
			m_planet->ToggleOrbit(true);
			return true;

		case SDLK_r:
			m_planet->ToggleRotation(!m_planet->IsRotating());
			return true;

		case SDLK_o:
			m_planet->ToggleOrbit(!m_planet->IsOrbiting());
			return true;

		case SDLK_LEFT:
			m_planet->SetOrbitSpeed(std::abs(m_planet->GetOrbitSpeed()));
			return true;

		case SDLK_RIGHT:
			m_planet->SetOrbitSpeed(-1 * std::abs(m_planet->GetOrbitSpeed()));
			return true;

		case SDLK_UP:
			m_planet->SetRotationSpeed(std::abs(m_planet->GetRotationSpeed()));
			return true;

		case SDLK_DOWN:
			m_planet->SetRotationSpeed(-1 * std::abs(m_planet->GetRotationSpeed()));
			return true;

		}
	}

	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			m_planet->SetOrbitSpeed(-1 * m_planet->GetOrbitSpeed());
			return true;
		} else if (event.button.button == SDL_BUTTON_RIGHT) {
			m_planet->SetRotationSpeed(-1 * m_planet->GetRotationSpeed());
			return true;
		} else {
			printf("Unhandeled SDL mouse press. Could not understand button pressed: %d \n", event.button.button);
		}
	}
	return false;
}

std::string Graphics::ErrorString(GLenum error) {
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
