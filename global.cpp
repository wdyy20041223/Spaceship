#include "CVector.h" 
#include "planet.h" 
#include "base.h"
#include "ship.h"
#include "camera.h"

ship myShip;
CMatrix transMat1, transMat2, rotateMat1, rotateMat2, scaleMat1, scaleMat2, finalMat;
Astronaut astronaut;
Camera globalCamera, astronautCamera, tempCamera,shipCamera;
bool ControlingGlobal, ControllingShip;

CVector g_lastCamPos;
CEuler g_lastCamEuler;
CVector g_lastCamForward;
CVector g_lastCamUp;
float g_lastCamSpeed = 0;
Camera::ControlMode g_lastCamMode = Camera::EULER;