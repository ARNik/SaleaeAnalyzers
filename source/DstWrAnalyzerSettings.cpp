#include "DstWrAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


DstWrAnalyzerSettings::DstWrAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mBit1_max(1200), mBit1_min(800), mBit0_min(300),
	mMsbFirst(false)
{
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "Serial", "Standard Texas DST write" );
	mInputChannelInterface->SetChannel( mInputChannel );

	mBit1_maxLenInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBit1_maxLenInterface->SetTitleAndTooltip( "Bit HIGH max length [uS]",  "Specify max lenght of high bit" );
	mBit1_maxLenInterface->SetMax(5000);
	mBit1_maxLenInterface->SetMin(100);
	mBit1_maxLenInterface->SetInteger(mBit1_max);

	mBit1_minLenInterface.reset(new AnalyzerSettingInterfaceInteger());
	mBit1_minLenInterface->SetTitleAndTooltip("Bit HIGH min length / bit LOW max length [uS]", "Specify max lenght of high bit (same as min length of low bit)");
	mBit1_minLenInterface->SetMax(5000);
	mBit1_minLenInterface->SetMin(100);
	mBit1_minLenInterface->SetInteger(mBit1_min);

	mBit0_minLenInterface.reset(new AnalyzerSettingInterfaceInteger());
	mBit0_minLenInterface->SetTitleAndTooltip("Bit LOW min length [uS]", "Specify min lenght of low bit");
	mBit0_minLenInterface->SetMax(5000);
	mBit0_minLenInterface->SetMin(100);
	mBit0_minLenInterface->SetInteger(mBit0_min);

	mBitOrderInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mBitOrderInterface->SetTitleAndTooltip("Bit order", "");
	mBitOrderInterface->AddNumber(0, "LSB first", "Least significant bit first");
	mBitOrderInterface->AddNumber(1, "MSB first", "Most significant bit first");
	mBitOrderInterface->SetNumber(0);

	AddInterface( mInputChannelInterface.get() );
	AddInterface(mBit1_maxLenInterface.get() );
	AddInterface(mBit1_minLenInterface.get());
	AddInterface(mBit0_minLenInterface.get());
	AddInterface(mBitOrderInterface.get());

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "Serial", false );
}

DstWrAnalyzerSettings::~DstWrAnalyzerSettings()
{
}

bool DstWrAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();
	mBit1_max = mBit1_maxLenInterface->GetInteger();
	mBit1_min = mBit1_minLenInterface->GetInteger();
	mBit0_min = mBit0_minLenInterface->GetInteger();
	mMsbFirst = mBitOrderInterface->GetNumber() > 0;

	ClearChannels();
	AddChannel( mInputChannel, "Texas DST write", true );

	return true;
}

void DstWrAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel( mInputChannel );
	mBit1_maxLenInterface->SetInteger(mBit1_max);
	mBit1_minLenInterface->SetInteger(mBit1_min);
	mBit0_minLenInterface->SetInteger(mBit0_min);
	mBitOrderInterface->SetNumber(mMsbFirst ? 1 : 0);
}

void DstWrAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mBit1_max;
	text_archive >> mBit1_min;
	text_archive >> mBit0_min;
	text_archive >> mMsbFirst;

	ClearChannels();
	AddChannel( mInputChannel, "Texas DST write", true );

	UpdateInterfacesFromSettings();
}

const char* DstWrAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mBit1_max;
	text_archive << mBit1_min;
	text_archive << mBit0_min;
	text_archive << mMsbFirst;

	return SetReturnString( text_archive.GetString() );
}
