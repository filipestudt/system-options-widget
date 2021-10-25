#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int system(const char *command);

GtkWidget *window;
GtkWidget *scale;

int count = 0;
char percentageChar = '%';
char quotationMarks = '"';
char openBracket = '[';
char closedBracket = ']';


void *change_volume_thread(void *vargp)
{
    FILE *fp;
    char buffer[40];
    double val = gtk_range_get_value(scale);
    sprintf(buffer, "amixer -Mq set Master %d%c", (int) val, percentageChar);    
    system(buffer);
    return NULL;
}

static void change_volume (GtkWidget *widget, gpointer   data) {
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, change_volume_thread, NULL);
  //pthread_join(thread_id, NULL);
}

static char* get_volume(int* volume) 
{  
  FILE *fp;
  char path[1035];
  char buffer[50];
  char vol[4] = "aaaa";
  char volClean[3];

  sprintf(buffer, "awk -F%c%c%c%c%c%c '/Left:/ { print $2 }' <(amixer sget Master)", 
    quotationMarks,
    openBracket,
    closedBracket,
    openBracket,
    closedBracket,
    quotationMarks);

  
  fp = popen(buffer, "r");

  if (fp == NULL) {
    g_print("Failed to run command\n" );
    exit(1);
  }

  /* Read the output */
  fgets(vol, sizeof(path), fp);

  pclose(fp);  

  /* Remove the percentage char */
  for(int i = 0; i < strlen(vol); i++) {
    if (vol[i] == '%') {
      break;
    }

    volClean[i] = vol[i];
  }

  g_print(volClean);

  /* Converts from string to int */
  volume = atoi(volClean);
}

int main (int* argc, char*** argv)
{
  GtkBuilder* builder;
  GdkScreen* screen;
  GdkVisual* visual;
  int volume;

  gtk_init(&argc, &argv);

  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_path (provider,
   "style.css", NULL);

  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
    GTK_STYLE_PROVIDER(provider), 
    GTK_STYLE_PROVIDER_PRIORITY_USER);
  
  builder = gtk_builder_new_from_file("ui.glade");  
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_builder_connect_signals(builder, NULL);

  // Transparency
  screen = gtk_window_get_screen(window);
  visual = gdk_screen_get_rgba_visual(screen);
  gtk_widget_set_visual(window, visual);

  gtk_widget_show (window);

  get_volume(volume);
  scale = gtk_builder_get_object(builder, "scale");
  gtk_range_set_range(scale, (double) 0, (double) 100);
  gtk_range_set_value(scale, (double) volume);
  gtk_range_set_show_fill_level(scale, false);

  g_signal_connect (scale, "value-changed", G_CALLBACK (change_volume), NULL);
    
  gtk_main();
  return EXIT_SUCCESS;
}