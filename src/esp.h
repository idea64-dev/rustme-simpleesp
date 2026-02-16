#ifndef ESP_H_
#define ESP_H_

#include <Windows.h>
#include <list>

#include <gl/GL.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "hook/PGHooker.hpp"

extern void *original_glOrtho_addr;
extern void *original_glScalef_addr;
extern void *original_glTranslatef_addr;

using glOrtho_t = void(WINAPI *)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
using glScalef_t = void(WINAPI *)(GLfloat, GLfloat, GLfloat);
using glTranslatef_t = void(WINAPI *)(GLfloat, GLfloat, GLfloat);

struct draw_object_t
{
	glm::mat4 m_projection;
	glm::mat4 m_modelview;

	draw_object_t()
	{
		glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(m_projection));
		glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(m_modelview));
	}
};

extern std::list<draw_object_t> objects;
extern bool enabled;
namespace esp
{
	void install();

	void draw_box();
	void WINAPI hk_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	void WINAPI hk_glScalef(GLfloat x, GLfloat y, GLfloat z);
	void WINAPI hk_glTranslatef(GLfloat x, GLfloat y, GLfloat z);
}

#endif