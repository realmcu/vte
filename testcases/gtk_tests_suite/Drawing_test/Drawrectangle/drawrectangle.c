/** flasher.c **/
#include <gtk/gtk.h>
#include <stdlib.h>

gint vt=FALSE;
gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data);
gint drawRandomRectangle(gpointer data);

#define WIDTH 240
#define HEIGHT 320
#define INTERVAL 30

static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",            NULL,         0,                     0, "<Branch>" },
  { "/File/_Quit-Pass", "<control>Q",  eventDestroyQuit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", eventDestroyExit,       0, "<StockItem>", GTK_STOCK_QUIT },

  { "/_Help",            NULL,         0,                     0, "<Branch>" }
};

int drawrectangle_main(int argc,char *argv[])
{
    GtkWidget *app;
    GtkWidget *area;
    GtkWidget *table;
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;

    gtk_init(&argc,&argv);
    
    app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (app), "Creating Colors");
    gtk_window_set_default_size (GTK_WINDOW (app),240,320);
           
    gtk_signal_connect(GTK_OBJECT(app),"delete_event",
            GTK_SIGNAL_FUNC(eventDelete),NULL);
    gtk_signal_connect(GTK_OBJECT(app),"destroy",
            GTK_SIGNAL_FUNC(eventDestroyQuit),NULL);


    table = gtk_table_new (2, 1, FALSE);
    gtk_widget_show(table);

   // gtk_container_set_border_width(GTK_CONTAINER(app),20);

      /* Create the menubar*/

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (app), accel_group);
    g_object_unref (accel_group);
    item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

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


    area = gtk_drawing_area_new();
    gtk_widget_set_usize(area,WIDTH,HEIGHT);
    gtk_timeout_add(INTERVAL,drawRandomRectangle,area);

    gtk_table_attach (GTK_TABLE (table),
                      area,
                        /* X direction */          /* Y direction */
                      0, 1,                      1, 2,
                      GTK_EXPAND | GTK_FILL,     0,
                      0,                         0);

   // gnome_app_set_contents(GNOME_APP(app),area);
    gtk_container_add(GTK_CONTAINER(app),table);
    
    gtk_widget_show_all(app);
    gtk_main();
    return(vt);
}
gint drawRandomRectangle(gpointer data)
{
    static GdkColormap *colormap = NULL;
    static GdkGC *gc = NULL;
    GdkColor color;
    GtkWidget *widget = GTK_WIDGET(data);
    gint x = 20;
    gint y = 20;
    gint width = 40;
    gint height = 40;

    if(colormap == NULL) {
        colormap = gdk_colormap_get_system();
        gc = gdk_gc_new(widget->window);
    }

    color.red += (((double)rand()*0xFFFF)/RAND_MAX);
    color.green += (((double)rand()*0xFFFF)/RAND_MAX);
    color.blue += (((double)rand()*0xFFFF)/RAND_MAX);
    gdk_color_alloc(colormap,&color);

    gdk_gc_set_foreground(gc,&color);

    x = (((double)rand()*WIDTH)/RAND_MAX) - (WIDTH/4);
    y = (((double)rand()*HEIGHT)/RAND_MAX) - (HEIGHT/4);
    width = (((double)rand()*(WIDTH/2))/RAND_MAX);
    height = (((double)rand()*(HEIGHT/2))/RAND_MAX);
    
    gdk_draw_rectangle(widget->window,
            gc,
            TRUE,
            x,y,
            width,height);
    return(TRUE);
}

gint eventDelete(GtkWidget *widget,
        GdkEvent *event,gpointer data) {
    return(FALSE);
}
gint eventDestroyQuit(GtkWidget *widget,
        GdkEvent *event,gpointer data) {

    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();
    return(0);
}
gint eventDestroyExit(GtkWidget *widget,
        GdkEvent *event,gpointer data) {

    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit ();
    return(1);
}
