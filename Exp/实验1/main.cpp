#include<iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

// 画头
float pi = 3.1415926536;
int i = 0;
int n = 1200;

void  drawHead()
{
   
    // 描边
    glLineWidth(7.0f);
    glBegin(GL_LINE_STRIP);// 线
    for (i = 0; i < n; i++)
    {
       
        glColor3f(0,0,0);
        glVertex2f( 0.25*cos( 2*pi * i / n)-0.15, 0.25*sin(2*pi * i / n)+0.1);
    
    }
    glEnd();
    // 画头的两个部分
    glBegin(GL_POLYGON);//圆1
    glColor3f(1.0, 1.0, 0.4);
    
    for (i = 0; i < n; i++)
    {
        
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.25*cos( 2*pi * i / n)-0.15, 0.25*sin(2*pi * i / n)+0.1);
    }
    
    glEnd();
    
    glBegin(GL_POLYGON);//椭圆
    //glColor3f(1.0, 1.0, 0.4);
    
    for (i = 0; i < n; i++)
    {
       
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.35*cos( 2*pi * i / n), 0.2*sin(2*pi * i / n)+0.2);
    
    }
    glEnd();
    // 鼻子
    glLineWidth(4.0f);
    glBegin(GL_LINE_STRIP);// 线
    for (i = 0; i < n; i++)
    {
       
        glColor3f(0,0,0);
        glVertex2f( 0.06*cos( 2*pi * i / n)+0.27, 0.1*sin(2*pi * i / n)+0.2);
    }
    glEnd();
    // 鼻孔
    glBegin(GL_POLYGON);//圆
    for (i = 0; i < n; i++)
    {
       
        glColor3f(0,0,0);
        glVertex2f( 0.02*cos( 2*pi * i / n)+0.25, 0.02*sin(2*pi * i / n)+0.2);
    }
    glEnd();
    glBegin(GL_POLYGON);//圆
   
    
    for (i = 0; i < n; i++)
    {
       
        glColor3f(0,0,0);
        glVertex2f( 0.02*cos( 2*pi * i / n)+0.3, 0.02*sin(2*pi * i / n)+0.2);
     
    }
    glEnd();
    glLineWidth(4.0f);
    // 眼睛
    glBegin(GL_LINE_STRIP);// 线
    for (i = 0; i < n; i++)
    {
        glColor3f(0,0,0);
        glVertex2f( 0.05*cos( 2*pi * i / n), 0.05*sin(2*pi * i / n)+0.3);
     
    }
    glEnd();
    glLineWidth(4.0f);
    glBegin(GL_LINE_STRIP);// 线
    for (i = 0; i < n; i++)
    {
      
        glColor3f(0,0,0);
        glVertex2f( 0.05*cos( 2*pi * i / n)-0.1, 0.05*sin(2*pi * i / n)+0.25);
     
    }
    glEnd();
    
    glBegin(GL_POLYGON);//椭圆
  
    
    for (i = 0; i < n; i++)
    {
       
        glColor3f(0,0,0);
        glVertex2f( 0.02*cos( 2*pi * i / n)-0.02, 0.02*sin(2*pi * i / n)+0.3);
    }
    glEnd();
    glBegin(GL_POLYGON);//圆
   
    
    for (i = 0; i < n; i++)
    {
      
        glColor3f(0,0,0);
        glVertex2f( 0.02*cos( 2*pi * i / n)-0.1, 0.02*sin(2*pi * i / n)+0.25);
    
    }
    glEnd();
    
    
    
    glBegin(GL_POLYGON);//椭圆
   
    
    for (i = 0; i < n; i++)
    {
      
        glColor3f(1,0,0);
        glVertex2f( 0.1*cos( 2*pi * i / n)-0.1, 0.05*sin(0.3+2*pi * i / n));
   
    }
    glEnd();
    
    
    glBegin(GL_POLYGON);//椭圆
    //glColor3f(1.0, 1.0, 0.4);
    
    for (i = 0; i < 5; i++)
    {
       if(i==0)
       {
           glColor3f(1.0,0.0,0.0);
          // glVertex2f(-0.3, 0.15);
       }
        else if(i==1)
        {
            glColor3f(0.0,1.0,0.0);
            //glVertex2f(-0.4, 0.05);
        }
        else if(i==2){
            glColor3f(0.0,0.0,1.0);
           // glVertex2f(-0.2, 0.05);
        }
        else if(i==3)
        {
            glColor3f(0.0,0.0,1.0);
           // glVertex2f(-0.35, 0.02);
        }
        else if(i==4)
        {
            glColor3f(0.0,0.0,1.0);
           // glVertex2f(-0.25, 0.02);
        }
        glVertex2f( 0.05*cos( 2*pi * i / 5)-0.3, 0.05*sin(2*pi * i / 5)+0.1);
     
    }
    glEnd();
    glBegin(GL_POLYGON);//椭圆
    //glColor3f(1.0, 1.0, 0.4);
    
    for (i = 0; i < n; i++)
    {
       // if(i<400)
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.05*cos( 0.2+2*pi * i / n)-0.2, 0.1*sin(2*pi * i / n)+0.35);
     //   cout << (0.15*cos(2 * pi * i / n))<<" "<<(0.15*sin(2 * pi * i / n)+0.5)<<endl;
    }
    glEnd();
    
    glBegin(GL_POLYGON);//椭圆
    //glColor3f(1.0, 1.0, 0.4);
    
    for (i = 0; i < n; i++)
    {
       // if(i<400)
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.05*cos( 0.3+2*pi * i / n)-0.3, 0.1*sin(2*pi * i / n)+0.3);
     //   cout << (0.15*cos(2 * pi * i / n))<<" "<<(0.15*sin(2 * pi * i / n)+0.5)<<endl;
    }
    glEnd();
    
    
    
    
}


// 画身子
void drawbody()
{
   
    glBegin(GL_POLYGON);//椭圆
    
    for (i = 0; i < n; i++)
    {
      
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.28*cos(2*pi * i / n)-0.15, 0.33*sin(2*pi * i / n)-0.18);
    
    }
    glEnd();
    
}


// 画手
void drawHand()
{
    glBegin(GL_POLYGON);//椭圆
    //glColor3f(1.0, 1.0, 0.4);
    
    for (i = 0; i < n; i++)
    {
     
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.2*cos(2*pi * i / n)+0.15, 0.1*sin(-0.5+2*pi * i / n)-0.25);
 
    }
    glEnd();
    
    glBegin(GL_POLYGON);//椭圆
   
    
    for (i = 0; i < n; i++)
    {
        glColor3f(1,0.75,0.796);
        glVertex2f( 0.2*cos(2*pi * i / n)-0.45, 0.1*sin(-0.5+2*pi * i / n)-0.15);
     
    }
    glEnd();
}


// 画脚
void drawFoot()
{
    glBegin(GL_POLYGON);//三角形
    glColor3f(0,1,0);
    glVertex2f( -0.0, -0.42);
    glVertex2f( -0.11, -0.57);
    glVertex2f( 0.15, -0.57);
    
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(0,1,0);
    glVertex2f( -0.3, -0.42);
    glVertex2f( -0.43, -0.57);
    glVertex2f( -0.17, -0.57);
    glEnd();

}
// 装饰
void draw()
{
    glBegin(GL_POLYGON);//椭圆
    for (i = 0; i < 4; i++)
    {
       
        glColor3f(1.0,1.0,.0);
        glVertex2f( 0.1*cos(2*pi * i / 4)-0.1, 0.05*sin(2*pi * i / 4)-0.3);
   
    }
    glEnd();
}
void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


// 窗口初始化
int main(int argc, const char * argv[]) {
    GLFWwindow* win;
    if(!glfwInit()){
        return -1;
    }
    win = glfwCreateWindow(800, 800, "Pig", NULL, NULL);
    if(!win) // GLFW
    {
        glfwTerminate();
       // exit(EXIT_FAILURE);
    }
    if(!glewInit())  // GLEW
    {
        return -1;
    }
   
    glfwMakeContextCurrent(win);
    while(!glfwWindowShouldClose(win)){
        init();
        drawHand();
        drawFoot();
        drawbody();
        drawHead();
        draw();
        
       
        glfwSwapBuffers(win);
       
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
