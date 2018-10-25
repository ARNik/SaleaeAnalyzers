#ifndef IMOIMI_SIMULATION_DATA_GENERATOR
#define IMOIMI_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class IMOIMIAnalyzerSettings;

class IMOIMISimulationDataGenerator
{
public:
	IMOIMISimulationDataGenerator();
	~IMOIMISimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, IMOIMIAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	IMOIMIAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //IMOIMI_SIMULATION_DATA_GENERATOR