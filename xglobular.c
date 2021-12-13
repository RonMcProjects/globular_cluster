/* 
 * gcc -Wall xglobular.c -lm -lX11 -o xglobular
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Constants
 */
char WINDOW_NAME[] = __FILE__;
char ICON_NAME[] = __FILE__;
#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

/*
 * Globals
 */
Display *display = NULL;
Window xwindow;
int screen;

uint16_t getrand(int fd)
{
    uint16_t retval;

    if (fd == -1)
    {
        retval = (((WINDOW_WIDTH)<(WINDOW_HEIGHT))?(WINDOW_WIDTH):(WINDOW_HEIGHT)) / 2;
    }
    else
    {
        read(fd, &retval, sizeof(uint16_t));
    }

    return(retval);
}

#define RAND ((float)getrand(fd) / (float)0xffff)

float R0, R2, R3;
float PI;
float C0;
float R1;
float XM, YM;
float X2, Y2, S;
float C;
int K;
float R, C1, D;
float X, Y, Z;
float S1;
float A;

#define ATN(val) (atanf(val))

void GOSUB_100()
{
    /* REM  NEWTON-RAPHSON ITERATION */
    A = R / R0;
    C1 = ATN(A) * 0.5 * R3;
    A = 1.0 + A * A;
    C1 = C1 + R * 0.5 * R2 / A;
    C1 = PI * (C1 - R * R2 / (A * A));
    D = 4.0 * PI * R * R / (A * A * A);
}

void GOSUB_200()
{
    /* REM  2-DIMENSIONAL PLOT */
    X = X * S + X2; Y = Y * S + Y2;
    if (X < 0.0 || Y < 0.0)
        return;
    if (X >= XM || Y >= YM)
        return;
    XDrawPoint(display, xwindow, DefaultGC(display, screen), (int)X, (int)Y);
    XFlush(display);
}

int generate_cluster(int iter)
{
    int i;
    int fd;

    fd = open("/dev/urandom", O_RDONLY);
    /* REM   MAKE A GLOBULAR
     */
    R0 = 20.0; R2 = R0 * R0; R3 = R2 * R0;
    PI = 3.14159265;
    C0 = PI * PI * R3 / 4.0;
    R1 = R0 / sqrt(2.0);
    XM = (float)WINDOW_WIDTH; YM = (float)WINDOW_HEIGHT;
    X2 = XM / 2.0; Y2 = YM / 2.0; S = 5.0;
    for (i = 1; i <= iter; i++)
    {
        C = C0 * RAND; R = R1;
        /*
         * NOW FIND R */
        for (K = 1; K <= 5; K++)
        {
            GOSUB_100();
            R = R + (C - C1) / D;
        }
        /* REM  3-DIMENTIONAL PLACE */
        do {
            X = (RAND - 0.5) * 2.0; /* Fixed bug from the original. */
            Y = (RAND - 0.5) * 2.0; /* Multiply by two for the unitary */
            Z = (RAND - 0.5) * 2.0; /* sphere radius. */
            S1 = sqrt(X * X + Y * Y + Z * Z);
        } while (S1 > 1.0);
        /* REM  POINT IS NOW IN SPHERE */
        R = R * S1; X = X * R; Y = Y * R; Z = Z * R;
        GOSUB_200();
    }
    close(fd);

    return (0);
}

void setup_xwindow(int argc, char **argv)
{
    XSizeHints size_hints;

    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    xwindow = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WINDOW_WIDTH, WINDOW_HEIGHT, 1,
                                   WhitePixel(display, screen), BlackPixel(display, screen));
    XSetBackground(display, DefaultGC(display, screen), BlackPixel(display, screen));
    XSetForeground(display, DefaultGC(display, screen), WhitePixel(display, screen));
    /* set up the size hints for the window manager. */
    size_hints.x = 10;
    size_hints.y = 10;
    size_hints.width = WINDOW_WIDTH;
    size_hints.height = WINDOW_HEIGHT;
    size_hints.flags = PPosition | PSize;
    /* and state what hints are included. */
    XSetStandardProperties(display, xwindow, WINDOW_NAME, ICON_NAME, None,   /* no icon map */
                           argv, argc, &size_hints);
    XSelectInput(display, xwindow, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display, xwindow);
}

int main(argc, argv)
int argc;
char **argv;
{
    XEvent event;
    int iter;

    if (argc > 1)
    {
        iter = atoi(argv[1]);
    }
    else
    {
        iter = 25000;
    }

    setup_xwindow(argc, argv);

    while(1)
    {
        XNextEvent(display, &event);
        if (event.type == Expose)
        {
            generate_cluster(iter);
        }
        if ((event.type == KeyPress) || (event.type == ButtonPress))
            break;
    }

    XCloseDisplay(display);
    return(0);
}
