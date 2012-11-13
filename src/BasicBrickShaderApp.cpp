#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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
    
    gl::Texture mTex;
    gl::Texture mTex2;
    gl::Texture mTex3;
    
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
    
    mLightPos = Vec3f(0.0f,0.0f, 100.0f);
    gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();
    

    
    mTex = gl::Texture( loadImage( loadResource( "et.jpg" ) ) );
    mTex2 = gl::Texture( loadImage( loadResource( "nt.jpg" ) ) );
    mTex3 = gl::Texture( loadImage( loadResource( "mt.png" ) ) );
    
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
    mTex.bind(0);
    mTex2.bind(1);
    mTex3.bind(2);
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    mShader.bind();
    mShader.uniform("LightPosition", mLightPos);
    mShader.uniform("EarthDay", 0);
    mShader.uniform("EarthNight", 1);
    mShader.uniform("EarthCloudGloss", 2);
    gl::drawSphere(Vec3f::zero(), 70.0f);
    //gl::drawSphere(Vec3f::zero(), 50.0f);
    mShader.unbind();

}

CINDER_APP_BASIC( BasicBrickShaderApp, RendererGl )
