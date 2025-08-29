#pragma once
#include <Windows.h>

namespace AutoAccept
{
	void StartAutoAccept();
	void StopAutoAccept();
	bool DetectAcceptButton();
	void ClickAcceptButton(int x, int y);
	bool IsGreenPixel(COLORREF color);
	void UpdateAutoAccept();
}