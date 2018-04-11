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
    #include "SDL.h"
    #include "SDL_version.h"
    #include "SDL_error.h"
}

using namespace std;

ShowBmp::ShowBmp()
{
    //nothing
}

void ShowBmp::getFfmpegVersion()
{
    //output the ffmpeg version
    av_register_all();

    unsigned version = avcodec_version();
    cout << "ffmpeg version is:" << version << endl;
    getchar();
}

void ShowBmp::getSDLVersion()
{
    SDL_version compiled;

    SDL_VERSION(&compiled);
    printf("We compiled against SDL version %d.%d.%d ...\n",
           compiled.major, compiled.minor, compiled.patch);
}

bool ShowBmp::testSDL(const char *path)
{
    //the window we`ll be rendering to
    SDL_Window *gWindow = NULL;

    //the surface contained by the window
    SDL_Surface *gScreenSurface = NULL;

    //the image we will load and show on the screen
    SDL_Surface *gHello = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL video init error!\n");
        return false;
    }

    //create window
    gWindow = SDL_CreateWindow("SHOW IMAGE",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH,
                               SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    if(gWindow == NULL)
    {
        printf("SDL create window error: %s!\n", SDL_GetError());
    }

    //use this function to get the SDL surface associated with the window.
    gScreenSurface=SDL_GetWindowSurface(gWindow);

    //load image
    gHello = SDL_LoadBMP(path);
    if(gHello == NULL)
    {
        printf("Unable to load image.. %s!", SDL_GetError());
        return false;
    }

    //use this function to perform a fast surface copy to a destination surface.
    //下面函数的参数分别为： SDL_Surface* src ,const SDL_Rect* srcrect , SDL_Surface* dst ,  SDL_Rect* dstrect
    SDL_BlitSurface(gHello, NULL, gScreenSurface, NULL);
    //更新显示copy the window surface to the screen
    SDL_UpdateWindowSurface(gWindow);
    getchar();

    //释放内存并销毁窗口
    SDL_FreeSurface(gHello);
    SDL_FreeSurface(gScreenSurface);
    gHello = NULL;
    gScreenSurface = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();

    return 0;
}
