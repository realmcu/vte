/**/
/**^M
    @file   progressbar.c^M
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


typedef struct _ProgressData {
  GtkWidget *window;
  GtkWidget *pbar;
  int timer;
  gboolean activity_mode;
} ProgressData;

/* Update the value of the progress bar so that we get
 * some movement */
gint progress_timeout( gpointer data )
{
  ProgressData *pdata  (ProgressData *)data;
  gdouble new_val;

  if (pdata->activity_mode)
    gtk_progress_bar_pulse (GTK_PROGRESS_BAR (pdata->pbar));
  else
    {
      /* Calculate the value of the progress bar using the
       * value range set in the adjustment object */

      new_val  gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (pdata->pbar)) + 0.01;

      if (new_val > 1.0)
 new_val  0.0;

      /* Set the new value */
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (pdata->pbar), new_val);
    }

  /* As this is a timeout function, return TRUE so that it
   * continues to get called */
  return TRUE;
}

/* Callback that toggles the text display within the progress bar trough */
void toggle_show_text( GtkWidget    *widget,
         ProgressData *pdata )
{
  const gchar *text;

  text  gtk_progress_bar_get_text (GTK_PROGRESS_BAR (pdata->pbar));
  if (text && *text)
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (pdata->pbar), "");
  else
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (pdata->pbar), "some text");
}

/* Callback that toggles the activity mode of the progress bar */
void toggle_activity_mode( GtkWidget    *widget,
      ProgressData *pdata )
{
  pdata->activity_mode  !pdata->activity_mode;
  if (pdata->activity_mode)
      gtk_progress_bar_pulse (GTK_PROGRESS_BAR (pdata->pbar));
  else
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (pdata->pbar), 0.0);
}


/* Callback that toggles the orientation of the progress bar */
void toggle_orientation( GtkWidget    *widget,
    ProgressData *pdata )
{
  switch (gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (pdata->pbar))) {
  case GTK_PROGRESS_LEFT_TO_RIGHT:
    gtk_progress_bar_set_orientation (GTK_PROGRESS_BAR (pdata->pbar),
          GTK_PROGRESS_RIGHT_TO_LEFT);
    break;
  case GTK_PROGRESS_RIGHT_TO_LEFT:
    gtk_progress_bar_set_orientation (GTK_PROGRESS_BAR (pdata->pbar),
          GTK_PROGRESS_LEFT_TO_RIGHT);
    break;
  }
}


/* Clean up allocated memory and remove the timer */
void destroy_progress( GtkWidget     *widget,
         ProgressData *pdata)
{
    gtk_timeout_remove (pdata->timer);
    pdata->timer  0;
    pdata->window  NULL;
    g_free (pdata);
    gtk_main_quit ();
}

int progressbar_main( int   argc,
          char *argv[])
{
    ProgressData *pdata;
    GtkWidget *align;
    GtkWidget *separator;
    GtkWidget *table;
    GtkWidget *button;
    GtkWidget *check;
    GtkWidget *vbox, *box1;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator1;

    gtk_init (&argc, &argv);

    /* Allocate memory for the data that is passed to the callbacks */
    pdata  g_malloc (sizeof (ProgressData));

    pdata->window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable (GTK_WINDOW (pdata->window), TRUE);

    g_signal_connect (G_OBJECT (pdata->window), "destroy",
               G_CALLBACK (gtk_main_quit),(gpointer) pdata);
    gtk_window_set_title (GTK_WINDOW (pdata->window), "GtkProgressBar");
      accel_group  gtk_accel_group_new ();
      item_factory  gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (pdata->window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (pdata->window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (pdata->window), 0);
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1  gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (pdata->window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
     FALSE, FALSE, 0);

      separator1  gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator1, FALSE, TRUE, 0);

/////////

    vbox  gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
    gtk_container_add (GTK_CONTAINER (box1), vbox);

    /* Create a centering alignment object */
    align  gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 5);

    /* Create the GtkProgressBar */
    pdata->pbar  gtk_progress_bar_new ();

    gtk_container_add (GTK_CONTAINER (align), pdata->pbar);

    /* Add a timer callback to update the value of the progress bar */
    pdata->timer  gtk_timeout_add (100, progress_timeout, pdata);

    separator  gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

    /* rows, columns, homogeneous */
    table  gtk_table_new (2, 3, FALSE);
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);

    /* Add a check button to select displaying of the trough text */
    check  gtk_check_button_new_with_label ("Show text");
    gtk_table_attach (GTK_TABLE (table), check, 0, 1, 0, 1,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL,
        5, 5);
    g_signal_connect (G_OBJECT (check), "clicked",
                      G_CALLBACK (toggle_show_text),
                      (gpointer) pdata);

    /* Add a check button to toggle activity mode */
    check  gtk_check_button_new_with_label ("Activity mode");
    gtk_table_attach (GTK_TABLE (table), check, 0, 1, 1, 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL,
                      5, 5);
    g_signal_connect (G_OBJECT (check), "clicked",
                      G_CALLBACK (toggle_activity_mode),
                      (gpointer) pdata);

    /* Add a check button to toggle orientation */
    check  gtk_check_button_new_with_label ("Right to Left");
    gtk_table_attach (GTK_TABLE (table), check, 0, 1, 2, 3,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL,
                      5, 5);
    g_signal_connect (G_OBJECT (check), "clicked",
                      G_CALLBACK (toggle_orientation),
                      (gpointer) pdata);

    /* Add a button to exit the program */
    button  gtk_button_new_with_label ("close");
    g_signal_connect_swapped (G_OBJECT (button), "clicked",
                              G_CALLBACK (gtk_widget_destroy),
                              G_OBJECT (pdata->window));
    gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

    /* This makes it so the button is the default. */
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

    /* This grabs this button to be the default button. Simply hitting
     * the "Enter" key will cause this button to activate. */
    gtk_widget_grab_default (button);

    gtk_widget_show_all (pdata->window);

    gtk_main ();

    return (vt);
}
