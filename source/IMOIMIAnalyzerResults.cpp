#include "IMOIMIAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "IMOIMIAnalyzer.h"
#include "IMOIMIAnalyzerSettings.h"
#include <iostream>
#include <fstream>

IMOIMIAnalyzerResults::IMOIMIAnalyzerResults( IMOIMIAnalyzer* analyzer, IMOIMIAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

IMOIMIAnalyzerResults::~IMOIMIAnalyzerResults()
{
}

void IMOIMIAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	U64 val = frame.mData1;
	if (mSettings->mRevBits)
		val = reverseBits(val);

	char number_str[128];
	AnalyzerHelpers::GetNumberString (val, display_base, 8, number_str, sizeof(number_str) );
	AddResultString( number_str );
}

void IMOIMIAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	const U64 trigger_sample = mAnalyzer->GetTriggerSample();
	const U32 sample_rate = mAnalyzer->GetSampleRate();

	std::ofstream file_stream(file, std::ios::out);
	file_stream << "Time [s],Value" << std::endl;

	const U64 num_frames = GetNumFrames();
	for (U32 i = 0; i < num_frames; i++)
	{
		Frame frame = GetFrame (i);
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString (frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, sizeof(time_str));

		U64 val = frame.mData1;
		if (mSettings->mRevBits)
			val = reverseBits(val);

		char number_str[128];
		AnalyzerHelpers::GetNumberString (val, display_base, 8, number_str, sizeof(number_str));

		file_stream << time_str << "," << number_str << std::endl;

		if (UpdateExportProgressAndCheckForCancel(i, num_frames))
			break;
	}

	file_stream.close();
}

void IMOIMIAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	U64 val = frame.mData1;
	if (mSettings->mRevBits)
		val = reverseBits(val);

	char number_str[128];
	AnalyzerHelpers::GetNumberString(val, display_base, 8, number_str, sizeof(number_str));
	AddTabularText( number_str );
#endif
}

void IMOIMIAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void IMOIMIAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}

U64 IMOIMIAnalyzerResults::reverseBits(U64 val)
{
	auto rev8 = [](uint8_t b) -> uint8_t
	{
		// http://graphics.stanford.edu/~seander/bithacks.html
		return (uint8_t)(((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
	};

	U64 res = 0;
	for (U32 i = 0; i < 64; i += 8)
		res |= (U64)rev8(val >> i) << i;

	return res;
}
