#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Surface.h"
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
    
    GLuint tex[2];    // Texture Object Array
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
    
    // 텍스쳐 오브젝트 생성
    
    Surface8u texImg1 = Surface8u( loadImage( loadResource( "rawTex1.jpg" ) ) );
    Surface8u texImg2 = Surface8u( loadImage( loadResource( "rawTex2.png" ) ) );
    
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(2, tex);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, texImg2.getData());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
    
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, texImg1.getData());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
    
    
    
    
    
    //glDisable(GL_TEXTURE_2D);
    

    
    
    
    mLightPos = Vec3f(0.0f,0.0f, 200.0f);
    
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
    mShader.uniform("LightPosition" , mLightPos);
    mShader.uniform("tex", 0);
    //gl::drawCube(Vec3f::zero(), Vec3f(50.0f,50.0f,50.0f));
    gl::drawSphere(Vec3f::zero(), 50.0f);
    gl::pushMatrices();
    gl::translate(Vec3f(100.0f, 0.0f, 0.0f));
    gl::drawSphere(Vec3f::zero(), 50.0f);
    gl::popMatrices();
    mShader.unbind();
}

CINDER_APP_BASIC( BasicBrickShaderApp, RendererGl )
