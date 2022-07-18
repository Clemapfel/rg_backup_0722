// 
// Copyright 2022 Clemens Cords
// Created on 7/18/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>
#include <string>

#include <include/text_entry.hpp>
#include <include/colors.hpp>

namespace rat
{
    class ColorHTMLCodeEntry : public TextEntry
    {
        public:
            ColorHTMLCodeEntry();

            RGBA get_value() const;

        private:
            virtual void on_enter(GtkEntry* self);
            virtual void on_backspace(GtkEntry* self);
            virtual void on_clipboard_paste(GtkEntry* self);

        private:
            RGBA parse_text(const std::string&);

            static inline RGBA _value = RGBA(1, 1, 1, 1);
    };
}

// ##############################################################################################

namespace rat
{
    ColorHTMLCodeEntry::ColorHTMLCodeEntry()
        : TextEntry()
    {
        gtk_entry_set_max_width_chars(GTK_ENTRY(get_native()), 1 + 2 * 4); // # and 2 per component
    }

    void ColorHTMLCodeEntry::on_enter(GtkEntry *self)
    {
        std::string text_in = gtk_entry_get_text(self);

        try
        {
            _value = html_code_to_rgba(text_in);
        }
        catch (...)
        {
            gtk_entry_set_text(self, "");
            return;
        }

        std::cout << _value.r << " " << _value.g << " " << _value.b << " " << _value.a << std::endl;
    }

    void ColorHTMLCodeEntry::on_clipboard_paste(GtkEntry *self)
    {
        on_enter(self);
    }

    void ColorHTMLCodeEntry::on_backspace(GtkEntry *self)
    {
        // noop
    }
}