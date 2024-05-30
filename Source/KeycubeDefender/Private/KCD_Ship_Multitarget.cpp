// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Ship_Multitarget.h"

bool AKCD_Ship_Multitarget::Hit(FName Letter)
{
	//We check if there still is letters in the list.
	//Mostly a failsafe since the ship should no longer be
	//targeted when this list is empty
	if(LettersInstances.IsEmpty())
		return false;

	//If the letter checked is the first one in the letters remaining,
	//We destroy the letter
	if(!isDestroyed)
	{
		for (int x = 0; x < LettersInstances.Num(); x++)
		{
			if(LettersInstances[x]->IsLetterHidden)
			{
				continue;
			}

			if(LettersInstances[x]->CurrentLetter == Letter)
			{
				LettersInstances[x]->Hide();
				
				if(LettersInstances.Last() == LettersInstances[x])
				{
					ShipDestroyed();
					return true;
				}
		
				LettersInstances[x + 1]->Highlight();
				return true;
			}
			return false;
		}
	}

	return false;
}