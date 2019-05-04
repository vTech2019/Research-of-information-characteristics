#include "OpenGL_device.h"

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}
void OpenGL_device::checkErrorShader(GLuint shader, const GLchar * text, GLuint status)
{
	GLint infoLength = 1;
	GLint messageLength = 0;
	glGetShaderiv(shader, status, &infoLength);
	if (infoLength == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
		GLchar* infoLog = (GLchar*)_malloca(infoLength * sizeof(GLchar));
		glGetShaderInfoLog(shader, infoLength, &messageLength, infoLog);
		printf("%s - %s\n", text, infoLog);
		_freea(infoLog);
	}
}
void OpenGL_device::programInfoLog(GLuint shader)
{
	GLint infoLength = 0;
	GLint lengthWriteInfo = 0;
	GLchar* infoLog;

	glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

	if (infoLength > 0)
	{
		GLchar* infoLog = (GLchar*)_malloca(infoLength * sizeof(GLchar));
		glGetProgramInfoLog(shader, infoLength, &lengthWriteInfo, infoLog);
		printf("Information - %s	\n", infoLog);
		_freea(infoLog);
	}
}

size_t OpenGL_device::push2DTexture(GLubyte4 * image, GLuint width, GLuint height) {
	textures.resize(textures.size() + 1);
	glGenTextures(1, &textures.back().x);
	textures.back().y = GL_TEXTURE_2D;
	glBindTexture(GL_TEXTURE_2D, textures.back().x);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return (size_t)& textures.back();
}

bool OpenGL_device::pushShader(GLuint typeShader, GLchar * code, size_t length)
{
	size_t i = 0;
	GLint infoLength;
	switch (typeShader) {
	case GL_VERTEX_SHADER: {
		vertexShaders.resize(vertexShaders.size() + 1);
		GLuint * ptrVertexShader = &vertexShaders.back();
		*ptrVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(*ptrVertexShader, 1, &code, NULL);
		glCompileShader(*ptrVertexShader);
		checkErrorShader(*ptrVertexShader, "GL_VERTEX_SHADER", GL_COMPILE_STATUS);
		break;
	}
	case GL_FRAGMENT_SHADER: {
		fragmentShaders.resize(fragmentShaders.size() + 1);
		GLuint * ptrFragmentShader = &fragmentShaders.back();
		*ptrFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(*ptrFragmentShader, 1, &code, NULL);
		glCompileShader(*ptrFragmentShader);
		checkErrorShader(*ptrFragmentShader, "GL_FRAGMENT_SHADER", GL_COMPILE_STATUS);
		break;
	}
	case GL_TESS_CONTROL_SHADER:
		break;
	case GL_TESS_EVALUATION_SHADER:
		break;
	case GL_GEOMETRY_SHADER:
		break;
	case GL_COMPUTE_SHADER: {
		computeShaders.resize(computeShaders.size() + 1);
		GLuint * ptrComputeShader = &computeShaders.back();
		*ptrComputeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(*ptrComputeShader, 1, &code, NULL);
		glCompileShader(*ptrComputeShader);
		checkErrorShader(*ptrComputeShader, "GL_COMPUTE_SHADER", GL_COMPILE_STATUS);
	}
	default: {
		break;
	}
	}
	return true;
}

size_t OpenGL_device::pushProgram()
{
	programs.resize(programs.size() + 1);
	programs.back() = glCreateProgram();
	for (size_t i = 0; i < fragmentShaders.size(); i++)
		glAttachShader(programs.back(), fragmentShaders[i]);
	for (size_t i = 0; i < vertexShaders.size(); i++)
		glAttachShader(programs.back(), vertexShaders[i]);
	for (size_t i = 0; i < computeShaders.size(); i++)
		glAttachShader(programs.back(), computeShaders[i]);
	glLinkProgram(programs.back());
	programInfoLog(programs.back());
	for (size_t i = 0; i < fragmentShaders.size(); i++)
		glDeleteShader(fragmentShaders[i]);
	for (size_t i = 0; i < vertexShaders.size(); i++)
		glDeleteShader(vertexShaders[i]);
	for (size_t i = 0; i < computeShaders.size(); i++)
		glDeleteShader(computeShaders[i]);

	GLint count;
	GLint number_active_attributes = 0;
	GLint number_active_uniforms = 0;
	GLint max_attributes_name_length = 0;
	GLint max_uniform_name_length = 0;
	glGetProgramiv(programs.back(), GL_ACTIVE_ATTRIBUTES, &number_active_attributes);
	glGetProgramiv(programs.back(), GL_ACTIVE_UNIFORMS, &number_active_uniforms);
	glGetProgramiv(programs.back(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attributes_name_length);
	glGetProgramiv(programs.back(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);
	std::vector<GLchar> nameAttributeData(max_attributes_name_length);
	std::vector<GLchar> nameUniformData(max_uniform_name_length);
	for (GLint i = 0; i < number_active_uniforms; ++i)
	{
		GLint array_size = 0;
		GLenum type = 0;
		GLsizei actual_length = 0;
		glGetActiveUniform(programs.back(), i, nameAttributeData.size(), &actual_length, &array_size, &type, &nameAttributeData[0]);
		std::string name((GLchar*)& nameAttributeData[0], actual_length);
		printf("%s\n", name.data());
	}
	for (GLint i = 0; i < number_active_attributes; ++i)
	{
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;
		glGetActiveAttrib(programs.back(), i, nameUniformData.size(), &actualLength, &arraySize, &type, &nameUniformData[0]);
		std::string name((GLchar*)& nameUniformData[0], actualLength);
		printf("%s\n", name.data());
	}
	vertex_index.push_back(glGetAttribLocation(programs.back(), "position"));
	color_index.push_back(glGetAttribLocation(programs.back(), "color"));
	projection_matrix_index.push_back(glGetUniformLocation(programs.back(), "projectionMatrix"));
	model_matrix_index.push_back(glGetUniformLocation(programs.back(), "modelMatrix"));
	view_matrix_index.push_back(glGetUniformLocation(programs.back(), "viewMatrix"));
	return (size_t)& programs.back();
}

std::vector<GLuint>::iterator OpenGL_device::pushBuffer(void* data, size_t number_objects, size_t length_object, size_t typeBuffer)
{
	switch (typeBuffer) {
	case GL_VECTOR_BUFFER:
		this->number_objects.push_back(number_objects);
		vector_buffer.resize(vector_buffer.size() + 1);
		glGenBuffers(1, &vector_buffer.back());
		glBindBuffer(GL_ARRAY_BUFFER, vector_buffer.back());
		glBufferData(GL_ARRAY_BUFFER, number_objects * length_object, data, GL_STATIC_DRAW);
		return vector_buffer.begin() + vector_buffer.size() - 1;
		break;
	case GL_COLOR_BUFFER:

		color_buffer.resize(color_buffer.size() + 1);
		glGenBuffers(1, &color_buffer.back());
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer.back());
		glBufferData(GL_ARRAY_BUFFER, number_objects * length_object, data, GL_STATIC_DRAW);
		return color_buffer.begin() + color_buffer.size() - 1;
		break;
	case GL_INDEX_BUFFER:

		index_buffer.resize(index_buffer.size() + 1);
		glGenBuffers(1, &index_buffer.back());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.back());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_objects * length_object, data, GL_STATIC_DRAW);
		return index_buffer.begin() + index_buffer.size() - 1;
		break;
	}
	return std::vector<GLuint>::iterator();
}
void OpenGL_device::setMouseOldPosition(uint2 data) {
	old_mouse_position = data;
}

void OpenGL_device::setMouseNewPosition(uint2 data) {
	new_mouse_position = data;
}
mat4x4* OpenGL_device::getModelMatrix() {
	return &modelMatrix;
}

mat4x4* OpenGL_device::getViewMatrix() {
	return &viewMatrix;
}
mat4x4* OpenGL_device::getProjectionMatrix() {
	return &projectionMatrix;
}
float4* OpenGL_device::getUpView() {
	return &Up;
}
float4* OpenGL_device::getCenterView() {
	return &Center;
}
float4* OpenGL_device::getEyeView() {
	return &Eye;
}

void OpenGL_device::cameraRotate()
{
	lookAt(Eye, Up, Center);
}
GLfloat t_1 = 0.0f;
GLfloat t_2 = 0.0f;
void OpenGL_device::Render() {

	getEyeView()->x = sin(t_1 += 0.01f) * 5;
	getEyeView()->z = cos(t_2 += 0.01f) * 5;
	lookAt(Eye, Up, Center);
	//modelMatrix = rotate(0.001, float4(0, 0, 1, 0));
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	for (size_t i = 0; i < programs.size(); i++) {
		if (last_program != programs[i])
			last_program = programs[i],
			glUseProgram(programs[i]);

		glUniformMatrix4fv(projection_matrix_index[i], 1, GL_FALSE, (GLfloat*)& projectionMatrix);
		glUniformMatrix4fv(view_matrix_index[i], 1, GL_FALSE, (GLfloat*)& viewMatrix);
		glUniformMatrix4fv(model_matrix_index[i], 1, GL_FALSE, (GLfloat*)& modelMatrix);

		for (size_t j = 0; j < vector_buffer.size(); j++) {
			glBindBuffer(GL_ARRAY_BUFFER, vector_buffer[j]);
			glEnableVertexAttribArray(vertex_index[i]);
			glVertexAttribPointer(vertex_index[i], 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, color_buffer[j]);
			glEnableVertexAttribArray(color_index[i]);
			glVertexAttribPointer(color_index[i], 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[j]);
			glDrawElements(GL_POINTS, number_objects[j], GL_UNSIGNED_INT, NULL);
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
}
OpenGL_device::OpenGL_device(HDC hdc)
{

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (nPixelFormat == 0) {
		MessageBox(NULL, std::to_string(GetLastError()).c_str(), "ChoosePixelFormat error ", MB_OK);
		return;
	}
	if (!SetPixelFormat(hdc, nPixelFormat, &pfd)) {
		MessageBox(NULL, std::to_string(GetLastError()).c_str(), "SetPixelFormat error ", MB_OK);
		return;
	}
	if (!(gl_context = wglCreateContext(hdc))) {
		MessageBox(NULL, std::to_string(GetLastError()).c_str(), "Creating temporary render context fail ", MB_OK);
		return;
	}
	if (!wglMakeCurrent(hdc, gl_context)) {
		MessageBox(NULL, "Error", "Error wglMakeCurrent!", MB_OK);
		wglDeleteContext(gl_context);
		return;
	}
	if (glewInit() != GLEW_OK) {
		MessageBox(NULL, "Error", "Error init glew!", MB_OK);
		wglDeleteContext(gl_context);
		return;
	}
	glewExperimental = GL_TRUE;
	glPointSize(3.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE);
	glDepthFunc(GL_LESS);
	GLint NumberOfExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
	for (GLint i = 0; i < NumberOfExtensions; i++) {
		const GLubyte* information = glGetStringi(GL_EXTENSIONS, i);
		printf("%s\n", information);
		if (strcmp((const char*)information, "GL_ARB_debug_output") == 0) {
			glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
			//glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageCallbackARB(MessageCallback, 0);
		}
	}
}

OpenGL_device::OpenGL_device()
{
}

OpenGL_device::~OpenGL_device()
{
	glDeleteBuffers(vector_buffer.size(), vector_buffer.data());
	glDeleteBuffers(color_buffer.size(), color_buffer.data());
	glDeleteBuffers(index_buffer.size(), index_buffer.data());
}

mat4x4 OpenGL_device::rotate(GLfloat angle, float4 vector) {
	GLfloat cosine = cosf(angle);
	GLfloat sine = sinf(angle);
	vector = normalize_vec3(vector);
	float4 temp = vector * (1.0f - cosine);
	float4 Rotate[3];
	mat4x4 Result = viewMatrix;
	Rotate[0] = vector * temp.x + float4(cosine, sine * vector.z, -sine * vector.y, 0.0f);
	Rotate[1] = vector * temp.y + float4(-sine * vector.z, cosine, sine * vector.x, 0.0f);
	Rotate[2] = vector * temp.z + float4(sine * vector.y, -sine * vector.x, cosine, 0.0f);
	viewMatrix.x = Result.x * Rotate[0].x + Result.y * Rotate[0].y + Result.z * Rotate[0].z;
	viewMatrix.y = Result.x * Rotate[1].x + Result.y * Rotate[1].y + Result.z * Rotate[1].z;
	viewMatrix.z = Result.x * Rotate[2].x + Result.y * Rotate[2].y + Result.z * Rotate[2].z;
	return viewMatrix;
}
void OpenGL_device::lookAt(float4 Eye, float4 Up, float4 Center) {

	Up = cross_vec3(normalize_vec3(cross_vec3(Up, normalize_vec3(Eye))), normalize_vec3(Eye));

	mat4x4 translation;
	translation.x.w = -Eye.x;
	translation.y.w = -Eye.y;
	translation.z.w = -Eye.z;
	float4 z = normalize_vec3(Eye - Center);
	float4 x = normalize_vec3(cross_vec3(normalize_vec3(Up), z));
	float4 y = normalize_vec3(cross_vec3(z, x));
	modelMatrix.x = { x.x, y.x, z.x, 0.0f };
	modelMatrix.y = { x.y, y.y, z.y, 0.0f };
	modelMatrix.z = { x.z, y.z, z.z, 0.0f };
	modelMatrix.w = { 0.0f, 0.0f, 0.0f, 1.0f };
	modelMatrix = modelMatrix * translation;
}
void OpenGL_device::genProjection(GLfloat width, GLfloat height, GLfloat z_near, GLfloat z_far, GLfloat FOV) {
	GLfloat aspect = width / height;
	GLfloat depth = z_far - z_near;
	GLfloat tanFOV = tanf(FOV * 0.5f);
	projectionMatrix.x.x = 1.0f / (aspect * tanFOV);
	projectionMatrix.y.y = 1.0f / tanFOV;
	projectionMatrix.z.z = -(z_far + z_near) / depth;
	projectionMatrix.z.w = 2.0f * (z_far * z_near) / depth;
	projectionMatrix.w.z = -1.0f;
}