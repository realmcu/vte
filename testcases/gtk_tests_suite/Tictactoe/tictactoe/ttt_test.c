/**/
/**^M
    @file   ttt_test.c^M
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


#include <stdlib.h>
#include <gtk/gtk.h>
#include "tictactoe.h"
gint vtFALSE;

void destroy_Quit( GtkWidget *widget,gpointer data )
{
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit();

}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vtTRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit();
}

static GtkItemFactoryEntry menu_items[] 
{
  { "/_File",   NULL,        0,        0, "<Branch>" },
  { "/File/sep1",  NULL,        0,       0, "<Separator>" },
  { "/File/_Quit - Pass","<control>Q", destroy_Quit,       0 },
  { "/File/_Exit - Fail","<control>E", destroy_Exit,       0 },
  { "/_Help",   NULL,        0,        0, "<Branch>" },
  { "/Help/_About",  "<control>H", 0,       0 },
};


void win( GtkWidget *widget,
          gpointer   data )
{
  g_print ("Yay!\n");
  tictactoe_clear (TICTACTOE (widget));
}

int ttt_test_main( int   argc,          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *ttt;
  GtkAccelGroup *accel_group;
  GtkItemFactory *item_factory;
  GtkWidget *separator;
  GtkWidget *box1;

  gtk_init (&argc, &argv);

  window  gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (window), "Aspect Frame");
  gtk_widget_set_size_request (window, 240, 320);

  g_signal_connect (G_OBJECT (window), "destroy",
      G_CALLBACK (gtk_main_quit), &window);

      accel_group  gtk_accel_group_new ();
      item_factory  gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1  gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
     FALSE, FALSE, 0);

      separator  gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

/////////

  ttt  tictactoe_new ();

  gtk_container_add (GTK_CONTAINER (box1), ttt);
      gtk_container_set_border_width (GTK_CONTAINER (box1), 10);

  g_signal_connect (G_OBJECT (ttt), "tictactoe",
      G_CALLBACK (win), NULL);

  gtk_widget_show_all (window);

  gtk_main ();

  return (vt);
}
