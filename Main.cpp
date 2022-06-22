//INCLUDES
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <math.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <thread>
#include <algorithm>
#include "jpeglib.h"
#ifdef WIN32
#include <windows.h>
#endif
//pacman resources
#include "MapAlgorithm.h"
#include "Block.h"
#include "PacmanGuy.h"
#include "Ghost.h"
#include "gameResources.h"
//glut
#include <GL/glut.h>

//DEFINES
#define WIDTH 500
#define HEIGHT 500
#define PI 3.1416
#define BGC_ARRAYSIZE 10
#define RANKING_FILE_NAME "HallOfFame"
#define RANKING_MAX_VALUES 10

using namespace std;
using namespace gameResources;


//GLOBAL VARIABLES
std::vector <std::vector<Block> > mapToPrint;
std::vector <Ghost> ghosts;
std::vector <PacmanGuy> pacmans (MAXPLAYERS);
std::vector <Status> statusNext(1);
std::vector <pair<int, string>> rankingInfo;
enum Elements { BLOCK, FOOD, PACMAN, GHOST, GROUND };
PacmanGuy player1;
PacmanGuy player2;
int columns;
int rows;
int totalFood;
int pacmanPoints;
int nPlayers    = 0;
int nGhosts     = 0;
int ghostsSpeed = 140;
float last_t    = 0;
float zoom      = 0.3;
float blockSize = 15;
bool pauseGame  = false;
bool gameEnded  = false;
Status pacmanPrevStatus = Status::DOWN;
IA ia = IA::RANDOM;
//New Game
int argc;
char **argv;
int level = 1;

//Arduino
ifstream myfile("/dev/ttyACM0");
bool startProcess = false;

std::vector <int> arraySensorGSR;
std::vector <int> arraySensorBPM;

unsigned positionZero = 0;

//3D
int angleAlpha = 83;
int angleBeta  = 0;

//Functions declaration
void newGame();
void run();
void saveRanking(string _fileName);

//Creates a new Pacman and situates in the map
PacmanGuy newPacman(std::pair<int, int> _position, int _id, int _currentPoints = 0)
{
    PacmanGuy pacman = PacmanGuy(_id, mapToPrint);

    pacman.sumPoints(_currentPoints);

    pacman.setPosition(_position);

    nPlayers++;

    return pacman;
}

//Creates a new Ghost and situates in the map
Ghost newGhost(std::pair<int, int> _position, int _id, IA _ia)
{
    Ghost ghost;
    ghostsSpeed = 500 - (level - 1) * 20;

    if(nGhosts < MAXGHOSTS)
    {
        ghost =  Ghost(_id, mapToPrint, ghostsSpeed, ia);
        ghost.setPacmanPosition(player1.getx(), player1.gety());
        nGhosts++;

        ghosts.push_back(ghost);
        //ghosts.back().init_movement(speed, Status::DOWN);
    }
    return ghost;
}

void drawText(const char *text, int length, int x, int y)
{
    glMatrixMode(GL_PROJECTION);
    double *matrix = new double[30];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0,200,0,200,-5,5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x,y);
    //glRasterPos2i(player1.getx()*blockSize,player1.gety()*blockSize);
    glScalef(0.09f,-0.08f,1);
    for(int i = 0; i<length; i++)
    {
        //cout << text << endl;
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)text[i]);
        //glutStrokeCharacter(GLUT_STROKE_ROMAN,(int)text[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}


void insertRanking(string _fileName)
{
    FILE*  rankingFile;
    string playerName;

    /*
    r      lee el archivo.
    w      reemplaza el contenido del archivo por otro texto.
    rw     lee y reemplaza el texto del archivo por otro.
    w+     agrega un texto al final del archivo.
    a+     es lo mismo que w+.
    */

    rankingFile = fopen((_fileName + ".txt").c_str(), "a+");
    cout << "Enter your name:" << endl;
    cin >> playerName;

    playerName =  to_string(player1.getPoints()) + " " + playerName  + "\n";

    fprintf(rankingFile, "%s", playerName.c_str());
    fclose(rankingFile);

    saveRanking(RANKING_FILE_NAME);

    ofstream file((_fileName + ".txt").c_str(), std::ios::trunc);

    rankingFile = fopen((_fileName + ".txt").c_str(), "a+");

    for(std::vector<pair<int, string>>::iterator it = rankingInfo.begin(); it != rankingInfo.end(); it++)
    {
        playerName = to_string((*it).first) + " " + (*it).second + "\n";
        fprintf(rankingFile, "%s", playerName.c_str());
    }
    fclose(rankingFile);
}

void saveRanking(string _fileName)
{
    std::vector<std::string> tokens;
    pair<string, int> info;
    string rankingLine;
    ifstream rankingFile((_fileName + ".txt").c_str());

    if (rankingFile.is_open())
    {
        rankingInfo.clear();

        while (!rankingFile.eof())
        {
            getline(rankingFile, rankingLine);

            std::istringstream split(rankingLine);

            for (std::string temp; std::getline(split, temp, ' '); tokens.push_back(temp));

            if (!tokens.empty() && rankingInfo.size() <= RANKING_MAX_VALUES + 1)
                rankingInfo.push_back( std::make_pair( atoi(tokens.front().c_str()), tokens.back() ) );

            else
                break;

            tokens.clear();
        }
        sort(rankingInfo.begin(), rankingInfo.end());
        reverse(rankingInfo.begin(), rankingInfo.end());

        if (rankingInfo.size() == RANKING_MAX_VALUES + 1)
            rankingInfo.pop_back();
    }
}

void printRanking()
{
    string line = "Points     Player";
    int    separation = 8;

    drawText(line.data(), line.length(), 0, 194 - separation);
    separation += 5;

    for(std::vector<pair<int, string>>::iterator it = rankingInfo.begin(); it != rankingInfo.end(); it++)
    {
        separation += 8;
        line = to_string((*it).first) + "           " + (*it).second;
        drawText(line.data(), line.length(), 0, 194 - separation);
    }
}


void endGame(bool _winLost)
{
    if (_winLost)
    {
        printf("YOU WIN!\n");
        level++;
        newGame();
    }
    else
    {
        printf("YOU LOST!\n");
        gameEnded = true;
        insertRanking(RANKING_FILE_NAME);
    }

    /*printf("Press enter to exit\n");

    getchar();

    exit(0);*/
}

void bgcDificulty(bool increase)
{
    if  (increase)
    {
        newGhost({columns/2, rows/2}, nGhosts + 1, ia);
    }

    else if (nGhosts > 0)
    {
        ghosts.pop_back();
        nGhosts--;
    }
}

void ReadJPEG(char *filename,unsigned char **image,int *width, int *height)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * infile;
  unsigned char **buffer;
  int i,j;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);


  if ((infile = fopen(filename, "rb")) == NULL) {
    printf("Unable to open file %s\n",filename);
    exit(1);
  }

  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_calc_output_dimensions(&cinfo);
  jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width;
  *height  = cinfo.output_height;


  *image=(unsigned char*)malloc(cinfo.output_width*cinfo.output_height*cinfo.output_components);

  buffer=(unsigned char **)malloc(1*sizeof(unsigned char **));
  buffer[0]=(unsigned char *)malloc(cinfo.output_width*cinfo.output_components);


  i=0;
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);

    for(j=0;j<cinfo.output_width*cinfo.output_components;j++)
      {
	(*image)[i]=buffer[0][j];
	i++;
      }

    }

  free(buffer);
  jpeg_finish_decompress(&cinfo);
}

void LoadTexture(char *filename,int dim)
{
    unsigned char *buffer;
    unsigned char *buffer2;
    int width,height;
    long i,j;
    long k,h;

    ReadJPEG(filename,&buffer,&width,&height);

    buffer2=(unsigned char*)malloc(dim*dim*3);

  //-- The texture pattern is subsampled so that its dimensions become dim x dim --
    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
        {
            k=i*height/dim;
            h=j*width/dim;

            buffer2[3*(i*dim+j)]=buffer[3*(k*width +h)];
            buffer2[3*(i*dim+j)+1]=buffer[3*(k*width +h)+1];
            buffer2[3*(i*dim+j)+2]=buffer[3*(k*width +h)+2];
        }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dim,dim,0,GL_RGB,GL_UNSIGNED_BYTE,buffer2);

    free(buffer);
    free(buffer2);
}

void PositionObserver(float alpha,float beta,int radi)
{
    float x,y,z;
    float upx,upy,upz;
    float modul;

    x = (float)radi*cos(alpha*2*PI/360.0)*cos(beta*2*PI/360.0);
    y = (float)radi*sin(beta*2*PI/360.0);
    z = (float)radi*sin(alpha*2*PI/360.0)*cos(beta*2*PI/360.0);

    if (beta > 0)
    {
          upx=-x;
          upz=-z;
          upy=(x*x+z*z)/y;
    }
    else if (beta == 0)
    {
          upx=0;
          upy=1;
          upz=0;
    }
    else
    {
          upx=x;
          upz=z;
          upy=-(x*x+z*z)/y;
    }


    modul=sqrt(upx*upx+upy*upy+upz*upz);

    upx=upx/modul;
    upy=upy/modul;
    upz=upz/modul;
    /*printf("Upx: %d, Upy: %d, Upz: %d\n", upx, upy, upz);//test
    printf("X: %d, Y: %d, Z: %d\n", x, y, z);//test
    printf("Alpha: % d, Beta: %d\n",angleAlpha, angleBeta);//test*/
    gluLookAt(x,y,z,  columns*blockSize/2, rows*blockSize/2, 0.0, upx,upy,upz);
}

void createSquare(float _x, float _y, float _size, Elements _element)
{
    /*Square views (vertexs direction)
      Up-Down    |   Front
     (->)(<-)         (->)
      A-----B       C-----D
      |     |       |     |
      |     |       |     |
      C-----D       C'----D'
    */
    float halfSize    = _size / 2;
    float squareHeigh = halfSize;

    float xMin = _x * blockSize - halfSize;
    float xMax = _x * blockSize + halfSize;
    float yMin = _y * blockSize - halfSize;
    float yMax = _y * blockSize + halfSize;

    pair <float, float> vertexA = {xMin, yMin};
    pair <float, float> vertexB = {xMax, yMin};
    pair <float, float> vertexC = {xMin, yMax};
    pair <float, float> vertexD = {xMax, yMax};

    GLint   position[4];
    GLfloat color[4];
    GLfloat material[4];

    float middle = ((vertexB.first-vertexA.first)/2);


    //Up
    if(_element == Elements::BLOCK)
    {
        glBindTexture(GL_TEXTURE_2D,1);
    }

    if(_element == Elements::GROUND)
    {
        glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        //A
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexA.first, vertexA.second, -squareHeigh);
        //A-B
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexA.first+middle, vertexA.second, -squareHeigh);
        //Middle
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexA.first+middle, vertexA.second+middle, -squareHeigh);
        //A-C
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexA.first, vertexA.second+middle, -squareHeigh);
        glEnd();

        glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        //A-B
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexA.first+middle, vertexA.second, -squareHeigh);
        //B
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexB.first, vertexB.second, -squareHeigh);
        //B-D
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexB.first, vertexB.second+middle, -squareHeigh);
        //Middle
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexA.first+middle, vertexA.second+middle, -squareHeigh);
         glEnd();

        glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        //Middle
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexA.first+middle, vertexA.second+middle, -squareHeigh);
         //B-D
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexB.first, vertexB.second+middle, -squareHeigh);
        //D
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexD.first, vertexD.second, -squareHeigh);
        //D-C
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexD.first-middle, vertexD.second, -squareHeigh);
        glEnd();

        glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        //A-C
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexA.first, vertexA.second+middle, -squareHeigh);
        //Middle
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexA.first+middle, vertexA.second+middle, -squareHeigh);
         //D-C
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexD.first-middle, vertexD.second, -squareHeigh);
        //C
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexC.first, vertexC.second, -squareHeigh);
        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexA.first, vertexA.second, squareHeigh);
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexB.first, vertexB.second, squareHeigh);
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexD.first, vertexD.second, squareHeigh);
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexC.first, vertexC.second, squareHeigh);
        glEnd();
    }



    //Down
    if(_element == Elements::BLOCK)
    {
        glBindTexture(GL_TEXTURE_2D,2);
    }
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0.0,1.0);
    glVertex3f(vertexA.first, vertexA.second, -squareHeigh);
    glTexCoord2f(1.0,1.0);
    glVertex3f(vertexC.first, vertexC.second, -squareHeigh);
    glTexCoord2f(1.0,0.0);
    glVertex3f(vertexD.first, vertexD.second, -squareHeigh);
    glTexCoord2f(0.0,0.0);
    glVertex3f(vertexB.first, vertexB.second, -squareHeigh);
    glEnd();

    if (_element != Elements::GROUND)
    {
        if(_element == Elements::BLOCK)
        {
            glBindTexture(GL_TEXTURE_2D,0);
        }
        //Front
        glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexC.first, vertexC.second, squareHeigh);
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexD.first, vertexD.second, squareHeigh);
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexD.first, vertexD.second, -squareHeigh);
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexC.first, vertexC.second, -squareHeigh);
        glEnd();

        //Left
        glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexA.first, vertexA.second, squareHeigh);
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexC.first, vertexC.second, squareHeigh);
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexC.first, vertexC.second, -squareHeigh);
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexA.first, vertexA.second, -squareHeigh);
        glEnd();

        //Right
        glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexB.first, vertexB.second, squareHeigh);
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexB.first, vertexB.second, -squareHeigh);
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexD.first, vertexD.second, -squareHeigh);
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexD.first, vertexD.second, squareHeigh);
        glEnd();

        //Back
        glBegin(GL_QUADS);
        glNormal3f(0,-1,0);
        glTexCoord2f(0.0,0.0);
        glVertex3f(vertexA.first, vertexA.second, squareHeigh);
        glTexCoord2f(0.0,1.0);
        glVertex3f(vertexA.first, vertexA.second, -squareHeigh);
        glTexCoord2f(1.0,1.0);
        glVertex3f(vertexB.first, vertexB.second, -squareHeigh);
        glTexCoord2f(1.0,0.0);
        glVertex3f(vertexB.first, vertexB.second, squareHeigh);
        glEnd();
    }
}

void printElements(Elements _element, float _col, float _row)
{
    float ghostSize  = blockSize - blockSize * 0.5;
    float pacmanSize = ghostSize;
    float foodSize   = blockSize - blockSize * 0.7;

    switch (_element)
    {
        case Elements::BLOCK:
            //glColor3ub(153.0, 76.0, 0.0);
            //glBindTexture(GL_TEXTURE_2D,1);
            createSquare(_col, _row, blockSize, _element);
            break;

        case Elements::FOOD:
            glBindTexture(GL_TEXTURE_2D,5);
            //glColor3f(0.8,0.8,0.8);
            createSquare(_col, _row, foodSize, _element);
            break;

        case Elements::PACMAN:
            glBindTexture(GL_TEXTURE_2D,3);
            //glColor3f(1.0f, 1.0f, 0.0f);
            createSquare(_col, _row, pacmanSize, _element);
            break;

        case Elements::GHOST:
            //glColor3f(9.0f, 100.0f, 35.0f);
            glBindTexture(GL_TEXTURE_2D,4);
            createSquare(_col, _row, ghostSize, _element);
            break;

        case Elements::GROUND:
            glBindTexture(GL_TEXTURE_2D,6);
            glColor3ub(76.0, 153.0, 0.0);
            createSquare(_col, _row, blockSize, _element);
            break;
    }
}

//Display Function
//Display Function
void printMap()
{
    string  text;
	Block   printBlock;
    GLint   position[4];
    GLfloat color[4];
    GLfloat dir[3];
    GLfloat material[4];
    Status  pacmanActStatus = player1.getStatus();



	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0,1.0,1.0,1.0);

	if (player1.getStatus() == Status::DEAD || pauseGame == true)
	{
        glLoadIdentity();
        printRanking();
	}

	else
	{

        text = "Lives:"+to_string(player1.getLives()) + " Points:" + to_string(player1.getPoints());

        glPushAttrib(GL_CURRENT_BIT);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_TEXTURE);
        glDisable(GL_AMBIENT_AND_DIFFUSE);
        glColor3f(0.8,0.8,0.8);

        drawText(text.data(), text.length(), 0, 0);

        glEnable(GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_TEXTURE);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glPopAttrib();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        PositionObserver(angleAlpha, angleBeta, 750);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-WIDTH*zoom, WIDTH*zoom, -HEIGHT*zoom, HEIGHT*zoom, 10, 2000);

        glMatrixMode(GL_MODELVIEW);

        glPolygonMode(GL_FRONT,GL_FILL);
        glPolygonMode(GL_BACK,GL_LINE);

        //-- Ambient light (light 0)
        position[0] = 0; position[1] = 0; position[2] = 0; position[3] = 1;
        glLightiv(GL_LIGHT0, GL_POSITION, position);

        color[0] = 0.0; color[1] = 0.0; color[2] = 0.0; color[3] = 1;
        glLightfv(GL_LIGHT0, GL_AMBIENT, color);
        color[0] = 0.0; color[1] = 0.0; color[2] = 0.0; color[3] = 1;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

        glEnable(GL_LIGHT0);

        //-- Spot light (light 1)
        //Position
        position[0] = player1.getx()*blockSize; // x's
        position[1] = player1.gety()*blockSize; // y's
        position[2] = blockSize - blockSize * 0.7;//Height
        position[3] = 1;//Scale
        glLightiv(GL_LIGHT1, GL_POSITION, position);

        //Color
        color[0] = 1.0; color[1] = 1.0; color[2] = 1.0; color[3] = 1;

        //Direction
        switch ( pacmanActStatus == Status::STOP ? pacmanPrevStatus : pacmanActStatus )
        {
            case Status::RIGHT :
                dir[0] = 1.0; dir[1] = 0.0; dir[2] = 0.0;
                break;

            case Status::LEFT :
                dir[0] = -1.0; dir[1] = 0.0; dir[2] = 0.0;
                break;

            case Status::UP :
                dir[0] = 0.0; dir[1] = 1.0; dir[2] = 0.0;
                break;

            case Status::DOWN :
                dir[0] = 0.0; dir[1] = -1.0; dir[2] = 0.0;
                break;
        }

        if (pacmanActStatus != Status::STOP)
        {
            pacmanPrevStatus = pacmanActStatus;
        }

        glLightfv(GL_LIGHT1, GL_DIFFUSE, color);

        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);

        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,30);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1);

        glEnable(GL_LIGHT1);

        //-- Polygons

        material[0]=1.0; material[1]=1.0; material[2]=1.0; material[3]=1.0;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);


        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < columns; col++)
            {
                printBlock = mapToPrint[col][row];

                if (printBlock.isWall())
                {
                    printElements(Elements::BLOCK, col, row);
                }

                else
                {
                    printElements(Elements::GROUND, col, row);
                }

                if (printBlock.isFood())
                {
                    printElements(Elements::FOOD, col, row);
                }
            }
        }

        for(std::vector<Ghost>::iterator it = ghosts.begin(); it != ghosts.end(); it++)
        {
            printElements(Elements::GHOST, (*it).getx(), (*it).gety());
        }

        printElements(Elements::PACMAN, player1.getx(), player1.gety());
    }

    glutSwapBuffers();
}


//Keyboard events
void keyboard(unsigned char key,int x,int y)
{
    Ghost ghost;

    switch (key)
    {
        case 'w' :
        case 'W' :
            if (player1.getStatus() == Status::STOP)
                player1.init_movement(200,Status::UP);

            else
            {
                statusNext[0] = Status::UP;
                player1.setStatusNext(statusNext);
            }
            break;

        case 'a' :
        case 'A' :
            if(player1.getStatus() == Status::STOP)
                player1.init_movement(200, Status::LEFT);

            else
            {
                statusNext[0] = Status::LEFT;
                player1.setStatusNext(statusNext);
            }
            break;

        case 'd' :
        case 'D' :
            if (player1.getStatus() == Status::STOP)
                player1.init_movement(200, Status::RIGHT);

            else
            {
                statusNext[0] = Status::RIGHT;
                player1.setStatusNext(statusNext);
            }
            break;

        case 's' :
        case 'S' :
            if (player1.getStatus() == Status::STOP)
                player1.init_movement(200, Status::DOWN);

            else
            {
                statusNext[0] = Status::DOWN;
                player1.setStatusNext(statusNext);
            }
            break;

        //Add Ghost
        case 'g' :
        case 'G' :
            newGhost({columns/2, rows/2}, nGhosts + 1, ia);
            break;

        //Quit Ghost
        case 't' :
        case 'T' :
            if (nGhosts > 0)
            {
                ghosts.pop_back();
                nGhosts--;
            }
            break;

        //Pause game and show ranking
        case 'e' :
        case 'E' :
            pauseGame = pauseGame ? false : true;
            break;

        //New player
        case 'p' :
        case 'P' :
            if (nPlayers < MAXPLAYERS)
                pacmans[nPlayers == 0 ? nPlayers : nPlayers - 1] = newPacman({columns/2, 1}, nPlayers);
            break;

        //Map controls
        case 'i' :
        case 'I' :
            if(angleBeta <= (90 - 4))
                angleBeta = (angleBeta + 3);
            break;

         case 'k' :
         case 'K' :
            if(angleBeta >= (-90 + 4))
                angleBeta = (angleBeta - 3);
            break;

        case 'j' :
        case 'J' :
            angleAlpha = (angleAlpha + 3)%360;
            break;

        case 'l' :
        case 'L' :
            angleAlpha = (angleAlpha - 3 + 360)%360;
            break;

        case '+': zoom -= 0.005; break;

        case '-': zoom += 0.005; break;

        case '1':
            for (std::vector<Ghost>::iterator it = ghosts.begin(); it != ghosts.end(); it++)
            {
                (*it).setGhostIA(IA::RANDOM);
            }
            break;

        case '2':
            for (std::vector<Ghost>::iterator it = ghosts.begin(); it != ghosts.end(); it++)
            {
                (*it).setGhostIA(IA::REFLEX);
            }
            break;

        case '3':
            for (std::vector<Ghost>::iterator it = ghosts.begin(); it != ghosts.end(); it++)
            {
                (*it).setGhostIA(IA::ALPHABETA);
            }
            break;

        //Esc - Quits the program.
        case 27:
            printf("done.\n");
            exit(1);
            break;
    }
  glutPostRedisplay();
}


void idle()
{
    long  t;
    Block block;
    float x, y;
  //player1 = pacmans[0]; //Upgrade
    if (!gameEnded && !pauseGame)
    {
        t = glutGet(GLUT_ELAPSED_TIME);

        if (last_t == 0)
            last_t = t;

        else
        {
            player1.integrate(t - last_t);
            x = player1.getx();
            y = player1.gety();

            if ((x - trunc(x) == 0.0 && y - trunc(y) == 0.0 ))
            {
                block = mapToPrint[x][y];
                if (block.isFood())
                {
                    block.setFood(false);
                    mapToPrint[x][y] = block;
                    player1.sumPoints(1);
                }
            }


            for (std::vector<Ghost>::iterator it = ghosts.begin(); it != ghosts.end(); it++)
            {
                (*it).run();

                (*it).integrate(t - last_t);

                (*it).setPacmanPosition(x, y);

                if ((*it).isPacmanDead())
                {
                    player1.removeLives();
                    if(player1.getLives() == 0)
                    {
                        player1.setStatus(Status::DEAD);
                        endGame(false);
                    }
                    else
                    {
                        player1.setStatus(Status::STOP);
                        player1.setPosition({1, rows - 2});

                        for (std::vector<Ghost>::iterator iter = ghosts.begin(); iter != ghosts.end(); iter++)
                        {
                            (*iter).respawn();
                        }
                    }
                    break;
                }
            }
            last_t = t;
        }

        //printf("Player1 points: %d\n",player1.getPoints());
        if (player1.getPoints() - pacmanPoints == totalFood)
        {
            pacmanPoints += player1.getPoints();
            endGame(true);
        }


    }

    glutPostRedisplay();
}

void processArduino()
{
    std::string line;
    std::string delimiter = ";";
    size_t pos = 0;
    std::string token;
    int sensorValues[5];


    while(true)
    {
    if (myfile.is_open())
        {
            int i = 0;
            getline(myfile,line);
            if (line.size() > 11)
            {
                while ((pos = line.find(delimiter)) != std::string::npos)
                {
                    token = line.substr(0, pos);
                    sensorValues[i] = atoi(token.c_str());
                    //std::cout << token << std::endl;
                    line.erase(0, pos + delimiter.length());
                    i++;
                }
                std::cout << "Pulse: "<<sensorValues[0] << std::endl;
                std::cout << "Skin: "<< sensorValues[1] << std::endl;
               // sensorValues[i] = atoi(line.c_str());

                if(sensorValues[2] < 45 && 300 < sensorValues[3] < 800)
                {
                    keyboard('s',0,0);
                }
                else if(sensorValues[2] > 630 && 200 < sensorValues[3] < 600)
                {
                    keyboard('w',0,0);
                }
                 else if(sensorValues[3] > 630 && 400 < sensorValues[2]< 800 )
                {
                    keyboard('d',0,0);
                }
                 else if(sensorValues[3] < 45 && 200 < sensorValues[2] < 600)
                {
                    keyboard('a',0,0);
                }

                if (arraySensorGSR.size() < BGC_ARRAYSIZE)
                {
                    arraySensorGSR.push_back(sensorValues[1]);
                    //cout << "gsr" << endl;
                }

                if (arraySensorBPM.size() < BGC_ARRAYSIZE)
                {
                    arraySensorBPM.push_back(sensorValues[0]);
                    //cout << "bpm" << endl;
                }

                if (arraySensorBPM.size() == BGC_ARRAYSIZE && arraySensorGSR.size() == BGC_ARRAYSIZE)
                {
                    int bpm = 0;
                    int gsr = 0;
                    cout << "-------------------------" << endl;
                    for (int i = 0; i <= BGC_ARRAYSIZE; i++)
                    {
                        bpm += arraySensorBPM[i];
                    }

                    for (int i = 0; i <= BGC_ARRAYSIZE; i++)
                    {
                        gsr += arraySensorGSR[i];
                    }

                    bpm /= BGC_ARRAYSIZE;
                    gsr /= BGC_ARRAYSIZE;
                    cout <<"Mitja bpm:" <<bpm << endl;
                    cout <<"Mitja gsr:" <<gsr << endl;
                    cout << "-------------------------" << endl;
                    if (bpm < 80 && gsr > 1000)
                    //if (bpm > 65 && gsr > 900)
                        bgcDificulty(true);

                    else if (bpm > 90 && gsr < 900)
                        bgcDificulty(false);

                    arraySensorBPM.clear();
                    arraySensorGSR.clear();
                    //std::fill_n(arraySensorBPM, BGC_ARRAYSIZE, 0);
                    //std::fill_n(arraySensorGSR, BGC_ARRAYSIZE, 0);*/
                }
            }
        }
    }
}

void newGame()
{
    MapAlgorithm pacmanMap = MapAlgorithm();

    mapToPrint = pacmanMap.setUpMap(argc, argv, level, level);
    totalFood  = pacmanMap.getTotalFood();
    columns    = mapToPrint[0].size();
    rows       = mapToPrint[1].size();

    //Set pacman into the map
    pacmans[0] = newPacman({1, rows - 2}, 1);
    player1    = pacmans[0];
    player1.setLives(3);
    player1.sumPoints(pacmanPoints);

    //Remove remaining ghosts
    while (nGhosts > 0)
    {
        ghosts.pop_back();
        nGhosts--;
    }

    //Set ghosts into the map
    for (int i = 1; i <= level; i++)
    {
        newGhost({columns/2, rows/2}, nGhosts + 1, ia);
    }

}

void run()
{
    saveRanking(RANKING_FILE_NAME);
    newGame();

    thread t1(processArduino);
    //t1.join();
    glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Pacman");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);



	glutDisplayFunc(printMap);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glBindTexture(GL_TEXTURE_2D,0);
    LoadTexture("grass_side.jpg",64);
    glBindTexture(GL_TEXTURE_2D,1);
    LoadTexture("grass_top.jpg",64);
    glBindTexture(GL_TEXTURE_2D,2);
    LoadTexture("ground.jpg",64);
    glBindTexture(GL_TEXTURE_2D,3);
    LoadTexture("human.jpg",64);
    glBindTexture(GL_TEXTURE_2D,4);
    LoadTexture("creeper.jpg",64);
    glBindTexture(GL_TEXTURE_2D,5);
    LoadTexture("diamon_block.jpg",64);
    glBindTexture(GL_TEXTURE_2D,6);
    LoadTexture("gravel.jpg",64);


    glutMainLoop();
}

int main(int _argc, char *_argv[])
{
    /*int   mapArguments = 2;
    char *mapInputs[]  = {"2", "3"};*/
    argc = _argc;
    argv = _argv;

    run();

	return 0;
}







