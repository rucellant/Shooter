// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() called!"));

	// {}->int를 제외한 다른 자료 스타일로 초기화하면 에러가 난다
	// c++11부터 추가된 문법. 자료의 형변환을 막음. uniform initialization 혹은 brace initialization이라고 한다.
	int myInt{ 42 };
	UE_LOG(LogTemp, Warning, TEXT("int myInt: %d"), myInt);

	float myFloat{ 3.14159f };
	UE_LOG(LogTemp, Warning, TEXT("float myFloat: %f"), myFloat);

	double myDouble{ 0.000756 };
	UE_LOG(LogTemp, Warning, TEXT("double myDouble: %lf"), myDouble);

	char myChar{ 'J' };
	UE_LOG(LogTemp, Warning, TEXT("char myChar: %c"), myChar);

	wchar_t wideChar{ L'J' };
	UE_LOG(LogTemp, Warning, TEXT("wchar_t wideChar: %lc"), wideChar);

	bool myBool{ true };
	UE_LOG(LogTemp, Warning, TEXT("bool myBool: %d"), myBool);

	UE_LOG(LogTemp, Warning, TEXT("int: %d, float: %f, bool: %d"), myInt, myFloat, myBool);
	// 매개변수 자리에 변수가 아니라 단순 데이터값을 넣어도 된다
	// ex)UE_LOG(LogTemp, Warning, TEXT("bool myBool: %d"), false);

	// ue4에서는 스트링을 쓸 때 FString타입의 스트링을 쓴다.
	// *myString는 const TCHAR*(배열임)를 반환함. 이 방식은 C스타일로 스트링을 사용한다.
	FString myString{ TEXT("My String!!!!") };
	UE_LOG(LogTemp, Warning, TEXT("FString myString: %s"), *myString);

	// GetName()은 인스턴스의 이름을 반환하는 함수
	UE_LOG(LogTemp, Warning, TEXT("Name of instance: %s"), *GetName());
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

