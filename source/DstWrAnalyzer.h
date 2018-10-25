#ifndef DSTWR_ANALYZER_H
#define DSTWR_ANALYZER_H

#include <Analyzer.h>
#include "DstWrAnalyzerResults.h"
#include "DstWrSimulationDataGenerator.h"

class DstWrAnalyzerSettings;
class ANALYZER_EXPORT DstWrAnalyzer : public Analyzer2
{
public:
	DstWrAnalyzer();
	virtual ~DstWrAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< DstWrAnalyzerSettings > mSettings;
	std::auto_ptr< DstWrAnalyzerResults > mResults;

	DstWrSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //DSTWR_ANALYZER_H
