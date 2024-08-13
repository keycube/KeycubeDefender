#include "KVA_CubeVisual.h"

#include <filesystem>

#include "KVA_KeyTranslation.h"
#include "KVA_SaveCubeOption.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AKVA_CubeVisual::AKVA_CubeVisual(): KeyTranslationTable(nullptr), CubeOptions(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AKVA_CubeVisual::BeginPlay()
{
	Super::BeginPlay();
	
	FillKeyMap();

	LoadKeyMatrix();

	BaseRotation = this->GetTransform().Rotator();

	if(UKVA_SaveCubeOption* CubeSave = Cast<UKVA_SaveCubeOption>(UGameplayStatics::LoadGameFromSlot(UKVA_SaveCubeOption::SlotName, 0)))
	{
		CubeOptions = CubeSave;
	} else
	{
		CubeOptions = Cast<UKVA_SaveCubeOption>(UGameplayStatics::CreateSaveGameObject(UKVA_SaveCubeOption::StaticClass()));
	}

	if(!CubeOptions->ShowCube)
	{
		SetActorHiddenInGame(true);
	}

	if(CubeOptions->InvertRotation)
	{
		FaceOneRotation = FaceOneRotation * -1;
		FaceZeroRotation = FaceZeroRotation * -1;
	}
}

void AKVA_CubeVisual::KeyPressed(FKey KeyToPress)
{
	if(!Keys.Contains(KeyToPress) || !CubeOptions->ShowTypeHighligh)
	{
		return;
	}
	
	AKVA_Keys* KeyPressed = Keys.FindRef(KeyToPress);
	KeyPressed->KeyPressed_Keys();
}

void AKVA_CubeVisual::KeyReleased(FKey KeyToRelease)
{
	if(!Keys.Contains(KeyToRelease) || !CubeOptions->ShowTypeHighligh)
	{
		return;
	}
	
	AKVA_Keys* KeyPressed = Keys.FindRef(KeyToRelease);

	KeyPressed->KeyReleased_Keys();
}

void AKVA_CubeVisual::HighlightKeys(TArray<FKey> keysToHighlight)
{
	if(CubeOptions->ShouldRotate)
	{
		RotateCube(keysToHighlight.Last());
	}
	
	if(!CubeOptions->ShowNextHighligh)
	{
		return;
	}
	
	for (auto key : keysToHighlight)
	{
		if(!Keys.Contains(key))
		{
			continue;
		}
		Keys[key]->HighlightKey();

		HighlitedKeys.AddUnique(key);
	}
}

void AKVA_CubeVisual::UnhighlightKeys(TArray<FKey> KeysToUnHighlight)
{
	if(!CubeOptions->ShowNextHighligh && !CubeOptions->ShowTypeHighligh)
	{
		return;
	}
	
	for(auto key : KeysToUnHighlight)
	{
		if(!Keys.Contains(key))
		{
			continue;
		}
		
		Keys[key]->UnhighlightKey();

		HighlitedKeys.Remove(key);
	}

	if(HighlitedKeys.IsEmpty())
	{
		TargetRotation = BaseRotation;
		NeedsRotation = true;
	} else
	{
		RotateCube(HighlitedKeys.Last());
	}
}

void AKVA_CubeVisual::FillKeyMatrix(TArray<UChildActorComponent*> KeyActors, int Row, int Face)
{
	while(!KeysMatrix.IsValidIndex(Face))
	{
		KeysMatrix.Add(FKeyFace{});
	}

	KeysMatrix[Face].Face.Add(FKeyRow{});
	
	for (auto Element : KeyActors)
	{
		AKVA_Keys* Key = Cast<AKVA_Keys>(Element->GetChildActor());
		if(Key != nullptr)
		{
			KeysMatrix[Face].Face[Row].Keys.Add(Key);
		}
	}
}

void AKVA_CubeVisual::FillKeyMap()
{
	//List of all the child components of the object
	TArray<USceneComponent*> ChildComponents;
	RootComponent->GetChildrenComponents(true, ChildComponents);

	Keys.Empty();
	
	// Check all the object's childComponent. If they are of the AKCD_Keys class, we add them
	// to the map and associate them with the right key
	for (auto Child : ChildComponents)
	{
		if(auto ChildActor = Cast<UChildActorComponent>(Child))
		{
			if(auto ChildKey = Cast<AKVA_Keys>(ChildActor->GetChildActor()))
			{
				if(!Keys.Find(ChildKey->AssociatedKey))
				{
					Keys.Add(ChildKey->AssociatedKey, ChildKey); 
				} else
				{
					UE_LOG(LogTemp, Warning, TEXT("Key : %s Was already present"), *ChildKey->AssociatedKey.GetDisplayName().ToString());
				}
				
			}
		}
	}
}

void AKVA_CubeVisual::SaveKeyMatrix()
{
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("InputLayout.txt")));

	//Open the file and check if it is opened
	std::ofstream myfile (path);
	if (myfile.is_open())
	{
		//We loop trough all the keys is the key matrix and save them to the file
		for (int x = 0; x < KeysMatrix.Num(); x++)
		{
			for (int y = 0; y < KeysMatrix[x].Face.Num(); y++)
			{
				for (int z = 0; z < KeysMatrix[x].Face[y].Keys.Num();z++)
				{
					std::string keyString = std::string(TCHAR_TO_UTF8(*KeysMatrix[x].Face[y].Keys[z]
						->AssociatedKey.GetFName().ToString()));
					myfile << keyString + "\n";
					KeysMatrix[x].Face[y].Keys[z]->UnhighlightKey();
				}
			}
		}
		//Close the file
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}
}

void AKVA_CubeVisual::ChangeKey(AKVA_Keys* ChangingKey, FKey NewKey)
{
	if(Keys.Contains(NewKey))
	{
		Keys[NewKey]->ChangeKey(ChangingKey->AssociatedKey);
	}
	
	ChangingKey->ChangeKey(NewKey);
	FillKeyMap();
}

FKey AKVA_CubeVisual::GetKey(int Face, int Row, int Column)
{
	return KeysMatrix[Face].Face[Row].Keys[Column]->AssociatedKey;
}

void AKVA_CubeVisual::LoadKeyMatrix()
{
	//Line we will read from the file
	std::string line;

	//Set the relative path where the file should be to read
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("InputLayout.txt")));

	//Dictionary of letters/key names to their FKey counterpart
	FKVA_KeyTranslation* KeyAssociation;

	//face
	int x = 0;
	//Line
	int y = 0;
	//Column
	int z = 0;

	//Try to open the file we want to read from.
	std::ifstream myfile (path);
	if (myfile.is_open())
	{
		//Read all the lines of the file
		while ( getline (myfile,line) )
		{
			//transform the std::string into an FString
			FString key = line.c_str();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *key);

			//Find the associated key
			KeyAssociation = KeyTranslationTable->FindRow<FKVA_KeyTranslation>(FName(key), "");

			//Apply the key if it is valid
			if(KeyAssociation->Key.IsValid())
			{
				KeysMatrix[x].Face[y].Keys[z]->ChangeKey(KeyAssociation->Key);
				KeysMatrix[x].Face[y].Keys[z]->UnhighlightKey();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Key invalid"));
			}

			//Advance in the key matrix
			z++;
			if(z >= 4)
			{
				y++;
				if(y >= 4)
				{
					x++;
					y = 0;
				}
				z = 0;
			}
		}
		//Close the file
		myfile.close();
		//Reset the key matrix
		FillKeyMap();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for read"));
	}
}

void AKVA_CubeVisual::RotateCube(FKey KeyToFace)
{

	if(!Keys.Contains(KeyToFace))
	{
		TargetRotation = BaseRotation;
		NeedsRotation = true;
		return;
	}
		
	FVector Direction = this->GetTransform().GetRelativeTransform(this->GetTransform()).GetLocation()-
		Keys[KeyToFace]->GetTransform().GetRelativeTransform(this->GetTransform()).GetLocation();
		
	Direction.Normalize();

	UE_LOG(LogTemp, Warning, TEXT("Direction : %s"), *Direction.ToString());
		
	if(abs(Direction.X) > abs(Direction.Y))
	{
		TargetRotation = BaseRotation + FaceZeroRotation;
	} else
	{
		TargetRotation = BaseRotation + FaceOneRotation;
	}
	NeedsRotation = true;
	
}

void AKVA_CubeVisual::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(NeedsRotation)
	{
		SetActorRotation(FMath::Lerp(GetActorRotation(), TargetRotation, 0.1f));

		if(FMath::IsNearlyEqual(GetActorRotation().Yaw, TargetRotation.Yaw, 0.5))
		{
			NeedsRotation = false;
		}
	}
}

