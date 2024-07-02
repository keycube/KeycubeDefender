#include "KVA_CubeVisual.h"

AKVA_CubeVisual::AKVA_CubeVisual()
{
}

void AKVA_CubeVisual::BeginPlay()
{
	Super::BeginPlay();

	FillKeyMap();
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
		Keys[key]->UnhighlightKey();

		HighlitedKeys.Remove(key);
	}
}

void AKVA_CubeVisual::FillKeyMap()
{
	//List of all the child components of the object
	TArray<USceneComponent*> ChildComponents;
	RootComponent->GetChildrenComponents(true, ChildComponents);
	
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
