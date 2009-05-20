/**/
/**^M
    @file   entry.c^M
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


#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
gint vtFALSE;
static GtkWidget *window;
static GtkWidget *entry1  NULL;
static GtkWidget *entry2  NULL;

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
  { "/File/_Quit-Pass", "<control>Q", destroy_Quit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", destroy_Exit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Help",   NULL,        0,        0, "<Branch>" },
  { "/Help/_About",  "<control>H", 0,       0 },
};

static void
message_dialog_clicked (GtkButton *button,
   gpointer   user_data)
{
  GtkWidget *dialog;
  static gint i  1;

  dialog  gtk_message_dialog_new (GTK_WINDOW (window),
       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
       GTK_MESSAGE_INFO,
       GTK_BUTTONS_OK,
       "This message box has been\n popped up the following\n"
       "number of times:\n\n"
       "%d", i);
  gtk_widget_set_usize(dialog,240,120);


  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  i++;
}

static void
interactive_dialog_clicked (GtkButton *button,
       gpointer   user_data)
{
  GtkWidget *dialog;
  GtkWidget *hbox;
  GtkWidget *stock;
  GtkWidget *table;
  GtkWidget *local_entry1;
  GtkWidget *local_entry2;
  GtkWidget *label;
  gint response;

  dialog  gtk_dialog_new_with_buttons ("Interactive Dialog",
     GTK_WINDOW (window),
     GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
     GTK_STOCK_OK,
     GTK_RESPONSE_OK,
                                        "_Non-stock Button",
                                        GTK_RESPONSE_CANCEL,
     NULL);
gtk_widget_set_usize(dialog,240,120);


  hbox  gtk_hbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 0);

  stock  gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (hbox), stock, FALSE, FALSE, 0);

  table  gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 4);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0);
  label  gtk_label_new_with_mnemonic ("_Entry 1");
  gtk_table_attach_defaults (GTK_TABLE (table),
        label,
        0, 1, 0, 1);
  local_entry1  gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (local_entry1), gtk_entry_get_text (GTK_ENTRY (entry1)));
  gtk_table_attach_defaults (GTK_TABLE (table), local_entry1, 1, 2, 0, 1);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), local_entry1);

  label  gtk_label_new_with_mnemonic ("E_ntry 2");
  gtk_table_attach_defaults (GTK_TABLE (table),
        label,
        0, 1, 1, 2);

  local_entry2  gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (local_entry2), gtk_entry_get_text (GTK_ENTRY (entry2)));
  gtk_table_attach_defaults (GTK_TABLE (table), local_entry2, 1, 2, 1, 2);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), local_entry2);

  gtk_widget_show_all (hbox);
  response  gtk_dialog_run (GTK_DIALOG (dialog));

  if (response  GTK_RESPONSE_OK)
    {
      gtk_entry_set_text (GTK_ENTRY (entry1), gtk_entry_get_text (GTK_ENTRY (local_entry1)));
      gtk_entry_set_text (GTK_ENTRY (entry2), gtk_entry_get_text (GTK_ENTRY (local_entry2)));
    }

  gtk_widget_destroy (dialog);
}

/* Create an Arrow widget with the specified parameters
 * and pack it into a button */
GtkWidget *create_arrow_button( GtkArrowType  arrow_type,
    GtkShadowType shadow_type )
{
  GtkWidget *button;
  GtkWidget *arrow;

  button  gtk_button_new ();
  arrow  gtk_arrow_new (arrow_type, shadow_type);

  gtk_container_add (GTK_CONTAINER (button), arrow);

  gtk_widget_show (button);
  gtk_widget_show (arrow);

  return button;
}

void enter_callback( GtkWidget *widget,
                     GtkWidget *entry )
{
  const gchar *entry_text;
  entry_text  gtk_entry_get_text (GTK_ENTRY (entry));
  printf("Entry contents: %s\n", entry_text);
}

void entry_toggle_editable( GtkWidget *checkbutton,
                            GtkWidget *entry )
{
  gtk_editable_set_editable (GTK_EDITABLE (entry),
                             GTK_TOGGLE_BUTTON (checkbutton)->active);
}

void entry_toggle_visibility( GtkWidget *checkbutton,
                              GtkWidget *entry )
{
  gtk_entry_set_visibility (GTK_ENTRY (entry),
       GTK_TOGGLE_BUTTON (checkbutton)->active);
}

int entry_main( int   argc,
          char *argv[] )
{

    GtkWidget *vbox, *hbox, *box1;
    GtkWidget *entry;
    GtkWidget *button;
    GtkWidget *check;
    gint tmp_pos;
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;
    GtkWidget *separator;
    GtkWidget *frame;
    GtkWidget *vbox2,*vbox3;
    GtkWidget *table;
    GtkWidget *label;

    gtk_init (&argc, &argv);

    /* create a new window */
    window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 240, 320);
    gtk_window_set_title (GTK_WINDOW (window), "GTK Entry");
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), &window);
/*    g_signal_connect_swapped (G_OBJECT (window), "delete_event",
                              G_CALLBACK (gtk_widget_destroy),
                              G_OBJECT (window));
*/      accel_group  gtk_accel_group_new ();
      item_factory  gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);

      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1  gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
     FALSE, FALSE, 0);

      separator  gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

/////////



    vbox  gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (box1), vbox);

    entry  gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (G_OBJECT (entry), "activate",
        G_CALLBACK (enter_callback),
        (gpointer) entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "hello");
    tmp_pos  GTK_ENTRY (entry)->text_length;
    gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry),
           0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);

    hbox  gtk_hbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (vbox), hbox);

    check  gtk_check_button_new_with_label ("Editable");
    gtk_box_pack_start (GTK_BOX (hbox), check, TRUE, TRUE, 0);
    g_signal_connect (G_OBJECT (check), "toggled",
               G_CALLBACK (entry_toggle_editable), (gpointer) entry);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);

    check  gtk_check_button_new_with_label ("Visible");
    gtk_box_pack_start (GTK_BOX (hbox), check, TRUE, TRUE, 0);
    g_signal_connect (G_OBJECT (check), "toggled",
               G_CALLBACK (entry_toggle_visibility), (gpointer) entry);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
///////////////////////////
      frame  gtk_frame_new ("Dialogs");
      gtk_container_add (GTK_CONTAINER (/*window*/box1), frame);

      vbox  gtk_vbox_new (FALSE, 8);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
      gtk_container_add (GTK_CONTAINER (frame), vbox);

      /* Standard message dialog */
      hbox  gtk_hbox_new (FALSE, 8);
      gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
      vbox2  gtk_vbox_new (FALSE, 0);
      vbox3  gtk_hbox_new (FALSE, 0);

      button  gtk_button_new_with_mnemonic ("_Message Dialog");
      gtk_box_pack_start (GTK_BOX (hbox), vbox2, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (vbox2), vbox3, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (vbox3), button, FALSE, FALSE, 0);

      button  create_arrow_button (GTK_ARROW_LEFT, GTK_SHADOW_ETCHED_IN);
      gtk_box_pack_start (GTK_BOX (vbox3), button, FALSE, FALSE,0);
      g_signal_connect (button, "clicked",
   G_CALLBACK (message_dialog_clicked), NULL);

      gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, FALSE, 0);

      /* Interactive dialog*/
      hbox  gtk_hbox_new (FALSE, 8);
      gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
      vbox2  gtk_vbox_new (FALSE, 0);
      vbox3  gtk_hbox_new (FALSE, 0);

      button  gtk_button_new_with_mnemonic ("_Interactive Dialog");
      gtk_box_pack_start (GTK_BOX (hbox), vbox2, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (vbox2), vbox3, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (vbox3), button, FALSE, FALSE, 0);

      button  create_arrow_button (GTK_ARROW_RIGHT, GTK_SHADOW_ETCHED_OUT);
      gtk_box_pack_start (GTK_BOX (vbox3), button, FALSE, FALSE,0);
      g_signal_connect (button, "clicked",
   G_CALLBACK (interactive_dialog_clicked), NULL);

      table  gtk_table_new (3, 2,FALSE);
      gtk_table_set_row_spacings (GTK_TABLE (table), 4);
      gtk_table_set_col_spacings (GTK_TABLE (table), 4);
      gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

      label  gtk_label_new_with_mnemonic ("_Entry 1");
      gtk_table_attach_defaults (GTK_TABLE (table),
     label,
     0, 1, 0, 1);
      button  create_arrow_button (GTK_ARROW_DOWN, GTK_SHADOW_ETCHED_IN);
      gtk_table_attach_defaults (GTK_TABLE (table),
     button,
     1,2, 0, 1);

      entry1  gtk_entry_new ();
      gtk_table_attach_defaults (GTK_TABLE (table), entry1, 2, 3, 0, 1);
      gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry1);
      gtk_widget_set_size_request (GTK_WIDGET (entry1), 40, 20);
      gtk_window_set_policy(GTK_WINDOW (entry1),TRUE,TRUE,TRUE);

      label  gtk_label_new_with_mnemonic ("E_ntry 2");

      gtk_table_attach_defaults (GTK_TABLE (table),
     label,
     0, 1, 1, 2);
      button  create_arrow_button (GTK_ARROW_UP, GTK_SHADOW_IN);
      gtk_table_attach_defaults (GTK_TABLE (table),
     button,
     1,2, 1, 2);

      entry2  gtk_entry_new ();
      gtk_table_attach_defaults (GTK_TABLE (table), entry2, 2, 3, 1, 2);
      gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry2);

    gtk_widget_show_all (window);

    gtk_main();

    return (vt);
}
