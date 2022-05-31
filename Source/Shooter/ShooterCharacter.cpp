// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
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
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

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

}

