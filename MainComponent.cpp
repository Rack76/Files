#include "Window.h"
#include "Object.h"
#include "MeshLoader.h"
#include "Globals.h"
#include "Camera.h"
#include "TextureImageLoader.h"
#include "Shader.h"
#include "Renderer.h"
#include "collisionDetection.h"
#include "CapsuleObject.h"
#include "Particle.h"
#include <iostream>

#define RADIAN(degrees) (degrees * (3.14159265359 / 180))
#define g 9.8

    unsigned int index[] = {0, 11, 5,
                     0, 5, 1,
                     0, 1, 7,
                     0, 7, 10,
                     0, 10, 11,
                     1, 5, 9,
                     5, 11, 4,
                     11, 10, 2,
                     10, 7, 6,
                     7, 1, 8,
                     3, 9, 4,
                     3, 4, 2,
                     3, 2, 6,
                     3, 6, 8,
                     3, 8, 9,
                     4, 9, 5,
                     2, 4, 11,
                     6, 2, 10,
                     8, 6, 7,
                     9, 8, 1};

std::vector<Object*> worldObject;
Camera actualCamera(640, 480);

    bool CAMERA_MOVE_FORWARD = false;
    bool CAMERA_MOVE_BACKWARD = false;
    bool CAMERA_MOVE_LEFT = false;
    bool CAMERA_MOVE_RIGHT = false;
    bool CAMERA_MOVE_UP = false;
    bool CAMERA_MOVE_DOWN = false;
    bool ROTATE_OBJECT = false;
    bool WINDOW_QUIT = false;

    SDL_Event event;

bool conservativeAdvancement(CapsuleObject* capsule1, CapsuleObject* capsule2, float dt, int maximumIterations);
void contactQueries(CapsuleObject* capsule1, CapsuleObject* capsule2, glm::vec3& contactPoint, glm::vec3& force1, glm::vec3& force2);
void closestPointsCapsules(CapsuleObject* capsule1, CapsuleObject* capsule2, glm::vec3& cp1, glm::vec3& cp2);
void applyCollisionForce(glm::vec3 contactPoint, glm::vec3 force, Object* object);

void updateWorld(float duration)
{
       if(CAMERA_MOVE_FORWARD)
       {
           actualCamera.translateOrientation(1);
       }
       if(CAMERA_MOVE_BACKWARD)
       {
           actualCamera.translateOrientation(-1);
       }
       if(CAMERA_MOVE_RIGHT)
       {
           actualCamera.translateNormal(1);
       }
       if(CAMERA_MOVE_LEFT)
       {
           actualCamera.translateNormal(-1);
       }
       if(CAMERA_MOVE_UP)
       {
           actualCamera.translateUp(1);
       }
       if(CAMERA_MOVE_DOWN)
       {
           actualCamera.translateUp(-1);
       }
       if(ROTATE_OBJECT)
       {

       }

}

void getInput()
{

        while(SDL_PollEvent(&event))              //inputHandling
        {
            switch(event.type)
            {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    WINDOW_QUIT = true;
                    break;
                case SDLK_z:
                    CAMERA_MOVE_FORWARD = true;
                    break;
                case SDLK_s:
                    CAMERA_MOVE_BACKWARD = true;
                    break;
                case SDLK_q:
                    CAMERA_MOVE_LEFT = true;
                    break;
                case SDLK_d:
                    CAMERA_MOVE_RIGHT = true;
                    break;
                case SDLK_SPACE:
                    CAMERA_MOVE_UP = true;
                    break;
                case SDLK_LSHIFT:
                    CAMERA_MOVE_DOWN = true;
                    break;
                case SDLK_r:
                    ROTATE_OBJECT = true;
                    break;
                case SDLK_LEFT:

                    break;
                case SDLK_RIGHT:

                    break;
                case SDLK_UP:

                    break;
                case SDLK_DOWN:

                    break;
                }
            break;

            case SDL_KEYUP:
                switch(event.key.keysym.sym)
                {
                case SDLK_z:
                    CAMERA_MOVE_FORWARD = false;
                    break;
                case SDLK_s:
                    CAMERA_MOVE_BACKWARD = false;
                    break;
                case SDLK_q:
                    CAMERA_MOVE_LEFT = false;
                    break;
                case SDLK_d:
                    CAMERA_MOVE_RIGHT = false;
                    break;
                case SDLK_SPACE:
                    CAMERA_MOVE_UP = false;
                    break;
                case SDLK_LSHIFT:
                    CAMERA_MOVE_DOWN = false;
                    break;
                case SDLK_r:
                    ROTATE_OBJECT = false;
                    break;
                }
            break;
            }

            switch(event.motion.type)
            {
            case SDL_MOUSEMOTION:
                actualCamera.rotateCamera(event.motion.xrel, event.motion.yrel);
                break;
            }
        }
}

void generateOutout(Renderer& renderer, Window& window)
{
        for(int i = 0; i < worldObject.size(); i++)
        {
            renderer.draw(i);
        }

        window.render();
}

int main(int argc, char **argv)
{
    Window window(1000, 500, 640, 480);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_SetRelativeMouseMode(SDL_TRUE);

     glewInit();

     glViewport(0, 0, 640, 640);
     glEnable(GL_DEPTH_TEST);

    Shader shader;
    shader.parseCreateAndCompileShader("basic.shader");

    Shader shader2;
    shader2.parseCreateAndCompileShader("color.shader");

    TextureImageLoader textureImageLoader;
    textureImageLoader.loadImage("texture1.jpg", GL_TEXTURE0);

    MeshLoader meshLoader;
    meshLoader.loadOBJFile("capsule3.obj");

    MeshLoader meshLoader1;
    meshLoader1.loadOBJFile("icoSphere.obj");

    Renderer renderer(shader);

    Renderer renderer2(shader);

    worldObject.push_back(new CapsuleObject(6, 0, 1, glm::vec3(0, 0, 0), glm::vec3(RADIAN(1), 0, 0)));
    worldObject[0]->loadVertices(meshLoader.verticeTextureArray);

    worldObject.push_back(new CapsuleObject(0, 0, 0, glm::vec3(0, 0, 0.5), glm::vec3(RADIAN(90), 0, 0)));
    worldObject[1]->loadVertices(meshLoader.verticeTextureArray);

    static_cast<CapsuleObject*>(worldObject[0])->setCapsule();
    static_cast<CapsuleObject*>(worldObject[1])->setCapsule();

    glm::vec3 cp1;
    glm::vec3 cp2;

     CapsuleIntersect(static_cast<CapsuleObject*>(worldObject[0])->capsule, static_cast<CapsuleObject*>(worldObject[1])->capsule, cp1, cp2);
     glm::vec3 intersection = cp1 + (cp2 - cp1) * 0.5f;

    int temp, duration;

    while(!WINDOW_QUIT)
    {

       getInput();

       static int lastTime = 0;
       static int firstTime = SDL_GetTicks();
       temp = lastTime;
       lastTime = SDL_GetTicks() - firstTime;
       duration = lastTime - temp;

       updateWorld((float)duration/1000);

       generateOutout(renderer2, window);

    }

    window.cleanUp();

    return 0;
}

 void closestPointsCapsules(CapsuleObject* capsule1, CapsuleObject* capsule2, glm::vec3& cp1, glm::vec3& cp2)
{
        glm::vec3 temp1, temp2;
        CapsuleIntersect(capsule1->capsule, capsule2->capsule, temp1, temp2);
        cp1 = 0.1f * glm::normalize(temp2 - temp1);

        cp2 = 0.1f * glm::normalize(temp1 - temp2);
}

void contactQueries(CapsuleObject* capsule1, CapsuleObject* capsule2, glm::vec3& contactPoint, glm::vec3& force1, glm::vec3& force2)
{
    glm::vec3 temp1, temp2;
    CapsuleIntersect(capsule1->capsule, capsule2->capsule, temp1, temp2);
    contactPoint = temp1 + (temp2 - temp1) * 0.5f;
    glm::vec3 contactNormal1 = - glm::normalize(contactPoint - temp1);
    glm::vec3 contactNormal2 = - glm::normalize(contactPoint - temp2);
    force1 = contactNormal1 * glm::length(capsule2->force + capsule2->torque);
    force2 = contactNormal2 * glm::length(capsule1->force + capsule1->torque);
}

bool conservativeAdvancement(CapsuleObject* capsule1, CapsuleObject* capsule2, float dt, int maximumIterations)
{
    glm::vec3 cp1, cp2;
    float t = 0;
    int iterations = 0;
    while(1)
    {
        closestPointsCapsules(capsule1, capsule2, cp1, cp2);
        float denominator = glm::dot(capsule1->linearVelocity - capsule2->linearVelocity, (cp2 - cp1) / glm::length(cp2 - cp1))
                                                + glm::length(capsule1->angularVelocity) * 2 + glm::length(capsule2->angularVelocity) * 2;

        if(denominator == 0)
        {
            return false;
        }

        float deltaT = glm::length(cp2 - cp1) / denominator;

       if(deltaT < 0)
       {
           return false;
       }

       t += deltaT;

       if(t > 1)
       {
           return false;
       }

       if(iterations > maximumIterations)
       {
           return true;
       }

        capsule1->position += capsule1->linearVelocity * dt * deltaT;
        capsule1->orientation += capsule1->linearVelocity * dt * deltaT;

       capsule2->position += capsule1->linearVelocity * dt * deltaT;
       capsule2->orientation += capsule1->linearVelocity * dt * deltaT;

       iterations++;
    }
}

void applyCollisionForce(glm::vec3 contactPoint, glm::vec3 force, Object* object)
{
    object->force = force;
    object->torque = glm::cross(contactPoint - object->position, force);
}

