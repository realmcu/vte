/*================================================================================================*/
/**^M
    @file   list_store.c^M
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


/* Tree View/List Store
 *
 * The GtkListStore is used to store data in list form, to be used
 * later on by a GtkTreeView to display it. This demo builds a
 * simple GtkListStore and displays it. See the Stock Browser
 * demo for a more advanced example.
 *
 */

#include <gtk/gtk.h>

static GtkWidget *window;
gint vt=FALSE;
typedef struct
{
  const gboolean  fixed;
  const guint     number;
  const gchar    *severity;
  const gchar    *description;
}
Bug;

enum
{
  COLUMN_FIXED,
  COLUMN_NUMBER,
  COLUMN_SEVERITY,
  COLUMN_DESCRIPTION,
  NUM_COLUMNS
};

static Bug data[] =
{
  { FALSE, 60482, "Normal",     "scrollable notebooks and hidden tabs" },
  { FALSE, 60620, "Critical",   "gdk_window_clear_area (gdkwindow-win32.c) is not thread-safe" },
  { FALSE, 50214, "Major",      "Xft support does not clean up correctly" },
  { TRUE,  52877, "Major",      "GtkFileSelection needs a refresh method. " },
  { FALSE, 56070, "Normal",     "Can't click button after setting in sensitive" },
  { TRUE,  56355, "Normal",     "GtkLabel - Not all changes propagate correctly" },
  { FALSE, 50055, "Normal",     "Rework width/height computations for TreeView" },
  { FALSE, 58278, "Normal",     "gtk_dialog_set_response_sensitive () doesn't work" },
  { FALSE, 55767, "Normal",     "Getters for all setters" },
  { FALSE, 56925, "Normal",     "Gtkcalender size" },
  { FALSE, 56221, "Normal",     "Selectable label needs right-click copy menu" },
  { TRUE,  50939, "Normal",     "Add shift clicking to GtkTextView" },
  { FALSE, 6112,  "Enhancement","netscape-like collapsable toolbars" },
  { FALSE, 1,     "Normal",     "First bug :=)" },
};


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
  { "/Help/_About",	  "<control>H",	0,	      0 },
};



static GtkTreeModel *
create_model (void)
{
  gint i = 0;
  GtkListStore *store;
  GtkTreeIter iter;

  /* create list store */
  store = gtk_list_store_new (NUM_COLUMNS,
			      G_TYPE_BOOLEAN,
			      G_TYPE_UINT,
			      G_TYPE_STRING,
			      G_TYPE_STRING);

  /* add data to the list store */
  for (i = 0; i < G_N_ELEMENTS (data); i++)
    {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
			  COLUMN_FIXED, data[i].fixed,
			  COLUMN_NUMBER, data[i].number,
			  COLUMN_SEVERITY, data[i].severity,
			  COLUMN_DESCRIPTION, data[i].description,
			  -1);
    }

  return GTK_TREE_MODEL (store);
}

static void
fixed_toggled (GtkCellRendererToggle *cell,
	       gchar                 *path_str,
	       gpointer               data)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreeIter  iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean fixed;

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, COLUMN_FIXED, &fixed, -1);

  /* do something with the value */
  fixed ^= 1;

  /* set new value */
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_FIXED, fixed, -1);

  /* clean up */
  gtk_tree_path_free (path);
}

static void
add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /* column for fixed toggles */
  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled",
		    G_CALLBACK (fixed_toggled), model);

  column = gtk_tree_view_column_new_with_attributes ("Fixed?",
						     renderer,
						     "active", COLUMN_FIXED,
						     NULL);

  /* set this column to a fixed sizing (of 50 pixels) */
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
				   GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 50);
  gtk_tree_view_append_column (treeview, column);

  /* column for bug numbers */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Bug number",
						     renderer,
						     "text",
						     COLUMN_NUMBER,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_NUMBER);
  gtk_tree_view_append_column (treeview, column);

  /* column for severities */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Severity",
						     renderer,
						     "text",
						     COLUMN_SEVERITY,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_SEVERITY);
  gtk_tree_view_append_column (treeview, column);

  /* column for description */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Description",
						     renderer,
						     "text",
						     COLUMN_DESCRIPTION,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_DESCRIPTION);
  gtk_tree_view_append_column (treeview, column);
}

int list_store_main(int argc, char *argv[])
{

      GtkWidget *vbox, *box1;
      GtkWidget *label;
      GtkWidget *sw;
      GtkTreeModel *model;
      GtkWidget *treeview;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;

      gtk_init (&argc,&argv);

      /* create window, etc */
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title (GTK_WINDOW (window), "GtkListStore");

      g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_main_quit), &window);
//      gtk_container_set_border_width (GTK_CONTAINER (window), 8);
      accel_group = gtk_accel_group_new ();
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);//8
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1 = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
			  FALSE, FALSE, 0);

      separator = gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

/////////
      gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);

      vbox = gtk_vbox_new (FALSE, 8);
      gtk_container_add (GTK_CONTAINER (box1), vbox);

      label = gtk_label_new ("This is the bug list (note: not based\non real data, it would be\n nice to have a nice ODBC\n interface to bugzilla or so, though).");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

      sw = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
					   GTK_SHADOW_ETCHED_IN);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				      GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);

      /* create tree model */
      model = create_model ();

      /* create tree view */
      treeview = gtk_tree_view_new_with_model (model);
      gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
      gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview),
				       COLUMN_DESCRIPTION);

      g_object_unref (model);

      gtk_container_add (GTK_CONTAINER (sw), treeview);

      /* add columns to the tree view */
      add_columns (GTK_TREE_VIEW (treeview));

      /* finish & show */
      gtk_window_set_default_size (GTK_WINDOW (window), 240, 320);
    

    gtk_widget_show_all (window);
  
    gtk_main();
    return (vt);
  
}
