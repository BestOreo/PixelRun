#include "ParticleGenerator.h"
#include "GameWorld.h"
#include "TimerManager.h"


void ParticleGenerator::ExecutionForTimer() {
	if (rate > 50) {
		//cout << "Down" << endl;
		rate = 50;
		velocity[1] -= 4;
	}
	else {
		//cout << "Up" << endl;
		rate = 600;
		velocity[1] += 4;
	}
	//TimerManager::AddTimer_Particles(this, &ParticleGenerator::AddTimer, 10);
}

void ParticleGenerator::AddTimer(GLfloat time) {
	TimerManager::AddTimer_Particles(this, &ParticleGenerator::ExecutionForTimer, 5);
}


void Particle::SetLifeTime(GLfloat LifeTime) {
	Life = LifeTime;
	OriginalLife = LifeTime;
}



void Particle::SetStepChange(GLfloat StepRate[4], glm::vec4  StepColor[4], float Size[4]) {
	for (int i = 0; i < 4; i++) {
		StepArray[i] = StepRate[i];
		ColorArray[i] = StepColor[i];
		SizeArray[i] = Size[i];
	}

}

glm::vec4 Particle::UpdateColor() {
	GLfloat NowRate = 1 - Life / OriginalLife;
	vec4 temp_color;
	for (int i = 0; i < 4 - 1; i++) {
		if (NowRate >= StepArray[i] && NowRate <= StepArray[i + 1]) {
			temp_color = ((StepArray[i + 1] - NowRate) / (StepArray[i + 1] - StepArray[i]))*ColorArray[i] + ((NowRate - StepArray[i]) / (StepArray[i + 1] - StepArray[i]))*ColorArray[i + 1];
		}
	}
	//temp_color[3] *= (1 - NowRate);
	return temp_color;
}

float Particle::UpdateScale() {
	GLfloat NowRate = 1 - Life / OriginalLife;
	for (int i = 0; i < 4 - 1; i++) {
		if (NowRate >= StepArray[i] && NowRate <= StepArray[i + 1]) {
			return  ((StepArray[i + 1] - NowRate) / (StepArray[i + 1] - StepArray[i]))*SizeArray[i] + ((NowRate - StepArray[i]) / (StepArray[i + 1] - StepArray[i]))*SizeArray[i + 1];
		}
	}
}


ParticleGenerator::ParticleGenerator(Shader *shader, GLuint textureId, GLuint amount,
	GLfloat CenterX, GLfloat CenterY, GLfloat CenterZ, GLfloat Radius, vec3 SpeedUp, int BlendMode, int DrawPatternMode)
	: shader(shader), textureId(textureId), amount(amount), CenterX(CenterX), CenterY(CenterY), CenterZ(CenterZ),
	Radius(Radius), SpeedUp(SpeedUp), BlendMode(BlendMode), DrawPatternMode(DrawPatternMode)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt)
{
	GLuint newParticles = 0;

	accumulateTime += dt;
	if (accumulateTime > 1 / rate) {
		newParticles = accumulateTime * rate;
		accumulateTime -= newParticles * 1 / rate;
	}

	// Add new particles
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		if (unusedParticle != -1) {
			this->respawnParticle(this->particles[unusedParticle]);
		}
		else
		{
			break;
		}
	}

	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{
			switch (DrawPatternMode) {
			case 0:UpdatePattern_Default(p, dt); break;
			case 1:UpdatePattern_LightLoop(p, dt); break;
			case 2:UpdatePattern_Fire(p, dt); break;
			default: std::cout << "You didn't set which parttern to draw" << std::endl;
			}
		}
	}
}

// Render all particles
void ParticleGenerator::Draw()
{
    glm::mat4 projection = glm::perspective(GameWorld::MainCamera->Zoom, (float)Global::WIN_WIDTH / (float)Global::WIN_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = GameWorld::MainCamera->GetViewMatrix();

	// Use additive blending to give it a 'glow' effect
	if (BlendMode == 0) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else if (BlendMode == 1) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->shader->Use();
	this->shader->SetMatrix4("vp_mat", projection * view);
	this->shader->SetVector3f("camera_right", GameWorld::MainCamera->Right);
	this->shader->SetVector3f("camera_up", GameWorld::MainCamera->Up);
	glBindTexture(GL_TEXTURE_2D, this->textureId);

	if (needSort) {
		Sort();
		int loop = copyoffset.size();
		for (int j = 0; j < loop; j++) {
			for (std::map<float, int>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
			{
				Particle particle = particles[it->second];

                particle.Position += copyoffset[j].offset;

				if (particle.Life > 0.0f)
				{
					this->shader->SetFloat("scale", particle.Scale);
					this->shader->SetVector2f("random_offset", particle.RandomOffset);
					this->shader->SetVector3f("particle_center_worldspace",
						particle.Position);
					this->shader->SetVector4f("particle_color", particle.Color);

					glBindVertexArray(this->VAO);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}
			}
		}
	}
        
	else {
		int loop = copyoffset.size();
		for (int j = 0;j<loop; j++) {
			int size = particles.size();
			for (GLuint i = 0; i < size; i++)
			{
				Particle particle = particles[i];

				particle.Position += copyoffset[j].offset;

				if (particle.Life > 0.0f)
				{
					this->shader->SetFloat("scale", particle.Scale);
					this->shader->SetVector2f("random_offset", particle.RandomOffset);
					this->shader->SetVector3f("particle_center_worldspace",
						particle.Position);
					this->shader->SetVector4f("particle_color", particle.Color);

					glBindVertexArray(this->VAO);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}
			}
		}
	}

	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::AddCopy(int id, vec3 offset) {
	struct copynode t;
	t.id = id;
	t.offset = vec3(offset);
	copyoffset.push_back(t);
}

void ParticleGenerator::RemoveCopy(int id) {
	vector<struct copynode>::iterator it;
	for (it = copyoffset.begin(); it != copyoffset.end();) {
		if ((*it).id == id) {
			it = copyoffset.erase(it);
		}
		else {
			it++;
		}
	}
}


#define PAR_SIZE 0.35
void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, PAR_SIZE, 0.0f, 1.0f,
        PAR_SIZE, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, PAR_SIZE, 0.0f, 1.0f,
        PAR_SIZE, PAR_SIZE, 1.0f, 1.0f,
        PAR_SIZE, 0.0f, 1.0f, 0.0f
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
	
	if (idd == 1) {
		int i = 0;
	}

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	return -1;
}

void ParticleGenerator::respawnParticle(Particle &particle)
{
	if (width > 0) {
		int widthInt = (int)(width * 100.0f);
		float randRadian = (rand() % 31415) / 10000.0f;
		/*GLfloat randomOffsetX = ((rand() % widthInt) - widthInt / 2) / 100.0f;
		GLfloat randomOffsetY = ((rand() % widthInt) - widthInt / 2) / 100.0f;*/
		//GLfloat randomOffsetX = Radius * cos(randRadian);
		//GLfloat randomOffsetY = Radius * sin(randRadian);
		particle.RandomOffset = vec2(0, 0);
	}

	particle.phase = delPI * RadianCounter;

	vec3 randPosition;
	switch (DrawPatternMode)
	{
	case 0: Init_Pattern_Default_Respawn(particle); break;
	case 1: Init_Pattern_LightLoop_Respawn(particle); break;
	case 2: Init_Pattern_Fire_Respawn(particle); break;
	default:break;
	}


	if (bindObj == nullptr) {
		//particle.Position = randPosition;
	}
	else
	{
		particle.Position += bindObj->trans->position;
	}

	particle.Color = startColor;
	particle.SetLifeTime(lifeTime);
	particle.Scale = GetRandomScale();
	particle.Velocity = velocity;

	// Change Color gradually
	particle.SetStepChange(DefaultStep, DefaultStepColor, DefaultStepSize);
}

void ParticleGenerator::Sort()
{
	sorted.clear();
    int size = particles.size();
	for (GLuint i = 0; i < size; i++) // windows contains all window positions
	{
		GLfloat distance = glm::length(GameWorld::MainCamera->Position - particles[i].Position);
		sorted[distance] = i;
	}
}

float ParticleGenerator::GetRandomScale()
{
	return MiniMath::GetRandomFloat(startScale, startScaleRandomPercent);
}

glm::vec4 ParticleGenerator::GetLerpColor(float percent)
{
	if (percent <= endColorPoint) {
		return endColor;
	}
	else
	{
		percent -= endColorPoint;
		percent = percent / (1 - endColorPoint);
		return startColor * percent + endColor * (1 - percent);
	}
}

void ParticleGenerator::SetStep(GLfloat step[4]) {
	memcpy(this->DefaultStep, step, 4 * sizeof(step[0]));
}

void ParticleGenerator::SetStepSize(GLfloat stepsize[4]) {
	memcpy(this->DefaultStepSize, stepsize, 4 * sizeof(stepsize[0]));
}

void ParticleGenerator::SetStepColor(vec4 stepcolor[4]) {
	memcpy(this->DefaultStepColor, stepcolor, 4 * sizeof(stepcolor[0]));
	;
}

void ParticleGenerator::UpdatePattern_LightLoop(Particle &p, float dt) {
	// particle is alive, thus update
	p.Position += p.Velocity * dt + vec3(Radius*cos(p.phase), 0, Radius*sin(p.phase))*dt; // Vertical move and horizontal enlargement
																						  //p.Color = GetLerpColor(p.Life / lifeTime);
	p.Velocity += SpeedUp*dt; // Accelerate
	p.Color = p.UpdateColor();
	//p.Scale -= scaleChangeSpeed * dt;
	p.Scale = p.UpdateScale();
	if (p.Scale < endScale) {
		p.Scale = endScale;
	}
}

void ParticleGenerator::Init_Pattern_LightLoop_Respawn(Particle &particle) {
	GLfloat randomPositionX = (this->CenterX + Radius * cos(delPI * RadianCounter));
	GLfloat randomPositionY = this->CenterY + 0;
	GLfloat randomPositionZ = (this->CenterZ + Radius * sin(delPI * RadianCounter));
	RadianCounter = (RadianCounter + 1) % 360;
	particle.Position = vec3(randomPositionX, randomPositionY, randomPositionZ);
	particle.Color = startColor;
	particle.SetLifeTime(lifeTime);
	particle.Scale = GetRandomScale();
	particle.Velocity = velocity;

	// Change Color gradually
	particle.SetStepChange(DefaultStep, DefaultStepColor, DefaultStepSize);
}

void ParticleGenerator::UpdatePattern_Default(Particle &p, float dt) {
	// particle is alive, thus update
	p.Position += p.Velocity * dt; // Vertical move and horizontal enlargement
								   //p.Color = GetLerpColor(p.Life / lifeTime);
	p.Velocity += SpeedUp*dt; // Accelerate
	p.Color = p.UpdateColor();
	//p.Scale -= scaleChangeSpeed * dt;
	p.Scale = p.UpdateScale();
	if (p.Scale < endScale) {
		p.Scale = endScale;
	}
}

void ParticleGenerator::Init_Pattern_Default_Respawn(Particle &particle) {
    if (Radius != 0) {
        float randRadius = (rand() % int(Radius * 1000)) / 1000.0;
        float randRadian = 2 * (rand() % 31415) / 10000;
        GLfloat randomPositionX = (this->CenterX + randRadius * cos(randRadian));
        GLfloat randomPositionY = this->CenterY + 0;
        GLfloat randomPositionZ = (this->CenterZ + randRadius * sin(randRadian));
        RadianCounter = (RadianCounter + 1) % 360;
        particle.Position = vec3(randomPositionX, randomPositionY, randomPositionZ);
    }
    else {
        particle.Position = vec3(0);
    }
	particle.Color = startColor;
	particle.SetLifeTime(lifeTime);
	particle.Scale = GetRandomScale();
	particle.Velocity = velocity;

	// Change Color gradually
	particle.SetStepChange(DefaultStep, DefaultStepColor, DefaultStepSize);
}

void ParticleGenerator::UpdatePattern_Fire(Particle &p, float dt) {
	// particle is alive, thus update
	p.Position += p.Velocity * dt; // Vertical move and horizontal enlargement
								   //p.Color = GetLerpColor(p.Life / lifeTime);
	p.Velocity += SpeedUp*dt + vec3((rand() % 1000 - 500) / 3500.0f, (rand() % 1000 - 300) / 8500.0f, (rand() % 1000 - 500) / 3500.0f); // Accelerate
	p.Color = p.UpdateColor();
	if (p.Life / p.OriginalLife > 0.4)
		p.Color[3] = 0.4 *  (1 - p.Life / p.OriginalLife);

	//p.Scale -= scaleChangeSpeed * dt;
	p.Scale = p.UpdateScale();
	if (p.Scale < endScale) {
		p.Scale = endScale;
	}
}

void  ParticleGenerator::Init_Pattern_Fire_Respawn(Particle &particle) {
	float randRadius = (rand() % int(Radius / 2 * 1000)) / 1000.0;
	float randRadian = 2 * (rand() % 31415) / 10000;
	GLfloat randomPositionX = (this->CenterX + randRadius * cos(randRadian));
	GLfloat randomPositionY = this->CenterY + rand() % 1000 / 3000.0f;
	GLfloat randomPositionZ = (this->CenterZ + randRadius * sin(randRadian));
	RadianCounter = (RadianCounter + 1) % 360;
	particle.Position = vec3(randomPositionX, randomPositionY, randomPositionZ);
	particle.SetLifeTime(lifeTime);
	particle.Velocity = velocity;

	// Change Color gradually
	particle.SetStepChange(DefaultStep, DefaultStepColor, DefaultStepSize);
}

ParticleGenerator::~ParticleGenerator() {
	delete shader;
}

