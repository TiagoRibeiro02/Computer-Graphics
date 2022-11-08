// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// GLM header file
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// shaders header file
#include <C:/OpenGL/ComputerGraphics-UBI-main/ComputerGraphics-UBI-main/OpenGLRoot/ogl-master/ogl-master/common/shader.hpp>
#include <iostream>

// Vertex array object (VAO)
GLuint VertexArrayID;

// Vertex buffer object (VBO)
GLuint vertexbuffer;
GLuint vertexbuffer2;

// color buffer object (CBO)
GLuint colorbuffer;
GLuint colorbuffer2;

// GLSL program from the shaders
GLuint programID;


GLint WindowWidth = 800;
GLint WindowHeight = 600;

//borders
const float borderxmax = 50;
const float borderxmin = -borderxmax;

const float borderymax = 40;
const float borderymin = -borderymax;

//speeds
float PADDLE_SPEED = 0.1f;
const float BALL_SPEED = 0.02f;


// Initial ball position and size
const int steps = 100;
float x = 0.0;
float y = 0.0;


float radius = 2.5f;

float xstep = BALL_SPEED;
float ystep = BALL_SPEED;

// Initial paddle position and size
float xpaddle = 0.0;
float ypaddle = 0.0;
const float size_paddle = 20;

// Brick size
const float size_brick = 10;
float xbrick = 0.0;
float ybrick = 0.0;

bool showcube1 = true;
bool showcube2 = true;
bool showcube3 = true;


//--------------------------------------------------------------------------------
void transferDataToGPUMemory(void)
{
    float xpos = 0.0f;
    float ypos = 0.0f;
    float prevx = xpos;
    float prevy = ypos;
    
    // VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("C:/shaders/movingHouse/SimpleVertexShader.vertexshader", "C:/shaders/movingHouse/SimpleFragmentShader.fragmentshader");

    
    // bricks
    static const GLfloat g_vertex_buffer_data[] = {
        0.0f,  0.0f,  0.0f,     0.0f, size_brick, 0.0f,           size_brick, size_brick, 0.0f,
        0.0f,  0.0f,  0.0f,     size_brick, size_brick, 0.0f,     size_brick, 0.0f, 0.0f,

        0.0f,  borderymin + 1.0f,  0.0f,     size_paddle, borderymin + 1.0f,  0.0f,     size_paddle, borderymin + 3.0f, 0.0f,
        0.0f,  borderymin + 1.0f,  0.0f,     size_paddle, borderymin + 3.0f, 0.0f,      0.0f,  borderymin + 3.0f, 0.0f,
    };

    // One color for each vertex
    static const GLfloat g_color_buffer_data[] = {
        0.0f,  0.0f,  1.0f,     0.0f,  0.0f,  1.0f,     0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,     0.0f,  0.0f,  1.0f,     0.0f,  0.0f,  1.0f,

        0.0f,  1.0f,  0.0f,     0.0f,  1.0f,  0.0f,     0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,     0.0f,  1.0f,  0.0f,     0.0f,  1.0f,  0.0f,
    };


    //------------------------------------------------------------------------
    

    // Ball
    static GLfloat g_vertex_buffer_data2[steps * 3];

    for (int i = -1; i <= steps; i++) {
        float newx = xpos + (radius * sin((3.1415926f * 2.f / steps) * i));
        float newy = ypos + (radius * cos((3.1415926f * 2.f / steps) * i));

        g_vertex_buffer_data2[i * 3] = prevx;
        g_vertex_buffer_data2[(i * 3) + 1] = prevy;
        g_vertex_buffer_data2[(i * 3) + 2] = 0.0f;

        prevx = newx;
        prevy = newy;

    }

    GLfloat g_color_buffer_data2[steps * 3];
    for (int i = -1; i < steps; i++)
    {

        g_color_buffer_data2[i * 3] = 1.0f;
        g_color_buffer_data2[(i * 3) + 1] = 0.0f;
        g_color_buffer_data2[(i * 3) + 2] = 0.0f;
    }

    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data2), g_vertex_buffer_data2, GL_STATIC_DRAW);

    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data2), g_color_buffer_data2, GL_STATIC_DRAW);

}


//--------------------------------------------------------------------------------
void cleanupDataFromGPU()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);


}

//--------------------------------------------------------------------------------
void draw(void)
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    // create transformations
    //glm::mat4 model = glm::mat4(1.0f);
    //glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = glm::ortho(borderxmin, borderxmax, borderymin, borderymax);

    // Our ModelViewProjection : multiplication of our 3 matrices
    //glm::mat4 mvp = projection * view * model;
    // Remember, matrix multiplication is the other way around

    // retrieve the matrix uniform locations
    unsigned int matrix = glGetUniformLocation(programID, "mvp");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);


    glm::mat4 trans;
    trans = glm::translate(glm::mat4(1.0), glm::vec3(x, y, 0.0f));
    unsigned int m = glGetUniformLocation(programID, "trans");
    glUniformMatrix4fv(m, 1, GL_FALSE, &trans[0][0]);


    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );


    //glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10);
    // Draw the triangle !
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLE_FAN, 0, steps); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------


void draw_brick(int muvx, int muvy, int clear)
{
    // Clear the screen
    if (clear == 1) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else {
        // Use our shader
        glUseProgram(programID);

        // create transformations
        //glm::mat4 model = glm::mat4(1.0f);
        //glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 mvp = glm::ortho(borderxmin, borderxmax, borderymin, borderymax);

        // Our ModelViewProjection : multiplication of our 3 matrices
        //glm::mat4 mvp = projection * view * model;
        // Remember, matrix multiplication is the other way around

        // retrieve the matrix uniform locations
        unsigned int matrix = glGetUniformLocation(programID, "mvp");
        glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);


        glm::mat4 trans;
        trans = glm::translate(glm::mat4(1.0), glm::vec3(xbrick + muvx, ybrick + muvy, 0.0f));
        unsigned int m = glGetUniformLocation(programID, "trans");
        glUniformMatrix4fv(m, 1, GL_FALSE, &trans[0][0]);


        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );


        //glEnable(GL_PROGRAM_POINT_SIZE);
        //glPointSize(10);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
        //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
}

void drawPaddle(void)
{
    // Clear the screen
    //glClear(GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    // create transformations
    //glm::mat4 model = glm::mat4(1.0f);
    //glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = glm::ortho(borderxmin, borderxmax, borderymin, borderymax);

    // Our ModelViewProjection : multiplication of our 3 matrices
    //glm::mat4 mvp = projection * view * model;
    // Remember, matrix multiplication is the other way around

    // retrieve the matrix uniform locations
    unsigned int matrix = glGetUniformLocation(programID, "mvp");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);


    glm::mat4 trans;
    trans = glm::translate(glm::mat4(1.0), glm::vec3(xpaddle, ypaddle, 0.0f));
    unsigned int m = glGetUniformLocation(programID, "trans");
    glUniformMatrix4fv(m, 1, GL_FALSE, &trans[0][0]);


    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );


    //glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 6, 6);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, steps); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 9); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------


int main(void)
{

    // bricks location
    float xbmin1 = -30;
    float xbmax1 = xbmin1 + size_brick;
    float ybmin1 = 10;
    float ybmax1 = ybmin1 + size_brick;

    float xbmin2 = 5;
    float xbmax2 = xbmin2 + size_brick;
    float ybmin2 = -10;
    float ybmax2 = ybmin2 + size_brick;

    float xbmin3 = -20;
    float xbmax3 = xbmin3 + size_brick;
    float ybmin3 = 20;
    float ybmax3 = ybmin3 + size_brick;
    
    // Initialise GLFW
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window
    window = glfwCreateWindow(WindowWidth, WindowHeight, "Bouncing Square in 2D ", NULL, NULL);

    // Create window context
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    glewInit();

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // White background
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // transfer my data (vertices, colors, and shaders) to GPU side
    transferDataToGPUMemory();

    // render scene for each frame
    do {
        
        // drawing callback
        draw();
        if(showcube1)
            draw_brick(xbmin1,ybmin1,0);
        if(showcube2)
            draw_brick(xbmin2,ybmin2,0);
        if (showcube3)
            draw_brick(xbmin3,ybmin3,0);
        drawPaddle();

        // Swap buffers
        glfwSwapBuffers(window);

        // looking for events
        glfwPollEvents();

        /*---------------------------------------------------------------------------------------------------------------------------------*/

        // BEGIN collision detection for a ball
        // reverse direction on left or right edge
        if (x + radius > borderxmax || x - radius < borderxmin)
            xstep = -xstep;

        // reverse direction on top or bottom edge
        if (y + radius > borderymax)
            ystep = -ystep;
        // update x- and y-coordinate for ball origin
        x += xstep;
        y += ystep;
        // END collision detection for a ball
        
        //bricks collisions
        if (showcube1)
            //colisões laterais
            if ((x + radius >= xbmin1 && x - radius <= xbmax1) && (y + radius >= ybmin1 && y - radius <= ybmax1))
                if ((y >= ybmin1 && y <= ybmax1) && (x - radius <= xbmax1 || x + radius >= xbmin1))
                    xstep = -xstep;

            //colisões horizontais
            if ((x + radius >= xbmin1 && x - radius <= xbmax1) && (y + radius >= ybmin1 && y - radius <= ybmax1))
                if ((x >= xbmin1 && x <= xbmax1) && (y - radius <= ybmax1 || y + radius >= ybmin1))
                    ystep = -ystep;

        if (showcube2)
            //colisões laterais
            if ((x + radius >= xbmin2 && x - radius <= xbmax2) && (y + radius >= ybmin2 && y - radius <= ybmax2))
                if ((y >= ybmin2 && y <= ybmax2) && (x - radius <= xbmax2 || x + radius >= xbmin2))
                    xstep = -xstep;
                    //showcube2 = !showcube2;

                //colisões horizontais
                if ((x + radius >= xbmin2 && x - radius <= xbmax2) && (y + radius >= ybmin2 && y - radius <= ybmax2))
                    if ((x >= xbmin2 && x <= xbmax2) && (y - radius <= ybmax2 || y + radius >= ybmin2))
                        ystep = -ystep;
                        //showcube2 = !showcube2;
            

        if (showcube3)
            //colisões laterais
            if ((x + radius >= xbmin3 && x - radius <= xbmax3) && (y + radius >= ybmin3 && y - radius <= ybmax3))
                if ((y >= ybmin3 && y <= ybmax3) && (x - radius <= xbmax3 || x + radius >= xbmin3))
                    xstep = -xstep;

            //colisões horizontais
            if ((x + radius >= xbmin3 && x - radius <= xbmax3) && (y + radius >= ybmin3 && y - radius <= ybmax3))
                if ((x >= xbmin3 && x <= xbmax3) && (y - radius <= ybmax3 || y + radius >= ybmin3))
                    ystep = -ystep;
        
        /*---------------------------------------------------------------------------------------------------------------------------------*/
        //paddle collisions and movement    
        if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS))
            xpaddle += PADDLE_SPEED;
            if (xpaddle + size_paddle >= borderxmax)
                xpaddle = borderxmax - size_paddle;

        if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
            xpaddle -= PADDLE_SPEED;
            if (xpaddle <= borderxmin)
                xpaddle = borderxmin;

        if ((x + radius >= xpaddle && x + radius <= xpaddle + size_paddle) && (y - radius <= borderymin + 3.0f))
            ystep = -ystep;

        if (y < borderymin)
            glfwTerminate();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);


    // Cleanup VAO, VBOs, and shaders from GPU
    cleanupDataFromGPU();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}