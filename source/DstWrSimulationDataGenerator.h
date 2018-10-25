#ifndef DSTWR_SIMULATION_DATA_GENERATOR
#define DSTWR_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class DstWrAnalyzerSettings;

class DstWrSimulationDataGenerator
{
public:
	DstWrSimulationDataGenerator();
	~DstWrSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, DstWrAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	DstWrAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //DSTWR_SIMULATION_DATA_GENERATOR