#pragma once

#include "Image.h"



struct AVFormatContext;
struct AVPacket;
struct AVCodec;
struct AVFrame;
struct AVCodecContext;
struct SwsContext;


namespace vrlib
{
	class Texture;


	class Video : public Image
	{
		int videoStream;
		AVFormatContext *pFormatCtx = NULL;
		AVPacket* packet;
		AVCodec *pCodec = NULL;
		AVFrame *pFrame = NULL;
		AVFrame *pFrameRGB = NULL;
		struct SwsContext *sws_ctx = NULL;
		AVCodecContext *pCodecCtxOrig = NULL;
		AVCodecContext *pCodecCtx = NULL;
		int frameFinished;
		float videoFrameTime = 0;


		Texture* texture;
	public:
		Video(const std::string &fileName, vrlib::Texture* texture);

		void update(float elapsedTime);
	};
}