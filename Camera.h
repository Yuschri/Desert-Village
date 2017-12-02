#ifndef _Camera_H
#define _Camera_H

#include <windows.h>
#include <gl\Gl.h>
#include <gl\glu.h>

#include "Vector3.h"


class Camera {

public:

	Camera() {}
	Camera( float posX, float posY, float posZ,
			 float viewX, float viewY, float viewZ,
			 float upX, float upY, float upZ );
	~Camera() {}

	// co-ordinates
	Vector3 up, view, pos;

	// movement functions
	void Rotate( float x, float y, float z );
	void Move( float speed );
	void MoveSide( float speed );
	void LookUp( float speed );
	void MoveUp( float speed );
	void MoveFront( float speed );

	// initialize / change the camera's position and view
	void Position( float posX, float posY, float posZ,
				   float viewX, float viewY, float viewZ,
				   float upX, float upY, float upZ );

	// call glulookat
	void Update();

    Vector3 getPos();
    Vector3 getView();
    Vector3 getUp();
};

#endif
