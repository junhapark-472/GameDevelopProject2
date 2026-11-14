// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(FName ItemID)
{
	// Áßº¹ È¹µæ ¹æÁö¿ë
	if (ItemID.IsNone() || Items.Contains(ItemID)) return false;

	Items.Add(ItemID);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID)
{
	if (Items.Remove(ItemID) == 0) return false;

	if (SelectedItem == ItemID) SelectedItem = NAME_None;
	OnInventoryChanged.Broadcast();
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
			OnInventoryChanged.Broadcast();
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
			OnInventoryChanged.Broadcast();
			return true;
		}
	}
	return false;
}

void UInventoryComponent::SelectItem(FName ItemID)
{
	SelectedItem = HasItem(ItemID) ? ItemID : NAME_None;
}
