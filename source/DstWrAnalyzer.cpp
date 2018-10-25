#include "DstWrAnalyzer.h"
#include "DstWrAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

DstWrAnalyzer::DstWrAnalyzer()
:	Analyzer2(),  
	mSettings( new DstWrAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

DstWrAnalyzer::~DstWrAnalyzer()
{
	KillThread();
}

void DstWrAnalyzer::SetupResults()
{
	mResults.reset( new DstWrAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );
}

void DstWrAnalyzer::WorkerThread()
{
	const U32 sampleRateHz = GetSampleRate();

	//const U32 samples_per_bit = mSampleRateHz / mSettings->mBitRate;
	//const U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings->mBitRate ) );
	const U32 bit1_min = (mSettings->mBit1_min * (U64)sampleRateHz) / 1000000;			// bit length, [samples]
	const U32 bit1_max = (mSettings->mBit1_max * (U64)sampleRateHz) / 1000000;
	const U32 bit0_min = (mSettings->mBit0_min * (U64)sampleRateHz) / 1000000;

	AnalyzerChannelData * raw = GetAnalyzerChannelData(mSettings->mInputChannel);

	//if (raw->GetBitState() == BIT_LOW )
	//	raw->AdvanceToNextEdge();
	// 0x0001F9
	//   000004B0


	for( ; ; )
	{
		U8 data = 0;
		bool error = false;

		if (! raw->GetBitState())
			raw->AdvanceToNextEdge();
		// high level now

		const U64 starting_sample = raw->GetSampleNumber();
		mResults->AddMarker(starting_sample, AnalyzerResults::Start, mSettings->mInputChannel);

		for (U32 bitNo = 0; bitNo < 8; bitNo++ )
		{
			const U64 edgeR1 = raw->GetSampleNumber();		// rising edge
			raw->AdvanceToNextEdge();
			const U64 edgeF1 = raw->GetSampleNumber();		// falling edge
			U64 lenH = edgeF1 - edgeR1;
			// low level now

			if (lenH > bit1_max / 2)
			{
				raw->AdvanceToNextEdge();
				// high level now
				error = true;
				// next byte cycle
				break;
			}

			U64 lenL;
			lenL = raw->GetSampleOfNextEdge() - edgeF1;
			if (lenL > lenH)
				lenL = lenH;

			raw->Advance((U32)lenL - 1);
			// low level still going

			// skip spikes
			//const U64 lenH2 = raw->GetSampleOfNextEdge() - raw->GetSampleNumber();
			//if (lenH2 >= bit0_min / 10)
			//	break;

			if (bitNo < 7)	// not last bit
				raw->AdvanceToNextEdge();
			// high level now

			if ((lenL + lenH < bit0_min) ||
				(lenL + lenH > bit1_max) ||
				(lenL > bit1_min/2 && lenH < bit1_min/2) ||
				(lenL < bit1_min/2 && lenH > bit1_min/2) )
			{
				error = true;
				break;
			}

			const bool dataBit = lenH + lenL > bit1_min;
			const int offset = (mSettings->mMsbFirst) ? (7 - bitNo) : bitNo;
			if (dataBit)
				data |= 1ULL << offset;

			mResults->AddMarker((edgeR1 + edgeF1)/2, 
				dataBit ? AnalyzerResults::One : AnalyzerResults::Zero, 
				mSettings->mInputChannel);

		}	// for (bitNo < 8)

		if (!error)
		{
			//we have a data to save. 
			Frame frame;
			frame.mData1 = data;
			frame.mFlags = 0;
			frame.mStartingSampleInclusive = starting_sample;
			frame.mEndingSampleInclusive = raw->GetSampleNumber();

			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);
		}
	}
}

bool DstWrAnalyzer::NeedsRerun()
{
	return false;
}

U32 DstWrAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 DstWrAnalyzer::GetMinimumSampleRateHz()
{
	return 4 * 1000000 / mSettings->mBit0_min;
}

const char* DstWrAnalyzer::GetAnalyzerName() const
{
	return "Texas DST write";
}

const char* GetAnalyzerName()
{
	return "Texas DST write";
}

Analyzer* CreateAnalyzer()
{
	return new DstWrAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}