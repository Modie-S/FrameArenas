// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"


ABuffComponent::ABuffComponent()
{
 
	PrimaryActorTick.bCanEverTick = true;

}


void ABuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABuffComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

