// 
// Copyright 2022 Clemens Cords
// Created on 7/18/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>
#include <string>

#include <include/text_entry.hpp>

namespace rat
{
    class ColorComponentEntry : public TextEntry
    {
        public:
            ColorComponentEntry(char id, size_t n_decimals = 3);
            ~ColorComponentEntry();

            using TextEntry::get_native;

            float get_value() const;
            char get_id() const;

        protected:
            virtual void on_enter(GtkEntry* self);
            virtual void on_backspace(GtkEntry* self);
            virtual void on_clipboard_paste(GtkEntry* self);

        private:
            size_t _n_decimals;
            char _id;
            static inline std::map<char, float> _values = {};

            float parse_text(const std::string&);

    };
}

// ##############################################################################################

#include <iostream>

namespace rat
{
    ColorComponentEntry::ColorComponentEntry(char id, size_t n_decimals)
       : TextEntry(), _id(id), _n_decimals(n_decimals)
    {
        gtk_entry_set_width_chars(GTK_ENTRY(get_native()), n_decimals + 2);

        if (_values.find(id) != _values.end())
            std::cerr << "[WARNING] In ColorComponentEntry::ColorComponentEntry: entry with id `" << id << "` already exist." << std::endl;

        _values.insert_or_assign(id, 0);
    }

    ColorComponentEntry::~ColorComponentEntry()
    {
        _values.erase(_id);
    }

    float ColorComponentEntry::get_value() const
    {
        return _values.at(_id);
    }

    char ColorComponentEntry::get_id() const
    {
        return _id;
    }

    float ColorComponentEntry::parse_text(const std::string& text)
    {
        static auto is_a_number = [](char in){
            return in == '0' or in == '1' or in == '2' or in == '3' or in == '4' or in == '5' or in == '6' or in == '7' or in == '8' or in == '9';
        };

        bool valid = true;
        auto parsed = std::string();
        for (auto c : text)
        {
            if (c == ' ')
                continue;
            else if (c == ',')
                parsed.push_back('.');
            else if (is_a_number(c) or c == '.')
                parsed.push_back(c);
            else
            {
                std::cerr << "[WARNING] In ColorComponentEntry::parse_text: Unable to parse string \"" << text << "\" to number. Detected non-number character `" << c << "`" << std::endl;
                valid = false;
                break;
            }
        }

        if (not valid or parsed.empty())
            return -1;
        else
            return std::stof(parsed);
    }

    void ColorComponentEntry::on_enter(GtkEntry *self)
    {
        std::string text_in = gtk_entry_get_text(self);
        float value = parse_text(text_in);

        value *= std::pow(10, _n_decimals);
        value = std::round(value);
        value /= std::pow(10, _n_decimals);

        std::string text_out;
        if (value <= 0.0)
        {
            value = 0.0;
            text_out = "0.0";
        }
        else if (value >= 1.0)
        {
            value = 1.0;
            text_out = "1.0";
        }
        else
        {
            text_out = std::to_string(value);
            while (text_out.back() == '0')
                text_out.erase(text_out.end() - 1);
        }

        _values.at(_id) = value;
        gtk_entry_set_text(self, text_out.c_str());
    }

    void ColorComponentEntry::on_clipboard_paste(GtkEntry *self)
    {
        on_enter(self);
    }

    void ColorComponentEntry::on_backspace(GtkEntry *self)
    {
        // noop
    }
}