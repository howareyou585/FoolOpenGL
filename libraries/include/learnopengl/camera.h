#ifndef CAMERA_H
#define CAMERA_H

//#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <vector>
#include "BoundingBox.h"
#include "Ray.h"
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;
const float FAR         = 10000.0f;
const float NEAR        = 0.1f;

const  float NEAR_PLANE_FACTOR = 0.01f;  // 近平面比例
const  float FAR_PLANE_FACTOR = 10.f;    // 远平面比例


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float FarPlane;
    float NearPlane;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        FarPlane = FAR;
        NearPlane = NEAR;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    glm::mat4 GetViewMatrix(glm::vec3& targetPos)
    {
        return glm::lookAt(Position, targetPos, Up);
    }
    glm::mat4 GetProjectionMatrix(float radio)
    {
        return glm::perspective(glm::radians(Zoom), radio,  NearPlane, FarPlane);   
    }
    glm::mat4 GetProjectionMatrix(float radio, float zoom)
    {
        return glm::perspective(zoom, radio, NearPlane, FarPlane);
    }
    bool InitCamera(BoundingBox& sceneBoxBoundingBox, float factor)
    {
        bool bRet = false;
        if (factor <= 0.0f)
        {
            return bRet;
        }
        auto length = sceneBoxBoundingBox.GetLength();
        if (length <= 0)
        {
            return bRet;
        }
        glm::vec3 center = sceneBoxBoundingBox.GetCenter();
		//Front = glm::normalize(center - Position);
        Position = center - Front * length * factor;
        //Right = glm::normalize(glm::cross(Front, Up));

        SetNearPlane(length *1.5 * NEAR_PLANE_FACTOR);
        SetFarPlane(length * 1.5 * FAR_PLANE_FACTOR);

        bRet = true;
        return bRet;
    }
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        Position.y = 0;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f; 
    }
    void SetPosition(glm::vec3 & pnt)
    {
        Position = pnt;
    }

    void SetNearPlane(float nearVal)
    {
        NearPlane = nearVal;
    }
    void SetFarPlane(float farVal)
    {
        FarPlane = farVal;
    }
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }

    Ray GetRayFromScreenCoord(float x, float y)
    {
        //glm::mat4 viewMulpyProjectionInvMatrix = glm::inverse(GetProjectionMatrix() * GetViewMatrix());
        //将坐标(x,y)从[0,1] 变换到[-1,1]
        //float rx = 2.f * x - 1.f;
        //float ry = 1.f - 2.f * y;
        ////计算射线的
        //glm::vec3 nearPnt(rx, ry, 0.f);
        //glm::vec3 farPnt(rx, ry, 1.f);
        //nearPnt = viewMulpyProjectionInvMatrix * nearPnt
        //glm::vec3 direction = glm(farPnt - nearPnt);
        Ray retRay;
        return retRay;
    }

    glm::quat Camera::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
    {

        start = normalize(start);

        dest = normalize(dest);

        float cosTheta = dot(start, dest);

        glm::vec3 rotationAxis;

        if (cosTheta < -1 + 0.001f) {

            // special case when vectors in opposite directions:

            // there is no "ideal" rotation axis

            // So guess one; any will do as long as it's perpendicular to start

            rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

            if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!

                rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = normalize(rotationAxis);

            return glm::angleAxis(180.0f, rotationAxis);

        }

        rotationAxis = cross(start, dest);

        float s = sqrt((1 + cosTheta) * 2);

        float invs = 1 / s;

        return glm::quat(

            s * 0.5f,

            rotationAxis.x * invs,

            rotationAxis.y * invs,

            rotationAxis.z * invs

        );

    }
};
#endif