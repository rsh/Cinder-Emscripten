#include "cinder/app/AppBasic.h"
#include "cinder/audio/Input.h"
#include "cinder/audio/FftProcessor.h"
#include <iostream>
#include <vector>

using namespace ci;
using namespace ci::app;

class AudioInputSampleApp : public AppBasic {
 public:
	void setup();
	void update();
	void draw();
	void drawFft();
	void drawWaveForm();
	
	audio::Input mInput;
	boost::shared_ptr<float> mFftDataRef;
	audio::PcmBuffer32fRef mPcmBuffer;
};

void AudioInputSampleApp::setup()
{
	const std::vector<audio::InputDeviceRef>& devices = audio::Input::getDevices();
	for( std::vector<audio::InputDeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter ) {
		console() << (*iter)->getName() << std::endl;
	}

	//initialize the audio Input, using the default input device
	mInput = audio::Input( devices[0] );
	
	//tell the input to start capturing audio
	mInput.start();
	
}

void AudioInputSampleApp::update()
{
	uint16_t bandCount = 512;
	mPcmBuffer = mInput.getPcmBuffer();
	if( ! mPcmBuffer ) {
		return;
	}
	mFftDataRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), bandCount );
}

void AudioInputSampleApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	
	glPushMatrix();
		drawFft();
		glTranslatef( 0.0f, 200.0f, 0.0f );
		drawWaveForm();
	glPopMatrix();
}

void AudioInputSampleApp::drawWaveForm()
{
	if( ! mPcmBuffer ) {
		return;
	}
	
	uint32_t bufferSamples = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	//audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );

	int displaySize = getWindowWidth();
	int endIdx = bufferSamples;
	
	float scale = displaySize / (float)endIdx;
	
	glColor3f( 1.0f, 0.5f, 0.25f );
	glBegin( GL_LINE_STRIP );
	for( int i = 0; i < endIdx; i++ ) {
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		glVertex2f( ( i * scale ) , y );
	}
	glEnd();
	
	/*glColor3f( 1.0f, 0.96f, 0.0f );
	glBegin( GL_LINE_STRIP );
	for( int i = 0; i < endIdx; i++ ) {
		float y = ( ( rightBuffer->mData[i] - 1 ) * - 100 );
		glVertex2f( ( i * scale ) , y );
	}
	glEnd();*/
}

void AudioInputSampleApp::drawFft()
{
	uint16_t bandCount = 512;
	float ht = 1000.0f;
	float bottom = 150.0f;
	
	if( ! mFftDataRef ) {
		return;
	}
	
	float * fftBuffer = mFftDataRef.get();
	
	for( int i = 0; i < ( bandCount ); i++ ) {
		float barY = fftBuffer[i] / bandCount * ht;
		glBegin( GL_QUADS );
			glColor3f( 255.0f, 255.0f, 0.0f );
			glVertex2f( i * 3, bottom );
			glVertex2f( i * 3 + 1, bottom );
			glColor3f( 0.0f, 255.0f, 0.0f );
			glVertex2f( i * 3 + 1, bottom - barY );
			glVertex2f( i * 3, bottom - barY );
		glEnd();
	}
}


CINDER_APP_BASIC( AudioInputSampleApp, RendererGl )