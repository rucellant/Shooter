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
	// ShooterCharacter��ȿ�� �� üũ�ؼ� ������ ��ȿ�ϰ� ���ش�.
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	// ��ȿ�� �� �ٽ� �ѹ� �˻�
	if (ShooterCharacter)
	{
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

		// Get the lateral speed of ther character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size(); // �ӵ��� ũ��� �ӷ�

		// is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// is the character accelerating?
		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

		// GetBaseAimRotation()�� ��Ʈ�ѷ��� ��� ��(���⼭�� x���� �������� Yawȸ���� �󸶳� �ߴ���)�� ��������
		// �󸶳� ȸ���ߴ� ���� ��ȯ
		// UKismetMathLibrary::MakeRotFromX() : �Է� �Ķ���� ���͸� X������ �Ͽ� ���ο� ������ �����ϰ�, �� ������ ȸ�� ����� �����Ѵ�.
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		// UKismetMathLibrary::NormalizedDeltaRotator()�� AimRotation�� �������� MovementRotation�� ��ŭ ȸ���� ������ ���� ��Ÿ��
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// LastMovementOffsetYaw = ShooterCharacter->GetVelocity().Size() > 0.f ? MovementOffsetYaw : 0.f;
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
			LastMovementOffsetYaw = MovementOffsetYaw;

		// FRotator�� Yaw���� X���� �ٶ󺸴� �������� 0�̰� �������� 180��, �������� -180���� �����ȴ�.
		// MovementRotation�� ���⿡�� AimRotation�� ������ ���� AimRotation�� ������ �� ���̸�ŭ ȸ���� ����� �ȴ�.
		// ���� �� ����� ȸ����(-180~180)�� ���� �ִϸ��̼��� �޶�����.

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
	// ���⼭ ShooterCharacter�� �����Ѵ�. BeginPlay()���� �Լ��� ��� ��ü���� �غ� �� ���¿��� ����Ǳ� ������
	// �������� Ȯ���� �� �ִ�.
	// TryGetPawnOwner()�Լ��� ���� �ִ��ν��Ͻ��� �����ϰ� �ִ� ���� ��ȯ�Ѵ�.
	// �ٸ� ��ȯ���� APawn�̶� ĳ������ ����� �Ѵ�.
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	// GetBaseAimRotation() -> ��ȯ���� ������ �����ϴ� ����� ��ġ�ϴ� �������͸� ��ȯ. ��Ʈ�ѷ��� ���̽��� �Ѵ�.
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

		// YawDelta��ŭ ȸ�� ������ -YawDelta��ŭ ȸ�����Ѿ��� ��Ʈ�� ȸ������ ������ �ʴ´�.
		// ��� �ִϸ��̼� BP���� ��Ʈ �� ȸ���̶�� ��忡 �Ķ���ͷ� ���δ�.
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
