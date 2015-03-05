/*****************************
 Copyright 2015 (c) Leonardo Malave. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are
 permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of
 conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list
 of conditions and the following disclaimer in the documentation and/or other materials
 provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY Leonardo Malave ''AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 The views and conclusions contained in the software and documentation are those of the
 authors and should not be interpreted as representing official policies, either expressed
 or implied, of Leonardo Malave.
 ********************************/

#pragma once

#include "CoherentUIComponent.h"
#include "Coherent/UI/View.h"
#include "GameFramework/Actor.h"
#include "Runtime/Core/Public/Misc/DateTime.h"

#include "UISurfaceActor.generated.h"

/**
 * 
 */
UCLASS()
class AUISurfaceActor : public AActor
{
	GENERATED_BODY()
	
public:
	AUISurfaceActor(const class FPostConstructInitializeProperties& PCIP);

	// if item is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	float HoverDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	float PixelToWheelTickScalingFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	float ScrollNumPixelsThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	float SwipeLengthPixelsThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	float SwipeWidthPixelsCutoff;

	// This property is to prevent multiple swipe events from being recorded from a single swipe action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	double MinMillisecondsBetweenSwipes;

	// The max amount of time between swipes to consider them linked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	double MaxMillisecondsLinkingSwipes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = CoherentUI)
	bool PointerFingerAcrossPlane;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = CoherentUI)
	bool PointerFingerIsHovering;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = CoherentUI)
	FVector LastMouseEventPixelCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoherentUI)
	FDateTime LastLeftSwipeTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoherentUI)
	FDateTime LastRightSwipeTime;

	// mesh to use
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = CoherentUI)
	TSubobjectPtr<UStaticMeshComponent>  UISurfaceMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = CoherentUI)
	TSubobjectPtr<UCoherentUIComponent>  CoherentUIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	FString CoherentUIViewURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoherentUI)
	float CoherentUIViewPixelWidth;

	UFUNCTION()
	void HandleMouseoverEventWorldLocation(FVector EventWorldLocation);

	UFUNCTION()
	void HandleMouseoverEventPixelCoordinates(FVector PixelCoordinates);

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleMouseDownEvent();

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleMouseDownEventAtCoordinates(FVector PixelCoordinates);

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleMouseUpEvent();

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleMouseUpEventAtCoordinates(FVector PixelCoordinates);

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleScrollDownEvent();

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleScrollUpEvent();

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleYScrollIncrementEvent(float NumYWheelTicksWithSign);

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleBackEvent();

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleVirtualTouchInput(FVector ActionHandPalmLocation, FVector ActionHandFingerLocation);

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void HandleMouseClick(FVector ViewPixelCoordinates);

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void InitializeView();

	UFUNCTION(BlueprintCallable, Category = CoherentUI)
	void ResetFlags();

	virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoherentUI)
    FVector ActionHandPreviousPalmLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoherentUI)
    FVector ActionHandPreviousFingerLocation;

protected:

	bool UIViewInitialized;

	UFUNCTION()
	FVector GetViewPixelCoordinatesFromWorldLocation(FVector ImpactPointWorldLocation);

	UFUNCTION()
	FVector GetViewPixelCoordinatesFromActorLocation(FVector ImpactPointActorLocation);

private:

	
	
};
