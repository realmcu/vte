/*================================================================================================*/
/**^M
    @file   images.c^M
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


/* Images
 *
 * GtkImage is used to display an image; the image can be in a number of formats.
 * Typically, you load an image into a GdkPixbuf, then display the pixbuf.
 *
 * This demo code shows some of the more obscure cases, in the simple
 * case a call to gtk_image_new_from_file() is all you need.
 *
 * If you want to put image data in your program as a C variable,
 * use the make-inline-pixbuf program that comes with GTK+.
 * This way you won't need to depend on loading external files, your
 * application binary can be self-contained.
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>

static GtkWidget *window;
static GdkPixbufLoader *pixbuf_loader;
static guint load_timeout = 0;
static FILE* image_stream = NULL;
gint vt=FALSE;

  void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit();

}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit();
}

static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",		 NULL,	       0,		      0, "<Branch>" },
  { "/File/sep1",	 NULL,	       0,	      0, "<Separator>" },
  { "/File/_Quit - Pass","<control>Q", destroy_Quit,	      0 },
  { "/File/_Exit - Fail", "<control>E", destroy_Exit,	      0 },
  { "/_Help",		 NULL,	       0,		      0, "<Branch>" },
  { "/Help/_About", "<control>H",	0,	      0 }
};


char *demo_find_file (char *filename)
{
    GError *error;
    gchar *filename_1;
    GDir *dir;
    gchar *path;

    path=get_current_dir_name();
    dir=g_dir_open (path,0,&error);
     while(filename_1)
      {
       filename_1= g_dir_read_name(dir);
       if(filename_1)
         if(strcmp(filename_1,filename)==0) {return filename;}
      }

       GtkWidget *dialog;
         dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                           GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_ERROR,
                                           GTK_BUTTONS_CLOSE,
                                           "file not found : %s",
                                           filename);
         g_signal_connect (dialog, "response",
                            G_CALLBACK (gtk_widget_destroy), NULL);
         gtk_widget_show (dialog);
 return NULL;
}

static void
progressive_prepared_callback (GdkPixbufLoader *loader,
			       gpointer		data)
{
  GdkPixbuf *pixbuf;
  GtkWidget *image;

  image = GTK_WIDGET (data);
  
  pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);

  /* Avoid displaying random memory contents, since the pixbuf
   * isn't filled in yet.
   */
  gdk_pixbuf_fill (pixbuf, 0xaaaaaaff);
  
  gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);
}

static void
progressive_updated_callback (GdkPixbufLoader *loader,
                              gint		   x,
                              gint		   y,
                              gint		   width,
                              gint		   height,
                              gpointer	   data)
{
  GtkWidget *image;
  image = GTK_WIDGET (data);

  /* We know the pixbuf inside the GtkImage has changed, but the image
   * itself doesn't know this; so queue a redraw.  If we wanted to be
   * really efficient, we could use a drawing area or something
   * instead of a GtkImage, so we could control the exact position of
   * the pixbuf on the display, then we could queue a draw for only
   * the updated area of the image.
   */
  
  gtk_widget_queue_draw (image);
}

static gint progressive_timeout (gpointer data)
{
  GtkWidget *image;

  image = GTK_WIDGET (data);
  
  /* This shows off fully-paranoid error handling, so looks scary.
   * You could factor out the error handling code into a nice separate
   * function to make things nicer.
   */
  
  if (image_stream)
    {
      size_t bytes_read;
      guchar buf[256];
      GError *error = NULL;
      
      bytes_read = fread (buf, 1, 256, image_stream);

      if (ferror (image_stream))
	{
	  GtkWidget *dialog;
	  
	  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_MESSAGE_ERROR,
					   GTK_BUTTONS_CLOSE,
					   "Failure reading image file 'alpha.png': %s",
					   g_strerror (errno));

	  g_signal_connect (dialog, "response",G_CALLBACK (gtk_widget_destroy), NULL);

	  fclose (image_stream);
	  image_stream = NULL;

	  gtk_widget_show (dialog);
	  
	  load_timeout = 0;

	  return FALSE; /* uninstall the timeout */
	}

      if (!gdk_pixbuf_loader_write (pixbuf_loader,
				    buf, bytes_read,
				    &error))
	{
	  GtkWidget *dialog;
	  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_MESSAGE_ERROR,
					   GTK_BUTTONS_CLOSE,
					   "Failed to load image: %s",
					   error->message);

	  g_error_free (error);
	  
	  g_signal_connect (dialog, "response",
			    G_CALLBACK (gtk_widget_destroy), NULL);

	  fclose (image_stream);
	  image_stream = NULL;
	  
	  gtk_widget_show (dialog);

	  load_timeout = 0;

	  return FALSE; /* uninstall the timeout */
	}  

      if (feof (image_stream))
	{
	  fclose (image_stream);
	  image_stream = NULL;

	  /* Errors can happen on close, e.g. if the image
	   * file was truncated we'll know on close that
	   * it was incomplete.
	   */
	  error = NULL;
	  if (!gdk_pixbuf_loader_close (pixbuf_loader,
					&error))
	    {
	      GtkWidget *dialog;
	      
	      dialog = gtk_message_dialog_new (GTK_WINDOW (window),
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       GTK_MESSAGE_ERROR,
					       GTK_BUTTONS_CLOSE,
					       "Failed to load image: %s",
					       error->message);
	      
	      g_error_free (error);
	      
	      g_signal_connect (dialog, "response",
				G_CALLBACK (gtk_widget_destroy), NULL);
	      
	      gtk_widget_show (dialog);

	      g_object_unref (pixbuf_loader);
	      pixbuf_loader = NULL;
	      
	      load_timeout = 0;
	      
	      return FALSE; /* uninstall the timeout */
	    }
	  
	  g_object_unref (pixbuf_loader);
	  pixbuf_loader = NULL;
	}
    }
  else
    {
      gchar *filename;
      gchar *error_message = NULL;

      filename =demo_find_file("alpha.png");
          if(filename)
	   {
	     image_stream = fopen (filename, "r");
	     if (!image_stream)
	     error_message = g_strdup_printf ("Unable to open image file 'alpha.png': %s",
					     g_strerror (errno));
            }    

      if (image_stream == NULL)
	{
	  GtkWidget *dialog;
	  
	  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_MESSAGE_ERROR,
					   GTK_BUTTONS_CLOSE,
					   "%s", error_message);
	  g_free (error_message);

	  g_signal_connect (dialog, "response",
			    G_CALLBACK (gtk_widget_destroy), NULL);
	  
	  gtk_widget_show (dialog);

	  load_timeout = 0;

	  return FALSE; /* uninstall the timeout */
	}

      if (pixbuf_loader)
	{ 
	  gdk_pixbuf_loader_close (pixbuf_loader, NULL);
	  g_object_unref (pixbuf_loader);
	  pixbuf_loader = NULL;
	}
      
    pixbuf_loader = gdk_pixbuf_loader_new ();
      
      g_signal_connect (pixbuf_loader, "area_prepared",
			G_CALLBACK (progressive_prepared_callback), image);
      
      g_signal_connect (pixbuf_loader, "area_updated",
			G_CALLBACK (progressive_updated_callback), image);
    }

  /* leave timeout installed */
  return TRUE;
}

static void
start_progressive_loading (GtkWidget *image)
{
  /* This is obviously totally contrived (we slow down loading
   * on purpose to show how incremental loading works).
   * The real purpose of incremental loading is the case where
   * you are reading data from a slow source such as the network.
   * The timeout simply simulates a slow data source by inserting
   * pauses in the reading process.
   */
  load_timeout = g_timeout_add (150,progressive_timeout,image);
}

static void
cleanup_callback (GtkObject *object,
		  gpointer   data)
{
  if (load_timeout)
    {
      g_source_remove (load_timeout);
      load_timeout = 0;
    }
  
  if (pixbuf_loader)
    {
      gdk_pixbuf_loader_close (pixbuf_loader, NULL);
      g_object_unref (pixbuf_loader);
      pixbuf_loader = NULL;
    }

  if (image_stream)
    fclose (image_stream);
  image_stream = NULL;
}

static void
toggle_sensitivity_callback (GtkWidget *togglebutton,
                             gpointer   user_data)
{
  GtkContainer *container = user_data;
  GList *list;
  GList *tmp;
  
  list = gtk_container_get_children (container);

  tmp = list;
  while (tmp != NULL)
    {
      /* don't disable our toggle */
      if (GTK_WIDGET (tmp->data) != togglebutton)
        gtk_widget_set_sensitive (GTK_WIDGET (tmp->data),
                                  !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (togglebutton)));
      
      tmp = tmp->next;
    }

  g_list_free (list);
}
  
int images_main(int argc, char *argv[])
{
  GtkWidget *frame;
  GtkWidget *vbox, *box1;
  GtkWidget *image;
  GtkWidget *label;
  GtkWidget *align;
  GtkWidget *button;
  GdkPixbuf *pixbuf;
  GError *error = NULL;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator;
  
  char *filename;
   gtk_init (&argc,&argv);
  
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title (GTK_WINDOW (window), "Images");

      g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_main_quit), &window);
      g_signal_connect (window, "destroy",
			G_CALLBACK (cleanup_callback), NULL);
      accel_group = gtk_accel_group_new ();
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);

      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box1 = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
			  FALSE, FALSE, 0);

      separator = gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

      vbox = gtk_vbox_new (FALSE, 8);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
      gtk_container_add (GTK_CONTAINER (box1), vbox);

      label = gtk_label_new (NULL);
      gtk_label_set_markup (GTK_LABEL (label),
			    "<u>Image loaded from a file</u>");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      
      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      /* The alignment keeps the frame from growing when users resize
       * the window
       */
      align = gtk_alignment_new (0.5, 0.5, 0, 0);
      gtk_container_add (GTK_CONTAINER (align), frame);
      gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 0);

      /* demo_find_file() looks in the the current directory first,
       * so you can run gtk-demo without installing GTK, then looks
       * in the location where the file is installed.
       */
      pixbuf = NULL;
      filename =demo_find_file("gtk-logo-rgb.gif");
      if(filename)
       pixbuf = gdk_pixbuf_new_from_file (filename, &error);

      if (error)
	{
	  /* This code shows off error handling. You can just use
	   * gtk_image_new_from_file() instead if you don't want to report
	   * errors to the user. If the file doesn't load when using
	   * gtk_image_new_from_file(), a "missing image" icon will
	   * be displayed instead.
	   */
	  GtkWidget *dialog;
	  
	  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_MESSAGE_ERROR,
					   GTK_BUTTONS_CLOSE,
					   "Unable to open image file 'gtk-logo-rgb.gif': %s",
					   error->message);
	  g_error_free (error);
	  
	  g_signal_connect (dialog, "response",
			    G_CALLBACK (gtk_widget_destroy), NULL);
	  
	  gtk_widget_show (dialog);
	}
      pixbuf=gdk_pixbuf_scale_simple (pixbuf,50,50,GDK_INTERP_NEAREST);
      image = gtk_image_new_from_pixbuf (pixbuf);

      gtk_container_add (GTK_CONTAINER (frame), image);


      /* Animation */

      label = gtk_label_new (NULL);
      gtk_label_set_markup (GTK_LABEL (label),
			    "<u>Animation loaded from a file</u>");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      
      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      /* The alignment keeps the frame from growing when users resize
       * the window
       */
      align = gtk_alignment_new (0.5, 0.5, 0, 0);
      gtk_container_add (GTK_CONTAINER (align), frame);
      gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 0);

      filename =demo_find_file ("floppybuddy.gif");
      image = gtk_image_new_from_file (filename);
      g_free (filename);

      gtk_widget_set_size_request (image, 70, 50);

      gtk_container_add (GTK_CONTAINER (frame), image);

      /* Progressive */
      
      
      label = gtk_label_new (NULL);
      gtk_label_set_markup (GTK_LABEL (label),
			    "<u>Progressive image loading</u>");
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      
      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      /* The alignment keeps the frame from growing when users resize
       * the window
       */
      align = gtk_alignment_new (0.5, 0.5, 0, 0);
      gtk_container_add (GTK_CONTAINER (align), frame);
      gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 0);

      /* Create an empty image for now; the progressive loader
       * will create the pixbuf and fill it in.   */
      image = gtk_image_new_from_pixbuf (NULL);
      gtk_container_add (GTK_CONTAINER (frame), image);

      start_progressive_loading (image);

      /* Sensitivity control */
      button = gtk_toggle_button_new_with_mnemonic ("_Insensitive");
      gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

      g_signal_connect (button, "toggled",
                        G_CALLBACK (toggle_sensitivity_callback),
                        vbox);

      gtk_window_set_default_size (GTK_WINDOW (window), 240, 320);
                        

      gtk_widget_show_all (window);
      gtk_main();

  return (vt);
}
