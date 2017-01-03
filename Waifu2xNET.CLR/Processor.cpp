#include "stdafx.hpp"
#include "Processor.hpp"

using namespace Waifu2xNET::CLR;

ProcessorType Processor::ProcessorType::get()
{
	switch (processor->type)
	{
	case W2XCONV_PROC_HOST:
		return CLR::ProcessorType::Host;
	case W2XCONV_PROC_CUDA:
		return CLR::ProcessorType::CUDA;
	case W2XCONV_PROC_OPENCL:
		return CLR::ProcessorType::OpenCL;
	default:
		throw gcnew System::InvalidOperationException("Processor::ProcessorType");
	}
}

ProcessorSubType Processor::ProcessorSubType::get()
{
	switch (processor->type)
	{
	case W2XCONV_PROC_HOST:
		switch (processor->sub_type)
		{
		case W2XCONV_PROC_HOST_OPENCV:
			return CLR::ProcessorSubType::Host_OpenCV;
		case W2XCONV_PROC_HOST_SSE3:
			return CLR::ProcessorSubType::Host_SSE3;
		case W2XCONV_PROC_HOST_AVX:
			return CLR::ProcessorSubType::Host_AVX;
		case W2XCONV_PROC_HOST_FMA:
			return CLR::ProcessorSubType::Host_FMA;
		case W2XCONV_PROC_HOST_NEON:
			return CLR::ProcessorSubType::Host_NEON;
		default:
			throw gcnew System::InvalidOperationException("Processor::ProcessorSubType");
		}

	case W2XCONV_PROC_CUDA:
		return CLR::ProcessorSubType::Cuda_nVidia;

	case W2XCONV_PROC_OPENCL:
		switch (processor->type)
		{
		case W2XCONV_PROC_OPENCL_NVIDIA_GPU:
			return CLR::ProcessorSubType::OpenCL_nVidia_GPU;
		case W2XCONV_PROC_OPENCL_AMD_CPU:
			return CLR::ProcessorSubType::OpenCL_AMD_CPU;
		case W2XCONV_PROC_OPENCL_AMD_GPU:
			return CLR::ProcessorSubType::OpenCL_AMD_GPU;
		case W2XCONV_PROC_OPENCL_INTEL_CPU:
			return CLR::ProcessorSubType::OpenCL_Intel_CPU;
		case W2XCONV_PROC_OPENCL_INTEL_GPU:
			return CLR::ProcessorSubType::OpenCL_Intel_GPU;
		case W2XCONV_PROC_OPENCL_UNKNOWN_CPU:
			return CLR::ProcessorSubType::OpenCL_Unknown_CPU;
		case W2XCONV_PROC_OPENCL_UNKNOWN_GPU:
			return CLR::ProcessorSubType::OpenCL_Unknown_GPU;
		default:
			return CLR::ProcessorSubType::OpenCL_Unknown;
		}
	default:
		throw gcnew System::InvalidOperationException("Processor::ProcessorSubType");
	}
}