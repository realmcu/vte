/*====//============================================================================================*/
/**
    @file   drawingarea.c
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
   Inkina Irina               10/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/



/* Drawing Area
 *
 * GtkDrawingArea is a blank area where you can draw custom displays
 * of various kinds.
 *
 * This demo has two drawing areas. The checkerboard area shows
 * how you can just draw something; all you have to do is write
 * a signal handler for expose_event, as shown here.
 *
 * The "scribble" area is a bit more advanced, and shows how to handle
 * events such as button presses and mouse motion. Click the mouse
 * and drag in the scribble area to draw squiggles. Resize the window
 * to clear the area.
 */

#include <gtk/gtk.h>
gint vt=FALSE;
static GtkWidget *window;
/* Pixmap for scribble area, to store current scribbles */
static GdkPixmap *pixmap = NULL;

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
  { "/File/_Quit - Pass","<control>Q", destroy_Quit,    0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit - Fail","<control>E", destroy_Exit,    0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Help",		 NULL,	       0,		      0, "<Branch>" },
  { "/Help/_About",	  "<control>H",	0,	      0 },
};



/* Create a new pixmap of the appropriate size to store our scribbles */
static gboolean
scribble_configure_event (GtkWidget	    *widget,
			  GdkEventConfigure *event,
			  gpointer	     data)
{
  if (pixmap)
    g_object_unref (pixmap);

  pixmap = gdk_pixmap_new (widget->window,
			   widget->allocation.width,
			   widget->allocation.height,
			   -1);

  /* Initialize the pixmap to white */
  gdk_draw_rectangle (pixmap,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);

  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

/* Redraw the screen from the pixmap */
static gboolean
scribble_expose_event (GtkWidget      *widget,
		       GdkEventExpose *event,
		       gpointer	       data)
{
  /* We use the "foreground GC" for the widget since it already exists,
   * but honestly any GC would work. The only thing to worry about
   * is whether the GC has an inappropriate clip region set.
   */
  
  gdk_draw_drawable (widget->window,
		     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		     pixmap,
		     /* Only copy the area that was exposed. */
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  
  return FALSE;
}

/* Draw a rectangle on the screen */
static void
draw_brush (GtkWidget *widget,
	    gdouble    x,
	    gdouble    y)
{
  GdkRectangle update_rect;

  update_rect.x = x - 3;
  update_rect.y = y - 3;
  update_rect.width = 6;
  update_rect.height = 6;

  /* Paint to the pixmap, where we store our state */
  gdk_draw_rectangle (pixmap,
		      widget->style->black_gc,
		      TRUE,
		      update_rect.x, update_rect.y,
		      update_rect.width, update_rect.height);

  /* Now invalidate the affected region of the drawing area. */
  gdk_window_invalidate_rect (widget->window,
			      &update_rect,
			      FALSE);
}

static gboolean
scribble_button_press_event (GtkWidget	    *widget,
			     GdkEventButton *event,
			     gpointer	     data)
{

  if (pixmap == NULL)
    return FALSE; /* paranoia check, in case we haven't gotten a configure event */
  
  if (event->button == 1){
    draw_brush (widget, event->x, event->y);
g_print("x=%f\n",event->x);}
  /* We've handled the event, stop processing */
  return TRUE;
}

static gboolean
scribble_motion_notify_event (GtkWidget	     *widget,
			      GdkEventMotion *event,
			      gpointer	      data)
{
  int x, y;
  GdkModifierType state;

  if (pixmap == NULL)
    return FALSE; /* paranoia check, in case we haven't gotten a configure event */

  /* This call is very important; it requests the next motion event.
   * If you don't call gdk_window_get_pointer() you'll only get
   * a single motion event. The reason is that we specified
   * GDK_POINTER_MOTION_HINT_MASK to gtk_widget_set_events().
   * If we hadn't specified that, we could just use event->x, event->y
   * as the pointer location. But we'd also get deluged in events.
   * By requesting the next event as we handle the current one,
   * we avoid getting a huge number of events faster than we
   * can cope.
   */
  
  gdk_window_get_pointer (event->window, &x, &y, &state);
    
  if (state & GDK_BUTTON1_MASK)
    draw_brush (widget, x, y);

  /* We've handled it, stop processing */
  return TRUE;
}


static gboolean
checkerboard_expose (GtkWidget	    *da,
		     GdkEventExpose *event,
		     gpointer	     data)
{
  gint i, j, xcount, ycount;
  GdkGC *gc1, *gc2;
  GdkColor color;
  
#define CHECK_SIZE 10
#define SPACING 2
  
  /* At the start of an expose handler, a clip region of event->area
   * is set on the window, and event->area has been cleared to the
   * widget's background color. The docs for
   * gdk_window_begin_paint_region() give more details on how this
   * works.
   */

  /* It would be a bit more efficient to keep these
   * GC's around instead of recreating on each expose, but
   * this is the lazy/slow way.
   */
  gc1 = gdk_gc_new (da->window);
  color.red = 30000;
  color.green = 0;
  color.blue = 30000;
  gdk_gc_set_rgb_fg_color (gc1, &color);

  gc2 = gdk_gc_new (da->window);
  color.red = 65535;
  color.green = 65535;
  color.blue = 65535;
  gdk_gc_set_rgb_fg_color (gc2, &color);
  
  xcount = 0;
  i = SPACING;
  while (i < da->allocation.width)
    {
      j = SPACING;
      ycount = xcount % 2; /* start with even/odd depending on row */
      while (j < da->allocation.height)
	{
	  GdkGC *gc;
	  
	  if (ycount % 2)
	    gc = gc1;
	  else
	    gc = gc2;

	  /* If we're outside event->area, this will do nothing.
	   * It might be mildly more efficient if we handled
	   * the clipping ourselves, but again we're feeling lazy.
	   */
	  gdk_draw_rectangle (da->window,
			      gc,
			      TRUE,
			      i, j,
			      CHECK_SIZE,
			      CHECK_SIZE);

	  j += CHECK_SIZE + SPACING;
	  ++ycount;
	}

      i += CHECK_SIZE + SPACING;
      ++xcount;
    }
  
  g_object_unref (gc1);
  g_object_unref (gc2);
  
  /* return TRUE because we've handled this event, so no
   * further processing is required.
   */
  return TRUE;
}

//GtkWidget *do_drawingarea (void)
int drawingarea_main(int argc, char *argv[])

{
  GtkWidget *frame;
  GtkWidget *vbox,*box1;
  GtkWidget *da;
  GtkWidget *label;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;

       gtk_init (&argc,&argv);
  

      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title (GTK_WINDOW (window), "Drawing Area");
      gtk_window_set_default_size (GTK_WINDOW (window),240,220);

      g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), &window);

      accel_group = gtk_accel_group_new ();
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
      gtk_container_set_border_width (GTK_CONTAINER (window), 0);//8

      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1 = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
			  FALSE, FALSE, 0);
 
      separator = gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);
      

      vbox = gtk_vbox_new (FALSE, 8);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
 //     gtk_container_add (GTK_CONTAINER (window), vbox);
      gtk_container_add (GTK_CONTAINER (box1), vbox);

      /*
       * Create the checkerboard area
       */
      
      label = gtk_label_new (NULL);
      gtk_label_set_markup (GTK_LABEL (label),
			    "<u>Checkerboard pattern</u>");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      
      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
      
      da = gtk_drawing_area_new ();
      /* set a minimum size */
      gtk_widget_set_size_request (da, 100, 100);

      gtk_container_add (GTK_CONTAINER (frame), da);

      g_signal_connect (da, "expose_event",
			G_CALLBACK (checkerboard_expose), NULL);

      /* * Create the scribble area     */
     
      label = gtk_label_new (NULL);
      gtk_label_set_markup (GTK_LABEL (label),
			    "<u>Scribble area</u>");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      
      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
      
      da = gtk_drawing_area_new ();
      /* set a minimum size */
      gtk_widget_set_size_request (da, 100, 100);

      gtk_container_add (GTK_CONTAINER (frame), da);

      /* Signals used to handle backing pixmap */
      
      g_signal_connect (da, "expose_event",
			G_CALLBACK (scribble_expose_event), NULL);
      g_signal_connect (da,"configure_event",
			G_CALLBACK (scribble_configure_event), NULL);
      
      /* Event signals */
      
      g_signal_connect (da, "motion_notify_event",
			G_CALLBACK (scribble_motion_notify_event), NULL);
      g_signal_connect (da, "button_press_event",
			G_CALLBACK (scribble_button_press_event), NULL);


      /* Ask to receive events the drawing area doesn't normally
       * subscribe to
       */
      gtk_widget_set_events (da, gtk_widget_get_events (da)
			     | GDK_LEAVE_NOTIFY_MASK
			     | GDK_BUTTON_PRESS_MASK
			     | GDK_POINTER_MOTION_MASK
			     | GDK_POINTER_MOTION_HINT_MASK);
  

    gtk_widget_show_all (window);
    gtk_main();
  return (vt);

  
}
