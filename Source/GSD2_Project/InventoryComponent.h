#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "InventoryComponent.generated.h"

class UTexture2D;

// 데이터 테이블 한 줄 = 아이템 1개 (Row Name = 아이템 아이디)

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;
};

// 결합 규칙: ItemA + ItemB = Result (분리는 거꿀로)

USTRUCT(BlueprintType)
struct FItemCombineRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName Result;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GSD2_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	// 아이템 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UDataTable> ItemTable;

	// 결합 규칙
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemCombineRule> CombineRules;

	// 인벤토리가 바뀌면 호출됨 (UI 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemID) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FName> GetItems() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool GetItemData(FName ItemID, FItemData& OutData) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CombineItems(FName ItemA, FName ItemB);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SplitItem(FName CombinedItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectItem(FName ItemID);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FName GetSelectedItem() const { return SelectedItem; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FName> Items;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	FName SelectedItem;
};