/*================================================================================================*/
/**^M
    @file   notebook.c^M
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


#include <gtk/gtk.h>
#include <stdio.h>
gint vt=FALSE;
static const char* const xpm_data[] = {
"48 48 64 1",
"       c None",
".      c #DF7DCF3CC71B",
"X      c #965875D669A6",
"o      c #71C671C671C6",
"O      c #A699A289A699",
"+      c #965892489658",
"@      c #8E38410330C2",
"#      c #D75C7DF769A6",
"$      c #F7DECF3CC71B",
"%      c #96588A288E38",
"&      c #A69992489E79",
"*      c #8E3886178E38",
"=      c #104008200820",
"-      c #596510401040",
";      c #C71B30C230C2",
":      c #C71B9A699658",
">      c #618561856185",
",      c #20811C712081",
"<      c #104000000000",
"1      c #861720812081",
"2      c #DF7D4D344103",
"3      c #79E769A671C6",
"4      c #861782078617",
"5      c #41033CF34103",
"6      c #000000000000",
"7      c #49241C711040",
"8      c #492445144924",
"9      c #082008200820",
"0      c #69A618611861",
"q      c #B6DA71C65144",
"w      c #410330C238E3",
"e      c #CF3CBAEAB6DA",
"r      c #71C6451430C2",
"t      c #EFBEDB6CD75C",
"y      c #28A208200820",
"u      c #186110401040",
"i      c #596528A21861",
"p      c #71C661855965",
"a      c #A69996589658",
"s      c #30C228A230C2",
"d      c #BEFBA289AEBA",
"f      c #596545145144",
"g      c #30C230C230C2",
"h      c #8E3882078617",
"j      c #208118612081",
"k      c #38E30C300820",
"l      c #30C2208128A2",
"z      c #38E328A238E3",
"x      c #514438E34924",
"c      c #618555555965",
"v      c #30C2208130C2",
"b      c #38E328A230C2",
"n      c #28A228A228A2",
"m      c #41032CB228A2",
"M      c #104010401040",
"N      c #492438E34103",
"B      c #28A2208128A2",
"V      c #A699596538E3",
"C      c #30C21C711040",
"Z      c #30C218611040",
"A      c #965865955965",
"S      c #618534D32081",
"D      c #38E31C711040",
"F      c #082000000820",
"                                                ",
"          .XoO                                  ",
"         +@#$%o&                                ",
"         *=-;#::o+                              ",
"           >,<12#:34                            ",
"             45671#:X3                          ",
"               +89<02qwo                        ",
"e*                >,67;ro                       ",
"ty>                 459@>+&&                    ",
"$2u+                  ><ipas8*                  ",
"%$;=*                *3:.Xa.dfg>                ",
"Oh$;ya             *3d.a8j,Xe.d3g8+             ",
" Oh$;ka          *3d$a8lz,,xxc:.e3g54           ",
"  Oh$;kO       *pd$%svbzz,sxxxxfX..&wn>         ",
"   Oh$@mO    *3dthwlsslszjzxxxxxxx3:td8M4       ",
"    Oh$@g& *3d$XNlvvvlllm,mNwxxxxxxxfa.:,B*     ",
"     Oh$@,Od.czlllllzlmmqV@V#V@fxxxxxxxf:%j5&   ",
"      Oh$1hd5lllslllCCZrV#r#:#2AxxxxxxxxxcdwM*  ",
"       OXq6c.%8vvvllZZiqqApA:mq:Xxcpcxxxxxfdc9* ",
"        2r<6gde3bllZZrVi7S@SV77A::qApxxxxxxfdcM ",
"        :,q-6MN.dfmZZrrSS:#riirDSAX@Af5xxxxxfevo",
"         +A26jguXtAZZZC7iDiCCrVVii7Cmmmxxxxxx%3g",
"          *#16jszN..3DZZZZrCVSA2rZrV7Dmmwxxxx&en",
"           p2yFvzssXe:fCZZCiiD7iiZDiDSSZwwxx8e*>",
"           OA1<jzxwwc:$d%NDZZZZCCCZCCZZCmxxfd.B ",
"            3206Bwxxszx%et.eaAp77m77mmmf3&eeeg* ",
"             @26MvzxNzvlbwfpdettttttttttt.c,n&  ",
"             *;16=lsNwwNwgsvslbwwvccc3pcfu<o    ",
"              p;<69BvwwsszslllbBlllllllu<5+     ",
"              OS0y6FBlvvvzvzss,u=Blllj=54       ",
"               c1-699Blvlllllu7k96MMMg4         ",
"               *10y8n6FjvllllB<166668           ",
"                S-kg+>666<M<996-y6n<8*          ",
"                p71=4 m69996kD8Z-66698&&        ",
"                &i0ycm6n4 ogk17,0<6666g         ",
"                 N-k-<>     >=01-kuu666>        ",
"                 ,6ky&      &46-10ul,66,        ",
"                 Ou0<>       o66y<ulw<66&       ",
"                  *kk5       >66By7=xu664       ",
"                   <<M4      466lj<Mxu66o       ",
"                   *>>       +66uv,zN666*       ",
"                              566,xxj669        ",
"                              4666FF666>        ",
"                               >966666M         ",
"                                oM6668+         ",
"                                  *4            ",
"                                                ",
"                                                "};

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
  { "/File/_Exit - Fail  ","<control>E", destroy_Exit,	      0 },
  { "/_Help",		 NULL,	       0,		      0, "<Branch>" },
  { "/Help/_About",	  "<control>H",	0,	      0 },
};

/* This function rotates the position of the tabs */
void rotate_book( GtkButton   *button,
                  GtkNotebook *notebook )
{
    gtk_notebook_set_tab_pos (notebook, (notebook->tab_pos + 1) % 4);
}

/* Add/Remove the page tabs and the borders */
void tabsborder_book( GtkButton   *button,
                      GtkNotebook *notebook )
{
    gint tval = FALSE;
    gint bval = FALSE;
    if (notebook->show_tabs == 0)
	    tval = TRUE;
    if (notebook->show_border == 0)
	    bval = TRUE;

    gtk_notebook_set_show_tabs (notebook, tval);
    gtk_notebook_set_show_border (notebook, bval);
}

/* Remove a page from the notebook */
void remove_book( GtkButton   *button,
                  GtkNotebook *notebook )
{
    gint page;

    page = gtk_notebook_get_current_page (notebook);
    gtk_notebook_remove_page (notebook, page);
    /* Need to refresh the widget --
     This forces the widget to redraw itself. */
    gtk_widget_queue_draw (GTK_WIDGET (notebook));
}


/* when invoked (via signal delete_event), terminates the application.
 */
gint close_application( GtkWidget *widget,
                        GdkEvent  *event,
                        gpointer   data )
{
    gtk_main_quit ();
    return FALSE;
}



int notebook_main( int   argc, char *argv[] )
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window, *pixmapwid;
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    GtkStyle *style;
    GtkWidget *button;
    GtkWidget *table,*table_button;
    GtkWidget *notebook;
    GtkWidget *frame,*frame_b;
    GtkWidget *label;
    GtkWidget *checkbutton;
    int i;
    char bufferf[32];
    char bufferl[32];
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;
    GtkWidget *separator;
    GtkWidget *box1, *bbox_h;
    
    /* create the main window, and attach delete_event signal to terminating
       the application */
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit),&window);
    gtk_container_set_border_width (GTK_CONTAINER (window),0);
    gtk_window_set_default_size(GTK_WINDOW(window),240,320);
    gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);
    gtk_widget_show (window);

    /* now for the pixmap from gdk */
    style = gtk_widget_get_style (window);
  
    
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



    table = gtk_table_new (2, 5, FALSE);
    gtk_container_add (GTK_CONTAINER (box1), table);

    /* Create a new notebook, place the position of the tabs */
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
    gtk_table_attach_defaults (GTK_TABLE (table), notebook,0, 2, 0, 4);

    /* Let's append a bunch of pages to the notebook */
    for (i = 0; i < 2; i++) {
	sprintf(bufferf, "Append Frame %d", i + 1);
	sprintf(bufferl, "Page %d", i + 1);

	frame = gtk_frame_new (bufferf);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_widget_set_size_request (frame,10,5);

        label = gtk_label_new (bufferf);
	gtk_container_add (GTK_CONTAINER (frame), label);

	label = gtk_label_new (bufferl);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);
    }

    /* Now let's add a page to a specific spot */
    checkbutton = gtk_check_button_new_with_label ("Check me please!");
    gtk_widget_set_size_request (checkbutton,10,5);

    label = gtk_label_new ("Add page");
    gtk_notebook_insert_page (GTK_NOTEBOOK (notebook), checkbutton, label, 2);

    /* Now finally let's prepend pages to the notebook */
    for (i = 0; i < 2; i++) {
  	sprintf (bufferf, "Prepend Frame %d", i + 1);
  	sprintf (bufferl, "PPage %d", i + 1);

	frame = gtk_frame_new (bufferf);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
	gtk_widget_set_size_request (frame, 30,15);
        pixmap = gdk_pixmap_create_from_xpm_d (window->window,  &mask,
                                           &style->bg[GTK_STATE_NORMAL],
                                           (gchar **)xpm_data);

    /* a pixmap widget to contain the pixmap */
    pixmapwid = gtk_image_new_from_pixmap (pixmap, mask);
    gtk_widget_show (pixmapwid);
  
   gtk_container_add (GTK_CONTAINER (frame), pixmapwid);
   gtk_widget_show (frame);

	label = gtk_label_new (bufferl);
	gtk_notebook_prepend_page (GTK_NOTEBOOK (notebook), frame, label);
    }

    /* Set what page to start at (page 4) */
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 3);
    bbox_h = gtk_vbox_new (FALSE, 25);
    gtk_container_set_border_width (GTK_CONTAINER (bbox_h), 5);

    frame_b = gtk_frame_new (NULL);
    gtk_widget_set_usize(frame_b,200,50);
    gtk_box_pack_start (GTK_BOX (bbox_h),frame_b,TRUE,TRUE,0);//FALSE,FALSE,0);
    table_button = gtk_table_new (2, 3,TRUE);

    gtk_container_add (GTK_CONTAINER (frame_b),table_button);
    gtk_table_attach_defaults (GTK_TABLE (table), bbox_h, 0,1,4,5);
    /* Create a bunch of buttons */
    button = gtk_button_new_with_label ("next page");
    g_signal_connect_swapped (G_OBJECT (button), "clicked",
			      G_CALLBACK (gtk_notebook_next_page),
			      G_OBJECT (notebook));
    gtk_widget_set_usize(button,5,5);

    gtk_table_attach_defaults (GTK_TABLE (table_button), button, 0,1, 0,1);


    button = gtk_button_new_with_label ("prev page");
    g_signal_connect_swapped (G_OBJECT (button), "clicked",
			      G_CALLBACK (gtk_notebook_prev_page),
			      G_OBJECT (notebook));
    gtk_table_attach_defaults (GTK_TABLE (table_button), button, 1,2, 0,1);

    button = gtk_button_new_with_label ("tab position");
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (rotate_book),
	              (gpointer) notebook);
    gtk_table_attach_defaults (GTK_TABLE (table_button), button, 0,2, 2,3);

    button = gtk_button_new_with_label ("tabs/border on/off");
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (tabsborder_book),
                      (gpointer) notebook);
    gtk_table_attach_defaults (GTK_TABLE (table_button), button, 0,1, 1,2);

    button = gtk_button_new_with_label ("remove page");
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (remove_book),
                      (gpointer) notebook);
    gtk_table_attach_defaults (GTK_TABLE (table_button), button, 1,2, 1,2);


    gtk_widget_show_all (window);


    /* show the window */
    gtk_main ();
          
    return (vt);
}
