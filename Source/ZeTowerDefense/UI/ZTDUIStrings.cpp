#include "ZTDUIStrings.h"
#include "Internationalization/Text.h"

// String templates
const FString UZTDUIStrings::WavePauseTitleTemplate = TEXT("Wave {0}");
const FString UZTDUIStrings::WavePauseEnemiesTemplate = TEXT("Enemies: {0} Tanks, {1} Helis");
const FString UZTDUIStrings::WavePauseInstructionTemplate = TEXT("Press C to start the wave");
const FString UZTDUIStrings::BaseHPTemplate = TEXT("Base HP: {0}/{1}");
const FString UZTDUIStrings::PointsTemplate = TEXT("Points: {0}");
const FString UZTDUIStrings::WaveNumberTemplate = TEXT("Wave: {0}");
const FString UZTDUIStrings::BuildPhaseTemplate = TEXT("Build Phase - R for Menu");
const FString UZTDUIStrings::WaveInProgressTemplate = TEXT("Wave In Progress");
const FString UZTDUIStrings::WaveSummaryTemplate = TEXT("Wave Complete - Press C");
const FString UZTDUIStrings::PausedTemplate = TEXT("Game Paused");
const FString UZTDUIStrings::GameOverTemplate = TEXT("Game Over");
const FString UZTDUIStrings::BuildTankTemplate = TEXT("Build Tank ({0})");
const FString UZTDUIStrings::BuildHeliTemplate = TEXT("Build Heli ({0})");
const FString UZTDUIStrings::CancelTemplate = TEXT("Cancel");

FString UZTDUIStrings::GetWavePauseTitle(int32 WaveNumber)
{
	return FString::Format(*WavePauseTitleTemplate, {WaveNumber});
}

FString UZTDUIStrings::GetWavePauseEnemiesText(int32 TankCount, int32 HeliCount)
{
	return FString::Format(*WavePauseEnemiesTemplate, {TankCount, HeliCount});
}

FString UZTDUIStrings::GetWavePauseInstruction()
{
	return WavePauseInstructionTemplate;
}

FString UZTDUIStrings::GetBaseHPText(float CurrentHP, float MaxHP)
{
	return FString::Format(*BaseHPTemplate, {FString::SanitizeFloat(CurrentHP), FString::SanitizeFloat(MaxHP)});
}

FString UZTDUIStrings::GetPointsText(int32 Points)
{
	return FString::Format(*PointsTemplate, {Points});
}

FString UZTDUIStrings::GetWaveNumberText(int32 WaveNumber)
{
	return FString::Format(*WaveNumberTemplate, {WaveNumber});
}

FString UZTDUIStrings::GetBuildPhaseText()
{
	return BuildPhaseTemplate;
}

FString UZTDUIStrings::GetWaveInProgressText()
{
	return WaveInProgressTemplate;
}

FString UZTDUIStrings::GetWaveSummaryText()
{
	return WaveSummaryTemplate;
}

FString UZTDUIStrings::GetPausedText()
{
	return PausedTemplate;
}

FString UZTDUIStrings::GetGameOverText()
{
	return GameOverTemplate;
}

FString UZTDUIStrings::GetBuildTankText(int32 Cost)
{
	return FString::Format(*BuildTankTemplate, {Cost});
}

FString UZTDUIStrings::GetBuildHeliText(int32 Cost)
{
	return FString::Format(*BuildHeliTemplate, {Cost});
}

FString UZTDUIStrings::GetCancelText()
{
	return CancelTemplate;
}
