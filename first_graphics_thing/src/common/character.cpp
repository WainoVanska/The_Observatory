#include "character.h"
#include "cube.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

void drawCharacter(GLuint VAO, GLuint modelLoc, float t, const glm::mat4& baseTransform)
{
    float legAngle = sin(t * 5.0f) * 30.0f;
    float armAngle = sin(t * 5.0f) * 60.0f;
    float headAngle = sin(t * 10.0f) * 2.0f;
    float bodyMovement = sin(t * 10.0f) * 2.0f;

    glm::mat4 character = baseTransform;

    character = glm::translate(character, glm::vec3(0.0f, bodyMovement / 100.0f, 0.0f));
    character = glm::rotate(character, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 body = character;
    body = glm::scale(body, glm::vec3(0.9f, 1.5f, 0.4f));
    drawCube(VAO, modelLoc, body);

    glm::mat4 head = character;
    head = glm::translate(head, glm::vec3(0.0f, (headAngle / 100.0f) + 0.55f, 0.0f));
    head = glm::scale(head, glm::vec3(0.6f, 0.6f, 0.6f));
    drawCube(VAO, modelLoc, head);

    glm::mat4 leftLeg = character;
    leftLeg = glm::translate(leftLeg, glm::vec3(-0.20f, -0.22f, 0.0f));
    leftLeg = glm::rotate(leftLeg, glm::radians(legAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    leftLeg = glm::translate(leftLeg, glm::vec3(0.0f, -0.5f, 0.0f));
    leftLeg = glm::scale(leftLeg, glm::vec3(0.2f, 1.5f, 0.2f));
    drawCube(VAO, modelLoc, leftLeg);

    glm::mat4 rightLeg = character;
    rightLeg = glm::translate(rightLeg, glm::vec3(0.20f, -0.22f, 0.0f));
    rightLeg = glm::rotate(rightLeg, glm::radians(-legAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    rightLeg = glm::translate(rightLeg, glm::vec3(0.0f, -0.5f, 0.0f));
    rightLeg = glm::scale(rightLeg, glm::vec3(0.2f, 1.5f, 0.2f));
    drawCube(VAO, modelLoc, rightLeg);

    glm::mat4 leftArm = character;
    leftArm = glm::translate(leftArm, glm::vec3(-0.35f, 0.20f, 0.1f));
    leftArm = glm::rotate(leftArm, glm::radians(-armAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    leftArm = glm::translate(leftArm, glm::vec3(0.0f, -0.4f, 0.0f));
    leftArm = glm::scale(leftArm, glm::vec3(0.2f, 1.7f, 0.2f));
    drawCube(VAO, modelLoc, leftArm);

    glm::mat4 rightArm = character;
    rightArm = glm::translate(rightArm, glm::vec3(0.35f, 0.20f, -0.1f));
    rightArm = glm::rotate(rightArm, glm::radians(armAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    rightArm = glm::translate(rightArm, glm::vec3(0.0f, -0.4f, 0.0f));
    rightArm = glm::scale(rightArm, glm::vec3(0.2f, 1.7f, 0.2f));
    drawCube(VAO, modelLoc, rightArm);
}