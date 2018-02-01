#include "LabelManager.h"
#include "GameWorld.h"
#include "SourceManager.h"
#include "GameManager.h"


enum DigitEnum
{
	_LEFTDIGIT = 0,
	_RIGHTDIGIT = 1
};

LabelManager::LabelManager(Shader *shader, GLuint textureId):
	shader(shader),textureId(textureId)
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void LabelManager::UpdateScore() {
    if (GameManager::player == nullptr) return;

	ScoreManager::score = GameManager::player->team.size()-1;
	char ch;
	if(this->digit == _LEFTDIGIT)
		ch = '0' + ScoreManager::score / 10;
	else
		ch = '0' + ScoreManager::score % 10;

	float uv_y = (ch / 16) / 16.0f;
	float uv_x = (ch % 16) / 16.0f;

	// Fill buffers
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> UVs;
	for (unsigned int i = 0; i<1; i++) {

		/*glm::vec4 vertex_up_left    =  glm::vec4(0.0 ,1.0, uv_x,				 uv_y);
		glm::vec4 vertex_up_right   =  glm::vec4(1.0 ,1.0, uv_x + 1.0f / 16.0f,  uv_y);
		glm::vec4 vertex_down_right =  glm::vec4(1.0, 0.0, uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec4 vertex_down_left  =  glm::vec4(0.0, 0.0, uv_x,				(uv_y + 1.0f / 16.0f));*/

		//glm::vec4 vertex_up_left = glm::vec4(x + i*size, y + size, uv_x, uv_y);
		//glm::vec4 vertex_up_right = glm::vec4(x + i*size + size, y + size, uv_x + 1.0f / 16.0f, uv_y);
		//glm::vec4 vertex_down_right = glm::vec4(x + i*size + size, y, uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		//glm::vec4 vertex_down_left = glm::vec4(x + i*size, y, uv_x, (uv_y + 1.0f / 16.0f));

		glm::vec4 vertex_up_left =    glm::vec4(0.0 + 0.10*this->digit, 0.2, uv_x, uv_y);
		glm::vec4 vertex_up_right =   glm::vec4(0.2 + 0.10*this->digit, 0.2, uv_x + 1.0f / 16.0f, uv_y);
		glm::vec4 vertex_down_right = glm::vec4(0.2 + 0.10*this->digit, 0.0, uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec4 vertex_down_left =  glm::vec4(0.0 + 0.10*this->digit, 0.0, uv_x, (uv_y + 1.0f / 16.0f));


		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);
	}

	GLuint VBO;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	
}

void LabelManager::DrawNameLabel() {
	if (IsScore == true) {
		UpdateScore();
		this->shader->SetVector3f("offset", this->offset);
	}

	glm::mat4 projection = glm::perspective(GameWorld::MainCamera->Zoom, (float)Global::WIN_WIDTH / (float)Global::WIN_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = GameWorld::MainCamera->GetViewMatrix();

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->shader->Use();
	this->shader->SetMatrix4("vp_mat", projection * view);
	this->shader->SetVector3f("camera_right", GameWorld::MainCamera->Right);
	this->shader->SetVector3f("camera_up", GameWorld::MainCamera->Up);
	glBindTexture(GL_TEXTURE_2D, this->textureId);

	this->shader->SetFloat("scale", scale);
	this->shader->SetVector3f("particle_center_worldspace",this->Position);
	this->shader->SetVector4f("particle_color", vec4(1.0,1.0,1.0,1.0));


	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);

}

void LabelManager::SetPositon(vec3 &positon) {
	this->Position = positon;
}

LabelManager::~LabelManager() {
	delete shader;
}

ScoreManager::ScoreManager() {
	Digit0 = new LabelManager(
		new Shader("_shader/Billboard1.vertexshader", "_shader/Billboard.fragmentshader"),
		TextureHelper::load2DTexture("_shader/Holstein.png", TEXTURE_FILTER_TYPE_MAN_MIN, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA)
		//TextureHelper::loadDDS("_shader/Holstein.DDS")
	);
	Digit0->digit = _RIGHTDIGIT;
	SourceManager::LabelVector.push_back(Digit0);
	Digit0->IsScore = true;
	//Digit0->offset = (vec3(-0.5f, -1.0, 0));

	Digit1 = new LabelManager(
		new Shader("_shader/Billboard1.vertexshader", "_shader/Billboard.fragmentshader"),
		TextureHelper::load2DTexture("_shader/Holstein.png", TEXTURE_FILTER_TYPE_MAN_MIN, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA)
		//TextureHelper::loadDDS("_shader/Holstein.DDS")
	);

	Digit1->digit = _LEFTDIGIT;
	SourceManager::LabelVector.push_back(Digit1);
	Digit1->IsScore = true;
}