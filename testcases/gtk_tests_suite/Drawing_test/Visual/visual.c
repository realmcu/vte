/*OC* visual.c **/
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

gint vtFALSE;
gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
static GtkItemFactoryEntry menu_items[] 
{
  { "/_File",            NULL,         0,                     0, "<Branch>" },
  { "/File/_Quit-Pass", "<control>Q",  eventDestroyQuit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", eventDestroyExit,       0, "<StockItem>", GTK_STOCK_QUIT },

  { "/_Help",            NULL,         0,                     0, "<Branch>" }

};


int visual_main(int argc,char *argv[])
{
    GtkWidget *app;
    GtkWidget *label;
    GdkVisual *visual;
    GtkWidget *table;
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;

    gchar work[80];
    gchar text[4096];
    gtk_init(&argc,&argv);
    app  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (app), "Creating Colors");
    gtk_window_set_default_size (GTK_WINDOW (app),240,320);

    gtk_signal_connect(GTK_OBJECT(app),"delete_event",
            GTK_SIGNAL_FUNC(eventDelete),NULL);
    gtk_signal_connect(GTK_OBJECT(app),"destroy",
            GTK_SIGNAL_FUNC(eventDestroyQuit),NULL);

    table  gtk_table_new (2, 1, FALSE);
    gtk_widget_show(table);

    accel_group  gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (app), accel_group);
    g_object_unref (accel_group);
    item_factory  gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

      /* Set up item factory to go away with the window */
    g_object_ref (item_factory);
    gtk_object_sink (GTK_OBJECT (item_factory));
    g_object_set_data_full (G_OBJECT (app),
                            "<main>",
                            item_factory,
                         (GDestroyNotify) g_object_unref);

      /* create menu items */
    gtk_item_factory_create_items (item_factory, G_N_ELEMENTS (menu_items),
                                   menu_items, app);

    gtk_table_attach (GTK_TABLE (table),
                      gtk_item_factory_get_widget (item_factory, "<main>"),
                        /* X direction */          /* Y direction */
                      0, 1,                      0, 1,
                      GTK_EXPAND | GTK_FILL,     0,
                      0,                         0);

    visual  gdk_visual_get_system();

    sprintf(text,"Type: ");
    switch(visual->type) {
    case GDK_VISUAL_STATIC_GRAY:
        strcat(text,"Static Gray\n");
        break;
    case GDK_VISUAL_GRAYSCALE:
        strcat(text,"Grayscale\n");
        break;
    case GDK_VISUAL_STATIC_COLOR:
        strcat(text,"Static Color\n");
        break;
    case GDK_VISUAL_PSEUDO_COLOR:
        strcat(text,"Pseudo Color\n");
        break;
    case GDK_VISUAL_TRUE_COLOR:
        strcat(text,"True Color\n");
        break;
    case GDK_VISUAL_DIRECT_COLOR:
        strcat(text,"Direct Color\n");
        break;
    }
    sprintf(work,"Depth (number of bits per pixel): %d\n",
            visual->depth);
    strcat(text,work);
    strcat(text,"Byte order: ");
    switch(visual->byte_order) {
    case GDK_LSB_FIRST:
        strcat(text,"LSB first\n");
        break;
    case GDK_MSB_FIRST:
        strcat(text,"MSB first\n");
        break;
    }
    sprintf(work,"\nSize of colormap array: %d\n",
            visual->colormap_size);
    strcat(text,work);
    sprintf(work,"Colormap bits per RGB: %d\n",
            visual->bits_per_rgb);
    strcat(text,work);

    strcat(text,
            "\nColor       Mask         Shift    Bits\n");
    sprintf(work,"   red   0x%08X    %2d      %2d\n",
            visual->red_mask,visual->red_shift,
            visual->red_prec);
    strcat(text,work);
    sprintf(work,"green   0x%08X    %2d      %2d\n",
            visual->green_mask,visual->green_shift,
            visual->green_prec);
    strcat(text,work);
    sprintf(work,"  blue   0x%08X    %2d      %2d\n",
            visual->blue_mask,visual->blue_shift,
            visual->blue_prec);
    strcat(text,work);

    label  gtk_label_new(text);
    gtk_label_set_justify(GTK_LABEL(label),
            GTK_JUSTIFY_LEFT);

   gtk_misc_set_padding(GTK_MISC(label),0,0);

   gtk_table_attach (GTK_TABLE (table),
                      label,
                       /* X direction */          /* Y direction */
                     0, 1,                      1, 2,
                      GTK_EXPAND | GTK_FILL,     0,
                      0,                         0);

    gtk_container_add(GTK_CONTAINER(app),table);

    gtk_widget_show_all(app);
    gtk_main();
    return (vt);
}
gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data) {
    return(FALSE);
}
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data) {

    vtFALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();
    return(0);
}
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data) {

    vtTRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit ();
    return(1);
}
