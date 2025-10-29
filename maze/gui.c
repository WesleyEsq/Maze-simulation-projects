#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include "popUpLabyrinth.h"

//gcc -o maiz gui.c -Wall -pthread `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

GtkBuilder *builder;//Transports stuff from the glade file to here
//GtkWidget *hexagonTogle;
GtkWidget *gmHeigth;
GtkWidget *gmWidth;
GtkWidget *filechooser;
GtkWidget *filename;
GtkWidget *folderchooser;
GtkWidget *menuPages;
GtkWidget *menuSave;
GtkWidget *menuGenerate;
GtkWidget *mainMenu;

GtkWidget *pathing;
GtkWidget *saveBtn;
GtkWidget *randomAL;
GtkWidget *rigthH;
GtkWidget *leftH;
GtkWidget *pledgeAL;

GtkWidget *randomMous;
GtkWidget *dtremaux;
GtkWidget *fatahAL;

GtkWidget *solveBtn;
GtkWidget *normalspeedBtn;
GtkWidget *maximumBtn;

int main(int argc, char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "maze.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    menuPages = GTK_WIDGET(gtk_builder_get_object(builder, "menuPages"));
    //hexagonTogle = GTK_WIDGET(gtk_builder_get_object(builder, "hexagonTogle"));
    gmHeigth = GTK_WIDGET(gtk_builder_get_object(builder, "gmHeigth"));
    gmWidth = GTK_WIDGET(gtk_builder_get_object(builder, "gmWidth"));
    filechooser = GTK_WIDGET(gtk_builder_get_object(builder, "filechooser"));
    folderchooser = GTK_WIDGET(gtk_builder_get_object(builder, "folderchooser"));
    filename = GTK_WIDGET(gtk_builder_get_object(builder, "filename"));
    menuSave = GTK_WIDGET(gtk_builder_get_object(builder, "menuSave"));
    menuGenerate = GTK_WIDGET(gtk_builder_get_object(builder, "menuGenerate"));
    mainMenu = GTK_WIDGET(gtk_builder_get_object(builder, "mainMenu"));
    pathing = GTK_WIDGET(gtk_builder_get_object(builder, "pathing"));
    randomMous = GTK_WIDGET(gtk_builder_get_object(builder, "randomAL"));
    dtremaux = GTK_WIDGET(gtk_builder_get_object(builder, "tremaux"));
    fatahAL  = GTK_WIDGET(gtk_builder_get_object(builder, "fatah"));


    saveBtn = GTK_WIDGET(gtk_builder_get_object(builder, "fSSavebtn"));
    randomAL = GTK_WIDGET(gtk_builder_get_object(builder, "randomAL"));
    rigthH = GTK_WIDGET(gtk_builder_get_object(builder, "rigthH"));
    leftH = GTK_WIDGET(gtk_builder_get_object(builder, "leftH"));
    pledgeAL = GTK_WIDGET(gtk_builder_get_object(builder, "pledge"));
    solveBtn = GTK_WIDGET(gtk_builder_get_object(builder, "solveBtn"));

    maximumBtn = GTK_WIDGET(gtk_builder_get_object(builder, "maximumBtn"));
    normalspeedBtn = GTK_WIDGET(gtk_builder_get_object(builder, "normalspeedBtn"));

    gtk_widget_set_sensitive(saveBtn, false);
    gtk_widget_set_sensitive(solveBtn, false);
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}


void exit_app()
{
	gtk_main_quit(); // Command to quit a GTK program
}

void on_close_clicked(){
    exit_app();
}

//-------------------------------------- Trivial useless menu buttons

//Changes the screen into the generate maze menu
void on_generateBtn_clicked(){
    gtk_stack_set_visible_child (GTK_STACK(menuPages), menuGenerate);
}
//sends you to the file menu
void on_loadBtn_clicked(){
    gtk_stack_set_visible_child (GTK_STACK(menuPages), menuSave);
}

//Pathing baby, now it's here :D
void on_solveBtn_clicked(){
    gtk_stack_set_visible_child (GTK_STACK(menuPages), pathing);
}

//sends you back to the main menu.
void on_notOk_clicked(){
    gtk_stack_set_visible_child (GTK_STACK(menuPages), mainMenu);
}
//------------------------------------------Ok, now this is where the fun begins...

//This function sets all the parameters for the maze and tells the backend to generate the maze like that. 
void on_okGmBtn_clicked(){
    int heigth = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gmHeigth));
    int width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gmWidth));
    //bool hexaFM = gtk_switch_get_active(GTK_SWITCH(hexagonTogle));
    if(IsInitialized())
        cleanUpBuddy();
    gtk_stack_set_visible_child (GTK_STACK(menuPages), mainMenu);
    gtk_widget_set_sensitive(saveBtn, true);
    gtk_widget_set_sensitive(solveBtn, true);
    //The user wants a cuadricular matrix.
    popUpMaze(width, heigth);
    
}

//This one grabs filechooser's file and tells the backend to load the maze with that file, and then it orders popupmaze to appear.
void on_fSokbtn_clicked(){
    char *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
    if(file==NULL)
        return;
    if(IsInitialized())
        cleanUpBuddy();
    LoadLoadFile(file);
    gtk_widget_set_sensitive(saveBtn, true);
    gtk_widget_set_sensitive(solveBtn, true);
    popUpMazeWithoutInit();
}

//This other one tells the program to save the maze in a specific direction.
void on_fSSavebtn_clicked(){
    const char *file = gtk_entry_get_text(GTK_ENTRY(filename));
    char *folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(folderchooser));
    if(gtk_entry_get_text_length(GTK_ENTRY(filename)) == 0 || !IsInitialized() || folder == NULL)
        return;
    SaveSaveFile(file, folder);
    //popUpMazeWithoutInit();
}

void maximaPotencia(){//(Inserte aqui el meme de plankton encima de la cangreburger)
   bool algoritmo1 = gtk_switch_get_active(GTK_SWITCH(randomAL)), 
        algoritmo2 = gtk_switch_get_active(GTK_SWITCH(rigthH)), 
        algoritmo3 = gtk_switch_get_active(GTK_SWITCH(leftH)), 
        algoritmo4 = gtk_switch_get_active(GTK_SWITCH(pledgeAL)),
        algoritmo5 = gtk_switch_get_active(GTK_SWITCH(dtremaux)),
        algoritmo6 = gtk_switch_get_active(GTK_SWITCH(fatahAL));
        aTodoGas(algoritmo1, algoritmo2, algoritmo3, algoritmo4, algoritmo5, algoritmo6, 0);
}

void decentePotencia(){
    bool algoritmo1 = gtk_switch_get_active(GTK_SWITCH(randomAL)), 
        algoritmo2 = gtk_switch_get_active(GTK_SWITCH(rigthH)), 
        algoritmo3 = gtk_switch_get_active(GTK_SWITCH(leftH)), 
        algoritmo4 = gtk_switch_get_active(GTK_SWITCH(pledgeAL)),
        algoritmo5 = gtk_switch_get_active(GTK_SWITCH(dtremaux)),
        algoritmo6 = gtk_switch_get_active(GTK_SWITCH(fatahAL));
        aTodoGas(algoritmo1, algoritmo2, algoritmo3, algoritmo4, algoritmo5, algoritmo6,500000);
}