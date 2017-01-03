#pragma once
#include "w2xconv.h"

namespace Waifu2xNET
{
	namespace CLR
	{
		/// <summary>
		/// 変換に用いるプロセッサーのタイプです。
		/// </summary>
		public enum class ProcessorType
		{
			/// <summary>
			/// ホストPCのCPU
			/// </summary>
			Host,
			/// <summary>
			/// CUDA
			/// </summary>
			CUDA,
			/// <summary>
			/// OpenCL
			/// </summary>
			OpenCL
		};

		/// <summary>
		/// 変換に用いるプロセッサーのサブタイプです。
		/// CUDA > OpenCL > FMA > AVX > OpenCV という優先順位で選択されます。
		/// </summary>
		public enum class ProcessorSubType
		{
			/// <summary>
			/// ホストPCのCPUによるOpenCV
			/// </summary>
			Host_OpenCV,
			/// <summary>
			/// ホストPCのCPUによるSSE3
			/// </summary>
			Host_SSE3,
			/// <summary>
			/// ホストPCのCPUによるAVX
			/// </summary>
			Host_AVX,
			/// <summary>
			/// ホストPCのCPUによるFMA
			/// </summary>
			Host_FMA,
			/// <summary>
			/// ホストPCのCPUによるNEON （ARMプロセッサ用）
			/// </summary>
			Host_NEON,
			/// <summary>
			/// nVidia製GPUによるCUDA
			/// </summary>
			Cuda_nVidia,
			/// <summary>
			/// nVidia製GPUによるOpenCL
			/// </summary>
			OpenCL_nVidia_GPU,
			/// <summary>
			/// AMD製CPUによるOpenCL
			/// </summary>
			OpenCL_AMD_CPU,
			/// <summary>
			/// AMD製GPUによるOpenCL
			/// </summary>
			OpenCL_AMD_GPU,
			/// <summary>
			/// Intel製CPUによるOpenCL
			/// </summary>
			OpenCL_Intel_CPU,
			/// <summary>
			/// Intel製GPUによるOpenCL
			/// </summary>
			OpenCL_Intel_GPU,
			/// <summary>
			/// 不明なCPUによるOpenCL
			/// </summary>
			OpenCL_Unknown_CPU,
			/// <summary>
			/// 不明なGPUによるOpenCL
			/// </summary>
			OpenCL_Unknown_GPU,
			/// <summary>
			/// 不明なプロセッサによるOpenCL
			/// </summary>
			OpenCL_Unknown
		};

		/// <summary>
		/// waifu2xの変換に用いるプロセッサーを表すクラスです。
		/// </summary>
		public ref class Processor
		{
			const W2XConvProcessor* processor;
		internal:
			Processor(const W2XConvProcessor* processor) : processor(processor) {};
		public:
			/// <summary>
			/// デバイスのID。
			/// </summary>
			property int DeviceID { int get() { return processor->dev_id; }}

			/// <summary>
			/// プロセッサのコア数。
			/// </summary>
			property int NumberOfCores { int get() { return processor->num_core; }}

			/// <summary>
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
			/// 変換に用いるプロセッサーのタイプ。
			/// </summary>
			property CLR::ProcessorType ProcessorType { CLR::ProcessorType get(); }

			/// <summary>
			/// 変換に用いるプロセッサーのサブタイプ。
			/// CUDA > OpenCL > FMA > AVX > OpenCV という優先順位で選択されます。
			/// </summary>
			property CLR::ProcessorSubType ProcessorSubType { CLR::ProcessorSubType get(); }

			// ネイティブのprocessor_listはstaticなのでデストラクタ・ファイナライザは定義しない
		};

	}
}

