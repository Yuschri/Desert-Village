#include <glut.h>
#include <math.h>
#include <iostream>

#include <cmath>
#include <cstdio>
#include <png.h>

#include "Camera.h"

using namespace std;

#define PI 3.14159265358979323846
#define GL_GENERATE_MIPMAP 0x8191

GLuint idtexture = -1;
GLuint stoneTexture = -1;
GLuint sandTexture = -1;
GLuint doorTexture = -1;
GLuint roofTexture = -1;

GLfloat vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
                        {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
                        {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

GLfloat normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

GLfloat colors[][3] = {{0.0,0.0,0.0},{1.0,0.0,0.0},
{1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
{1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}};

GLuint loadBmpFile(const char* fileName) {
    GLuint texture_id;
    unsigned char * pBitmapData;
    int	width, height,bpp;

    FILE				* fp;
	BITMAPFILEHEADER	bmpFH;
	BITMAPINFOHEADER	bmpIH;
	unsigned char		temp;

	fp = fopen( fileName, "rb" ); // rb = read binary
	if( fp == NULL )
		return( -1 );
	// read in the file header
	fread( ( void * )&bmpFH, sizeof( BITMAPFILEHEADER ), 1, fp );
	if( bmpFH.bfType != 0x4D42 ) {
		fclose( fp );
		return( -1 );
	}

	// read in the info header
	fread( ( void * )&bmpIH, sizeof( BITMAPINFOHEADER ), 1, fp );
	// move the file stream to teh start of the image data
	fseek( fp, bmpFH.bfOffBits, SEEK_SET );
	// set size in bytes
	bmpIH.biSizeImage = bmpIH.biHeight * bmpIH.biWidth * ( bmpIH.biBitCount / 8 );
	// allocate mem for the image data
	pBitmapData = new unsigned char[ bmpIH.biSizeImage ];
	if( pBitmapData == NULL ){
		// if there was trouble allocating the mem
		fclose( fp );
		return( -1 );
	}
	// read from the stream ( 1 byte at a time, biSizeImage times )
	fread( ( void * )pBitmapData, 1, bmpIH.biSizeImage, fp );
	if( pBitmapData == NULL ) {
		fclose( fp );
		return( -1 );
	}
	for( int c = 0; c < bmpIH.biSizeImage; c += 3 ) {
		// swap the red and blue bytes
		temp					= pBitmapData[ c ];
		pBitmapData[ c ]		= pBitmapData[ c + 2 ];
		pBitmapData[ c + 2 ]	= temp;
	}

	fclose( fp );

	width	= bmpIH.biWidth;
	height	= bmpIH.biHeight;
	bpp		= bmpIH.biBitCount;

	glGenTextures( 1, &texture_id );
	// bind and pass texure data into openGL
	glBindTexture( GL_TEXTURE_2D, texture_id );
	// set parameters to make mipmaps
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// create the textures
	glTexImage2D( GL_TEXTURE_2D, 0,
				  GL_RGB, width, height, 0,
				  GL_RGB, GL_UNSIGNED_BYTE, pBitmapData );

    return texture_id;
}


//static GLdouble viewer[]= {0.0, 0.0, 10.0};
static GLfloat theta = 0.0, speed = 0.05;
static bool spin = true;
GLfloat cvertices[8][3] = {{-0.5 , -0.5, 0.5},
                           { 0.5 , -0.5, 0.5},
                           { 0.5 ,  0.5, 0.5},
                           {-0.5 ,  0.5, 0.5},

                           {-0.5 , -0.5, -0.5},
                           { 0.5 , -0.5, -0.5},
                           { 0.5 ,  0.5, -0.5},
                           {-0.5 ,  0.5, -0.5}
                           };

Camera*	camera;

void normalize(float*v)
{
    float length =sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

    for(int i=0; i<3; i++)
    {
        v[i]=v[i]/length;
    }
}

float *cross_product(float*a, float*b)
{
    float* result = new float[3];
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] =  -(a[0] * b[2] - a[2] * b[0]);
    result[2] = a[0] * b[1] - a[1] * b[0];

    normalize(result);

    return result;
}

float *calculate_normal(float *a, float *b, float *c)
{
    float x[]={b[0]-a[0], b[1]-a[1], b[2]-a[2] };
    float y[]={c[0]-a[0], c[1]-a[1], c[2]-a[2] };

    float *result=cross_product(x,y);

    return result;
}

void myinit ()
{
    camera = new Camera( 0.0, 0.0, 750.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	stoneTexture = loadBmpFile("stone.bmp");
	sandTexture = loadBmpFile("sand.bmp");
	doorTexture = loadBmpFile("door.bmp");
	roofTexture = loadBmpFile("roof.bmp");

	//kalau pakai color langsung material
    //glColorMaterial(GL_FRONT, GL_DIFFUSE);
    //glEnable(GL_COLOR_MATERIAL);

    glClearColor (1.0, 1.0, 1.0, 1.0);
}

void setmaterialCactus(){
    /* comment the material*/
    GLfloat amb[]={0.0f,0.45f,0.0f,1.0f};
    GLfloat diff[]={0.0f,0.1f,0.0f,0.0f};
    GLfloat spec[]={0.0f,0.0f,0.0f,0.0f};
    GLfloat shine=0.1f;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diff);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}

void setmaterialPyramid(){
    /* comment the material*/
    GLfloat amb[]={0.55f,0.25f,0.0f,1.0f};
    GLfloat diff[]={0.5f,0.49f,0.0f,1.0f};
    GLfloat spec[]={0.0f,0.0f,0.0f,1.0f};
    GLfloat shine=15.0f;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diff);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}

void setmaterialRumah(){
    /* comment the material*/
    GLfloat amb[]={0.5f,0.5f,0.2f,1.0f};
    GLfloat diff[]={0.7f,0.65f,0.35f,1.0f};
    GLfloat spec[]={0.0f,0.0f,0.0f,1.0f};
    GLfloat shine=0.0f;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diff);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}

// http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
void setmaterialSand(){
    /* comment the material*/
    GLfloat amb[]={0.85f,0.8f,0.25f,1.0f};
    GLfloat diff[]={0.55f,0.5f,0.2f,1.0f};
    GLfloat spec[]={0.0f,0.0f,0.0f,1.0f};
    GLfloat shine=0.0f;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diff);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}

void setmaterialMill(){
    /* comment the material*/
    GLfloat amb[]={1.0f,0.85f,0.28f,1.0f};
    GLfloat diff[]={0.35f,0.35f,0.2f,1.0f};
    GLfloat spec[]={0.2f,0.2f,0.2f,1.0f};
    GLfloat shine=7.0f;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diff);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}

void setmaterialPerl(){
    float MatAmbient[] = { 0.25f, 0.20725f, 0.20725f, 0.922f};
    float MatDiffuse[] = { 1.0f, 0.829f, 0.829f, 0.922f  };
    float MatSpecular[] = { 0.296648f, 0.296648f, 0.296648f, 0.922f };
    float MatShininess = 11.264f;
    float black[] = {0.0f,0.0f,0.0f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}

void setmaterialRuby(){
    float MatAmbient[] = { 0.1745f, 0.01175f, 0.01175f, 0.55f};
    float MatDiffuse[] = { 0.61424f, 0.04136f, 0.04136f, 0.55f  };
    float MatSpecular[] = { 0.61424f, 0.04136f, 0.04136f, 0.55f };
    float MatShininess = 76.8f;
    float black[] = {0.0f,0.0f,0.0f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}

void setmaterialEmerald(){
    float MatAmbient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f};
    float MatDiffuse[] = { 0.07568f, 0.61424f, 0.07568f, 0.55f  };
    float MatSpecular[] = { 0.633f, 0.727811f, 0.633f, 0.55f  };
    float MatShininess = 76.8f;
    float black[] = {0.0f,0.0f,0.0f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}

void cyl(float posisiX, float posisiY, float posisiZ, float tinggi, float lebarBawah, float lebarAtas, int sisi,  float texture, GLint texture_skin, int detail, float derajadY){

    float oldX, oldY, oldZ, oldX1, oldY1, oldZ1, X, Y, Z, X1, Y1, Z1;
    oldX=lebarBawah*cos(0* PI / 180.0);
    oldZ=lebarBawah*sin(0* PI / 180.0);
    oldX1=lebarAtas*cos(0* PI / 180.0);
    oldZ1=lebarAtas*sin(0* PI / 180.0);

    X=lebarBawah*cos(0* PI / 180.0);
    Z=lebarBawah*sin(0* PI / 180.0);
    X1=lebarAtas*cos(0* PI / 180.0);
    Z1=lebarAtas*sin(0* PI / 180.0);

    float sizeX, sizeY, texX=0, texY=0;

    sizeY=abs(sqrt(pow((X+posisiX)-(oldX+posisiX),2)+pow(abs((posisiY+tinggi)-(posisiY)),2)));

    glBindTexture(GL_TEXTURE_2D, texture_skin);

    for(int i=0; i<=360; i+=(360/sisi)){
        X=lebarBawah*cos(i* PI / 180.0);
        Z=lebarBawah*sin(i* PI / 180.0);
        X1=lebarAtas*cos(i* PI / 180.0);
        Z1=lebarAtas*sin(i* PI / 180.0);

        GLfloat vertices[4][3]=
        {
            {oldX+posisiX, posisiY, oldZ+posisiZ},
            {oldX1+posisiX, posisiY+tinggi, oldZ1+posisiZ},
            {X1+posisiX, posisiY+tinggi, Z1+posisiZ},
            {X+posisiX, posisiY, Z+posisiZ}
        };

        glBegin(GL_POLYGON);
        glNormal3fv(calculate_normal(vertices[0], vertices[1], vertices[2]));
            sizeX=abs(sqrt(pow(abs((X+posisiX)-(oldX+posisiX)),2)+pow(abs((Z+posisiZ)-(oldZ+posisiZ)),2)));

            glTexCoord2f(texX/texture, texY/texture);
            glVertex3fv(vertices[0]);
            glTexCoord2f((texX)/texture, (texY+sizeY)/texture);

            glVertex3fv(vertices[1]);
            glTexCoord2f((texX+sizeX)/texture, (texY+sizeY)/texture);
            glVertex3fv(vertices[2]);

            glTexCoord2f((texX+sizeX)/texture, texY/texture);
            glVertex3fv(vertices[3]);
        glEnd();
        oldX=X;
        oldZ=Z;
        oldX1=X1;
        oldZ1=Z1;
        texX+=sizeX;
    }
}

void cube(float besar, GLuint texture_skin, float skala){
    glBindTexture(GL_TEXTURE_2D, texture_skin);
    // Depan
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[0], cvertices[1], cvertices[2]));
        glTexCoord2f(cvertices[0][0]*besar/skala, cvertices[0][1]*besar/skala);
        glVertex3f(cvertices[0][0]*besar, cvertices[0][1]*besar, cvertices[0][2]*besar);
        glTexCoord2f(cvertices[1][0]*besar/skala, cvertices[1][1]*besar/skala);
        glVertex3f(cvertices[1][0]*besar, cvertices[1][1]*besar, cvertices[1][2]*besar);
        glTexCoord2f(cvertices[2][0]*besar/skala, cvertices[2][1]*besar/skala);
        glVertex3f(cvertices[2][0]*besar, cvertices[2][1]*besar, cvertices[2][2]*besar);
        glTexCoord2f(cvertices[3][0]*besar/skala, cvertices[3][1]*besar/skala);
        glVertex3f(cvertices[3][0]*besar, cvertices[3][1]*besar, cvertices[3][2]*besar);
    glEnd();
    // Atas
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[3], cvertices[2], cvertices[6]));
        glTexCoord2f(cvertices[3][0]*besar/skala, cvertices[3][2]*besar/skala);
        glVertex3f(cvertices[3][0]*besar, cvertices[3][1]*besar, cvertices[3][2]*besar);
        glTexCoord2f(cvertices[2][0]*besar/skala, cvertices[2][2]*besar/skala);
        glVertex3f(cvertices[2][0]*besar, cvertices[2][1]*besar, cvertices[2][2]*besar);
        glTexCoord2f(cvertices[6][0]*besar/skala, cvertices[6][2]*besar/skala);
        glVertex3f(cvertices[6][0]*besar, cvertices[6][1]*besar, cvertices[6][2]*besar);
        glTexCoord2f(cvertices[7][0]*besar/skala, cvertices[7][2]*besar/skala);
        glVertex3f(cvertices[7][0]*besar, cvertices[7][1]*besar, cvertices[7][2]*besar);
    glEnd();
    // Belakang
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[4], cvertices[5], cvertices[6]));
        glTexCoord2f(cvertices[4][0]*besar/skala, cvertices[4][1]*besar/skala);
        glVertex3f(cvertices[4][0]*besar, cvertices[4][1]*besar, cvertices[4][2]*besar);
        glTexCoord2f(cvertices[5][0]*besar/skala, cvertices[5][1]*besar/skala);
        glVertex3f(cvertices[5][0]*besar, cvertices[5][1]*besar, cvertices[5][2]*besar);
        glTexCoord2f(cvertices[6][0]*besar/skala, cvertices[6][1]*besar/skala);
        glVertex3f(cvertices[6][0]*besar, cvertices[6][1]*besar, cvertices[6][2]*besar);
        glTexCoord2f(cvertices[7][0]*besar/skala, cvertices[7][1]*besar/skala);
        glVertex3f(cvertices[7][0]*besar, cvertices[7][1]*besar, cvertices[7][2]*besar);
    glEnd();
    // Bawah
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[0], cvertices[1], cvertices[5]));
        glVertex3f(cvertices[0][0]*besar, cvertices[0][1]*besar, cvertices[0][2]*besar);
        glVertex3f(cvertices[1][0]*besar, cvertices[1][1]*besar, cvertices[1][2]*besar);
        glVertex3f(cvertices[5][0]*besar, cvertices[5][1]*besar, cvertices[5][2]*besar);
        glVertex3f(cvertices[4][0]*besar, cvertices[4][1]*besar, cvertices[4][2]*besar);
    glEnd();
    // Kiri
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[4], cvertices[0], cvertices[3]));
        glTexCoord2f(cvertices[4][2]*besar/skala, cvertices[4][1]*besar/skala);
        glVertex3f(cvertices[4][0]*besar, cvertices[5][1]*besar, cvertices[5][2]*besar);
        glTexCoord2f(cvertices[0][2]*besar/skala, cvertices[0][1]*besar/skala);
        glVertex3f(cvertices[0][0]*besar, cvertices[0][1]*besar, cvertices[0][2]*besar);
        glTexCoord2f(cvertices[3][2]*besar/skala, cvertices[3][1]*besar/skala);
        glVertex3f(cvertices[3][0]*besar, cvertices[3][1]*besar, cvertices[3][2]*besar);
        glTexCoord2f(cvertices[7][2]*besar/skala, cvertices[7][1]*besar/skala);
        glVertex3f(cvertices[7][0]*besar, cvertices[7][1]*besar, cvertices[7][2]*besar);
    glEnd();
    // Kanan
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[5], cvertices[1], cvertices[2]));
        glTexCoord2f(cvertices[5][2]*besar/skala, cvertices[5][1]*besar/skala);
        glVertex3f(cvertices[5][0]*besar, cvertices[4][1]*besar, cvertices[4][2]*besar);
        glTexCoord2f(cvertices[1][2]*besar/skala, cvertices[1][1]*besar/skala);
        glVertex3f(cvertices[1][0]*besar, cvertices[1][1]*besar, cvertices[1][2]*besar);
        glTexCoord2f(cvertices[2][2]*besar/skala, cvertices[2][1]*besar/skala);
        glVertex3f(cvertices[2][0]*besar, cvertices[2][1]*besar, cvertices[2][2]*besar);
        glTexCoord2f(cvertices[6][2]*besar/skala, cvertices[6][1]*besar/skala);
        glVertex3f(cvertices[6][0]*besar, cvertices[6][1]*besar, cvertices[6][2]*besar);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, -1);
}

void balok(float besarX, float besarY, float besarZ, GLuint texture_skin, float skala){
    // Depan
    glBindTexture(GL_TEXTURE_2D, texture_skin);
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[0], cvertices[1], cvertices[2]));
        glTexCoord2f(cvertices[0][0]*besarX/skala, cvertices[0][1]*besarY/skala);
        glVertex3f(cvertices[0][0]*besarX, cvertices[0][1]*besarY, cvertices[0][2]*besarZ);
        glTexCoord2f(cvertices[1][0]*besarX/skala, cvertices[1][1]*besarY/skala);
        glVertex3f(cvertices[1][0]*besarX, cvertices[1][1]*besarY, cvertices[1][2]*besarZ);
        glTexCoord2f(cvertices[2][0]*besarX/skala, cvertices[2][1]*besarY/skala);
        glVertex3f(cvertices[2][0]*besarX, cvertices[2][1]*besarY, cvertices[2][2]*besarZ);
        glTexCoord2f(cvertices[3][0]*besarX/skala, cvertices[3][1]*besarY/skala);
        glVertex3f(cvertices[3][0]*besarX, cvertices[3][1]*besarY, cvertices[3][2]*besarZ);
    glEnd();
    // Atas
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[3], cvertices[2], cvertices[6]));
        glTexCoord2f(cvertices[3][0]*besarX/skala, cvertices[3][2]*besarZ/skala);
        glVertex3f(cvertices[3][0]*besarX, cvertices[3][1]*besarY, cvertices[3][2]*besarZ);
        glTexCoord2f(cvertices[2][0]*besarX/skala, cvertices[2][2]*besarZ/skala);
        glVertex3f(cvertices[2][0]*besarX, cvertices[2][1]*besarY, cvertices[2][2]*besarZ);
        glTexCoord2f(cvertices[6][0]*besarX/skala, cvertices[6][2]*besarZ/skala);
        glVertex3f(cvertices[6][0]*besarX, cvertices[6][1]*besarY, cvertices[6][2]*besarZ);
        glTexCoord2f(cvertices[7][0]*besarX/skala, cvertices[7][2]*besarZ/skala);
        glVertex3f(cvertices[7][0]*besarX, cvertices[7][1]*besarY, cvertices[7][2]*besarZ);
    glEnd();
    // Belakang
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[4], cvertices[5], cvertices[6]));
        glTexCoord2f(cvertices[4][0]*besarX/skala, cvertices[4][1]*besarY/skala);
        glVertex3f(cvertices[4][0]*besarX, cvertices[4][1]*besarY, cvertices[4][2]*besarZ);
        glTexCoord2f(cvertices[5][0]*besarX/skala, cvertices[5][1]*besarY/skala);
        glVertex3f(cvertices[5][0]*besarX, cvertices[5][1]*besarY, cvertices[5][2]*besarZ);
        glTexCoord2f(cvertices[6][0]*besarX/skala, cvertices[6][1]*besarY/skala);
        glVertex3f(cvertices[6][0]*besarX, cvertices[6][1]*besarY, cvertices[6][2]*besarZ);
        glTexCoord2f(cvertices[7][0]*besarX/skala, cvertices[7][1]*besarY/skala);
        glVertex3f(cvertices[7][0]*besarX, cvertices[7][1]*besarY, cvertices[7][2]*besarZ);
    glEnd();
    // Bawah
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[0], cvertices[1], cvertices[5]));
        glVertex3f(cvertices[0][0]*besarX, cvertices[0][1]*besarY, cvertices[0][2]*besarZ);
        glVertex3f(cvertices[1][0]*besarX, cvertices[1][1]*besarY, cvertices[1][2]*besarZ);
        glVertex3f(cvertices[5][0]*besarX, cvertices[5][1]*besarY, cvertices[5][2]*besarZ);
        glVertex3f(cvertices[4][0]*besarX, cvertices[4][1]*besarY, cvertices[4][2]*besarZ);
    glEnd();
    // Kiri
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[4], cvertices[0], cvertices[3]));
        glTexCoord2f(cvertices[4][2]*besarZ/skala, cvertices[4][1]*besarY/skala);
        glVertex3f(cvertices[4][0]*besarX, cvertices[4][1]*besarY, cvertices[4][2]*besarZ);
        glTexCoord2f(cvertices[0][2]*besarZ/skala, cvertices[0][1]*besarY/skala);
        glVertex3f(cvertices[0][0]*besarX, cvertices[0][1]*besarY, cvertices[0][2]*besarZ);
        glTexCoord2f(cvertices[3][2]*besarZ/skala, cvertices[3][1]*besarY/skala);
        glVertex3f(cvertices[3][0]*besarX, cvertices[3][1]*besarY, cvertices[3][2]*besarZ);
        glTexCoord2f(cvertices[7][2]*besarZ/skala, cvertices[7][1]*besarY/skala);
        glVertex3f(cvertices[7][0]*besarX, cvertices[7][1]*besarY, cvertices[7][2]*besarZ);
    glEnd();
    // Kanan
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[5], cvertices[1], cvertices[2]));
        glTexCoord2f(cvertices[5][2]*besarZ/skala, cvertices[5][1]*besarY/skala);
        glVertex3f(cvertices[5][0]*besarX, cvertices[5][1]*besarY, cvertices[5][2]*besarZ);
        glTexCoord2f(cvertices[1][2]*besarZ/skala, cvertices[1][1]*besarY/skala);
        glVertex3f(cvertices[1][0]*besarX, cvertices[1][1]*besarY, cvertices[1][2]*besarZ);
        glTexCoord2f(cvertices[2][2]*besarZ/skala, cvertices[2][1]*besarY/skala);
        glVertex3f(cvertices[2][0]*besarX, cvertices[2][1]*besarY, cvertices[2][2]*besarZ);
        glTexCoord2f(cvertices[6][2]*besarZ/skala, cvertices[6][1]*besarY/skala);
        glVertex3f(cvertices[6][0]*besarX, cvertices[6][1]*besarY, cvertices[6][2]*besarZ);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, -1);

}

void bidang(float besarX, float besarY, float besarZ, GLuint texture_skin){
    glBindTexture(GL_TEXTURE_2D, texture_skin);
    glBegin(GL_QUADS);
        glNormal3fv(calculate_normal(cvertices[0], cvertices[1], cvertices[2]));
        glTexCoord2f(0, 0);
        glVertex3f(cvertices[0][0]*besarX, cvertices[0][1]*besarY, cvertices[0][2]*besarZ);
        glTexCoord2f(1, 0);
        glVertex3f(cvertices[1][0]*besarX, cvertices[1][1]*besarY, cvertices[1][2]*besarZ);
        glTexCoord2f(1, 1);
        glVertex3f(cvertices[2][0]*besarX, cvertices[2][1]*besarY, cvertices[2][2]*besarZ);
        glTexCoord2f(0, 1);
        glVertex3f(cvertices[3][0]*besarX, cvertices[3][1]*besarY, cvertices[3][2]*besarZ);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, -1);
}

void polygon(int a, int b, int c , int d)
{
	glBegin(GL_POLYGON);
		glColor3fv(colors[a]);
		glNormal3fv(normals[a]);
		glVertex3fv(vertices[a]);
		glColor3fv(colors[b]);
		glNormal3fv(normals[b]);
		glVertex3fv(vertices[b]);
		glColor3fv(colors[c]);
		glNormal3fv(normals[c]);
		glVertex3fv(vertices[c]);
		glColor3fv(colors[d]);
		glNormal3fv(normals[d]);
		glVertex3fv(vertices[d]);
	glEnd();
}

void floor(){
    glPushMatrix();
    glTranslatef(0.0, -20.0, 0.0);
    //glScalef(1500.0, 0.5, 1500.0);
    balok(1500.0, 0.5, 1500.0, sandTexture, 100);
    glPopMatrix();
}

void trapesium(){
    //Sisi depan belakang
    glBegin(GL_POLYGON);
        glNormal3f(-20.0, 0.0, -20.0);
        glVertex3f(-20.0, 0.0, -20.0);
        glNormal3f(20.0, 0.0, -20.0);
        glVertex3f(20.0, 0.0, -20.0);
        glNormal3f(20.0, 40.0, -20.0);
        glVertex3f(20.0, 40.0, -20.0);
        glNormal3f(10.0, 40.0, -20.0);
        glVertex3f(10.0, 40.0, -20.0);
        glNormal3f(-20.0, 10.0, -20.0);
        glVertex3f(-20.0, 10.0, -20.0);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(-20.0, 0.0, 20.0);
        glVertex3f(-20.0, 0.0, 20.0);
        glNormal3f(20.0, 0.0, 20.0);
        glVertex3f(20.0, 0.0, 20.0);
        glNormal3f(20.0, 40.0, 20.0);
        glVertex3f(20.0, 40.0, 20.0);
        glNormal3f(10.0, 40.0, 20.0);
        glVertex3f(10.0, 40.0, 20.0);
        glNormal3f(-20.0, 10.0, 20.0);
        glVertex3f(-20.0, 10.0, 20.0);
    glEnd();

    //Penutup samping
    glBegin(GL_QUADS);
        glNormal3f(10.0, 40.0, 20.0);
        glVertex3f(10.0, 40.0, 20.0);
        glNormal3f(10.0, 40.0, -20.0);
        glVertex3f(10.0, 40.0, -20.0);
        glNormal3f(-20.0, 10.0, -20.0);
        glVertex3f(-20.0, 10.0, -20.0);
        glNormal3f(-20.0, 10.0, 20.0);
        glVertex3f(-20.0, 10.0, 20.0);
    glEnd();

    // Penutup kiri
    glBegin(GL_QUADS);
        glNormal3f(-20.0, 10.0, 20.0);
        glVertex3f(-20.0, 10.0, 20.0);
        glNormal3f(-20.0, 10.0, -20.0);
        glVertex3f(-20.0, 10.0, -20.0);
        glNormal3f(-20.0, 0.0, -20.0);
        glVertex3f(-20.0, 0.0, -20.0);
        glNormal3f(-20.0, 0.0, 20.0);
        glVertex3f(-20.0, 0.0, 20.0);
    glEnd();

    //Penutup Bawah
    glBegin(GL_QUADS);
        glNormal3f(-20.0, 0.0, -20.0);
        glVertex3f(-20.0, 0.0, -20.0);
        glNormal3f(-20.0, 0.0, 20.0);
        glVertex3f(-20.0, 0.0, 20.0);
        glNormal3f(20.0, 0.0, 20.0);
        glVertex3f(20.0, 0.0, 20.0);
        glNormal3f(20.0, 0.0, -20.0);
        glVertex3f(20.0, 0.0, -20.0);
    glEnd();

    //Penutup Kanan
    glBegin(GL_QUADS);
        glNormal3f(20.0, 0.0, 20.0);
        glVertex3f(20.0, 0.0, 20.0);
        glNormal3f(20.0, 0.0, -20.0);
        glVertex3f(20.0, 0.0, -20.0);
        glNormal3f(20.0, 40.0, -20.0);
        glVertex3f(20.0, 40.0, -20.0);
        glNormal3f(20.0, 40.0, 20.0);
        glVertex3f(20.0, 40.0, 20.0);
    glEnd();

    //Penutup Atas
    glBegin(GL_QUADS);
        glNormal3f(20.0, 40.0, -20.0);
        glVertex3f(20.0, 40.0, -20.0);
        glNormal3f(20.0, 40.0, 20.0);
        glVertex3f(20.0, 40.0, 20.0);
        glNormal3f(10.0, 40.0, 20.0);
        glVertex3f(10.0, 40.0, 20.0);
        glNormal3f(10.0, 40.0, -20.0);
        glVertex3f(10.0, 40.0, -20.0);
    glEnd();
}

void mosque(){
    glPushMatrix();
        balok(70, 50, 70, stoneTexture, 50);
        cyl(0, 20, 0, 20, 20, 20, 8, 50,stoneTexture, 1, 45);
        glPushMatrix();
            glTranslatef(0,40,0);
            glutSolidSphere(19,20,20);
        glPopMatrix();
        glPushMatrix();
            cyl(-30, -25, -30, 70, 10, 10, 360, 50,stoneTexture, 1, 45);
            cyl(-30, 45, -30, 20, 12, 0, 360, 50,roofTexture, 1, 45);
        glPopMatrix();
        glPushMatrix();
            cyl(-30, -25, 30, 70, 10, 10, 360, 50,stoneTexture, 1, 45);
            cyl(-30, 45, 30, 20, 12, 0, 360, 50,roofTexture, 1, 45);
        glPopMatrix();
        glPushMatrix();
            cyl(30, -25, 30, 70, 10, 10, 360, 50,stoneTexture, 1, 45);
            cyl(30, 45, 30, 20, 12, 0, 360, 50,roofTexture, 1, 45);
        glPopMatrix();
        glPushMatrix();
            cyl(30, -25, -30, 70, 10, 10, 360, 50,stoneTexture, 1, 45);
            cyl(30, 45, -30, 20, 12, 0, 360, 50,roofTexture, 1, 45);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, -8.0, 25.0);
            glScalef(1.0,1.0,1.0);
            cube(35, stoneTexture, 50);
            glPushMatrix();
                glTranslatef(0,0,0.1);
                bidang(35,35,35,doorTexture);
            glPopMatrix();
        glPopMatrix();

    glPopMatrix();
}

void pyramid(){
    setmaterialPyramid();
    glPushMatrix();
    glTranslatef(0.0, -15.0, 0.0);
    for(int sta=25; sta>=0; sta--){
        glPushMatrix();
        glTranslatef(0.0, (25-sta)*15.0, 0.0);
            balok(25*sta, 20, 25*sta, stoneTexture, 100);
        glPopMatrix();
    }
    glPopMatrix();
}

void atap(){
    setmaterialRumah();
    cyl(0,10,0,0,25,35.35,4, 100, stoneTexture, 1, 90);
    cyl(0,0,0,10,35.35,35.35,4, 100, stoneTexture, 1, 90);
    cyl(0,0,0,10,25,25,4, 100, stoneTexture, 1, 90);
    cyl(0,0,0,0,25,35.35,4, 100, stoneTexture, 1, 90);
}

void rumah(){
    setmaterialRumah();
    glPushMatrix();
        glTranslatef(0.0,25.0,0.0);
        glRotatef(45,0,1,0);
        atap();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        //glScalef(50.0,50.0,50.0);
        cube(50, stoneTexture, 50);
    glPopMatrix();
}

void rumahA(){
    setmaterialRumah();
    glPushMatrix();
        glTranslatef(0.0, 40.0, 0.0);
        glScalef(0.85, 0.85, 0.85);
        glRotatef(45.0 ,0.0, 1.0, 0.0);
        atap();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, 10.0, 0.0);
        glScalef(1.0,1.2,1.0);
        cube(50, stoneTexture, 50);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, -5.0, 22.0);
        glScalef(1.0,1.0,1.0);
        balok(20, 30, 20, stoneTexture, 50);
        glPushMatrix();
            glTranslatef(0,0,0.1);
            bidang(20,30,20,doorTexture);
        glPopMatrix();
    glPopMatrix();
}

void rumahB(){
    glPushMatrix();
        glTranslatef(-28.0, 5.0, 0.0);
        //glScalef(50.0,50.0,50.0);
        rumah();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(29.0, 12.0, 0.0);
        glScalef(1.3,1.3,1.0);
        rumah();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(29.0, -3.0, 19.0);
        glScalef(1.0,1.0,1.0);
        cube(35, stoneTexture, 50);
        glPushMatrix();
            glTranslatef(0,0,0.1);
            bidang(35,35,35,doorTexture);
        glPopMatrix();
    glPopMatrix();

}

void rumahC(){
    setmaterialRumah();
    glPushMatrix();
        glTranslatef(0.0, 30.0, 0.0);
        glRotatef(45,0,1,0);
        glScalef(1.2,1.2,1.2);
        atap();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, 5.0, 0.0);
        glScalef(1.0,1.0,1.0);
        cube(50, stoneTexture, 50);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, -5.0, 22.0);
        glScalef(1.0,1.0,1.0);
        balok(20, 30, 20, stoneTexture, 50);
        glPushMatrix();
            glTranslatef(0,0,0.1);
            bidang(20,30,20,doorTexture);
        glPopMatrix();
    glPopMatrix();
}

void millFan(){
    glPushMatrix();
        glRotatef(theta, 0, 0, 1);
        glPushMatrix();
            glRotatef(-90,1,0,0);
            cyl(0,0,0,50,15,10,300,1,-1,1,300);
        glPopMatrix();
        glutSolidTorus(10,20,10,10);
        glPushMatrix();
            glScalef(1,1,0.2);
            glRotatef(45, 0, 1, 0);
            cyl(0,0,0,150,10,70,4,1,-1,1,90);
        glPopMatrix();
        glPushMatrix();
            glScalef(1,1,0.2);
            glRotatef(90, 0, 0, 1);
            glRotatef(45, 0, 1, 0);
            cyl(0,0,0,150,10,70,4,1,-1,1,90);
        glPopMatrix();
        glPushMatrix();
            glScalef(1,1,0.2);
            glRotatef(180, 0, 0, 1);
            glRotatef(45, 0, 1, 0);
            cyl(0,0,0,150,10,70,4,1,-1,1,90);
        glPopMatrix();
        glPushMatrix();
            glScalef(1,1,0.2);
            glRotatef(-90, 0, 0, 1);
            glRotatef(45, 0, 1, 0);
            cyl(0,0,0,150,10,70,4,1,-1,1,90);
        glPopMatrix();
    glPopMatrix();
}

void mill(){
    setmaterialMill();
    glPushMatrix();
        glPushMatrix();
            glRotatef(45, 0,1,0);
            cyl(0,10,0,90,40,20,4,1,-1,1,90);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,100,0);
            cube(45,-1,1);
        glPopMatrix();
        balok(70,20,70,-1,1);
        glPushMatrix();
            glTranslatef(0,100,33);
            glScalef(0.4,0.4,0.4);
            millFan();
        glPopMatrix();
    glPopMatrix();
}

void cactus() {
	float vertices[][3] = {
		{0.000000, -0.746330, -0.746330},
		{0.000000, 6.269746, -0.746330},
		{0.527735, -0.746330, -0.527735},
		{0.527735, 6.269746, -0.527735},
		{0.746330, -0.746330, 0.000000},
		{0.746330, 6.269746, 0.000000},
		{0.527735, -0.746330, 0.527735},
		{0.527735, 6.269746, 0.527735},
		{-0.000000, -0.746330, 0.746330},
		{-0.000000, 6.269746, 0.746330},
		{-0.527735, -0.746330, 0.527735},
		{-0.527735, 6.269746, 0.527735},
		{-0.746330, -0.746330, -0.000000},
		{-0.746330, 6.269746, -0.000000},
		{-0.527735, -0.746330, -0.527735},
		{-0.527735, 6.269746, -0.527735},
		{0.000000, 2.099079, -0.746330},
		{0.527735, 2.099079, -0.527735},
		{0.746330, 2.099079, 0.000000},
		{0.527735, 2.099079, 0.527735},
		{-0.000000, 1.745521, 0.746330},
		{-0.527735, 1.745521, 0.527735},
		{-0.746330, 2.099079, -0.000000},
		{-0.527735, 2.099079, -0.527735},
		{0.527735, 1.144113, -0.527735},
		{0.746330, 1.144113, 0.000000},
		{0.527735, 1.144113, 0.527735},
		{-0.000000, 1.144113, 0.746330},
		{-0.527735, 1.144113, 0.527735},
		{-0.746330, 1.144113, -0.000000},
		{-0.527735, 1.144113, -0.527735},
		{0.000000, 1.144113, -0.746330},
		{1.953517, 2.099079, -1.426851},
		{2.172112, 2.099079, -0.899116},
		{1.953517, 1.144113, -1.426851},
		{2.172112, 1.144113, -0.899116},
		{1.953517, 2.099079, -1.426851},
		{2.172112, 2.099079, -0.899116},
		{1.953517, 1.144113, -1.426851},
		{2.172112, 1.144113, -0.899116},
		{2.769988, 2.099079, -1.765045},
		{2.988583, 2.099079, -1.237310},
		{2.484902, 1.571742, -1.765045},
		{2.703497, 1.571742, -1.237310},
		{1.953517, 3.817296, -1.426851},
		{2.172112, 3.817296, -0.899116},
		{2.769988, 3.817296, -1.765045},
		{2.988583, 3.817296, -1.237310},
		{2.356472, 4.381490, -1.353062},
		{2.404867, 4.381490, -1.236226},
		{2.537232, 4.381490, -1.427935},
		{2.585628, 4.381490, -1.311099},
		{0.109610, 6.617173, -0.380744},
		{0.378837, 6.617173, -0.269226},
		{0.490354, 6.617173, 0.000000},
		{0.378837, 6.617173, 0.269226},
		{0.109610, 6.617173, 0.380744},
		{-0.159616, 6.617173, 0.269226},
		{-0.271134, 6.617173, -0.000000},
		{-0.159616, 6.617173, -0.269226},
		{0.666772, 6.443460, 0.000000},
		{0.487531, 6.443460, 0.432726},
		{-0.377921, 6.443460, 0.432726},
		{-0.557162, 6.443460, -0.000000},
		{0.054805, 6.443460, 0.611967},
		{-0.377921, 6.443460, -0.432726},
		{0.487531, 6.443460, -0.432726},
		{0.054805, 6.443460, -0.611967},
		{-0.343179, 1.745521, 1.574837},
		{-0.870914, 1.745521, 1.356242},
		{-0.343179, 1.144113, 1.574837},
		{-0.870914, 1.144113, 1.356242},
		{-0.506941, 2.099079, 1.970193},
		{-1.034676, 2.099079, 1.751598},
		{-0.506941, 1.144113, 1.970193},
		{-1.034676, 1.144113, 1.751598},
		{-0.343179, 2.865063, 1.574837},
		{-0.870914, 2.865063, 1.356242},
		{-0.506941, 2.865063, 1.970193},
		{-1.034676, 2.865063, 1.751598},
		{-0.579537, 3.014929, 1.635255},
		{-0.746505, 3.014929, 1.566094},
		{-0.631349, 3.014929, 1.760340},
		{-0.798317, 3.014929, 1.691180},
	};
	float tex_coords[][3] = {
	};
	float normals[][3] = {
		{0.9239, 0.0000, -0.3827},
		{0.3827, 0.0000, 0.9239},
		{-0.9239, 0.0000, 0.3827},
		{-0.3827, 0.0000, -0.9239},
		{0.0000, -1.0000, -0.0000},
		{0.5334, 0.0000, 0.8459},
		{0.0000, 0.0000, 1.0000},
		{-0.5334, 0.0000, -0.8459},
		{0.0000, 1.0000, 0.0000},
		{0.4694, -0.0903, 0.8783},
		{0.8265, -0.4468, -0.3424},
		{0.5243, -0.8233, -0.2172},
		{-0.4694, 0.0903, -0.8783},
		{-0.3560, 0.3667, -0.8595},
		{0.7888, 0.5206, -0.3267},
		{0.3560, 0.3667, 0.8595},
		{-0.7888, 0.5206, 0.3267},
		{-0.5075, 0.8356, 0.2102},
		{0.2563, 0.7427, 0.6187},
		{0.6737, 0.6842, -0.2791},
		{-0.2277, 0.8037, -0.5498},
		{-0.2937, 0.6411, -0.7090},
		{0.8508, 0.3897, -0.3524},
		{0.3290, 0.5106, 0.7944},
		{-0.6513, 0.7092, 0.2698},
		{0.9239, 0.0000, 0.3827},
		{-0.9239, 0.0000, -0.3827},
		{-0.3827, 0.0000, 0.9239},
		{-0.5625, 0.7933, -0.2330},
		{0.3827, 0.0000, -0.9239},
		{-0.2739, 0.6985, 0.6612},
		{0.5625, 0.7933, 0.2330},
		{0.2739, 0.6985, -0.6612},
		{0.6737, 0.6842, 0.2791},
		{-0.5075, 0.8356, -0.2102},
		{-0.2277, 0.8037, 0.5498},
		{0.2563, 0.7427, -0.6187},
		{0.3290, 0.5106, -0.7944},
		{-0.2937, 0.6411, 0.7090},
		{-0.6513, 0.7092, -0.2698},
		{0.8508, 0.3897, 0.3524},
	};
	int faces[][8][3] = {
		{{18, NULL, 1}, {4, NULL, 1}, {6, NULL, 1}, {19, NULL, 1}, },
		{{20, NULL, 2}, {8, NULL, 2}, {10, NULL, 2}, {21, NULL, 2}, },
		{{22, NULL, 3}, {12, NULL, 3}, {14, NULL, 3}, {23, NULL, 3}, },
		{{24, NULL, 4}, {16, NULL, 4}, {2, NULL, 4}, {17, NULL, 4}, },
		{{1, NULL, 5}, {3, NULL, 5}, {5, NULL, 5}, {7, NULL, 5}, {9, NULL, 5}, {11, NULL, 5}, {13, NULL, 5}, {15, NULL, 5}, },
		{{31, NULL, 4}, {24, NULL, 4}, {17, NULL, 4}, {32, NULL, 4}, },
		{{29, NULL, 3}, {22, NULL, 3}, {23, NULL, 3}, {30, NULL, 3}, },
		{{29, NULL, 5}, {28, NULL, 5}, {71, NULL, 5}, {72, NULL, 5}, },
		{{27, NULL, 2}, {20, NULL, 2}, {21, NULL, 2}, {28, NULL, 2}, },
		{{19, NULL, 6}, {26, NULL, 6}, {36, NULL, 6}, {34, NULL, 6}, },
		{{3, NULL, 1}, {25, NULL, 1}, {26, NULL, 1}, {5, NULL, 1}, },
		{{7, NULL, 2}, {27, NULL, 2}, {28, NULL, 2}, {9, NULL, 2}, },
		{{11, NULL, 3}, {29, NULL, 3}, {30, NULL, 3}, {13, NULL, 3}, },
		{{15, NULL, 4}, {31, NULL, 4}, {32, NULL, 4}, {1, NULL, 4}, },
		{{36, NULL, 7}, {35, NULL, 7}, {39, NULL, 7}, {40, NULL, 7}, },
		{{25, NULL, 8}, {18, NULL, 8}, {33, NULL, 8}, {35, NULL, 8}, },
		{{18, NULL, 9}, {19, NULL, 9}, {34, NULL, 9}, {33, NULL, 9}, },
		{{26, NULL, 5}, {25, NULL, 5}, {35, NULL, 5}, {36, NULL, 5}, },
		{{38, NULL, 10}, {40, NULL, 10}, {44, NULL, 10}, {42, NULL, 10}, },
		{{35, NULL, 7}, {33, NULL, 7}, {37, NULL, 7}, {39, NULL, 7}, },
		{{34, NULL, 7}, {36, NULL, 7}, {40, NULL, 7}, {38, NULL, 7}, },
		{{33, NULL, 7}, {34, NULL, 7}, {38, NULL, 7}, {37, NULL, 7}, },
		{{43, NULL, 11}, {41, NULL, 11}, {42, NULL, 11}, {44, NULL, 11}, },
		{{42, NULL, 1}, {41, NULL, 1}, {47, NULL, 1}, {48, NULL, 1}, },
		{{40, NULL, 12}, {39, NULL, 12}, {43, NULL, 12}, {44, NULL, 12}, },
		{{39, NULL, 13}, {37, NULL, 13}, {41, NULL, 13}, {43, NULL, 13}, },
		{{47, NULL, 14}, {45, NULL, 14}, {49, NULL, 14}, {51, NULL, 14}, },
		{{38, NULL, 2}, {42, NULL, 2}, {48, NULL, 2}, {46, NULL, 2}, },
		{{37, NULL, 3}, {38, NULL, 3}, {46, NULL, 3}, {45, NULL, 3}, },
		{{41, NULL, 4}, {37, NULL, 4}, {45, NULL, 4}, {47, NULL, 4}, },
		{{49, NULL, 9}, {50, NULL, 9}, {52, NULL, 9}, {51, NULL, 9}, },
		{{48, NULL, 15}, {47, NULL, 15}, {51, NULL, 15}, {52, NULL, 15}, },
		{{46, NULL, 16}, {48, NULL, 16}, {52, NULL, 16}, {50, NULL, 16}, },
		{{45, NULL, 17}, {46, NULL, 17}, {50, NULL, 17}, {49, NULL, 17}, },
		{{64, NULL, 18}, {63, NULL, 18}, {58, NULL, 18}, {59, NULL, 18}, },
		{{65, NULL, 19}, {62, NULL, 19}, {56, NULL, 19}, {57, NULL, 19}, },
		{{61, NULL, 20}, {67, NULL, 20}, {54, NULL, 20}, {55, NULL, 20}, },
		{{68, NULL, 21}, {66, NULL, 21}, {60, NULL, 21}, {53, NULL, 21}, },
		{{2, NULL, 22}, {16, NULL, 22}, {66, NULL, 22}, {68, NULL, 22}, },
		{{6, NULL, 23}, {4, NULL, 23}, {67, NULL, 23}, {61, NULL, 23}, },
		{{10, NULL, 24}, {8, NULL, 24}, {62, NULL, 24}, {65, NULL, 24}, },
		{{14, NULL, 25}, {12, NULL, 25}, {63, NULL, 25}, {64, NULL, 25}, },
		{{71, NULL, 26}, {69, NULL, 26}, {73, NULL, 26}, {75, NULL, 26}, },
		{{21, NULL, 9}, {22, NULL, 9}, {70, NULL, 9}, {69, NULL, 9}, },
		{{22, NULL, 27}, {29, NULL, 27}, {72, NULL, 27}, {70, NULL, 27}, },
		{{28, NULL, 26}, {21, NULL, 26}, {69, NULL, 26}, {71, NULL, 26}, },
		{{75, NULL, 28}, {73, NULL, 28}, {74, NULL, 28}, {76, NULL, 28}, },
		{{70, NULL, 27}, {72, NULL, 27}, {76, NULL, 27}, {74, NULL, 27}, },
		{{73, NULL, 26}, {69, NULL, 26}, {77, NULL, 26}, {79, NULL, 26}, },
		{{72, NULL, 5}, {71, NULL, 5}, {75, NULL, 5}, {76, NULL, 5}, },
		{{78, NULL, 29}, {80, NULL, 29}, {84, NULL, 29}, {82, NULL, 29}, },
		{{74, NULL, 28}, {73, NULL, 28}, {79, NULL, 28}, {80, NULL, 28}, },
		{{70, NULL, 27}, {74, NULL, 27}, {80, NULL, 27}, {78, NULL, 27}, },
		{{69, NULL, 30}, {70, NULL, 30}, {78, NULL, 30}, {77, NULL, 30}, },
		{{81, NULL, 9}, {82, NULL, 9}, {84, NULL, 9}, {83, NULL, 9}, },
		{{80, NULL, 31}, {79, NULL, 31}, {83, NULL, 31}, {84, NULL, 31}, },
		{{79, NULL, 32}, {77, NULL, 32}, {81, NULL, 32}, {83, NULL, 32}, },
		{{77, NULL, 33}, {78, NULL, 33}, {82, NULL, 33}, {81, NULL, 33}, },
		{{17, NULL, 30}, {2, NULL, 30}, {4, NULL, 30}, {18, NULL, 30}, },
		{{19, NULL, 26}, {6, NULL, 26}, {8, NULL, 26}, {20, NULL, 26}, },
		{{21, NULL, 28}, {10, NULL, 28}, {12, NULL, 28}, {22, NULL, 28}, },
		{{62, NULL, 34}, {61, NULL, 34}, {55, NULL, 34}, {56, NULL, 34}, },
		{{23, NULL, 27}, {14, NULL, 27}, {16, NULL, 27}, {24, NULL, 27}, },
		{{30, NULL, 27}, {23, NULL, 27}, {24, NULL, 27}, {31, NULL, 27}, },
		{{26, NULL, 26}, {19, NULL, 26}, {20, NULL, 26}, {27, NULL, 26}, },
		{{32, NULL, 30}, {17, NULL, 30}, {18, NULL, 30}, {25, NULL, 30}, },
		{{1, NULL, 30}, {32, NULL, 30}, {25, NULL, 30}, {3, NULL, 30}, },
		{{5, NULL, 26}, {26, NULL, 26}, {27, NULL, 26}, {7, NULL, 26}, },
		{{9, NULL, 28}, {28, NULL, 28}, {29, NULL, 28}, {11, NULL, 28}, },
		{{13, NULL, 27}, {30, NULL, 27}, {31, NULL, 27}, {15, NULL, 27}, },
		{{54, NULL, 9}, {53, NULL, 9}, {60, NULL, 9}, {59, NULL, 9}, {58, NULL, 9}, {57, NULL, 9}, {56, NULL, 9}, {55, NULL, 9}, },
		{{66, NULL, 35}, {64, NULL, 35}, {59, NULL, 35}, {60, NULL, 35}, },
		{{63, NULL, 36}, {65, NULL, 36}, {57, NULL, 36}, {58, NULL, 36}, },
		{{67, NULL, 37}, {68, NULL, 37}, {53, NULL, 37}, {54, NULL, 37}, },
		{{4, NULL, 38}, {2, NULL, 38}, {68, NULL, 38}, {67, NULL, 38}, },
		{{12, NULL, 39}, {10, NULL, 39}, {65, NULL, 39}, {63, NULL, 39}, },
		{{16, NULL, 40}, {14, NULL, 40}, {64, NULL, 40}, {66, NULL, 40}, },
		{{8, NULL, 41}, {6, NULL, 41}, {61, NULL, 41}, {62, NULL, 41}, },
	};

	for (int i = 0; i < (sizeof(faces))/ sizeof(faces[0]); ++i) {
		glBegin(GL_POLYGON);
		for (int j = 0; j < (sizeof(faces[0]))/ sizeof(faces[0][0]); ++j) {
			if (faces[i][j][0] != NULL) {
				glVertex3fv(vertices[faces[i][j][0] - 1]);
				if (faces[i][j][1] != NULL) {
					if(tex_coords[faces[i][j][1] - 1][2] == NULL)
						glTexCoord2f(tex_coords[faces[i][j][1] - 1][0], tex_coords[faces[i][j][1] - 1][1]);
					else
						glTexCoord3fv(tex_coords[faces[i][j][1] - 1]);
				}
				if (faces[i][j][2] != NULL) {
					glNormal3fv(normals[faces[i][j][2] - 1]);
				}
			}
		}
		glEnd();
	}
}

float LightPosition[] = { 10.0f, 50.0f, 50.0f };

void setlight(){
    float LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float LightEmission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float LightDiffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    float LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //diganti dengan camera
	//gluLookAt(viewer[0],viewer[1],viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    camera->Update();

    setlight();

    setmaterialSand();
    floor();

    glPushMatrix();
        glTranslatef(500.0,0.0,500.0);
        rumah();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(350.0,0.0,350.0);
        rumahA();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(250.0,0.0,350.0);
        rumahB();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(700.0,0.0,600.0);
        rumahC();
    glPopMatrix();

    setmaterialCactus();
    glPushMatrix();
        glTranslatef(320,-15,0);
        glScalef(5,7,5);
        cactus();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 0.0, -350.0);
        pyramid();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-350,-10,350);
        mill();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0,0,0);
        mosque();
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

//    if (w <= h)
//	glOrtho(-400.0, 400.0, -400.0*(GLfloat)h/(GLfloat)w,
//	    400.0*(GLfloat)h/(GLfloat)w, -400.0, 400.0);
//    else
//	glOrtho(-400.0*(GLfloat)w/(GLfloat)h,
//	    400.0*(GLfloat)w/(GLfloat)h, -400.0, 400.0, -400.0, 400.0);

    gluPerspective(45, (GLfloat)w / (GLfloat)h, 1.0, 5000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void keys(unsigned char key, int x, int y)
{

///* Use x, X, y, Y, z, and Z keys to move viewer */
//
//   if(key == 'x') viewer[0]-= 1.0;
//   if(key == 'X') viewer[0]+= 1.0;
//   if(key == 'y') viewer[1]-= 1.0;
//   if(key == 'Y') viewer[1]+= 1.0;
//   if(key == 'z') viewer[2]-= 1.0;
//   if(key == 'Z') viewer[2]+= 1.0;

switch( key )
    {
    case 27:
    case 'q':
        exit(0);
        break;
    case 'a':
        camera->LookUp(10.0f);
        break;
    case 'z':
        camera->LookUp(-10.0f);
        break;
    case 's':
        camera->MoveUp(5.0f);
        break;
    case 'x':
        camera->MoveUp(-5.0f);
        break;
    case 'd':
        camera->Move(20.0f);
        break;
    case 'c':
        camera->Move(-20.0f);
        break;
    case '.':
        camera->Rotate(0.0f,0.1f,0.0f );
        break;
    case ',':
        camera->Rotate(0.0f,-0.1f,0.0f );
        break;
    case 'r':
        camera = new Camera( 0.0, 0.0, 750.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
        break;
    case 'o':
        camera->MoveSide(20.0f);
        break;
    case 'p':
        camera->MoveSide(-20.0f);
        break;
    default:
        break;
    }

   if(key == 'b' || key == 'B') spin = !spin;

   if(key == '-') speed-= 0.05;
   if(key == '=') speed+= 0.05;


   if(key == '1') LightPosition[0]-= 20.0f;
   if(key == '2') LightPosition[0]+= 20.0f;
   if(key == '3') LightPosition[1]-= 20.0f;
   if(key == '4') LightPosition[1]+= 20.0f;
   if(key == '5') LightPosition[2]-= 20.0f;
   if(key == '6') LightPosition[2]+= 20.0f;


   if(key == 'q' || key == 'Q') exit(0);

   display();
}


void specialkeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        camera->Rotate(0.0f,0.05f,0.0f );
        break;
    case GLUT_KEY_LEFT:
        camera->Rotate(0.0f,-0.05f,0.0f );
        break;
    case GLUT_KEY_UP:
        camera->Move( 5.0f );
        break;
    case GLUT_KEY_DOWN:
        camera->Move( -5.0f );
        break;
    case GLUT_KEY_PAGE_UP:
        camera->LookUp(20.0f);
        break;
    case GLUT_KEY_PAGE_DOWN:
        camera->LookUp(-20.0f);
        break;
    case GLUT_KEY_HOME:
        camera->MoveUp(20.0f);
        break;
    case GLUT_KEY_END:
        camera->MoveUp(-20.0f);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void idle()
{
    if(spin)
    {
        theta += speed; //theta = theta + speed
        if( theta > 360.0 ) theta -= 360.0;
    } else if(!spin&&theta!=0.0)
    {
        theta += speed;
        if( theta > 360.0 ) theta = 0.0;
    }
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc,argv);
    glutInitWindowSize(1200, 700);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Dessert Village");
    myinit();
	glutDisplayFunc(display);
    glutReshapeFunc(reshape);
	glutKeyboardFunc(keys);
	glutSpecialFunc(specialkeys);
	glutIdleFunc(idle);
	glutMainLoop();

    delete camera;
    return 0;
}
