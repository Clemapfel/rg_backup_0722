# Colors

RGB
HSV
HSL
GrayScale
HTML

quickbind colors: press 1, 2, etc. to swap to color. 0 is always erase

Color Replace:
    select color a, identified by exact hue, saturation range and value range
    select color b, identified by exact hue, keep saturation and value range

# Selection

### Shapes:
+ Rectangle
+ Eclipse
+ Paintbrush
+ Lasso

### Modes:
+ Add : `+` next to tool icon
+ Subtract: `-` next to tool icon
+ Replace (default): ` ` next to icon
+ invert

# Brushes
## Brush Designer

square, 1-bit (value is 1 or 0) per px grid
pasted color: 
+ 1 * (rgb), a for 1-bits
+ (0, 0, 0, 0) for 0-bits

Brush sizes: 3x3, 4x4, 8x8, 16x16, 32x32, 64x64<br>
send warning if not at least 1 pixel is set

### defaults
+ squares: rectangles: 1x1 px (default), 2x2
+ circles: 2x2, 4x4 circle

## Brush Controls:
    
hold control to lock x-y movement

# Tiling

Default: 1x1 tiles


# Keymap

| Value  | Condition | Meaning                              |
|--------|-----------|--------------------------------------|
| 0      | Brush     | Select Eraser                        |
| 1 - 9  | Brush     | Select Color                         |
| Esc    | Any       | Cancel action                        |
| Arrows | Selected  | Move selected area                   |
| Arrows | None      | Move canvas                          |
| Tab    | Any       | cycle through toolbar                |
| Plus   | Any       | Zoom in                              |
| Minus  | Any       | Zoom out                             | 
 | Ctrl+x | Selected  | Erase Selection and add to clipboard |
 | Ctrl+c | Selected  | add to clipboard                     |
 | Ctrl+v | Any       | paste clipboard                      |
 | TODO   | Any       | toggle tiling guide                  |
 | Ctrl+a | Any       | Switch to select tool, select all    | 


# Accessibility

All icons are 32x32 px images, have option to scale them 2x, 3x, etc.
Option to scale font in tooltips arbitrarily large

# Top Bar

```
File > 
    Save
    Load
    Export
    Export as Spritesheet

Selection > 
    Select All
    Select None
    Crop to Selection
    Invert Selection
    Set Mode: Replace
    Set Mode: Add
    Set Mode: Subtract
    
Layers >
    Resize Image
    New Layer
    Delete Layer
    Hide Layer
    Merge Down
    Flatten All
    Toggle Layer Locked
    Layer Transparency
    Blending >
        Default 
        Add
        Subtract
        Reverse Subtract
        Multiply
            
Animation >
    New Frame
    Duplicate Frame
    Delete Frame
    Toggle Frame Enabled
    Toggle Preview Animated
    Set FPS
        
Color > 
    Invert Colors
    Make Grayscale
    Palette >
        load palette
        save palette
        save as default
        sort by hue
        create palette from image
    Replace 
    Color Picker
    
Tools >
    Brush
    Brush Designer
    Eraser
    Selection > 
        Rectangular
        Ellipsoid
        Lasso
        Wand
        Fuzzy   
    Shape >
        Line
        Line Sequence
        Rectangle
        Rectangle Frame
        Ellipse
        Ellipse Frame
    Tile Placer
    Tile Designer

Preferences >
    Tiling 
    Keymap 
    Font Size
    Icon Size
    Performance >
        limit undo history size
        limit number of cores
        disable shaders

Help > 
    Show Keybindings
    Documentation > link to github
```

