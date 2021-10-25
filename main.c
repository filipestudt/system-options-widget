#include <gtk/gtk.h>

int main (int* argc, char*** argv)
{
  GtkBuilder* builder;
  GtkWidget* window;
  GdkScreen* screen;
  GdkVisual* visual;

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
    
  gtk_main();
  return EXIT_SUCCESS;
}