#ifndef PROJECT_1_FONT_H
#define PROJECT_1_FONT_H
#include <iostream>
#include <GLFW/glfw3.h>
#include <cstring>


constexpr uint32_t kFontHeight{13};

extern GLubyte space[];
extern GLubyte rasters[][kFontHeight];

const GLubyte* getBitmapForCharacter(char c);
void print_string(const char *s);

#endif //PROJECT_1_FONT_H
