
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "gtkdial.h"
gint vtFALSE;

void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vtFALSE;
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

void value_changed( GtkAdjustment *adjustment,
                    GtkWidget     *label )
{
  char buffer[16];

  sprintf(buffer,"%4.2f",adjustment->value);
  gtk_label_set_text (GTK_LABEL (label), buffer);
}

int dial_main( int   argc,char *argv[])
{
  GtkWidget *window;
  GtkAdjustment *adjustment;
  GtkWidget *dial;
  GtkWidget *frame;
  GtkWidget *vbox,*box1;
  GtkWidget *label;
    GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;

  gtk_init (&argc, &argv);

  window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 240, 320);

  gtk_window_set_title (GTK_WINDOW (window), "Dial");

  g_signal_connect (G_OBJECT (window), "destroy",
      G_CALLBACK (gtk_main_quit), &window);

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

  vbox  gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (box1), vbox);

  frame  gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (vbox), frame);

  adjustment  GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 100, 0.01, 0.1, 0));

  dial  gtk_dial_new (adjustment);
  gtk_dial_set_update_policy (GTK_DIAL (dial), GTK_UPDATE_DELAYED);

  gtk_container_add (GTK_CONTAINER (frame), dial);
  label  gtk_label_new ("0.00");
  gtk_box_pack_end (GTK_BOX (vbox), label, 0, 0, 0);
  g_signal_connect (G_OBJECT (adjustment), "value_changed",
      G_CALLBACK (value_changed), (gpointer) label);

  gtk_widget_show_all (window);

  gtk_main ();

  return (vt);
}
