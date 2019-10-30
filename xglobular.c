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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 200

/*
 * Constants
 */
char WINDOW_NAME[] = __FILE__;
char ICON_NAME[] = __FILE__;

/*
 * Globals
 */
Display *display;
int screen;
Window main_window;
GC gc;
unsigned long foreground, background, otherfg;
XColor RGB_color, hardware_color;    /* added for color. */
Colormap color_map;      /* added for color. */

/*
 * Connect to the server and get the display device
 * and the screen number.
 */
void initX()
{
    int depth;                  /* depth of the color map. */

    /*
     * Set the display name from the environment variable DISPLAY.
     */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Unable to open display %s\n", XDisplayName(NULL));
        exit(1);
    }
    screen = DefaultScreen(display);

    /* Find the depth of the colour map. */
    depth = DefaultDepth(display, screen);

    /* Set the default foreground and background, in case we cannot use colour. */
    foreground = BlackPixel(display, screen);
    background = WhitePixel(display, screen);
    otherfg = BlackPixel(display, screen);

    if (depth > 1)              /* not monochrome */
    {
        color_map = DefaultColormap(display, screen);
        if (XLookupColor(display, color_map, "black", &RGB_color, &hardware_color) != 0
            && XAllocColor(display, color_map, &hardware_color) != 0)
            background = hardware_color.pixel;

        if (XLookupColor(display, color_map, "DarkSlateBlue", &RGB_color, &hardware_color) != 0
            && XAllocColor(display, color_map, &hardware_color) != 0)
            foreground = hardware_color.pixel;

        if (XLookupColor(display, color_map, "MediumSlateBlue", &RGB_color, &hardware_color) != 0
            && XAllocColor(display, color_map, &hardware_color) != 0)
            otherfg = hardware_color.pixel;

    }
}

/*
 * Opens a window on the display device, and returns
 * the windows ID.
 */
Window openWindow(x, y, width, height, border_width, argc, argv)
int x, y,                       /* co-ords of the upper left corner in pixels. */
width, height,                  /* size of the windows in pixels. */
    border_width;               /* the border width is not included in the other dimensions. */
int argc;
char **argv;
{
    Window new_window;
    XSizeHints size_hints;

    /* now create the window. */
    new_window = XCreateSimpleWindow(display,
                                     DefaultRootWindow(display),
                                     x, y, width, height, border_width, foreground, background);

    /* set up the size hints for the window manager. */
    size_hints.x = x;
    size_hints.y = y;
    size_hints.width = width;
    size_hints.height = height;

    size_hints.flags = PPosition | PSize;

    /* and state what hints are included. */
    XSetStandardProperties(display, new_window, WINDOW_NAME, ICON_NAME, None,   /* no icon map */
                           argv, argc, &size_hints);

    /* Return the window ID. */
    return (new_window);
}

/*
 * Create a graphics context using default values and,
 * return it in the pointer gc.
 */
GC getGC()
{
    GC gc;
    XGCValues gcValues;

    gc = XCreateGC(display, main_window, (unsigned long)0, &gcValues);

    XSetBackground(display, gc, background);
    XSetForeground(display, gc, foreground);

    return (gc);
}

/*
 * Terminate the program gracefully.
 */
void quitX()
{
    XCloseDisplay(display);
    exit(0);
}

int32_t getrand(int fd)
{
    int32_t retval;

    if (fd == -1)
    {
        retval = (((WINDOW_WIDTH)<(WINDOW_HEIGHT))?(WINDOW_WIDTH):(WINDOW_HEIGHT)) / 2;
    }
    else
    {
        read(fd, &retval, sizeof(int32_t));
        retval &= RAND_MAX;
    }

    return(retval);
}

#define RAND ((double)getrand(fd) / (double)RAND_MAX)

double R0, R2, R3;
double PI;
double C0;
double R1;
double XM, YM;
double X2, Y2, S;
double C;
int K;
double R, C1, D;
double X, Y, Z;
double S1;
double A;

#if defined(DEG)
#define ATN(val) (atan(val) * 90.0 / (PI / 2.0))
#else
#define ATN(val) (atan(val))
#endif

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
    XDrawPoint(display, main_window, gc, (int)X, (int)Y);
    XFlush(display);
}

int display_something(int iter)
{
    int i, w;
    int fd;
    XEvent event;

    /* flush event queue */
    XSelectInput(display, main_window, ExposureMask);
    XNextEvent(display, &event);

    fd = open("/dev/urandom", O_RDONLY);
    /* REM   MAKE A GLOBULAR
     */
    R0 = 20.0; R2 = R0 * R0; R3 = R2 * R0;
    PI = 3.14159265;
    C0 = PI * PI * R3 / 4.0;
    R1 = R0 / sqrt(2.0);
    XM = (double)WINDOW_WIDTH; YM = (double)WINDOW_HEIGHT;
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
    XSetForeground(display, gc, otherfg);
    for (w = 0; w <= WINDOW_HEIGHT; w += WINDOW_HEIGHT/25)
    {
      XDrawLine (display, main_window, gc,
                 w*WINDOW_WIDTH/WINDOW_HEIGHT, WINDOW_HEIGHT, 0, w);
      XDrawLine (display, main_window, gc,
                 w*WINDOW_WIDTH/WINDOW_HEIGHT, 0, WINDOW_WIDTH, w);
    }

    return (0);
}

int main(argc, argv)
int argc;
char **argv;
{
    int done;
    int iter;
    XEvent event;

    initX();
    main_window = openWindow(10, 20, WINDOW_WIDTH, WINDOW_HEIGHT, 5, argc, argv);
    gc = getGC();

    /* Display the window on the screen. */
    XMapWindow(display, main_window);

    if (argc > 1)
    {
        iter = atoi(argv[1]);
    }
    else
    {
        iter = 25000;
    }
    display_something(iter);

    XSelectInput(display, main_window, KeyPressMask | ButtonPressMask);

    done = 0;
    while (!done)
    {
        XNextEvent(display, &event);
        if ((event.type == KeyPress) || (event.type == ButtonPress))
            done = 1;
    }

    /*  sleep( 10 ); */

    quitX();

    return (0);
}
