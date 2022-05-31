// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
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
