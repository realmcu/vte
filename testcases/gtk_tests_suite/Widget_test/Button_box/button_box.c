/**/
/**^M
    @file   button_box.c^M
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
#include<string.h>
#include<stdlib.h>
GtkWidget *window,*text[10],*box,*button[15],*box1,*box2,*box3,*box4,*sep,*box2_2;
GtkWidget *box2_3,*box5;
gint vtFALSE;
GtkWidget *main_window;
GtkWidget *main_window1;
gboolean expand0,fill0,homogen0;

int t,padding0,space0;
char name[60],no;
char isim[15][30]{"Expand FALSE","Fill FALSE","Padding 10",
                   "Homojen FALSE","Spacing 0",
                   "Expand TRUE","Fill TRUE",
                   "Padding 0","Homojen TRUE","Spacing 10",
                   "Vertical \nButton \nBoxes","Horizontal\n Button\n Boxes","Quit_Pass","Exit_Fail"
                   };
char labelisim[6][15]{"expand  ","fill  ","padding   ","homojen  ","spacing  "};
char button_stock[4][30]{"Vertical Button Boxes","Horizontal Button Boxes","Quit","Exit"};
void button_click(GtkWidget *,GdkEventButton *,gpointer );
void button_click2(GtkWidget *,GdkEventButton *,gpointer );
static GtkWidget *create_bbox (gint  ,char *,gint ,gint);

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

void destroy_Win2( GtkWidget *widget,gpointer data )
{  gtk_widget_hide(main_window1);}
void destroy_Win1( GtkWidget *widget,gpointer data )
{ gtk_widget_hide(main_window);}

void destroy(GtkWidget *widget, gpointer data){ gtk_main_quit();}

void click(GtkWidget *widget, GdkEventButton *data, gpointer *po)
{
  char *str;
gint  horizontal;

  strgtk_widget_get_name(GTK_WIDGET(po));
    if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button05"))
        expand1;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button00"))
        expand0;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button06"))
        fill1;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button01"))
        fill0;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button07"))
        padding0;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button02"))
        padding10;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button08"))
        homogen1;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button03"))
        homogen0;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button09"))
        space10;
    else if(!strcmp(gtk_widget_get_name(GTK_WIDGET(po)),"button04"))
        space0;
    else g_print("hello\n");

    gtk_box_set_homogeneous(GTK_BOX(box3),homogen);
    gtk_box_set_spacing(GTK_BOX(box3),space);
    gtk_box_set_homogeneous(GTK_BOX(box4),homogen);
    gtk_box_set_spacing(GTK_BOX(box4),space);

    for(t4;t<5;t++){
        gtk_box_set_child_packing (GTK_BOX(box3),text[t],expand,fill,padding,GTK_PACK_START);
        gtk_box_set_child_packing (GTK_BOX(box4),button[t],expand,fill,padding,GTK_PACK_START);
    }

    strcpy(name,labelisim[4]);
    if(space0)
        strcat(name,"0");
    else
        strcat(name,"10");

    gtk_label_set_text(GTK_LABEL(text[4]),name);

    strcpy(name,labelisim[0]);
    tstrlen(name);
    name[t]expand+48;
    name[t+1]0;
    gtk_label_set_text(GTK_LABEL(text[0]),name);

    strcpy(name,labelisim[1]);
    tstrlen(name);
    name[t]fill+48;
    name[t+1]0;
    gtk_label_set_text(GTK_LABEL(text[1]),name);

    strcpy(name,labelisim[2]);
    if(padding0)
        strcat(name,"0");
    else
        strcat(name,"10");

    gtk_label_set_text(GTK_LABEL(text[2]),name);

    strcpy(name,labelisim[3]);
    tstrlen(name);
    name[t]homogen+48;
    name[t+1]0;
    gtk_label_set_text(GTK_LABEL(text[3]),name);



}

/* Create a new hbox with an image and a label packed into it
 * and return the box. */

GtkWidget *xpm_label_box( gchar     *xpm_filename,
                          gchar     *label_text )
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;

    /* Create box for image and label */
    box  gtk_hbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image  gtk_image_new_from_file (xpm_filename);

    /* Create a label for the button */
    label  gtk_label_new (label_text);

    /* Pack the image and label into the box */
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 3);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);

    gtk_widget_show (image);
    gtk_widget_show (label);

    return box;
}


int button_box_main( int   argc,char *argv[] )
{
GtkWidget *sep1;
GtkWidget *box_p;
  gtk_init (&argc, &argv);

        window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title((GtkWindow *)window,"Box_button Program");
        gtk_widget_set_usize(window,240,320);
        gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
        gtk_signal_connect (GTK_OBJECT (window), "destroy",
                             GTK_SIGNAL_FUNC (gtk_main_quit), &window);
        gtk_container_set_border_width (GTK_CONTAINER (window), 5);


    box1gtk_vbox_new(TRUE,0);
    boxgtk_vbox_new(TRUE,0);
    box2gtk_vbox_new(FALSE,5);
    box3gtk_vbox_new(homogen,space);
    box4gtk_vbox_new(homogen,space);
    box2_2gtk_hbox_new(FALSE,5);
    box2_3gtk_hbox_new(FALSE,0);
    box5gtk_vbox_new(FALSE,0);


    for(t0;t<5;t++){
        strcpy(name,"label");
        name[6]t+48;
        name[7]'\0';
        text[t]gtk_label_new(labelisim[t]);
        gtk_label_set_justify(GTK_LABEL(text[t]),GTK_JUSTIFY_FILL);
        gtk_box_pack_start(GTK_BOX(box3),text[t],expand,fill,space);

    }

    gtk_box_pack_start(GTK_BOX(box2_2),box3,FALSE,FALSE,0);

    sep1gtk_hseparator_new();

    for(t0;t<14;t++){
        strcpy(name,"button");
        name[6]t/10+48;
        name[7]t%10+48;
        name[8]'\0';
        button[t]gtk_button_new_with_label(isim[t]);
        gtk_widget_set_name(GTK_WIDGET(button[t]),name);
        if(t<10 || t>11)gtk_widget_set_usize(button[t],100,20);
     switch(t)
      {
       default:

        gtk_signal_connect_object(GTK_OBJECT(button[t]),
                   "button_release_event",GTK_SIGNAL_FUNC(click),(gpointer)box3);
        if(t>4)
            gtk_box_pack_start(GTK_BOX(box1),button[t],FALSE,FALSE,0);
        else
            gtk_box_pack_start(GTK_BOX(box),button[t],FALSE,FALSE,0);
       break;
       case 10:
        gtk_box_pack_start(GTK_BOX(box5),button[t],FALSE,FALSE,0);
        gtk_signal_connect (GTK_OBJECT (button[t]), "clicked",
                                GTK_SIGNAL_FUNC (button_click2),NULL);

        break;
       case 11:
        gtk_box_pack_start(GTK_BOX(box5),button[t],FALSE,FALSE,0);
        gtk_signal_connect (GTK_OBJECT (button[t]), "clicked",
                                GTK_SIGNAL_FUNC (button_click),NULL);
       break;
       case 12:
        gtk_widget_set_usize(button[t],100,20);
         gtk_box_pack_start(GTK_BOX(box1),button[t],FALSE,FALSE,0);
         gtk_signal_connect (GTK_OBJECT (button[t]), "clicked",
                                GTK_SIGNAL_FUNC (destroy_Quit),NULL);
        break;
       case 13:
        gtk_widget_set_usize(button[t],100,20);
        gtk_box_pack_start(GTK_BOX(box),button[t],FALSE,FALSE,0);
        gtk_signal_connect (GTK_OBJECT (button[t]), "clicked",
                                GTK_SIGNAL_FUNC (destroy_Exit),NULL);
       break;
      }
    }

    for(t0;t<5;t++){
        button[t]gtk_button_new_with_label("button");
        gtk_box_pack_start(GTK_BOX(box4),button[t],expand,fill,space);
    }
    sepgtk_vseparator_new();

      button[14]gtk_button_new();
    /* This calls our box creating function */
    box_p  xpm_label_box ("info.xpm", "cool button");

    /* Pack and show all our widgets */
    gtk_container_add (GTK_CONTAINER (button[14]), box_p);

    gtk_box_pack_start (GTK_BOX (box2_2), box4,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box2_2), sep,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(box2), box2_2,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box2), box2_3,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box2_2), box5,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (box2), sep1,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (box2_3), box1,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (box2_3), box,FALSE,FALSE,0);
    gtk_container_add (GTK_CONTAINER (window), box2);
    gtk_box_pack_start(GTK_BOX(box2),button[14],FALSE,FALSE,0);

    gtk_widget_show_all  (window);

    gtk_main ();


    return(vt);
}

void button_click2(GtkWidget *widget,GdkEventButton *data,gpointer po)
{
  GtkWidget *bbox;

  if(main_window1)
   {gtk_window_present(GTK_WINDOW(main_window1));
    return;}
    main_window1 gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(main_window1),GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (main_window1),240,320);

    gtk_window_set_title (GTK_WINDOW (main_window1), "Button Vertical");

    gtk_container_set_border_width (GTK_CONTAINER (main_window1), 5);
      bboxgtk_hbox_new (TRUE, 0);
    gtk_container_add (GTK_CONTAINER (main_window1), bbox);
    gtk_window_set_policy(GTK_WINDOW (main_window1),TRUE,TRUE,TRUE);

    gtk_container_set_border_width (GTK_CONTAINER (bbox), 2); //10
    gtk_box_pack_start (GTK_BOX (bbox),
   create_bbox (FALSE, "Spread", 5, GTK_BUTTONBOX_SPREAD), //20
   TRUE, TRUE, 0);

     gtk_box_pack_start (GTK_BOX (bbox),
   create_bbox (FALSE, "Edge", 5, GTK_BUTTONBOX_EDGE),  //20
   TRUE, TRUE,0);
    gtk_widget_show_all (main_window1);

}

void button_click(GtkWidget *widget,GdkEventButton *data,gpointer po)
{
  GtkWidget *bbox;

  if(main_window)
   {gtk_window_present(GTK_WINDOW(main_window));
    return;}

    main_window  gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(main_window),GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (main_window),240,320);

    gtk_window_set_title (GTK_WINDOW (main_window), "Button Horizontal");

    gtk_container_set_border_width (GTK_CONTAINER (main_window), 5);
    bbox  gtk_vbox_new (TRUE, 0);
    gtk_container_add (GTK_CONTAINER (main_window), bbox);
    gtk_window_set_policy(GTK_WINDOW (main_window),TRUE,TRUE,TRUE);
    gtk_container_set_border_width (GTK_CONTAINER (bbox), 2);
    gtk_box_pack_start (GTK_BOX (bbox),
    create_bbox (TRUE, "Spread", 5, GTK_BUTTONBOX_SPREAD),
   TRUE, TRUE, 0);

     gtk_box_pack_start (GTK_BOX (bbox),
   create_bbox (TRUE, "Edge", 5, GTK_BUTTONBOX_EDGE),
   TRUE, TRUE,0);

    gtk_box_pack_start (GTK_BOX (bbox),
   create_bbox (TRUE, "Start", 5, GTK_BUTTONBOX_START),
     TRUE, TRUE, 0);

    gtk_box_pack_start (GTK_BOX (bbox),
   create_bbox (TRUE, "End", 5, GTK_BUTTONBOX_END),
   TRUE, TRUE, 5);

    gtk_widget_show_all (main_window);

}

static GtkWidget *create_bbox (gint  horizontal,char *title,gint  spacing,
      gint  layout)
{
  GtkWidget *frame;
  GtkWidget *bbox;
  GtkWidget *button1,*button2,*button3;

  frame  gtk_frame_new (title);

  if (horizontal)
    bbox  gtk_hbutton_box_new ();
  else
    bbox  gtk_vbutton_box_new ();

  gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (bbox),10,5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), layout);
  gtk_box_set_spacing (GTK_BOX (bbox), spacing);
  button1  gtk_button_new_from_stock (GTK_STOCK_OK);
  button2 gtk_button_new_with_label ("CANCEL");
  if (horizontal)  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
        GTK_SIGNAL_FUNC (destroy_Win1),NULL);
  else
   {
     gtk_signal_connect (GTK_OBJECT (button2), "clicked",
     GTK_SIGNAL_FUNC (destroy_Win2),NULL);
     button3  gtk_button_new_from_stock (GTK_STOCK_HELP);
     gtk_box_pack_start (GTK_BOX (bbox), button3, TRUE, FALSE, 0);
   }
  gtk_container_add (GTK_CONTAINER (frame), bbox);
  gtk_box_pack_start (GTK_BOX (bbox), button1, TRUE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (bbox), button2, TRUE, FALSE, 0);
  return frame;
}
