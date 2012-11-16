#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;


// 노이즈 펑션에 피료한거

#define MAXB 0x100
#define N 0x1000
#define NP 12   // 2^N
#define NM 0xfff

#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup_(i, b0, b1, r0, r1)\
t = vec[i] + N;\
b0 = ((int)t) & BM;\
b1 = (b0+1) & BM;\
r0 = t - (int)t;\
r1 = r0 - 1.;
#define at2(rx, ry) ( rx * q[0] + ry * q[1] )
#define at3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

// 각종 자료들

int noise3DTexSize = 128;
GLuint noise3DTexName = 0;  // Texture Object
GLubyte *noise3DTexPtr;

static int p[MAXB + MAXB + 2];
static double g3[MAXB + MAXB + 2][3];
static double g2[MAXB + MAXB + 2][2];
static double g1[MAXB + MAXB + 2];

void make3DNoiseTexture();
static void initNoise();

int start;
int B;
int BM;

void setNoiseFrequency(int frequency)
{
	start = 1;
	B = frequency;
	BM = B-1;
}

double noise3(double vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;
    
	if (start)
	{
		start = 0;
		initNoise();
	}
    
	setup_(0, bx0, bx1, rx0, rx1);
	setup_(1, by0, by1, ry0, ry1);
	setup_(2, bz0, bz1, rz0, rz1);
    
	i = p[bx0];
	j = p[bx1];
    
	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];
    
	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);
    
	q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
	q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
	a = lerp(t, u, v);
    
	q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0);
	q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0);
	b = lerp(t, u, v);
    
	c = lerp(sy, a, b);
    
	q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1);
	q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1);
	a = lerp(t, u, v);
    
	q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1);
	q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1);
	b = lerp(t, u, v);
    
	d = lerp(sy, a, b);
    
	return lerp(sz, c, d);
}

void normalize2(double v[2])
{
	double s;
    
	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void normalize3(double v[3])
{
	double s;
    
	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void initNoise()
{
	int i, j, k;
    
	srand(30757);
	for (i = 0; i < B; i++)
	{
		p[i] = i;
		g1[i] = (double)((rand() % (B + B)) - B) / B;
        
		for (j = 0; j < 2; j++)
			g2[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);
        
		for (j = 0; j < 3; j++)
			g3[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}
    
	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}
    
	for (i = 0; i < B + 2; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}

void make3DNoiseTexture()
{
    int f, i, j, k, inc;
    int startFrequency = 4;
    int numOctaves = 4;
    double ni[3];
    double inci, incj, inck;
    int frequency = startFrequency;
    GLubyte *ptr;
    double amp = 0.5;
    
    if( (noise3DTexPtr = (GLubyte *) malloc(noise3DTexSize * noise3DTexSize * noise3DTexSize * 4)) == NULL ) {
        fprintf(stderr, "ERROR: Coud not allocate 3D noise texture\n");
        exit(1);
    }
    
    for (f = 0, inc = 0; f < numOctaves; ++f, frequency *=2, ++inc, amp *= 0.5) {
        setNoiseFrequency(frequency);
        ptr = noise3DTexPtr;
        ni[0] = ni[1] = ni[2] = 0;
        
        inci = 1.0 / (noise3DTexSize / frequency);
        for (i = 0; i < noise3DTexSize; ++i, ni[0] += inci) {
            incj = 1.0 / (noise3DTexSize / frequency);
            for (j = 0; j < noise3DTexSize;  ++j, ni[1] += incj) {
                inck = 1.0 / (noise3DTexSize / frequency );
                for (k = 0; k < noise3DTexSize; ++k, ni[2] +=inck, ptr += 4) {
                    *(ptr + inc) = (GLubyte) (((noise3(ni) + 1.0) * amp) *128.0);
                }
            }
        }
    }
}











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
    Perlin mPerlin;
    
    GLubyte     *noise2DTexPtr;
    GLuint      noise2DTexName[2];
    int         noise2DTexSize;
    
    float mScale;
    Vec3f   mSkyColor;
    Vec3f   mCloudColor;
    Vec3f   mOffset;
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
    
    // 표면
    

    Surface8u surface = Surface8u( loadImage( loadResource("rawTex1.jpg") ) );
    
    // 생성되는 표면
    
    noise2DTexSize = 128;
    noise2DTexPtr = (GLubyte *) malloc(noise2DTexSize * noise2DTexSize * 4);
    
    for (int i = 0; i < noise2DTexSize; ++i) {
        for(int j =0; j < noise2DTexSize; ++j) {
            float val = (mPerlin.fBm(Vec2f(i * 0.1, j * 0.1)) + 1) / 2 * 255.0;
            
            int pos = (noise2DTexSize * i + j ) * 4;
            * (noise2DTexPtr + pos ) = (GLubyte) val;   // R
            * (noise2DTexPtr + pos + 1 ) = (GLubyte) val;   // G
            * (noise2DTexPtr + pos + 2 ) = (GLubyte) val;   // B
            * (noise2DTexPtr + pos + 3 ) = (GLubyte) val;   // A
        }
    }
    
    // 생성되는 3d 텍스쳐 데이터
    
    make3DNoiseTexture();
    
    
    // 텍스쳐 오브젝트 생성
    glGenTextures(2, noise2DTexName);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noise2DTexName[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface.getWidth(), surface.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, surface.getData());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, noise2DTexSize, noise2DTexSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise2DTexPtr);
    

    glGenTextures(1, &noise3DTexName);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, noise3DTexName);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, noise3DTexSize, noise3DTexSize, noise3DTexSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise3DTexPtr);
    



    
    
    
    mLightPos = Vec3f(0.0f,0.0f, 300.0f);
    
    mScale = 0.02;
    mSkyColor = Vec3f(0.0, 0.0, 0.8);
    mCloudColor = Vec3f(0.8, 0.8, 0.8);
    mOffset = Vec3f::zero();
    
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
    float m = 0.1 * getElapsedSeconds();
    mOffset = Vec3f(m,m,m);
}

void BasicBrickShaderApp::draw()
{
    gl::setMatrices(mCam);
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    //gl::drawSphere(Vec3f::zero(), 50.0f);
    
    mShader.bind();
    mShader.uniform("LightPos" , mLightPos);
    mShader.uniform("Scale", mScale);
    mShader.uniform("Noise", 1);
    mShader.uniform("SkyColor", mSkyColor);
    mShader.uniform("CloudColor", mCloudColor);
    mShader.uniform("Offset", mOffset);
    
    //gl::drawCube(Vec3f::zero(), Vec3f(50.0f,50.0f,50.0f));
      gl::drawSphere(Vec3f::zero(), 50.0f);
      gl::pushMatrices();
      gl::translate(Vec3f(100.0f, 0.0f, 0.0f));
      gl::drawCube(Vec3f::zero(), Vec3f(50.0f,50.0f,50.0f));
      gl::popMatrices();
      mShader.unbind();
}

CINDER_APP_BASIC( BasicBrickShaderApp, RendererGl )
