#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <vector>


#include "shader.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"
#include "text.h"

using namespace irrklang;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
void SendLightDate(const Shader &shader);
void playSound(int i);

int treasure = 0, treasure_sound = true,
warn = false, warn_sound = true;


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -0.1f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// craft
glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 0.0f);
//开关
glm::vec4 dirlight_switch = glm::vec4(1.0f), pointlight_switch = glm::vec4(1.0f), spotlight_switch = glm::vec4(0.0f);
// positions of the craft
glm::vec3 craftPositions[] = {
    glm::vec3(2.0f,  0.0f,  -20.0f),
    glm::vec3(0.0f, 0.0f, -30.0f),
    glm::vec3(3.0f,  0.0f, -40.0f),
    glm::vec3(-2.0f,  0.0f, -50.0f),
    glm::vec3(-15.0f,  0.0f, -60.0f),
    glm::vec3(2.0f,  0.0f,  -70.0f),
    glm::vec3(0.0f, 0.0f, -80.0f),
    glm::vec3(8.0f,  0.0f, -90.0f),
    glm::vec3(-2.0f,  0.0f, -100.0f),
    glm::vec3(-15.0f,  0.0f, -110.0f),
    glm::vec3(12.0f,  0.0f,  -120.0f),
    glm::vec3(0.0f, 0.0f, -130.0f),
    glm::vec3(8.0f,  0.0f, -140.0f),
    glm::vec3(17.0f,  0.0f, -150.0f),
    glm::vec3(-15.0f,  0.0f, -160.0f),
    glm::vec3(2.0f,  0.0f,  -170.0f),
    glm::vec3(0.0f, 0.0f, -180.0f),
    glm::vec3(3.0f,  0.0f, -190.0f),
    glm::vec3(7.0f,  0.0f, -200.0f),
    glm::vec3(-5.0f,  0.0f, -210.0f)
};
//craft的发光颜色
glm::vec3 craftColor[] = {
    glm::vec3(1.0f,  0.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  1.0f, 1.0f),
    glm::vec3(0.0f,  1.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  0.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  1.0f, 1.0f),
    glm::vec3(0.0f,  1.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  0.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  1.0f, 1.0f),
    glm::vec3(0.0f,  1.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  0.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f),
    glm::vec3(1.0f,  1.0f, 1.0f),
    glm::vec3(0.0f,  1.0f, 1.0f),
    glm::vec3(1.0f,  1.0f, 0.0f)
};
//左右移动状态
int state[] = { 1,-1,1,-1,1,1,-1,1,-1,1, 1,-1,1,-1,1, 1,-1,1,-1,1 };
//警戒状态
bool wring[20] = { false }, wring_sound[20];
//难度
int speedNumber = 0;
float craftSpeed[6] = { 1.0f,2.5f,3.5f,4.5f,6.0f ,8.0f };
int point_powerlevel = 4, spot_powerlevel = 0;

//rock
bool collect[20] = { false }, collect_sound[20];

glm::vec3 rockPositions[] = {
    glm::vec3(2.0f,  0.0f,  -220.0f),
    glm::vec3(0.0f, 0.0f, -230.0f),
    glm::vec3(3.0f,  0.0f, -240.0f),
    glm::vec3(-2.0f,  0.0f, -250.0f),
    glm::vec3(-15.0f,  0.0f, -260.0f),
    glm::vec3(2.0f,  0.0f,  -270.0f),
    glm::vec3(0.0f, 0.0f, -280.0f),
    glm::vec3(8.0f,  0.0f, -290.0f),
    glm::vec3(-2.0f,  0.0f, -300.0f),
    glm::vec3(-15.0f,  0.0f, -310.0f),
    glm::vec3(12.0f,  0.0f,  -320.0f),
    glm::vec3(0.0f, 0.0f, -330.0f),
    glm::vec3(8.0f,  0.0f, -340.0f),
    glm::vec3(17.0f,  0.0f, -350.0f),
    glm::vec3(-15.0f,  0.0f, -360.0f),
    glm::vec3(2.0f,  0.0f,  -370.0f),
    glm::vec3(0.0f, 0.0f, -380.0f),
    glm::vec3(3.0f,  0.0f, -390.0f),
    glm::vec3(7.0f,  0.0f, -400.0f),
    glm::vec3(-5.0f,  0.0f, -410.0f)
};

// start the sound engine with default parameters
ISoundEngine* SoundEngine = createIrrKlangDevice();

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    // 设置剔除背面
    //glCullFace(GL_BACK);
    // 设置逆时针定义的为正面
    //glFrontFace(GL_CCW);

    //启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!SoundEngine)
        return 0; // error starting up the engine

    SoundEngine->play2D("resources/media/sound/background.mp3", GL_TRUE);

    //freetype
    /*Shader tshader("resources/glsl/freetype/type_vs.glsl", "resources/glsl/freetype/type_fs.glsl");
    TextRenderer text(SCR_WIDTH, SCR_HEIGHT, tshader);
    text.Load("resources/fonts/arial.ttf", 48);*/


    // build and compile shaders
    // -------------------------
    Shader shader("resources/glsl/obj/obj_vs.glsl", "resources/glsl/obj/obj_fs.glsl");
    //Shader shadowShader("resources/glsl/shadow/depth_vs.glsl", "resources/glsl/shadow/depth_fs.glsl");
    Shader skyboxShader("resources/glsl/skybox/skybox_vs.glsl", "resources/glsl/skybox/skybox_fs.glsl");

    Model street_light("resources/model/Street Lamp/StreetLamp1.obj");
    Model spacecraft("resources/model/spacecraft/spacecraft.obj");
    Model goldspacecraft("resources/model/spacecraft/goldspacecraft.obj");
    Model craft("resources/model/craft/craft.obj");
    Model wringcraft("resources/model/craft/wringcraft.obj");
    Model planet("resources/model/planet/planet.obj");
    Model rock("resources/model/rock/rock.obj");
    Model goldrock("resources/model/rock/goldrock.obj");
    Model moon("resources/model/moon/moon.obj");

    //五个行星带
    unsigned int amount = 100;
    glm::mat4* modelMatrices[5];
    float radius[5] = { 130.0f,135.0f,140.0f,145.0f,150.f };
    srand(glfwGetTime()); // 初始化随机种子    
    for (int j = 0; j < 5; j++)
    {
        modelMatrices[j] = new glm::mat4[amount];
        float offset = 5.0f;
        for (unsigned int i = 0; i < amount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -500.0f));
            // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
            float angle = (float)i / (float)amount * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius[j] + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = 7.0f + displacement * 0.6f; // 让行星带的高度比x和z的宽度要小
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius[j] + displacement;
            model = glm::translate(model, glm::vec3(x, y, z));

            // 2. 缩放：在 0.05 和 0.25f 之间缩放
            float scale = (rand() % 20) / 100.0f + 0.8f;
            model = glm::scale(model, glm::vec3(scale));

            // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
            float rotAngle = (rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            // 4. 添加到矩阵的数组中
            modelMatrices[j][i] = model;
        }
    }
    

    //声音只播放一次
    for (int i = 0; i < 20; i++)
    {
        collect_sound[i] = true;
        wring_sound[i] = true;
    }

    float skyboxVertices[] = {
        // positions          
        -10.0f,  10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,

        -10.0f, -10.0f,  10.0f,
        -10.0f, -10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f,  10.0f,
        -10.0f, -10.0f,  10.0f,

         10.0f, -10.0f, -10.0f,
         10.0f, -10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,

        -10.0f, -10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f, -10.0f,  10.0f,
        -10.0f, -10.0f,  10.0f,

        -10.0f,  10.0f, -10.0f,
         10.0f,  10.0f, -10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,
        -10.0f,  10.0f, -10.0f,

        -10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f,  10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f,  10.0f,
         10.0f, -10.0f,  10.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------
    vector<std::string> faces
    {
        "resources/textures/skybox/right.bmp",
        "resources/textures/skybox/left.bmp",
        "resources/textures/skybox/top.bmp",
        "resources/textures/skybox/bottom.bmp",
        "resources/textures/skybox/front.bmp",
        "resources/textures/skybox/back.bmp",
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // draw scene as normal
        shader.use();

        SendLightDate(shader);

        //mvp
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        
        //draw spacecraft
        //跟着视角旋转
        glm::vec3 dis = camera.Front * 8.0f + camera.Position ;
        model[3][0] = dis.x;
        model[3][1] = -4.0f;
        model[3][2] = dis.z;

        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::rotate(model, -glm::radians(camera.Yaw+90.0f), glm::vec3(0.0, 1.0, 0.0));

        shader.setMat4("model", model);
        shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setVec3("material.diffuse", glm::vec3(1.8f, 1.8f, 1.8f));
        shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("material.shininess", 32.0f);
        
        if (treasure == 13 && treasure_sound)
        {
            SoundEngine->play2D("resources/media/sound/bell.wav", GL_FALSE);
            treasure_sound = false;
        }

        if (treasure == 13)
        {

            goldspacecraft.Draw(shader);
        }
        else spacecraft.Draw(shader);


        //planet
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -500.0f));
        model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setFloat("material.shininess", 32.0f);
        planet.Draw(shader);

        //moon
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 20.0f, -420.0f));
        model = glm::translate(model, glm::vec3(150.0f * sin(glfwGetTime()/2), 0.0f, 150.0f * cos(glfwGetTime()/2)));
        model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
        //model = glm::rotate(model, glm::radians((float)glfwGetTime() * 200.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
        moon.Draw(shader);

        // draw meteorites
        for (int j = 0; j < 5; j++)
        {
            for (unsigned int i = 0; i < amount; i++)
            {
                model = modelMatrices[j][i];
                //圆轨道更新位置
                model[3][0] = model[3][0] + deltaTime * 3 > radius[j] ? model[3][0] + deltaTime * 3 - 2 * radius[j] : model[3][0] + deltaTime * 3;
                model[3][2] = float(sqrt(radius[j] * radius[j] - model[3][0] * model[3][0]) - 500);
                modelMatrices[j][i] = model;
                //自转
                model = glm::rotate(model, glm::radians((float)glfwGetTime() * 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                rock.Draw(shader);
            }
        }
        

        //craft
        for (int i = 0; i < 20; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, craftPositions[i]);
            model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.4f));
            
            //左右巡航警戒
            int p = i % 4 + 1;
            if (model[3][0] >= 20.0f || model[3][0] <= -20.0f)
            {
                //到边界回头
                model[3][0] = model[3][0] > 0.0f ? 20.0f : -20.0f;
                state[i] *= -1;
            }
            //更新位置
            model = glm::translate(model, glm::vec3(float(state[i]) * float(p), 0.0f, 0.0f) * deltaTime * craftSpeed[speedNumber]);
            
            //警戒检测
            wring[i] = false;
            if (abs(camera.Position.z - 8.0f - model[3][2]) < 10.0f && abs(camera.Position.x - model[3][0]) < 5.0f)
            {
                wring[i] = true;
            }
            else
            {
                craftPositions[i][0] = model[3][0];
            }

            if (wring[i] && wring_sound[i])
            {
                SoundEngine->play2D("resources/media/sound/wring.mp3", GL_FALSE);
                wring_sound[i] = false;
            }

            //警戒时自转
            model = glm::rotate(model, glm::radians((float)glfwGetTime() * 100.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            //draw
            shader.setMat4("model", model);
            shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            shader.setVec3("material.diffuse", glm::vec3(1.8f, 1.8f, 1.8f));
            shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            shader.setFloat("material.shininess", 32.0f);

            if (wring[i])
            {
                wringcraft.Draw(shader);
            }
            else craft.Draw(shader);
        }
        



        //draw gold rock 
        for (int i = 0; i < 13; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, rockPositions[i]);
            model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f));

            if (abs(camera.Position.z - 8.0f - model[3][2]) < 10.0f && abs(camera.Position.x - model[3][0]) < 5.0f)
            {
                collect[i] = true;
            }
            if (collect[i] && collect_sound[i])
            {
                SoundEngine->play2D("resources/media/sound/reward.mp3", GL_FALSE);
                collect_sound[i] = false;
                treasure++;//计数加一
            }

            shader.setMat4("model", model);
            shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
            shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            shader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setFloat("material.shininess", 32.0f);
            if(!collect[i])
                goldrock.Draw(shader);
        }

        //text.RenderText(tshader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Sets the Keyboard callback for the current window.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        speedNumber++;
        if (speedNumber == 6)
            speedNumber = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        point_powerlevel++;
        if (point_powerlevel == 6)
            point_powerlevel = 5;
    }
        
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        point_powerlevel--;
        if (point_powerlevel == -1)
            point_powerlevel = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        spot_powerlevel++;
        if (spot_powerlevel == 6)
            spot_powerlevel = 5;
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        spot_powerlevel--;
        if (spot_powerlevel == -1)
            spot_powerlevel = 0;
    }


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Sets the mouse-button callback for the current window.	
    // Sets the Keyboard callback for the current window.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (pointlight_switch == glm::vec4(1.0f))pointlight_switch = glm::vec4(0.0f);
        else pointlight_switch = glm::vec4(1.0f);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (spotlight_switch == glm::vec4(1.0f))spotlight_switch = glm::vec4(0.0f);
        else spotlight_switch = glm::vec4(1.0f);
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Texture load at path: " << path << std::endl;

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            std::cout << "Cubemap texture load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void SendLightDate(const Shader &shader)
{
    // directional light
    shader.setVec3("viewPos", camera.Position);

    shader.setVec4("dirLight.dir_switch", dirlight_switch);
    shader.setVec3("dirLight.direction", -1.0f, -1.0f, 0.0f);
    shader.setVec3("dirLight.ambient", 1.1f, 1.1f, 1.1f);
    shader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
    shader.setVec3("dirLight.specular", 0.9f, 0.9f, 0.9f);
    shader.setVec3("dirLight.lightColor", 1.0f, 1.0f, 1.0f);

    // point light
    for (int i = 0; i < 20; i++)
    {
        string s1 = "pointlight[", s3 = "]";
        string s2 = to_string(i);
        string str = s1 + s2 + s3;
        shader.setVec4(str+".point_switch", pointlight_switch);
        shader.setVec3(str + ".position", craftPositions[i]);
        shader.setVec3(str + ".ambient", 1.05f, 1.05f, 1.05f);
        shader.setVec3(str + ".diffuse", 0.6f, 0.6f, 0.6f);
        shader.setVec3(str + ".specular", 1.0f, 1.0f, 1.0f);
        shader.setInt(str + ".powerlevel", point_powerlevel);
        shader.setVec3(str + ".lightColor", craftColor[i]);
    }
    
    //spot light
    shader.setVec4("spotLight.spot_switch", spotlight_switch);
    shader.setVec3("spotLight.position", camera.Position + glm::vec3(0.0f, -4.0f, -16.0f));
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
    shader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shader.setInt("spotLight.powerlevel", spot_powerlevel);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
    shader.setVec3("spotLight.lightColor", 1.0f, 1.0f, 1.0f);
}


