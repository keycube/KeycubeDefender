// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_WaveManager.h"

#include "KCD_LaneHolder.h"
#include "KCD_WordDictionnary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_WaveManager::AKCD_WaveManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WordIndexUsed.Add(FEncapsule{});
	WordIndexUsed.Add(FEncapsule{});
	WordIndexUsed.Add(FEncapsule{});
}

// Called when the game starts or when spawned
void AKCD_WaveManager::BeginPlay()
{
	Super::BeginPlay();

	if (WaveData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("WAVE DATA NOT FOUND"));
	}

	//Get the current game mode and cast it to the required game mode
	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	FTimerHandle TimerHandleGamemodeRefs;

	// Fetch the references set in the gamemode after a delay
	//to allow it to finish fetching them
	GetWorld()->GetTimerManager().SetTimer(TimerHandleGamemodeRefs, [&]()
	{
		LaneHolder = GameModeInstance->GetLaneHolder();
		if(LaneHolder == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("LaneHolder is invalid"));
		} else
		{
			LaneHolder->OnShipCrashedDelegate.AddDynamic(this, &AKCD_WaveManager::ShipCrashed);
		}
	},  0.1, false);

	NextWave();
}

void AKCD_WaveManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* w= GetWorld() ;

	if(w->IsValidLowLevel())
	{
		w->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void AKCD_WaveManager::PrepareShip(int ShipTier)
{
	//Get all the possible words for the ship's tier
	FString ShipTierString = FString::FromInt(ShipTier);
	FKCD_WordDictionnary* possibleWords = WordBank->FindRow<FKCD_WordDictionnary>(FName(ShipTierString), "");

	bool wordFound = false;

	//Get a random word in the list of possible ones
	//We loop so we can find an available word if the one chosen isn't
	int ShipWordIndex = -1;
	for (int x = 0; x <= 20; x++)
	{
		ShipWordIndex = FMath::RandRange(0, possibleWords[0].WordList.Num() - 1);
		if (WordIndexUsed[ShipTier].index.Contains(ShipWordIndex))
			continue;

		wordFound = true;
		WordIndexUsed[ShipTier].index.Add(ShipWordIndex);
		break;
	}

	if (!wordFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Word not found"));
		return;
	}

	//Use a deferred spawn so we can set the ship's word before spawning it
	AKCD_Ship* Ship;

	auto lane = FetchRandomLane();
	if (lane == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get a valid lane"));
		return;
	}

	Ship = lane->SpawnShip(Ships[ShipTier], ShipWordIndex, possibleWords[0].WordList[ShipWordIndex], CurrentWaveData.SpeedModifier);

	ShipsAlive.Add(Ship);

	OnShipSpawnDelegate.Broadcast();

	Ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_WaveManager::RemoveShip);
}

void AKCD_WaveManager::NextWave()
{
	CurrentWaveIndex++;

	if (CurrentWaveIndex > WaveData->GetRowNames().Num())
	{
		OnVictoryDelegate.Broadcast();
		return;
	}
	
	ReadCurrentWaveData(CurrentWaveIndex);

	//Start a looping timer to spawn ships at an interval
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AKCD_WaveManager::PlayWaveSequence,
		CurrentWaveData.SpawnTime, true);
}

void AKCD_WaveManager::PlayWaveSequence()
{
	int ShipTier = CurrentWaveData.availableTiers[FMath::RandRange(0, CurrentWaveData.availableTiers.Num() - 1)];

	PrepareShip(ShipTier);

	--CurrentWaveData.NumShipTier[ShipTier];
	//Once all the ships for a tier are spawned,
	//remove the tier from the list of possible tiers
	if (CurrentWaveData.NumShipTier[ShipTier] <= 0)
		CurrentWaveData.availableTiers.Remove(ShipTier);

	//Stop the spawning loop when all ships are spawned
	if(CurrentWaveData.availableTiers.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		CurrentWaveData.NumShipTier.Empty();
	}
}

void AKCD_WaveManager::ReadCurrentWaveData(int WaveIndex)
{
	const FKCD_WaveData* CurrentWave = WaveData->FindRow<FKCD_WaveData>(FName(FString::FromInt(WaveIndex)), "");

	//Check if the wave is valid (there should always be at least one ship of T0)
	if (CurrentWave->NumberOfT0 <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave is invalid"));
		return;
	}
	//Fill the current wave number of ships
	CurrentWaveData.NumShipTier.Add(0, CurrentWave->NumberOfT0);
	if(CurrentWaveData.NumShipTier[0] > 0)
		CurrentWaveData.availableTiers.Add(0);
	
	CurrentWaveData.NumShipTier.Add(1, CurrentWave->NumberOfT1);
	if(CurrentWaveData.NumShipTier[1] > 0)
		CurrentWaveData.availableTiers.Add(1);
	
	CurrentWaveData.NumShipTier.Add(2, CurrentWave->NumberOfT2);
	if(CurrentWaveData.NumShipTier[2] > 0)
		CurrentWaveData.availableTiers.Add(2);

	//fill the variable for the speed of this wave
	CurrentWaveData.SpawnTime = CurrentWave->SpawnSpeed;
	CurrentWaveData.SpeedModifier = CurrentWave->SpeedModifier;
}

void AKCD_WaveManager::RemoveShip(AKCD_Ship* Ship)
{
	ShipsAlive.Remove(Ship);
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
	WordIndexUsed[Ship->Tier].index.Remove(Ship->WordIndex);

	//When it is the last ship of the wave, we wait a bit then start the new wave
	if(ShipsAlive.IsEmpty() && CurrentWaveData.availableTiers.IsEmpty())
	{
		if(CurrentWaveIndex != WaveData->GetRowNames().Num())
			OnWaveCompleteDelegate.Broadcast(CurrentWaveIndex);
		
		GetWorld()->GetTimerManager().SetTimer(NewWaveTimerHandle, this, &AKCD_WaveManager::NextWave,
			3, false);
	}
}

AKCD_Lane* AKCD_WaveManager::FetchRandomLane()
{
	return LaneHolder->Lanes[FMath::RandRange(0, LaneHolder->Lanes.Num() - 1)];
}

void AKCD_WaveManager::ShipCrashed()
{
	AverageStats();
}

void AKCD_WaveManager::AverageStats()
{
	FKCD_TypingStats AverageStat;

	//Makes an average of all main target stats
	for (auto TypeStat : MainTypingStats)
	{
		AverageStat.Mistakes += TypeStat.Mistakes;
		AverageStat.Score += TypeStat.Score;
		AverageStat.TimeTaken += TypeStat.TimeTaken;
		AverageStat.WordDistance += TypeStat.WordDistance;
		AverageStat.WordSize += TypeStat.WordSize;
	}

	int size = MainTypingStats.Num();
	
	AverageStat.Mistakes = AverageStat.Mistakes / size;
	AverageStat.Score = AverageStat.Score / size;
	AverageStat.TimeTaken = AverageStat.TimeTaken / size;
	AverageStat.WordDistance = AverageStat.WordDistance / size;
	AverageStat.WordSize = AverageStat.WordSize / size;
	AverageStat.WasAltTarget = false;

	WriteStats("Main target", AverageStat);

	//Reset the var
	AverageStat = FKCD_TypingStats();

	//Makes an average of all alt target stats
	for (auto TypeStat : AltTypingStats)
	{
		AverageStat.Mistakes += TypeStat.Mistakes;
		AverageStat.Score += TypeStat.Score;
		AverageStat.TimeTaken += TypeStat.TimeTaken;
		AverageStat.WordDistance += TypeStat.WordDistance;
		AverageStat.WordSize += TypeStat.WordSize;
	}

	size = AltTypingStats.Num();
	
	AverageStat.Mistakes = AverageStat.Mistakes / size;
	AverageStat.Score = AverageStat.Score / size;
	AverageStat.TimeTaken = AverageStat.TimeTaken / size;
	AverageStat.WordDistance = AverageStat.WordDistance / size;
	AverageStat.WordSize = AverageStat.WordSize / size;
	AverageStat.WasAltTarget = false;


	WriteStats("Alt target", AverageStat);
}

void AKCD_WaveManager::WriteStats(FString RowName, FKCD_TypingStats Stat)
{
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("Result.csv")));

	srand(time(0));
	std::string Str;
	for(unsigned int i = 0; i < 20; ++i)
	{
		Str += genRandom();
	}

	//Open the file in append mode and check if it is opened
	std::ofstream myfile (path, std::ios::app);
	if (myfile.is_open())
	{
		//Create an FString with all results
		FString ResultFString;
		ResultFString = ",," + FString::SanitizeFloat(Stat.Score) + "," +
			FString::SanitizeFloat(Stat.TimeTaken) + "," +
				FString::SanitizeFloat(Stat.Mistakes) + "," +
					FString::SanitizeFloat(Stat.WordSize) + "," +
						FString::SanitizeFloat(Stat.WordDistance) + ",";

		//Convert the FString into a std::string
		std::string ResultString = std::string(TCHAR_TO_UTF8(*ResultFString));

		myfile << "user_" + Str + "\n";
		//RowName
		myfile << std::string((TCHAR_TO_UTF8(*("," + RowName + "\n"))));
		//Titles
		myfile << ",,,Score,Time Taken,Mistakes,Word Size, Word Distance\n";
		//Data
		myfile << ResultString + "\n";
		//Skip lines for readability
		myfile << "\n\n";
		
		//Close the file
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}
}

TArray<AKCD_Ship*> AKCD_WaveManager::GetValidShips(FName Letter)
{
	//Verify if there is a ship alive
	if (ShipsAlive.IsEmpty())
		return TArray<AKCD_Ship*>();

	TArray<AKCD_Ship*> matchingShips;

	//We go trough the list of ship to find one who's first
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

void AKCD_WaveManager::AddStats(FKCD_TypingStats StatsToAdd)
{
	if(!StatsToAdd.WasAltTarget)
		MainTypingStats.Add(StatsToAdd);
	else
		AltTypingStats.Add(StatsToAdd);
}
