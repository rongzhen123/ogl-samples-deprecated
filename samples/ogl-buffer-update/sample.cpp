//**********************************
// OpenGL buffer update
// 11/08/2009
//**********************************
// Christophe Riccio
// g.truc.creation@gmail.com
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include "sample.hpp"

namespace
{
	std::string const SAMPLE_NAME = "OpenGL buffer update";	
	GLint const SAMPLE_MAJOR_VERSION = 2;
	GLint const SAMPLE_MINOR_VERSION = 1;
	std::string const VERTEX_SHADER_SOURCE(glf::DATA_DIRECTORY + "210/flat-color.vert");
	std::string const FRAGMENT_SHADER_SOURCE(glf::DATA_DIRECTORY + "210/flat-color.frag");

	GLsizei const VertexCount = 6;
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(-1.0f,-1.0f)
	};
}

sample::sample
(
	std::string const & Name, 
	glm::ivec2 const & WindowSize,
	glm::uint32 VersionMajor,
	glm::uint32 VersionMinor
) :
	window(Name, WindowSize, VersionMajor, VersionMinor),
	ProgramName(0)
{}

sample::~sample()
{}

bool sample::check() const
{
	return glf::checkError("sample::check");
}

bool sample::begin(glm::ivec2 const & WindowSize)
{
	this->WindowSize = WindowSize;

	bool Validated = true;
	if(Validated)
		Validated = this->initProgram();
	if(Validated)
		Validated = this->initArrayBuffer();

	return Validated && glf::checkError("sample::begin");
}

bool sample::end()
{
	// Delete objects
	glDeleteBuffers(1, &this->BufferName);
	glDeleteProgram(this->ProgramName);

	return glf::checkError("sample::end");
}

void sample::render()
{
	// Compute the MVP (Model View Projection matrix)
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, RotationCurrent.y, glm::vec3(-1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// Set the display viewport
	glViewport(0, 0, WindowSize.x, WindowSize.y);

	// Clear color buffer with black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind program
	glUseProgram(ProgramName);

	// Set the value of MVP uniform.
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, this->BufferName);
	glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), GLF_BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glEnableVertexAttribArray(glf::semantic::attr::POSITION);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);

	glf::checkError("sample::render");
}

bool sample::initProgram()
{
	bool Validated = true;
	
	// Create program
	if(Validated)
	{
		this->ProgramName = glf::createProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
		glLinkProgram(this->ProgramName);
		Validated = glf::checkProgram(this->ProgramName);
	}

	// Get variables locations
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		UniformColor = glGetUniformLocation(ProgramName, "Diffuse");
	}

	// Set some variables 
	if(Validated)
	{
		// Bind the program for use
		glUseProgram(ProgramName);

		// Set uniform value
		glUniform4fv(UniformColor, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		// Unbind the program
		glUseProgram(0);
	}

	return Validated && glf::checkError("sample::initProgram");
}
/*
// Buffer update using glBufferSubData
bool sample::initArrayBuffer()
{
	// Generate a buffer object
	glGenBuffers(1, &BufferName);

	// Bind the buffer for use
    glBindBuffer(GL_ARRAY_BUFFER, BufferName);

	// Reserve buffer memory but don't copy the values
    glBufferData(GL_ARRAY_BUFFER, PositionSize, NULL, GL_STATIC_DRAW);

	// Copy the vertex data in the buffer, in this sample for the whole range of data.
    glBufferSubData(GL_ARRAY_BUFFER, 0, PositionSize, &PositionData[0][0]);

	// Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return glf::checkError("sample::initArrayBuffer");
}
*/

// Buffer update using glMapBuffer
bool sample::initArrayBuffer()
{
	// Generate a buffer object
	glGenBuffers(1, &BufferName);

	// Bind the buffer for use
    glBindBuffer(GL_ARRAY_BUFFER, BufferName);

	// Reserve buffer memory but don't copy the values
    glBufferData(GL_ARRAY_BUFFER, PositionSize, 0, GL_STATIC_DRAW);

	// Copy the vertex data in the buffer, in this sample for the whole range of data.
    GLvoid* Data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(Data, PositionData, PositionSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return glf::checkError("sample::initArrayBuffer");
}

int main(int argc, char* argv[])
{
	glm::ivec2 ScreenSize = glm::ivec2(640, 480);

	sample * Sample = new sample(
		SAMPLE_NAME, 
		ScreenSize, 
		SAMPLE_MAJOR_VERSION,
		SAMPLE_MINOR_VERSION);

	if(Sample->check())
	{
		Sample->begin(ScreenSize);
		Sample->run();
		Sample->end();

		delete Sample;
		return 0;
	}

	delete Sample;
	Sample = 0;

	return 1;
}