/*================================================================================================*/
/**
    @file   linestyle.c
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
  Filinova Natalia           13/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/



#include <gtk/gtk.h>

gint vt=FALSE;
gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data);

gboolean eventDraw(GtkWidget *widget,
        GdkEvent *event,gpointer data);
        
#define WIDTH 240
#define HEIGHT 320

static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",            NULL,         0,                     0, "<Branch>" },
  { "/File/_Quit-Pass", "<control>Q",  eventDestroyQuit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", eventDestroyExit,       0, "<StockItem>", GTK_STOCK_QUIT },

  { "/_Help",            NULL,         0,                     0, "<Branch>" }
        
};

int linestyle_main(int argc,char *argv[])
{
    GtkWidget *app;
    GtkWidget *area;
    GtkWidget *table;
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;
    

    gtk_init(&argc,&argv);

    app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (app), "Creating Colors");
    gtk_window_set_default_size (GTK_WINDOW (app),240,320);

    gtk_signal_connect(GTK_OBJECT(app),"delete_event",
            GTK_SIGNAL_FUNC(eventDelete),NULL);
    gtk_signal_connect(GTK_OBJECT(app),"destroy",
            GTK_SIGNAL_FUNC(eventDestroyQuit),NULL);


    table = gtk_table_new (2, 1, FALSE);
    gtk_widget_show(table);

   // gtk_container_set_border_width(GTK_CONTAINER(app),20);

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


    area = gtk_drawing_area_new();
    gtk_widget_set_usize(area,WIDTH,HEIGHT);
    gtk_table_attach (GTK_TABLE (table),
                      area,
                        /* X direction */          /* Y direction */
                      0, 1,                      1, 2,
                      GTK_EXPAND | GTK_FILL,     0,
                      0,                         0);

    gtk_signal_connect(GTK_OBJECT(area),"event",
            GTK_SIGNAL_FUNC(eventDraw),NULL);
    gtk_container_add(GTK_CONTAINER(app),table);
    gtk_widget_show_all(app);
    gtk_main();
    return(vt);
}
gboolean eventDraw(GtkWidget *widget,
        GdkEvent *event,gpointer data) {
    int i;
    int x = 25;
    int y = 160;
    char dashList[4] = { 5, 15, 20, 30 };
    GdkGC *gc;
    GdkColormap *colormap;
    GdkColor foreground;
    GdkColor background;
    GdkFont *font;
    GdkPoint points[3] = {
        { 60, 40 },
        { 20, 120 },
        { 60, 120 }
    };

    font = gdk_font_load("vga");
    gc = gdk_gc_new(widget->window);
    colormap = gdk_colormap_get_system();
    if(gdk_color_parse("blue",&foreground)) {
        gdk_color_alloc(colormap,&foreground);
        gdk_gc_set_foreground(gc,&foreground);
    }
    if(gdk_color_parse("yellow",&background)) {
        gdk_color_alloc(colormap,&background);
        gdk_gc_set_background(gc,&background);
    }

    /*- The MITER join -*/
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_BUTT,
            GDK_JOIN_MITER);
    gdk_draw_polygon(widget->window,
            gc,
            FALSE,
            points,
            (int)(sizeof(points)/sizeof(GdkPoint)));
    gdk_draw_string(widget->window,
            font,
            gc,
            points[1].x,points[1].y + 20,
            "Miter");

    /*- The ROUND join -*/
    for(i=0; i<3; i++)
        points[i].x += 80;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_BUTT,
            GDK_JOIN_ROUND);
    gdk_draw_polygon(widget->window,
            gc,
            FALSE,
            points,
            (int)(sizeof(points)/sizeof(GdkPoint)));
    gdk_draw_string(widget->window,
            font,
            gc,
            points[1].x,points[1].y + 20,
            "Round");

    /*- The BEVEL join -*/
    for(i=0; i<3; i++)
        points[i].x += 80;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_BUTT,
            GDK_JOIN_BEVEL);
    gdk_draw_polygon(widget->window,
            gc,
            FALSE,
            points,
            (int)(sizeof(points)/sizeof(GdkPoint)));
    gdk_draw_string(widget->window,
            font,
            gc,
            points[1].x,points[1].y + 20,
            "Bevel");

    /*- Cap BUTT -*/
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_BUTT,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "Butt");
    
    /*- Cap NOT LAST -*/
    y += 20;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_NOT_LAST,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "Not last");
    
    /*- Cap ROUND -*/
    y += 20;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_ROUND,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "Round");
    
    /*- Cap PROJECTING -*/
    y += 20;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_PROJECTING,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "Projecting");

    /*- Style SOLID -*/
    y += 40;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_SOLID,
            GDK_CAP_BUTT,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "Solid");

    /*- Style ON_OFF_DASH -*/
    y += 20;
    gdk_gc_set_dashes(gc,
            0,
            dashList,
            4);
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_ON_OFF_DASH,
            GDK_CAP_BUTT,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "On off dash");

    /*- Style DOUBLE DASH -*/
    y += 20;
    gdk_gc_set_line_attributes(gc,
            10,
            GDK_LINE_DOUBLE_DASH,
            GDK_CAP_BUTT,
            GDK_JOIN_MITER);
    gdk_draw_line(widget->window,
            gc,
            x,y,
            x + 120,y);
    gdk_draw_string(widget->window,
            font,
            gc,
            x + 130,y + 5,
            "Double dash");

    gdk_font_unref(font);
    gdk_gc_unref(gc);
    return(TRUE);
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
