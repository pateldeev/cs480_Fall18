#include "board.h"

Board::Board(const gameInfo & game) :
		m_shaderCurrent(nullptr), m_ambientLevel(game.m_ambientLevel), m_diffuseLevel(game.m_object.m_diffuseLevel), m_specularLevel(
				game.m_object.m_specularLevel), m_shininessConst(game.m_object.m_shininess), m_spotlightLoc(0.0, 0.0, 0.0) {

	m_floor = new Object(game.m_object.m_objFile, game.m_sides[0].m_size, game.m_sides[0].m_startingLoc);
	m_floor->SetChangeRow(game.m_sides[0].m_directionRow);
	m_floor->SetChangeCol(game.m_sides[0].m_directionCol);

	m_roof = new Object(game.m_object.m_objFile, game.m_sides[1].m_size, game.m_sides[1].m_startingLoc);
	m_roof->SetChangeRow(game.m_sides[1].m_directionRow);
	m_roof->SetChangeCol(game.m_sides[1].m_directionCol);

	//load textures
	for (int i = 0; i < 11; ++i)
		m_floor->LoadTexture(game.m_textures[i], static_cast<ObjType>(i));
	m_floor->BindTextures();
}

Board::~Board(void) {
	for (std::pair<std::string, Shader *> temp : m_shaders)
		delete temp.second;

	delete m_floor;
	delete m_roof;
}

void Board::AddShaderSet(const std::string & setName, const std::string & vertexShaderSrc, const std::string & fragmentShaderSrc) {
	m_shaders.push_back(std::pair<std::string, Shader *>(setName, new Shader())); //Set up the shader
	m_shaders.back().second->AddShader(GL_VERTEX_SHADER, vertexShaderSrc); //Add the vertex shader
	m_shaders.back().second->AddShader(GL_FRAGMENT_SHADER, fragmentShaderSrc); //Add the fragment shader
	m_shaders.back().second->Finalize(); //Connect the program
}

void Board::UseShaderSet(const std::string & setName) {
	//find shader set
	unsigned int i;
	for (i = 0; i < m_shaders.size(); ++i) {
		if (m_shaders[i].first == setName) {
			m_shaderCurrent = m_shaders[i].second;
			break;
		}
	}
	if (i == m_shaders.size())
		throw std::string("Could not find shader set: " + std::string(setName));

	//helper function to locate uniform and bind it to variable
	auto bindUniform = [this](GLint & uniformBinding, const std::string & uniformName)->void {
		uniformBinding = this->m_shaderCurrent->GetUniformLocation(uniformName);
		if (uniformBinding == -1)
		throw std::string(std::string(uniformName) + " not found");
	};

	//find MVP matricies
	bindUniform(m_projectionMatrix, "projection");
	bindUniform(m_viewMatrix, "view");
	bindUniform(m_modelMatrix, "model");

	//find lighting uniforms
	bindUniform(m_lightPos, "lightPos");
	bindUniform(m_cameraPos, "eyePos");
	bindUniform(m_ambientProduct, "ambientP");
	bindUniform(m_diffuseProduct, "diffuseP");
	bindUniform(m_specularProduct, "specularP");
	bindUniform(m_shininess, "shininess");

	//find instancing uniforms
	bindUniform(m_instanceChangeRow, "changeRow");
	bindUniform(m_instanceChangeCol, "changeCol");
	bindUniform(m_instanceNumPerRow, "numPerRow");
	bindUniform(m_samplers, "samplers");
	bindUniform(m_sampleTypes, "sampleType");

	m_shaderCurrent->Enable();

	const int samplerNums[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	glUniform1iv(m_samplers, 11, samplerNums);
}

void Board::Update() {
	m_floor->SetType(0, 0, ObjType::P1_DEAD_FUTURE);
	m_floor->SetType(2, 3, ObjType::P1_DEAD_MARKED);
	m_floor->SetType(7, 3, ObjType::P1_ALIVE);
	m_floor->SetType(9, 9, ObjType::P2_DEAD_MARKED);
	m_floor->SetType(14, 8, ObjType::P2_ALIVE_MARKED);

	m_floor->Update();
	
	m_roof->Update();
}

void Board::Render(void) {
	if (!m_shaderCurrent) //Ensure shader is enabled
		throw std::string("No shader has been enabled!");

	UpdateInstanceBindings (m_floor);
	glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_floor->GetModel()));
	m_floor->Render();

	UpdateInstanceBindings (m_roof);
	glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_roof->GetModel()));
	m_roof->Render();
}

void Board::ChangeAmbientLight(const glm::vec3 & change) {
	m_ambientLevel += change;
	EnforceBounds (m_ambientLevel);
	UpdateLightBindings();
}

void Board::ChangeDiffuseLight(const glm::vec3 & change) {
	m_diffuseLevel += change;
	EnforceBounds (m_diffuseLevel);
	UpdateLightBindings();
}

void Board::ChangeSpecularLight(const glm::vec3 & change) {
	m_specularLevel += change;
	EnforceBounds (m_specularLevel);
	UpdateLightBindings();
}

void Board::UpdateSpotlightLoc(const glm::vec3 & location) {
	m_spotlightLoc = location;
	if (!m_shaderCurrent) //Ensure shader is enabled
		throw std::string("No shader has been enabled!");

	//update location
	glUniform3f(m_lightPos, m_spotlightLoc.x, m_spotlightLoc.y, m_spotlightLoc.z);
}

//updates bindings for camera in shader
void Board::UpdateCameraBindings(const glm::mat4 & viewMat, const glm::mat4 & projectionMat, const glm::vec3 & cameraPos) {
	if (!m_shaderCurrent) //Ensure shader is enabled
		throw std::string("No shader has been enabled!");

	//Bind shader uniforms
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(projectionMat));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniform3f(m_cameraPos, cameraPos.x, cameraPos.y, cameraPos.z);
}

//updates lighting binding in shader
void Board::UpdateLightBindings(void) {
	if (!m_shaderCurrent) //Ensure shader is enabled
		throw std::string("No shader has been enabled!");

	//add lighting varibles/uniforms to shaders if needed
	glUniform3f(m_lightPos, m_spotlightLoc.x, m_spotlightLoc.y, m_spotlightLoc.z);

	glUniform3f(m_ambientProduct, m_ambientLevel.x, m_ambientLevel.y, m_ambientLevel.z);
	glUniform3f(m_diffuseProduct, m_diffuseLevel.x, m_diffuseLevel.y, m_diffuseLevel.z);
	glUniform3f(m_specularProduct, m_specularLevel.x, m_specularLevel.y, m_specularLevel.z);
	glUniform1f(m_shininess, m_shininessConst);
}

void Board::UpdateInstanceBindings(Object * obj) {
	glm::vec3 changeRow = obj->GetChangeRow();
	glm::vec3 changeCol = obj->GetChangeCol();
	const std::vector<ObjType> types = obj->GetTypesList();

	glUniform1iv(m_sampleTypes, types.size(), reinterpret_cast<const int*>(&types[0]));
	glUniform3f(m_instanceChangeRow, changeRow.x, changeRow.y, changeRow.z);
	glUniform3f(m_instanceChangeCol, changeCol.x, changeCol.y, changeCol.z);
	glUniform1i(m_instanceNumPerRow, (int) obj->GetSize().x);
}

//rounds everything to be in range [min, max]
void Board::EnforceBounds(glm::vec3 & v, float min, float max) {
	if (v.x < min)
		v.x = min;
	else if (v.x > max)
		v.x = max;

	if (v.y < min)
		v.y = min;
	else if (v.y > max)
		v.y = max;

	if (v.z < min)
		v.z = min;
	else if (v.z > max)
		v.z = max;
}

