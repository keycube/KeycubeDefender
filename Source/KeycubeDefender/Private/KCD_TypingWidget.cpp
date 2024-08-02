// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_TypingWidget.h"

void UKCD_TypingWidget::NewSentence()
{
	ModifiedSentence = SentenceInstance->CurrentSentence;
	IndexOffset = 0;
	LetterAssociation.Empty();

	TargetLetter(SentenceInstance->GetCurrentIndex());
	UE_LOG(LogTemp, Warning, TEXT("New sentence"));
}

void UKCD_TypingWidget::LetterFeedback(bool wasRight)
{

	RemoveTarget(SentenceInstance->GetCurrentIndex() + IndexOffset);
	
	if(SentenceInstance->GetCurrentIndex() >= SentenceInstance->CurrentSentence.Len())
	{
		UE_LOG(LogTemp, Warning, TEXT("End of the sentence"));
		return;
	}
	
	 int index = (SentenceInstance->GetCurrentIndex()) + IndexOffset;
	
	int checkedIndex = SentenceInstance->GetCurrentIndex();
	
	if(checkedIndex > 0)
	{
		if(wasRight == LetterAssociation[checkedIndex - 1])
		{
			std::string tempString = std::string(TCHAR_TO_UTF8(*RemoveEndMarker(index, ModifiedSentence)));
			tempString.insert(index - 2, TCHAR_TO_UTF8(*STYLE_END));
			ModifiedSentence = tempString.c_str();
			
			LetterAssociation.Add(checkedIndex, wasRight);
	
			UE_LOG(LogTemp, Warning, TEXT("%s"), *ModifiedSentence);

			TargetLetter(SentenceInstance->GetCurrentIndex() + IndexOffset + 1);
			
			return;
		}
	}
	
	std::string convertedString = TCHAR_TO_UTF8(*ModifiedSentence);
	
	FString Marker = GetMarker(wasRight);
	
	convertedString.insert(index + 1, TCHAR_TO_UTF8(*STYLE_END));
	convertedString.insert(index, TCHAR_TO_UTF8(*Marker));
	
	IndexOffset += Marker.Len() + STYLE_END.Len();
	
	ModifiedSentence = convertedString.c_str();
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ModifiedSentence);
	
	LetterAssociation.Add(checkedIndex, wasRight);
	
	TargetLetter(SentenceInstance->GetCurrentIndex() + IndexOffset + 1);

	UE_LOG(LogTemp, Warning, TEXT("Sentence after manipulations : %s"), *ModifiedSentence);
}

void UKCD_TypingWidget::BackspaceFeedback()
{
	int checkedIndex = SentenceInstance->GetCurrentIndex() - 1;
	
	if(checkedIndex <= 0)
	{
		return;
	}

	RemoveTarget(SentenceInstance->GetCurrentIndex() + IndexOffset);
	
	int index = (SentenceInstance->GetCurrentIndex()) + IndexOffset;
	
	if(LetterAssociation[checkedIndex] == LetterAssociation[checkedIndex - 1])
	{
		//No need to remove from the global index since we add the end tag back
		std::string tempString = std::string(TCHAR_TO_UTF8(*RemoveEndMarker(index, ModifiedSentence)));
		index -= STYLE_END.Len();
		tempString.insert(index - 1, TCHAR_TO_UTF8(*STYLE_END));
		ModifiedSentence = tempString.c_str();
	} else
	{
		std::string tempString = std::string(TCHAR_TO_UTF8(*RemoveEndMarker(index, ModifiedSentence)));
		IndexOffset -= STYLE_END.Len();
		index -= STYLE_END.Len();
		tempString = std::string(TCHAR_TO_UTF8(*RemoveBeginningMarker(index, tempString.c_str())));
		IndexOffset -= STYLE_RIGHT.Len();
		ModifiedSentence = tempString.c_str();
	}
	
	TargetLetter(index - 1);
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ModifiedSentence);
}

FString UKCD_TypingWidget::RemoveEndMarker(int Index, FString Sentence)
{
	std::string convertedString = TCHAR_TO_UTF8(*Sentence);

	convertedString.erase(Index - 3, 3);

	FString Temp = convertedString.c_str();

	UE_LOG(LogTemp, Warning, TEXT("String after end marker removed : %s"), *Temp);
	return Temp;
}

FString UKCD_TypingWidget::RemoveBeginningMarker(int Index, FString Sentence)
{
	std::string convertedString = TCHAR_TO_UTF8(*Sentence);

	convertedString.erase(Index - 8, 7);

	FString Temp = convertedString.c_str();

	UE_LOG(LogTemp, Warning, TEXT("String after begin marker removed : %s"), *Temp);
	
	return Temp;
}

FString UKCD_TypingWidget::RefreshSentence()
{
	return ModifiedSentence;
}

void UKCD_TypingWidget::TargetLetter(int index)
{
	if(index < 0 || index - IndexOffset >= SentenceInstance->CurrentSentence.Len())
	{
		UE_LOG(LogTemp, Warning, TEXT("Last letter"));
		return;
	}
	
	std::string tempString = std::string(TCHAR_TO_UTF8(*ModifiedSentence));
	tempString.insert(index, TCHAR_TO_UTF8(*STYLE_CURRENT));
	index += STYLE_CURRENT.Len();
	IndexOffset += STYLE_CURRENT.Len();
	tempString.insert(index + 1, TCHAR_TO_UTF8(*STYLE_END));
	IndexOffset += STYLE_END.Len();
	ModifiedSentence = tempString.c_str();

	UE_LOG(LogTemp, Warning, TEXT("TEST : %s"), *ModifiedSentence);
}

void UKCD_TypingWidget::RemoveTarget(int index)
{
	auto pos = std::string(TCHAR_TO_UTF8(*ModifiedSentence)).find(TCHAR_TO_UTF8(*STYLE_CURRENT));

	if(pos != std::string::npos)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found a current marker"));
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO current marker FOUND"));
		return;
	}
	
	std::string convertedString = TCHAR_TO_UTF8(*RemoveEndMarker(index + 1, ModifiedSentence));
	
	IndexOffset -= STYLE_END.Len();
	
	convertedString.erase(pos, 9);

	IndexOffset -= STYLE_CURRENT.Len();
	
	ModifiedSentence = convertedString.c_str();
	
}
