// これは メイン DLL ファイルです。

#include "stdafx.hpp"
#include "Waifu2xNET.hpp"
#include "ImageHelper.hpp"

using namespace System::Threading::Tasks;
using namespace System::Drawing;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace Runtime::InteropServices;
using namespace msclr::interop;
using namespace Waifu2xNET::CLR;

/// <summary>
/// 画像の拡大に用いる<c>Waifu2xConverter</c>クラスのインスタンスを生成します。
/// このクラスはIDisposableを継承しているため、using句またはDisposeメソッドでリソースの解放が必要です。
/// </summary>
/// <param name="gpuMode">画像拡大時のGPU使用モード</param>
/// <returns>Waifu2xConverterクラスのインスタンス</returns>
/// <exception cref="System::IO::FileLoadException"/>
/// <exception cref="System::InvalidOperationException"/>
Waifu2xConverter::Waifu2xConverter(GpuMode gpuMode)
{
	W2XConvGPUMode nativeGPUMode;

	switch (gpuMode)
	{
	case Waifu2xNET::CLR::GpuMode::Auto:
		nativeGPUMode = W2XCONV_GPU_AUTO;
		break;
	case Waifu2xNET::CLR::GpuMode::ForceOpenCL:
		nativeGPUMode = W2XCONV_GPU_FORCE_OPENCL;
		break;
	case Waifu2xNET::CLR::GpuMode::Disable:
		nativeGPUMode = W2XCONV_GPU_DISABLE;
		break;
	default:
		break;
	}

	converter = w2xconv_init(nativeGPUMode, 0, 0);

	char* modelPath = "models_rgb";

	if (w2xconv_load_models(converter, modelPath) < 0)
	{
		String^ path;
		switch (converter->last_error.code)
		{
		case W2XCONV_ERROR_MODEL_LOAD_FAILED:
			path = marshal_as<String^>(converter->last_error.u.path);
			throw gcnew IO::FileLoadException(String::Format("モデルファイルの読み込みエラーが発生しました。 Path:{0}", path), path);
		case W2XCONV_ERROR_OPENCL:
			throw gcnew InvalidOperationException(String::Format("OpenCLのエラーが発生しました。 Error code:{0}, Dev ID:{1}",
				converter->last_error.u.cl_error.error_code, converter->last_error.u.cl_error.dev_id));
		default:
			throw gcnew InvalidOperationException(String::Format("不明なエラーが発生しました。 W2XConvErrorCode:{0}", (int)converter->last_error.code));
		}
	}
}

/// <summary>
/// <c>Waifu2xConverter</c>クラスの持つアンマネージドリソースの解放を行います。
/// </summary>
Waifu2xConverter::~Waifu2xConverter()
{
	this->!Waifu2xConverter();
}

/// <summary>
/// <c>Waifu2xConverter</c>クラスのファイナライザです。
/// </summary>
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
		case W2XCONV_ERROR_OPENCL:
			throw gcnew InvalidOperationException(String::Format("OpenCLのエラーが発生しました。 Error code:{0}, Dev ID:{1}",
				converter->last_error.u.cl_error.error_code, converter->last_error.u.cl_error.dev_id));
		default:
			throw gcnew InvalidOperationException(String::Format("不明なエラーが発生しました。 W2XConvErrorCode:{0}", (int)converter->last_error.code));
		}
	}
}

/// <summary>
/// 入力ファイルおよび出力ファイルのパスを指定し、画像の拡大を非同期に行います。
/// </summary>
/// <param name="sourcePath">変換元画像のパス</param>
/// <param name="destinationPath">変換後ファイルの保存先パス</param>
/// <param name="denoiseLevel">ノイズ除去の強さ</param>
/// <param name="scale">拡大率</param>
/// <exception cref="System::IO::IOException"/>
/// <exception cref="System::IO::FileLoadException"/>
/// <exception cref="System::InvalidOperationException"/>
Task ^ Waifu2xConverter::ConvertFileAsync(String^ sourcePath, String^ destinationPath, DenoiseLevel denoiseLevel, double scale)
{
	return ConvertFileAsync(sourcePath, destinationPath, denoiseLevel, scale, 0);
}

/// <summary>
/// 入力ファイルおよび出力ファイルのパスを指定し、画像の拡大を非同期に行います。
/// </summary>
/// <param name="sourcePath">変換元画像のパス</param>
/// <param name="destinationPath">変換後ファイルの保存先パス</param>
/// <param name="denoiseLevel">ノイズ除去の強さ</param>
/// <param name="scale">拡大率</param>
/// <param name="blockSize">変換時のブロックサイズ。0を渡すと自動で設定されます。</param>
/// <exception cref="System::IO::IOException"/>
/// <exception cref="System::IO::FileLoadException"/>
/// <exception cref="System::InvalidOperationException"/>
Task ^ Waifu2xConverter::ConvertFileAsync(String^ sourcePath, String^ destinationPath, DenoiseLevel denoiseLevel, double scale, int blockSize)
{
	auto helper = gcnew Waifu2xConverter::ConvertFileHelper(converter, sourcePath, destinationPath, denoiseLevel, scale, blockSize);
	return Task::Run(gcnew Action(helper, &Waifu2xConverter::ConvertFileHelper::ConvertFile));
}

WriteableBitmap^ Waifu2xConverter::ConvertHelper::Convert()
{
	auto sourceBGR24 = gcnew WriteableBitmap(gcnew FormatConvertedBitmap(source, PixelFormats::Bgr24, nullptr, 100));
	auto resultBGR24 = gcnew WriteableBitmap((int)(source->PixelWidth * scale), (int)(source->PixelHeight * scale), source->DpiX, source->DpiY, PixelFormats::Bgr24, nullptr);

	WriteableBitmap^ resultAlpha;
	bool hasAlpha = HasAlphaChannel(source);

	if (hasAlpha)
	{
		resultAlpha = ResizeBicubicAlpha(GetAlphaChannel(source), scale);
	}

	resultBGR24->Lock();
	pin_ptr<Byte> sourceBytes = (Byte*)sourceBGR24->BackBuffer.ToPointer();
	pin_ptr<Byte> resultBytes = (Byte*)resultBGR24->BackBuffer.ToPointer();

	auto result = w2xconv_convert_rgb(converter, resultBytes, resultBGR24->BackBufferStride, sourceBytes, sourceBGR24->BackBufferStride,
		source->PixelWidth, source->PixelHeight, (int)denoiseLevel, scale, blockSize);

	if (result < 0)
	{
		switch (converter->last_error.code)
		{
		case W2XCONV_ERROR_OPENCL:
			throw gcnew InvalidOperationException(String::Format("OpenCLのエラーが発生しました。 Error code:{0}, Dev ID:{1}", 
				converter->last_error.u.cl_error.error_code, converter->last_error.u.cl_error.dev_id));
		default:
			throw gcnew InvalidOperationException(String::Format("不明なエラーが発生しました。 W2XConvErrorCode:{0}", (int)converter->last_error.code));
		}
	}

	resultBGR24->AddDirtyRect(System::Windows::Int32Rect(0, 0, resultBGR24->PixelWidth, resultBGR24->PixelHeight));
	resultBGR24->Unlock();
	resultBGR24->Freeze();

	return !hasAlpha ? resultBGR24 : ComposeAlpha(resultBGR24, resultAlpha);
}

/// <summary>
/// 変換元画像を指定し、画像の拡大を非同期に行います。
/// </summary>
/// <param name="source">変換元画像のパス</param>
/// <param name="denoiseLevel">ノイズ除去の強さ</param>
/// <param name="scale">拡大率</param>
/// <returns>変換後の画像</returns>
Task<WriteableBitmap^>^ Waifu2xConverter::ConvertAsync(BitmapSource^ source, DenoiseLevel denoiseLevel, double scale)
{
	return ConvertAsync(source, denoiseLevel, scale, 0);
}

/// <summary>
/// 変換元画像を指定し、画像の拡大を非同期に行います。
/// </summary>
/// <param name="source">変換元画像のパス</param>
/// <param name="denoiseLevel">ノイズ除去の強さ</param>
/// <param name="scale">拡大率</param>
/// <param name="blockSize">変換時のブロックサイズ。0を渡すと自動で設定されます。</param>
/// <returns>変換後の画像</returns>
Task<WriteableBitmap^>^ Waifu2xConverter::ConvertAsync(BitmapSource^ source, DenoiseLevel denoiseLevel, double scale, int blockSize)
{
	auto helper = gcnew Waifu2xConverter::ConvertHelper(converter, source, denoiseLevel, scale, blockSize);
	return Task<WriteableBitmap^>::Run(gcnew Func<WriteableBitmap^>(helper, &Waifu2xConverter::ConvertHelper::Convert));
}