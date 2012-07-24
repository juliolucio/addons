#include "ofxFlyingCamera.h"
//---------------------------------------------------------------------------------------------------------
void ofxFlyingCamera::setup( string theFileName ){
	//if( load( theFileName ) != "ok")
	//	printf( "Error Enviroment:Couyyldt load file \n" );

	cameraBufferSettings.width				= ofGetWidth();				// width of images attached to fbo
    cameraBufferSettings.height				= ofGetHeight();				// height of images attached to fbo
    cameraBufferSettings.numColorbuffers	= 1;				// how many color buffers to create
    cameraBufferSettings.useDepth			= true;				// whether to use depth buffer or not
    cameraBufferSettings.useStencil			= false;			// whether to use stencil buffer or not
    cameraBufferSettings.textureTarget		= GL_TEXTURE_2D;	// GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_ARB
    cameraBufferSettings.internalformat		= GL_RGBA;			// GL_RGBA, GL_RGBA16F_ARB, GL_RGBA32F_ARB, GL_LUMINANCE32F_ARB etc.
    cameraBufferSettings.wrapModeHorizontal	= GL_CLAMP_TO_EDGE;	// GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER etc.
    cameraBufferSettings.wrapModeVertical	= GL_CLAMP_TO_EDGE;	// GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER etc.
    cameraBufferSettings.minFilter			= GL_LINEAR;		// GL_NEAREST, GL_LINEAR etc.
    cameraBufferSettings.maxFilter			= GL_LINEAR;		// GL_NEAREST, GL_LINEAR etc.
    cameraBufferSettings.numSamples			= 0;                 // number of samples for multisampling (set 0 to disable)if( isSimpleRender ) 
		    
	frameBufferCamera = new ofFbo();
    frameBufferCamera->allocate( cameraBufferSettings );

	//loading shader
	printf( "\nLoading camera:  ---- shaders/cameraTexturingMap -----\n" );	
    shaderTexturing.load( ofToDataPath( "shaders/cameraTexturingMap" ) );

	shaderTexturingHandle = shaderTexturing.getProgram();	

	printf( "Getting shader indexes for uniform parameters for cameraTexturingMap shader\n"  );	
	uniformParamIndexModelTexture	= glGetUniformLocationARB( shaderTexturingHandle , "modelTexture");	
	printf( "	Parameter index for modelTexture		=   %d\n" , uniformParamIndexModelTexture );  

	//loading shader shaderColor textured -----------------------------------------
	printf( "\nLOADING SHADER:  ---- shaders/colorMap -----\n" );	
    shaderColor.load( ofToDataPath( "shaders/colorMap" ) );
	//storing shader handle
	shaderColorHandle = shaderColor.getProgram();	

	load( theFileName );

	cameraAspectRatio = ofGetWidth() / ofGetHeight();
	
	cameraRight		= cameraNear * tan( cameraAngle * ( ( 2 * PI ) / 360.0f ) );
	cameraLeft		= -cameraRight;
	cameraTop		= cameraRight / cameraAspectRatio;
	cameraBottom	= -cameraTop;

	cameraDeltaRadius	= 0;
	cameraDeltaTeta		= 0;
	cameraDeltaFi		= 0;

	cameraEye.x = cameraRadius * cos( cameraTeta * ( ( 2 * PI ) / 360.0f )) * sin( cameraFi * ( ( 2 * PI ) / 360.0f ) );
	cameraEye.y = cameraRadius * sin( cameraTeta * ( ( 2 * PI ) / 360.0f )) * sin( cameraFi * ( ( 2 * PI ) / 360.0f ) );
	cameraEye.z = cameraRadius * cos( cameraFi	 * ( ( 2 * PI ) / 360.0f ));

	haveToUpdateCamera = true;
	isDrawingTrihedrum = false;
}
//---------------------------------------------------------------------------------------------------------
string ofxFlyingCamera::load( string theFileName ){
	string path = "cameras/";
	fileName = path + theFileName;
	fileName = ofToDataPath( fileName );
	fileIn.open( fileName.c_str() , std::ios_base::binary | std::ios_base::in );
	if( !fileIn.is_open() ){
		fileIn.close();
		return "ofxFlyingCamera ERROR: Couldn't load file \n";
	}

	string dataName;
	int dataValueInt;
	string dataValueString;

	fileIn >> dataName;
	if( dataName != "version=" ){
		fileIn.close();
		return "ofxFlyingCamera ERROR: Bad project Info file format loading version\n";
	}
	fileIn >> dataValueString;

	if( dataValueString == "0.1" ){
        
        fileIn >> dataName;
		if( dataName != "cameraAngle=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraAngle\n";
		}
		fileIn >> cameraAngle;
        
        fileIn >> dataName;
		if( dataName != "cameraFar=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraFar\n";
		}
		fileIn >> cameraFar;
        
        fileIn >> dataName;
		if( dataName != "cameraNear=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraNear\n";
		}
		fileIn >> cameraNear;
        
        
        fileIn >> dataName;
		if( dataName != "cameraCenter=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraCenter\n";
		}
		fileIn >> cameraCenter.x;
		fileIn >> cameraCenter.y;
		fileIn >> cameraCenter.z;

		fileIn >> dataName;
		if( dataName != "cameraUp=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraUp\n";
		}

		fileIn >> cameraUp.x;
		fileIn >> cameraUp.y;
		fileIn >> cameraUp.z;

		fileIn >> dataName;
		if( dataName != "cameraRadius=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraRadius\n";
		}
		fileIn >> cameraRadius;
        
        fileIn >> dataName;
		if( dataName != "cameraTeta=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraTeta\n";
		}
		fileIn >> cameraTeta;
        
        fileIn >> dataName;
		if( dataName != "cameraFi=" ){
			fileIn.close();
			return "ofxFlyingCamera ERROR: Bad project Info file format loading cameraFi\n";
		}
		fileIn >> cameraFi;

        fileIn.close();
		return "ok";
	}

	else{
		fileIn.close();
		return "ofxFlyingCamera ERROR: Bad file version\n";
	}
}
//---------------------------------------------------------------------------------------------------------
string ofxFlyingCamera::save(  string theFileName  ){
	string path = "cameras/";
	fileName = path + theFileName;
	fileName = ofToDataPath( fileName );
	fileOut.open( fileName.c_str() , std::ios_base::binary | std::ios_base::out );
	if( !fileOut.is_open() ){
		fileOut.close();
		return "ofxFlyingCamera ERROR: Couldn't save file\n";
	}
	
	fileOut << "version= ";
	fileOut << FILE_VERSION;
	fileOut << "\n";
        
	fileOut << "cameraAngle=\t ";
	fileOut << cameraAngle;
	fileOut << "\n";	
    
	fileOut << "cameraFar=\t ";
	fileOut << cameraFar;
	fileOut << "\n";	
    
	fileOut << "cameraNear=\t ";
	fileOut << cameraNear;
	fileOut << "\n";	
   
	fileOut << "cameraCenter=\t ";
	fileOut << cameraCenter.x;
	fileOut << "\t";
	fileOut << cameraCenter.y;
	fileOut << "\t";
	fileOut << cameraCenter.z;
	fileOut << "\n";
		
	fileOut << "cameraEye=\t ";
	fileOut << cameraEye.x;
	fileOut << "\t";
	fileOut << cameraEye.y;
	fileOut << "\t";
	fileOut << cameraEye.z;
	fileOut << "\n";
		
	fileOut << "cameraRadius=\t ";
	fileOut << cameraRadius;
	fileOut << "\n";

	fileOut << "cameraTeta=\t ";
	fileOut << cameraTeta;
	fileOut << "\n";	
    
	fileOut << "cameraFi=\t ";
	fileOut << cameraFi;
	fileOut << "\n";	
    
	
	fileOut.close();
	return "ok";
}
//--------------------------------------------------------------
void ofxFlyingCamera::update(){		
	if( haveToUpdateCamera ){
		cameraRadius	+= cameraDeltaRadius;
		cameraTeta		+= cameraDeltaTeta;
		cameraFi		+= cameraDeltaFi;
		
		//clamping
		if( cameraRadius > 1000 ) 
			cameraRadius = 1000;
		if( cameraRadius <= 10 * cameraNear ) 
			cameraRadius = 10 * cameraNear;
		if( cameraTeta > 360 )
			cameraTeta = 0;
		if( cameraTeta < 0 )
			cameraTeta= 360;
		if( cameraFi >= 180 )
			cameraFi = 180;
		if( cameraFi <= 0.00001 )
			cameraFi = 0.00001;		

		cameraEye.x = cameraRadius * cos( cameraTeta * ( ( 2 * PI ) / 360.0f )) * sin( cameraFi * ( ( 2 * PI ) / 360.0f ) );
		cameraEye.y = cameraRadius * sin( cameraTeta * ( ( 2 * PI ) / 360.0f )) * sin( cameraFi * ( ( 2 * PI ) / 360.0f ) );
		cameraEye.z = cameraRadius * cos( cameraFi	 * ( ( 2 * PI ) / 360.0f ));

		haveToUpdateCamera = false;
		cameraDeltaRadius = 0;
		cameraDeltaTeta = 0;
		cameraDeltaFi = 0;
		
		//apllying frustrum
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum( cameraLeft , cameraRight , cameraBottom  , cameraTop , cameraNear , cameraFar );
	}
}
//--------------------------------------------------------------
void ofxFlyingCamera::begin(){
	
	glActiveTexture( GL_TEXTURE0 );
	glGetError();
	glBindTexture( GL_TEXTURE_2D , 0 );	
	glGetError();
    
	frameBufferCamera->begin();	
    
    //binding shader
	//shaderTexturing.begin();
    shaderColor.begin();

   /* glGetError();
    glUniform1i( uniformParamIndexModelTexture , 0 );
    GLint error =  glGetError();
    if(error )
        printf( "** ERROR: Setting param modekl texture in shader texturing\n");
    */
    
    //setting GL state
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    //clearing buffers before drawing
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();	
	gluLookAt( cameraEye.x , cameraEye.y , cameraEye.z , cameraCenter.x , cameraCenter.y , cameraCenter.z , cameraUp.x , cameraUp.y , cameraUp.z );
		
	drawTrihedrum();
}
//--------------------------------------------------------------
void ofxFlyingCamera::end(){    
    glActiveTexture( GL_TEXTURE0 );
    glGetError();
    glBindTexture( GL_TEXTURE_2D ,0 );	
    glGetError();

	//shaderTexturing.end();
	shaderColor.end();
	
	//finishing frame buffer draw
	frameBufferCamera->end();

	ofSetColor( 255 , 255, 255 );

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);	
}
//--------------------------------------------------------------
void ofxFlyingCamera::draw( int x , int y , int width , int height ){ 
	
	ofSetColor( 0x555555 );
	if( width >= height ){
		ofRect( x - 5 , y - 5 , width + 10 , width + 10 );
		frameBufferCamera->draw( x , y , width , width );
	}
	else{
		ofRect( x - 5 , y - 5 , height + 10 , height + 10 );
		frameBufferCamera->draw( x , y , height , height );
	}
}
//--------------------------------------------------------------
void ofxFlyingCamera::setDeltaCameraSphericalCoordinatesRadius( float theDeltaRadius ){
	cameraDeltaRadius	= theDeltaRadius;
	haveToUpdateCamera	= true;
}
//--------------------------------------------------------------
void ofxFlyingCamera::setDeltaCameraSphericalCoordinatesTeta( float theDeltaTeta ){
	cameraDeltaTeta		= theDeltaTeta;
	haveToUpdateCamera	= true;
}
//--------------------------------------------------------------
void ofxFlyingCamera::setDeltaCameraSphericalCoordinatesFi( float theDeltaFi ){
	cameraDeltaFi		= theDeltaFi;
	haveToUpdateCamera	= true;
}
//--------------------------------------------------------------
float ofxFlyingCamera::getCameraRadius( ){
	return cameraRadius;
}
//--------------------------------------------------------------
float ofxFlyingCamera::getCameraTeta( ){
	return cameraTeta;
}
//--------------------------------------------------------------
float ofxFlyingCamera::getCameraFi( ){
	return cameraFi;
}   
//--------------------------------------------------------------
void ofxFlyingCamera::setCameraRadius( float theRadius ){
	cameraRadius = theRadius;
}
//--------------------------------------------------------------
void ofxFlyingCamera::setCameraTeta( float theTeta ){
	cameraTeta = theTeta;
}
//--------------------------------------------------------------
void ofxFlyingCamera::setCameraFi( float theFi ){
	cameraFi = theFi;
}
//--------------------------------------------------------------
void ofxFlyingCamera::setDrawTrihedrum( bool haveToisDrawTrihedrum ){
	isDrawingTrihedrum = haveToisDrawTrihedrum;
}
//--------------------------------------------------------------
void ofxFlyingCamera::drawTrihedrum(){
	if( isDrawingTrihedrum ){
		ofMesh trihedrum;
		float side = 100;

		//face xy
		trihedrum.addVertex( ofVec3f( 0 , 0 , 0 ) );
		trihedrum.addVertex( ofVec3f( side , 0 , 0 ) );
		trihedrum.addVertex( ofVec3f( 0 , side , 0 ) );
		trihedrum.addVertex( ofVec3f( side , side , 0 ) );
	
		trihedrum.addColor( ofColor( 0 , 0 , 0 ));
		trihedrum.addColor( ofColor( 255 , 0 , 0 ));
		trihedrum.addColor( ofColor( 0 , 255 , 0 ));
		trihedrum.addColor( ofColor( 255 , 255 , 0 ));

		trihedrum.addNormal( ofVec3f( 0 , 0 , 1 ) );
		trihedrum.addNormal( ofVec3f( 0 , 0 , 1 ) );
		trihedrum.addNormal( ofVec3f( 0 , 0 , 1 ) );
		trihedrum.addNormal( ofVec3f( 0 , 0 , 1 ) );

		trihedrum.addIndex( 0 );
		trihedrum.addIndex( 1 );
		trihedrum.addIndex( 3 );

		trihedrum.addIndex( 0 );
		trihedrum.addIndex( 3 );
		trihedrum.addIndex( 2 );

		//face YZ
		trihedrum.addVertex( ofVec3f( 0 , 0 , 0 ) );
		trihedrum.addVertex( ofVec3f( 0 , side , 0 ) );
		trihedrum.addVertex( ofVec3f( 0 , 0 ,side ) );
		trihedrum.addVertex( ofVec3f( 0 , side , side ) );
	
		trihedrum.addColor( ofColor( 0 , 0 , 0 ));
		trihedrum.addColor( ofColor( 0 , 255 , 0 ));
		trihedrum.addColor( ofColor( 0 , 0 , 255 ));
		trihedrum.addColor( ofColor( 0 , 255 , 255 ));

		trihedrum.addNormal( ofVec3f( 1 , 0 , 0 ) );
		trihedrum.addNormal( ofVec3f( 1 , 0 , 0 ) );
		trihedrum.addNormal( ofVec3f( 1 , 0 , 0 ) );
		trihedrum.addNormal( ofVec3f( 1 , 0 , 0 ) );

		trihedrum.addIndex( 4 );
		trihedrum.addIndex( 5 );
		trihedrum.addIndex( 7 );

		trihedrum.addIndex( 4 );
		trihedrum.addIndex( 7 );
		trihedrum.addIndex( 6 );

		//face XZ
		trihedrum.addVertex( ofVec3f( 0 , 0 , 0 ) );
		trihedrum.addVertex( ofVec3f( side , 0 , 0 ) );
		trihedrum.addVertex( ofVec3f( 0 , 0 ,side ) );
		trihedrum.addVertex( ofVec3f( side , 0 , side ) );
	
		trihedrum.addColor( ofColor( 0 , 0 , 0 ));
		trihedrum.addColor( ofColor( 255 , 0 , 0 ));
		trihedrum.addColor( ofColor( 0 , 0 , 255 ));
		trihedrum.addColor( ofColor( 255 , 0 , 255 ));

		trihedrum.addNormal( ofVec3f( 0 , 1 , 0 ) );
		trihedrum.addNormal( ofVec3f( 0 , 1 , 0 ) );
		trihedrum.addNormal( ofVec3f( 0 , 1 , 0 ) );
		trihedrum.addNormal( ofVec3f( 0 , 1 , 0 ) );

		trihedrum.addIndex( 8 );
		trihedrum.addIndex( 9 );
		trihedrum.addIndex( 11 );

		trihedrum.addIndex( 8 );
		trihedrum.addIndex( 11 );
		trihedrum.addIndex( 10 );

		trihedrum.drawWireframe();
	}
}