#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "../inc/gui.h"
#include "../inc/archive_engine.h"

#include <iostream>
#include <string>

#define WINDOW_COLOR FL_WHITE
#define BUTTON_COLOR 0x4682B400
#define TEXT_COLOR FL_BLACK
#define HIGHLIGHT_COLOR 0x4169E100

Fl_Input* filePathInput;
Fl_Text_Display* outputDisplay;
Fl_Text_Buffer* outputBuffer;

void archive_callback(Fl_Widget* widget, void* data) {
    std::string sourcePath = filePathInput->value();
    
    if (sourcePath.empty()) {
        fl_alert("Please select a file first");
        return;
    }

    if (archiveFile(&sourcePath) == 0) {
        outputBuffer->append("File packaged successfully as: ");
        outputBuffer->append((sourcePath + ".access.pkg").c_str());
        outputBuffer->append("\n");
    } else {
        fl_alert("Failed to package file!");
    }
}

void restore_callback(Fl_Widget* widget, void* data) {
    std::string sourcePath = filePathInput->value();
    
    if (sourcePath.empty()) {
        fl_alert("Please select a file first");
        return;
    }

    if (restoreFile(&sourcePath) == 0) {
        outputBuffer->append("File unpackaged successfully\n");
    } else {
        fl_alert("Failed to unpackage file!");
    }
}

void file_browse_callback(Fl_Widget* widget, void* data) {
    const char* filename = fl_file_chooser("Select File", "*", "");
    if (filename) {
        filePathInput->value(filename);
    }
}

int run_gui(int argc, char** argv) {
    Fl_Window* window = new Fl_Window(500, 350, "File Permissions Packager");
    window->color(WINDOW_COLOR);
    
    Fl_Box* titleBox = new Fl_Box(20, 10, 460, 30, "File Permissions Packager");
    titleBox->labelfont(FL_BOLD);
    titleBox->labelsize(18);
    titleBox->labelcolor(0x33333300);

    filePathInput = new Fl_Input(100, 50, 300, 30, "File:");
    filePathInput->textcolor(TEXT_COLOR);
    
    Fl_Button* browseButton = new Fl_Button(410, 50, 70, 30, "Browse...");
    browseButton->color(BUTTON_COLOR);
    browseButton->callback(file_browse_callback);

    Fl_Button* serializeButton = new Fl_Button(100, 100, 150, 40, "Package");
    serializeButton->color(BUTTON_COLOR);
    serializeButton->selection_color(HIGHLIGHT_COLOR);
    serializeButton->callback(archive_callback);
    
    Fl_Button* deserializeButton = new Fl_Button(260, 100, 150, 40, "Unpackage");
    deserializeButton->color(BUTTON_COLOR);
    deserializeButton->selection_color(HIGHLIGHT_COLOR);
    deserializeButton->callback(restore_callback);

    outputBuffer = new Fl_Text_Buffer();
    outputDisplay = new Fl_Text_Display(20, 160, 460, 170, "Status:");
    outputDisplay->buffer(outputBuffer);
    outputDisplay->textcolor(TEXT_COLOR);
    outputDisplay->color(FL_WHITE);

    window->end();
    window->show(argc, argv);
    return Fl::run();
}