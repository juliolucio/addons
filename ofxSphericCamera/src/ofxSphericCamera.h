#ifndef _OFX_SAR_ENVIROMENT_
#define _OFX_SAR_ENVIROMENT_

#define FILE_VERSION 0.1

#include "ofMain.h"
#include "ofMesh.h"

using namespace std;

#include <vector>

class ofxSphericCamera{

private:
	ofVec3f						cameraCenter;
	ofVec3f						cameraEye;
	ofVec3f						cameraUp;
    
    float						cameraRadius;
	float						cameraTeta;
	float						cameraFi;

	float						cameraDeltaRadius;
	float						cameraDeltaTeta;
	float						cameraDeltaFi;

	float						cameraAngle;
	float						cameraAspectRatio;

	float						cameraFar;
	float						cameraNear;

	float						cameraLeft;
	float						cameraRight;
	float						cameraTop;
	float						cameraBottom;

	bool						haveToUpdateCamera;

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
     
	void						setDeltaCameraSphericalCoordinatesRadius( float theDeltaRadius );
	void						setDeltaCameraSphericalCoordinatesTeta	( float theDeltaTeta );
	void						setDeltaCameraSphericalCoordinatesFi	( float theDeltaFi );
	
};

#endif
	
