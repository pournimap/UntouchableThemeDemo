cls

del Source.exe

rc.exe Text.rc

cl.exe /c /EHsc Source.cpp -I "Include/" -I "resources/" -I "Include/glew/include/"

link.exe Source.obj Text.res user32.lib gdi32.lib kernel32.lib opengl32.lib "Include/glew/lib/Release/Win32/glew32.lib"

del Source.obj

Source.exe