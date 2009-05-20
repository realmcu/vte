/**/
/**^M
    @file   label.c^M
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
  { "/Help/_About",   "<control>H", 0,       0 },
};

int label_main( int   argc,char *argv[] )
{
  static GtkWidget *window  NULL;
  GtkWidget *hbox;
  GtkWidget *vbox, *box1;
  GtkWidget *frame;
  GtkWidget *label;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;

  /* Initialise GTK */
  gtk_init (&argc, &argv);

    window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 240, 320);
    g_signal_connect (G_OBJECT (window), "destroy",
      G_CALLBACK (gtk_main_quit),&window);

    gtk_window_set_title (GTK_WINDOW (window), "Label");

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
  hbox  gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (box1), hbox);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 0);

  frame  gtk_frame_new ("Normal Label");
  label  gtk_label_new ("Normal label");//This is a Normal label
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  frame  gtk_frame_new ("Multi-line Label");  //This is a Multi-line label

  label  gtk_label_new ("Multi-line label.\nSecond line\n" \
  "Third line");
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  frame  gtk_frame_new ("Left Justified Label");//This is a Left-Justified
  label  gtk_label_new ("Left-Justified\n" \
    "Multi-line label.\nThird      line");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  frame  gtk_frame_new ("Right Justified Label");
  label  gtk_label_new ("This is a Right-Justified\nMulti-line label.\n" \
    "Fourth line, (j/k)");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  vbox  gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 0);
  frame  gtk_frame_new ("Line wrapped label");
  label  gtk_label_new ("This is an example of a\n line-wrapped label.");//  It " \
//    "should not be taking up the entire             " /* big space to test spacing */\
/*    "width allocated to it, but automatically " \
    "wraps the words to fit.  " \
    "The time has come, for all good men, to come to " \
    "the aid of their party.  " \
    "The sixth sheik's six sheep's sick.\n" \
    "     It supports multiple paragraphs correctly, " \
    "and  correctly   adds "\
    "many          extra  spaces. "); */
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  frame  gtk_frame_new ("Filled, wrapped label");
  label  gtk_label_new ("This is an example of a\n line-wrapped,filled label.");/* \
    "It should be taking "\
    "up the entire              width allocated to it.  " \
    "Here is a sentence to prove "\
    "my point.  Here is another sentence. "\
    "Here comes the sun, do de do de do.\n"\
    "    This is a new paragraph.\n"\
    "    This is another newer, longer, better " \
    "paragraph.  It is coming to an end, "\
    "unfortunately.");                   */
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_FILL);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  frame  gtk_frame_new ("Underlined label");
  label  gtk_label_new ("This label is underlined!\n"
    "This one is underlined\n in quite a funky fashion");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_label_set_pattern (GTK_LABEL (label),
    "_________________________ _ _____     __ _______ ___");
  gtk_container_add (GTK_CONTAINER (frame), label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  gtk_widget_show_all (window);

  gtk_main ();

  return (vt);
}
