// main.c

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static void destroyWindow(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void consoleMessageCallback(WebKitWebView *webview, gchar *message, gint lineNumber,
                            const gchar *sourceID, gpointer userData) {
    g_print("Console message: %s\n", message);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindow), NULL);

    GtkWidget *webView = webkit_web_view_new();
    gtk_container_add(GTK_CONTAINER(window), webView);

    const char *htmlPath = "file:///home/makano/workspace/rinwebkit/public/index.html";
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webView), htmlPath);

    g_signal_connect(webView, "console-message", G_CALLBACK(consoleMessageCallback), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
