#ifndef SHOWBMP_H
#define SHOWBMP_H

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class ShowBmp
{
public:
    ShowBmp();
    void getFfmpegVersion();
    void getSDLVersion();
    bool testSDL(const char *path);
};

#endif // SHOWBMP_H
