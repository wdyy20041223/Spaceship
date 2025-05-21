#pragma once
#include "ship.h";
#include "CMatrix.h" 
#include "Astronaut.h"
#include "Camera.h"

class CMatrix;

extern ship myShip;
extern CMatrix transMat1, transMat2, rotateMat1, rotateMat2, scaleMat1, scaleMat2, finalMat;
extern Astronaut astronaut;
extern Camera globalCamera, astronautCamera,tempCamera, shipCamera;
extern bool ControlingGlobal;
extern bool ControllingShip;
extern CVector g_lastCamPos;
extern CEuler g_lastCamEuler;
extern CVector g_lastCamForward;
extern CVector g_lastCamUp;
extern float g_lastCamSpeed;
extern Camera::ControlMode g_lastCamMode;