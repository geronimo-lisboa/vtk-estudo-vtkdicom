#include "interfaceDll.h"
#include <iostream>
#include <sstream>

#define PrintPosition {	std::stringstream ss;ss << GetCurrentTime() << "," << __FUNCTION__ << ", " << __LINE__;std::cout << ss.str() << std::endl; }

void __stdcall CreateScreen(HWND handle){
	PrintPosition;
	std::cout << __FUNCTION__ << ", " << __LINE__ << std::endl;
}
int _stdcall MouseMove(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
int _stdcall LeftMouseDown(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
int _stdcall LeftMouseUp(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
int _stdcall MiddleMouseDown(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
int _stdcall MiddleMouseUp(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
int _stdcall RightMouseDown(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
int _stdcall RightMouseUp(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	return 0;
}
void _stdcall Resize(int w, int h){
	PrintPosition;
}
void _stdcall Render(){
	PrintPosition;
}