// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_TypingWidget.h"

void UKCD_TypingWidget::NewSentence()
{
	ModifiedSentence = SentenceInstance->CurrentSentence;
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
			ModifiedSentence = tempString.c_str();
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

FString UKCD_TypingWidget::RemoveEndMarker(int Index, FString Sentence)
{
	std::string convertedString = TCHAR_TO_UTF8(*Sentence);

	convertedString.erase(Index - 3, 3);

	FString Temp = convertedString.c_str();
	return Temp;
}

void UKCD_TypingWidget::RemoveBeginningMarker(int Index)
{
	
}

FString UKCD_TypingWidget::RefreshSentence()
{
	return ModifiedSentence;
}
