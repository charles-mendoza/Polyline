#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "Structures.h"

bool IsLeftMouseDown();
bool IsLeftMouseClicked();
Point GetMousePosition();
bool OnMouseMoved(int x, int y);
bool OnMouseClicked(int iButton, bool bDown);

#endif