// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Sentence.h"

#include <fstream>
#include <iomanip>

#include "KCD_GameMode.h"
#include "KCD_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AKCD_Sentence::AKCD_Sentence()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ActorCenter = CreateDefaultSubobject<USceneComponent>("Actor center");
	RootComponent = ActorCenter;
	
	SentenceHolder = CreateDefaultSubobject<USceneComponent>("Sentence Holder");
	SentenceHolder->SetupAttachment(RootComponent);

	LetterMarker = CreateDefaultSubobject<UPaperSpriteComponent>("Marker");
	LetterMarker->SetupAttachment(RootComponent);
}

void AKCD_Sentence::KeyPress(FKey KeyParam)
{
	UE_LOG(LogTemp, Warning, TEXT("Event called"));
	CubeInstance->KeyPressed(KeyParam);
}

// Called when the game starts or when spawned
void AKCD_Sentence::BeginPlay()
{
	Super::BeginPlay();

	//Get the current player controller
	AKCD_PlayerController* PlayerController = Cast<AKCD_PlayerController>(UGameplayStatics::GetPlayerController(
		this, 0));

	//Subscribes to the delegate for then a key is pressed and when it's released
	PlayerController->KeyPressDelegate.AddDynamic(this, &AKCD_Sentence::KeyPress);
	
	SetSentence(FetchNewSentence());
	//SetSentence("This is a test sentence, you are not supposed to see this");
}

void AKCD_Sentence::SetSentence(FString Sentence)
{
	CurrentLetterIndex = 0;
	CurrentSentence = Sentence;

	SpawnLetters();
	MoveMarker();
}

void AKCD_Sentence::SpawnLetters()
{
	//Base transform of the letters, used to spawn the letter in local position
	FTransform spawnTransform{
		FRotator{0.0f, -90.0f, 0.0f}, // Rotation
		FVector{0.0f, 0.0f, 0.0f}, // Translation
		FVector{1.0f, 1.0f, 1.0f} // Scale
	};
	//Total size of the word
	const float WordSize = Lettersize * CurrentSentence.Len();

	//We spawn a KCD_Letter BP for each letters in the word 
	int x = 0;
	int y = 0;
	//For every words in the sentence
	for (auto Element : WordsFromString())
	{
		//Check if the word would be too big for the screen size
		if ((Lettersize * (x + 0.5)) + (Lettersize * (Element.Len() + 0.5)) > ScreenSize)
		{
			x = 0;
			y++;
		}
		//for every letters in the word
		for (auto letter : UKismetStringLibrary::GetCharacterArrayFromString(Element))
		{
			//Position uses half the size of the word and offset that position
			//with the leght of th eprevious letters
			float yPosition = (ScreenSize / 2) - (Lettersize * (x + 0.5));
			float zPosition = Lettersize * -(2 * y);
			spawnTransform.SetLocation(FVector{0.0f, yPosition, zPosition});

			LettersInstances.Add(AddChildLetter(letter, spawnTransform));
			x++;
		}
		//Add a space at the end of the word
		float yPosition = (ScreenSize / 2) - (Lettersize * (x + 0.5));
		float zPosition = Lettersize * -(2 * y);
		spawnTransform.SetLocation(FVector{0.0f, yPosition, zPosition});

		LettersInstances.Add(AddChildLetter(" ", spawnTransform));
		x++;
	}
	//Remove the last element, which is a stray space
	LettersInstances.Last()->Destroy();
	LettersInstances.Pop();
}

bool AKCD_Sentence::Hit(FName Letter)
{
	bool WasGood = false;
	
	TotalTypeSentence += Letter.ToString().ToLower();
	TotalTypeInput += Letter.ToString().ToLower();

	//Keep the moment when the last keystroke is done
	LastInputTime = GetWorld()->GetRealTimeSeconds();

	//Don't try to replace the last typed letter if we are at the end of the sentence
	if(LettersInstances.Num() <= CurrentLetterIndex)
	{
		Mistakes++;
		return false;
	}
	
	//Start the timer if it is the 1st keystroke of the sentence
	if (!HasStarted)
	{
		HasStarted = true;
		StartTime = GetWorld()->GetRealTimeSeconds();
	}

	//If the HEX value of the letters match, we give positive feedback
	//If not, we add a mistake and give negative feedback
	if (LettersInstances[CurrentLetterIndex]->CurrentLetter == ToHex(Letter.ToString()))
	{
		LettersInstances[CurrentLetterIndex]->Hide();
		WasGood = true;
	}
	else
	{
		Mistakes++;
		LettersInstances[CurrentLetterIndex]->ErrorHighlight();
		WasGood = false;
	}
	
	return WasGood;
}

void AKCD_Sentence::Backspace()
{
	//Check if we are at the beginning of the sentence
	if(CurrentLetterIndex != 0)
	{
		//Don't try to unhighlight the current letter if we are after the end
		if(CurrentLetterIndex < LettersInstances.Num())
			LettersInstances[CurrentLetterIndex]->Unhighlight();
			
		CurrentLetterIndex--;
		LettersInstances[CurrentLetterIndex]->PrimaryTargetHighlight();
		TotalTypeSentence.LeftChopInline(1);
		TotalTypeInput.Append("~");
		MoveMarker();
	}
}

void AKCD_Sentence::MoveMarker()
{
	//Get the position of the targeted letter and give is an offset
	FVector markerLocation = LettersInstances[CurrentLetterIndex]->GetTransform().GetLocation();
	markerLocation.Z = markerLocation.Z - (Lettersize / 1.5);

	//Move the marker to the location
	LetterMarker->SetWorldLocation(markerLocation);
}

int AKCD_Sentence::EditDistance()
{
	//Get the lenght of both what is written and what we wanted
	int m = GetNum(CurrentSentence);
	int n = GetNum(TotalTypeSentence);

	std::string s = TCHAR_TO_UTF8(*CurrentSentence);
	std::string t = TCHAR_TO_UTF8(*TotalTypeSentence);

	std::vector<int> prev(n + 1, 0), curr(n + 1, 0);
	
	for (int j = 0; j <= n; j++)
	{
		prev[j] = j;
	}

	//For every letters, we compare and calculate the minimum modification (add, remove, change) required for them to match
	for (int i = 1; i <= m; i++)
	{
		curr[0] = i;
		for (int j = 1; j <= n; j++)
		{
			if (s[i - 1] == t[j - 1])
			{
				curr[j] = prev[j - 1];
			}
			else
			{
				int mn
					= std::min(1 + prev[j], 1 + curr[j - 1]);
				curr[j] = std::min(mn, 1 + prev[j - 1]);
			}
		}
		prev = curr;
	}

	return prev[n];
}

FString AKCD_Sentence::ToHex(FString Letter)
{
	//Transforms the character into an Hex value to fetch
	//the right sprite (Unreal doesn't allow special character as
	//Data table row name)
	std::string f = TCHAR_TO_UTF8(*Letter);
	char c = f[0];
	std::stringstream stream;

	stream << std::hex << std::setw(2) << std::setfill('0') <<
		(int)static_cast<unsigned char>(std::tolower(c));

	std::string result(stream.str());
	FString resultFstring(result.c_str());

	return resultFstring;
}

TArray<FString> AKCD_Sentence::WordsFromString()
{
	TArray<FString> wordArray;
	std::string sentence = TCHAR_TO_UTF8(*CurrentSentence);
	size_t pos = 0;
	std::string word;
	
	//Find the spaces and divides the sentence
	while ((pos = sentence.find(" ")) != std::string::npos)
	{
		word = sentence.substr(0, pos);

		wordArray.Add(word.c_str());

		sentence.erase(0, pos + 1);
	}
	//Last word in the sentence
	wordArray.Add(sentence.c_str());

	return wordArray;
}

AKCD_Letters* AKCD_Sentence::AddChildLetter(FString Letter, FTransform SpawnTransform)
{
	//Add the letter as a child component of the sentence
	UChildActorComponent* child = NewObject<UChildActorComponent>(this);
	child->SetupAttachment(SentenceHolder);
	child->RegisterComponent();
	child->SetChildActorClass(LetterBP);
	child->SetRelativeTransform(SpawnTransform);
	//Get a reference to the child's class and set it's letter
	AKCD_Letters* letterObject = Cast<AKCD_Letters>(child->GetChildActor());
	if (letterObject != nullptr)
	{
		letterObject->SetLetter(FName(ToHex(Letter)));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Letter object reference NULL"));
	}

	return letterObject;
}

void AKCD_Sentence::SentenceComplete()
{
	//Get the sentence completion time
	const double completionTime = LastInputTime - StartTime;

	//Create a stat for the sentence
	FKCD_TypingStats CurrentStat;

	CurrentStat.TimeTaken = completionTime;
	CurrentStat.Score = ((CurrentSentence.Len() - 1) / CurrentStat.TimeTaken) * 60 * 0.2;;
	CurrentStat.Mistakes = Mistakes;
	CurrentStat.WordDistance = EditDistance();
	CurrentStat.WordSize = CurrentSentence.Len();
	CurrentStat.WantedSentence = CurrentSentence;
	CurrentStat.TypedSentence = TotalTypeSentence;
	CurrentStat.Keystrokes = TotalTypeInput;

	Stats.Add(CurrentStat);

	//Write the current sentence's stat to the file
	WriteStats(("Sentence : " + FString::FromInt(SentenceNum)), CurrentStat);

	//Cleans up the sentence variables
	for (auto LettersInstance : LettersInstances)
	{
		LettersInstance->Destroy();
	}
	LettersInstances.Empty();
	TotalTypeSentence = "";
	TotalTypeInput = "";
	HasStarted = false;
	Mistakes = 0;

	//Advances the sentence to the next one
	//If it was the last sentence, launches the end of the test
	SentenceNum++;
	if(SentenceNum >= RequiredSentences)
	{
		TestOver();
		return;
	}

	//Set the new sentence
	SetSentence(FetchNewSentence());
	
	OnSentenceCompleteDelegate.Broadcast();
}

FString AKCD_Sentence::FetchNewSentence()
{
	//Line we will read from the file
	std::string line;

	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("phrases2.txt")));

	//Try to open the file we want to read from.
	std::ifstream myfile(path);
	if (myfile.is_open())
	{
		//Generates a random number
		int x = 0;
		srand(time(0));
		int randomLine = rand() % 501;

		while(SentencesIndexUsed.Contains(randomLine))
		{
			randomLine++;
			if(randomLine >= 501)
			{
				randomLine = 0;
			}
		}
		
		SentencesIndexUsed.AddUnique(randomLine);

		//Read all the lines of the file
		while (getline(myfile, line))
		{
			//if we are at the random number, we close the file and return the sentence
			if (x == randomLine)
			{
				myfile.close();
				return line.c_str();
			}
			x++;
		}
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open sentence file for read"));
	}

	return "";
}

FKCD_TypingStats AKCD_Sentence::AverageStats()
{
	FKCD_TypingStats AverageStat;

	//Makes an average of all the stats
	for (auto TypeStat : Stats)
	{
		AverageStat.Mistakes += TypeStat.Mistakes;
		AverageStat.Score += TypeStat.Score;
		AverageStat.TimeTaken += TypeStat.TimeTaken;
		AverageStat.WordDistance += TypeStat.WordDistance;
		AverageStat.WordSize += TypeStat.WordSize;
	}

	int size = Stats.Num();

	AverageStat.Mistakes = AverageStat.Mistakes / size;
	AverageStat.Score = AverageStat.Score / size;
	AverageStat.TimeTaken = AverageStat.TimeTaken / size;
	AverageStat.WordDistance = AverageStat.WordDistance / size;
	AverageStat.WordSize = AverageStat.WordSize / size;
	AverageStat.WasAltTarget = false;


	return AverageStat;
}

void AKCD_Sentence::AdvanceIndex()
{
	
	if(LettersInstances.Num() <= CurrentLetterIndex)
	{
		return;
	}
	//Advance the letter to the next one
	CurrentLetterIndex++;
	
	//If we aren't at the last letter, we target the next letter
	if(LettersInstances.Num() > CurrentLetterIndex)
	{
		LettersInstances[CurrentLetterIndex]->PrimaryTargetHighlight();
		MoveMarker();
	}
}

void AKCD_Sentence::WriteStats(FString RowName, FKCD_TypingStats Stat)
{
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string path = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("TypingStats.csv")));

	//Open the file in append mode and check if it is opened
	std::ofstream myfile(path, std::ios::app);
	if (myfile.is_open())
	{
		//Create an FString with all results
		FString ResultFString;
		ResultFString = ",," + FString::SanitizeFloat(Stat.Score) + "," +
			FString::SanitizeFloat(Stat.TimeTaken) + "," +
			FString::SanitizeFloat(Stat.Mistakes) + "," +
			FString::SanitizeFloat(Stat.WordSize) + "," +
			FString::SanitizeFloat(Stat.WordDistance);

		//Convert the FString into a std::string
		std::string ResultString = std::string(TCHAR_TO_UTF8(*ResultFString));

		//RowName
		myfile << std::string(TCHAR_TO_UTF8(*("," + RowName + "\n")));
		//Titles
		myfile << ",,WPM,Time Taken,Mistakes,Word Size, Word Distance, Wanted sentence, Typed sentence, Keystrokes\n";
		//Data
		myfile << ResultString + ",";
		//Quote the sentences to avoid csv dividing them with used symbols
		myfile << std::quoted(TCHAR_TO_UTF8(*Stat.WantedSentence));
		myfile << ",";
		myfile << std::quoted(TCHAR_TO_UTF8(*Stat.TypedSentence));
		myfile << ",";
		myfile << std::quoted(TCHAR_TO_UTF8(*Stat.Keystrokes));
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

void AKCD_Sentence::TestOver()
{
	OnTestCompleteDelegate.Broadcast();
	
	WriteStats("Average", AverageStats());
	
	
	UE_LOG(LogTemp, Warning, TEXT("Test is over"));
	this->Destroy();
}