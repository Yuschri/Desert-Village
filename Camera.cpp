#include <math.h>
#include <windows.h>
#include <gl/Gl.h>

#include "Camera.h"


Camera::Camera( float posX, float posY, float posZ,
		   		  float viewX, float viewY, float viewZ,
				  float upX, float upY, float upZ ) {

	Position( posX,  posY,  posZ,
			  viewX,  viewY,  viewZ,
			  upX,  upY,  upZ );

}



void Camera::Move( float speed ) {

	Vector3 d = view - pos;

	// noralize the vector
	d.Normalize();

	pos.x = pos.x + d.x * speed;
	pos.z = pos.z + d.z * speed;
	pos.y = pos.y + d.y * speed;
	view.x = view.x + d.x * speed;
	view.z = view.z + d.z * speed;
	view.y = view.y + d.y * speed;

}

void Camera::MoveSide( float speed ) {

	Vector3 d = view - pos;

	// noralize the vector
	d.Normalize();

	pos.x = pos.x + d.x * speed;
	pos.z = pos.z + d.z * speed;
	pos.y = pos.y + d.y * speed;
	view.x = view.x + d.x * speed;
	view.z = view.z + d.z * speed;
	view.y = view.y + d.y * speed;

}

void Camera::LookUp( float speed ) {

	view.y = view.y + speed;
}

void Camera::MoveUp( float speed ) {

	pos.y = pos.y + speed;
	view.y = view.y + speed;
}

void Camera::MoveFront( float speed ) {

	Vector3 d = view - pos;

	// noralize the vector
	d.Normalize();

	pos.x = pos.x + d.x * speed;
	pos.z = pos.z + d.z * speed;
	view.x = view.x + d.x * speed;
	view.z = view.z + d.z * speed;
}

void Camera::Position( float posX, float posY, float posZ,
					    float viewX, float viewY, float viewZ,
						float upX, float upY, float upZ ) {

	pos		= Vector3( posX, posY, posZ );
	view	= Vector3( viewX, viewY, viewZ );
	up		= Vector3( upX, upY, upZ );

}

void Camera::Update() {

	gluLookAt( pos.x, pos.y, pos.z,
			   view.x, view.y, view.z,
			   up.x, up.y, up.z );

}

void Camera::Rotate( float x, float y, float z ) {

	Vector3 d = view - pos;

	//d.Normalize();

	if( x ) {

		view.y = float( pos.y + cos( x ) * d.y - sin( x ) * d.z );
		view.z = float( pos.z + sin( x ) * d.y + cos( x ) * d.z );

	}

	if( y ) {

		view.x = float( pos.x + cos( y ) * d.x - sin( y ) * d.z );
		view.z = float( pos.z + sin( y ) * d.x + cos( y ) * d.z );

	}

	if( z ) {

		view.x = float( pos.x + sin( z ) * d.y + cos( z ) * d.x );
		view.y = float( pos.y + cos( z ) * d.y - sin( z ) * d.x );

	}
}

Vector3 Camera::getPos() {
    return pos;
}

Vector3 Camera::getView() {
    return view;
}

Vector3 Camera::getUp() {
    return up;
}
