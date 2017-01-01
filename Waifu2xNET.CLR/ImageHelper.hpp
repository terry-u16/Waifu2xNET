// Waifu2xNET.hpp

#pragma once

namespace Waifu2xNET
{
	namespace CLR
	{
		bool HasAlphaChannel(System::Windows::Media::Imaging::BitmapSource ^ image);
		System::Windows::Media::Imaging::WriteableBitmap^ GetAlphaChannel(System::Windows::Media::Imaging::BitmapSource ^ image);
		double BicubicWeightFunc(double d);
		System::Byte TrimByte(double x);
		System::Windows::Media::Imaging::WriteableBitmap ^ ResizeBicubicAlpha(System::Windows::Media::Imaging::WriteableBitmap ^ source, double scale);
		System::Windows::Media::Imaging::WriteableBitmap ^ ComposeAlpha(System::Windows::Media::Imaging::WriteableBitmap ^ bgr24, System::Windows::Media::Imaging::WriteableBitmap ^ alpha8);
	}
}

