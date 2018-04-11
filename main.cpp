#include <iostream>
#include <stdio.h>
#include "showbmp.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavformat/version.h"
    #include "libswscale/swscale.h"
    #include "libavdevice/avdevice.h"
    #include "libavutil/imgutils.h"
    #include "SDL.h"
    #include "SDL_version.h"
    #include "SDL_error.h"
}

#define __STDC_CONSTANT_MACROS
#undef main

using namespace std;

int main(void)
{
    int             i, videoIndex;
//    int             y_size;
    int             ret;
//    int             got_picture;
    unsigned char   *out_buffer;
    AVCodec         *pCodec;
    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx;
    AVFrame         *pFrame, *pFrameYUV;
    AVStream        *pStream;
    AVPacket        *packet;

    int             screen_w=0, screen_h=0;
    SDL_Window      *screen;
    SDL_Renderer    *sdlRenderer;
    SDL_Texture     *sdlTexture;
    SDL_Rect        sdlRect;

#if OUTPUT_YUV420P
    FILE *fp_yuv;
#endif

    struct SwsContext *img_convert_ctx;
    char kPath[] = "D://Code//QTProject//KyPlayer-master//Girls_H264.avi";

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, kPath, NULL, NULL) != 0)
    {
        printf("Couldn't open input stream.\n");
        return -1;
    }

    //find the valid video streams
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        printf("Couldn't find stream information.\n");
        return -1;
    }

    videoIndex = -1;
    for (i=0; i<(int)pFormatCtx->nb_streams; i++)
    {
        //if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            break;
        }
    }

    if (videoIndex == -1)
    {
        printf("there is no video stream!\n");
        return -1;
    }

    //find the decoder
    //    pCodecCtx = pFormatCtx->streams[videoIndex]->codecpar;
    //    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    pStream = pFormatCtx->streams[videoIndex];
    pCodec = avcodec_find_decoder(pStream->codecpar->codec_id);
    if (pCodec == NULL)
    {
        printf("cannot find the codec!");
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pStream->codecpar);

    //open codec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("cannot open codec!");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    printf("*****************************  File Information *****************************\n");
    av_dump_format(pFormatCtx, 0, kPath, 0);
    printf("*****************************************************************************\n");

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,pCodecCtx->width,
                                     pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

#if OUTPUT_YUV420P
    fp_yuv = fopen("output.yuv", "wb+");
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        printf("cannot initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    screen_w = pCodecCtx->width;
    screen_h = pCodecCtx->height;
    //SDL2.0 support for multiple windows
    screen = SDL_CreateWindow("My First Esay Player!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              screen_w, screen_h, SDL_WINDOW_OPENGL);
    if (!screen)
    {
        printf("SDL create window fail! %s\n", SDL_GetError());
    }

    sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = screen_w;
    sdlRect.h = screen_h;

    //--------- SDL End ---------


    while (av_read_frame(pFormatCtx, packet) >= 0)
    {
        if (packet->stream_index==videoIndex)
        {
            //ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret != 0)
            {
                printf("decode error!\n");
                return -1;
            }
            //if (got_picture)
            if (avcodec_receive_frame(pCodecCtx, pFrame) == 0)
            {
                sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

#if OUTPUT_YUV420P
                y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);  //Y
                fwrite(pFrameYUV->data[1], 1, y_size, fp_yuv);  //U
                fwrite(pFrameYUV->data[2], 1, y_size, fp_yuv);  //V
#endif

                //--------- SDL Start ---------
#if 0
                SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
#else
                SDL_UpdateYUVTexture(sdlTexture, &sdlRect,
                                    pFrameYUV->data[0], pFrameYUV->linesize[0],
                                    pFrameYUV->data[1], pFrameYUV->linesize[1],
                                    pFrameYUV->data[2], pFrameYUV->linesize[2]);
#endif
                SDL_RenderClear(sdlRenderer);
                SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
                SDL_RenderPresent(sdlRenderer);

                //--------- SDL End ---------

                //delay 40ms
                SDL_Delay(40);
            }
        }
        //av_free_packet(packet);
        av_packet_unref(packet);
    }

    while (1)
    {
        //ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        ret = avcodec_send_packet(pCodecCtx, packet);
        if (ret != 0)
            break;
        //if (!got_picture)
        if (avcodec_receive_frame(pCodecCtx, pFrame) != 0)
            break;
        sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);

#if OUTPUT_YUV420P
        int y_size=pCodecCtx->width*pCodecCtx->height;
        fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
        fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
        fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
#endif

        //--------- SDL Start ---------
        SDL_UpdateTexture(sdlTexture, &sdlRect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
        SDL_RenderPresent(sdlRenderer);
        //--------- SDL End ---------
        SDL_Delay(40);
    }

    sws_freeContext(img_convert_ctx);

#if OUTPUT_YUV420P
    fclse(fp_yuv);
#endif

    SDL_Quit();

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    getchar();

    return 0;
}
