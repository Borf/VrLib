#include "Video.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <VrLib/Log.h>
#include <VrLib/Texture.h>
#include <GL/glew.h>

using vrlib::logger;


namespace vrlib
{
	Video::Video(const std::string &fileName, vrlib::Texture* texture) : Image(10,10)
	{
		this->texture = texture;
		if (avformat_open_input(&pFormatCtx, fileName.c_str(), NULL, NULL) != 0)
			logger << "Could not open video" << Log::newline;
		// Retrieve stream information
		if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
			logger << "Couldn't find stream information" << Log::newline;


		unsigned int i;

		// Find the first video stream
		videoStream = -1;
		for (i = 0; i<pFormatCtx->nb_streams; i++)
			if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStream = i;
				break;
			}
		if (videoStream == -1)
			logger << "Didn't find a video stream" << Log::newline;

		// Get a pointer to the codec context for the video stream
		pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;

		// Find the decoder for the video stream
		pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
		if (pCodec == NULL) {
			fprintf(stderr, "Unsupported codec!\n");
		}
		// Copy context
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
			fprintf(stderr, "Couldn't copy codec context");
		}
		// Open codec
		if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
			logger << "Could not open codec" << Log::newline;


		// Allocate video frame
		pFrame = av_frame_alloc();
		pFrameRGB = av_frame_alloc();
		if (pFrameRGB == NULL)
			return;
		uint8_t *buffer = NULL;
		int numBytes;
		// Determine required buffer size and allocate buffer
		numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
		buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

		avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
			pCodecCtx->width, pCodecCtx->height);

		// initialize SWS context for software scaling
		sws_ctx = sws_getContext(pCodecCtx->width,
			pCodecCtx->height,
			pCodecCtx->pix_fmt,
			pCodecCtx->width,
			pCodecCtx->height,
			AV_PIX_FMT_RGB24,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL
		);
		packet = new AVPacket();
		width = pCodecCtx->width;
		height = pCodecCtx->height;
		data = nullptr;
	}



	void Video::update(float elapsedTime)
	{
		videoFrameTime -= elapsedTime + 0.1f;
		if (videoFrameTime < 0)
		{
			videoFrameTime = 1 / 24.0f;
			bool done = true;
			while (av_read_frame(pFormatCtx, packet) >= 0) {
				// Is this a packet from the video stream?
				if (packet->stream_index == videoStream) {
					// Decode video frame
					avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet);

					// Did we get a video frame?
					if (frameFinished) {
						// Convert the image from its native format to RGB
						sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
							pFrame->linesize, 0, pCodecCtx->height,
							pFrameRGB->data, pFrameRGB->linesize);


						{
							texture->bind();
							//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pCodecCtx->width, pCodecCtx->height, 0, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
							//glGenerateMipmap(GL_TEXTURE_2D);
							glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pCodecCtx->width, pCodecCtx->height, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
							glGenerateMipmap(GL_TEXTURE_2D);

							done = false;

						}
						break;
					}
				}
			}
			if (done)
				avformat_seek_file(pFormatCtx, videoStream, 0, 0, 0, 0);

			// Free the packet that was allocated by av_read_frame
			av_free_packet(packet);
		}
	}


}