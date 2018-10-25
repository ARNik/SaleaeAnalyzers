#include "IMOIMIAnalyzer.h"
#include "IMOIMIAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

IMOIMIAnalyzer::IMOIMIAnalyzer()
:	Analyzer2(),  
	mSettings( new IMOIMIAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

IMOIMIAnalyzer::~IMOIMIAnalyzer()
{
	KillThread();
}

void IMOIMIAnalyzer::SetupResults()
{
	mResults.reset( new IMOIMIAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );
}

void IMOIMIAnalyzer::WorkerThread()
{
	using Marker = AnalyzerResults::MarkerType;

	const U64 sampleRateHz = GetSampleRate();
	const bool version2 = mSettings->mVersion2;
	const U64 bit_length_usec = version2 ? 5125 : 20500;
	const U32 bitLen = (U32)(sampleRateHz * bit_length_usec / 1000000);		// samples per bit
	Channel channel = mSettings->mInputChannel;
	AnalyzerChannelData* raw = GetAnalyzerChannelData(channel);

	auto addMarker = [&](Marker marker, U32 offset)
	{	mResults->AddMarker(raw->GetSampleNumber() + offset, marker, channel);	};

	auto nextBitValue = [&]() -> bool
	{
		const bool cur = (raw->GetBitState() == BIT_HIGH);
		if (raw->WouldAdvancingCauseTransition(bitLen / 2))
			return !cur;
		else
			return cur;
	};


	for( ; ; )
	{
		if (raw->GetBitState())
			raw->AdvanceToNextEdge();
		// low level at this point

		// search start pattern 
		// 24 (32) zero bits at version 2
		// 8 zero bits at version 1
		const U64 start_pulse_min = version2 ? 24 : 8;
		const U64 start_pulse_max = version2 ? 32 : 8;

		const U64 starting_sample = raw->GetSampleNumber();
		const U64 startPulseLen = raw->GetSampleOfNextEdge() - starting_sample;
		if (startPulseLen < (bitLen * (start_pulse_min - 1)) ||
			startPulseLen > (bitLen * (start_pulse_max + 1)) )
		{
			// skip pulse
			raw->AdvanceToNextEdge();
			raw->AdvanceToNextEdge();
			continue;
		}
	
		// start marker
		addMarker(AnalyzerResults::Start, 0);
		// sync marker
		addMarker(AnalyzerResults::Dot, (U32)(startPulseLen / 2));

		raw->AdvanceToNextEdge();
		// high level at this point
		
		// 11 (3) one bits at version2
		if (version2)
		{
			// total length should be 36 bits
			const U64 nextEdge = raw->GetSampleOfNextEdge();
			const U64 startPulseFull = nextEdge - starting_sample;
			const bool valid = 
				(startPulseFull > bitLen * (36 - 1)) &&
				(startPulseFull < bitLen * (36 + 1));
			
			// move cursor to sync pattern
			const U64 syncStart = nextEdge - bitLen;

			Marker marker = valid ? AnalyzerResults::Dot : AnalyzerResults::ErrorDot;
			addMarker(marker, (U32)(syncStart - raw->GetSampleNumber()) / 2);

			raw->AdvanceToAbsPosition(syncStart);
		}
		// collect data
		U32 value = 0;

		// sync pattern
		auto sync = [&]() -> bool
		{
			bool err = false;
			if (!nextBitValue())
				err = true;
			
			// check if sync pattern starting AFTER cursor
			if (!raw->GetBitState())
			{
				U64 prevBitLength = raw->GetSampleOfNextEdge() - raw->GetSampleNumber();
				if (prevBitLength < (bitLen / 4))
					raw->Advance((U32)prevBitLength);
			}

			// should be only one bit
			U64 syncOneLen = raw->GetSampleOfNextEdge() - raw->GetSampleNumber();
			if (syncOneLen < (bitLen * 3 / 4) ||
				syncOneLen > (bitLen * 5 / 4))
				err = true;

			Marker marker = !err ? AnalyzerResults::Square : AnalyzerResults::ErrorSquare;

			addMarker(marker, bitLen / 2);
			raw->Advance(bitLen / 2);
			raw->AdvanceToNextEdge();
			addMarker(marker, bitLen / 2);
			raw->Advance(bitLen);
			return err;
			// low level at this point
		};

		// collect data
		auto data = [&]()
		{
			for (U32 i = 0; i < 4; i++)
			{
				bool bit = nextBitValue();
				value <<= 1;
				if (bit)
					value |= 1;
				Marker marker = bit ? AnalyzerResults::One : AnalyzerResults::Zero;
				addMarker(marker, bitLen / 2);
				raw->Advance(bitLen);
			}
		};

		// data length - 32 bits at version2, 16 bits at version1
		const U32 group_cnt = version2 ? 8 : 4;

		bool syncError = false;
		for (U32 i = 0; i < group_cnt; i++)
		{
			if (syncError = sync())
				break;
			data();
		}

		// end marker
		const U64 ending_sample = raw->GetSampleNumber();
		addMarker(AnalyzerResults::Stop, 0);

		//we have a halfbyte to save. 
		Frame frame;
		frame.mData1 = value;
		frame.mFlags = syncError ? DISPLAY_AS_ERROR_FLAG : 0;
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = ending_sample;

		mResults->AddFrame(frame);
		mResults->CommitResults();
		ReportProgress(ending_sample);
	}
}

bool IMOIMIAnalyzer::NeedsRerun()
{
	return false;
}

U32 IMOIMIAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 IMOIMIAnalyzer::GetMinimumSampleRateHz()
{
	// minimum bit time - 5 msec
	return 200 * 5;
}

const char* IMOIMIAnalyzer::GetAnalyzerName() const
{
	return ::GetAnalyzerName();
}

const char* GetAnalyzerName()
{
	return "Toyota IMO/IMI analyzer";
}

Analyzer* CreateAnalyzer()
{
	return new IMOIMIAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}