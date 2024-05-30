// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Spawner_MultiTarget.h"

TArray<AKCD_Ship*> AKCD_Spawner_MultiTarget::GetClosestShips(FName Letter)
{
	//Verify if there is a ship alive
	if (ShipsAlive.IsEmpty())
		return TArray<AKCD_Ship*>();

	TArray<AKCD_Ship*> matchingShips;

	//We go trough the list of ship to find one who's current
	//letter to destroy is the one we try to shoot
	for (AKCD_Ship* shipChecked : ShipsAlive)
	{
		if (shipChecked->isDestroyed)
			continue;
		if (shipChecked->LettersInstances[0]->CurrentLetter == Letter)
		{
			matchingShips.Add(shipChecked);
		}
	}

	return matchingShips;
}
