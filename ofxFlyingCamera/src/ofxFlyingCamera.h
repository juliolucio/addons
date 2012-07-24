#ifndef _OFX_FLYING_CAMERA_
#define _OFX_FLYING_CAMERA_

#define FILE_VERSION 0.1

#include "ofMain.h"
#include "ofMesh.h"

using namespace std;

#include <vector>

class ofxFlyingCamera{

private:
	ofVec3f						cameraCenter;
	ofVec3f						cameraEye;
	ofVec3f						cameraUp;
	ofVec3f						cameraEyeCenterDirection;

	float						cameraAngle;
	float						cameraAspectRatio;

	float						cameraFar;
	float						cameraNear;

	float						cameraLeft;
	float						cameraRight;
	float						cameraTop;
	float						cameraBottom;

	float						cameraRadius;
	float						cameraTeta;
	float						cameraFi;

	bool						haveToUpdateCamera;
	bool						isDrawingTrihedrum;

	ofFbo::Settings				cameraBufferSettings;
	ofFbo*						frameBufferCamera;

	//for texturing  shader
	GLuint						activeTextureTexturing;
	ofShader					shaderTexturing;
	GLhandleARB					shaderTexturingHandle;
	GLint						uniformParamIndexModelTexture;

	//for color  shader
	ofShader					shaderColor;
	GLhandleARB					shaderColorHandle;

	ifstream					fileIn;
	ofstream					fileOut;

	string						fileName;

	void						drawTrihedrum			();
public:
	void						setup					( string theFileName );
	string						load					( string theFileName );
	string						save					( string theFileName );
	void						update					( );
	void						clear					( );
    void                        begin                   ( );
    void                        end                    ( );  
	void						draw                    ( int x , int y , int width , int height );

    float						getCameraRadius         ( );
	float						getCameraTeta           ( );
	float						getCameraFi             ( );
    
    void						setCameraRadius         ( float theRadius );
	void						setCameraTeta           ( float theTeta );
	void						setCameraFi             ( float theFi );

	void						setDrawTrihedrum		( bool haveToisDrawTrihedrum );
     
	void						drawCameraView							( int x , int y , int width , int height );
	
	void						setDeltaCameraSphericalCoordinatesGoBackward( float theDeltaRadius );
	void						setDeltaCameraSphericalCoordinatesGoForward	( float theDistance );
	void						setDeltaCameraSphericalCoordinatesOrbitLef	( float theDistance );
	void						setDeltaCameraSphericalCoordinatesOrbitRight( float theDeltaAngle );
	void						setDeltaCameraSphericalCoordinatesOrbitUp	( float theDeltaAngle );
	void						setDeltaCameraSphericalCoordinatesOrbitDown	( float theDeltaAngle );
	
};

#endif
	
