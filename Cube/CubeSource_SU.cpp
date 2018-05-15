#include <Windows.h>
#include <cmath>
#include <glut.h>
#include <iostream>
#include <string>

#define M_PI 3.141592

using namespace std;

//base point. 8 points for cube
const GLfloat bp[8][3] = {
	{ 0, 1, 0 },
	{ 1, 1, 0 },
	{ 0, 1, 1 },
	{ 1, 1, 1 },
	{ 0, 0, 0 },
	{ 1, 0, 0 },
	{ 0, 0, 1 },
	{ 1, 0, 1 }
};
//base quard. for 6 plane
const GLint bq[6][4] = {
	{ 0, 1, 3, 2 },
	{ 2, 3, 7, 6 },
	{ 3, 1, 5, 7 },
	{ 0, 2, 6, 4 },
	{ 1, 0, 4, 5 },
	{ 6, 7, 5, 4 }
};
//color for plane
GLfloat red[3] = { 1.0f, 0.0f, 0.0f };
GLfloat green[3] = { 0.0f, 1.0f, 0.0f };
GLfloat blue[3] = { 0.0f, 0.0f, 1.0f };
GLfloat yellow[3] = { 1.0f, 1.0f, 0.0f };
GLfloat orange[3] = { 1.0f, 0.5f, 0.0f };
GLfloat white[3] = { 1.0f, 1.0f, 1.0f };
GLfloat black[3] = { 0.2f, 0.2f, 0.2f };

//planeをclickしたとき,どのsideにrotateができるか。 R : right, L : left 等, RSA : right side all
enum rotatableSide {RU, LU, RD, LD, UF, UB, DF, DB, RF, LF, RB, LB, RSA, USA, FSA};
//planeがrotateしたときどのsideがrotateしたか。RS : right side, LS : left side, AX : axis X 等
enum rotatedSide {RS, LS, US, DS, FS, BS, AX, AY, AZ};

class Cube
{
private:
	GLfloat p[8][3]; //point x, y, z
	bool R, U, F; //このcubeがどのsideにあるか。left : R == false
	GLfloat *color[6]; //planes color vector
	GLfloat upVector[3], rightVector[3]; //for finish test

	//内面を黒くするため、point設定のときに一回だけ呼び出す。
	void setColor()
	{
		for (int i = 0; i < 6; i++)
			color[i] = black;

		if (U)
			color[0] = yellow;
		else
			color[5] = white;

		if (F)
			color[1] = red;
		else
			color[4] = orange;

		if (R)
			color[2] = green;
		else
			color[3] = blue;
	}
	//use base quards
	void drawPlane(GLint i)
	{
		glColor3fv(color[i]);
		glBegin(GL_QUADS);
		glVertex3fv(p[bq[i][0]]);
		glVertex3fv(p[bq[i][1]]);
		glVertex3fv(p[bq[i][2]]);
		glVertex3fv(p[bq[i][3]]);
		glEnd();
	}
public:
	Cube() {}
	//cubeを移動
	void setPosition(GLfloat x, GLfloat y, GLfloat z)
	{
		for (int i = 0; i < 8; i++)
		{
			p[i][0] = bp[i][0] + x;
			p[i][1] = bp[i][1] + y;
			p[i][2] = bp[i][2] + z;
		}
		setRUF();
		setColor();
	}
	void drawCube()
	{
		for (int i = 0; i < 6; i++)
			drawPlane(i);
		//draw 輪郭線
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		for (int i = 0; i < 8; i += 2)
		{
			glVertex3fv(p[i]);
			glVertex3fv(p[i + 1]);
		}
		for (int i = 0; i < 4; i++)
		{
			glVertex3fv(p[i]);
			glVertex3fv(p[i + 4]);
		}
		for (int i = 0; i < 2; i++)
		{
			glVertex3fv(p[i]);
			glVertex3fv(p[i + 2]);
			glVertex3fv(p[i + 4]);
			glVertex3fv(p[i + 6]);
		}
		glEnd();
	}
	void rotate(GLfloat rot, GLfloat x, GLfloat y, GLfloat z)
	{
		//rotated point. use Quaternion. 小さい誤差が発生
		GLfloat rp[8][3];
		rot = rot * M_PI / 180;
		float s = cos(rot / 2);
		float a = sin(rot / 2) * x;
		float b = sin(rot / 2) * y;
		float c = sin(rot / 2) * z;

		for (int i = 0; i < 8; i++)
		{
			rp[i][0] = ((1 - (2 * b*b) - (2 * c*c)) * p[i][0]) + (((2 * a*b) - (2 * s*c))*p[i][1]) + (((2 * a*c) + (2 * s*b)) * p[i][2]);
			rp[i][1] = (((2 * a*b) + (2 * s*c)) * p[i][0]) + ((1 - (2 * a*a) - (2 * c*c)) * p[i][1]) + (((2 * b*c) - (2 * s*a)) * p[i][2]);
			rp[i][2] = (((2 * a*c) - (2 * s*b))*p[i][0]) + (((2 * b*c) + (2 * s*a)) * p[i][1]) + ((1 - (2 * a*a) - (2 * b*b)) *p[i][2]);
			p[i][0] = rp[i][0];
			p[i][1] = rp[i][1];
			p[i][2] = rp[i][2];
		}
		drawCube();
		setRUF();
	}
	//cubeの位置を知るため。cubeの中心点の座標で
	void setRUF()
	{
		R = (p[0][0] + p[7][0]) / 2 >= 0;
		U = (p[0][1] + p[7][1]) / 2 >= 0;
		F = (p[0][2] + p[7][2]) / 2 >= 0;
	}
	bool getR()
	{
		return R;
	}
	bool getU()
	{
		return U;
	}
	bool getF()
	{
		return F;
	}
	//誤差を修正して-1, 0, 1だけを持つ
	GLfloat *getUpVector()
	{
		upVector[0] = floor(p[0][0] - p[4][0] + 0.5);
		upVector[1] = floor(p[0][1] - p[4][1] + 0.5);
		upVector[2] = floor(p[0][2] - p[4][2] + 0.5);
		return upVector;
	}
	GLfloat *getRightVector()
	{
		rightVector[0] = floor(p[1][0] - p[0][0] + 0.5);
		rightVector[1] = floor(p[1][1] - p[0][1] + 0.5);
		rightVector[2] = floor(p[1][2] - p[0][2] + 0.5);
		return rightVector;
	}
};

bool mouseLeftDown;
double xPoint, yPoint; //mouse ckilck 座標
double xDrag, yDrag; //mouseを移動するときの座標
bool canRotate, hold1, hold2; //for rotate. one plane has two rotatable side. if side1 is rotating, side2 can't rotate
int frame; //using timer
float playTime_second = 0;
rotatableSide r;
rotatedSide rds; //RotateDSide
bool settingAnimation; //for control rotate animatain
bool playing; //if it is ture, counting playtime
GLint rotatedAngle; //for rotate only 90degree 

//1 / sqrt(3)
const double slope = 1 / sqrt(3);
char *str; //playtime conting text

Cube cube[8];

void U(GLint rot), D(GLint rot), R(GLint rot), L(GLint rot), F(GLint rot), B(GLint rot);
void X(GLint rot), Y(GLint rot), Z(GLint rot);
void settingCube(void(*rf)(GLint)); //for rotate animation. used in timer
void randomRotate(); //for shuffle cube
void printString(char *string, int m); //game start, finish text and playtime print
void start(); //when clicked
void finish(); //when put 'f' or 'F'

//base function for my OpenGL prigraming
void Draw_Axis();
void timer(int value);
void keyboard(unsigned char c, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void init();
void SetupRC();
void RenderScene();
void ChangeSize(int w, int h);

void U(GLint rot)
{
	for (int i = 0; i < 8; i++)
	{
		if (cube[i].getU())
			cube[i].rotate(rot, 0, 1, 0);
	}
	rotatedAngle += rot;
	rds = US;
}

void D(GLint rot)
{
	for (int i = 0; i < 8; i++)
	{
		if (!cube[i].getU())
			cube[i].rotate(rot, 0, -1, 0);
	}
	rotatedAngle += rot;
	rds = DS;
}

void R(GLint rot)
{
	for (int i = 0; i < 8; i++)
	{
		if (cube[i].getR())
			cube[i].rotate(rot, 1, 0, 0);
	}
	rotatedAngle += rot;
	rds = RS;
}

void L(GLint rot)
{
	for (int i = 0; i < 8; i++)
	{
		if (!cube[i].getR())
			cube[i].rotate(rot, -1, 0, 0);
	}
	rotatedAngle += rot;
	rds = LS;
}

void F(GLint rot)
{
	for (int i = 0; i < 8; i++)
	{
		if (cube[i].getF())
			cube[i].rotate(rot, 0, 0, 1);
	}
	rotatedAngle += rot;
	rds = FS;
}

void B(GLint rot)
{
	for (int i = 0; i < 8; i++)
	{
		if (!cube[i].getF())
			cube[i].rotate(rot, 0, 0, -1);
	}
	rotatedAngle += rot;
	rds = BS;
}

void X(GLint rot)
{
	for (int i = 0; i < 8; i++)
		cube[i].rotate(rot, 1, 0, 0);
	rotatedAngle += rot;
	rds = AX;
}

void Y(GLint rot)
{
	for (int i = 0; i < 8; i++)
		cube[i].rotate(rot, 0, 1, 0);
	rotatedAngle += rot;
	rds = AY;
}

void Z(GLint rot)
{
	for (int i = 0; i < 8; i++)
		cube[i].rotate(rot, 0, 0, 1);
	rotatedAngle += rot;
	rds = AZ;
}

void rotateSelect(bool b, void(*rf1)(GLint), void(*rf2)(GLint))
{
	if (b)
	{
		if (hold1)
		{
			if (rf1 == R || rf1 == B || rf2 == Z)
				rf1((int)(-(yDrag - yPoint)));
			else
				rf1((int)(yDrag - yPoint));
			hold2 = false;
		}
	}
	else
	{
		if (hold2)
		{
			if (rf2 == R || rf2 == D || rf2 == Y)
				rf2((int)(-(xDrag - xPoint)));
			else
				rf2((int)(xDrag - xPoint));
			hold1 = false;
		}
	}
}

void settingCube(void(*rf)(GLint))
{
	if (rotatedAngle > 0)
		rf(-1);
	else
		rf(1);
}

void randomRotate()
{
	int random = rand() % 6;
	switch (random)
	{
	case 0:
		U(90);
		break;
	case 1:
		D(90);
		break;
	case 2:
		R(90);
		break;
	case 3:
		L(90);
		break;
	case 4:
		F(90);
		break;
	case 5:
		B(90);
		break;
	}
}

void printString(char *string, int t)
{
	int len, i;
	std::string time = to_string(t);
	const char *cstr = time.c_str();
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(2, 5);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);

	len = (int)strlen(cstr);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cstr[i]);
}

void start()
{
	playing = true;
	canRotate = true;
	playTime_second = 0;
	str = "PlayTime : ";
}

void finish()
{
	canRotate = false;
	playing = false;
	str = "Finish : ";
}

void Draw_Axis()
{
	glBegin(GL_LINES);
	//x
	glColor3f(1.0f, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(20, 0, 0);
	//y
	glColor3f(0, 1.0f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 20, 0);
	//z
	glColor3f(0, 0, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 20);
	glEnd();
}

void timer(int value)
{
	frame++; //1 second 60 frame
	//dragするときdragの方向を知るためにたまたまxPoint, yPointを変える
	if (frame % 30 == 0)
	{
		xPoint = xDrag;
		yPoint = yDrag;
	}
	//counting playtime
	if (frame % 60 == 0 && playing)
		playTime_second++;

	//90degree rocate animation
	if (settingAnimation)
	{
		switch (rds)
		{
		case US:
			settingCube(U);
			break;
		case DS:
			settingCube(D);
			break;
		case RS:
			settingCube(R);
			break;
		case LS:
			settingCube(L);
			break;
		case FS:
			settingCube(F);
			break;
		case BS:
			settingCube(B);
			break;
		case AX:
			settingCube(X);
			break;
		case AY:
			settingCube(Y);
			break;
		case AZ:
			settingCube(Z);
			break;
		}
	}
	//stop rocate(animation)
	if (rotatedAngle == 0)
		settingAnimation = false;
		
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 1);
}

void keyboard(unsigned char c, int x, int y)
{
	switch (c)
	{
	case VK_ESCAPE:
		exit(0);
		break;
	//random shuffle
	case 'r':
	case 'R':
		playTime_second = 0;
		playing = false;

		for (int i = 0; i < 10; i++)
		{
			randomRotate();
		}
		break;
	//for finish game
	case 'f':
	case 'F':
		for (int i = 0; i < 7; i++)
		{
			if ((cube[i].getUpVector()[0] != cube[i + 1].getUpVector()[0])
				|| (cube[i].getUpVector()[1] != cube[i + 1].getUpVector()[1])
				|| (cube[i].getUpVector()[2] != cube[i + 1].getUpVector()[2]))
				break;
			if (i == 6) //all cubes has same upVetor
				if ((cube[0].getRightVector()[0] == cube[4].getRightVector()[0])
					&& (cube[0].getRightVector()[1] == cube[4].getRightVector()[1])
					&& (cube[0].getRightVector()[2] == cube[4].getRightVector()[2]))
					finish();
		}
		break;
	}
	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			//click to play
			if (!playing)
				start();
			mouseLeftDown = true;
			
			xPoint = (x * 20 / 600.0) - 10;
			yPoint = -1 * (y * 20 / 600.0) + 10;
			
			//どのsideのどんなplaneをclickしたか判別
			if (yPoint > slope * xPoint && yPoint > -slope * xPoint) //upSide
			{
				r = USA;
				if (yPoint < slope * xPoint + 2)
				{
					if (yPoint < -slope * xPoint + 2)
						r = RF; //8
					else if (yPoint < -slope * xPoint + 4)
						r = RB; //10
				}
				else if (yPoint < slope * xPoint + 4)
				{
					if (yPoint < -slope * xPoint + 2)
						r = LF; //9
					else if (yPoint < -slope * xPoint + 4)
						r = LB; //11
				}
			}
			else if ((yPoint < slope * xPoint) && (xPoint > 0)) //rightSide
			{
				r = RSA;
				if (xPoint < sqrt(3))
				{
					if (yPoint > slope * xPoint - 2)
						r = UF; //4
					else if (yPoint > slope * xPoint - 4)
						r = DF; //6
				}
				else if (xPoint < sqrt(3) * 2)
				{
					if (yPoint > slope * xPoint - 2)
						r = UB; //5
					else if (yPoint > slope * xPoint - 4)
						r = DB; //7
				}
			}
			else //fowardSide
			{
				r = FSA;
				if (yPoint > -slope * xPoint - 2)
				{
					if (xPoint > -sqrt(3))
						r = RU; //0
					else if (xPoint > -sqrt(3) * 2)
						r = LU; //1
				}
				else if (yPoint > -slope * xPoint - 4)
				{
					if (xPoint > -sqrt(3))
						r = RD; //2
					else if (xPoint > -sqrt(3) * 2)
						r = LD; //3
				}
			}
		}
		else if (state == GLUT_UP)
		{
			if (mouseLeftDown)
				mouseLeftDown = false;
			xPoint = yPoint = xDrag = yDrag = 0;
			hold1 = hold2 = true;

			//90度までの残り角度を計算して入れる。
			rotatedAngle %= 90;
			if (rotatedAngle > 30)
				rotatedAngle -= 90;
			else if (rotatedAngle < -30)
				rotatedAngle += 90;
			settingAnimation = true; //timerの中で90度までrotate	
		}
	}
	glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
	//dragしているときだけ、 rotate中じゃないときだきrotateさせる。
	if (mouseLeftDown && canRotate && !settingAnimation)
	{
		xDrag = (x * 20 / 600.0) - 10;
		yDrag = -1 * (y * 20 / 600.0) + 10;
		bool boolForFoward = ((yDrag - yPoint > (xDrag - xPoint) * slope && yDrag - yPoint > -(xDrag - xPoint) / slope)
			|| (yDrag - yPoint < (xDrag - xPoint) * slope && yDrag - yPoint < -(xDrag - xPoint) / slope));
		bool boolForRight = ((yDrag - yPoint < (xDrag - xPoint) / slope && yDrag - yPoint < -(xDrag - xPoint) * slope)
			|| (yDrag - yPoint > (xDrag - xPoint) / slope && yDrag - yPoint > -(xDrag - xPoint) * slope));
		bool boolForUp = ((xDrag - xPoint) > 0 && (yDrag - yPoint) < 0) || ((xDrag - xPoint) < 0 && (yDrag - yPoint) > 0);
		switch (r)
		{
		case RU:
			rotateSelect(boolForFoward, R, U);
			break;
		case LU:
			rotateSelect(boolForFoward, L, U);
			break;
		case RD:
			rotateSelect(boolForFoward, R, D);
			break;
		case LD:
			rotateSelect(boolForFoward, L, D);
			break;
		case UF:
			rotateSelect(boolForRight, F, U);
			break;
		case UB:
			rotateSelect(boolForRight, B, U);
			break;
		case DF:
			rotateSelect(boolForRight, F, D);
			break;
		case DB:
			rotateSelect(boolForRight, B, D);
			break;
		case RF:
			rotateSelect(boolForUp, F, R);
			break;
		case LF:
			rotateSelect(boolForUp, F, L);
			break;
		case RB:
			rotateSelect(boolForUp, B, R);
			break;
		case LB:
			rotateSelect(boolForUp, B, L);
			break;
		case RSA:
			rotateSelect(boolForRight, X, X);
			break;
		case USA:
			rotateSelect(boolForUp, Y, Y);
			break;
		case FSA:
			rotateSelect(boolForFoward, Z, Z);
			break;
		}
	}
	glutPostRedisplay();
}

void init()
{
	canRotate = true;
	hold1 = hold2 = true;
	rotatedAngle = 0;
	xPoint = yPoint = xDrag = yDrag = 0;
	settingAnimation = false;
	playing = false;
	str = "Click to play : ";

	cube[0].setPosition(0, 0, 0);
	cube[1].setPosition(-1, 0, 0);
	cube[2].setPosition(-1, 0, -1);
	cube[3].setPosition(0, 0, -1);
	cube[4].setPosition(0, -1, 0);
	cube[5].setPosition(-1, -1, 0);
	cube[6].setPosition(-1, -1, -1);
	cube[7].setPosition(0, -1, -1);
}

void SetupRC()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(1, 1, 1, 0, 0, 0, 0, 1, 0);

	//Draw_Axis();

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	for (int i = 0; i < 8; i++)
	{
		cube[i].drawCube();
	}

	printString(str, playTime_second);

	glutSwapBuffers();
}

void ChangeSize(int w, int h)
{
	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
		glOrtho(-4.0, 4.0, -4.0*(float)h / (float)w, 4.0*(float)h / (float)w, -20.0, 20.0);
	else
		glOrtho(-4.0*(float)w / (float)h, 4.0*(float)w / (float)h, -4.0, 4.0, -20.0, 20.0);
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Simple");

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	glutTimerFunc(1000 / 60, timer, 1);
	init();
	SetupRC();

	glutMainLoop();
}