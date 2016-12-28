// これは メイン DLL ファイルです。

#include "stdafx.h"
#include "Waifu2xNET.h"

using namespace System::Threading::Tasks;
using namespace msclr::interop;
using namespace Waifu2xNET::CLR;

Waifu2xConverter::Waifu2xConverter(GpuMode gpuMode)
{
	converter = w2xconv_init(gpuMode == GpuMode::Auto ? W2XCONV_GPU_AUTO : W2XCONV_GPU_DISABLE, 0, 0);
	if (w2xconv_load_models(converter, "models") < 0)
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
	Console::WriteLine("Destructor is called.");
	this->!Waifu2xConverter();
}

Waifu2xConverter::!Waifu2xConverter()
{
	Console::WriteLine("Finalizer is called.");
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

System::Threading::Tasks::Task ^ Waifu2xConverter::ConvertFileAsync(String^ sourcePath, String^ distinationPath, DenoiseLevel denoiseLevel, double scale, int blockSize)
{
	auto helper = gcnew Waifu2xConverter::ConvertFileHelper(converter, sourcePath, distinationPath, denoiseLevel, scale, blockSize);
	return Task::Run(gcnew Action(helper, &Waifu2xConverter::ConvertFileHelper::ConvertFile));
}
