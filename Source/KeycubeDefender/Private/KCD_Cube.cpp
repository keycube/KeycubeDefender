// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Cube.h"

#include "IPropertyTable.h"
#include "KCD_GameMode.h"
#include "KCD_PlayerController.h"
#include "KCD_WaveManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Cube::AKCD_Cube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Instantiate the object component
	RootObject = CreateDefaultSubobject<USceneComponent>("ObjectRoot");
	RootComponent = RootObject;
}

// Called when the game starts or when spawned
void AKCD_Cube::BeginPlay()
{
	Super::BeginPlay();

	//Get the current player controller
	AKCD_PlayerController* PlayerController = Cast<AKCD_PlayerController>(UGameplayStatics::GetPlayerController(
		this, 0));

	//Subscribes to the delegate for then a key is pressed and when it's released
	PlayerController->KeyPressDelegate.AddDynamic(this, &AKCD_Cube::KeyPress);
	PlayerController->KeyReleaseDelegate.AddDynamic(this, &AKCD_Cube::KeyRelease);

	//Get the current game mode and cast it to the required game mode
	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	if(!GameModeInstance->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game mode is invalid"));
	}

	FTimerHandle TimerHandleGamemodeRefs;

	// Fetch the references set in the gamemode after a delay
	//to allow it to finish fetching them
	GetWorld()->GetTimerManager().SetTimer(TimerHandleGamemodeRefs, [&]()
	{
		SpawnerInstance = GameModeInstance->GetShipSpawner();
		if(SpawnerInstance == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SpawnerInstance is invalid"));
		}

		SpawnerInstance->OnShipSpawnDelegate.AddDynamic(this, &AKCD_Cube::UpdateHighlight);

		Cube = GameModeInstance->GetCubeVisual();
	},  0.1, false);
	
}

void AKCD_Cube::KeyPress(FKey key)
{
	//Check if the key is present in the accepted key list
	if(!Cube->Keys.Contains(key))
	{
		return;
	}

	//Feedback
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TypeSound, GetTransform().GetLocation());
	Cube->KeyPressed(key);
	
	//We see if new targets are linked to this key
	NewTargets(Cast<AKCD_WaveManager>(SpawnerInstance)->GetValidShips(key.GetFName()));
	
	if(CurrentTargets.IsEmpty())
	{
		UpdateMultiplicator(false);
		return;
	}
	
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
			Score += 1 * (ScoreMultiplicator * ( 1 + ComboCounter));
		}
	}

	//We remove from the current targets those who's next letter doesn't match
	if(!TargetsToRemove.IsEmpty())
	{
		for (auto ToRemove : TargetsToRemove)
		{
			RemoveTarget(ToRemove);
		}
	}
	
	if(!isHitSuccess)
	{
		UpdateMultiplicator(false);
	}

	UpdateHighlight();
	
	OnScoreUpdateDelegate.Broadcast();
}

void AKCD_Cube::KeyRelease(FKey key)
{
	Cube->KeyReleased(key);
}

void AKCD_Cube::RemoveTarget(AKCD_Ship* Ship)
{
	//We check if the remove target is the primary target
	if(PrimaryTarget->IsValidLowLevel())
	{
		//If he is, we check if it's destroy and remove it if it is 
		if(Ship == PrimaryTarget)
		{
			if(Ship->isDestroyed)
			{
				PrimaryTarget = nullptr;
				CurrentTargets.Remove(Ship);
				Ship->OnShipDestroyedDelegate.RemoveAll(this);
				if(CurrentTargets.IsEmpty())
				{
					ComboCounter = 0;
				}
				FindPrimaryTarget();
				return;
			} 
			return;
		}

	}

	//If it isn't the primary target, we remove it from the current targets
	CurrentTargets.Remove(Ship);
	Ship->Untargeted();
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
	if(CurrentTargets.IsEmpty())
	{
		ComboCounter = 0;
	}
}

TArray<FKey> AKCD_Cube::TranslateKeys(TArray<FName> KeysToTranslate)
{
	TArray<FKey> translatedKeys;

	for (auto key : KeysToTranslate)
	{
		translatedKeys.Add(FKey(key));
	}
	
	return translatedKeys;
}

//While this function is not optimized, the limited amount of 
//possible letters and ships to check won't make it inefficient
void AKCD_Cube::UpdateHighlight()
{
	//We fill an array with all possible next letters to press
	//If we already have targets, we only check them
	//If we don't have any target, we check all alive ships
	TArray<FName> targetableLetters;
	if(CurrentTargets.IsEmpty())
	{
		for (auto ship : SpawnerInstance->ShipsAlive)
		{
			targetableLetters.AddUnique(ship->CurrentLetter);
		}
	}
	else
	{
		for(auto ship : CurrentTargets)
		{
			targetableLetters.AddUnique(ship->CurrentLetter);
		}
	}

	Cube->HighlightKeys(TranslateKeys(targetableLetters));

	//Same for here, we fill and array while checking if a letter in the 
	//highlighted keys list is no longer present in the list of letters to target
	//and feed that list to a function to untarget them
	TArray<FName> LettersToUntarget;
	for (auto Letter : Cube->HighlitedKeys)
	{
		if(!targetableLetters.Contains(Letter.GetFName()))
		{
			LettersToUntarget.Add(Letter.GetFName());
		}
	}

	Cube->UnhighlightKeys(TranslateKeys(LettersToUntarget));
	
}

void AKCD_Cube::FindPrimaryTarget()
{
	float closestDistance = 10000;
	AKCD_Ship* lowestShip = nullptr;
	
	if(!PrimaryTarget->IsValidLowLevel() && !CurrentTargets.IsEmpty())
	{
		//Check for the lowest ship in the current targets
		for(auto ship : CurrentTargets)
		{
			if(ship->GetTransform().GetLocation().Z < closestDistance)
			{
				closestDistance = ship->GetTransform().GetLocation().Z;
				lowestShip = ship;
			}
		}

		PrimaryTarget = lowestShip;
		PrimaryTarget->SetMainTarget();
	}
}

void AKCD_Cube::ShipDestroyed(AKCD_Ship* Ship)
{
	Score += Ship->Reward * (ScoreMultiplicator * ( 1 + ComboCounter));
	RemoveTarget(Ship);
	
	if(!CurrentTargets.IsEmpty())
	{
		ComboCounter++;
	}

	OnScoreUpdateDelegate.Broadcast();
}

void AKCD_Cube::UpdateMultiplicator(bool Success)
{
	if(Success)
	{
		MultiplicatorGauge++;
		if(MultiplicatorGauge % (10 * ScoreMultiplicator) == 0)
		{
			MultiplicatorGauge = 0;
			ScoreMultiplicator += 1;
		}
	} else
	{
		MultiplicatorGauge = 0;
		ScoreMultiplicator = 1;
	}
}

void AKCD_Cube::NewTargets(const TArray<AKCD_Ship*>& ShipList)
{
	for (auto NewShip : ShipList)
	{
		if(!CurrentTargets.Contains(NewShip))
		{
			NewShip->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Cube::ShipDestroyed);
			CurrentTargets.Add(NewShip);
		}
	}

	if(!PrimaryTarget->IsValidLowLevel())
	{
		FindPrimaryTarget();
	}
}

TArray<AKCD_Ship*> AKCD_Cube::GetCurrentTargets()
{
	return CurrentTargets;
}

void AKCD_Cube::WriteScore(FString Name)
{
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("Leaderboard.csv")));

	TArray<FKCD_PlayerScore> Leaderboard = FetchScores();
	
	//Open the file in append mode and check if it is opened
	std::ofstream myfile (path);
	if (myfile.is_open())
	{
		//Create an FString with the score data
		FString ResultFString;
		ResultFString = Name + "," +
			FString::FromInt(Score);

		//Convert the FString into a std::string
		std::string ResultString = std::string(TCHAR_TO_UTF8(*ResultFString));

		bool isWritten = false;

		if(Leaderboard.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Leaderboard was empty"));
			myfile << ResultString + "\n";
			//Close the file
			myfile.close();
			return;
		}

		for(auto score : Leaderboard)
		{
			if(score.Score < Score && !isWritten)
			{
				myfile << ResultString + "\n";
				isWritten = true;
			}
			
			//Create an FString with the score data
			FString LeaderboardFString;
			LeaderboardFString = score.Name + "," +
				FString::FromInt(score.Score);

			//Convert the FString into a std::string
			std::string LeaderboardString = std::string(TCHAR_TO_UTF8(*LeaderboardFString));
			//Data
			myfile << LeaderboardString + "\n";
		}

		if(!isWritten)
			myfile << ResultString + "\n";

		//Close the file
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}
}

TArray<FKCD_PlayerScore> AKCD_Cube::FetchScores()
{
	TArray<FKCD_PlayerScore> Scores;

	//Line we will read from the file
	std::string line;
	std::string cell;
	
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("Leaderboard.csv")));

	//Try to open the file we want to read from.
	std::ifstream myfile (path);
	if (myfile.is_open())
	{
		
		//Read all the lines of the file
		while ( getline (myfile,line) )
		{
			// Create a stringstream from line
			std::stringstream ss(line);
			
			std::string stringGot[2];
			int x = 0;
			
			while( getline (ss, cell, ','))
			{
				stringGot[x] = cell;
				x++;
			}
			
			FKCD_PlayerScore CheckedScore;

			CheckedScore.Name = stringGot[0].c_str();
			CheckedScore.Score = std::stoi(stringGot[1]);

			Scores.Add(CheckedScore);
		}
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open score file for read"));
	}
	
	return Scores;
}

