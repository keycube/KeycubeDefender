#include "KVA_CubeVisual.h"

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
	if(!Keys.Contains(KeyToPress))
	{
		return;
	}
	
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
	for(auto key : KeysToUnHighlight)
	{
		if(!Keys.Contains(key))
		{
			continue;
		}
		
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