// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_TypingWidget.h"

void UKCD_TypingWidget::NewSentence()
{
	ModifiedSentence = SentenceInstance->CurrentSentence;
}

void UKCD_TypingWidget::LetterFeedback(bool wasRight)
{
	int index = (SentenceInstance->GetCurrentIndex() - 1) + IndexOffset;

	//Cut the string where the letter is
	FString begining = ModifiedSentence.Left(index);
	FString end = ModifiedSentence.Mid(index);

	if(wasRight)
	{
		begining.Append(STYLE_RIGHT);
		IndexOffset += STYLE_RIGHT.Len();
	} else
	{
		begining.Append(STYLE_WRONG);
		IndexOffset += STYLE_WRONG.Len();
	}

	end.InsertAt(1, STYLE_END);
	IndexOffset += STYLE_END.Len();

	ModifiedSentence = begining + end;

	UE_LOG(LogTemp, Warning, TEXT("%s"), *ModifiedSentence);
	
}

FString UKCD_TypingWidget::RefreshSentence()
{
	return ModifiedSentence;
}
