// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		// UKismetMathLibrary::MakeRotFromX()�� ���������� �Ķ���ͷ� ���� �ӵ��� ��� ���� �������� ��� �������� ���ϴ� �� ��ȯ
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		// UKismetMathLibrary::NormalizedDeltaRotator()�� �� �������Ͱ��� ȸ������ ��
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		//FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
		//FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
		//FString OffsetMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw);

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, OffsetMessage);
		//	//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, RotationMessage);
		//	//GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::White, MovementRotationMessage);
		//}
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// ���⼭ ShooterCharacter�� �����Ѵ�. BeginPlay()���� �Լ��� ��� ��ü���� �غ� �� ���¿��� ����Ǳ� ������
	// �������� Ȯ���� �� �ִ�.
	// TryGetPawnOwner()�Լ��� ���� �ִ��ν��Ͻ��� �����ϰ� �ִ� ���� ��ȯ�Ѵ�.
	// �ٸ� ��ȯ���� APawn�̶� ĳ������ ����� �Ѵ�.
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
