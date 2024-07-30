// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_TypingWidget.h"

void UKCD_TypingWidget::NewSentence()
{
	ModifiedSentence = SentenceInstance->CurrentSentence;
	IndexOffset = 0;
	LetterAssociation.Empty();
}

void UKCD_TypingWidget::LetterFeedback(bool wasRight)
{
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
	
}

void UKCD_TypingWidget::BackspaceFeedback()
{
	int index = (SentenceInstance->GetCurrentIndex()) + IndexOffset;
	
	int checkedIndex = SentenceInstance->GetCurrentIndex() - 1;
	
	if(checkedIndex <= 0)
	{
		return;
	}

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
