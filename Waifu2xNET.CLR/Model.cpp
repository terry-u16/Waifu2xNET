#include "stdafx.hpp"
#include "Model.hpp"

using namespace System;
using namespace Waifu2xNET::CLR;

Waifu2xNET::CLR::StepParameter::StepParameter()
{
}

Waifu2xNET::CLR::Model::Model(cli::array<StepParameter^>^ stepParameters)
{
	StepParameters = stepParameters;
}

Model^ Waifu2xNET::CLR::Model::CreateFromJSON(String ^ json)
{
	auto model = gcnew Model(JsonConvert::DeserializeObject<cli::array<StepParameter^>^>(json, (JsonSerializerSettings^)nullptr));
	return model;
}
