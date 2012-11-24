#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Rand.h"

#define BOX_NUM 500
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FBO_RES 1

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicBrickShaderApp : public AppBasic {
  public:
    
    //MEMBER FUNCTIONS
	void prepareSettings( Settings *settings );
    void setup();
	void mouseDown( MouseEvent event );
    void mouseMove( MouseEvent event );
    void drawIntoFbo();
    void applyEffectToFbo();
	void update();
	void draw();
    
    
    
    // MEMBER VARIABLES FOR A SCENE
    CameraPersp mCam;
    
    Vec3f mLightPos;
    float mCameraZ;
    
    Vec3f mRandVecs[BOX_NUM];
    
    
    // SHADERS
    gl::GlslProg mShader;
    gl::GlslProg mEffectShader;
    
    // FBOS
    
    gl::Fbo mFbo;
    gl::Fbo mEffectFbo;
};

void BasicBrickShaderApp::prepareSettings(Settings *settings)
{
    settings->setWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void BasicBrickShaderApp::setup()
{
   
    // 카메라 퍼스펙티브 설정
    mCam.setPerspective(75, getWindowAspectRatio(), 50.0f, 5000.0f);
    
    
    // 셰이더 설정
    try {
        mShader = gl::GlslProg( loadResource( "brickVert.glsl" ), loadResource( "brickFrag.glsl" ) );
    } catch (gl::GlslProgCompileExc &exc) {
        std::cout << "Shader compile error: " << std::endl;
        std::cout << exc.what() << std::endl;
    } catch (...) {
        std::cout << "Unable to load shader" << std::endl;
    }
    
    // 이펙트 셰이더
    
    try {
        mEffectShader = gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "radialBlur.glsl" ) );
    } catch (gl::GlslProgCompileExc &exc) {
        std::cout << "Shader compile error: " << std::endl;
        std::cout << exc.what() << std::endl;
    } catch (...) {
        std::cout << "Unable to load shader" << std::endl;
    }
    
    
    // 빛의 위치 설정
    mLightPos = Vec3f(0.0f,0.0f, 300.0f);
    gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();
    
    // 박스들의 위치 설정 

    for (int i =0; i< BOX_NUM; i++) {
        mRandVecs[i] = Rand::randVec3f()* Rand::randInt(1, 300);
    }
    
    
    // SETUP FBO
    
    mFbo = gl::Fbo(SCREEN_WIDTH/FBO_RES, SCREEN_HEIGHT/FBO_RES);
    mEffectFbo = gl::Fbo(SCREEN_WIDTH/FBO_RES, SCREEN_HEIGHT/FBO_RES);
    
    
}

void BasicBrickShaderApp::mouseDown( MouseEvent event )
{
}

void BasicBrickShaderApp::mouseMove(MouseEvent event)
{
    mCameraZ = lmap((float)event.getX(), 0.0f, (float)getWindowWidth(), 200.0f, -100.0f);
}



void BasicBrickShaderApp::drawIntoFbo()
{
    mFbo.bindFramebuffer();
    
    gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ), true );
    mCam.setPerspective(75, mFbo.getAspectRatio(), 50.0f, 5000.0f);
    mCam.lookAt(Vec3f(200.0f,200.0f, mCameraZ), Vec3f::zero(), Vec3f::yAxis());
    
    gl::setMatrices(mCam);
    
    float t = ( sinf(getElapsedSeconds() * 5) + 1 )/ 2.0;
    
    mShader.bind();
    mShader.uniform("LightPosition" , mLightPos);
    mShader.uniform("SurfaceColor" , Vec3f(0.8, 0.4, 0.6));
    mShader.uniform("Radius" , 50.0f);
    mShader.uniform("Blend" , t);
    
    for (int i = 0; i < BOX_NUM; i++) {
        gl::pushMatrices();
        gl::translate(mRandVecs[i]);
        gl::drawCube(Vec3f::zero(), Vec3f(10.0f,10.0f,10.0f));
        gl::popMatrices();
    }
    mShader.unbind();
    
    
    mFbo.unbindFramebuffer();
}

void BasicBrickShaderApp::applyEffectToFbo()
{
    mEffectFbo.bindFramebuffer();
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatricesWindow(getWindowSize(), false);
    gl::setViewport(getWindowBounds());
    
    gl::color(1.0, 1.0, 1.0);
    mFbo.getTexture().enableAndBind();
    
    mEffectShader.bind();
    mEffectShader.uniform("tex0", 0);
    mEffectShader.uniform("resolution", Vec2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    mEffectShader.uniform("time", (float)getElapsedSeconds());
    gl::drawSolidRect(getWindowBounds());
    mEffectShader.unbind();
    mEffectFbo.unbindFramebuffer();
}


void BasicBrickShaderApp::update()
{
    drawIntoFbo();
    applyEffectToFbo();
}



void BasicBrickShaderApp::draw()
{
   
    
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatricesWindow(getWindowSize(), false);
    gl::setViewport(getWindowBounds());
    
    gl::color(1.0, 1.0, 1.0);
    mEffectFbo.getTexture().enableAndBind();
    
    gl::drawSolidRect(getWindowBounds());
}

CINDER_APP_BASIC( BasicBrickShaderApp, RendererGl )
