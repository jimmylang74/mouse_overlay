#!/bin/bash

echo "=== Building mouse_overlay.exe ==="

x86_64-w64-mingw32-gcc \
    main.c \
    mouse_hook.c \
    overlay_window.c \
    window_finder.c \
    -lgdi32 \
    -luser32 \
    -o mouse_overlay.exe

if [ $? -eq 0 ]; then
    echo "Build successful!"
else
    echo "Build failed!"
fi
