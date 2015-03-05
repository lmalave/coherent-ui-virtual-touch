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

#include "OculusUIPOC.h"

#include "CoherentUIComponent.h"
#include <Coherent/UI/InputEvents.h>
#include <Coherent/UI/View.h>
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "Runtime/Core/Public/Misc/DateTime.h"
#include <string>
#include "StringConv.h"
#include "UISurfaceActor.h"

AUISurfaceActor::AUISurfaceActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	UIViewInitialized = false;
	HoverDistance = 30.0;
	LastMouseEventPixelCoordinates = *(new FVector());

	UISurfaceMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("UISurfaceMesh"));
	RootComponent = UISurfaceMesh;
	CoherentUIComponent = PCIP.CreateDefaultSubobject<UCoherentUIComponent>(this, TEXT("UCoherentUIComponent"));
	CoherentUIViewURL = TEXT("http://www.google.com");
	CoherentUIViewPixelWidth = 1024;
	// enable tick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PixelToWheelTickScalingFactor = 100.0; // looks like a full wheel tick is approximately 100 pixels (9 or 10 wheel ticks to scroll 1024 pixels)
	ScrollNumPixelsThreshold = 3.0;
	SwipeLengthPixelsThreshold = 40.0;
	SwipeWidthPixelsCutoff = 10.0;
	MinMillisecondsBetweenSwipes = 500;
	MaxMillisecondsLinkingSwipes = 1500;

	LastLeftSwipeTime = FDateTime::Now(); // initialize swipe time
	LastRightSwipeTime = FDateTime::Now(); // initialize swipe time
}

void AUISurfaceActor::HandleMouseoverEventWorldLocation(FVector EventWorldLocation)
{
	if (!PointerFingerIsHovering) { // don't handle raycast mouseover if in hover state
		Coherent::UI::View* UIView = CoherentUIComponent->GetView();
		if (UIView) {
			// Get XY pixel coordinates in the UI View
			LastMouseEventPixelCoordinates = GetViewPixelCoordinatesFromWorldLocation(EventWorldLocation);
			HandleMouseoverEventPixelCoordinates(LastMouseEventPixelCoordinates);
		}
	}
}

void AUISurfaceActor::HandleMouseoverEventPixelCoordinates(FVector PixelCoordinates)
{
	Coherent::UI::View* UIView = CoherentUIComponent->GetView();
	if (UIView) {
		// Generate view mouse events from user input
		Coherent::UI::MouseEventData* UIMouseEvent = new Coherent::UI::MouseEventData();
		UIMouseEvent->X = PixelCoordinates.X;
		UIMouseEvent->Y = PixelCoordinates.Y;
		UIMouseEvent->Type = Coherent::UI::MouseEventData::EventType::MouseMove;
		UIView->MouseEvent(*UIMouseEvent);
	}
}

void AUISurfaceActor::HandleMouseDownEventAtCoordinates(FVector PixelCoordinates)
{
	Coherent::UI::MouseEventData* UIMouseEvent = new Coherent::UI::MouseEventData();
	UIMouseEvent->Type = Coherent::UI::MouseEventData::EventType::MouseDown;
	UIMouseEvent->X = PixelCoordinates.X;
	UIMouseEvent->Y = PixelCoordinates.Y;
	CoherentUIComponent->GetView()->MouseEvent(*UIMouseEvent);
}

void AUISurfaceActor::HandleMouseDownEvent()
{
	HandleMouseDownEventAtCoordinates(LastMouseEventPixelCoordinates);
}

void AUISurfaceActor::HandleMouseUpEventAtCoordinates(FVector PixelCoordinates)
{
	Coherent::UI::MouseEventData* UIMouseEvent = new Coherent::UI::MouseEventData();
	UIMouseEvent->Type = Coherent::UI::MouseEventData::EventType::MouseUp;
	UIMouseEvent->X = PixelCoordinates.X;
	UIMouseEvent->Y = PixelCoordinates.Y;
	CoherentUIComponent->GetView()->MouseEvent(*UIMouseEvent);
}

void AUISurfaceActor::HandleMouseUpEvent()
{
	HandleMouseUpEventAtCoordinates(LastMouseEventPixelCoordinates);
}

void AUISurfaceActor::HandleMouseClick(FVector ViewPixelCoordinates)
{
	HandleMouseDownEventAtCoordinates(ViewPixelCoordinates);
	HandleMouseUpEventAtCoordinates(ViewPixelCoordinates);
}


void AUISurfaceActor::HandleYScrollIncrementEvent(float NumYWheelTicksWithSign)
{
	Coherent::UI::MouseEventData* UIMouseEvent = new Coherent::UI::MouseEventData();
	UIMouseEvent->Type = Coherent::UI::MouseEventData::EventType::MouseWheel;
	UIMouseEvent->WheelY = NumYWheelTicksWithSign;
	CoherentUIComponent->GetView()->MouseEvent(*UIMouseEvent);
}

void AUISurfaceActor::HandleScrollDownEvent()
{
	HandleYScrollIncrementEvent(-1.0);
}

void AUISurfaceActor::HandleScrollUpEvent()
{
	HandleYScrollIncrementEvent(1.0);
}

void AUISurfaceActor::HandleBackEvent()
{
	Coherent::UI::View* UIView = CoherentUIComponent->GetView();
	if (UIView) {
		UIView->ExecuteScript("history.back();");
	}
}

void AUISurfaceActor::HandleVirtualTouchInput(FVector ActionHandPalmLocation, FVector ActionHandFingerLocation) {
	FVector ActorSpaceActionFingerLocation = this->GetTransform().InverseTransformPosition(ActionHandFingerLocation);
	FVector ActorSpaceActionPalmLocation = this->GetTransform().InverseTransformPosition(ActionHandPalmLocation);
	FVector ActorSpaceActionFingerPreviousLocation = this->GetTransform().InverseTransformPosition(ActionHandPreviousFingerLocation);
	FVector ActorSpaceActionPalmPreviousLocation = this->GetTransform().InverseTransformPosition(ActionHandPreviousPalmLocation);
	FVector ActorSpaceActionFingerLocationXY = ActorSpaceActionFingerLocation;
	ActorSpaceActionFingerLocationXY.Z = 0.0;
	FVector ActorSpacePointerFingerLocationXYWorld = this->GetTransform().TransformPosition(ActorSpaceActionFingerLocationXY);
	FVector PointerFingerPixelCoordinates = GetViewPixelCoordinatesFromActorLocation(ActorSpaceActionFingerLocationXY);
	if (ActorSpaceActionFingerLocation.Z <= 0) { // finger is across plane
		if (!PointerFingerAcrossPlane) { // finger was not previously across plane, so handle as mouse click and set across flag to true
			HandleMouseDownEventAtCoordinates(PointerFingerPixelCoordinates);
			DrawDebugSphere(GetWorld(), ActorSpacePointerFingerLocationXYWorld, 0.6, 12, FColor::Cyan, true, 0.1);
			PointerFingerAcrossPlane = true;
		}
		else { // was already across plane
		}
		// handle scrolling
		FVector CurrentPalmPixelCoordinates = GetViewPixelCoordinatesFromActorLocation(ActorSpaceActionPalmLocation);
		FVector PreviousPalmPixelCoordinates = GetViewPixelCoordinatesFromActorLocation(ActorSpaceActionPalmPreviousLocation);
		float NumPixelsMovedY = CurrentPalmPixelCoordinates.Y - PreviousPalmPixelCoordinates.Y; // if negative means palm is moving up so should scroll down
		if (abs(NumPixelsMovedY) >= ScrollNumPixelsThreshold) {
			float WheelTicksY = NumPixelsMovedY / PixelToWheelTickScalingFactor;
			HandleYScrollIncrementEvent(WheelTicksY);
		}
		// Handle left/right swipe
		// NOTE: was a simple left swipe to go Back before, but since there were too many false positives have more complicated gesture linking left and right swipes to go Back
		float NumPixelsMovedX = CurrentPalmPixelCoordinates.X - PreviousPalmPixelCoordinates.X; 
		if (abs(NumPixelsMovedX) >= SwipeLengthPixelsThreshold) { 
			FDateTime CurrentTime = FDateTime::Now();
			if (NumPixelsMovedX < 0) { // is left swipe
				FTimespan TimeBetweenLeftSwipes = CurrentTime - LastLeftSwipeTime;
				if (TimeBetweenLeftSwipes.GetTotalMilliseconds() >= MinMillisecondsBetweenSwipes) { // check if there has been enough elapsed time since last swipe
					LastLeftSwipeTime = CurrentTime;
				}
			}
			else { // is right swipe
				FTimespan TimeBetweenRightSwipes = CurrentTime - LastRightSwipeTime;
				if (TimeBetweenRightSwipes.GetTotalMilliseconds() >= MinMillisecondsBetweenSwipes) { // check if there has been enough elapsed time since last swipe
					LastRightSwipeTime = CurrentTime;
					FTimespan TimeBetweenLeftAndRightSwipes = LastRightSwipeTime - LastLeftSwipeTime;
					if (TimeBetweenLeftAndRightSwipes.GetTotalMilliseconds() <= MaxMillisecondsLinkingSwipes) { // check right swipe is linked to the previous left swipe
						HandleBackEvent();
					}
				}
			}
		}
		
	}
	else { // finger is not across plane
		if (PointerFingerAcrossPlane) { // set across flag to false if it was set to true
			PointerFingerAcrossPlane = false ;
			HandleMouseUpEventAtCoordinates(PointerFingerPixelCoordinates);
		}
		// check for hover state
		if (ActorSpaceActionFingerLocation.Z <= this->HoverDistance) { // check for hovering
			PointerFingerIsHovering = true;
			DrawDebugSphere(GetWorld(), ActorSpacePointerFingerLocationXYWorld, 0.5, 12, FColor::Magenta);
			HandleMouseoverEventPixelCoordinates(PointerFingerPixelCoordinates);
		}
		else {
			if (PointerFingerIsHovering) {
				PointerFingerIsHovering = false;
			}
		}
	}
    // Now that we are done with Leap processing let's set the previous hand locations to the current hand locations
    ActionHandPreviousPalmLocation = ActionHandPalmLocation;
    ActionHandPreviousFingerLocation = ActionHandFingerLocation;
}

FVector AUISurfaceActor::GetViewPixelCoordinatesFromWorldLocation(FVector WorldLocation) {
	FVector* ViewPixelCoordinates = new FVector();
	FVector ActorLocation = this->GetTransform().InverseTransformPosition(WorldLocation);
	return GetViewPixelCoordinatesFromActorLocation(ActorLocation);
}

FVector AUISurfaceActor::GetViewPixelCoordinatesFromActorLocation(FVector ActorLocation) {
	FVector* ViewPixelCoordinates = new FVector();
	float NormalizedUIViewX = (ActorLocation.X + 50.0) / 100.0;
	float NormalizedUIViewY = (ActorLocation.Y + 50.0) / 100.0;
	ViewPixelCoordinates->X = NormalizedUIViewX * CoherentUIComponent->GetView()->GetWidth();
	ViewPixelCoordinates->Y = NormalizedUIViewY * CoherentUIComponent->GetView()->GetHeight();
	ViewPixelCoordinates->Z = 0.0;
	return *ViewPixelCoordinates;
}

void AUISurfaceActor::Tick(float DeltaTime)
{

}

void AUISurfaceActor::InitializeView()
{
	CoherentUIComponent.Get()->URL = CoherentUIViewURL;
	float AspectRatio = this->GetActorScale().X / this->GetActorScale().Y;
	CoherentUIComponent.Get()->Width = CoherentUIViewPixelWidth;
	CoherentUIComponent.Get()->Height = CoherentUIViewPixelWidth / AspectRatio;

}

void AUISurfaceActor::ResetFlags()
{
	PointerFingerIsHovering = false;
	PointerFingerAcrossPlane = false;
}


