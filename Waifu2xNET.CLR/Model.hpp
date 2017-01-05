#pragma once

#include "Stdafx.hpp"

namespace Waifu2xNET
{
	namespace CLR
	{
		using namespace Newtonsoft::Json;

		ref class StepParameter
		{
		public:
			StepParameter();
			[JsonProperty(PropertyName = "nInputPlane")]
			property int NumberOfInputPlane;
			[JsonProperty(PropertyName = "nOutputPlane")]
			property int NumberOfOutputPlane;
			[JsonProperty(PropertyName = "kW")]
			property int KernelWidth;
			[JsonProperty(PropertyName = "kH")]
			property int KernelHeight;
			[JsonProperty(PropertyName = "bias")]
			property cli::array<float>^ Bias;
			[JsonProperty(PropertyName = "weight")]
			property cli::array<float, 4>^ Weight;
		};

		ref class Model
		{
		private:
			Model(cli::array<StepParameter^>^ stepParameters);
		public:
			static Model^ CreateFromJSON(System::String^ json);
			property cli::array<StepParameter^>^ StepParameters;
			property int LayerDepth { int get() { return StepParameters->Length; }}
			property int NumberOfFirstInputPlane { int get() { return StepParameters[0]->NumberOfInputPlane; }}
			property cli::array<float>^ WeightList
			{
				cli::array<float>^ get()
				{
					auto length = gcnew cli::array<int>(StepParameters->Length);
					int lengthAll = 0;

					for (int i = 0; i < StepParameters->Length; i++)
					{
						length[i] = StepParameters[i]->Weight->Length;
						lengthAll += StepParameters[i]->Weight->Length;
					}

					auto weightList = gcnew cli::array<float>(lengthAll);
					int index = 0;

					for (int i = 0; i < StepParameters->Length; i++)
					{
						pin_ptr<float> destinationPointer = &weightList[index];
						pin_ptr<float> sourcePointer = &StepParameters[i]->Weight[0, 0, 0, 0];

						std::memcpy(destinationPointer, sourcePointer, sizeof(float) * length[i]);
						index += length[i];
					}

					return weightList;
				}
			}

			property cli::array<float>^ BiasList
			{
				cli::array<float>^ get()
				{
					auto length = gcnew cli::array<int>(StepParameters->Length);
					int lengthAll = 0;

					for (int i = 0; i < StepParameters->Length; i++)
					{
						length[i] = StepParameters[i]->Bias->Length;
						lengthAll += StepParameters[i]->Bias->Length;
					}

					auto biasList = gcnew cli::array<float>(lengthAll);
					int index = 0;

					for (int i = 0; i < StepParameters->Length; i++)
					{
						pin_ptr<float> destinationPointer = &biasList[index];
						pin_ptr<float> sourcePointer = &StepParameters[i]->Bias[0];

						std::memcpy(destinationPointer, sourcePointer, sizeof(float) * length[i]);
						index += length[i];
					}

					return biasList;
				}
			}

			property cli::array<int>^ NumberOfPlanesList
			{
				cli::array<int>^ get()
				{
					auto numberOfPlanesList = gcnew cli::array<int>(StepParameters->Length);

					for (int i = 0; i < StepParameters->Length; i++)
					{
						numberOfPlanesList[i] = StepParameters[i]->NumberOfOutputPlane;
					}

					return numberOfPlanesList;
				}
			}
		};

	}
}
