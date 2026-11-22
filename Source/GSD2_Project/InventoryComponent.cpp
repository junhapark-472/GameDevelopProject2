#include "InventoryComponent.h"
#include "GSD2GameInstance.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 레벨이 바뀌어 캐릭터가 새로 생겨도 GameInstance에 저장된 아이템을 불러온다
	if (UGSD2GameInstance* GI = Cast<UGSD2GameInstance>(GetWorld()->GetGameInstance()))
	{
		Items = GI->SavedItems;
	}
}

void UInventoryComponent::SaveToProgress()
{
	if (UGSD2GameInstance* GI = Cast<UGSD2GameInstance>(GetWorld()->GetGameInstance()))
	{
		GI->SavedItems = Items;
	}
}

bool UInventoryComponent::AddItem(FName ItemID)
{
	// 이 게임의 아이템은 전부 하나씩만 있으므로 중복 획득은 막는다
	if (ItemID.IsNone() || Items.Contains(ItemID)) return false;

	Items.Add(ItemID);
	SaveToProgress();
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID)
{
	if (Items.Remove(ItemID) == 0) return false;

	if (SelectedItem == ItemID) SelectedItem = NAME_None;
	SaveToProgress();
	return true;
}

bool UInventoryComponent::HasItem(FName ItemID) const
{
	return Items.Contains(ItemID);
}

TArray<FName> UInventoryComponent::GetItems() const
{
	return Items;
}

bool UInventoryComponent::GetItemData(FName ItemID, FItemData& OutData) const
{
	if (!ItemTable) return false;

	if (const FItemData* Row = ItemTable->FindRow<FItemData>(ItemID, TEXT("GetItemData")))
	{
		OutData = *Row;
		return true;
	}
	return false;
}

bool UInventoryComponent::CombineItems(FName ItemA, FName ItemB)
{
	if (!HasItem(ItemA) || !HasItem(ItemB)) return false;

	for (const FItemCombineRule& Rule : CombineRules)
	{
		const bool bMatch = (Rule.ItemA == ItemA && Rule.ItemB == ItemB) ||
			(Rule.ItemA == ItemB && Rule.ItemB == ItemA);
		if (bMatch)
		{
			Items.Remove(ItemA);
			Items.Remove(ItemB);
			Items.AddUnique(Rule.Result);
			SelectedItem = NAME_None;
			SaveToProgress();
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::SplitItem(FName CombinedItem)
{
	if (!HasItem(CombinedItem)) return false;

	for (const FItemCombineRule& Rule : CombineRules)
	{
		if (Rule.Result == CombinedItem)
		{
			Items.Remove(CombinedItem);
			Items.AddUnique(Rule.ItemA);
			Items.AddUnique(Rule.ItemB);
			SelectedItem = NAME_None;
			SaveToProgress();
			return true;
		}
	}
	return false;
}

void UInventoryComponent::SelectItem(FName ItemID)
{
	SelectedItem = HasItem(ItemID) ? ItemID : NAME_None;
}