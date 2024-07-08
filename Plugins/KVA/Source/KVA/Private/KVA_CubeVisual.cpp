﻿#include "KVA_CubeVisual.h"

#include <filesystem>

#include "KVA_CubeInfo.h"
#include "KVA_KeyTranslation.h"
#include "Kismet/GameplayStatics.h"

AKVA_CubeVisual::AKVA_CubeVisual()
{
}

void AKVA_CubeVisual::BeginPlay()
{
	Super::BeginPlay();
	
	FillKeyMap();

	LoadKeyMatrix();
}

void AKVA_CubeVisual::KeyPressed(FKey KeyToPress)
{
	AKVA_Keys* KeyPressed = Keys.FindRef(KeyToPress);
	KeyPressed->KeyPressed_Keys();
}

void AKVA_CubeVisual::KeyReleased(FKey KeyToRelease)
{
	if(!Keys.Contains(KeyToRelease))
	{
		return;
	}
	
	AKVA_Keys* KeyPressed = Keys.FindRef(KeyToRelease);

	KeyPressed->KeyReleased_Keys();
}

void AKVA_CubeVisual::HighlightKeys(TArray<FKey> keysToHighlight)
{
	for (auto key : keysToHighlight)
	{
		if(key.GetFName() == "")
		{
			continue;
		}
		Keys[key]->HighlightKey();

		HighlitedKeys.AddUnique(key);
	}
}

void AKVA_CubeVisual::UnhighlightKeys(TArray<FKey> KeysToUnHighlight)
{
	for(auto key : KeysToUnHighlight)
	{
		if(key.GetFName() == "")
		{
			continue;
		}
		UE_LOG(LogTemp, Warning, TEXT("Key : %s is being untargeted"), *key.GetFName().ToString());
		
		Keys[key]->UnhighlightKey();

		HighlitedKeys.Remove(key);
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

void AKVA_CubeVisual::UpdateHighlight()
{
	return;
}

void AKVA_CubeVisual::SaveKeyMatrix()
{

	WriteToFile();

	
	// UKVA_CubeInfo* CubeInfo = NewObject<UKVA_CubeInfo>(this);
	//
	// UE_LOG(LogTemp, Warning, TEXT("Saving key matrix"));
	//
	// for (int x = 0; x < KeysMatrix.Num(); x++)
	// {
	// 	CubeInfo->SavedKeyMatrix.Add(FKeyFaceLoad{});
	// 	for (int y = 0; y < KeysMatrix[x].Face.Num(); y++)
	// 	{
	// 		CubeInfo->SavedKeyMatrix[x].Face.Add(FKeyRowLoad{});
	// 		for (int z = 0; z < KeysMatrix[x].Face[y].Keys.Num();z++)
	// 		{
	// 			CubeInfo->SavedKeyMatrix[x].Face[y].Keys.Add(KeysMatrix[x].Face[y].Keys[z]->AssociatedKey);
	// 			KeysMatrix[x].Face[y].Keys[z]->UnhighlightKey();
	// 		}
	// 	}
	// }
	//
	// UGameplayStatics::SaveGameToSlot(CubeInfo, UKVA_CubeInfo::SlotName, 0);
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

void AKVA_CubeVisual::LoadKeyMatrix()
{
	ReadFile();



	
	// if (auto LoadedKeyMatrix = Cast<UKVA_CubeInfo>(UGameplayStatics::LoadGameFromSlot(UKVA_CubeInfo::SlotName, 0)))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Key matrix to load foun. Starting load"));
	// 	for (int x = 0; x < LoadedKeyMatrix->SavedKeyMatrix.Num(); x++)
	// 	{
	// 		for (int y = 0; y < LoadedKeyMatrix->SavedKeyMatrix[x].Face.Num(); y++)
	// 		{
	// 			for (int z = 0; z < LoadedKeyMatrix->SavedKeyMatrix[x].Face[y].Keys.Num();z++)
	// 			{
	// 				KeysMatrix[x].Face[y].Keys[z]->ChangeKey(LoadedKeyMatrix->SavedKeyMatrix[x].Face[y].Keys[z]);
	// 				KeysMatrix[x].Face[y].Keys[z]->UnhighlightKey();
	// 			}
	// 		}
	// 	}
	// 	UE_LOG(LogTemp, Warning, TEXT("Key matrix loaded from save data"));
	// }
}

void AKVA_CubeVisual::WriteToFile()
{
	FString RelativePath = FPaths::ProjectContentDir();

	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("TestInput.txt")));
	
	std::ofstream myfile (path);
	if (myfile.is_open())
	{
		std::filesystem::path cwd = std::filesystem::current_path();
		
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
		
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}
}

void AKVA_CubeVisual::ReadFile()
{
	std::string line;

	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("TestInput.txt")));

	FKVA_KeyTranslation* KeyAssociation;
	
	int x = 0;
	int y = 0;
	int z = 0;
	
	std::ifstream myfile (path);
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			
			FString key = line.c_str();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *key);

			KeyAssociation = KeyTranslationTable->FindRow<FKVA_KeyTranslation>(FName(key), "");

			if(KeyAssociation->Key.IsValid())
			{
				KeysMatrix[x].Face[y].Keys[z]->ChangeKey(KeyAssociation->Key);
				KeysMatrix[x].Face[y].Keys[z]->UnhighlightKey();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Key invalid"));
			}

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
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for read"));
	}
}