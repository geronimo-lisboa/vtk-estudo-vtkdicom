#pragma once
#include <Windows.h>
#define DLL_INTERFACE __declspec(dllexport)
extern "C"
{
	DLL_INTERFACE void _stdcall CreateScreen(HWND handle);
	DLL_INTERFACE int _stdcall MouseMove(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE int _stdcall LeftMouseDown(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE int _stdcall LeftMouseUp(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE int _stdcall MiddleMouseDown(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE int _stdcall MiddleMouseUp(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE int _stdcall RightMouseDown(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE int _stdcall RightMouseUp(HWND wnd, UINT nFlags, int X, int Y);
	DLL_INTERFACE void _stdcall Resize(int w, int h);
	DLL_INTERFACE void _stdcall Render();
}