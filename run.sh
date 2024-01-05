g++ -o my_webkit_program main.cpp $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0) -lgtk-3 -lwebkit2gtk-4.0 -lssl -lcrypto

./my_webkit_program
