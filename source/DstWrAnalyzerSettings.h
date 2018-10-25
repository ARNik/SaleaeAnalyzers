#ifndef DSTWR_ANALYZER_SETTINGS
#define DSTWR_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class DstWrAnalyzerSettings : public AnalyzerSettings
{
public:
	DstWrAnalyzerSettings();
	virtual ~DstWrAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mBit1_min, mBit1_max, mBit0_min;	// bit length, [uS]
	bool mMsbFirst;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBit1_maxLenInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBit1_minLenInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBit0_minLenInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mBitOrderInterface;
};

#endif //DSTWR_ANALYZER_SETTINGS
