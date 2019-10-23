#include "Input.h"

struct KeyData_t
{
	KeyData_t()
	{
		LeftMouseDown = false;
		LeftMouseClicked = false;
	}

	bool LeftMouseDown;
	bool LeftMouseClicked;

} KeyData;

Point MousePosition;

bool IsLeftMouseDown()
{
	return KeyData.LeftMouseDown;
}

bool IsLeftMouseClicked()
{
	bool ret = KeyData.LeftMouseClicked;
	KeyData.LeftMouseClicked = false;
	return ret;
}

Point GetMousePosition()
{
	return MousePosition;
}

bool OnMouseMoved(int x, int y)
{
	MousePosition.x = x;
	MousePosition.y = y;
	return true;
}

bool OnMouseClicked(int iMouseButton, bool bDown)
{
	if (iMouseButton == 0)
	{
		KeyData.LeftMouseClicked = IsLeftMouseDown() && !bDown;
		KeyData.LeftMouseDown = bDown;
		return true;
	}

	return false;
}