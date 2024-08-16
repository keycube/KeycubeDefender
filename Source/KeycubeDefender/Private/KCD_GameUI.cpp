// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_GameUI.h"

#include <fstream>

void UKCD_GameUI::WriteNewUser()
{
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string ShipPath = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("ShipStats.csv")));

	std::string typingPath = (std::string((TCHAR_TO_UTF8(*RelativePath))
	+ std::string("TypingStats.csv")));

	std::string Str = std::string((TCHAR_TO_UTF8(*GenerateUser())));

	bool isNewFile = false;

	//Verification if the file is already created
	std::ifstream readShipFile(ShipPath);
	isNewFile = false;
	//If the file
	if(!readShipFile.good())
		isNewFile = true;
	
	readShipFile.close();
	
	//Open the file in append mode and check if it is opened
	std::ofstream myfile (ShipPath, std::ios::app);
	if (myfile.is_open())
	{
		if(isNewFile)
		{
			//Titles
			myfile << "User,Wave,Target Type,Score,Time Taken,Mistakes,Word Size, Word Distance\n";
		}
		
		myfile << "user_" + Str + "\n";
		
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}

	//Verification if the file is already created
	std::ifstream readfile(typingPath);
	isNewFile = false;
	//If the file
	if(!readfile.good())
		isNewFile = true;
	
	readfile.close();
	
	std::ofstream myfileType (typingPath, std::ios::app);
	//Writes the lines titles at the beginning of the file
	if(isNewFile)
	{
		//Titles
		myfileType << "User,Sentence number,WPM,Time Taken,Mistakes,Word Size,"
			  " Word Distance,Error Rate,Wanted sentence,Typed sentence,Keystrokes\n";
	}
	if (myfileType.is_open())
	{
		myfileType << "user_" + Str + "\n";
		
		myfileType.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}
}

FString UKCD_GameUI::GenerateUser()
{
	srand(time(0));
	FString Str;
	for(unsigned int i = 0; i < 20; ++i)
	{
		Str += genRandom();
	}

	return Str;	
}
