#include "stdafx.hpp"
#include "ImageHelper.hpp"

using namespace System;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;

bool Waifu2xNET::CLR::HasAlphaChannel(BitmapSource^ image)
{
	return image->Format == PixelFormats::Bgra32 || image->Format == PixelFormats::Pbgra32
		|| image->Format == PixelFormats::Prgba128Float || image->Format == PixelFormats::Prgba64
		|| image->Format == PixelFormats::Rgba128Float || image->Format == PixelFormats::Rgba64;
}

WriteableBitmap ^ Waifu2xNET::CLR::GetAlphaChannel(BitmapSource ^ image)
{
	WriteableBitmap^ imageBgra32 = gcnew WriteableBitmap(gcnew FormatConvertedBitmap(image, PixelFormats::Bgra32, nullptr, 100));
	WriteableBitmap^ alphaChannel = gcnew WriteableBitmap(image->PixelWidth, image->PixelHeight,
		image->DpiX, image->DpiY, PixelFormats::Gray8, nullptr);

	int pixelLength = imageBgra32->PixelWidth * imageBgra32->PixelHeight;
	auto sourcePixels = (UInt32*)imageBgra32->BackBuffer.ToPointer();
	auto alphaPixels = (Byte*)alphaChannel->BackBuffer.ToPointer();

	for (int i = 0; i < pixelLength; i++)
	{
		UInt32 color = sourcePixels[i];
		auto alpha = (Byte)((color >> 24) & 0xff);
		alphaPixels[i] = alpha;
	}

	return alphaChannel;
}

double Waifu2xNET::CLR::BicubicWeightFunc(double d)
{
	if (d <= 1)
	{
		return 1 - 2 * d * d + d * d * d;
	}
	else if (d <= 2)
	{
		return 4 - 8 * d + 5 * d * d - d * d * d;
	}
	else
	{
		return 0;
	}
}

Byte Waifu2xNET::CLR::TrimByte(double x)
{
	return Math::Min(Math::Max(0, (int)Math::Round(x)), 255);
}

WriteableBitmap^ Waifu2xNET::CLR::ResizeBicubicAlpha(WriteableBitmap^ source, double scale)
{
	if (source->Format != PixelFormats::Gray8)
	{
		throw gcnew ArgumentException("Unsupported color format.");
	}

	double invScale = 1 / scale;
	auto result = gcnew WriteableBitmap((int)(source->PixelWidth * scale), (int)(source->PixelHeight * scale), 
		source->DpiX, source->DpiY, PixelFormats::Gray8, nullptr);
	auto sourceBackBuffer = (Byte*)source->BackBuffer.ToPointer();
	auto resultBackBuffer = (Byte*)result->BackBuffer.ToPointer();

	for (int iy = 0; iy < result->PixelHeight; iy++)
	{
		for (int ix = 0; ix < result->PixelWidth; ix++)
		{
			// å≥âÊëúÇ…Ç®ÇØÇÈëŒâûç¿ïW
			double xf = ix * invScale;
			double yf = iy * invScale;
			int x = (int)Math::Truncate(xf);
			int y = (int)Math::Truncate(yf);

			double alpha = 0;

			for (int jy = y - 1; jy <= y + 2; jy++)
			{
				for (int jx = x - 1; jx <= x + 2; jx++)
				{
					double weight = BicubicWeightFunc(Math::Abs(xf - jx)) * BicubicWeightFunc(Math::Abs(yf - jy));
					int sx = (jx < 0 || jx > source->PixelWidth) ? x : jx;
					int sy = (jy < 0 || jy > source->PixelHeight) ? y : jy;

					UInt32 sourcePos = sx + source->BackBufferStride * sy;
					alpha += sourceBackBuffer[sourcePos] * weight;
				}
			}

			size_t resultPos = ix + result->BackBufferStride * iy;

			resultBackBuffer[resultPos] = TrimByte(alpha);
		}
	}

	result->Freeze();
	return result;
}

WriteableBitmap^ Waifu2xNET::CLR::ComposeAlpha(WriteableBitmap^ bgr24, WriteableBitmap^ alpha8)
{
	if (bgr24->Format != PixelFormats::Bgr24 || alpha8->Format != PixelFormats::Gray8)
	{
		throw gcnew ArgumentException("Unsupported color format.");
	}

	auto result = gcnew WriteableBitmap(bgr24->PixelWidth, bgr24->PixelHeight, bgr24->DpiX, bgr24->DpiY, PixelFormats::Bgra32, nullptr);

	result->Lock();

	int size = result->PixelWidth * result->PixelHeight;
	auto bgr24BackBuffer = (Byte*)bgr24->BackBuffer.ToPointer();
	auto alpha8BackBuffer = (Byte*)alpha8->BackBuffer.ToPointer();
	auto resultBackBuffer = (UInt32*)result->BackBuffer.ToPointer();

	for (int i = 0; i < size; i++)
	{
		Byte b = bgr24BackBuffer[3 * i];
		Byte g = bgr24BackBuffer[3 * i + 1];
		Byte r = bgr24BackBuffer[3 * i + 2];
		Byte a = alpha8BackBuffer[i];
		resultBackBuffer[i] = (a << 24) | (r << 16) | (g << 8) | b;
	}

	result->AddDirtyRect(System::Windows::Int32Rect(0, 0, result->PixelWidth, result->PixelHeight));
	result->Unlock();
	result->Freeze();

	return result;
}