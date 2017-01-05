#pragma once
#include "w2xconv.h"

namespace Waifu2xNET
{
	namespace CLR
	{
		/// <summary>
		/// The processor type used by waifu2x.
		/// </summary>
		/// <summary xml:lang="ja">
		/// 変換に用いるプロセッサーのタイプです。
		/// </summary>
		public enum class ProcessorType
		{
			/// <summary>
			/// CPU on host PC.
			/// </summary>
			/// <summary xml:lang="ja">
			/// ホストPCのCPU
			/// </summary>
			Host,
			/// <summary>
			/// CUDA
			/// </summary>
			/// <summary xml:lang="ja">
			/// CUDA
			/// </summary>
			CUDA,
			/// <summary>
			/// OpenCL
			/// </summary>
			/// <summary xml:lang="ja">
			/// OpenCL
			/// </summary>
			OpenCL
		};

		/// <summary>
		/// The sub type of the processor used by waifu2x.
		/// Priority by default : CUDA > OpenCL > FMA > AVX > OpenCV
		/// </summary>
		/// <summary xml:lang="ja">
		/// 変換に用いるプロセッサーのサブタイプです。
		/// CUDA > OpenCL > FMA > AVX > OpenCV という優先順位で選択されます。
		/// </summary>
		public enum class ProcessorSubType
		{
			/// <summary>
			/// OpenCV by CPU on host PC.
			/// </summary>
			/// <summary xml:lang="ja">
			/// ホストPCのCPUによるOpenCV
			/// </summary>
			Host_OpenCV,
			/// <summary>
			/// SSE3 by CPU on host PC.
			/// </summary>
			/// <summary xml:lang="ja">
			/// ホストPCのCPUによるSSE3
			/// </summary>
			Host_SSE3,
			/// <summary>
			/// AVX by CPU on host PC
			/// </summary>
			/// <summary xml:lang="ja">
			/// ホストPCのCPUによるAVX
			/// </summary>
			Host_AVX,
			/// <summary>
			/// FMA by CPU on host PC
			/// </summary>
			/// <summary xml:lang="ja">
			/// ホストPCのCPUによるFMA
			/// </summary>
			Host_FMA,
			/// <summary>
			/// Neon by CPU on host PC (for ARM Processors)
			/// </summary>
			/// <summary xml:lang="ja">
			/// ホストPCのCPUによるNEON （ARMプロセッサ用）
			/// </summary>
			Host_NEON,
			/// <summary>
			/// CUDA by nVidia GPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// nVidia製GPUによるCUDA
			/// </summary>
			Cuda_nVidia,
			/// <summary>
			/// OpenCL by nVidia GPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// nVidia製GPUによるOpenCL
			/// </summary>
			OpenCL_nVidia_GPU,
			/// <summary>
			/// OpenCL by AMD CPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// AMD製CPUによるOpenCL
			/// </summary>
			OpenCL_AMD_CPU,
			/// <summary>
			/// OpenCL by AMD GPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// AMD製GPUによるOpenCL
			/// </summary>
			OpenCL_AMD_GPU,
			/// <summary>
			/// OpenCL by Intel CPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// Intel製CPUによるOpenCL
			/// </summary>
			OpenCL_Intel_CPU,
			/// <summary>
			/// OpenCL by Intel GPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// Intel製GPUによるOpenCL
			/// </summary>
			OpenCL_Intel_GPU,
			/// <summary>
			/// OpenCL by unknown CPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// 不明なCPUによるOpenCL
			/// </summary>
			OpenCL_Unknown_CPU,
			/// <summary>
			/// OpenCL by unknown GPU
			/// </summary>
			/// <summary xml:lang="ja">
			/// 不明なGPUによるOpenCL
			/// </summary>
			OpenCL_Unknown_GPU,
			/// <summary>
			/// OpenCL by unknown processor
			/// </summary>
			/// <summary xml:lang="ja">
			/// 不明なプロセッサによるOpenCL
			/// </summary>
			OpenCL_Unknown
		};

		/// <summary>
		/// The processor used by waifu2x.
		/// </summary>
		/// <summary xml:lang="ja">
		/// waifu2xの変換に用いるプロセッサーを表すクラスです。
		/// </summary>
		public ref class Processor
		{
			const W2XConvProcessor* processor;
		internal:
			Processor(const W2XConvProcessor* processor) : processor(processor) {};
		public:
			/// <summary>
			/// The device ID.
			/// </summary>
			/// <summary xml:lang="ja">
			/// デバイスのID。
			/// </summary>
			property int DeviceID { int get() { return processor->dev_id; }}

			/// <summary>
			/// The number of cores of the processor.
			/// </summary>
			/// <summary xml:lang="ja">
			/// プロセッサのコア数。
			/// </summary>
			property int NumberOfCores { int get() { return processor->num_core; }}

			/// <summary>
			/// The name of the device.
			/// </summary>
			/// <summary xml:lang="ja">
			/// デバイスの名称。
			/// </summary>
			property System::String^ DeviceName
			{
				System::String^ get() 
				{
					return msclr::interop::marshal_as<System::String^>(processor->dev_name);
				}
			}

			/// <summary>
			/// The processor type used by waifu2x.
			/// </summary>
			/// <summary xml:lang="ja">
			/// 変換に用いるプロセッサーのタイプ。
			/// </summary>
			property CLR::ProcessorType ProcessorType { CLR::ProcessorType get(); }

			/// <summary>
			/// The sub type of the processor used by waifu2x.
			/// Priority by default : CUDA > OpenCL > FMA > AVX > OpenCV
			/// </summary>
			/// <summary xml:lang="ja">
			/// 変換に用いるプロセッサーのサブタイプ。
			/// CUDA > OpenCL > FMA > AVX > OpenCV という優先順位で選択されます。
			/// </summary>
			property CLR::ProcessorSubType ProcessorSubType { CLR::ProcessorSubType get(); }

			// ネイティブのprocessor_listはstaticなのでデストラクタ・ファイナライザは定義しない
		};

	}
}

