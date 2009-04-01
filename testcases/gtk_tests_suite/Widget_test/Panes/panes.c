/*================================================================================================*/
/**^M
    @file   paned.c^M
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
   Inkina irina               10/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/* Paned Widgets
 *
 * The GtkHPaned and GtkVPaned Widgets divide their content
 * area into two panes with a divider in between that the
 * user can adjust. A separate child is placed into each
 * pane.
 *
 * There are a number of options that can be set for each pane.
 * This test contains both a horizontal (HPaned) and a vertical
 * (VPaned) widget, and allows you to adjust the options for
 * each side of each widget.
 */

#include <gtk/gtk.h>
gint vt=FALSE;
void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();
}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit ();
}


static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",		 NULL,	       0,		      0, "<Branch>" },
  { "/File/sep1",	 NULL,	       0,	      0, "<Separator>" },
  { "/File/_Quit - Pass","<control>Q", destroy_Quit,	      0 },
  { "/File/_Exit - Fail","<control>E", destroy_Exit,	      0 },
  { "/_Help",		 NULL,	       0,		      0, "<Branch>" },
  { "/Help/_About",      "<control>H",	0,	      0 },
};


void
toggle_resize (GtkWidget *widget,
	       GtkWidget *child)
{
  GtkPaned *paned = GTK_PANED (child->parent);
  gboolean is_child1 = (child == paned->child1);
  gboolean resize, shrink;

  resize = is_child1 ? paned->child1_resize : paned->child2_resize;
  shrink = is_child1 ? paned->child1_shrink : paned->child2_shrink;

  gtk_widget_ref (child);
  gtk_container_remove (GTK_CONTAINER (child->parent), child);
  if (is_child1)
    gtk_paned_pack1 (paned, child, !resize, shrink);
  else
    gtk_paned_pack2 (paned, child, !resize, shrink);
  gtk_widget_unref (child);
}

void
toggle_shrink (GtkWidget *widget,
	       GtkWidget *child)
{
  GtkPaned *paned = GTK_PANED (child->parent);
  gboolean is_child1 = (child == paned->child1);
  gboolean resize, shrink;

  resize = is_child1 ? paned->child1_resize : paned->child2_resize;
  shrink = is_child1 ? paned->child1_shrink : paned->child2_shrink;

  gtk_widget_ref (child);
  gtk_container_remove (GTK_CONTAINER (child->parent), child);
  if (is_child1)
    gtk_paned_pack1 (paned, child, resize, !shrink);
  else
    gtk_paned_pack2 (paned, child, resize, !shrink);
  gtk_widget_unref (child);
}

GtkWidget *
create_pane_options (GtkPaned	 *paned,
		     const gchar *frame_label,
		     const gchar *label1,
		     const gchar *label2)
{
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *label;
  GtkWidget *check_button;
  
  frame = gtk_frame_new (frame_label);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 4);
  
  table = gtk_table_new (3, 2, TRUE);
  gtk_container_add (GTK_CONTAINER (frame), table);
  
  label = gtk_label_new (label1);
  gtk_table_attach_defaults (GTK_TABLE (table), label,
			     0, 1, 0, 1);
  
  check_button = gtk_check_button_new_with_mnemonic ("_Resize");
  gtk_table_attach_defaults (GTK_TABLE (table), check_button,
			     0, 1, 1, 2);
  g_signal_connect (check_button, "toggled",
		    G_CALLBACK (toggle_resize), paned->child1);
  
  check_button = gtk_check_button_new_with_mnemonic ("_Shrink");
  gtk_table_attach_defaults (GTK_TABLE (table), check_button,
			     0, 1, 2, 3);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button),
			       TRUE);
  g_signal_connect (check_button, "toggled",
		    G_CALLBACK (toggle_shrink), paned->child1);
  
  label = gtk_label_new (label2);
  gtk_table_attach_defaults (GTK_TABLE (table), label,
			     1, 2, 0, 1);
  
  check_button = gtk_check_button_new_with_mnemonic ("_Resize");
  gtk_table_attach_defaults (GTK_TABLE (table), check_button,
			     1, 2, 1, 2);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button),
			       TRUE);
  g_signal_connect (check_button, "toggled",
		    G_CALLBACK (toggle_resize), paned->child2);
  
  check_button = gtk_check_button_new_with_mnemonic ("_Shrink");
  gtk_table_attach_defaults (GTK_TABLE (table), check_button,
			     1, 2, 2, 3);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button),
			       TRUE);
  g_signal_connect (check_button, "toggled",
		    G_CALLBACK (toggle_shrink), paned->child2);

  return frame;
}

int panes_main(int argc, char *argv[])
{
  static GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *hpaned;
  GtkWidget *vpaned;
  GtkWidget *button;
  GtkWidget *vbox, *box1;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;

      gtk_init(&argc,&argv);
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

      g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_main_quit), &window);

      gtk_window_set_title (GTK_WINDOW (window), "Panes");
      accel_group = gtk_accel_group_new ();
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1 = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
			  FALSE, FALSE, 0);

      separator = gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

/******************/      
      vbox = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (box1), vbox);
      
      vpaned = gtk_vpaned_new ();
      gtk_box_pack_start (GTK_BOX (vbox), vpaned, TRUE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER(vpaned), 5);

      hpaned = gtk_hpaned_new ();
      gtk_paned_add1 (GTK_PANED (vpaned), hpaned);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
      gtk_widget_set_size_request (frame, 60, 60);
      gtk_paned_add1 (GTK_PANED (hpaned), frame);
      
      button = gtk_button_new_with_mnemonic ("_Hi there");
      gtk_container_add (GTK_CONTAINER(frame), button);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
      gtk_widget_set_size_request (frame, 80, 60);
      gtk_paned_add2 (GTK_PANED (hpaned), frame);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
      gtk_widget_set_size_request (frame, 60, 80);
      gtk_paned_add2 (GTK_PANED (vpaned), frame);

      /* Now create toggle buttons to control sizing */

      gtk_box_pack_start (GTK_BOX (vbox),
			  create_pane_options (GTK_PANED (hpaned),
					       "Horizontal",
					       "Left",
					       "Right"),
			  FALSE, FALSE, 0);

      gtk_box_pack_start (GTK_BOX (vbox),
			  create_pane_options (GTK_PANED (vpaned),
					       "Vertical",
					       "Top",
					       "Bottom"),
			  FALSE, FALSE, 0);

      gtk_window_set_default_size (GTK_WINDOW (window), 240, 320);


    gtk_widget_show_all(window);
    gtk_main();
    return (vt);  
}
