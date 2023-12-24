////
////#include <stdlib.h>
////#include <stdio.h>
////#include <math.h>
////#include <time.h>
////#include <GLUT/glut.h>
////#include <math.h>
////void init(void) {
////    //背景设定为白色
////    glClearColor(1.0, 1.0, 1.0, 0.0);
//// 
////    //正投影
////    glMatrixMode(GL_PROJECTION);
////    gluOrtho2D(-250.0, 200.0, -250.0, 150.0);
////}
//// 
//void myDisplay1(void)
//{
//    //显示窗口
//    glClear(GL_COLOR_BUFFER_BIT);
// 
//    //设置颜色
//    glColor3f(1.0, 0.5, 0.4);
// 
//    //设置线的粗细
//    glLineWidth(4.0f);
// 
////#pragma region 折线
////    //画折线
////    glBegin(GL_LINE_STRIP);
////    glVertex2i(-20, -20);
////    glVertex2i(-82, -82);
////    glVertex2i(-92, -75);
////    glVertex2i(-30, -15);
////    glEnd();
////#pragma endregion
//// 
//#pragma region 同心圆
//    //画同心圆
//    float pi = 3.1415926536;
//    int i = 0;
//    int n = 1000;
////    glBegin(GL_POLYGON);//圆1
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(50 * cos(2 * pi * i / n), 50 * sin(2 * pi * i / n));
////    }
////    glEnd();
// 
//    glBegin(GL_POLYGON);//圆2
//    //glColor3f(1.0, 1.0, 1.0);
//    for (i = 0; i < n; i++)
//    {
//        glVertex2f(40 * cos(2 * pi * i / n), 40 * sin(2 * pi * i / n));
//    }
//    glEnd();
// 
////    glBegin(GL_POLYGON);//圆3
////    glColor3f(1.0, 0.5, 0.4);
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(30 * cos(2 * pi * i / n), 30 * sin(2 * pi * i / n));
////    }
////    glEnd();
//// 
////    glBegin(GL_POLYGON);//圆4
////    glColor3f(1.0, 1.0, 1.0);
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(20 * cos(2 * pi * i / n), 20 * sin(2 * pi * i / n));
////    }
////    glEnd();
//// 
////    glBegin(GL_POLYGON);//圆5
////    glColor3f(1.0, 0.5, 0.4);
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(10 * cos(2 * pi * i / n), 10 * sin(2 * pi * i / n));
////    }
////    glEnd();
// 
//#pragma endregion
//    
//    //保证前面的OpenGL命令立即执行（而不是让它们在缓冲区中等待）
//    glFlush();
// 
//}
//void myDisplay(void)
//{
//    //显示窗口
//    glClear(GL_COLOR_BUFFER_BIT);
// 
//    //设置颜色
//    //glColor3f(1.0, 0.5, 0.4);
// 
//    //设置线的粗细
//    glLineWidth(4.0f);
// 
////#pragma region 折线
////    //画折线
////    glBegin(GL_LINE_STRIP);
////    glVertex2i(-20, -20);
////    glVertex2i(-82, -82);
////    glVertex2i(-92, -75);
////    glVertex2i(-30, -15);
////    glEnd();
////#pragma endregion
////
//#pragma region 同心圆
//    //画同心圆
//    float pi = 3.1415926536;
//    int i = 0;
//    int n = 1000;
//    glBegin(GL_POLYGON);//圆1
//   
//    for (i = 0; i < n; i++)
//    {
//        glVertex2f(50 * cos(2 * pi * i / n), 50 * sin(2 * pi * i / n));
//    }
//    glEnd();
//    n=3;
//    glColor3f(1.0, 1.0, 0.4);
//    glBegin(GL_POLYGON);//圆2
//    //glColor3f(1.0, 1.0, 1.0);
//    for (i = 0; i < n; i++)
//    {
//        glVertex2f(100 * cos(2 * pi * i / n), 100 * sin(2 * pi * i / n));
//    }
//    glEnd();
////    n=3;
////    glBegin(GL_POLYGON);//圆3
////    glColor3f(1.0, 0.5, 0.4);
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(30 * cos(2 * pi * i / n), 30 * sin(2 * pi * i / n));
////    }
////    glEnd();
////
////    glBegin(GL_POLYGON);//圆4
////    glColor3f(1.0, 1.0, 1.0);
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(20 * cos(2 * pi * i / n), 20 * sin(2 * pi * i / n));
////    }
////    glEnd();
////
////    glBegin(GL_POLYGON);//圆5
////    glColor3f(1.0, 0.5, 0.4);
////    for (i = 0; i < n; i++)
////    {
////        glVertex2f(10 * cos(2 * pi * i / n), 10 * sin(2 * pi * i / n));
////    }
////    glEnd();
// 
//#pragma endregion
//    
//    //保证前面的OpenGL命令立即执行（而不是让它们在缓冲区中等待）
//    glFlush();
// 
//}
//////带命令行参数的main函数
////int main(int argc, char *argv[])
//// 
////{
////    glutInit(&argc, argv);
//// 
////    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
//// 
////    glutInitWindowPosition(100, 100);
//// 
////    glutInitWindowSize(400, 400);
//// 
////    glutCreateWindow("OpenGL画一个棒棒糖程序");
////    const GLubyte * OpenGLVersion = glGetString(GL_VERSION);
////       const GLubyte * gluVersion = gluGetString(GLU_VERSION);
////
////    printf("OOpenGL实现的版本号：%s\n", OpenGLVersion);
////    printf("OGLU工具库版本：%s\n", gluVersion);
////    init();
////    glutDisplayFunc(&myDisplay);
////   // init2();
////    //gluOrtho2D(-250.0, 200.0, -100, 150.0);
////   // glutDisplayFunc(&myDisplay1);
//// 
////    glutMainLoop();
//// 
////    return 0;
//// 
////}
