// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Cube_Multitarget.h"

#include "KCD_Spawner_MultiTarget.h"

void AKCD_Cube_Multitarget::KeyPress(FKey key)
{
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	KeyPressed->KeyPressed_Keys();
	//If the target is invalid, we try to get a new one
	NewTargets(Cast<AKCD_Spawner_MultiTarget>(SpawnerInstance)->GetClosestShips(key.GetFName()));

	if(CurrentTargets.IsEmpty())
	 return;

	TArray<AKCD_Ship*> TargetsToRemove;
	//New list to avoid error when removing target during iteration
	TArray<AKCD_Ship*> TargetsToCheck = CurrentTargets;

	bool isHitSuccess = false;
	//Try to hit the current targets
	for (auto Ship : TargetsToCheck)
	{
		if(!Ship->Hit(key.GetFName()))
		{
			TargetsToRemove.Add(Ship);
		} else
		{
			isHitSuccess = true;
			UpdateMultiplicator(true);
			Score += 1 * ScoreMultiplicator;
		}
	}

	if(!TargetsToRemove.IsEmpty())
	{
		for (auto ToRemove : TargetsToRemove)
		{
			ToRemove->Untargeted();
			RemoveTarget(ToRemove);
		}
	}
	
	if(!isHitSuccess)
	{
		UpdateMultiplicator(false);
		CurrentTargets.Empty();
	} 
	
	OnScoreUpdateDelegate.Broadcast();
}

void AKCD_Cube_Multitarget::NewTargets(const TArray<AKCD_Ship*>& ShipList)
{
	for (auto NewShip : ShipList)
	{
		if(!CurrentTargets.Contains(NewShip))
		{
			NewShip->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Cube_Multitarget::RemoveTarget);
			CurrentTargets.Add(NewShip);
		}
	}
}

void AKCD_Cube_Multitarget::RemoveTarget(AKCD_Ship* Ship)
{
	CurrentTargets.Remove(Ship);
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
}
