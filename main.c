// vc80: disable deprecated functions warning
#pragma warning( disable: 4996 )

#include <stdio.h>					// sprintf, printf
#include <stdlib.h>					// atexit
#include <glut.h>					// GLUT

#define UNUSED(p) p

//-------------------------------------------------------------------------------------------------
// global variables
//-------------------------------------------------------------------------------------------------

// viewport
int				width=1, height=1;
// camera
int				button = -1;		// кнопочка мыши, которая нажата (-1 - нет нажатых, 0 - левая, 1 - средняя, 2 - правая)
float			angleX, angleY;		// текущий угол поворота сцены (см в правом нижнем углу)
float			mouseX, mouseY;		// нуно.
float			distZ = -10;		// расст по оси Z до нашей сцены

int				mode = 0, id = 2;
GLfloat			r = 1, g = 1, b = 1, a = 1; // цвет

// functions from main.c
float clamp(float value, float min, float max);
void draw_string(void *font, const char* sz, float x, float y);	// выводит дебаг текст по нужным координатам
// functions from png.c
unsigned char* read_png(const char* filename, int *width, int *height, int *components);	// читает данные из BMP картинки
void free_png(unsigned char* data);

//-------------------------------------------------------------------------------------------------
// message handlers
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------
// changeable

void on_paint(void)
{
	char sz[128] = { 0 };

	// set viewport
	glViewport(0, 0, width, height);

	// clear window
	glClearColor(0, 0.25, 0.25, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set camera position & settings
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width/height, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0, distZ);
	glRotatef(angleX, 0,1,0);
	glRotatef(angleY, 1,0,0);

	switch (mode)
	{
	case 0:	// premultiplied alpha
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case 1:	// alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case 2:	// additive blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}

	glEnable(GL_TEXTURE_2D);					// подключаем текстурирование
	glBindTexture(GL_TEXTURE_2D, id);			// устанавливаем id текстуры, кот. будет использоваться при текстурировании

	glBegin(GL_QUADS);							// рисуем квадратик, основное отличие - установка текстурных координат

		glColor4f(1, 1, 1, 1);
		glTexCoord2f(0, 0);	glVertex2f(-3, -3);
		glTexCoord2f(1,0);	glVertex2f( 1, -3);
		glTexCoord2f(1,1);	glVertex2f( 1,  1);
		glTexCoord2f(0,1);	glVertex2f(-3,  1);

		glColor4f(r, g, b, a);
		glTexCoord2f(0, 0);	glVertex2f(-1, -1);
		glTexCoord2f(1, 0);	glVertex2f( 3, -1);
		glTexCoord2f(1, 1);	glVertex2f( 3,  3);
		glTexCoord2f(0, 1);	glVertex2f(-1,  3);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);			// id == 0, означает что нет активной текстуры
	glDisable(GL_TEXTURE_2D);					// отключаем текстурирование

	glDisable(GL_BLEND);

	// camera for output debug text
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);	// для отображения текста, лучше использовать ортографическую проекцию
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1, 1, 0);					// сделаем текст желтеньким

	// debug text
	sprintf(sz, "R:%i, G:%i, B:%i, A:%i", (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
	draw_string(GLUT_BITMAP_8_BY_13, sz, 10, 10);
	switch (mode)
	{
	case 0: sprintf(sz, "Mode (SPACE): Premultiplied Alpha"); break;
	case 1: sprintf(sz, "Mode (SPACE): Alpha Blending"); break;
	case 2: sprintf(sz, "Mode (SPACE): Additive Blending"); break;
	case 3: sprintf(sz, "Mode (SPACE): No Blending"); break;
	}
	draw_string(GLUT_BITMAP_8_BY_13, sz, 10, 30);
	switch (id)
	{
	case 1: sprintf(sz, "Texture (ENTER): Normal"); break;
	case 2: sprintf(sz, "Texture (ENTER): Premultiplied"); break;
	}
	draw_string(GLUT_BITMAP_8_BY_13, sz, 10, 50);


	// end paint
	glFlush();
	// swap buffers
	glutSwapBuffers();
}

void on_keyboard(unsigned char key, int x, int y)
{
	UNUSED(x);
	UNUSED(y);

	switch (key)
	{
		case 27 : exit(0); break;							// escape - выход
		case ' ': mode++; if (mode > 3) mode = 0; break;
		case 13: id = (id == 1) ? 2 : 1; break;
		case 'q': r = clamp(r + 0.1f, 0, 1); break;
		case 'w': g = clamp(g + 0.1f, 0, 1); break;
		case 'e': b = clamp(b + 0.1f, 0, 1); break;
		case 'r': a = clamp(a + 0.1f, 0, 1); break;
		case 'a': r = clamp(r - 0.1f, 0, 1); break;
		case 's': g = clamp(g - 0.1f, 0, 1); break;
		case 'd': b = clamp(b - 0.1f, 0, 1); break;
		case 'f': a = clamp(a - 0.1f, 0, 1); break;
		case 't':
			r = clamp(r + 0.1f, 0, 1);
			g = clamp(g + 0.1f, 0, 1);
			b = clamp(b + 0.1f, 0, 1);
			a = clamp(a + 0.1f, 0, 1);
			break;
		case 'g':
			r = clamp(r - 0.1f, 0, 1);
			g = clamp(g - 0.1f, 0, 1);
			b = clamp(b - 0.1f, 0, 1);
			a = clamp(a + 0.1f, 0, 1);
			break;
	}
}

void on_exit(void)
{
	// удаляем текстуры из видеопамяти
	if (glIsTexture(1))
	{
		GLuint ids[] = { 1, 2 };
		glDeleteTextures(2, ids);
	}
}

//-------------------------------------------------------------------
// static

void on_motion(int x, int y)
{
	switch (button) {
		case 0:
			angleX += x - mouseX;
			angleY += y - mouseY;
			mouseX = x;
			mouseY = y;
			break;
		case 2:
			distZ += (y - mouseY) / 10.f;
			distZ = clamp(distZ, -100, 0);
			mouseY = y;
			break;
	}
}

void on_mouse(int _button, int state, int x, int y)
{
	if (state == 1) {		// 0 - нажали на кнопку, 1 - отпустили кнопку
		button = -1;		// никакая кнопка не нажата
		return;
	}
	switch (button = _button) {
		case 0: mouseX = x; mouseY = y; break;
		case 1: angleX = 0; angleY = 0; distZ = -15; break;
		case 2: mouseY = y; break;
	}
}

void on_size(int w, int h)
{
	width = w;
	height = h;
	if (height == 0) height = 1;		// на случай деления на 0
}

void on_timer(int value)
{
	UNUSED(value);
	on_paint();							// перерисуем экран
	glutTimerFunc(33, on_timer, 0);		// через 33мс вызовется эта функция
}

//-------------------------------------------------------------------------------------------------
// auxiliary
//-------------------------------------------------------------------------------------------------

float clamp(float value, float min, float max)
{
	if (value > max)
		value = max;
	if (value < min)
		value = min;
	return value;
}

void draw_string(void *font, const char* sz, float x, float y)
{
	if (!sz) return;										// если пихнули 0, вместо строки - вылетим
	glRasterPos2f(x,y);										// установка позиции текста
	while(*sz) { glutBitmapCharacter(font, *sz); sz++; }	// строка выводится посимвольно
}

//-------------------------------------------------------------------------------------------------
// entry point
//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	unsigned char*	data = 0;
	int		w = 0, h = 0, c = 0;

	atexit(on_exit);												// укажем, какая функция отвечает за очистку при выходе

	glutInit(&argc, argv);											// инициализация GLUT
    glutInitWindowSize(800, 600);									// установка размеров окна
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);		// настройка OpenGL контекста
    glutCreateWindow("Premultiplied Alpha");						// создание окна
	glutDisplayFunc(on_paint);										// указываем функцию отвечающую за перерисовку
    glutReshapeFunc(on_size);										// --//-- за изменение размеров окна
	glutKeyboardFunc(on_keyboard);									// --//-- за нажатие на клавишу
	glutMotionFunc(on_motion);										// --//-- за перемещение мыши с нажатой кнопкой
	glutMouseFunc(on_mouse);										// --//-- за нажатие на кнопку мыши

	//-------------------------------------------------------------------
	// !!! загрузка текстур !!!

	data = read_png("star.png", &w, &h, &c);		// читаем данные из файла
	if (data) {														// если получилось...

		int i = 0;

		glBindTexture(GL_TEXTURE_2D, 1);							// 1 - нормальная текстура
		gluBuild2DMipmaps(GL_TEXTURE_2D, c, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 2);							// 2 - премультиплайед текстура
		for (i = 0; i < w * h; ++i)
		{
			data[i * 4 + 0] = data[i * 4 + 0] * data[i * 4 + 3] / 255;
			data[i * 4 + 1] = data[i * 4 + 1] * data[i * 4 + 3] / 255;
			data[i * 4 + 2] = data[i * 4 + 2] * data[i * 4 + 3] / 255;
		}
		gluBuild2DMipmaps(GL_TEXTURE_2D, c, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		free_png(data);												// данные из системной памяти можно удалить
	}

	//-------------------------------------------------------------------

	glutTimerFunc(33, on_timer, 0);									// через 33мс вызовется эта функция
	glutMainLoop();													// цикл обработки сообщений

	return(0);
}