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
			/// GPUの使用モードを自動で判別します。
			/// </summary>
			Auto,
			/// <summary>
			/// GPUを使用せず、CPUのみで変換を行います。
			/// </summary>
			Disable
		};

		public enum class DenoiseLevel
		{
			None = 0,
			Level1 = 1,
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
				ConvertFileHelper(W2XConv* converter, String^ sourcePath, String^ distinationPath, DenoiseLevel denoiseLevel, double scale, int blockSize)
					: converter(converter), sourcePath(sourcePath), distinationPath(distinationPath), denoiseLevel(denoiseLevel), scale(scale), blockSize(blockSize) {}
				void ConvertFile();
			};


		public:
			/// <summary>
			/// waifu2xのアルゴリズムを用いて画像の拡大処理を行うクラスです。
			/// </summary>
			Waifu2xConverter(GpuMode gpuMode);
			~Waifu2xConverter();
			!Waifu2xConverter();

			System::Threading::Tasks::Task^ ConvertFileAsync(String^ sourcePath, String^ distinationPath, DenoiseLevel denoiseLevel, double Scale, int blockSize);
		};

	}
}
