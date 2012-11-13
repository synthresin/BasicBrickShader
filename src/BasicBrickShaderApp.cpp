#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "CubeMap.h"

using namespace ci;
using namespace ci::app;
using namespace std;
const int		CUBE_MAP_LOC = 4;


class BasicBrickShaderApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void mouseMove( MouseEvent event );
	void update();
	void draw();
    CameraPersp mCam;
    gl::GlslProg mShader;
    Vec3f mLightPos;
    float mCameraZ;
    
    CubeMap *mEnvMap;
    Vec3f mBaseColor;
    float mMixRatio;
};

void BasicBrickShaderApp::setup()
{
    mCam.setPerspective(75, getWindowAspectRatio(), 50.0f, 5000.0f);
    
    try {
        mShader = gl::GlslProg( loadResource( "brickVert.glsl" ), loadResource( "brickFrag.glsl" ) );
    } catch (gl::GlslProgCompileExc &exc) {
        std::cout << "Shader compile error: " << std::endl;
        std::cout << exc.what() << std::endl;
    } catch (...) {
        std::cout << "Unable to load shader" << std::endl;
    }
    
    
    gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();
    

    mLightPos = Vec3f(0.0f,0.0f, 300.0f);
    mBaseColor = Vec3f(0.4, 0.4, 1.0);
    mMixRatio = 0.8;
    mEnvMap = new CubeMap( GLsizei(128), GLsizei(128),
                          Surface8u( loadImage( loadResource( "berkeley_positive_x.jpg" ) ) ),
                          Surface8u( loadImage( loadResource( "berkeley_positive_y.jpg" ) ) ),
                          Surface8u( loadImage( loadResource( "berkeley_positive_z.jpg" ) ) ),
                          Surface8u( loadImage( loadResource( "berkeley_negative_x.jpg" ) ) ),
                          Surface8u( loadImage( loadResource( "berkeley_negative_y.jpg" ) ) ),
                          Surface8u( loadImage( loadResource( "berkeley_negative_z.jpg" ) ) )
                          );
    
}

void BasicBrickShaderApp::mouseDown( MouseEvent event )
{
}

void BasicBrickShaderApp::mouseMove(MouseEvent event)
{
    mCameraZ = lmap((float)event.getX(), 0.0f, (float)getWindowWidth(), 100.0f, -100.0f);
}

void BasicBrickShaderApp::update()
{
    mCam.lookAt(Vec3f(100.0f,100.0f, mCameraZ), Vec3f::zero(), Vec3f::yAxis());
    gl::setMatrices(mCam);
}

void BasicBrickShaderApp::draw()
{
    mEnvMap->bindMulti(CUBE_MAP_LOC);
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    mShader.bind();
    mShader.uniform("LightPos", mLightPos);
    mShader.uniform("BaseColor", mBaseColor);
    mShader.uniform("MixRatio", mMixRatio);
    mShader.uniform("EnvMap", CUBE_MAP_LOC);
    gl::drawSphere(Vec3f::zero(), 70.0f);
    //gl::drawSphere(Vec3f::zero(), 50.0f);
    mShader.unbind();

}

CINDER_APP_BASIC( BasicBrickShaderApp, RendererGl )
