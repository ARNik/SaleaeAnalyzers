#include "IMOIMIAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


IMOIMIAnalyzerSettings::IMOIMIAnalyzerSettings()
:	mInputChannel(UNDEFINED_CHANNEL),
	mVersion2(true),
	mRevBits(false)
{
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "IMO/IMI", "Toyota IMO/IMI analyzer" );
	mInputChannelInterface->SetChannel( mInputChannel );

	mVersionInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mVersionInterface->SetTitleAndTooltip("Protocol version", "");
	mVersionInterface->AddNumber(0, "Old version (16 bit)", "16 bits per packet, 48 bit/sec");
	mVersionInterface->AddNumber(1, "New version (32 bit)", "32 bits per packet, 195 bit/sec");
	mVersionInterface->SetNumber(1);

	mBitsOrderInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mBitsOrderInterface->SetTitleAndTooltip("Bits order", "");
	mBitsOrderInterface->AddNumber(0, "MSB first", "Don't reverse");
	mBitsOrderInterface->AddNumber(1, "LSB first", "Reverse bits in bytes");
	mBitsOrderInterface->SetNumber(0);

	AddInterface( mInputChannelInterface.get() );
	AddInterface(mVersionInterface.get() );
	AddInterface(mBitsOrderInterface.get());

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "IMO/IMI", false );
}

IMOIMIAnalyzerSettings::~IMOIMIAnalyzerSettings()
{
}

bool IMOIMIAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();
	mVersion2 = (mVersionInterface->GetNumber() != 0);
	mRevBits = (mBitsOrderInterface->GetNumber() != 0);

	ClearChannels();
	AddChannel( mInputChannel, "IMO/IMI", true );

	return true;
}

void IMOIMIAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel(mInputChannel);
	mVersionInterface->SetNumber (mVersion2);
	mBitsOrderInterface->SetNumber (mRevBits);
}

void IMOIMIAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mVersion2;
	text_archive >> mRevBits;

	ClearChannels();
	AddChannel(mInputChannel, "IMO/IMI", true);

	UpdateInterfacesFromSettings();
}

const char* IMOIMIAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mVersion2;
	text_archive << mRevBits;

	return SetReturnString( text_archive.GetString() );
}
