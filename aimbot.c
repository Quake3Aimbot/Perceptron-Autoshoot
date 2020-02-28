/*
--------------------------------------------------
    James William Fletcher (james@voxdsp.com)
--------------------------------------------------

    r_picmip 20 & force models to blue bones
        You don't have to set `r_picmip` to 16 or above
    but it does help.

    You will however need to set `cg_forceModel 1`
    and select a player model with uniform colour,
    and I do consider the bright blue bones model
    to be the best option. This is important for
    the colour aimbot detections which train the
    perceptrons.

    This is created to only run on Linux under X11.

    I created this initially because I just wanted
    something fun to do, and I love playing Quake3
    Instagib.
        When I started looking into CNN's
    such as AlexNet I decided I wanted to have a
    go at a simple Perceptron implementation, thus
    expanding on this project. I was supprised to
    see gradient descent do such a good job.

    Tested using the ioQuake3 client.
    https://ioquake3.org/

--------------------------------------------------
    Recommended Client Settings
--------------------------------------------------
    cg_oldRail "1"
    cg_noProjectileTrail "1"
    cg_forceModel "1"
    cg_railTrailTime "100"
    cg_crosshairSize "24"
    cg_drawFPS "1"
    cg_draw2D "1"
    cg_gibs "1"
    cg_fov "160"
    cg_zoomfov "90"
    cg_drawGun "1"
    cg_brassTime "0"
    cg_drawCrosshair "7"
    cg_drawCrosshairNames "1"
    cg_marks "0"
    cg_crosshairPulse "1"
    cg_stepTime "100"
    cg_centertime "3"
    xp_noParticles "1"
    xp_noShotgunTrail "1"
    xp_noMip "2047"
    xp_ambient "1"
    xp_modelJump "0"
    xp_corpse "3"
    xp_crosshairColor "7"
    xp_improvePrediction "1"
    cm_playerCurveClip "1"
    com_maxfps "600"
    com_blood "0"
    cg_autoswitch "0"
    rate "25000"
    snaps "40"
    model "bones/default"
    headmodel "bones/default"
    team_model "bones/default"
    team_headmodel "bones/default"
    color1 "6"
    color2 "5"
    cg_predictItems "1"
    r_picmip "16"
    r_overBrightBits "1"
    r_mode "-2"
    r_fullscreen "1"
    r_customwidth "1920"
    r_customheight "1080"
    r_simpleMipMaps "1"
    r_railWidth "16"
    r_railCoreWidth "6"
    r_railSegmentLength "32"
    cg_shadows "0"
    com_zoneMegs "24"
    com_hunkmegs "512"
--------------------------------------------------
    ^ Thats how I like to configure my Quake 3 client.
--------------------------------------------------

--------------------------------------------------
    ~~ MLP - Multi-Layer Perceptron learning ~~
--------------------------------------------------
    - There is traditional Colour aim which is used to train
      the perceptron based models.
    - The "Deep Aim" option actually uses a MLP rather than
      a DNN.
    - The Neural Aim option only uses one perceptron per
      pixel.
    - To quickly train the MLP / Perceptrons just load a
      a local server full of bots on extreme difficulty,
      then go into first person spectator mode and leave
      the colour autoshoot turned on. It will now flick
      between bot views every time it detects a target.

    Quake 3 always has been a game dominated by cheaters
    augmenting their skills with bots, console commands,
    wider fov, better gaming hardware, anything to gain
    a competative advantage.

    I can confidently say that most people playing on the
    traditional servers are cheating to some extent, most
    popularly is the autoshoot bot to the point now that
    it should just be a built-in function of the game.

    I have seen the owner of Elitez (https://elitez.eu)
    servers using an autoshoot bot once himself while
    configuring the server's anti-cheat system which will
    boot you if you lower or remove the 100ms wait between
    mouse triggers.

    There is a simpler version of this bot which I uploaded to:
    https://www.youtube.com/channel/UCYe31SWsmK3X9H3r1WM6OKQ
    This just features Neural Aim.

--------------------------------------------------
    ~~ Notes ~~
--------------------------------------------------
    - This bot only trains in Colour Aim or Neural Aim modes.
    - This bot stops training itself in Deep Aim mode.

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

//https://www.cl.cam.ac.uk/~mgk25/ucs/keysymdef.h
int key_is_pressed(KeySym ks)
{
    Display *dpy = XOpenDisplay(":0");
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    int isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    XCloseDisplay(dpy);
    return isPressed;
}

//Perceptron
const unsigned int _nquality = 0; // 0 - low, 1 - high
const float _pbias = 1;			  // Neuron Trigger Bias
const float _lrate = 1;			  // Learning Rate
float pw[16][4] = {0}; //Upto 16 weights, 3 inputs per weight and one bias

void softmax_transform(double* w)
{
    const size_t max_index = sizeof(w) / sizeof(double);

    double d = 0;
    for(size_t i = 0; i < max_index; i++)
        d += exp(w[i]);

    for(size_t i = 0; i < max_index; i++)
        w[i] = exp(w[i]) / d;
}

double sigmoid(double x)
{
    return 1 / (1 + exp((double) - x));
}

double fastSigmoid(double x)
{
    return x / (1 + abs(x));
}

double gPerceptron(double r, double g, double b, float* w)
{
    double ro = r*w[0]+g*w[1]+b*w[2]+_pbias*w[3]; //Output
    if(_nquality == 1){sigmoid(ro);} //Sigmoid function
    if(ro < 0){ro = 0;} //ReLU
    if(ro >= 1){ro = 1;} //Saturate
    return ro;
}

double doPerceptron(double r, double g, double b, double eo, float* w)
{
    double ro = r*w[0]+g*w[1]+b*w[2]+_pbias*w[3]; //Output
    if(_nquality == 1){sigmoid(ro);} //Sigmoid function
    if(ro < 0){ro = 0;} //ReLU
    if(ro >= 1){ro = 1;} //Saturate
    const double error = eo - ro; //Error Gradient
    w[0] += error * r * _lrate;
    w[1] += error * g * _lrate;
    w[2] += error * b * _lrate;
    w[3] += error * _pbias * _lrate;
    return ro;
}

//Entry Point
int main()
{
    printf("James William Fletcher (james@voxdsp.com)\nSet r_picmip 9 or higher\n & force player models ON\n & set your player model to aqua blue bones\n & select the only aiming reticule/cursor that doesnt obstruct the center of the screen\n\nKey-Mapping's:\nF10 - Preset Max Tollerance\nUP - Preset Medium Tollerance\nDOWN - Preset Low Tollerence\nLEFT - Manual Lower Tollerance\nRIGHT - Manual Higher Tollerance\n\nH - Retrain/Target on current center screen colour\nG - Same as H but uses an average of 9 surrounding colours\n\nF1 - Target Aqua Blue\nF2 - Target Blue\nF3 - Target Red\n\nLeft CTRL - Enable/Disable Auto-Shoot\n\nB - Deep Aim Only\nN - Neural Aim Only (trains Neural Net)\nM - Colour Aim Only (trains Neural Net)\n\n");

    //Variables
    unsigned short lr=0, lg=0, lb=0;
    unsigned short tr=57753,tg=65535,tb=65535;
    unsigned short tol = 14000;

    XColor c[9];
    Display *d;
    int si;
    unsigned int x=0, y=0;

    XEvent event;
    memset(&event, 0x00, sizeof(event));
    
    unsigned int enable = 0;
    unsigned int mode = 2;
    
    while(1)
    {
        //Loop every 10 ms
        usleep(10000);

        //Inputs / Keypress
        if(key_is_pressed(XK_Control_L))
        {
            if(enable == 0)
            {
                enable = 1;
                printf("\a\n");
                usleep(300000);
                printf("\aAUTO-SHOOT: ON\n");
            }
            else
            {
                enable = 0;
                printf("\aAUTO-SHOOT: OFF\n");
                sleep(1);
            }
        }
        
        //Enable / Disable entire bot
        if(enable == 1)
        {
            //Open Display 0
            d = XOpenDisplay((char *) NULL);
            if(d == NULL)
                continue;

            //Get default screen
            si = XDefaultScreen(d);
            
            //Reset mouse event
            memset(&event, 0x00, sizeof(event));
            
            if(key_is_pressed(XK_Left))
            {
                tol -= 100;
                printf("\aTOL: %i\n", tol);
            }

            if(key_is_pressed(XK_Right))
            {
                tol += 100;
                printf("\aTOL: %i\n", tol);
            }

            if(key_is_pressed(XK_Down))
            {
                tol = 7333;
                printf("\aTOL: %i\n", tol);
                sleep(1);
            }

            if(key_is_pressed(XK_Up))
            {
                tol = 14000;
                printf("\aTOL: %i\n", tol);
                sleep(1);
            }

            if(key_is_pressed(XK_F10))
            {
                tol = 26000;
                printf("\aTOL: %i\n", tol);
                sleep(1);
            }

            if(key_is_pressed(XK_F1)) //Aqua Blue
            {
                tr=57753; //53827 - 61680
                tg=65535;
                tb=65535;
                printf("\a:: Aqua Blue\n");
                sleep(1);
            }

            if(key_is_pressed(XK_F2)) //Blue
            {
                tr=18247;
                tg=5397;
                tb=37265;
                printf("\a:: Blue\n");
                sleep(1);
            }

            if(key_is_pressed(XK_F3)) //Red
            {
                tr=37265;
                tg=4883;
                tb=4112;
                printf("\a:: Red\n");
                sleep(1);
            }
            
            if(key_is_pressed(XK_F4)) //Red 2
            {
                tr=35094;
                tg=9766;
                tb=3712;
                printf("\a:: Red 2\n");
                sleep(1);
            }

            if(key_is_pressed(XK_B))
            {
                mode = 0;
                printf("\aDeep Aim\n");
                sleep(1);
            }
            if(key_is_pressed(XK_N))
            {
                mode = 1;
                printf("\aNeural Aim\n");
                sleep(1);
            }
            if(key_is_pressed(XK_M))
            {
                mode = 2;
                printf("\aColour Aim\n");
                sleep(1);
            }
            

            //Ready to press down mouse 1
            event.type = ButtonPress;
            event.xbutton.button = Button1;
            event.xbutton.same_screen = True;
            
            //Find target window
            XQueryPointer(d, RootWindow(d, si), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
            event.xbutton.subwindow = event.xbutton.window;
            while(event.xbutton.subwindow)
            {
                event.xbutton.window = event.xbutton.subwindow;
                XQueryPointer(d, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
            }

            //Get Center Window
            XWindowAttributes attr;
            XGetWindowAttributes(d, event.xbutton.window, &attr);
            x = attr.width/2; 
            y = attr.height/2;

            //Get Image Block
            XImage *i = XGetImage(d, event.xbutton.window, x-1, y-1, 3, 3, AllPlanes, XYPixmap);
            if(i == NULL)
            {
                XCloseDisplay(d);
                continue;
            }

            //Get Pixels
            c[0].pixel = XGetPixel(i, 0, 0);
            c[1].pixel = XGetPixel(i, 0, 1);
            c[2].pixel = XGetPixel(i, 0, 2);
            c[3].pixel = XGetPixel(i, 1, 0);
            c[4].pixel = XGetPixel(i, 1, 1);
            c[5].pixel = XGetPixel(i, 1, 2);
            c[6].pixel = XGetPixel(i, 2, 0);
            c[7].pixel = XGetPixel(i, 2, 1);
            c[8].pixel = XGetPixel(i, 2, 2);
            XFree(i);
            
            //Get Colour Map
            const Colormap map = XDefaultColormap(d, si);
            //printf("M: %li %li\n", map, c.pixel);

            //For each pixel ...
            int phi = 0;
            int bhi = 0;
            int shots = 0;
            if(mode != 0)
            {
                for(int i = 0; i < 9; i++)
                {
                    XQueryColor(d, map, &c[i]);

                    const unsigned short r = c[i].red;
                    const unsigned short g = c[i].green;
                    const unsigned short b = c[i].blue;
                    
                    //Center Pixel Target
                    if(key_is_pressed(XK_H) && i == 0)
                    {
                        tr = r;
                        tg = g;
                        tb = b;
                        printf("\aH-SET: %i %i %i\n", tr, tg, tb);
                    }
                    
                    //Shoot if colour has locked on
                    int fire = 0;

                    if(mode == 2)
                        if(r > tr-tol && r < tr+tol && g > tg-tol && g < tg+tol && b > tb-tol && b < tb+tol)
                            fire = 1;
                    
                    if(mode == 1)
                    {
                        const double ghit = gPerceptron(r/65535, g/65535, b/65535, pw[i]);
                        if(ghit > 0)
                            fire = 1;
                    }

                    if(fire == 1)
                    {
                        //Fire mouse down
                        XSendEvent(d, PointerWindow, True, 0xfff, &event);
                        XFlush(d);
                        
                        //Wait 100ms (or ban for suspected cheating)
                        usleep(100000);
                        
                        //Release mouse down
                        event.type = ButtonRelease;
                        event.xbutton.state = 0x100;
                        XSendEvent(d, PointerWindow, True, 0xfff, &event);
                        XFlush(d);

                        //printf("Colour Hit");

                        //Train Perceptron
                        const double nr = r/65535, ng = g/65535, nb = b/65535;
                        const double hit = doPerceptron(nr, ng, nb, 1, pw[i]);
                        if(hit > 0)
                        {
                            //printf(", Perceptron Hit: %u, %lf", i, hit);
                            phi++;
                        }

                        //printf("\n");	

                        shots++;
                    }
                    else
                    {
                        const double hit = doPerceptron(r/65535, g/65535, b/65535, 0, pw[i]);
                        if(hit > 0)
                        {
                            //printf("Bad Hit: %u, %lf\n", i, hit);
                            bhi++;
                        }
                    }
                }
            }

            /*if(phi > 0)
                printf("Total Good: %u / 9\n", phi);
            if(bhi > 0)
                printf("Total bad: %u / 9\n", bhi);*/

            //Train DeepAim
            if(mode != 0)
            {
                if(shots > 0)
                {
                    double p[9]={0};

                    for(int i = 0; i < 9; i++)
                    {
                        XQueryColor(d, map, &c[i]);

                        const double r = c[i].red / 65535;
                        const double g = c[i].green / 65535;
                        const double b = c[i].blue / 65535;

                        p[i] = gPerceptron(r, g, b, pw[i]);
                    }

                    double h[3] = {0};
                    h[0] = doPerceptron(p[0], p[1], p[2], 1, pw[9]);
                    h[1] = doPerceptron(p[3], p[4], p[5], 1, pw[10]);
                    h[2] = doPerceptron(p[6], p[7], p[8], 1, pw[11]);
                    if(_nquality == 1){softmax_transform(h);}
                    const double deep_result = doPerceptron(h[0], h[1], h[2], 1, pw[12]);
                }
                else
                {
                    double p[9]={0};

                    for(int i = 0; i < 9; i++)
                    {
                        XQueryColor(d, map, &c[i]);

                        const double r = c[i].red / 65535;
                        const double g = c[i].green / 65535;
                        const double b = c[i].blue / 65535;

                        p[i] = gPerceptron(r, g, b, pw[i]);
                    }

                    double h[3] = {0};
                    h[0] = doPerceptron(p[0], p[1], p[2], 0, pw[9]);
                    h[1] = doPerceptron(p[3], p[4], p[5], 0, pw[10]);
                    h[2] = doPerceptron(p[6], p[7], p[8], 0, pw[11]);
                    if(_nquality == 1){softmax_transform(h);}
                    const double deep_result = doPerceptron(h[0], h[1], h[2], 0, pw[12]);
                }
            }

            //Deep Aim
            if(mode == 0)
            {
                double p[9]={0};

                for(int i = 0; i < 9; i++)
                {
                    XQueryColor(d, map, &c[i]);

                    const double r = c[i].red / 65535;
                    const double g = c[i].green / 65535;
                    const double b = c[i].blue / 65535;

                    p[i] = gPerceptron(r, g, b, pw[i]);
                }

                double h[3] = {0};
                h[0] = gPerceptron(p[0], p[1], p[2], pw[9]);
                h[1] = gPerceptron(p[3], p[4], p[5], pw[10]);
                h[2] = gPerceptron(p[6], p[7], p[8], pw[11]);
                if(_nquality == 1){softmax_transform(h);}
                const double deep_result = gPerceptron(h[0], h[1], h[2], pw[12]);

                if(deep_result > 0)
                {
                    //Fire mouse down
                    XSendEvent(d, PointerWindow, True, 0xfff, &event);
                    XFlush(d);
                    
                    //Wait 100ms (or ban for suspected cheating)
                    usleep(100000);
                    
                    //Release mouse down
                    event.type = ButtonRelease;
                    event.xbutton.state = 0x100;
                    XSendEvent(d, PointerWindow, True, 0xfff, &event);
                    XFlush(d);
                }
            }
            
            //Average Target
            if(key_is_pressed(XK_G))
            {
                tr = (c[0].red + c[1].red + c[2].red + c[3].red + c[4].red + c[5].red + c[6].red + c[7].red + c[8].red) / 9;
                tg = (c[0].green + c[1].green + c[2].green + c[3].green + c[4].green + c[5].green + c[6].green + c[7].green + c[8].green) / 9;
                tb = (c[0].blue + c[1].blue + c[2].blue + c[3].blue + c[4].blue + c[5].blue + c[6].blue + c[7].blue + c[8].blue) / 9;
                printf("\aG-SET: %i %i %i\n", tr, tg, tb);
            }

            //Close the display
            XCloseDisplay(d);
        }
    }

    //Done, never gets here in regular execution flow
    return 0;
}
