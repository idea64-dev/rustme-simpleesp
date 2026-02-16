#include "esp.h"
#include "gui.h"

static bool world_to_screen(
	const glm::vec3 &pos,
	const glm::mat4 &view,
	const glm::mat4 &proj,
	int width,
	int height,
	glm::vec2 &out)
{
	glm::vec4 clip =
		proj * view * glm::vec4(pos, 1.0f);

	if (clip.w < 0.1f)
		return false;

	glm::vec3 ndc =
		glm::vec3(clip) / clip.w;

	out.x = (ndc.x + 1.0f) * 0.5f * width;
	out.y = (1.0f - ndc.y) * 0.5f * height;

	return true;
}

static void draw_tracer(
	float x,
	float y,
	float sx,
	float sy)
{
	glLineWidth(1.5f);

	glBegin(GL_LINES);

	glVertex2f(x, y);
	glVertex2f(sx, sy);

	glEnd();
}

static glm::vec4 color = glm::vec4(0.18f, 1.f, 0.52f, 1.f);

void *original_glOrtho_addr = nullptr;
void *original_glScalef_addr = nullptr;
void *original_glTranslatef_addr = nullptr;

bool enabled = false;

std::list<draw_object_t> objects;

void esp::draw_box()
{
	glColor4fv(glm::value_ptr(color));

	glBegin(GL_LINES);
	{
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);

		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);

		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);

		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);

		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);

		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
	}

	glEnd();
}

void WINAPI esp::hk_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	PGHooker::DisableHookForOnce(original_glOrtho_addr);
	((glOrtho_t)original_glOrtho_addr)(left, right, bottom, top, zNear, zFar);
	if (!gui::state.esp) return;
	{
		::std::printf("called\n");
		if (objects.empty())
		{
			::std::printf("object empty\n");
			return;
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		int width = viewport[2];
		int height = viewport[3];

		for (auto &object : objects)
		{
			glm::mat4 &modelview = object.m_modelview;
			glm::mat4 &projection = object.m_projection;

			glm::vec3 translate = glm::vec3(0.0f, -1.0f, 0.0f);
			glm::vec3 scale = glm::vec3(0.5f, 1.0f, 0.5f);

			modelview = glm::translate(modelview, translate);
			modelview = glm::scale(modelview, scale);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(glm::value_ptr(projection));

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(glm::value_ptr(modelview));

			::std::printf("draw_box\n");
			draw_box();
		}

		objects.clear();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glPopAttrib();
	} 
}

void WINAPI esp::hk_glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	if (x == 0.9375f and y == 0.9375f and z == 0.9375f)
	{
		objects.emplace_back();
		::std::printf("emplaced\n");
	}

	PGHooker::DisableHookForOnce(original_glScalef_addr);
	((glScalef_t)original_glScalef_addr)(x, y, z);
}

DWORD WINAPI rehook(LPVOID)
{
	while (true)
	{
		Sleep(100);

		PGHooker::RemoveHook(original_glOrtho_addr);
		PGHooker::RemoveHook(original_glScalef_addr);

		//Sleep(25);

		PGHooker::CreateHook(original_glOrtho_addr, esp::hk_glOrtho);
		PGHooker::CreateHook(original_glScalef_addr, esp::hk_glScalef);
	}

	return 0;
}

void esp::install()
{	
	original_glOrtho_addr = (void *)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glOrtho");
	original_glScalef_addr = (void *)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glScalef");

	PGHooker::CreateHook(original_glOrtho_addr, hk_glOrtho);
	PGHooker::CreateHook(original_glScalef_addr, hk_glScalef);

	CreateThread(
		nullptr,
		0,
		rehook,
		nullptr,
		0,
		nullptr
	);
}