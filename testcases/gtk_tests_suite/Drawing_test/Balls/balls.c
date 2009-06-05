/*================================================================================================*/
/**
    @file   balls.c
*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
  Filinova Natalia           13/09/2004     ??????      Initial Version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>

gint vt=FALSE;
static struct ballStruct {
    double x;
    double y;
    double xVelocity;
    double yVelocity;
    gint colRed;
    gint colGreen;
    gint colBlue;
} ball[20];

gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
                
gint nextFrame(gpointer data);
void newBall(struct ballStruct *b);
void nextBall(struct ballStruct *b);
void setColor();

#define WIDTH  240
#define HEIGHT 320
#define GRAVITY 0.8
#define RADIUS 5
#define DIAMETER (RADIUS * 2)
#define INTERVAL 20
#define RAND_MAX 2147483647

static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",            NULL,         0,                     0, "<Branch>" },
  { "/File/_Quit-Pass", "<control>Q",  eventDestroyQuit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", eventDestroyExit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Help",            NULL,         0,                     0, "<Branch>" },
};

int balls_main(int argc,char *argv[])
{
    int i;
    GtkWidget *app;
    GtkWidget *area;
    GtkWidget *table;
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;

    gtk_init(&argc,&argv);
    app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (app), "Animation");
    gtk_window_set_default_size (GTK_WINDOW (app),240,320);

    gtk_signal_connect(GTK_OBJECT(app),"delete_event",
                       GTK_SIGNAL_FUNC(eventDelete),NULL);
    gtk_signal_connect(GTK_OBJECT(app),"destroy",
                       GTK_SIGNAL_FUNC(eventDestroyQuit),NULL);
    
    table = gtk_table_new (2, 1, FALSE);
    gtk_widget_show(table);
    
      /* Create the menubar*/
    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (app), accel_group);
    g_object_unref (accel_group);
    item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

      /* Set up item factory to go away with the window */
    g_object_ref (item_factory);
    gtk_object_sink (GTK_OBJECT (item_factory));
    g_object_set_data_full (G_OBJECT (app),
                            "<main>",
                            item_factory,
                         (GDestroyNotify) g_object_unref);

      /* create menu items */
    gtk_item_factory_create_items (item_factory, G_N_ELEMENTS (menu_items),
                                   menu_items, app);
    gtk_table_attach (GTK_TABLE (table),
                      gtk_item_factory_get_widget (item_factory, "<main>"),
                        /* X direction */          /* Y direction */

                      0, 1,                      0, 1,
                      GTK_EXPAND | GTK_FILL,     0,
                      0,                         0);
                                                                                                                    
    setColor();
    area = gtk_drawing_area_new();
    gtk_widget_set_usize(area,WIDTH-50,HEIGHT-50);   
    gtk_timeout_add(INTERVAL,nextFrame,area);
    for(i=0; i<20; i++) newBall(&ball[i]);
    gtk_table_attach (GTK_TABLE (table),
                      area,
                        /* X direction */          /* Y direction */
                      0, 1,                      1, 2,
                      GTK_EXPAND | GTK_FILL,     0,
                      10,                         10);
    gtk_container_add(GTK_CONTAINER(app),table);
    gtk_widget_show_all(app);
    gtk_main();
    return(vt);
}
void setColor()
{
  gint i;	
  for(i=0; i<20; i++) 
    {
    	ball[i].colRed = (((double)rand()*0xFFFF)/RAND_MAX);
    	ball[i].colGreen = (((double)rand()*0xFFFF)/RAND_MAX);
    	ball[i].colBlue = (((double)rand()*0xFFFF)/RAND_MAX);
    }
}
gint nextFrame(gpointer data)
{
    int i;
    static GdkPixmap *pixmap = NULL;
    static GdkColormap *colormap = NULL;
    static GdkGC *gc = NULL;
    GdkColor color;
    GtkWidget *widget = GTK_WIDGET(data);

    if(colormap == NULL) {
        colormap = gdk_colormap_get_system();
        gc = gdk_gc_new(widget->window);
    }
    if(pixmap == NULL) {
        pixmap = gdk_pixmap_new(widget->window,
                    WIDTH,HEIGHT,-1);
    }
    gdk_draw_rectangle(pixmap,
            widget->style->white_gc,
            TRUE,
            0,0,
            WIDTH,HEIGHT);
    for(i=0; i<20; i++) {
        nextBall(&ball[i]);
		color.red = ball[i].colRed;
		color.green = ball[i].colGreen;
		color.blue = ball[i].colBlue;
		gdk_color_alloc(colormap,&color);
		gdk_gc_set_foreground(gc,&color);
                gdk_draw_arc(pixmap,
                gc,
                TRUE,
				(int)ball[i].x,(int)ball[i].y,
                DIAMETER*i/2,DIAMETER*i/2,
                0,360*64);
    }

    gdk_draw_pixmap(widget->window,
            widget->style->black_gc,
            pixmap,
            0,0,
            0,0,
            WIDTH,HEIGHT);

    return(TRUE);
}
void newBall(struct ballStruct *b)
{
    b->x = (((double)rand()*WIDTH)/RAND_MAX);
    b->y = (((double)rand()*HEIGHT)/RAND_MAX) - HEIGHT;
    do {
        b->xVelocity = (((double)rand()*10)/RAND_MAX) - 5;
        b->yVelocity = (((double)rand()*10)/RAND_MAX) - 5;
    } while(fabs(b->xVelocity) < 0.5);
}
void nextBall(struct ballStruct *b)
{
    if((b->x < -DIAMETER) || (b->x > WIDTH)) {
        newBall(b);
        return;
    }
    if((b->y + DIAMETER) >= HEIGHT) {
        if(b->yVelocity > 0)
            b->yVelocity = -b->yVelocity;
        b->yVelocity *= 0.9;
    } else {
        b->yVelocity += GRAVITY;
    }
    b->x += b->xVelocity;
    b->y += b->yVelocity;
}
gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data) {
    return(FALSE);
}
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data) {

    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();
    return(0);
}
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data) {

    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit ();
    return(1);
}
