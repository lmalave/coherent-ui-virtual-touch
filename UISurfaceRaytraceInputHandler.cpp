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
#include "Engine.h"
#include "UISurfaceRaytraceInputHandler.h"

UISurfaceRaytraceInputHandler::UISurfaceRaytraceInputHandler(ACharacter* Character, UCameraComponent* FirstPersonCamera)
{
    this->Character = Character;
    this->FirstPersonCameraComponent = FirstPersonCamera;
    SelectedUISurfaceActor = nullptr;
    RaytraceInputEnable = true;
    RaytraceUpOffset = -10.0;
    RaytraceForwardOffset = 50.0;
    RaytraceDistance = 4000.0;
    RaytraceDrawFraction = 0.75;
}

UISurfaceRaytraceInputHandler::~UISurfaceRaytraceInputHandler()
{
}

AUISurfaceActor* UISurfaceRaytraceInputHandler::GetSelectedUISurfaceActor() {
    return SelectedUISurfaceActor;
}

void UISurfaceRaytraceInputHandler::HandleRaytrace() {
    
    // Perform raytrace to get Hit result
    FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
    FVector TraceDirection = FirstPersonCameraComponent->GetForwardVector();
    UWorld* World = Character->GetWorld();
    FVector EndTrace = StartTrace + TraceDirection * RaytraceDistance;
    
    FCollisionQueryParams TraceParams(FName(TEXT("LookTrace")), true, Character);
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bReturnPhysicalMaterial = true;
    FHitResult Hit(ForceInit);
    World->LineTraceSingle(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams); // simple trace function
    
    // now check Hit result to see if it has hit a UISurfaceActor object
    AUISurfaceActor* UISurfaceActor = Cast<AUISurfaceActor>(Hit.GetActor());
    if (UISurfaceActor)
    {
        if (SelectedUISurfaceActor == nullptr) {
            SelectedUISurfaceActor = UISurfaceActor;
            SelectedUISurfaceActor->EnableInput(UGameplayStatics::GetPlayerController(World, 0));
        }
    }
    else { // if not targeting a UISurfaceActor, then reset flags on previously selected UISurface actor and null it out
        if (SelectedUISurfaceActor != nullptr) {
            SelectedUISurfaceActor->DisableInput(UGameplayStatics::GetPlayerController(World, 0));
            SelectedUISurfaceActor->ResetFlags();
            SelectedUISurfaceActor = nullptr;
        }
    }
    if (SelectedUISurfaceActor) {
        // Draw line and point if targeting a UISurfaceActor
        float ForwardOffset = RaytraceForwardOffset;
        float UpOffset = RaytraceUpOffset;
        float RaytraceLength = FVector::Dist(StartTrace,Hit.ImpactPoint);
        if (RaytraceLength < 100.f) {
            ForwardOffset = RaytraceLength * 0.5;
            UpOffset = (0.5 * RaytraceUpOffset) * (1 + RaytraceLength / 100.f);  // starts going from 100% to 50% of up offset
        }
        FVector LineStart = StartTrace + FirstPersonCameraComponent->GetUpVector() * RaytraceUpOffset + FirstPersonCameraComponent->GetForwardVector() * RaytraceForwardOffset;
        DrawDebugLine(World, LineStart, Hit.ImpactPoint, FColor::Blue.WithAlpha(50)); // NOTE:  DrawDebug methods are ignoring the Alpha
        if (!SelectedUISurfaceActor->PointerFingerIsHovering) {
            DrawDebugSphere(World, Hit.ImpactPoint, 0.5, 12, FColor::Blue.WithAlpha(50));
            UISurfaceActor->HandleMouseoverEventWorldLocation(Hit.ImpactPoint);
        }
    }
}

