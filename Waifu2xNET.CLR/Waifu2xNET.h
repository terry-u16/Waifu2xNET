// Waifu2xNET.h

#pragma once

using namespace System;

namespace Waifu2xNET {
	namespace CLR
	{
		/// <summary>
		/// 画像拡大時のGPU使用モードを表します。
		/// </summary>
		public enum class GpuMode
		{
			/// <summary>
			/// GPUの使用モードを自動で判別します。（推奨）
			/// </summary>
			Auto,
			/// <summary>
			/// OpenCLを使用して変換を行います。
			/// </summary>
			ForceOpenCL,
			/// <summary>
			/// GPUを使用せず、CPUのみで変換を行います。
			/// </summary>
			Disable
		};

		/// <summary>
		/// ノイズ除去フィルタの強さを表します。
		/// </summary>
		public enum class DenoiseLevel
		{
			/// <summary>
			/// ノイズ除去なし（PNGファイル向け）
			/// </summary>
			None = 0,
			/// <summary>
			/// ノイズ除去弱（一般のJPEGファイル向け）
			/// </summary>
			Level1 = 1,
			/// <summary>
			/// ノイズ除去強（ノイズの多いJPEGファイル向け）
			/// </summary>
			Level2 = 2
		};

		/// <summary>
		/// waifu2xのアルゴリズムを用いて画像の拡大処理を行うクラスです。
		/// </summary>
		public ref class Waifu2xConverter
		{
		private:
			W2XConv* converter;

			ref class ConvertFileHelper
			{
			private:
				W2XConv* converter;
				String^ sourcePath;
				String^ distinationPath;
				DenoiseLevel denoiseLevel;
				double scale;
				int blockSize;
			public:
				ConvertFileHelper(W2XConv* converter, String^ sourcePath, String^ destinationPath, DenoiseLevel denoiseLevel, double scale, int blockSize)
					: converter(converter), sourcePath(sourcePath), distinationPath(destinationPath), denoiseLevel(denoiseLevel), scale(scale), blockSize(blockSize) {}
				void ConvertFile();
			};

			ref class ConvertHelper
			{
			private:
				W2XConv* converter;
				System::Windows::Media::Imaging::BitmapSource^ source;
				DenoiseLevel denoiseLevel;
				double scale;
				int blockSize;
			public:
				ConvertHelper(W2XConv* converter, System::Windows::Media::Imaging::BitmapSource^ source, DenoiseLevel denoiseLevel, double scale, int blockSize)
					: converter(converter), source(source), denoiseLevel(denoiseLevel), scale(scale), blockSize(blockSize) {}
				System::Windows::Media::Imaging::WriteableBitmap^ Convert();

			};

		public:
			Waifu2xConverter(GpuMode gpuMode);
			~Waifu2xConverter();
			!Waifu2xConverter();

			System::Threading::Tasks::Task^ ConvertFileAsync(String^ sourcePath, String^ destinationPath, DenoiseLevel denoiseLevel, double scale);
			System::Threading::Tasks::Task^ ConvertFileAsync(String^ sourcePath, String^ destinationPath, DenoiseLevel denoiseLevel, double scale, int blockSize);
			System::Threading::Tasks::Task<System::Windows::Media::Imaging::WriteableBitmap^>^ ConvertAsync(System::Windows::Media::Imaging::BitmapSource^ source, DenoiseLevel denoiseLevel, double scale);
			System::Threading::Tasks::Task<System::Windows::Media::Imaging::WriteableBitmap^>^ ConvertAsync(System::Windows::Media::Imaging::BitmapSource^ source, DenoiseLevel denoiseLevel, double scale, int blockSize);
		};

	}
}
