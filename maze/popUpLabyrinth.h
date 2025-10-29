#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cairo.h>
#include "labyrinth.c"
#include <gtk/gtk.h>
#include <string.h>
#include <time.h>

//gcc -o maiz gui.c -Wall -pthread `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

int amountOfPaths = 0;
int screensizeX = 1050, screenStartX = 250, screenStartY = 20;//The size of the screen showing the matrix
int screensizeY = 900;
int xposition = 0; 
int yposition = 0; //These two are used to move the maze
long scale = 10;
int clickPos[2];
bool firstClick = true;
bool movedScale = false;
cairo_surface_t *walls;
cairo_surface_t *floors;
bool generated = false;
cairo_pattern_t *wallTexture;
cairo_pattern_t *floorTexture;
GtkWidget *window;

static void do_drawing(cairo_t *);
static void drawPaths(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{      
  do_drawing(cr);
  drawPaths(cr);
  return FALSE;
}

bool IsInitialized(){
    return generated;
}

void drawLines(int x, int y, cairo_t *cr){
    char c = raster[x][y];
    double px = (scale*(x-xposition))+screenStartX;
    double py = (scale*(y-yposition))+screenStartY;
    char rigth = 0, down = 0;
        if(x+2 < rows*2+1 && raster[x+1][y] != 'X')
            rigth = raster[x+2][y];
        if(y+2 < columns*2+1 && raster[x][y+1] != 'X')
            down = raster [x][y+2];
        int cantidad = 1;
        int linewidth;
        int carril;
        for(int power = 1; power <64; power *=2)
            {
                switch(power)
                    {
                        case 1:  cairo_set_source_rgb(cr,0.02745098039,0.23137254901960785,0.298039215686); break;
                        case 2:  cairo_set_source_rgb(cr,0.066666666666,0.54117647058,0.6980392156862745); break;
                        case 4:  cairo_set_source_rgb(cr,0.5568627450980392,0.2549019607843137,0.1803921568627451); break;
                        case 8:  cairo_set_source_rgb(cr,0.9921568627450981,0.349019607843,0.00392156862745098); break;
                        case 16:  cairo_set_source_rgb(cr,0.9372549019607843,0.27843137254,0.43529411764705883); break;
                        case 32:  cairo_set_source_rgb(cr,0.223529411764,0.721568627450,0.60392156862); break;
                    }
                    linewidth = (scale/6);
                    if(linewidth <= 0) linewidth = 1;
                    if(power == (power & c))
                    {
                        cairo_set_line_width(cr, linewidth);
                        carril = linewidth*cantidad-linewidth/2;
                        if(power == (power&down))
                            {
                                cairo_move_to(cr, px+carril, py+carril-linewidth/2);
                                px = (scale*(x-xposition))+screenStartX+carril;
                                py = (scale*(y+2-yposition))+screenStartY+carril+linewidth/2;
                                cairo_line_to(cr, px, py);
                                px = (scale*(x-xposition))+screenStartX;
                                py = (scale*(y-yposition))+screenStartY;
                            }

                        if(power == (power&rigth))
                            {
                                cairo_move_to(cr, px+carril-linewidth/2, py+carril);
                                px = (scale*(2+x-xposition))+screenStartX+carril+linewidth/2;
                                py = (scale*(y-yposition))+screenStartY+carril;
                                cairo_line_to(cr, px, py);
                                px = (scale*(x-xposition))+screenStartX;
                                py = (scale*(y-yposition))+screenStartY;
                            }
                            cairo_stroke(cr);   
                        }
                        cantidad++;
            }
}

static void do_drawing(cairo_t *cr)
{
    char textBuffer[50];
    double px = 0;
    double py = 0;    
    cairo_set_source_rgb(cr, 0.17254901960784313,0.6627450980392157,0.16470588235294117);
    cairo_rectangle(cr, screenStartX-10, screenStartY-10, screensizeX+5, screensizeY+5);
    cairo_fill(cr);

    if(scale < 1)
    {   cairo_set_source_rgb(cr, 0.105882352941,0.43137254901960786,0.10588235294117647);
        int smallscale = 0;
        if (scale < 0)
            smallscale = scale*-1;
        else
            smallscale = scale; 
        smallscale +=2;

        px = (rows/smallscale);
        py = (columns/smallscale);
        if (px > screensizeX)
            px = screensizeX;
        if (py > screensizeY)
            py = screensizeY;
        cairo_rectangle(cr, screenStartX-10, screenStartY-10, screensizeX-screenStartX, screensizeY+5-screenStartY);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
    }
    else
    {
        //draw floor;
        if(scale > 2)
        {
            cairo_set_source(cr, floorTexture);
            cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
            cairo_rectangle(cr, screenStartX-10, screenStartY-10, screensizeX-screenStartX, screensizeY+5-screenStartY);
            cairo_fill(cr);

            cairo_set_source(cr, wallTexture);
            cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
        }
        else{
            cairo_set_source_rgb(cr, 0.105882352941,0.43137254901960786,0.10588235294117647);
        }

        for (int x=xposition; x<rows*2+1; x++){
            for (int y=yposition; y<columns*2+1; y++){
                px = (scale*(x-xposition))+screenStartX;
                py = (scale*(y-yposition))+screenStartY;
                char c = raster[x][y];
                if(px>screensizeX || px < screenStartX || py > screensizeY)
                    break;

                if(c=='X' && py >= screenStartY-scale)
                {
                    int longness = 1;
                    while(true){
                        c = raster[x][y+1];
                        if(py+(scale*longness) > screensizeY||c!='X')
                            break;
                        y++;
                        longness++;
                    }
                    cairo_rectangle(cr, px, py, scale, scale*longness);
                    //cairo_stroke_preserve(cr);
                }
            }
            cairo_fill(cr);
        }
    }
    cairo_set_source_rgb(cr, 0,0,0);
    cairo_move_to(cr,10,20);
    sprintf(textBuffer,"Width: %d", rows);
    cairo_show_text(cr,textBuffer);
    
    cairo_move_to(cr,10,30);
    sprintf(textBuffer,"Heigth: %d", columns);
    cairo_show_text(cr,textBuffer);

    cairo_move_to(cr,10,60);
    sprintf(textBuffer,"start: (%d,%d)", entranceCellI, entranceCellJ);
    cairo_show_text(cr,textBuffer);

    cairo_move_to(cr,10,70);
    sprintf(textBuffer,"end: (%d,%d)", exitCellI, exitCellJ);
    cairo_show_text(cr,textBuffer);

    cairo_move_to(cr,10,200);
    sprintf(textBuffer, "X: %d", xposition/2);
    cairo_show_text(cr,textBuffer);
    cairo_move_to(cr,60,200);
    sprintf(textBuffer, "Y: %d", yposition/2);
    cairo_show_text(cr,textBuffer);
    cairo_move_to(cr,10,220);
    sprintf(textBuffer,"zoom level: %ld", scale); 
    cairo_show_text(cr, textBuffer);
    cairo_stroke(cr);
}

static void drawPaths(cairo_t *cr)
{
    char textBuffer[50];
    double px = 0;
    double py = 0;
    if(scale > 0)
    {
        for (int x=xposition; x<rows*2+1; x++){
            for (int y=yposition; y<columns*2+1; y++){
                px = (scale*(x-xposition))+screenStartX;
                py = (scale*(y-yposition))+screenStartY;
                char c = raster[x][y];
                if(px>screensizeX || py > screensizeY)
                    break;
                if(c != 'X'&& c!=0)
                {                    
                    drawLines(x,y,cr);
                }
            }
        }

    }
    px = (scale*((entranceCellI*2+1)-xposition))+screenStartX;
    py = (scale*((entranceCellJ*2+1)-yposition))+screenStartY;
    if(px< screensizeX && px > screenStartX && py < screensizeY && py >= screenStartY-scale)
    {
        cairo_set_source_rgb(cr, 0,1,0);
        cairo_rectangle(cr, px, py, scale, scale);
        cairo_fill(cr);
    }
    px = (scale*((exitCellI*2+1)-xposition))+screenStartX;
    py = (scale*((exitCellJ*2+1)-yposition))+screenStartY;
    if(px< screensizeX && px > screenStartX && py < screensizeY && py >= screenStartY-scale)
    {
        cairo_set_source_rgb(cr, 1,0,0);
        cairo_rectangle(cr, px, py, scale, scale);
        cairo_fill(cr);
    }

    cairo_set_source_rgb(cr, 0,0,0);
    cairo_move_to(cr,10,400);
    sprintf(textBuffer, "Amount of Algorithms now: %d", amountOfPaths);
    cairo_show_text(cr,textBuffer);

    cairo_move_to(cr,10,420);
    cairo_show_text(cr, "Total of spaces visited by each algorithm:");
    cairo_move_to(cr,10,433);

    
    cairo_set_source_rgb(cr,0.066666666666,0.54117647058,0.6980392156862745);
    sprintf(textBuffer, "1-Random Mouse: %d",randomMouseAmount);
    cairo_show_text(cr,textBuffer);

    cairo_move_to(cr,10,445);
    cairo_set_source_rgb(cr,0.02745098039,0.23137254901960785,0.298039215686);
    sprintf(textBuffer, "2-Left Mouse: %d", leftHandAmount);
    cairo_show_text(cr, textBuffer);

    cairo_move_to(cr,10,455);
    cairo_set_source_rgb(cr,0.5568627450980392,0.2549019607843137,0.1803921568627451);
    sprintf(textBuffer, "3-Pledge Algorithm: %d", pledgeAmount);
    cairo_show_text(cr, textBuffer);
    
    cairo_move_to(cr,10,465);
    cairo_set_source_rgb(cr,0.9921568627450981,0.349019607843,0.00392156862745098);
    sprintf(textBuffer, "4-Tremaux Algorithm: %d", pledgeAmount);
    cairo_show_text(cr, textBuffer);

    cairo_move_to(cr,10,475);
    cairo_set_source_rgb(cr,0.9372549019607843,0.27843137254,0.43529411764705883);
    sprintf(textBuffer, "5-Fattah Algorithm: %d", fatahAmount);
    cairo_show_text(cr, textBuffer);

    cairo_move_to(cr,10,485);
    cairo_set_source_rgb(cr,0.223529411764,0.721568627450,0.60392156862); 
    sprintf(textBuffer, "6-Rigth Hand: %d", rigthHandAmount);
    cairo_show_text(cr, textBuffer);

    cairo_set_source_rgb(cr, 0,0,0);
    cairo_move_to(cr, screensizeX, screensizeY);
    cairo_set_line_width(cr, 15);
    cairo_rectangle(cr, screenStartX-15, screenStartY-15, screensizeX-screenStartX, screensizeY);
    cairo_stroke_preserve(cr);
}

void *refresher()
{
    while(amountOfPaths > 0)
    {  
        usleep(500000);
        gtk_widget_queue_draw(window);
    }
    return NULL;
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    if(event->button ==2) //Hacer click a la rueda del mouse resetea la posicion y el zoom del laberinto.
    {
        xposition = yposition = 0;
        scale = 10;
        gtk_widget_queue_draw(widget);
        return true;
    }
    if(scale >= 1){
    movedScale = true;
    int x = event->x;
    int y = event->y;
    bool validClick = (x>screenStartX && y>screenStartY && y<screensizeX && x<screensizeY);

    if(firstClick && validClick){
        firstClick = false;
        clickPos[0]=x;
        clickPos[1]=y;
        return true;
    }
    
    if(!firstClick && validClick){//User moved the labyrinth
        xposition += (clickPos[0]-x)/scale;
        yposition += (clickPos[1]-y)/scale;
        if(xposition < 0) xposition = 0;
        if(yposition < 0) yposition = 0;
        firstClick = true;
    }
    
    gtk_widget_queue_draw(widget);
    }
    return true;
}

static gboolean mouse_scroll (GtkWidget *widget, GdkEventScroll  *event, gpointer user_data)
{
    if(event->direction ==1){//Down, the zoom decreases
        movedScale = false;
        if(scale < 15 && scale-1>0)
            scale -= 1;
        else{
            scale -=3;
        }
        gtk_widget_queue_draw(widget);
        return true;
    }
    if(event->direction==0){//Up, the zoom increases
        if(scale < 15)
            scale += 1;
        else{
            scale +=3;
        }
        gtk_widget_queue_draw(widget);
    }
    return true;
}

void popUpMaze(int x, int y){
  walls = cairo_image_surface_create_from_png("images/bush.png");
  floors = cairo_image_surface_create_from_png("images/grass.png");
  wallTexture = cairo_pattern_create_for_surface(walls);
  floorTexture = cairo_pattern_create_for_surface(floors);
  generated = true;
  redefine(x,y);
  GtkWidget *darea;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), darea);
  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(window, GDK_SCROLL_MASK);
  
  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL); 
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window, "button-press-event", 
      G_CALLBACK(clicked), NULL);

  g_signal_connect(window, "scroll-event", 
      G_CALLBACK(mouse_scroll), NULL);

  gtk_window_set_default_size(GTK_WINDOW(window), screensizeY+140, screensizeY+10); 
  gtk_window_set_title(GTK_WINDOW(window), "Maze window");

  
  gtk_widget_show_all(window);

  gtk_main();
}

void popUpMazeWithoutInit(){
  generated = true;
  walls = cairo_image_surface_create_from_png("images/bush.png");
  floors = cairo_image_surface_create_from_png("images/grass.png");
  wallTexture = cairo_pattern_create_for_surface(walls);
  floorTexture = cairo_pattern_create_for_surface(floors);
  RastaGotSoul();
  GtkWidget *darea;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), darea);
  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(window, GDK_SCROLL_MASK);
  
  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL); 
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window, "button-press-event", 
      G_CALLBACK(clicked), NULL);

  g_signal_connect(window, "scroll-event", 
      G_CALLBACK(mouse_scroll), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), screensizeY, screensizeY); 
  gtk_window_set_title(GTK_WINDOW(window), "Maze window");

  
  gtk_widget_show_all(window);

  gtk_main();
}

//Carrying function into
void LoadLoadFile(char fileName[]){
    readFile(fileName);
}

void cleanUpBuddy(){
    cleanUp();
}

void SaveSaveFile(const char fileName[], char folderName[])
{
    printf("%s",folderName);
    char buf[256];
    snprintf(buf, sizeof(buf), "%s%s%s%s", folderName, "/", fileName, ".txt");
    makeFile(buf);
}

void *randomMouseThread(){
    amountOfPaths ++;
    randomMouse();
    amountOfPaths --;
    return NULL;
}

void *rigthHandThread(){
    amountOfPaths ++;
    rigthHand();
    amountOfPaths --;
    return NULL;
}

void *leftHandThread(){
    amountOfPaths ++;
    leftHand();
    amountOfPaths --;
    return NULL;
}

void *pledgeThread(){
    amountOfPaths ++;
    pledge();
    amountOfPaths --;
    return NULL;
}

void *tremauxThread(){
    amountOfPaths ++;
    tremaux();
    amountOfPaths --;
    return NULL;
}

void *fatalThread(){
    amountOfPaths ++;
    fatah();
    amountOfPaths --;
    return NULL;
}

void aTodoGas(bool rMouse, bool rHand, bool lHand, bool pldge, bool tremaux, bool fatah, int intensidad){   
    resetCounters();
    RastaGotSoul();
    setInterval(intensidad);
    amountOfPaths = 0;
    //cleanThePaths();
    if(rMouse)
    {
        pthread_t tid1;
        pthread_create(&tid1, NULL, randomMouseThread, NULL);
    }

    if(rHand)
    {
        pthread_t tid2;
        pthread_create(&tid2, NULL, rigthHandThread, NULL);
    }
    if(lHand)
    {
        pthread_t tid3;
        pthread_create(&tid3, NULL, leftHandThread, NULL);
    }
    if(pldge)
    {
        pthread_t tid4;
        pthread_create(&tid4, NULL, pledgeThread, NULL);
    }
    if(tremaux)
    {
        pthread_t tid5;
        pthread_create(&tid5, NULL, tremauxThread, NULL);
    }
    if(fatah)
    {
        pthread_t tid6;
        pthread_create(&tid6, NULL, fatalThread, NULL);
    }
    pthread_t tid7;
    pthread_create(&tid7, NULL, refresher, NULL);
}

void cleanThePaths()
{
    char c;
    for (int x=xposition; x<rows*2+1; x++)
            for (int y=yposition; y<columns*2+1; y++){
                c = raster[rows*2+1][columns*2+1];
                if(c!='X')
                    raster[rows*2+1][columns*2+1]=0;
            }
}