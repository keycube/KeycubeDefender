// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_TypingWidget.h"

void UKCD_TypingWidget::NewSentence()
{
	ModifiedSentence = SentenceInstance->CurrentSentence;
	IndexOffset = 0;
	LetterAssociation.Empty();

	TargetLetter(SentenceInstance->GetCurrentIndex());
}

void UKCD_TypingWidget::LetterFeedback(bool wasRight)
{

	//Remove the "current letter" visual of the letter
	RemoveTarget(SentenceInstance->GetCurrentIndex() + IndexOffset);
	
	if(SentenceInstance->GetCurrentIndex() >= SentenceInstance->CurrentSentence.Len())
	{
		return;
	}
	
	int index = (SentenceInstance->GetCurrentIndex()) + IndexOffset;
	
	int checkedIndex = SentenceInstance->GetCurrentIndex();
	
	//Check if the letter we want to give to feedback to is not the 1st one
	//and if the previous one has the same feedback state. If it is the case, we can move the end marker
	//since the text style stays the same.
	if(checkedIndex > 0)
	{
		if(wasRight == LetterAssociation[checkedIndex - 1])
		{
			std::string tempString = std::string(TCHAR_TO_UTF8(*RemoveEndMarker(index, ModifiedSentence)));
			tempString.insert(index - 2, TCHAR_TO_UTF8(*STYLE_END));
			ModifiedSentence = tempString.c_str();
			
			LetterAssociation.Add(checkedIndex, wasRight);

			TargetLetter(SentenceInstance->GetCurrentIndex() + IndexOffset + 1);
			
			return;
		}
	}

	//if the style is different from the previous letter (or it is the 1st letter),
	//we need to add a beginning marker and an end marker around the letter
	std::string convertedString = TCHAR_TO_UTF8(*ModifiedSentence);
	
	FString Marker = GetMarker(wasRight);
	
	convertedString.insert(index + 1, TCHAR_TO_UTF8(*STYLE_END));
	convertedString.insert(index, TCHAR_TO_UTF8(*Marker));
	
	IndexOffset += Marker.Len() + STYLE_END.Len();
	
	ModifiedSentence = convertedString.c_str();
	
	LetterAssociation.Add(checkedIndex, wasRight);
	
	TargetLetter(SentenceInstance->GetCurrentIndex() + IndexOffset + 1);

}

void UKCD_TypingWidget::BackspaceFeedback()
{
	int checkedIndex = SentenceInstance->GetCurrentIndex() - 1;
	
	if(checkedIndex < 0)
	{
		return;
	}

	RemoveTarget(SentenceInstance->GetCurrentIndex() + IndexOffset);
	
	int index = SentenceInstance->GetCurrentIndex() + IndexOffset;

	//In case the player uses backspace for the 1st letter, we can't check the previous 
	if(checkedIndex == 0)
	{
		RemoveMarker(index);

		TargetLetter(SentenceInstance->GetCurrentIndex() + IndexOffset - 1);
		return;
	}
	//Move the end marker if the last letter has the same beginning marker
	if(LetterAssociation[checkedIndex] == LetterAssociation[checkedIndex - 1])
	{
		//No need to remove from the global index since we add the end tag back
		std::string tempString = std::string(TCHAR_TO_UTF8(*RemoveEndMarker(index, ModifiedSentence)));
		index -= STYLE_END.Len();
		tempString.insert(index - 1, TCHAR_TO_UTF8(*STYLE_END));
		ModifiedSentence = tempString.c_str();
	} else
	{
		RemoveMarker(index);
	}
	
	TargetLetter(SentenceInstance->GetCurrentIndex() + IndexOffset - 1);
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ModifiedSentence);
}

FString UKCD_TypingWidget::RemoveEndMarker(int Index, FString Sentence)
{
	std::string convertedString = TCHAR_TO_UTF8(*Sentence);

	convertedString.erase(Index - 3, 3);

	FString Temp = convertedString.c_str();

	return Temp;
}

FString UKCD_TypingWidget::RemoveBeginningMarker(int Index, FString Sentence)
{
	std::string convertedString = TCHAR_TO_UTF8(*Sentence);

	convertedString.erase(Index - 8, 7);

	FString Temp = convertedString.c_str();
	
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
		return;
	}
	
	std::string tempString = std::string(TCHAR_TO_UTF8(*ModifiedSentence));
	tempString.insert(index, TCHAR_TO_UTF8(*STYLE_CURRENT));
	index += STYLE_CURRENT.Len();
	IndexOffset += STYLE_CURRENT.Len();
	tempString.insert(index + 1, TCHAR_TO_UTF8(*STYLE_END));
	IndexOffset += STYLE_END.Len();
	ModifiedSentence = tempString.c_str();
}

void UKCD_TypingWidget::RemoveTarget(int index)
{
	auto pos = std::string(TCHAR_TO_UTF8(*ModifiedSentence)).find(TCHAR_TO_UTF8(*STYLE_CURRENT));

	//Verify if we have a current target marker
	if(pos != std::string::npos)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found a current marker"));
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO current marker FOUND"));
		return;
	}

	//Removes the end marker and the beginning marker for the target
	std::string convertedString = TCHAR_TO_UTF8(*RemoveEndMarker(index + 1, ModifiedSentence));
	
	IndexOffset -= STYLE_END.Len();
	
	convertedString.erase(pos, 9);

	IndexOffset -= STYLE_CURRENT.Len();
	
	ModifiedSentence = convertedString.c_str();
}

void UKCD_TypingWidget::RemoveMarker(int index)
{
	int checkedIndex = SentenceInstance->GetCurrentIndex() - 1;
	
	std::string tempString = std::string(TCHAR_TO_UTF8(*RemoveEndMarker(index, ModifiedSentence)));
	IndexOffset -= STYLE_END.Len();
	index -= STYLE_END.Len();
	tempString = std::string(TCHAR_TO_UTF8(*RemoveBeginningMarker(index, tempString.c_str())));
	IndexOffset -= GetMarker(LetterAssociation[checkedIndex]).Len();
	ModifiedSentence = tempString.c_str();
}
