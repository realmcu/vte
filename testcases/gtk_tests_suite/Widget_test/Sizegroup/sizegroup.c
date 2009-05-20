/**/
/**^M
    @file   sizegroup.c^M
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



/* Size Groups
 *
 * GtkSizeGroup provides a mechanism for grouping a number of
 * widgets together so they all request the same amount of space.
 * This is typically useful when you want a column of widgets to
 * have the same size, but you can't use a GtkTable widget.
 *
 * Note that size groups only affect the amount of space requested,
 * not the size that the widgets finally receive. If you want the
 * widgets in a GtkSizeGroup to actually be the same size, you need
 * to pack them in such a way that they get the size they request
 * and not more. For example, if you are packing your widgets
 * into a table, you would not include the GTK_FILL flag.

 * Menus
 *
 * There are several widgets involved in displaying menus. The
 * GtkMenuBar widget is a horizontal menu bar, which normally appears
 * at the top of an application. The GtkMenu widget is the actual menu
 * that pops up. Both GtkMenuBar and GtkMenu are subclasses of
 * GtkMenuShell; a GtkMenuShell contains menu items
 * (GtkMenuItem). Each menu item contains text and/or images and can
 * be selected by the user.
 *
 * There are several kinds of menu item, including plain GtkMenuItem,
 * GtkCheckMenuItem which can be checked/unchecked, GtkRadioMenuItem
 * which is a check menu item that's in a mutually exclusive group,
 * GtkSeparatorMenuItem which is a separator bar, GtkTearoffMenuItem
 * which allows a GtkMenu to be torn off, and GtkImageMenuItem which
 * can place a GtkImage or other widget next to the menu text.
 *
 * A GtkMenuItem can have a submenu, which is simply a GtkMenu to pop
 * up when the menu item is selected. Typically, all menu items in a menu bar
 * have submenus.
 *
 * The GtkOptionMenu widget is a button that pops up a GtkMenu when clicked.
 * It's used inside dialogs and such.
 *
 * GtkItemFactory provides a higher-level interface for creating menu bars
 * and menus; while you can construct menus manually, most people don't
 * do that. There's a separate demo for GtkItemFactory.
 *
 */


#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
gint vtFALSE;
GtkWidget *window;

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



/* Convenience function to create an option menu holding a number of strings
 */
GtkWidget *
create_option_menu (const char **strings)
{
  GtkWidget *menu;
  GtkWidget *option_menu;
  const char **str;

  menu  gtk_menu_new ();

  for (str  strings; *str; str++)
    {
      GtkWidget *menu_item  gtk_menu_item_new_with_label (*str);
      gtk_widget_show (menu_item);

      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
    }

  option_menu  gtk_option_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);

  return option_menu;
}

static void
add_row (GtkTable     *table,
  int           row,
  GtkSizeGroup *size_group,
  const char   *label_text,
  const char  **options)
{
  GtkWidget *option_menu;
  GtkWidget *label;

  label  gtk_label_new_with_mnemonic (label_text);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 1);
  gtk_table_attach (GTK_TABLE (table), label,
      0, 1,                  row, row + 1,
      GTK_EXPAND | GTK_FILL, 0,
      0,                     0);

  option_menu  create_option_menu (options);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), option_menu);
  gtk_size_group_add_widget (size_group, option_menu);
  gtk_table_attach (GTK_TABLE (table), option_menu,
      1, 2,                  row, row + 1,
      0,                     0,
      0,                     0);
}

static void
toggle_grouping (GtkToggleButton *check_button,
   GtkSizeGroup    *size_group)
{
  GtkSizeGroupMode new_mode;

  /* GTK_SIZE_GROUP_NONE is not generally useful, but is useful
   * here to show the effect of GTK_SIZE_GROUP_HORIZONTAL by
   * contrast.
   */
  if (gtk_toggle_button_get_active (check_button))
    new_mode  GTK_SIZE_GROUP_HORIZONTAL;
  else
    new_mode  GTK_SIZE_GROUP_NONE;

  gtk_size_group_set_mode (size_group, new_mode);
}

static GtkWidget *
create_menu (gint     depth,
      gboolean tearoff)
{
  GtkWidget *menu;
  GtkWidget *menuitem;
  GSList *group;
  char buf[32];
  int i, j;

  if (depth < 1)
    return NULL;

  menu  gtk_menu_new ();
  group  NULL;

  if (tearoff)
    {
      menuitem  gtk_tearoff_menu_item_new ();
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
    }

  for (i  0, j  1; i < 5; i++, j++)
    {
      sprintf (buf, "item %2d - %d", depth, j);
      menuitem  gtk_radio_menu_item_new_with_label (group, buf);
      group  gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menuitem));

      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
      if (i  3)
 gtk_widget_set_sensitive (menuitem, FALSE);

      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), create_menu (depth - 1, TRUE));
    }

  return menu;
}


int sizegroup_main(int argc, char *argv[])
{
  GtkWidget *table;
  GtkWidget *frame;
  GtkWidget *vbox, *box1,*box2;
  GtkWidget *check_button;
  GtkWidget *optionmenu;
  GtkSizeGroup *size_group;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;
      GtkWidget *menubar;
      GtkWidget *menu;
      GtkWidget *menuitem;

  static const char *color_options[]  {
    "Red", "Green", "Blue", NULL
  };

  static const char *dash_options[]  {
    "Solid", "Dashed", "Dotted", NULL
  };

  static const char *end_options[]  {
    "Square", "Round", "Arrow", NULL
  };

    gtk_init (&argc,&argv);

      window  gtk_dialog_new_with_buttons ("GtkSizeGroup",
         NULL, 0,NULL,
         GTK_STOCK_CLOSE,//destroy_Quit,
         GTK_RESPONSE_NONE,
         NULL);

      g_signal_connect (window, "response",
   G_CALLBACK (gtk_widget_destroy), NULL);
      g_signal_connect (window, "destroy",G_CALLBACK (gtk_main_quit), &window);
      accel_group  gtk_accel_group_new ();
      item_factory  gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1  gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER ((GTK_DIALOG (window)->vbox)), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
     FALSE, FALSE, 0);

      separator  gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

////////

      vbox  gtk_vbox_new (FALSE, 5);
      gtk_box_pack_start (GTK_BOX (box1), vbox, TRUE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
///////////
      menubar  gtk_menu_bar_new ();
      gtk_box_pack_start (GTK_BOX (vbox/*box1*/), menubar, FALSE, TRUE, 0);
      gtk_widget_show (menubar);

      menu  create_menu (2, TRUE);

      menuitem  gtk_menu_item_new_with_label ("test\nline2");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

      menuitem  gtk_menu_item_new_with_label ("foo");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), create_menu (3, TRUE));
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

      menuitem  gtk_menu_item_new_with_label ("bar");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), create_menu (4, TRUE));
      gtk_menu_item_set_right_justified (GTK_MENU_ITEM (menuitem), TRUE);
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

      box2  gtk_vbox_new (FALSE, 10);
      gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
      gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
      gtk_widget_show (box2);

      menu  create_menu (1, FALSE);
      gtk_menu_set_accel_group (GTK_MENU (menu), accel_group);

      menuitem  gtk_separator_menu_item_new ();
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);

      menuitem  gtk_check_menu_item_new_with_label ("Accelerate Me");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
      gtk_widget_add_accelerator (menuitem,
      "activate",
      accel_group,
      GDK_F1,
      0,
      GTK_ACCEL_VISIBLE);
      menuitem  gtk_check_menu_item_new_with_label ("Accelerator Locked");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
      gtk_widget_add_accelerator (menuitem,
      "activate",
      accel_group,
      GDK_F2,
      0,
      GTK_ACCEL_VISIBLE | GTK_ACCEL_LOCKED);
      menuitem  gtk_check_menu_item_new_with_label ("Accelerators Frozen");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
      gtk_widget_add_accelerator (menuitem,
      "activate",
      accel_group,
      GDK_F2,
      0,
      GTK_ACCEL_VISIBLE);
      gtk_widget_add_accelerator (menuitem,
      "activate",
      accel_group,
      GDK_F3,
      0,
      GTK_ACCEL_VISIBLE);

      optionmenu  gtk_option_menu_new ();
      gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu), menu);
      gtk_option_menu_set_history (GTK_OPTION_MENU (optionmenu), 3);
      gtk_box_pack_start (GTK_BOX (box2), optionmenu, TRUE, TRUE, 0);
      gtk_widget_show (optionmenu);

      separator  gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);
      gtk_widget_show (separator);

///////////
      size_group  gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

      /* Create one frame holding color options */
      frame  gtk_frame_new ("Color Options");
      gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

      table  gtk_table_new (2, 2, FALSE);
      gtk_container_set_border_width (GTK_CONTAINER (table), 5);
      gtk_table_set_row_spacings (GTK_TABLE (table), 5);
      gtk_table_set_col_spacings (GTK_TABLE (table), 10);
      gtk_container_add (GTK_CONTAINER (frame), table);

      add_row (GTK_TABLE (table), 0, size_group, "_Foreground", color_options);
      add_row (GTK_TABLE (table), 1, size_group, "_Background", color_options);

      /* And another frame holding line style options */
      frame  gtk_frame_new ("Line Options");
      gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

      table  gtk_table_new (2, 2, FALSE);
      gtk_container_set_border_width (GTK_CONTAINER (table), 5);
      gtk_table_set_row_spacings (GTK_TABLE (table), 5);
      gtk_table_set_col_spacings (GTK_TABLE (table), 10);
      gtk_container_add (GTK_CONTAINER (frame), table);

      add_row (GTK_TABLE (table), 0, size_group, "_Dashing", dash_options);
      add_row (GTK_TABLE (table), 1, size_group, "_Line ends", end_options);

      /* And a check button to turn grouping on and off */
      check_button  gtk_check_button_new_with_mnemonic ("_Enable grouping");
      gtk_box_pack_start (GTK_BOX (vbox), check_button, FALSE, FALSE, 0);

      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button), TRUE);
      g_signal_connect (check_button, "toggled",
   G_CALLBACK (toggle_grouping), size_group);
      gtk_window_set_default_size (GTK_WINDOW (window), 240, 220);

    gtk_widget_show_all (window);
    gtk_main();

    return (vt);

}
