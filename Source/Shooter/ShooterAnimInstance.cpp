// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		// Get the lateral speed of ther character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size(); // 속도의 크기는 속력

		// is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// is the character accelerating?
		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// 여기서 ShooterCharacter를 세팅한다. BeginPlay()같은 함수는 모든 객체들이 준비가 된 상태에서 실행되기 때문에
	// 안전성을 확보할 수 있다.
	// TryGetPawnOwner()함수는 현재 애님인스턴스를 소유하고 있는 폰을 반환한다.
	// 다만 반환형이 APawn이라서 캐스팅은 해줘야 한다.
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
