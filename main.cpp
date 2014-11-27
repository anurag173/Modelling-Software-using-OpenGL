//#include "screencasts.h"
#include<stdlib.h>
#include<GL/glut.h>
#include "imageloader.h"

#include<math.h>

#include<stdio.h>

#define PI 3.1415926535898/*  Poor man's approximation of PI */

#define Cos(th) cos(PI/180*(th))/*  Macro for sin & cos in degrees */
#define Sin(th) sin(PI/180*(th))

#define DEF_D 1/*  D degrees of rotation */
 
/*  Globals */
double dim=4.0,Q=10; /* dimension of orthogonal box */
char *windowName="checkpoint1";
int windowWidth=900;
int windowHeight=900;
 
/*  Various global state */
int toggleAxes = 1;   /* toggle axes on and off */
int toggleValues = 1; /* toggle values on and off */
int toggleMode = 0;   /* projection mode */
int th = 0;   /* azimuth of view angle */
int ph = 0;   /* elevation of view angle */
int fov = 55; /* field of view for perspective */
int asp = 1;  /* aspect ratio */
int t=0; 
int objId = 9;      /* object to draw */
int cel=0;
int point=0;
int edit_flag=0;
int final=0;
int total=0;
float r=1.0,g=1.0,b=1.0;
float X=0,Y=0,Z=0;
float scaleX=1,scaleY=1,scaleZ=1;
float theta=0;
float phi=0;
float a[20][13];


//
typedef struct tagVECTOR										// A Structure To Hold A Single Vector ( NEW )
{
	float X, Y, Z;												// The Components Of The Vector ( NEW )
}
VECTOR;

VECTOR light;
	
VECTOR light_translate(float x,float y,float z, VECTOR l){
	l.X-=x;
	l.Y-=y;
	l.Z-=z;
	return l;
}

VECTOR light_scale(float x,float y,float z, VECTOR l){
	l.X/=x;
	l.Y/=y;
	l.Z/=z;
	return l;
}

VECTOR light_rotate(float th_x,float ph_y,VECTOR l){
	VECTOR t=l;
	l.X=Cos(ph_y)*t.X+Sin(ph_y)*Sin(th_x)*t.Y	+Cos(th_x)*Sin(ph_y)*t.Z;	
	l.Y=Cos(th_x)*t.Y	-Sin(th_x)*t.Z;
	l.Z=-Sin(ph_y)*t.Z	+Cos(ph_y)*Sin(th_x)*t.Y	+Cos(th_x)*Cos(ph_y)*t.Z;
	return l;
}


inline float DotProduct (VECTOR &V1, VECTOR &V2)				// Calculate The Angle Between The 2 Vectors ( NEW )
{
	return V1.X * V2.X + V1.Y * V2.Y + V1.Z * V2.Z;				// Return The Angle ( NEW )
}

inline float Magnitude (VECTOR &V)								// Calculate The Length Of The Vector ( NEW )
{
	return sqrtf (V.X * V.X + V.Y * V.Y + V.Z * V.Z);			// Return The Length Of The Vector ( NEW )
}

float cel_shading(VECTOR n,VECTOR l){
float cos_angle= DotProduct(n,l)/(Magnitude(n)*Magnitude(l));
if (cos_angle < 0.0f)
	cos_angle = 0.1f;		// Clamp The Value to 0 If Negative ( NEW )
//printf("%f",cos_angle);
//return (cos_angle*.8);
return(cos_angle);
//glColor3f(cos_angle,cos_angle,cos_angle);
}
//
/*
 * project()
 * ------
 * Sets the projection
 */
void project() 
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
 
  if (toggleMode) {
    /* perspective */
    gluPerspective(fov,asp,dim/4,4*dim);
  }
  else {
    /* orthogonal projection*/
    glOrtho(-dim*asp,+dim*asp, -dim,+dim, -dim,+dim);
  }
 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

 
/*
 * setEye()
 * ------
 * Set the eye position
 */
void setEye()
{
  if (toggleMode) {
    double Ex = -2*dim*(Sin(th))*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    /* camera/eye position, aim of camera lens, up-vector */
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
  }
  /*  Orthogonal - set world orientation */
  else {
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
  }
}

void read_file(){
FILE *fp;
	fp=fopen("data.txt","r");
	int i=0;
	fscanf(fp," %i ",&total);
	
	for(;i<total;i++){
	fscanf(fp," %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&(a[i][0]),&(a[i][1]),&(a[i][2]),&(a[i][3]),&(a[i][4]),&(a[i][5]),&(a[i][6]),&(a[i][7]),&(a[i][8]),&(a[i][9]),&(a[i][10]),&(a[i][11]),&(a[i][12]));
	}
	
	fclose(fp);
}

void write_file(){
FILE *fp;
	fp=fopen("data.txt","w");
	int i=0;
	fprintf(fp,"%i\n",total);
	
	for(;i<total;i++){
	fprintf(fp,"%f %f %f %f %f %f %f %f %f %f %f %f %f\n",(a[i][0]),(a[i][1]),(a[i][2]),(a[i][3]),(a[i][4]),(a[i][5]),(a[i][6]),(a[i][7]),(a[i][8]),(a[i][9]),(a[i][10]),(a[i][11]),(a[i][12]));
	}
	
	fclose(fp);


}
//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

GLuint _textureId[11]; //The id of the textur
GLUquadric *quad;

void texture(){

	//_textureId = new GLuint[2]; 
   //glGenTextures(2, textures);
	Image* image = loadBMP("point/1.bmp");
	_textureId[0] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/2.bmp");
	_textureId[1] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/3.bmp");
	_textureId[2] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/4.bmp");
	_textureId[3] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/5.bmp");
	_textureId[4] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/6.bmp");
	_textureId[5] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/7.bmp");
	_textureId[6] = loadTexture(image);
	delete image;
  
	 image = loadBMP("point/8.bmp");
	_textureId[7] = loadTexture(image);
	delete image;

 image = loadBMP("cel2.bmp");
	_textureId[8] = loadTexture(image);
	delete image;
  
	 image = loadBMP("ocean.bmp");
	_textureId[9] = loadTexture(image);
	delete image;
  
	 image = loadBMP("checker.bmp");
	_textureId[10] = loadTexture(image);
	delete image;
   
//	glEnable(GL_TEXTURE_2D);
	 /*texture*
  if(cel){
  Image* image = loadBMP("cel2.bmp");
	_textureId = loadTexture(image);
	delete image;
  
  }
  else if(t==1){
  Image* image = loadBMP("ocean.bmp");
	_textureId = loadTexture(image);
	delete image;
  }
  else if(t==2){
  Image* image = loadBMP("checker.bmp");
	_textureId = loadTexture(image);
	delete image;
  }
  else if(t==3){
  Image* image = loadBMP("earth.bmp");
	_textureId = loadTexture(image);
	delete image;
  }
 */	

}
void printtext(float x,float y,float z,char* s,int l){
	int i=0;glRasterPos3d(x,y,z);
	while(i!=l){	
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s[i]);
	i++;
	}
}
/*
 * drawAxes()
 * ------
 * Draw the axes
 */
void drawAxes() 
{
  if (toggleAxes) {
    /*  Length of axes */
    double len = 3.0;
    glColor3f(0.7,0.7,0.7);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(len,0,0);
    glVertex3d(0,0,0);
    glVertex3d(0,len,0);
    glVertex3d(0,0,0);
    glVertex3d(0,0,len);
    glEnd();
    /*  Label axes */
    glRasterPos3d(len,0,0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'X');
	
	glRasterPos3d(0,len,0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'Y');
	glRasterPos3d(0,0,len);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'Z');
  }
}
 void drawXYZ(){
 glColor3f(0.2,0.2,0.4);
    glBegin(GL_LINES);
    glVertex3d(X,Y,0);
    glVertex3d(X,Y,Z);
    glVertex3d(X,0,Z);
    glVertex3d(X,Y,Z);
    glVertex3d(0,Y,Z);
    glVertex3d(X,Y,Z);
    glEnd();
 
 }
/*
 *  drawValues()
 *  ------
 *  Draw the values in the lower left corner
 */
void drawValues()
{
  if (toggleValues) {
    glColor3f(0.8,0.8,0.8);
    //printf(5,5,"View Angle (th, ph) =(%d, %d)", th,ph);
    //printf(5,25,"Projection mode =(%s)", toggleMode?"Perspective":"Orthogonal");
  }
}
 
/*
 *  vertex
 *  ------
 *  Draw vertex in polar coordinates 
 */
int sketch_texture(float angle){
	int j=0;
			if(angle>=.8&&angle<=.9)j=6;
			else if(angle>=.9&&angle<=1)j=7;
			else if(angle>=.7&&angle<=.8)j=5;
			else if(angle>=.6&&angle<=.7)j=4;
			else if(angle>=.5&&angle<=.6)j=3;
			else if(angle>=.4&&angle<=.5)j=2;
			else if(angle>=.3&&angle<=.4)j=1;
			else if(angle>=0&&angle<=.3)j=0;
			//glColor3f(angle,angle,angle);
			return j;
}
void vertex(double th2,double ph2,float angle,int tex)
{
   double x = Sin(th2)*Cos(ph2);
   double y = Cos(th2)*Cos(ph2);
   double z =          Sin(ph2);
   double th= (atan(-z/x)+PI/2)/(2*PI);
   double ph= acos(-y)/PI;
   if(x<=0) th=th+0.5;
   
   if((!cel) && (!point)&&(tex))glTexCoord2f(th,ph);
   glVertex3d(x,y,z);
}
void sphere(int DEF,VECTOR l,int tex){

	 int ph2, th2;

	 
	VECTOR n;
	/*VECTOR l;
		l.X=-1;
		l.Y=2;
		l.Z=2;*/
		float angle=0;
		int texture_number=0;//to store which texture to apply

		for (ph2=-90;ph2<90;ph2+=DEF) {
		
			
			for (th2=0;th2<=360;th2+=DEF) { 

				double x = Sin(th2-DEF)*Cos(ph2)+Sin(th2-DEF)*Cos(ph2+DEF)+Sin(th2)*Cos(ph2)+Sin(th2)*Cos(ph2+DEF);
				double y = Cos(th2-DEF)*Cos(ph2)+Cos(th2-DEF)*Cos(ph2+DEF)+Cos(th2)*Cos(ph2)+Cos(th2)*Cos(ph2+DEF);
				double z = Sin(ph2)+Sin(ph2+DEF);
				glNormal3f(x,y,z);
				n.X=x;
				n.Y=y;
				n.Z=z;
				if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;



				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}
				
				glBegin(GL_QUAD_STRIP);
				

				
				
				th2=th2-DEF;
				if(cel)	glTexCoord2f(angle,0.5);
				else if(point)glTexCoord2f(0,0);
				vertex(th2,ph2,angle,tex);

				if(cel)	glTexCoord2f(angle,0.5);
				else if(point)glTexCoord2f(0,DEF);
				vertex(th2,ph2+DEF,angle,tex);

				th2=th2+DEF;
				if(cel)	glTexCoord2f(angle,0.5);
				else if(point)glTexCoord2f(DEF,0);
				vertex(th2,ph2,angle,tex);
				
				if(cel)	glTexCoord2f(angle,0.5);
				else if(point)glTexCoord2f(DEF,DEF);
				vertex(th2,ph2+DEF,angle,tex);


				glEnd();
			}
		
		}
		/*
		for (ph2=-90;ph2<90;ph2+=DEF) {
		
			
			for (th2=0;th2<=360;th2+=2*DEF) { 
				glBegin(GL_QUAD_STRIP);
				
				th2=th2-DEF;				
				vertex(th2,ph2);
				vertex(th2,ph2+DEF);
				th2=th2+DEF;
				vertex(th2,ph2);
				vertex(th2,ph2+DEF);


				glEnd();
			}
		
		}*/
	}
 void cylinder(int DEF,VECTOR l ,int tex){
	 double i,k,j;
	 /*
	  glBegin(GL_QUAD_STRIP);
		
		for (j=0;j<=360;j+=DEF) {
		//if(!t)glColor3f(r,g,b);
		
		glNormal3f(Cos(j)+Cos(j+1),0,Sin(j)+Sin(j+1));
		if(t)glTexCoord2f(j/360,0);
		glVertex3f(Cos(j),+1,Sin(j));
		if(t)glTexCoord2f(j/360,1);
		//glColor3f(r,g,b);
		glVertex3f(Cos(j),-1,Sin(j));
		
		}
    glEnd();
 */
	VECTOR n;
	
		float angle=0;
		int texture_number=0;//to store which texture to apply

	 for (j=0;j<=360;j+=DEF) {
		//if(!t)glColor3f(r,g,b);
			 j=j+DEF;

			n.X=Cos(j)+Cos(j-DEF);
			n.Y=0;
			n.Z=Sin(j)+Sin(j-DEF);
	
			if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		glBegin(GL_QUAD_STRIP);
		//glNormal3f(Cos(j)+Cos(j+DEF),0,Sin(j)+Sin(j+DEF));
		
		
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(tex)glTexCoord2f(j/360,1);
		glVertex3f(Cos(j),+1,Sin(j));
		
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(5,0);
		else if(tex)glTexCoord2f(j/360,0);
		glVertex3f(Cos(j),-1,Sin(j));

		j=j-DEF;

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(DEF,0);
		else if(tex)glTexCoord2f(j/360,1);
		glVertex3f(Cos(j),+1,Sin(j));

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(DEF,5);
		else if(tex)glTexCoord2f(j/360,0);
		//glColor3f(r,g,b);
		glVertex3f(Cos(j),-1,Sin(j));
		glEnd();
		}
    /* top and bottom circles */
    /* reuse the currentTexture on top and bottom) */
    
	 
	n.X=0;
	n.Y=1;
	n.Z=0;
	 
	l=light_translate(0,1,0,l);
	angle=cel_shading(n,l);
	  if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;



	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  }
	glPushMatrix();
	
		//glRotatef(180,1,0,0);
	glTranslatef(0,1,0);
	glBegin(GL_TRIANGLES);
    for (k=0;k<=360;k+=DEF) {
		glNormal3f(0.0,1,0.0);
    if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(t)glTexCoord2f((2*k+DEF)/720,1);
      glVertex3f(0,0,0);

		
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k+DEF),15*Sin(k+DEF));
		else if(t)glTexCoord2f((k+DEF)/360,1);
      glVertex3f(Cos(k+DEF),0,Sin(k+DEF));  
	  if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k),15*Sin(k));
		else if(t)glTexCoord2f(k/360,1);
      glVertex3f(Cos(k),0,Sin(k));
	}
	glEnd();
	glPopMatrix();


	n.X=0;
	n.Y=-1;
	n.Z=0;
	
	angle=cel_shading(n,l);
	if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;


		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	}	
	
	glPushMatrix();
	
		//glRotatef(180,1,0,0);
	glTranslatef(0,-1,0);
	glBegin(GL_TRIANGLES);
    for (k=0;k<=360;k+=DEF) {
	glNormal3f(0.0,-1,0.0);	
    if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(tex)glTexCoord2f((2*k+DEF)/720,1);
      glVertex3f(0,0,0);

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k),15*Sin(k));
		else if(tex)glTexCoord2f(k/360,1);
      glVertex3f(Cos(k),0,Sin(k));

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k+DEF),15*Sin(k+DEF));
		else if(tex)glTexCoord2f((k+DEF)/360,1);
      glVertex3f(Cos(k+DEF),0,Sin(k+DEF));
	}
	glEnd();
	glPopMatrix();
	
 }
 void cone(int DEF,VECTOR l ,int tex){

	 double k;
	 VECTOR n;			//to store normal vector
	 	int texture_number=0;//to store which texture to apply
	float angle=0;  //to find angle between normal and light vector
	/* sides */
    for (k=0;k<=360;k+=DEF){
			
			//////////normal	
			glNormal3f(Cos(k)+Cos(k+DEF),Sin(k)+Sin(k+DEF),1.0);
			n.X=Cos(k)+Cos(k+DEF);
			n.Y=Sin(k)+Sin(k+DEF);
			n.Z=1;
			//////////

			if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;



	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		////////////////////////////////
		glBegin(GL_TRIANGLES);
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(tex)glTexCoord2f((2*k+DEF)/720,1);
		glVertex3f(0,0,2);

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(5,0);
		else if(tex)glTexCoord2f(k/360,0);		
		glVertex3f(Cos(k),Sin(k),0);

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(5,5);
		else if(tex)glTexCoord2f((k+DEF)/360,0);		
		glVertex3f(Cos(k+DEF),Sin(k+DEF),0);

		glEnd();
		///////////////////////////////////		
	}
   
 /*
    /* bottom circle */ 
    /* rotate back */
	//l=light_rotate(270,0,l);
//	float angle=0;  //to find angle between normal and light vector
	
			
			//////////normal	
			glNormal3f(0,1,0);
			n.X=0;
			n.Y=0;
			n.Z=-1;
			//////////

			if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;


	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
	glPushMatrix();
    glRotated(270,1,0,0);
    glBegin(GL_TRIANGLES);
	glNormal3f(0.0,1.0,0.0);
    for (k=0;k<=360;k+=DEF) {
	  if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(tex)glTexCoord2f((2*k+DEF)/720,1);
      glVertex3f(0,0,0);

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k),15*Sin(k));
		else if(tex)glTexCoord2f(k/360,1);
      glVertex3f(Cos(k),0,Sin(k));

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k+DEF),15*Sin(k+DEF));
		else if(tex)glTexCoord2f((k+DEF)/360,1);
      glVertex3f(Cos(k+DEF),0,Sin(k+DEF));
    }
    glEnd();
	glPopMatrix();
 
 }
 void circle(int DEF,VECTOR l,int tex){
	 double k;
	  VECTOR n;			//to store normal vector
	 int texture_number=0;//to store which texture to apply
	float angle=0;  //to find angle between normal and light vector
	
			
			//////////normal	
			glNormal3f(0,1,0);
			n.X=0;
			n.Y=1;
			n.Z=0;
			//////////

			if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;



	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
	//glRotated(90,1,0,0);
    glBegin(GL_TRIANGLES);
	//glNormal3f(0.0,1.0,0.0);
    for (k=0;k<=360;k+=DEF) {
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(tex)glTexCoord2f((2*k+DEF)/720,1);
      glVertex3f(0,0,0);

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k),15*Sin(k));
		else if(tex)glTexCoord2f(k/360,1);
      glVertex3f(Cos(k),0,Sin(k));

		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(15*Cos(k+DEF),15*Sin(k+DEF));
		else if(tex)glTexCoord2f((k+DEF)/360,1);
      glVertex3f(Cos(k+DEF),0,Sin(k+DEF));
	  }
	glEnd();
 }
 void square(VECTOR l,int tex){

	 
	 VECTOR n;			//to store normal vector
	 int texture_number=0;//to store which texture to apply
	float angle=0;  //to find angle between normal and light vector
	
			
			//////////normal	
			glNormal3f(0,1,0);
			n.X=0;
			n.Y=1;
			n.Z=0;
			//////////

			if(cel||point||tex){
					angle=cel_shading(n,l);
					if(point)			texture_number=sketch_texture(angle);
					else if(cel)				texture_number=8;
					else if(tex)		texture_number=tex+8;


		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId[texture_number]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
	 //glRotated(90,1,0,0);
   glBegin(GL_QUAD_STRIP);
		
		
		//glColor3f(r,g,b);
   		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(2,0);
		else if(tex)glTexCoord2f(1,0);
		//glNormal3f(Cos(j)+Cos(j+1),0,Sin(j)+Sin(j+1));
		glVertex3f(-.5,0,.5);
		//glColor3f(r,g,b);
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,0);
		else if(tex)glTexCoord2f(0,0);
		glVertex3f(-.5,0,-.5);
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(2,2);
		else if(tex)glTexCoord2f(1,1);
		//glNormal3f(Cos(j)+Cos(j+1),0,Sin(j)+Sin(j+1));
		glVertex3f(.5,0,.5);
		//glColor3f(r,g,b);
		if(cel)	glTexCoord2f(angle,0.5);
		else if(point)glTexCoord2f(0,2);
		else if(tex)glTexCoord2f(0,1);
		glVertex3f(.5,0,-.5);
		//glVertex3f(.5,0,.5);
		
    glEnd();
 }

/*
 *  drawShape()
 *  ------
 *  Draw the GLUT shape
 */

 void shape(int obj,VECTOR l,int tex)
{
	//	glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, _textureId[1]);

	//Bottom
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	 double k;
  //if(obj == 0) //=> draw nothing
	 if(1){
	//glColor3f(r,g,b);
   //if (objId == 0)  fractal2D();
  if (obj == 10)  sphere(1,l,tex);			/* Sphere */
  else if (obj == 8)  cylinder(10,l,tex);	/* cylinder */
  else if (obj == 9)	cone(10,l,tex);		/* cone */
  else if (obj == 3)	circle(10,l,tex);		/* circle  */  
  else if (obj == 11){                 /*face + cap*/  
		
			//	sphere(1);	
		//		glTranslatef(0,0,.5);
	//			cone(10);
  }
  else if (obj == 4)		circle(72,l,tex);//pentagon
  else if (obj == 7)		sphere(60,l,tex);
  else if (obj == 6)		cylinder(90,l,tex); /**Cuboid**/  
  else if (obj == 5)		 cone(120,l,tex);  /*  PYRAMID  */  
  else if (obj == 2){
	  square(l,tex);/*square*/
  
		}
  else if (obj == 1)		circle(120,l,tex);/*Triangle*/  
  else if (obj == 12)     cylinder(120,l,tex);
 }
  
 }
  //printf("error in draw shape");
    void	outline(int obj,int flag)											// Check To See If We Want To Draw The Outline ( NEW )
	{   //int tex=0;
		glEnable (GL_CULL_FACE);
		//glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable (GL_BLEND);									// Enable Blending ( NEW )
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		// Set The Blend Mode ( NEW )

								// Draw Backfacing Polygons As Wireframes ( NEW )
		glLineWidth (2);								// Set The Line Width ( NEW )

		if(flag){glCullFace (GL_FRONT);		glPolygonMode (GL_BACK, GL_LINE);}							// Don't Draw Any Front-Facing Polygons ( NEW )
		else 
		{glCullFace (GL_BACK);		glPolygonMode (GL_FRONT, GL_LINE);}

		glDepthFunc (GL_LEQUAL);								// Change The Depth Mode ( NEW )

		glColor3f (0,0,0);							// Set The Outline Color ( NEW )

		if (obj == 10)  sphere(30,light,0);			/* Sphere */
  else if (obj == 8)  cylinder(10,light,0);	/* cylinder */
  else if (obj == 9)	cone(10,light,0);		/* cone */
  else if (obj == 3)	circle(10,light,0);		/* circle  */  
  else if (obj == 11){                 /*face + cap*/  
		
	  sphere(20,light,0);	
				glTranslatef(0,0,.5);
				cone(10,light,0);}
  else if (obj == 4)		circle(72,light,0);//pentagon
  else if (obj == 7)		sphere(60,light,0);
  else if (obj == 6)		cylinder(90,light,0); /**Cuboid**/  
  else if (obj == 5)		 cone(120,light,0);  /*  PYRAMID  */  
  else if (obj == 2){
	  square(light,0);/*square*/
  
		}
  else if (obj == 1)		circle(120,light,0);/*Triangle*/  
  else if (obj == 12)     cylinder(120,light,0);
		
		glDepthFunc (GL_LESS);									// Reset The Depth-Testing Mode ( NEW )
		if(flag){glCullFace (GL_BACK);		glPolygonMode (GL_BACK, GL_FILL);}							// Don't Draw Any Front-Facing Polygons ( NEW )
		else 
		{glCullFace (GL_BACK);		glPolygonMode (GL_FRONT, GL_FILL);}

		//glCullFace (GL_BACK);									// Reset The Face To Be Culled ( NEW )

		//glPolygonMode (GL_BACK, GL_FILL);						// Reset Back-Facing Polygon Drawing Mode ( NEW )

		glDisable (GL_BLEND);									// Disable Blending ( NEW )
	}

 
void drawShape()
{
	glClearColor(0.50f, 0.40f, 0.1f, 0.40f);
	//if(edit_flag)glClearColor(0.0f,0.0f,.8f,1.0f);
	/*******************aaaaa**********************/
 //Add ambient light
	if(0){
	GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 1.0f}; //Color (0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	//Add light at infinite distace
	GLfloat lightColor0[] = {1.0f, 1.0f, 0.2f, 0.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {-1.0f, 0.5f, 0.5f, 0.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//Add directed light
	GLfloat lightColor1[] = {0.4f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 2, 2)
	GLfloat lightPos1[] = {-1.0f, 2.0f, 2.0f, 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	
	glEnable(GL_SHININESS);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	glShadeModel(GL_SMOOTH); //Enable smooth shading
	
	}
	/*********************************************************************************/
	GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	GLfloat shininess[] = {50};
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	texture();
  /*****************************************/
	VECTOR templ;
		templ=light;	
	//shape(objId);
	//outline(objId);
	read_file();
	int i=0;
	drawXYZ();
	if(edit_flag){
		point=0,cel=0;
		glClearColor(0.0f,0.0f,.8f,1.0f);
		for(i=0;i<total;i++){
			
		glPushMatrix();		
		glTranslatef(a[i][1],a[i][2],a[i][3]);
		glScalef(a[i][4],a[i][5],a[i][6]);
		glRotatef(a[i][7],1,0,0);
		glRotatef(a[i][8],0,1,0);
		outline(a[i][0],0);
		glPopMatrix();

		}
		glColor3f(r,g,b);
		glPushMatrix();		
		glTranslatef(X,Y,Z);
		glScalef(scaleX,scaleY,scaleZ);
		glRotatef(theta,1,0,0);
		glRotatef(phi,0,1,0);
		shape(objId,light,t);
		//outline(objId);
		glPopMatrix();

		if(final){
			total=total+1;
			a[total-1][0]=objId;
			a[total-1][1]=X;
			a[total-1][2]=Y;
			a[total-1][3]=Z;
			a[total-1][4]=scaleX;
			a[total-1][5]=scaleY;
			a[total-1][6]=scaleZ;
			a[total-1][7]=theta;
			a[total-1][8]=phi;
			a[total-1][9]=r;
			a[total-1][10]=g;
			a[total-1][11]=b;
			a[total-1][12]=t;

			write_file();
			final=0;
			edit_flag=0;
		}
	}
	else{
		for(i=0;i<total;i++){
		glColor3f(a[i][9],a[i][10],a[i][11]);
		glPushMatrix();
		glTranslatef(a[i][1],a[i][2],a[i][3]);
		glScalef(a[i][4],a[i][5],a[i][6]);
		glRotatef(a[i][7],1,0,0);
		glRotatef(a[i][8],0,1,0);
		templ=light_rotate(-a[i][7],-a[i][8],templ);
		templ=light_scale(a[i][4],a[i][5],a[i][6],templ);
		//templ=light_translate(a[i][1],a[i][2],a[i][3],templ);
		shape(a[i][0],templ,a[i][12]);
		templ=light;
		outline(a[i][0],1);
		glPopMatrix();
		}
	}
	/*
	for(i=0;i<total;i++){
		glPushMatrix();
		glTranslatef(a[i][1],a[i][2],a[i][3]);
		//shape(a[i][0]);

		glPopMatrix();
	}
	for(i=0;i<total;i++){
		glPushMatrix();
		glTranslatef(a[i][1],a[i][2],a[i][3]);
		//outline(a[i][0]);

		glPopMatrix();
	}*/
}	

 
/*
 *  display()
 *  ------
 *  Display the scene
 */
void display()
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  	glEnable(GL_COLOR_MATERIAL);
//following are disabled for cel shading
	if(0){
	glEnable(GL_SHININESS);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	glShadeModel(GL_SMOOTH); //Enable smooth shading
	}
  glLoadIdentity();
  
 
  /* setup functions */
  setEye();

  /*texture*
  if(cel){
  Image* image = loadBMP("cel3.bmp");
	_textureId = loadTexture(image);
	delete image;
  
  }
  else if(t==1){
  Image* image = loadBMP("ocean.bmp");
	_textureId = loadTexture(image);
	delete image;
  }
  else if(t==2){
  Image* image = loadBMP("checker.bmp");
	_textureId = loadTexture(image);
	delete image;
  }
  else if(t==3){
  Image* image = loadBMP("earth.bmp");
	_textureId = loadTexture(image);
	delete image;
  }*/
  /* draw */
  drawAxes();
  drawValues();
 
  /* magic here */
  drawShape();

 
  /*  Flush and swap */
  glFlush();
  glutSwapBuffers();
}
 
/*
 *  reshape()
 *  ------
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
  asp = (height>0) ? (double)width/height : 1;
  glViewport(0,0, width,height);
  project();
}
 
/*
 *  windowKey()
 *  ------
 *  GLUT calls this routine when a non-special key is pressed
 */
void windowKey(unsigned char key,int x,int y)
{
  /*  Exit on ESC */
  if (key == 27) exit(0);
  /*  Spacebar - Toggle through shapes */
  else if (key=='i'){edit_flag =1-edit_flag;
  printf(" %i",edit_flag);
  }
  else if ((key==13)&&(edit_flag))final =1-final;
  
  else if (key == 32) {
    if (objId == 12) objId = 0;
    else objId++;
  }
  else if (key == 'a' || key == 'A') toggleAxes = 1-toggleAxes;
  else if (key == 'c' || key == 'C') cel = 1-cel;
  else if (key == 'p' || key == 'P') point = 1-point;
  else if (key == 'm' || key == 'M') toggleMode = 1-toggleMode;
  /*  Change field of view angle */
  else if (key == '-' && key>1) fov--;
  else if (key == '+' && key<179) fov++;
  /*  Change dimensions */
  else if (key == 'D') dim += 0.1;
  else if (key == 'd' && dim>1) dim -= 0.1;
  
 else if (key == 'r'){if (r < 1.0) r=r+0.05; else r=0;}
 else if (key == 'g'){if (g < 1.0) g=g+0.05; else g=0;}
 else if (key == 'b'){if (b < 1.0) b=b+0.05; else b=0;}

 else if (key == '1'){if (X < 5.0) X=X+0.05; else X=0;}
 else if (key == '2'){if (Y < 5.0) Y=Y+0.05; else Y=0;}
 else if (key == '3'){if (Z < 5.0) Z=Z+0.05; else Z=0;}
 else if (key == '!'){if (X > -5.0) X=X-0.05; else X=0;}
 else if (key == '@'){if (Y > -5.0) Y=Y-0.05; else Y=0;}
 else if (key == '#'){if (Z > -5.0) Z=Z-0.05; else Z=0;}
 
 else if (key == '4'){if (scaleX < 5.0) scaleX=scaleX+0.05; else X=1;}
 else if (key == '5'){if (scaleY < 5.0) scaleY=scaleY+0.05; else scaleY=1;}
 else if (key == '6'){if (scaleZ < 5.0) scaleZ=scaleZ+0.05; else scaleZ=1;}
 else if (key == '$'){if (scaleX > 0.2) scaleX=scaleX-0.05; else scaleX=1;}
 else if (key == '%'){if (scaleY > 0.2) scaleY=scaleY-0.05; else scaleY=1;}
 else if (key == '^'){if (scaleZ > 0.2) scaleZ=scaleZ-0.05; else scaleZ=1;}

  else if (key == '7'){if (theta < 180) theta=theta+5; else theta=-180;}
 else if (key == '&'){if (theta >-180 ) theta=theta-5; else theta=180;}
 else if (key == '8'){if (phi < 180) phi=phi+5; else phi=-180;}
 else if (key == '*'){if (phi >-180 ) phi=phi-5; else phi=180;}

 else if (key == 't'){t++; t=t%3;}
  project();
  glutPostRedisplay();
}
 
/*
 *  windowSpecial()
 *  ------
 *  GLUT calls this routine when an arrow key is pressed
 */
void windowSpecial(int key,int x,int y)
{
  /*  Right arrow key - increase azimuth by 5 degrees */
  if (key == GLUT_KEY_RIGHT) th += 5;
  /*  Left arrow key - decrease azimuth by 5 degrees */
  else if (key == GLUT_KEY_LEFT) th -= 5;
  /*  Up arrow key - increase elevation by 5 degrees */
  else if (key == GLUT_KEY_UP) ph += 5;
  /*  Down arrow key - decrease elevation by 5 degrees */
  else if (key == GLUT_KEY_DOWN) ph -= 5;
 
  /*  Keep angles to +/-360 degrees */
  th %= 360;
  ph %= 360;
 
  project();
  glutPostRedisplay();
}
 
/*
 *  windowMenu
 *  ------
 *  Window menu is the same as the keyboard clicks
 */
void windowMenu(int value)
{
  windowKey((unsigned char)value, 0, 0);
}

void init() {
  GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
  GLfloat cyan[] = { 0.0, 1.0, 1.0, 1.0 };
  GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat direction[] = { 1.0, 1.0, 1.0, 0.0 };

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
  glMaterialfv(GL_FRONT, GL_SPECULAR, white);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);

  glLightfv(GL_LIGHT0, GL_AMBIENT, black);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  glLightfv(GL_LIGHT0, GL_POSITION, direction);

  glEnable(GL_LIGHTING);                // so the renderer considers light
  glEnable(GL_LIGHT0);                  // turn LIGHT0 on
  glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
}
 
/*
 *  main()
 *  ----
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
 {

	 light.X=-5;
		light.Y=4;
		light.Z=4;
  glutInit(&argc,argv);//Initialize glut
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//setting up memory for our display
  glutInitWindowSize(windowWidth,windowHeight);//setting window size
  glutCreateWindow(windowName);//putting window name
 //init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(windowKey);
  glutSpecialFunc(windowSpecial);
 
  glutCreateMenu(windowMenu); 
  glutAddMenuEntry("Load Next object",' ');
  glutAddMenuEntry("Toggle Axes [a]",'a');
  glutAddMenuEntry("Toggle Mode [m]",'m');
  glutAddMenuEntry("Toggle Texture [t]",'t');
  glutAttachMenu(GLUT_RIGHT_BUTTON);
 
  glutMainLoop();//stars up the loop that rins in background
  return 0;
}
