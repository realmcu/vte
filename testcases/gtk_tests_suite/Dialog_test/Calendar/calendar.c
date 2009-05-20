/*
 * Copyright (C) 1998 Cesar Miquel, Shawn T. Amundson, Mattias Grönlund
 * Copyright (C) 2000 Tony Gale
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/**/
/**
    @file   calendar.c
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
#include <string.h>
#include <gtk/gtk.h>

#define DEF_PAD 10
#define DEF_PAD_SMALL 5

#define TM_YEAR_BASE 1900
gint vtFALSE;
typedef struct _CalendarData {
  GtkWidget *flag_checkboxes[5];
  gboolean  settings[5];
  GtkWidget *font_dialog;
  GtkWidget *window;
  GtkWidget *prev2_sig;
  GtkWidget *prev_sig;
  GtkWidget *last_sig;
  GtkWidget *month;
} CalendarData;

enum {
  calendar_show_header,
  calendar_show_days,
  calendar_month_change,
  calendar_show_week,
  calendar_monday_first
};

/*
 * GtkCalendar
 */

void calendar_date_to_string (CalendarData *data,
         char         *buffer,
         gint          buff_len)
{
  GDate *date;
  guint year, month, day;

  gtk_calendar_get_date (GTK_CALENDAR (data->window),
    &year, &month, &day);
  date  g_date_new_dmy (day, month + 1, year);
  g_date_strftime (buffer, buff_len - 1, "%x", date);

  g_date_free (date);
}

void calendar_set_signal_strings (char         *sig_str,
      CalendarData *data)
{
  const gchar *prev_sig;

  prev_sig  gtk_label_get_text (GTK_LABEL (data->prev_sig));
  gtk_label_set_text (GTK_LABEL (data->prev2_sig), prev_sig);

  prev_sig  gtk_label_get_text (GTK_LABEL (data->last_sig));
  gtk_label_set_text (GTK_LABEL (data->prev_sig), prev_sig);
  gtk_label_set_text (GTK_LABEL (data->last_sig), sig_str);
}

void calendar_month_changed (GtkWidget    *widget,
                             CalendarData *data)
{
  char buffer[256]  "month_changed: ";

  calendar_date_to_string (data, buffer + 15, 256 - 15);
  calendar_set_signal_strings (buffer, data);
}

void calendar_day_selected (GtkWidget    *widget,
                            CalendarData *data)
{
  char buffer[256]  "day_selected: ";

  calendar_date_to_string (data, buffer + 14, 256 - 14);
  calendar_set_signal_strings (buffer, data);
}

void calendar_day_selected_double_click (GtkWidget    *widget,
                                         CalendarData *data)
{
  char buffer[256]  "day_selected_double_click: ";
  guint day;

  calendar_date_to_string (data, buffer + 27, 256 - 27);
  calendar_set_signal_strings (buffer, data);

  gtk_calendar_get_date (GTK_CALENDAR (data->window),
    NULL, NULL, &day);

  if (GTK_CALENDAR (data->window)->marked_date[day-1]  0) {
    gtk_calendar_mark_day (GTK_CALENDAR (data->window), day);
  } else {
    gtk_calendar_unmark_day (GTK_CALENDAR (data->window), day);
  }
}

void calendar_prev_month (GtkWidget    *widget,
                          CalendarData *data)
{
  char buffer[256]  "prev_month: ";

  calendar_date_to_string (data, buffer + 12, 256 - 12);
  calendar_set_signal_strings (buffer, data);
}

void calendar_next_month (GtkWidget    *widget,
                          CalendarData *data)
{
  char buffer[256]  "next_month: ";

  calendar_date_to_string (data, buffer + 12, 256 - 12);
  calendar_set_signal_strings (buffer, data);
}

void calendar_prev_year (GtkWidget    *widget,
                         CalendarData *data)
{
  char buffer[256]  "prev_year: ";

  calendar_date_to_string (data, buffer + 11, 256 - 11);
  calendar_set_signal_strings (buffer, data);
}

void calendar_next_year (GtkWidget    *widget,
                         CalendarData *data)
{
  char buffer[256]  "next_year: ";

  calendar_date_to_string (data, buffer + 11, 256 - 11);
  calendar_set_signal_strings (buffer, data);
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

void calendar_set_flags (CalendarData *calendar)
{
  gint i;
  gint options  0;
  for (i  0;i < 5; i++)
    if (calendar->settings[i])
      {
 options  options + (1 << i);
      }
  if (calendar->window)
    gtk_calendar_display_options (GTK_CALENDAR (calendar->window), options);
}

void calendar_toggle_flag (GtkWidget    *toggle,
                           CalendarData *calendar)
{
  gint i;
  gint j;
  j  0;
  for (i  0; i < 5; i++)
    if (calendar->flag_checkboxes[i]  toggle)
      j  i;

  calendar->settings[j]  !calendar->settings[j];
  calendar_set_flags (calendar);

}

void calendar_font_selection_ok (GtkWidget    *button,
                                 CalendarData *calendar)
{
  GtkRcStyle *style;
  char *font_name;

  if (calendar->window)
    {
      font_name  gtk_font_selection_dialog_get_font_name (GTK_FONT_SELECTION_DIALOG (calendar->font_dialog));
      if (font_name)
 {
   style  gtk_rc_style_new ();
   pango_font_description_free (style->font_desc);
   style->font_desc  pango_font_description_from_string (font_name);
   gtk_widget_modify_style (calendar->window, style);
   g_free (font_name);
 }
    }

  gtk_widget_destroy (calendar->font_dialog);
}

void calendar_select_font (GtkWidget    *button,
                           CalendarData *calendar)
{
  GtkWidget *window;

  if (!calendar->font_dialog) {
    window  gtk_font_selection_dialog_new ("Font Selection Dialog");
    g_return_if_fail (GTK_IS_FONT_SELECTION_DIALOG (window));
    calendar->font_dialog  window;
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
    g_signal_connect (window, "destroy",
        G_CALLBACK (gtk_widget_destroyed),
        &calendar->font_dialog);

    g_signal_connect (GTK_FONT_SELECTION_DIALOG (window)->ok_button,
        "clicked", G_CALLBACK (calendar_font_selection_ok),
        calendar);
    g_signal_connect_swapped (GTK_FONT_SELECTION_DIALOG (window)->cancel_button,
        "clicked", G_CALLBACK (gtk_widget_destroy),
        calendar->font_dialog);
   gtk_window_set_default_size (GTK_WINDOW (window), 240, 320);
   gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);
  }
  window  calendar->font_dialog;
  if (!GTK_WIDGET_VISIBLE (window))
    gtk_widget_show (window);
  else
    gtk_widget_destroy (window);

}

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
  { "/_Quit-Pass", "<control>Q", destroy_Quit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Exit-Fail", "<control>E", destroy_Exit,       0, "<StockItem>", GTK_STOCK_QUIT }
};

void create_calendar ()
{
  GtkWidget *window;
  GtkWidget *vbox, *vbox2, *vbox3,*box3;
  GtkWidget *hbox;
  GtkWidget *calendar;
  GtkWidget *toggle;
  GtkWidget *frame;
  GtkWidget *label;
  GtkWidget *table;
  GtkWidget *button;

  GtkItemFactory *item_factory;
  static CalendarData calendar_data;
  GtkWidget *menu;
  gint i;

  struct {
    char *label;
  } flags[] 
    {
      { "Show Heading" },
      { "Show Day Names" },
      { "No Month Change" },
      { "Show Week Numbers" },
      { "Week Start Monday" }
    };


  calendar_data.window  NULL;
  calendar_data.font_dialog  NULL;

  for (i  0; i < 5; i++) {
    calendar_data.settings[i]  0;
  }

  window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GtkCalendar Example");
  gtk_container_set_border_width (GTK_CONTAINER (window),0);
  g_signal_connect (window, "destroy",G_CALLBACK (gtk_main_quit),NULL);
  g_signal_connect (window, "delete-event", G_CALLBACK (gtk_false),
      NULL);
   gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
   gtk_window_set_default_size (GTK_WINDOW (window), 240, 320);
   gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);

   vbox  gtk_vbox_new (FALSE, DEF_PAD);
   gtk_container_add (GTK_CONTAINER (window), vbox);


  /* Build the right font-button */
    item_factory  gtk_item_factory_new (GTK_TYPE_MENU, "<main>",
                                        NULL);
    gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
    menu  gtk_item_factory_get_widget(item_factory, "<main>");

    gtk_widget_show (menu);
    g_signal_connect_swapped (G_OBJECT (window), "button_press_event",G_CALLBACK (button_press),
                              G_OBJECT (menu));

    gtk_widget_set_events (window, gtk_widget_get_events (window)
                             | GDK_BUTTON_PRESS_MASK);
  /*
   * The top part of the window, Calendar, flags and fontsel.
   */
  table  gtk_table_new (2, 3, FALSE);
  /* Calendar widget */
  frame  gtk_frame_new ("Calendar");
  gtk_container_add (GTK_CONTAINER (vbox), table);
  gtk_box_pack_start(GTK_BOX (table), frame, FALSE, TRUE, DEF_PAD);
  calendargtk_calendar_new ();
  calendar_data.window  calendar;
  calendar_set_flags (&calendar_data);
  gtk_calendar_mark_day (GTK_CALENDAR (calendar), 19);
    gtk_table_attach_defaults (GTK_TABLE (table), calendar,0, 1, 0, 1);
  g_signal_connect (calendar, "month_changed",
      G_CALLBACK (calendar_month_changed),
      &calendar_data);
  g_signal_connect (calendar, "day_selected",
      G_CALLBACK (calendar_day_selected),
      &calendar_data);
  g_signal_connect (calendar, "day_selected_double_click",
      G_CALLBACK (calendar_day_selected_double_click),
      &calendar_data);
  g_signal_connect (calendar, "prev_month",
      G_CALLBACK (calendar_prev_month),
      &calendar_data);
  g_signal_connect (calendar, "next_month",
      G_CALLBACK (calendar_next_month),
      &calendar_data);
  g_signal_connect (calendar, "prev_year",
      G_CALLBACK (calendar_prev_year),
      &calendar_data);
  g_signal_connect (calendar, "next_year",
      G_CALLBACK (calendar_next_year),
      &calendar_data);

  vbox2  gtk_vbox_new (FALSE, DEF_PAD);
    gtk_table_attach_defaults (GTK_TABLE (table), vbox2,0, 1, 1,2);


  /* Build the Right frame with the flags in */

  frame  gtk_frame_new ("Flags");
  gtk_box_pack_start (GTK_BOX (vbox2), frame, TRUE, TRUE, DEF_PAD);
  vbox3  gtk_vbox_new (TRUE, DEF_PAD_SMALL);
  gtk_container_add (GTK_CONTAINER (frame), vbox3);

  for (i  0; i < 5; i++)
    {
      toggle  gtk_check_button_new_with_label (flags[i].label);
      g_signal_connect (toggle,
   "toggled",
   G_CALLBACK (calendar_toggle_flag),
   &calendar_data);
      gtk_box_pack_start (GTK_BOX (vbox3), toggle,FALSE,FALSE, 0);
      calendar_data.flag_checkboxes[i]  toggle;
    }
  /*
   *  Build the Signal-event part.
   */

  frame  gtk_frame_new ("Signal events");
  gtk_table_attach_defaults (GTK_TABLE (table), frame,1,2, 0,1);

  vbox2  gtk_vbox_new (TRUE, DEF_PAD_SMALL);
  gtk_container_add (GTK_CONTAINER (frame), vbox2);

  hbox  gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 0);
  label  gtk_label_new ("Signal:");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
  calendar_data.last_sig  gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (hbox), calendar_data.last_sig, FALSE, TRUE, 0);

  hbox  gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 0);
  label  gtk_label_new ("Previous signal:");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
  calendar_data.prev_sig  gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (hbox), calendar_data.prev_sig, FALSE, TRUE, 0);

  hbox  gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 0);
  label  gtk_label_new ("Second previous signal:");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
  calendar_data.prev2_sig  gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (hbox), calendar_data.prev2_sig, FALSE, TRUE, 0);

  frame  gtk_frame_new ("Select Font");
  gtk_table_attach_defaults (GTK_TABLE (table), frame,1,2, 1,2);
  gtk_container_set_border_width (GTK_CONTAINER (frame),10);
  vbox2  gtk_vbox_new (TRUE, DEF_PAD_SMALL);
  gtk_container_add (GTK_CONTAINER (frame), vbox2);
  hbox  gtk_hbox_new(FALSE,25);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 0);
  box3  gtk_vbox_new(FALSE,15);
  gtk_box_pack_start (GTK_BOX (hbox), box3, FALSE, TRUE, 0);
  buttongtk_button_new_with_label("Font");
  gtk_container_set_border_width (GTK_CONTAINER (hbox),5);
  gtk_box_pack_start (GTK_BOX (box3), button, FALSE, TRUE, 0);
  gtk_widget_set_usize(button,70,30);
  g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (calendar_select_font),
                                         &calendar_data);

  gtk_widget_show_all (window);
}


int calendar_main (int   argc,
          char *argv[])
{
  gtk_init (&argc, &argv);

  create_calendar ();
  gtk_main ();
  return (vt);
}
