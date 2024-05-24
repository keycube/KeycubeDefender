// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_PlayerController.h"

void AKCD_PlayerController::HandleKeyPressedDelegate(FKey key)
{
	KeyPressDelegate.Broadcast(key);
}

void AKCD_PlayerController::HandleKeyReleasedDelegate(FKey key)
{
	KeyReleaseDelegate.Broadcast(key);
}
