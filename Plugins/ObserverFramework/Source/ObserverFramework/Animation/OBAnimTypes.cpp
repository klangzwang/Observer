#include "OBAnimTypes.h"

UOBAnimTypes::UOBAnimTypes()
{
	// Brow curves
	TArray<FName> BrowCurves = {
		"none"
	};
	// Eyes curves
	TArray<FName> EyeCurves = {
		"none"
	};
	// Blinks curves
	TArray<FName> BlinksCurves = {
		"none"
	};
	// Nose curves
	TArray<FName> NoseCurves = {
		"none"
	};
	// Cheeks curves
	TArray<FName> CheekCurves = {
		"none"
	};
	// Mouth curves
	TArray<FName> MouthCurves = {
		"none"
	};
	// Jaw curves
	TArray<FName> JawCurves = {
		"none"
	};

	for (const FName& CurveName : BrowCurves)
	{
		Brows.Add(CurveName, 0.0f);
	}
	for (const FName& CurveName : EyeCurves)
	{
		Eyes.Add(CurveName, 0.0f);
	}
	for (const FName& CurveName : BlinksCurves)
	{
		Blinks.Add(CurveName, 0.0f);
	}
	for (const FName& CurveName : NoseCurves)
	{
		Nose.Add(CurveName, 0.0f);
	}
	for (const FName& CurveName : CheekCurves)
	{
		Cheeks.Add(CurveName, 0.0f);
	}
	for (const FName& CurveName : MouthCurves)
	{
		Mouth.Add(CurveName, 0.0f);
	}
	for (const FName& CurveName : JawCurves)
	{
		Jaw.Add(CurveName, 0.0f);
	}
}
