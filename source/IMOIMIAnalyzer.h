#ifndef IMOIMI_ANALYZER_H
#define IMOIMI_ANALYZER_H

#include <Analyzer.h>
#include "IMOIMIAnalyzerResults.h"
#include "IMOIMISimulationDataGenerator.h"

class IMOIMIAnalyzerSettings;
class ANALYZER_EXPORT IMOIMIAnalyzer : public Analyzer2
{
public:
	IMOIMIAnalyzer();
	virtual ~IMOIMIAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< IMOIMIAnalyzerSettings > mSettings;
	std::auto_ptr< IMOIMIAnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	IMOIMISimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //IMOIMI_ANALYZER_H
