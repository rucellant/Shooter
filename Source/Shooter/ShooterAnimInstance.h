// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UShooterAnimInstance();

	// UAnimInstance는 일종의 tick같은 함수인 NativeUpdateAnimation()함수를 가지고 있다.
	// 따라서 애니메이션BP에서 NativeUpdateAnimation()가 우리가 만든 UpdateAnimationProperties()함수를 호출하게 할거임.
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	
	// 이 함수는 UAnimInstance에서 상속받은 함수임.
	// 이 함수는 액터클래스에서의 BeginPlay()같은 함수임.
	virtual void NativeInitializeAnimation() override;

protected:

	/** Handle turning in place variables */
	void TurnInPlace();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* ShooterCharacter;

	/** The speed of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	/** Whether or not the character is in the air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/** Whether or not the character is moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** Offset yaw used for strafing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/** Offset yaw the frame before we stopped moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Yaw of the Character this frame */
	float CharacterYaw;

	/** Yaw of the Character the previous frame */
	float CharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	/** Rotation curve value this frame */
	float RotationCurve;

	/** Rotation curve value last frame */
	float RotationCurveLastFrame;

	/** The pitch of the aim rotation, used for Aim offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/** True when Reloading, used to prevent Aim Offset while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;
};
