// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuffComponent.generated.h"

UCLASS()
class FRAMEMULTIPLAYER_API ABuffComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABuffComponent();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

};
