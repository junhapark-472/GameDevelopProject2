// Fill out your copyright notice in the Description page of Project Settings.
#include "GSD2GameInstance.h"

void UGSD2GameInstance::SetFlag(FName Flag)
{
	if (Flag.IsNone()) return;
	Flags.AddUnique(Flag);
}

bool UGSD2GameInstance::HasFlag(FName Flag) const
{
	return Flags.Contains(Flag);
}

bool UGSD2GameInstance::TryDoOnce(FName Flag)
{
	if (HasFlag(Flag)) return false;

	SetFlag(Flag);
	return true;
}

void UGSD2GameInstance::ResetProgress()
{
	CurrentObjective = FText::GetEmpty();
	CurrentHint = FText::GetEmpty();
	Flags.Empty();
	bRadioNoiseOnly = false;
	SavedItems.Empty();
}