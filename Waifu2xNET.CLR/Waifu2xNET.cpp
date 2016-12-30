// これは メイン DLL ファイルです。

#include "stdafx.h"
#include "Waifu2xNET.h"

using namespace System::Threading::Tasks;
using namespace System::Drawing;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace msclr::interop;
using namespace Waifu2xNET::CLR;

Waifu2xConverter::Waifu2xConverter(GpuMode gpuMode, ConvertModel model)
{
	converter = w2xconv_init(gpuMode == GpuMode::Auto ? W2XCONV_GPU_AUTO : W2XCONV_GPU_DISABLE, 0, 0);
	convertModel = model;

	char* modelPath;

	switch (model)
	{
	case Waifu2xNET::CLR::ConvertModel::RGB:
		modelPath = "models_rgb";
		break;
	case Waifu2xNET::CLR::ConvertModel::YUV:
		modelPath = "models";
		break;
	default:
		throw gcnew ArgumentException("モデルのタイプが不正です。");
	}

	if (w2xconv_load_models(converter, modelPath) < 0)
	{
		String^ path;
		switch (converter->last_error.code)
		{
		case W2XCONV_ERROR_MODEL_LOAD_FAILED:
			path = marshal_as<String^>(converter->last_error.u.path);
			throw gcnew IO::FileLoadException(String::Format("モデルファイルの読み込みエラーが発生しました。 Path:{0}", path), path);
		default:
			throw gcnew InvalidOperationException(String::Format("不明なエラーが発生しました。 W2XConvErrorCode:{0}", (int)converter->last_error.code));
		}
	}
}

Waifu2xConverter::~Waifu2xConverter()
{
	this->!Waifu2xConverter();
}

Waifu2xConverter::!Waifu2xConverter()
{
	w2xconv_fini(converter);
}


void Waifu2xConverter::ConvertFileHelper::ConvertFile()
{
	marshal_context marshalContext;

	auto result = w2xconv_convert_file(converter, 
		marshalContext.marshal_as<const char*>(distinationPath), 
		marshalContext.marshal_as<const char*>(sourcePath), 
		(int)denoiseLevel, scale, blockSize);

	if (result < 0)
	{
		String^ path;
		switch (converter->last_error.code)
		{
		case W2XCONV_ERROR_IMREAD_FAILED:
			path = marshal_as<String^>(converter->last_error.u.path);
			throw gcnew IO::FileLoadException(String::Format("ファイルの読み込みエラーが発生しました。 Path:{0}", path), path);
		case W2XCONV_ERROR_IMWRITE_FAILED:
			path = marshal_as<String^>(converter->last_error.u.path);
			throw gcnew IO::IOException(String::Format("ファイルの書き込みエラーが発生しました。 Path:{0}", path));
		default:
			throw gcnew InvalidOperationException(String::Format("不明なエラーが発生しました。 W2XConvErrorCode:{0}", (int)converter->last_error.code));
		}
	}
}

Task ^ Waifu2xConverter::ConvertFileAsync(String^ sourcePath, String^ distinationPath, DenoiseLevel denoiseLevel, double scale)
{
	return ConvertFileAsync(sourcePath, distinationPath, denoiseLevel, scale, 0);
}


Task ^ Waifu2xConverter::ConvertFileAsync(String^ sourcePath, String^ distinationPath, DenoiseLevel denoiseLevel, double scale, int blockSize)
{
	auto helper = gcnew Waifu2xConverter::ConvertFileHelper(converter, sourcePath, distinationPath, denoiseLevel, scale, blockSize);
	return Task::Run(gcnew Action(helper, &Waifu2xConverter::ConvertFileHelper::ConvertFile));
}

WriteableBitmap^ Waifu2xConverter::ConvertHelper::Convert()
{
	auto sourceBGR24 = gcnew WriteableBitmap(gcnew FormatConvertedBitmap(source, System::Windows::Media::PixelFormats::Bgr24, nullptr, 0));
	auto resultBGR24 = gcnew WriteableBitmap((int)(source->PixelWidth * scale), (int)(source->PixelHeight * scale), source->DpiX, source->DpiY, PixelFormats::Bgr24, nullptr);

	resultBGR24->Lock();
	pin_ptr<Byte> sourceBytes = (Byte*)sourceBGR24->BackBuffer.ToPointer();
	pin_ptr<Byte> resultBytes = (Byte*)resultBGR24->BackBuffer.ToPointer();

	w2xconv_convert_rgb(converter, resultBytes, resultBGR24->BackBufferStride, sourceBytes, sourceBGR24->BackBufferStride,
		source->PixelWidth, source->PixelHeight, (int)denoiseLevel, scale, blockSize);
	resultBGR24->AddDirtyRect(System::Windows::Int32Rect(0, 0, resultBGR24->PixelWidth, resultBGR24->PixelHeight));
	resultBGR24->Unlock();
	resultBGR24->Freeze();

	return resultBGR24;
}

Task<WriteableBitmap^>^ Waifu2xConverter::ConvertAsync(BitmapSource^ source, DenoiseLevel denoiseLevel, double scale)
{
	return ConvertAsync(source, denoiseLevel, scale, 0);
}

Task<WriteableBitmap^>^ Waifu2xConverter::ConvertAsync(BitmapSource^ source, DenoiseLevel denoiseLevel, double scale, int blockSize)
{
	auto helper = gcnew Waifu2xConverter::ConvertHelper(converter, source, denoiseLevel, scale, blockSize);
	return Task<WriteableBitmap^>::Run(gcnew Func<WriteableBitmap^>(helper, &Waifu2xConverter::ConvertHelper::Convert));
}

