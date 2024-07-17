// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_GameUI.h"

#include <fstream>

void UKCD_GameUI::WriteNewUser()
{
	//Set the relative path where the file is saved and the name of the file
	FString RelativePath = FPaths::ProjectContentDir();
	std::string resultPath = (std::string((TCHAR_TO_UTF8(*RelativePath))
		+ std::string("Result.csv")));

	std::string typingPath = (std::string((TCHAR_TO_UTF8(*RelativePath))
	+ std::string("TypingStats.csv")));

	std::string Str = std::string((TCHAR_TO_UTF8(*GenerateUser())));
	
	//Open the file in append mode and check if it is opened
	std::ofstream myfile (resultPath, std::ios::app);
	if (myfile.is_open())
	{
		myfile << "user_" + Str + "\n";
		
		myfile.close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to open file for write"));
	}

	std::ofstream myfileType (typingPath, std::ios::app);
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
