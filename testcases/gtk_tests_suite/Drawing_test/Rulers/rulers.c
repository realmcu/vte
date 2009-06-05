/*================================================================================================*/
/**^M
    @file   rulers.c^M
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
   Inkina Irina               10/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/



#include <gtk/gtk.h>
#include <stdio.h>

#define EVENT_METHOD(i, x) GTK_WIDGET_GET_CLASS(i)->x

#define XSIZE  210
#define YSIZE  280
static gint button_press (GtkWidget *, GdkEventButton *);
static void menuitem_response (gchar *);
static gint key_press( GtkWidget *widget,GdkEvent *event );
gint vt=FALSE;
void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit();

}

void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit();
}
/* This routine gets control when the close button is clicked */
gint close_application( GtkWidget *widget,
                        GdkEvent  *event,
                        gpointer   data )
{
    gtk_main_quit ();
    return FALSE;
}

static GtkItemFactoryEntry menu_items[] =
{
  { "/_Quit-Pass", "<control>Q", destroy_Quit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Exit-Fail", "<control>E", destroy_Exit,       0, "<StockItem>", GTK_STOCK_QUIT }
};

/* The main routine */
int rulers_main( int   argc,char *argv[] )
{
    GtkWidget *window, *table, *area, *hrule, *vrule;
    GtkWidget *menu;
    char buf[128];
    GtkItemFactory *item_factory;
   
    
    /* Initialize GTK and create the main window */
    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_default_size (GTK_WINDOW (window),240,320);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (close_application), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    item_factory = gtk_item_factory_new (GTK_TYPE_MENU, "<main>",
                                        NULL);
    gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
    menu = gtk_item_factory_get_widget(item_factory, "<main>");
    gtk_widget_show (menu);
    g_signal_connect_swapped (G_OBJECT (window), "button_press_event",G_CALLBACK (button_press),
                              G_OBJECT (menu));

    g_signal_connect_swapped (G_OBJECT (window), "event", G_CALLBACK (key_press),
                              G_OBJECT (menu));

    gtk_widget_set_events (window, gtk_widget_get_events (window)
                             | GDK_BUTTON_PRESS_MASK);

    /* Create a table for placing the ruler and the drawing area */
    table = gtk_table_new (3, 2, FALSE);
    gtk_container_add (GTK_CONTAINER (window), table);

    area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (GTK_WIDGET (area), XSIZE, YSIZE);

    g_signal_connect_swapped (G_OBJECT (area), "event",
	                      G_CALLBACK (button_press),
                              G_OBJECT (menu));
     
    gtk_table_attach (GTK_TABLE (table), area, 1, 2, 1, 2,
                      GTK_EXPAND|GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_set_events (area, GDK_POINTER_MOTION_MASK |
                                 GDK_POINTER_MOTION_HINT_MASK);

    /* The horizontal ruler goes on top. As the mouse moves across the
     * drawing area, a motion_notify_event is passed to the
     * appropriate event handler for the ruler. */
    hrule = gtk_hruler_new ();
    gtk_ruler_set_metric (GTK_RULER (hrule), GTK_PIXELS);
    gtk_ruler_set_range (GTK_RULER (hrule), 7, 13, 0, 20);
    g_signal_connect_swapped (G_OBJECT (area), "motion_notify_event",
                              G_CALLBACK (EVENT_METHOD (hrule, motion_notify_event)),
                              G_OBJECT (hrule));
    gtk_table_attach (GTK_TABLE (table), hrule, 1, 2, 0, 1,
                      GTK_EXPAND|GTK_SHRINK|GTK_FILL, GTK_FILL, 0, 0);
    
    /* The vertical ruler goes on the left. As the mouse moves across
     * the drawing area, a motion_notify_event is passed to the
     * appropriate event handler for the ruler. */
    vrule = gtk_vruler_new ();
    gtk_ruler_set_metric (GTK_RULER (vrule), GTK_PIXELS);
    gtk_ruler_set_range (GTK_RULER (vrule), 0, YSIZE, 10, YSIZE );
    g_signal_connect_swapped (G_OBJECT (area), "motion_notify_event",
                              G_CALLBACK (EVENT_METHOD (vrule, motion_notify_event)),
                              G_OBJECT (vrule));
    gtk_table_attach (GTK_TABLE (table), vrule, 0, 1, 1, 2,
                      GTK_FILL, GTK_EXPAND|GTK_SHRINK|GTK_FILL, 0, 0);

    /* Now show everything */
    gtk_widget_show_all (window);
    gtk_main ();

    return (vt);
}

static gint key_press( GtkWidget *widget,
                          GdkEvent *event )
{
    if (event->type == GDK_KEY_PRESS)
     {

        gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL, NULL,
            ((GdkEventKey *) event)->keyval,((GdkEventKey *) event)->time);
        return TRUE;
    }

    /* Tell calling code that we have not handled this event; pass it on. */
    return FALSE;

}

static gint button_press( GtkWidget *widget,GdkEventButton *event )
{

if (event->button==3){
       gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL, NULL,
                        event->button, event->time);
        /* Tell calling code that we have handled this event; the buck
         * stops here. */
        return TRUE;
    }

    /* Tell calling code that we have not handled this event; pass it on. */
    return FALSE;
}



