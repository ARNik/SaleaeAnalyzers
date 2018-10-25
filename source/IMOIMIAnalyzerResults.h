#ifndef IMOIMI_ANALYZER_RESULTS
#define IMOIMI_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class IMOIMIAnalyzer;
class IMOIMIAnalyzerSettings;

class IMOIMIAnalyzerResults : public AnalyzerResults
{
public:
	IMOIMIAnalyzerResults( IMOIMIAnalyzer* analyzer, IMOIMIAnalyzerSettings* settings );
	virtual ~IMOIMIAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	IMOIMIAnalyzerSettings* mSettings;
	IMOIMIAnalyzer* mAnalyzer;
};

#endif //IMOIMI_ANALYZER_RESULTS
