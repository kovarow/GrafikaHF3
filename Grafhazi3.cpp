//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Kõvári András
// Neptun : ER0PQF
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;

	Vector() {
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0) {
		x = x0; y = y0; z = z0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) { 	// dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) { 	// cross product
		return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	float Length() { return sqrt(x * x + y * y + z * z); }

	Vector normalize(){
		return Vector(x, y, z)*(1 / Length());
	}
	bool operator==(const Vector& v){
	
		if (fabs(x-v.x) < 0.0001&& fabs(y-v.y)<0.0001&& fabs(z-v.z)<0.0001 )
			return true;
		return false;
	}

	bool operator<(Vector v){
		if (Length() < v.Length())
			return true;
		return false;
	
	}

};



const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;
GLuint textId;
const int res = 256;
unsigned char texture[res*res][4];


// A billentyûzetkezeléshez és textúrázáshoz felhasználtam az itt olvasható anyagokat is: https://wiki.sch.bme.hu/Sz%C3%A1m%C3%ADt%C3%B3g%C3%A9pes_grafika_h%C3%A1zi_feladat_tutorial
enum keys{W,E,R,Space, keynum};
bool keydown[keynum];
float LightPosition[] = { 10.0, 5.0, 10.0, 0.0 };
bool shadow = false;

float white[4] = { 1, 1, 1, 1 };
float gray[4] = { 0.2f, 0.2f, 0.2f, 1 };
float black[4] = { 0, 0, 0, 1 };
float blue[4] = { 0.2f, 0.3f, 1, 1 };
float orange[4] = { 1.0f, 0.6, 0, 1 };







struct vertex;
struct face;
struct hedge {
	vertex* endVertex;
	hedge* pairHedge;
	face* borderedFace;
	hedge* nextHedge;
	vertex* startVertex;
	hedge(vertex* _startvertex=NULL, vertex* _endVertex=NULL, hedge* _pairHedge=NULL, face* _borderedFace=NULL,hedge* _nextHedge=NULL){
		startVertex = _startvertex;
		endVertex = _endVertex;
		pairHedge = _pairHedge;
		borderedFace = _borderedFace;
		nextHedge = _nextHedge;
	}
};
struct vertex {
	
	Vector point;
	hedge* oneEmanatingHedge;
	vertex(float _x=0, float _y=0, float _z=0, hedge* _oneEmanatingHedge=NULL){
	
		point.x = _x;
		point.y = _y;
		point.z = _z;
		oneEmanatingHedge = _oneEmanatingHedge;
	}
};
struct face {
	hedge* oneBorder;
	face(hedge* _oneBorder=NULL){
		oneBorder = oneBorder;
	}
};
struct edge{
	Vector a, b;
	edge(Vector _a=Vector(0,0,0), Vector _b=Vector(0,0,0)){
		a = _a;
		b = _b;
	}
	Vector getEdgePoint(){
		return a*0.5f + b*0.5f;
	}
};


class SubdivisionSurface{
public:
	bool isnottextured;
	hedge hedges[50];
	face faces[10];
	vertex vertices[20];
	int vertexNum;
	int faceNum;
	int hedgeNum;

	hedge secondhedges[500];
	face secondfaces[300];
	vertex secondvertices[200];
	int secondvertexNum;
	int secondfaceNum;
	int secondhedgeNum;



	hedge subhedges[100];
	face subfaces[50];
	vertex subvertices[50];
	int subvertexNum;
	int subfaceNum;
	int subhedgeNum;




	SubdivisionSurface(Vector *points=NULL, bool _isnottextured=false){
	
		vertexNum = 0;
		faceNum = 0;
		hedgeNum = 0;
		subvertexNum=0;
		subfaceNum=0;
		subhedgeNum=0;

		secondvertexNum=0;
		secondfaceNum=0;
		secondhedgeNum=0;

		
		isnottextured = _isnottextured;

		if (points != NULL){
			Vector pointsforFaceA[4];
			for (int i = 0; i < 4; i++){
				pointsforFaceA[i] = points[i];
			}
			addFace(pointsforFaceA, faces, &faceNum, vertices, &vertexNum, hedges, &hedgeNum);


			Vector pointsforFaceB[4];

			pointsforFaceB[0] = points[3];
			pointsforFaceB[1] = points[2];
			pointsforFaceB[2] = points[6];
			pointsforFaceB[3] = points[7];
			addFace(pointsforFaceB, faces, &faceNum, vertices, &vertexNum, hedges, &hedgeNum);


			Vector pointsforFaceC[4];
			pointsforFaceC[0] = points[2];
			pointsforFaceC[1] = points[1];
			pointsforFaceC[2] = points[5];
			pointsforFaceC[3] = points[6];
			addFace(pointsforFaceC, faces, &faceNum, vertices, &vertexNum, hedges, &hedgeNum);


			Vector pointsforFaceD[4];
			pointsforFaceD[0] = points[0];
			pointsforFaceD[1] = points[3];
			pointsforFaceD[2] = points[7];
			pointsforFaceD[3] = points[4];
			addFace(pointsforFaceD, faces, &faceNum, vertices, &vertexNum, hedges, &hedgeNum);


			Vector pointsforFaceE[4];
			pointsforFaceE[0] = points[1];
			pointsforFaceE[1] = points[0];
			pointsforFaceE[2] = points[4];
			pointsforFaceE[3] = points[5];
			addFace(pointsforFaceE, faces, &faceNum, vertices, &vertexNum, hedges, &hedgeNum);


			Vector pointsforFaceF[4];
			pointsforFaceF[0] = points[5];
			pointsforFaceF[1] = points[4];
			pointsforFaceF[2] = points[7];
			pointsforFaceF[3] = points[6];
			addFace(pointsforFaceF, faces, &faceNum, vertices, &vertexNum, hedges, &hedgeNum);

			for (int i = 0; i < hedgeNum; i++)
				setPairHedge(&hedges[i], hedges, &hedgeNum);

			for (int i = 0; i < vertexNum; i++){
				setEmanatingHedges(&vertices[i], hedges, &hedgeNum);
			}


			subdivision(vertices, &vertexNum, hedges, &hedgeNum, faces, &faceNum, subvertices, &subvertexNum, subhedges, &subhedgeNum, subfaces, &subfaceNum);
			subdivision(subvertices, &subvertexNum, subhedges, &subhedgeNum, subfaces, &subfaceNum, secondvertices, &secondvertexNum, secondhedges, &secondhedgeNum, secondfaces, &secondfaceNum);
		
			

		}

	
		


	
	}


	void addFace(Vector *points, face * _faces, int *_faceNum, vertex * _vertices, int *_vertexNum,
				hedge * _hedges, int *_hedgeNum
		){
		int a = addVertex(points[0], _vertices, _vertexNum);
		int b = addVertex(points[1], _vertices, _vertexNum);
		int c = addVertex(points[2], _vertices, _vertexNum);
		int d = addVertex(points[3], _vertices, _vertexNum);
		_faces[(*_faceNum)].oneBorder = addHedge(&_vertices[a], &_vertices[b], &_faces[(*_faceNum)], _hedges, _hedgeNum);
	
		addHedge(&_vertices[b], &_vertices[c], &_faces[*_faceNum], _hedges, _hedgeNum);
		addHedge(&_vertices[c], &_vertices[d], &_faces[*_faceNum], _hedges, _hedgeNum);
		addHedge(&_vertices[d], &_vertices[a], &_faces[*_faceNum], _hedges, _hedgeNum);
		_hedges[*_hedgeNum - 4].nextHedge = &_hedges[*_hedgeNum - 3];
		_hedges[*_hedgeNum - 3].nextHedge = &_hedges[*_hedgeNum - 2];
		_hedges[*_hedgeNum - 2].nextHedge = &_hedges[*_hedgeNum - 1];
		_hedges[*_hedgeNum - 1].nextHedge = &_hedges[*_hedgeNum - 4];

		*_faceNum = *_faceNum + 1;
	}
	int addVertex(Vector point, vertex * _vertices, int *_vertexNum){
		for (int i = 0; i < *_vertexNum; i++){
			if ((_vertices[i].point) == point)
				return i;
		}

		_vertices[*_vertexNum].point = point;
		*_vertexNum+=1;
		return (*_vertexNum - 1);
	}
	hedge* addHedge(vertex* a, vertex* b, face* f, hedge * _hedges, int *_hedgeNum){
		_hedges[*_hedgeNum].borderedFace = f;
		_hedges[*_hedgeNum].startVertex = a;
		_hedges[*_hedgeNum].endVertex = b;
		*_hedgeNum+=1;
		return &_hedges[(*_hedgeNum - 1)];

	}

	void setPairHedge(hedge *h, hedge *_hedges, int * _hedgeNum){
		for (int i = 0; i < *_hedgeNum; i++)
		if (_hedges[i].startVertex == h->endVertex && _hedges[i].endVertex == h->startVertex)
		{

				h->pairHedge = &_hedges[i];
				_hedges[i].pairHedge = h;

		}
		
	}
	void setEmanatingHedges(vertex* v, hedge * _hedges, int *_hedgeNum){
		for (int i = 0; i < *_hedgeNum; i++){
			if (_hedges[i].startVertex == v){
				if (v->oneEmanatingHedge==NULL)
				v->oneEmanatingHedge = &_hedges[i];
			}
		
		}
	}



	void subdivision(vertex * invertices, int *invertexNum, hedge* inhedges, int *inhedgeNum, 
				face * infaces, int * infaceNum,
				vertex * outvertices, int * outvertexNum, hedge * outhedges, int *outhedgeNum, face * outfaces, int* outfaceNum
		)
	{
		for (int i = 0; i < *invertexNum; i++){
			int valence = 0;
			int connectinghedgelist[8];
			for (int j = 0; j < *inhedgeNum; j++){
				if (inhedges[j].endVertex->point == *(&invertices[i].point)){
					connectinghedgelist[valence] = j;
					valence++;
				}
					
			}
			for (int k = 0; k < valence; k++){
				Vector tempPoints[4];
				tempPoints[0] = invertices[i].point;
				tempPoints[1] = (inhedges[connectinghedgelist[k]].nextHedge->startVertex->point + inhedges[connectinghedgelist[k]].nextHedge->endVertex->point)*0.5f;
				tempPoints[3] = (inhedges[connectinghedgelist[k]].startVertex->point + inhedges[connectinghedgelist[k]].endVertex->point)*0.5f;
				
				Vector facepoint = Vector(0, 0, 0);
				hedge *actual = &inhedges[connectinghedgelist[k]];

				hedge *next = actual;
				do{
					facepoint =facepoint+ next->endVertex->point* (0.25f);
					next = next->nextHedge;
				} while (next != actual);
				tempPoints[2] = facepoint;
				addFace(tempPoints, outfaces, outfaceNum, outvertices, outvertexNum, outhedges, outhedgeNum);
			}

		}
		

		for (int i = 0; i < *outhedgeNum; i++)
			setPairHedge(&outhedges[i], outhedges, outhedgeNum);

		for (int i = 0; i < *outvertexNum; i++){
			setEmanatingHedges(&outvertices[i], outhedges, outhedgeNum);
		}


		bool moved[200] = { false };
		for (int i = 0; i < *outvertexNum; i++)
		{
			for (int j = 0; j < *invertexNum; j++)
			{
				if (outvertices[i].point == invertices[j].point&&moved[i]==false){
					int valence = 0;
					int connectinghedgelist[8];
					Vector sum = Vector(0, 0, 0);
					for (int k = 0; k < *outhedgeNum; k++){
						if (outhedges[k].endVertex->point == outvertices[i].point){
							connectinghedgelist[valence] = k;
							valence++;
						}
					
					}
					for (int k = 0; k < valence; k++){
						sum = sum + outhedges[connectinghedgelist[k]].startVertex->point *(2.0f / float(valence*valence));
						sum = sum + (outhedges[connectinghedgelist[k]].nextHedge->nextHedge->endVertex->point)  *(1.0f / float(valence*valence));
						}

					outvertices[i].point = outvertices[i].point*((float)(valence - 3) / (float) valence) + sum;
					moved[i] = true;
				}
			}


		}

		for (int k = 0; k < *outvertexNum;k++){
			if (moved[k]==true)
					for (int i = 0; i < *outhedgeNum; i++){
						if (outhedges[i].endVertex->point == outvertices[k].point)
						outhedges[i].nextHedge->endVertex->point = outhedges[i].nextHedge->endVertex->point*0.5 +
							(outhedges[i].nextHedge->endVertex->point)*0.25f
							+ outhedges[i].nextHedge->pairHedge->nextHedge->endVertex->point*0.25f;
					}
					

				}
	}



	void draw(){
		glBegin(GL_QUADS);
		for (int i = 0; i < faceNum; i++)
		{
			hedge *actual = faces[i].oneBorder;
			hedge *next = actual->nextHedge;
			Vector normal= ((actual->endVertex->point - actual->startVertex->point) % (next->endVertex->point - next->startVertex->point)).normalize()*-1;
			glNormal3f(normal.x, normal.y, normal.z);
			glVertex3f(actual->endVertex->point.x, actual->endVertex->point.y, actual->endVertex->point.z);
			while (next != actual){
				
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(next->endVertex->point.x, next->endVertex->point.y, next->endVertex->point.z);
				next = next->nextHedge;
			}
		}
		glEnd();

	}

	void drawsub(){
		glPushMatrix();
		glRotatef(-90, 0, 1, 0);
		glBegin(GL_QUADS);
		for (int i = 0; i < subfaceNum; i++)
		{
			hedge *actual = subfaces[i].oneBorder;
			hedge *next = actual->nextHedge;
			Vector normal = ((actual->endVertex->point - actual->startVertex->point) % (next->endVertex->point - next->startVertex->point)).normalize()*-1;
			glNormal3f(normal.x, normal.y, normal.z);
			glVertex3f(actual->endVertex->point.x, actual->endVertex->point.y, actual->endVertex->point.z);
			while (next != actual){
				
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(next->endVertex->point.x, next->endVertex->point.y, next->endVertex->point.z);
				next = next->nextHedge;
			}
		}
		glEnd();
		glPopMatrix();
	}

	void drawsub2()
	{
		glPushMatrix();
		glRotatef(-90, 0, 1, 0);
		
		glBegin(GL_QUADS);

		for (int k = 0; k < secondfaceNum; k++){
			hedge * actual = secondfaces[k].oneBorder;
			hedge* next = actual;
			do
			{
				vertex* actualVertex = next->endVertex;

				int valence = 0;
				int connectinghedgelist[8];
				Vector normal;
				for (int j = 0; j < secondhedgeNum; j++)
				{
					if (secondhedges[j].endVertex->point == actualVertex->point)
					{
						connectinghedgelist[valence] = j;
						valence++;
					}
				}

				for (int j = 0; j < valence; j++)
				{
					normal = normal + ((secondhedges[connectinghedgelist[j]].endVertex->point - secondhedges[connectinghedgelist[j]].startVertex->point) % (secondhedges[connectinghedgelist[j]].nextHedge->endVertex->point - secondhedges[connectinghedgelist[j]].nextHedge->startVertex->point))*(1.0f / (float)valence);
				}
				normal = normal.normalize()*-1;
				glNormal3f(normal.x, normal.y, normal.z);
				if (!isnottextured){
					glEnable(GL_TEXTURE_2D);
					glTexCoord2f((atan2f(actualVertex->point.z, actualVertex->point.x) + M_PI) / (M_PI * 2), actualVertex->point.y / 3.0f);
					
				}
				else {
					glDisable(GL_TEXTURE_2D);
				}
				glVertex3f(actualVertex->point.x, actualVertex->point.y, actualVertex->point.z);
				next = next->nextHedge;
			} while (next != actual);
			
		}
		glEnd();
		glPopMatrix();

	}


		
};
Vector Ppoints[8] = { Vector(-1.75, 0, 1.75), Vector(1.75, 0, 1.75), Vector(1.75, 0, -1.75),
Vector(-1.75, 0, -1.75), Vector(-0.5, 3, 0.5), Vector(0.5, 3, 0.5), Vector(0.5, 3, -0.5), Vector(-0.5, 3, -0.5) };

Vector HeadPoints[8] = { Vector(-1, 0, 1), Vector(1, 0, 1), Vector(1, 0, -1), Vector(-1, 0, -1),
Vector(-1, 2, 1), Vector(1, 2, 1), Vector(1, 2, -1), Vector(-1, 2, -1) };



SubdivisionSurface subdivisonsurface=SubdivisionSurface(Ppoints);
SubdivisionSurface subdivforhead = SubdivisionSurface(HeadPoints, true);


void makePenguinTexture(){

	for (int i = 0; i <res; i++){
		
		for (int j = 0; j < res; j++){

				float temp = powf((float)(128- i)/256.0f / 0.35f, 2) + pow((float)(j-128.0f)/256.0f / 0.15f, 2);
				if (temp <= 1){
					texture[i * res + j][0] = 255;
					texture[(i * res) + j][1] = 255;
					texture[(i * res) + j][2] = 255;
					texture[(i * res) + j][3] = 255;
				}
				else{
				
					texture[i * res + j][0] = 0;
					texture[(i * res) + j][1] = 0;
					texture[(i * res) + j][2] = 0;
					texture[(i * res) + j][3] = 255;
				}
		}
		
	}




}




class ParamSurface{
	
public:
	int NTESS;
	ParamSurface(){
	
		NTESS =20;
	}
	void set(){}
	void draw(){
	
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		for (int i = 0; i < NTESS; i++)
		for (int j = 0; j < NTESS; j++) {
			VertexOGL((float)i / NTESS, (float)j / NTESS);
			VertexOGL((float)(i + 1) / NTESS, (float)j / NTESS);
			VertexOGL((float)(i + 1) / NTESS, (float)(j + 1) / NTESS);
			VertexOGL((float)i / NTESS, (float)(j + 1) / NTESS);
		}

		glEnd();

	
	}
	virtual void VertexOGL(float u, float v) = 0;

};

class Cone :public ParamSurface{
public:
	Vector center;
	float height;
	float radius;
	Cone(float rad=0.5f, float h=1){
		radius = rad;
		height = h;
	}
	void VertexOGL(float u, float v){
		Vector r;
		u = u *height;
		v = v *2*  M_PI;
		r.x = radius*u*cos(v);
		r.y = -u;
		r.z = radius*u*sin(v);

		Vector du;
		du.x = cos(v);
		du.y = -1;
		du.z = sin(v);
		Vector dv;
		dv.x = -sin(v)*u;
		dv.y = 0;
		dv.z = cos(v)*u;
		Vector normal = (du%dv).normalize()*-1;
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(r.x, r.z, r.y);
	}


};



class Ellipsoid :public ParamSurface{
public:
	Vector axis;
	Vector center;
	Ellipsoid(Vector _axis= Vector (1.5, 0.75, 1)){
		center =Vector( 0, 0, 0);
		axis = _axis;
	}
	void VertexOGL(float u, float v){
		Vector r;
		u = (u-M_PI/2)*M_PI;
		v = (v-M_PI)*M_PI*2;
		r.x = center.x+axis.x*cos(u)*cos(v);
		r.y = center.y+axis.y*cos(u)*sin(v);
		r.z = center.z+axis.z*sin(u);
		Vector du;
		du.x = -sin(u)*cos(v);
		du.y = -sin(u)*sin(v);
		du.z = cos(u);
		du = du.normalize();
		Vector dv;
		dv.x = cos(u)*-sin(v);
		dv.y = cos(u)*cos(v);
		dv.z = 0;
		dv = dv.normalize();
		Vector normal = (du%dv).normalize();
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(r.x, r.y, r.z);
	
	}

};

class Sphere : public ParamSurface{
public:
	Vector center;
	float radius;
	Sphere(Vector _center=Vector(0,0,0), float rad=1.0f){
		center = _center;
		radius=rad;

	}
	void VertexOGL(float u, float v){
		Vector r;
		u = u * 2 * M_PI;
		v = (v*2*M_PI);
		r.x =center.x+ radius*sin(u)*cos(v);
		r.y =center.y+ radius*cos(u)*cos(v);
		r.z =center.z+ radius*sin(v);
		
		Vector normal;
		normal.x = 2 * (r.x - center.x);
		normal.y = 2 * (r.y - center.y);
		normal.z = 2 * (r.z - center.z);

		normal=normal.normalize();
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(r.x, r.y, r.z);
	}
};

class Egg :public Ellipsoid{
public:
	Vector position;
	Egg(){
		position = Vector(rand()%20-10, 0.5, rand()%20-10);
		center = position;
	

	}
	void drawIt(){
		glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
		draw();
		glPopMatrix();
	}
	virtual ~Egg(){
		
	}

};
const int maxEggNum=5;
int eggNum = maxEggNum;
Egg* egglist[maxEggNum];

class Cylinder :public ParamSurface{
public:
	float radius;
	float height;
	Cylinder(float rad=0.35, float h=2){
		radius = rad;
		height = h;
	}
	void VertexOGL(float u, float v){
		Vector r;
		Vector normal;
		u = u*height;
		v = v * 2 * M_PI;
		r.x = radius*cos(v);
		r.y = u;
		r.z = radius*sin(v);
		Vector du=Vector(0, 1, 0);
		Vector dv= Vector(-sin(v), 0, cos(v)) ;
		normal = du%dv;
		normal = normal.normalize();
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(r.x, r.y, r.z);
	
	}

};


class PenguinBase{
public:
	
	
	float speed;
	Vector position;
	Vector forward;
	float rotation_angle;
	float leftlegrotation;
	float dleftlegrotation;
	float rightlegrotation;
	float drightlegrotation;
	float leftfootrotation;
	float rightfootrotation;
	float headrotation;
	float dheadrotation;
	Sphere eyes[2];
	Sphere iris[2];
	Sphere pupil[2];
	Sphere joints[2];
	
	SubdivisionSurface* head2;
	Cone beak2;

	Cylinder cylinderlegs[2];

	Cone cfeet[2];

	SubdivisionSurface* body2;
	float timeToHatch;
	void init(){
		eyes[0] = Sphere(Vector(0, 0, 0), 1.0f);
		eyes[1] = Sphere(Vector(0, 0, 0), 1.0f);
		pupil[0] = Sphere(Vector(0, 0, 0), 0.5f);
		pupil[1] = Sphere(Vector(0, 0, 0), 0.5f);
		iris[0] = Sphere(Vector(0, 0, 0), 0.75f);
		iris[1] = Sphere(Vector(0, 0, 0), 0.75f);

		
		beak2 = Cone();


		cylinderlegs[0] = Cylinder();
		cylinderlegs[1] = Cylinder();

	
		
		for (int i = 0; i < 8; i++)
		{
			position = position + Ppoints[i];

		}
		position = position*(1.0f / 8.0f);
		position.y -= 0.5;

		body2 = &subdivisonsurface;
		head2 = &subdivforhead;

		joints[0] = Sphere();
		joints[1] = Sphere();
		cfeet[0] = Cone();
		cfeet[1] = Cone();

		
		forward = Vector(0, 0, 1);

		speed = 2.0f;
		rotation_angle = 0;
		leftlegrotation = 0;
		dleftlegrotation = 30;
		rightlegrotation = 0;
		drightlegrotation = -30;
		leftfootrotation = 0;
		rightfootrotation = 0;
		dheadrotation = 30;
		headrotation = 0;
		timeToHatch = 0;
	}
	void draw(){
		
		glPushMatrix();
		glTranslatef(position.x, position.y-0.3, position.z);
		glRotatef(rotation_angle, 0, 1, 0);
	
		glPushMatrix();
		glScalef(0.8, 0.8, 0.8);
		glTranslatef(0, 3, 0);
		
		glRotatef(headrotation, 1, 0, 0);
	
		glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
		head2->drawsub2();
			glPushMatrix();
				
				glTranslatef(-0.4, 1.2, 0.6);
				glScalef(0.3, 0.3, 0.3);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
				eyes[0].draw();
				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
				glTranslatef(-0.1, 0, 0.4);
				iris[0].draw();
				glPopMatrix();


				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
				glTranslatef(-0.15, 0, 0.7);
				pupil[0].draw();
				glPopMatrix();
			glPopMatrix();

			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
				glTranslatef(0.4, 1.2,0.6);
				glScalef(0.3, 0.3, 0.3);
				eyes[1].draw();

				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
				glTranslatef(0.1, 0, 0.4);
				iris[1].draw();
				glPopMatrix();


				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
				glTranslatef(0.15, 0, 0.7);
				pupil[1].draw();
				glPopMatrix();

			glPopMatrix();


			glPushMatrix();

			glTranslatef(0,  0.8, 1.6);

			glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		
			
			beak2.draw();
			glPopMatrix();

		glPopMatrix();
		

		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		
		glRotatef(leftlegrotation, 1, 0, 0);
	
		glTranslatef(-0.4, -0.6, 0.0);
		glScalef(0.5, 0.5, 0.5);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, gray);
		
		cylinderlegs[0].draw();
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
			glScalef(0.4, 0.4, 0.4);
			joints[0].draw();
			
		glPopMatrix();
		
		glPushMatrix();
		glScalef(0.8, 0.8, 0.8);
		glRotatef(leftfootrotation, 1, 0, 0);
		glTranslatef(0, 0, 1.2);
	
		cfeet[0].draw();
	
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		glRotatef(rightlegrotation, 1, 0, 0);

		glTranslatef(0.4, -0.6, 0);
		glScalef(0.5, 0.5, 0.5);

		glMaterialfv(GL_FRONT, GL_DIFFUSE, gray);

		cylinderlegs[1].draw();
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		glScalef(0.4, 0.4, 0.4);
		joints[1].draw();

		glPopMatrix();
	
		glPushMatrix();
		glScalef(0.8, 0.8, 0.8);
		glRotatef(rightfootrotation, 1, 0, 0);
		glTranslatef(0, 0, 1.2);
	
		cfeet[1].draw();

	
		
		glPopMatrix();


		glPopMatrix();

		glPushMatrix();
		
		float tempwhite[4] = { white[0], white[1], white[2], white[3] };
		for (int i = 0; i < 4; i++)
			tempwhite[i] *= 3;
		glEnable(GL_TEXTURE_2D);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tempwhite);

		body2->drawsub2();
		glDisable(GL_TEXTURE_2D);


		glPopMatrix();
		glPopMatrix();


	}
	virtual void animate(float dt) = 0;
	
};

PenguinBase* penguinlist[maxEggNum];
int penguinNum = 0;

class PenguinBot :public PenguinBase{
public:
	PenguinBot(){
	}

	void animate(float dt){
		if (eggNum > 0){
			float dist = 1000;
			int minindex = 0;
			float mindist = 1000;
		
			for (int i = 0; i < maxEggNum; i++){
				if (egglist[i] != NULL){
					if ((egglist[i]->position - position).Length() < mindist){
						mindist = (egglist[i]->position - position).Length();
						minindex = i;
					}
				}

			}
			dist = (egglist[minindex]->position - position).Length();
			if (dist<2.5){

				timeToHatch += dt;
				if (timeToHatch > 3){

					if (egglist[minindex] != NULL){

						penguinlist[penguinNum] = new PenguinBot();
						penguinlist[penguinNum]->init();
						penguinlist[penguinNum]->position = egglist[minindex]->position;
						penguinlist[penguinNum]->position.y += 0.5;
						penguinNum++;
						delete egglist[minindex];
						egglist[minindex] = NULL;
						eggNum--;
					}
					timeToHatch = 0;
					forward = Vector(0, 0, 1);
					rotation_angle = 0;
				}
			}
			if (egglist[minindex] != NULL){
				
				Vector temp = Vector((egglist[minindex]->position - position).x, 0, (egglist[minindex]->position - position).z).normalize();

				float desired_angle = atan2(temp.x, temp.z)*180/M_PI;
				if ((fabs(rotation_angle - desired_angle) >10)){
			
					rotation_angle += desired_angle*dt;
				
				}
				else
				{
				
					if (dist > 2.5){
					
     				
						forward.x = sin(rotation_angle / 180 * M_PI);
						forward.z = cos(rotation_angle / 180 * M_PI);
						position = position + forward* speed*dt;
						rightlegrotation += drightlegrotation*dt*speed;
						leftlegrotation += dleftlegrotation*dt*speed;
					}
			
					if (rightlegrotation > 30){
						drightlegrotation = -30;
						dleftlegrotation = 30;
					}
					if (leftlegrotation > 30){
						dleftlegrotation = -30;
						drightlegrotation = 30;
					}
				}

			}
			
		}
		else{
			headrotation += dheadrotation*dt*speed;
			if (headrotation > 30)
				dheadrotation = -30;
			if (headrotation < -10)
				dheadrotation = 30;
		}
	}
	
	virtual ~PenguinBot(){
		
	}
};


class Penguin:public PenguinBase{
public:
	
	Penguin(){
		

	}
	
	void animate(float dt){
		forward.x = sin(rotation_angle /180*M_PI);
		forward.z = cos(rotation_angle /180*M_PI);
		if (keydown[E]){
			position = position + forward* speed*dt;
			rightlegrotation += drightlegrotation*dt*speed;
			leftlegrotation += dleftlegrotation*dt*speed;
			rightfootrotation += dleftlegrotation*dt*speed;
			leftfootrotation += drightlegrotation*dt*speed;
			headrotation += dheadrotation*dt*speed;
			if (rightlegrotation > 30){
				drightlegrotation = -30;
				dleftlegrotation = 30;
			}
			if (leftlegrotation > 30){
				dleftlegrotation = -30;
				drightlegrotation = 30;
			}
			if (headrotation > 20)
				dheadrotation = -30;
			if (headrotation < -10)
				dheadrotation = 30;

		}
		if (keydown[W] && !keydown[R]){
			rotation_angle -= speed;
			
		}
		if (keydown[R] && !keydown[W]){
			rotation_angle += speed;
		
		}
		if (keydown[Space]&&!keydown[E]){
		
			if (eggNum>0){
				float dist = 1000;
				int minindex = 0;
				float mindist = 1000;
				
				for (int i = 0; i < maxEggNum; i++){
					if (egglist[i] != NULL){
						if ((egglist[i]->position - position).Length() < mindist){
							mindist = (egglist[i]->position - position).Length();
							minindex = i;
						}
					}

				}
				dist = (egglist[minindex]->position - position).Length();
				if (dist<2.5){
					timeToHatch += dt;
					if (timeToHatch > 3){

						if (egglist[minindex] != NULL){

							penguinlist[penguinNum] = new PenguinBot();
							penguinlist[penguinNum]->init();
							penguinlist[penguinNum]->position = egglist[minindex]->position;
							penguinlist[penguinNum]->position.y += 0.5;
							penguinNum++;
							delete egglist[minindex];
							egglist[minindex] = NULL;
							eggNum--;
						}
						timeToHatch = 0;
					}
				}
			}
		}
		

	}
	

};


Penguin ping;



class Camera{
public:
	Vector eye, lookat, vup;
	float fov, asp, fp, bp, viewport, speed;
	float turnspeed;
	Vector forward;
	Camera(){
		fov = 45;
		asp = screenWidth / screenHeight;
		fp = 1;
		bp = 100;
		viewport = 600;
		speed = 2.0f;
		turnspeed = 0.1f;
		eye = Vector(20, 10,10);
		lookat = ping.position;
		vup = Vector(0, 1, 0);
		forward = Vector(0, 0, -1).normalize();
	}
	void animate(float dt){
		lookat = (ping.position-eye)*speed;
		
		

	}


	void SetOGL(){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(fov, asp, fp, bp);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x, eye.y, eye.z,
			lookat.x, lookat.y, lookat.z,
			vup.x, vup.y, vup.z);
	
	}
};
Camera camera;


void Render(){


	camera.SetOGL();
	shadow=false;
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);	

	for (int x = -20; x < 20; x++) {
		for (int z = -20; z <20; z++) {
			bool iswhite = (x ^ z) & 1;
	
			glMaterialfv(GL_FRONT, GL_DIFFUSE, iswhite ? white : blue);
		
			glNormal3f(0, 1, 0);
			glVertex3f(x * 5, 0, z * 5);
			glVertex3f((x + 1) * 5, 0, z * 5);
			glVertex3f((x + 1) * 5, 0, (z + 1) * 5);
			glVertex3f(x * 5, 0, (z + 1) * 5);
		}
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res, res, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);


	glMaterialfv(GL_FRONT, GL_DIFFUSE, gray);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	
	ping.draw();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, black);

	float s = 10.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &s);
	for (int i = 0; i < maxEggNum; i++){
		if (egglist[i]!=NULL)
			egglist[i]->draw();

	}
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	
	for (int j = 0; j < maxEggNum; j++){
		if (penguinlist[j] != NULL){
			penguinlist[j]->draw();
		}
	}
	
	

	shadow = true;
	float shadowmatrix[4][4] = { 1, 0, 0, 0,
		-LightPosition[0] / LightPosition[1], 0, -LightPosition[2] / LightPosition[1], 0,
		0, 0, 1, 0,
		0, 0.01, 0, 1 };
	glMultMatrixf(&shadowmatrix[0][0]);
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 0);
	ping.draw();
	for (int i = 0; i < maxEggNum; i++){
		if (egglist[i] != NULL)
			egglist[i]->draw();

	}
	for (int j = 0; j < maxEggNum; j++){
		if (penguinlist[j] != NULL){
			penguinlist[j]->draw();
		}
	}
	shadow = false;

	
}



// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_LIGHTING);
	float LightAmbient[] = { 0.1, 0.1, 0.1, 1.0 };
	float LightDiffuse[] = { 1, 1, 1, 1.0 };
	float LightSpecular[] = { 1, 1, 1, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	makePenguinTexture();
	ping = Penguin();
	ping.init();
	glViewport(0, 0, screenWidth, screenHeight);
	camera = Camera();
	glGenTextures(1, &textId);
	glBindTexture(GL_TEXTURE_2D, textId);
	for (int i = 0; i < maxEggNum; i++)
		egglist[i] = new Egg();
	for (int i = 0; i < maxEggNum; i++)
		penguinlist[i] = NULL;
	
	
}


// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	
	glClearColor(0.2f, 0.3f, 1.0f, 1.0f);		// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	// ..

	Render();

	// ...

	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	switch (key){
	case 'W': case 'w': keydown[W] = true; break;
	case 'E': case 'e': keydown[E] = true; break;
	case 'R': case 'r': keydown[R] = true; break;
	case ' ': keydown[Space] = true; break;

	}


	

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

	switch (key){
	case 'W': case 'w': keydown[W] = false; break;
	case 'E': case 'e': keydown[E] = false; break;
	case 'R': case 'r': keydown[R] = false; break;
	case ' ': keydown[Space] = false; break;

	}


}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

long prevtime = 0;
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
	float dt = (time - prevtime)/1000.0f;	
	prevtime = time;
	ping.animate(dt);
	camera.animate(dt);
	for (int j = 0; j < maxEggNum; j++){
		if (penguinlist[j] != NULL){
			penguinlist[j]->animate(dt);
		}
	}
	
	glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
	glutInit(&argc, argv); 				// GLUT inicializalasa
	glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
	glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();					// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}
