// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterYaw(0.f),
	CharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip)
{

}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// ShooterCharacter유효한 지 체크해서 없으면 유효하게 해준다.
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	// 유효한 지 다시 한번 검사
	if (ShooterCharacter)
	{
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

		// Get the lateral speed of ther character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size(); // 속도의 크기는 속력

		// is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// is the character accelerating?
		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

		// GetBaseAimRotation()은 컨트롤러가 어떠한 축(여기서는 x축을 기준으로 Yaw회전을 얼마나 했는지)을 기준으로
		// 얼마나 회전했는 지를 반환
		// UKismetMathLibrary::MakeRotFromX() : 입력 파라미터 벡터를 X축으로 하여 새로운 기저를 구축하고, 그 기저의 회전 행렬을 리턴한다.
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		// UKismetMathLibrary::NormalizedDeltaRotator()는 AimRotation을 기준으로 MovementRotation가 얼만큼 회전한 상태인 지를 나타냄
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// LastMovementOffsetYaw = ShooterCharacter->GetVelocity().Size() > 0.f ? MovementOffsetYaw : 0.f;
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
			LastMovementOffsetYaw = MovementOffsetYaw;

		// FRotator의 Yaw값은 X축을 바라보는 방향으로 0이고 우측으로 180도, 좌측으로 -180도로 구성된다.
		// MovementRotation의 방향에서 AimRotation의 방향을 빼면 AimRotation을 축으로 그 차이만큼 회전한 결과가 된다.
		// 따라서 그 결과인 회전값(-180~180)에 따라 애니메이션이 달라진다.

		bAiming = ShooterCharacter->GetAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}

	TurnInPlace();
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// 여기서 ShooterCharacter를 세팅한다. BeginPlay()같은 함수는 모든 객체들이 준비가 된 상태에서 실행되기 때문에
	// 안전성을 확보할 수 있다.
	// TryGetPawnOwner()함수는 현재 애님인스턴스를 소유하고 있는 폰을 반환한다.
	// 다만 반환형이 APawn이라서 캐스팅은 해줘야 한다.
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	// GetBaseAimRotation() -> 반환값은 유저가 조준하는 방향과 일치하는 로테이터를 반환. 컨트롤러를 베이스로 한다.
	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0.f || bIsInAir)
	{
		// Don't want to turn in place; Chracter is moving
		RootYawOffset = 0.f;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta{ CharacterYaw - CharacterYawLastFrame };

		// YawDelta만큼 회전 했으면 -YawDelta만큼 회전시켜야지 루트의 회전값이 변하질 않는다.
		// 얘는 애니메이션 BP에서 루트 본 회전이라는 노드에 파라미터로 쓰인다.
		// Root Yaw Offset, updated and clamped to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		// 1.0 if turning, 0.0 if not
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootYawOffset > 0, -> Turning left. RootYawOffset < 0, -> Turning right.
			RootYawOffset > 0.f ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0.f ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
}
