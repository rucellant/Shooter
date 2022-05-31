// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (�浹�� ���� ��� ĳ���� ������ ������ϴ�.)
	// ����ο��� ������ ������Ʈ ������ ������ �����ο��� CreateDefaultSubobject<>�̿��ؼ� ���� ������Ʈ�� �����ϰ� �ش� �ּҸ� ����Ű�� �ȴ�.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// ACharacter�� ����� ���͵��� RootComponent�� ������ CapsuleComponent��.
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;			// The camera follows at the distance behind the character
	// bUsePawnControlRotation�� true�� �����ϸ� ���������� ��Ʈ�ѷ��� ȸ������ ���󰣴�.
	CameraBoom->bUsePawnControlRotation = true;		// Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// �Ʒ��� �Լ����� �� �� �ִ� ���� ��� ī�޶�Ӹ��� �ƴ϶� ���� ������Ʈ���� ���Ͽ��� ������ �� �ִٴ� ��.
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// ī�޶�� ���������� �������� ȸ������ �ʴ´�.
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// �Ʒ��� �� ������ ĳ���Ͱ� ȸ���� ��, ���� ���� ȸ������ �ʵ��� ��.
	// Dont rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	// ĳ���Ͱ� �̵��ϴ� �������� ĳ������ ���� ȸ���ϰ� ��
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate.
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// ��� ������ forward�� �� ã�´�.
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f,Rotation.Yaw,0.f };

		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// ��� ������ right�� �� ã�´�.
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f,Rotation.Yaw,0.f };

		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame
}

void AShooterCharacter::FireWeapon()
{
	// FireSound�� ���� ť�ε� �����Ϳ��� ���� ������ ������ ť�� ���� ����Ѵ�. 
	if (FireSound)
	{
		// ����ť�� ����ϴ� ���� UGameplayStatics���� ������ PlaySound2D�Լ��ε� 
		// UGameplayStaticsŬ������ ���ο� ���Ӱ��߿� �ʿ��� ����� �͵��� �������� �־ �ĺ��� �ĺ����� ����.
		// �׸��� ���� ���� ����ť�� ��쿡 10���� ���带 �������� �ϳ� �̾Ƽ� ����ϰ� �ִµ� �� ������ �����Ϳ��� ����������
		// �� ������ ����ϸ� ����.
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	// ���̷�Ż �޽ÿ� ������ ������ �ҷ��ͼ� �ش� ������ ��ġ�� ���� �� �� ��ġ�� ��ƼŬ�� �����Ѵ�.
	// �� ���⼭�� Ʈ�������� ���� �� �ű⼭ Location���� ��´�.
	// �ֳĸ� ��ƼŬ�� UGameplayStatics::SpawnEmitterAtLocation()���� �����ϴµ� �̶� �Ķ���� ������ Ʈ�������� �Ѱܾ� �ؼ�
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(TEXT("BarrelSocket"));
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		// GetWorld()->LineTraceSingleByChannel�� ����Ʈ���̽� �Լ��� FHitResult�� ���ڷ� �ѱ�� FHitResult�� ��� ����� �����Ѵ�
		// ���۰� ������ �ѱ�� �� ���̿� ���� �ε��ƴ����� ���� ������ ��ȯ
		// FQuat�� ���ʹϿ��̰� ���ʹϿ��� ȸ���� ���� ������ ������ �ִ�.
		FHitResult FireHit;
		const FVector Start{ SocketTransform.GetLocation() };
		const FQuat Rotation{ SocketTransform.GetRotation() };
		const FVector RotationAxis{ Rotation.GetAxisX() };
		const FVector End{ Start + RotationAxis * 50'000.f };

		GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (FireHit.bBlockingHit) // bBlockingHit�� true�� ����Ʈ���̽̿��� ������ �ε��ƴٴ� ��
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f); // true�� ������ ��� �׷����ٴ� ��.false�� �� ���� �Ű�������ŭ �����ϴٰ� �����
			DrawDebugPoint(GetWorld(), FireHit.Location, 5.f, FColor::Red, false, 2.f);
		}
	}

	// ��Ÿ���� ����� �ִ��ν��Ͻ��� ����Ѵ�.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �Է� ������Ʈ�� ��ȿ�� �� üũ�ϴ� ��ũ����
	check(PlayerInputComponent);

	//�� ������ �� �����Ӹ��� �Է��� ������ �׼� ������ ������ �Է��� �� ���� �Է��� ���´�
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("FireButton"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FireWeapon);
}

