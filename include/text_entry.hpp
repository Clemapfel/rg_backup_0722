// 
// Copyright 2022 Clemens Cords
// Created on 7/18/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

namespace rat
{
    namespace text_entry_wrapper
    {
        void on_enter(GtkEntry*, void* instance);
        void on_backspace(GtkEntry*, void* instance);
        void on_clipboard_paste(GtkEntry*, void* instance);
    }

    class TextEntry
    {
        friend void text_entry_wrapper::on_enter(GtkEntry*, void* instance);
        friend void text_entry_wrapper::on_backspace(GtkEntry*, void* instance);
        friend void text_entry_wrapper::on_clipboard_paste(GtkEntry*, void* instance);

        public:
            TextEntry();
            ~TextEntry();

            GtkWidget* get_native();

        protected:
            virtual void on_enter(GtkEntry*) = 0;
            virtual void on_backspace(GtkEntry*) = 0;
            virtual void on_clipboard_paste(GtkEntry*) = 0;

        private:
            GtkWidget* _native;
    };
}

// ##############################################################################################

namespace rat
{
    namespace text_entry_wrapper
    {
        void on_enter(GtkEntry* entry, void* instance)
        {
            ((TextEntry*) instance)->on_enter(entry);
        }

        void on_backspace(GtkEntry* entry, void* instance)
        {
            ((TextEntry*) instance)->on_backspace(entry);
        }

        void on_clipboard_paste(GtkEntry* entry, void* instance)
        {
            ((TextEntry*) instance)->on_clipboard_paste(entry);
        }
    }

    TextEntry::TextEntry()
        : _native(gtk_entry_new())
    {
        g_signal_connect(_native, "activate", G_CALLBACK(text_entry_wrapper::on_enter), this);
        g_signal_connect(_native, "backspace", G_CALLBACK(text_entry_wrapper::on_backspace), this);
        g_signal_connect(_native, "paste-clipboard", G_CALLBACK(text_entry_wrapper::on_clipboard_paste), this);
    }

    TextEntry::~TextEntry()
    {
        if (GTK_IS_WIDGET(_native))
            gtk_widget_destroy(_native);
    }

    GtkWidget *TextEntry::get_native()
    {
        return _native;
    }
}