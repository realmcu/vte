/**/
/**^M
    @file   colorsel.c^M
*

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.


Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
   Inkina Irina               10/09/2004     ??????      Initial version


Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

*/


/* Color Selector
 *
 * GtkColorSelection lets the user choose a color. GtkColorSelectionDialog is
 * a prebuilt dialog containing a GtkColorSelection.
 *
 */

#include <gtk/gtk.h>
gint vtFALSE;
static GtkWidget *window ;
static GtkWidget *da;
static GdkColor color;
static GtkWidget *frame;


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

static GtkItemFactoryEntry menu_items[] 
{
  { "/_File",   NULL,        0,        0, "<Branch>" },
  { "/File/sep1",  NULL,        0,       0, "<Separator>" },
  { "/File/_Quit-Pass", "<control>Q", destroy_Quit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", destroy_Exit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Help",   NULL,        0,        0, "<Branch>" },
  { "/Help/_About",  "<control>H", 0,       0 }
};

/* Expose callback for the drawing area  */

static gboolean
expose_event_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  if (widget->window)
    {
      GtkStyle *style;

      style  gtk_widget_get_style (widget);

      gdk_draw_rectangle (widget->window,
                          style->bg_gc[GTK_STATE_NORMAL],
                          TRUE,
                          event->area.x, event->area.y,
                          event->area.width, event->area.height);
    }

  return TRUE;
}

static void
change_color_callback (GtkWidget *button,
         gpointer   data)
{
  GtkWidget *dialog;
  GtkColorSelection *colorsel;
  gint response;

  dialog  gtk_color_selection_dialog_new ("Changing color");

  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
  gtk_window_set_default_size (GTK_WINDOW (dialog), 230, 310);
  gtk_window_set_policy(GTK_WINDOW (dialog),TRUE,TRUE,TRUE);

  colorsel  GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (dialog)->colorsel);

  gtk_color_selection_set_previous_color (colorsel, &color);
  gtk_color_selection_set_current_color (colorsel, &color);
  gtk_color_selection_set_has_palette (colorsel, TRUE);

  response  gtk_dialog_run (GTK_DIALOG (dialog));

  if (response  GTK_RESPONSE_OK)
    {
      gtk_color_selection_get_current_color (colorsel,
          &color);

      gtk_widget_modify_bg (da, GTK_STATE_NORMAL, &color);
    }

  gtk_widget_destroy (dialog);
}

GtkWidget *left_align_button_new (const char *label)
{
  GtkWidget *button  gtk_button_new_with_mnemonic (label);
  GtkWidget *child  gtk_bin_get_child (GTK_BIN (button));

  gtk_misc_set_alignment (GTK_MISC (child), 0., 0.5);

  return button;
}


int colorsel_main(int argc, char **argv)
{
  GtkWidget *vbox, *box1;
  GtkWidget *button;
  GtkWidget *alignment;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;

    gtk_init (&argc,&argv);

      color.red  0;
      color.blue  65535;
      color.green  0;

      window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title (GTK_WINDOW (window), "Color Selection");

      g_signal_connect (window, "destroy",
   G_CALLBACK (gtk_main_quit), &window);
      gtk_window_set_default_size (GTK_WINDOW (window), 240, 320);

      accel_group  gtk_accel_group_new ();
      item_factory  gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);//8
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1  gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
     FALSE, FALSE, 0);

      separator  gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

/////////
      vbox  gtk_vbox_new (FALSE, 8);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
      gtk_container_add (GTK_CONTAINER (box1/*window*/), vbox);

      /*  * Create the color swatch area */

      frame  gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

      da  gtk_drawing_area_new ();

      g_signal_connect (da, "expose_event",G_CALLBACK (expose_event_callback), NULL);

      /* set a minimum size */
      gtk_widget_set_size_request (da, 200, 200);
      /* set the color */
      gtk_widget_modify_bg (da, GTK_STATE_NORMAL, &color);

      gtk_container_add (GTK_CONTAINER (frame), da);

      alignment  gtk_alignment_new (1.0, 0.5, 0.0, 0.0);

      button  gtk_button_new_with_mnemonic ("_Change the above color");
      gtk_container_add (GTK_CONTAINER (alignment), button);

      gtk_box_pack_start (GTK_BOX (vbox), alignment, FALSE, FALSE, 0);

      g_signal_connect (button, "clicked",G_CALLBACK (change_color_callback), NULL);

    gtk_widget_show_all (window);
    gtk_main();


return (vt);
}
