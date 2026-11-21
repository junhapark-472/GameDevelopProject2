// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GSD2GameInstance.generated.h"

// 레벨이 바뀌어도 플레이어 상태 유지
UCLASS()
class GSD2_PROJECT_API UGSD2GameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	// 지금 임무
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void SetObjective(const FText& NewObjective) { CurrentObjective = NewObjective; }
	UFUNCTION(BlueprintPure, Category = "Progress")
	FText GetObjective() const { return CurrentObjective; }

	// 기록
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void SetFlag(FName Flag);
	UFUNCTION(BlueprintPure, Category = "Progress")
	bool HasFlag(FName Flag) const;

	// 처음이면 저장, 아니면 false
	UFUNCTION(BlueprintCallable, Category = "Progress")
	bool TryDoOnce(FName Flag);

	// 무전기
	UFUNCTION(BlueprintCallable, Category = "Radio")
	void SetHint(const FText& NewHint) { CurrentHint = NewHint; }
	UFUNCTION(BlueprintPure, Category = "Radio")
	FText GetHint() const { return CurrentHint; }

	// true면 힌트 말고 잡음
	UFUNCTION(BlueprintCallable, Category = "Radio")
	void SetRadioNoiseOnly(bool bNoiseOnly) { bRadioNoiseOnly = bNoiseOnly; }
	UFUNCTION(BlueprintPure, Category = "Radio")
	bool IsRadioNoiseOnly() const { return bRadioNoiseOnly; }

	//  인벤토리 보관 (InventoryComponent가 사용)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FName> SavedItems;

	// 새 게임 초기화
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void ResetProgress();

private:
	FText CurrentObjective;
	FText CurrentHint;
	TArray<FName> Flags;
	bool bRadioNoiseOnly = false;
};