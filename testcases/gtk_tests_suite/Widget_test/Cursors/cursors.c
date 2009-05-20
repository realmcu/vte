/**/
/**^M
    @file   cursors.c^M
*

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.


Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
   Inkina irina               10/09/2004     ??????      Initial version


Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

*/


#include <gtk/gtk.h>
#include <stdio.h>
gint vtFALSE;
static gint button_press (GtkWidget *, GdkEventButton *);

void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vtFALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();
}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vtTRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit ();
}



/* makes the sample window */

void create_cursors( void )
{
    GtkWidget *window;
    GtkWidget *event_box[22];
    GtkWidget *button;
    GtkWidget *bbox[8], *box;
    GtkWidget *menu_p;
    GtkWidget *menu_items;
    char buf[128];
     int i,j;
    struct Curs{int curs;char *buffer;} ref[]{
             {0,"SPIDER"}, {2,"HAND1"},
             {4,"CURSOR"},{6,"ARROW"},
             {8,"BOAT"},{10,"BOGOSITY"} ,
             {12,"BOTTOM_LEFT_CORNER"},
             {14,"BOTTOM_RIGHT_CORNER"},
             {16,"BOTTOM_SIDE"},
             {18,"BOTTOM_TEE"},{20,"BOX_SPIRAL"},
             {22,"CENTER_PTR" }};


    /* Standard window-creating stuff */
    window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window),240,320);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit),
                      &window);
    gtk_window_set_title (GTK_WINDOW (window), "range controls");
      gtk_container_set_border_width (GTK_CONTAINER (window), 0);//8
///////
    menu_p  gtk_menu_new ();

            /* Copy the names to the buf. */
            sprintf (buf, " Quit - pass ");
            /* Create a new menu-item with a name... */
            menu_items  gtk_menu_item_new_with_label (buf);
            /* ...and add it to the menu. */
            gtk_menu_shell_append (GTK_MENU_SHELL (menu_p), menu_items);
          /* Do something interesting when the menuitem is selected */
          g_signal_connect_swapped (G_OBJECT (menu_items), "activate",
                        G_CALLBACK (destroy_Quit),
                                      (gpointer) g_strdup (buf));
            /* Show the widget */
            gtk_widget_show (menu_items);

            sprintf (buf, " Exit - fail ");
            menu_items  gtk_menu_item_new_with_label (buf);
            gtk_menu_shell_append (GTK_MENU_SHELL (menu_p), menu_items);
           g_signal_connect_swapped (G_OBJECT (menu_items), "activate",
                        G_CALLBACK (destroy_Exit),(gpointer) g_strdup (buf));
            gtk_widget_show (menu_items);

    g_signal_connect_swapped (G_OBJECT (window), "button_press_event",G_CALLBACK (button_press),
                              G_OBJECT (menu_p));
    gtk_widget_set_events (window, gtk_widget_get_events (window)| GDK_BUTTON_PRESS_MASK);
///////
    box  gtk_vbox_new (FALSE,5);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    gtk_container_add (GTK_CONTAINER (window), box);

  for (j  0; j < 6; j++)
   {

    bbox[j]  gtk_hbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (bbox[j]), 2);
    gtk_container_add (GTK_CONTAINER (box), bbox[j]);
    gtk_box_set_spacing(GTK_BOX(bbox[j]),15);
    gtk_box_set_homogeneous(GTK_BOX(bbox[j]),15);


    /* Create an EventBox and add it to our toplevel window */
    for (i  0; i <2; i++)
     {
        event_box[i]  gtk_event_box_new ();
        gtk_container_add (GTK_CONTAINER (bbox[j]), event_box[i]);
        button  gtk_button_new_with_label (ref[i+j*2].buffer);
        gtk_container_add (GTK_CONTAINER (event_box[i]), button);
        gtk_widget_realize (event_box[i]);
    /* Yet one more thing you need an X window for ... */
        gdk_window_set_cursor (event_box[i]->window, gdk_cursor_new(ref[i+j*2].curs));
      }
   }


   gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);


    gtk_widget_show_all (window);
}

static gint button_press( GtkWidget *widget,GdkEventButton *event )
{
if (event->button3){
        gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL, NULL,
                        event->button, event->time);
        /* Tell calling code that we have handled this event; the buck
         * stops here. */
        return TRUE;
    }

    /* Tell calling code that we have not handled this event; pass it on. */
    return FALSE;
}

int cursors_main( int   argc, char *argv[] )
{
    gtk_init (&argc, &argv);

    create_cursors ();

    gtk_main ();

    return (vt);
}

