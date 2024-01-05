// main.cpp

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <JavaScriptCore/JavaScript.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <fstream>

#include <openssl/bio.h>
#include <openssl/evp.h>

using namespace std;

static const char *publicFolderPath = "/home/makano/workspace/rinwebkit/public";

static void destroyWindow(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static GtkWidget *webView;

std::string base64Encode(const std::string &input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr = NULL;
    std::string result;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);

    char *outputBuffer;
    long outputLength = BIO_get_mem_data(bio, &outputBuffer);

    if (outputLength > 0) {
        result.assign(outputBuffer, outputLength);
    }

    BIO_free_all(bio);

    return result;
}

static void scriptMessageReceivedCallback(WebKitUserContentManager *manager,
                                          WebKitJavascriptResult *js_result,
                                          gpointer user_data) {
    JSGlobalContextRef context = webkit_javascript_result_get_global_context(js_result);
    JSValueRef value = webkit_javascript_result_get_value(js_result);

    JSStringRef valueString = JSValueToStringCopy(context, value, NULL);
    size_t bufferSize = JSStringGetMaximumUTF8CStringSize(valueString);
    char message[bufferSize];
    JSStringGetUTF8CString(valueString, message, bufferSize);

    if (strncmp(message, "CONSOLE:", strlen("CONSOLE:")) == 0) {
        // Extract the content after "CONSOLE:"
        const char *consoleContent = message + strlen("CONSOLE:");
        g_print("%s\n", consoleContent);
    } else if (strncmp(message, "EXEC:", strlen("EXEC:")) == 0) {
        // Extract the command part
        const char *timestampStart = message + strlen("EXEC:");
        const char *timestampEnd = strchr(timestampStart, ',');
        
        if (timestampEnd == NULL) {
            g_print("Invalid EXEC message format\n");
            return;
        }

        const char *timestamp = strndup(timestampStart, timestampEnd - timestampStart);
        const char *command = timestampEnd + 1;

        // Open a process and establish a pipe for reading its output
        FILE *pipe = popen(command, "r");
        if (!pipe) {
            g_print("Error executing command\n");
            return;
        }

        // Read the output from the pipe
        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }

        // Close the pipe
        pclose(pipe);

        // Send the output back to JavaScript (replace webView with your actual WebKitWebView)
        const gchar *jsCode = g_strdup_printf("window.handleCommandExecutions('%s',`%s`);", timestamp, result.c_str());

        webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(webView), jsCode, NULL, NULL, NULL);
        g_free(const_cast<gchar *>(jsCode));
    } else if (strncmp(message, "FOPEN:", strlen("FOPEN:")) == 0) {
        const char *timestampStart = message + strlen("FOPEN:");
        const char *timestampEnd = strchr(timestampStart, ',');
        
        if (timestampEnd == NULL) {
            g_print("Invalid EXEC message format\n");
            return;
        }

        const char *timestamp = strndup(timestampStart, timestampEnd - timestampStart);
        const char *filepath = timestampEnd + 1;

        std::ifstream file(filepath, std::ios::binary);

        string fileContents = "";

        if (file.is_open()) {
            // Read the file content
            std::stringstream fileContent;
            fileContent << file.rdbuf();
            file.close();

            // Encode the file content to base64
            std::string base64Data = base64Encode(fileContent.str());

            fileContents = base64Data;
        } else {
            fileContents = "not_found";
        }

        // JSGlobalContextRef context = webkit_javascript_result_get_global_context(js_result);
        // JSStringRef jsBase64Data = JSStringCreateWithUTF8CString(fileContents.c_str());
        // JSValueRef jsFileContents = JSValueMakeString(context, jsBase64Data);

        // // Release the JSString reference
        // JSStringRelease(jsBase64Data);

        // // Set the result in the JavaScript context
        // webkit_javascript_result_set_js_value(js_result, jsFileContents);

        const gchar *jsCode = g_strdup_printf("window.handleFileEvent('read', '%s',`%s`);", timestamp, fileContents.c_str());
        
        webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(webView), jsCode, NULL, NULL, NULL);
        g_free(const_cast<gchar *>(jsCode));
    } else if (strncmp(message, "FWRITE:", strlen("FWRITE:")) == 0) {
        // Extract the timestamp and file path from the message
        const char *timestampStart = message + strlen("FWRITE:");
        const char *timestampEnd = strchr(timestampStart, ',');

        if (timestampEnd == NULL) {
            g_print("Invalid FWRITE message format\n");
            return;
        }

        const char *timestamp = strndup(timestampStart, timestampEnd - timestampStart);
        const char *filepathStart = timestampEnd + 1;

        // Find the position of the newline character in the remaining message
        const char *contentStart = strchr(filepathStart, '\n');

        const char *filepath = strndup(filepathStart, contentStart - filepathStart);

        if (contentStart == NULL) {
            g_print("Invalid FWRITE message format\n");
            return;
        }

        const char *content = contentStart + 1;

        // Open the file for writing
        std::ofstream file(filepath, std::ios::binary);

        if (file.is_open()) {
            // Write the decoded content to the file
            file << content;
            file.close();

            const gchar *jsCode = g_strdup_printf("window.handleFileEvent('write', '%s',`%s`);", timestamp, content);
        
            webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(webView), jsCode, NULL, NULL, NULL);
            g_free(const_cast<gchar *>(jsCode));
        } else {
            // Handle file not found or other errors
            g_print("Error opening file for writing\n");
        }
    } else if (strncmp(message, "DIRLIST:", strlen("DIRLIST:")) == 0) {
        // Implement DIRLIST command
        // ... (your DIRLIST logic)
    } else if (strncmp(message, "FSTAT:", strlen("FSTAT:")) == 0) {
        // Implement FSTAT command
        // ... (your FSTAT logic)
    } else {
        g_print("Unknown message: %s\n", message);
    }

    JSStringRelease(valueString);
}

static gboolean refreshWebView(gpointer data) {
    GtkWidget *webView = GTK_WIDGET(data);
    webkit_web_view_reload(WEBKIT_WEB_VIEW(webView));
    return G_SOURCE_CONTINUE;
}

static gboolean handleInotifyEvent(GIOChannel *source, GIOCondition condition, gpointer data) {
    int inotifyFd = g_io_channel_unix_get_fd(source);
    char buffer[4096];

    ssize_t bytesRead = read(inotifyFd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        g_timeout_add(500, refreshWebView, data);  // Delayed refresh to allow file changes to complete
    }

    return TRUE;
}

static void injectJavaScript(WebKitWebView *webView) {
    const char *jsCode = R"(
        window.OS = {
            exec: function() {
                webkit.messageHandlers.OS.postMessage('openTerminal');
            }
        };
    )";
    webkit_web_view_run_javascript(webView, jsCode, NULL, NULL, NULL);
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindow), NULL);

    webView = webkit_web_view_new();

    gtk_container_add(GTK_CONTAINER(window), webView);

    WebKitUserContentManager *contentManager = webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(webView));
    webkit_user_content_manager_register_script_message_handler(contentManager, "OS");

    g_signal_connect(contentManager, "script-message-received::OS", G_CALLBACK(scriptMessageReceivedCallback), NULL);

    // Create an inotify instance to watch for changes in the "public" folder
    int inotifyFd = inotify_init();
    int watchDescriptor = inotify_add_watch(inotifyFd, publicFolderPath, IN_MODIFY);

    GIOChannel *inotifyChannel = g_io_channel_unix_new(inotifyFd);
    g_io_add_watch(inotifyChannel, G_IO_IN, handleInotifyEvent, webView);

    const char *htmlPath = "file:///home/makano/workspace/rinwebkit/public/index.html";
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webView), htmlPath);

    injectJavaScript(WEBKIT_WEB_VIEW(webView));

    gtk_widget_show_all(window);

    gtk_main();

    // Cleanup: Remove the inotify watch and close the inotify descriptor
    inotify_rm_watch(inotifyFd, watchDescriptor);
    close(inotifyFd);

    return 0;
}
