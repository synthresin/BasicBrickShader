#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"

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
    
    Vec3f mBrickColor, mMortarColor;
    Vec2f mBrickPct, mBrickSize;
    
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
    
    // 벽돌 색깔 설정
    mBrickColor = Vec3f(0.6, 0.2, 0.3);
    mMortarColor = Vec3f(0.7, 0.7, 0.7);
    
    // 벽돌 사이즈 설정
    mBrickSize = Vec2f(10.0, 4.0);
    mBrickPct = Vec2f(0.9, 0.8);
    
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
}

void BasicBrickShaderApp::draw()
{
    gl::setMatrices(mCam);
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    mShader.bind();
    mShader.uniform("LightPosition", mLightPos);
    mShader.uniform("BrickColor", mBrickColor);
    mShader.uniform("BrickPct", mBrickPct);
    mShader.uniform("BrickSize", mBrickSize);
    mShader.uniform("MortarColor", mMortarColor);
    gl::drawCube(Vec3f::zero(), Vec3f(50.0f,50.0f,50.0f));
    //gl::drawSphere(Vec3f::zero(), 50.0f);
    mShader.unbind();
}

CINDER_APP_BASIC( BasicBrickShaderApp, RendererGl )
