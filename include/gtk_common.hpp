// 
// Copyright 2022 Clemens Cords
// Created on 7/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/vector.hpp>

#include <gtk/gtk.h>

namespace rat
{
    template<typename T>
    Vector2f gtk_widget_get_true_size(T* widget)
    {
        int width, height;
        gtk_widget_get_size_request(GTK_WIDGET(widget), &width, &height);

        int left = gtk_widget_get_margin_start(GTK_WIDGET(widget));
        int right = gtk_widget_get_margin_end(GTK_WIDGET(widget));
        int top = gtk_widget_get_margin_top(GTK_WIDGET(widget));
        int bottom = gtk_widget_get_margin_bottom(GTK_WIDGET(widget));

        width += (left + right);
        height += (top + bottom);

        return Vector2f(width, height);
    }
}
